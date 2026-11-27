#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <windows.h>
#include <string.h>
#include "win32_shared_frame.h"
#include <vulkan-local/vulkan_win32.h>

void win32_shared_frame_destroy(struct win32_shared_frame *frame)
{
	if (frame->released)
		frame->destroy_semaphore(frame->device, frame->released, NULL);
	if (frame->ready)
		frame->destroy_semaphore(frame->device, frame->ready, NULL);
	if (frame->image)
		frame->destroy_image(frame->device, frame->image, NULL);
	if (frame->memory)
		frame->free_memory(frame->device, frame->memory, NULL);
	memset(frame, 0, sizeof(*frame));
}

VkResult win32_shared_frame_import(struct win32_shared_frame *out,
	VkInstance instance, VkPhysicalDevice physical, VkDevice device, uint32_t queue_family,
	PFN_vkGetInstanceProcAddr get_instance, PFN_vkGetDeviceProcAddr get_device,
	const struct win32_shared_frame_description *description)
{
	struct win32_shared_frame frame = {0};
	VkPhysicalDeviceIDProperties identity = {VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_ID_PROPERTIES};
	VkPhysicalDeviceProperties2 properties = {VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_PROPERTIES_2};
	VkPhysicalDeviceMemoryProperties memory_properties;
	VkExternalMemoryImageCreateInfo external = {VK_STRUCTURE_TYPE_EXTERNAL_MEMORY_IMAGE_CREATE_INFO};
	VkImageCreateInfo image = {VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO};
	VkMemoryRequirements requirements;
	VkImportMemoryWin32HandleInfoKHR import = {VK_STRUCTURE_TYPE_IMPORT_MEMORY_WIN32_HANDLE_INFO_KHR};
	VkMemoryDedicatedAllocateInfo dedicated = {VK_STRUCTURE_TYPE_MEMORY_DEDICATED_ALLOCATE_INFO};
	VkMemoryAllocateInfo allocate = {VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO};
	VkSemaphoreCreateInfo semaphore = {VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO};
	VkImportSemaphoreWin32HandleInfoKHR sync = {VK_STRUCTURE_TYPE_IMPORT_SEMAPHORE_WIN32_HANDLE_INFO_KHR};
	VkResult result;
	PFN_vkGetPhysicalDeviceProperties2 get_properties;
	PFN_vkGetPhysicalDeviceMemoryProperties get_memory_properties;
	PFN_vkCreateImage create_image;
	PFN_vkGetImageMemoryRequirements get_requirements;
	PFN_vkAllocateMemory allocate_memory;
	PFN_vkBindImageMemory bind_memory;
	PFN_vkCreateSemaphore create_semaphore;
	PFN_vkImportSemaphoreWin32HandleKHR import_semaphore;
	const VkExternalMemoryHandleTypeFlagBits handle_type = VK_EXTERNAL_MEMORY_HANDLE_TYPE_OPAQUE_WIN32_BIT;

	if (!out || out->device || !instance || !physical || !device || !get_instance || !get_device || !description ||
		!description->width || !description->height || !description->allocation_size ||
		queue_family >= VK_QUEUE_FAMILY_EXTERNAL ||
		!description->memory_handle || description->memory_handle == INVALID_HANDLE_VALUE ||
		!description->ready_handle || description->ready_handle == INVALID_HANDLE_VALUE ||
		!description->released_handle || description->released_handle == INVALID_HANDLE_VALUE ||
		description->memory_type >= VK_MAX_MEMORY_TYPES)
		return VK_ERROR_INITIALIZATION_FAILED;

	get_properties = (PFN_vkGetPhysicalDeviceProperties2)get_instance(instance, "vkGetPhysicalDeviceProperties2");
	get_memory_properties = (PFN_vkGetPhysicalDeviceMemoryProperties)get_instance(instance, "vkGetPhysicalDeviceMemoryProperties");
#define DEVICE_PROC(name) (PFN_vk##name)get_device(device, "vk" #name)
	frame.device = device;
	frame.queue_family = queue_family;
	frame.destroy_image = DEVICE_PROC(DestroyImage);
	frame.free_memory = DEVICE_PROC(FreeMemory);
	frame.destroy_semaphore = DEVICE_PROC(DestroySemaphore);
	frame.pipeline_barrier = DEVICE_PROC(CmdPipelineBarrier);
	create_image = DEVICE_PROC(CreateImage);
	get_requirements = DEVICE_PROC(GetImageMemoryRequirements);
	allocate_memory = DEVICE_PROC(AllocateMemory);
	bind_memory = DEVICE_PROC(BindImageMemory);
	create_semaphore = DEVICE_PROC(CreateSemaphore);
	import_semaphore = DEVICE_PROC(ImportSemaphoreWin32HandleKHR);
#undef DEVICE_PROC
	if (!get_properties || !get_memory_properties || !frame.destroy_image || !frame.free_memory ||
		!frame.destroy_semaphore || !frame.pipeline_barrier || !create_image || !get_requirements ||
		!allocate_memory || !bind_memory || !create_semaphore || !import_semaphore)
		return VK_ERROR_EXTENSION_NOT_PRESENT;

	properties.pNext = &identity;
	get_properties(physical, &properties);
	if (memcmp(description->device_uuid, identity.deviceUUID, VK_UUID_SIZE))
		return VK_ERROR_INVALID_EXTERNAL_HANDLE;
	if (description->width > properties.properties.limits.maxImageDimension2D ||
		description->height > properties.properties.limits.maxImageDimension2D)
		return VK_ERROR_FORMAT_NOT_SUPPORTED;
	get_memory_properties(physical, &memory_properties);
	if (description->memory_type >= memory_properties.memoryTypeCount)
		return VK_ERROR_INVALID_EXTERNAL_HANDLE;

	external.handleTypes = handle_type;
	image.pNext = &external;
	image.imageType = VK_IMAGE_TYPE_2D;
	image.format = VK_FORMAT_R8G8B8A8_UNORM;
	image.extent.width = description->width;
	image.extent.height = description->height;
	image.extent.depth = 1;
	image.mipLevels = image.arrayLayers = 1;
	image.samples = VK_SAMPLE_COUNT_1_BIT;
	image.tiling = VK_IMAGE_TILING_OPTIMAL;
	image.usage = VK_IMAGE_USAGE_TRANSFER_SRC_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT |
		VK_IMAGE_USAGE_SAMPLED_BIT | VK_IMAGE_USAGE_STORAGE_BIT | VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
	result = create_image(device, &image, NULL, &frame.image);
	if (result != VK_SUCCESS)
		goto failed;

	get_requirements(device, frame.image, &requirements);
	if (requirements.size != description->allocation_size ||
		!(requirements.memoryTypeBits & (1u << description->memory_type)))
	{
		result = VK_ERROR_INVALID_EXTERNAL_HANDLE;
		goto failed;
	}

	import.handleType = handle_type;
	import.handle = description->memory_handle;
	dedicated.pNext = &import;
	dedicated.image = frame.image;
	allocate.pNext = &dedicated;
	allocate.allocationSize = description->allocation_size;
	allocate.memoryTypeIndex = description->memory_type;
	result = allocate_memory(device, &allocate, NULL, &frame.memory);
	if (result != VK_SUCCESS)
		goto failed;

	result = bind_memory(device, frame.image, frame.memory, 0);
	if (result != VK_SUCCESS)
		goto failed;

	result = create_semaphore(device, &semaphore, NULL, &frame.ready);
	if (result != VK_SUCCESS)
		goto failed;
	result = create_semaphore(device, &semaphore, NULL, &frame.released);
	if (result != VK_SUCCESS)
		goto failed;
	sync.handleType = VK_EXTERNAL_SEMAPHORE_HANDLE_TYPE_OPAQUE_WIN32_BIT;
	sync.semaphore = frame.ready;
	sync.handle = description->ready_handle;
	result = import_semaphore(device, &sync);
	if (result != VK_SUCCESS)
		goto failed;
	sync.semaphore = frame.released;
	sync.handle = description->released_handle;
	result = import_semaphore(device, &sync);
	if (result != VK_SUCCESS)
		goto failed;

	*out = frame;
	return VK_SUCCESS;
failed:
	win32_shared_frame_destroy(&frame);
	return result;
}

static void shared_frame_barrier(const struct win32_shared_frame *frame, VkCommandBuffer commands,
	VkPipelineStageFlags source_stage, VkPipelineStageFlags destination_stage,
	VkAccessFlags source_access, VkAccessFlags destination_access, uint32_t source_queue, uint32_t destination_queue)
{
	VkImageMemoryBarrier barrier = {VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER};

	barrier.srcAccessMask = source_access;
	barrier.dstAccessMask = destination_access;
	barrier.oldLayout = barrier.newLayout = VK_IMAGE_LAYOUT_GENERAL;
	barrier.srcQueueFamilyIndex = source_queue;
	barrier.dstQueueFamilyIndex = destination_queue;
	barrier.image = frame->image;
	barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
	barrier.subresourceRange.levelCount = barrier.subresourceRange.layerCount = 1;
	frame->pipeline_barrier(commands, source_stage, destination_stage, 0, 0, NULL, 0, NULL, 1, &barrier);
}

void win32_shared_frame_acquire(const struct win32_shared_frame *frame,
	VkCommandBuffer commands, VkPipelineStageFlags stage, VkAccessFlags access)
{
	shared_frame_barrier(frame, commands, VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT, stage,
		0, access, VK_QUEUE_FAMILY_EXTERNAL, frame->queue_family);
}

void win32_shared_frame_release(const struct win32_shared_frame *frame,
	VkCommandBuffer commands, VkPipelineStageFlags stage, VkAccessFlags access)
{
	shared_frame_barrier(frame, commands, stage, VK_PIPELINE_STAGE_BOTTOM_OF_PIPE_BIT,
		access, 0, frame->queue_family, VK_QUEUE_FAMILY_EXTERNAL);
}
