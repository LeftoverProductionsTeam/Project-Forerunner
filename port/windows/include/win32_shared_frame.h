#ifndef FORERUNNER_WIN32_SHARED_FRAME_H
#define FORERUNNER_WIN32_SHARED_FRAME_H

#ifndef VK_NO_PROTOTYPES
#define VK_NO_PROTOTYPES
#endif
#include <vulkan-local/vulkan_core.h>

#ifdef __cplusplus
extern "C" {
#endif

/* OpenRemix's headless RGBA8/GENERAL frame. The caller owns the three opaque
 * Win32 NT handles and closes them after this import returns. */
struct win32_shared_frame_description
{
	uint32_t width, height, memory_type;
	uint64_t allocation_size;
	uint8_t device_uuid[VK_UUID_SIZE];
	void *memory_handle, *ready_handle, *released_handle;
};

struct win32_shared_frame
{
	VkDevice device;
	VkImage image;
	VkDeviceMemory memory;
	VkSemaphore ready, released;
	uint32_t queue_family;
	PFN_vkDestroyImage destroy_image;
	PFN_vkFreeMemory free_memory;
	PFN_vkDestroySemaphore destroy_semaphore;
	PFN_vkCmdPipelineBarrier pipeline_barrier;
};

/* Use a Vulkan 1.1+ device with both Win32 external-handle extensions enabled.
 * Pass an empty output. Keep the provider alive until all consumers retire. */
VkResult win32_shared_frame_import(struct win32_shared_frame *out,
	VkInstance instance, VkPhysicalDevice physical, VkDevice device, uint32_t queue_family,
	PFN_vkGetInstanceProcAddr get_instance, PFN_vkGetDeviceProcAddr get_device,
	const struct win32_shared_frame_description *description);

/* Submit these around the reads. Wait on ready and signal released in the
 * same queue submission. The image stays in GENERAL. */
void win32_shared_frame_acquire(const struct win32_shared_frame *frame,
	VkCommandBuffer commands, VkPipelineStageFlags stage, VkAccessFlags access);
void win32_shared_frame_release(const struct win32_shared_frame *frame,
	VkCommandBuffer commands, VkPipelineStageFlags stage, VkAccessFlags access);

/* The caller must finish GPU use before destroying an imported frame. */
void win32_shared_frame_destroy(struct win32_shared_frame *frame);

#ifdef __cplusplus
}
#endif
#endif
