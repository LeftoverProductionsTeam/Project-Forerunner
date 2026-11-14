/* Native Vulkan interop only; never include this from the Xbox game ABI.
   Creation callbacks adapted from OpenH1/goath1 host_vr_vulkan.h/host_vr.c,
   baed77571d53abe32b27434b4ba869f11db00fe9 (AGPL-3.0-or-later).
   Source only, unverified. The existing bgfx renderer owns Vulkan. */
#ifndef HALO_VR_OPENXR_VULKAN_H
#define HALO_VR_OPENXR_VULKAN_H

#ifndef VK_NO_PROTOTYPES
#define VK_NO_PROTOTYPES
#endif
/* Use precisely the Vulkan declarations bgfx itself compiles against. */
#include <vulkan-local/vulkan_core.h>

#ifdef __cplusplus
extern "C" {
#endif
VkResult vr_openxr_vulkan_create_instance(PFN_vkGetInstanceProcAddr get_proc,
	const VkInstanceCreateInfo *create, const VkAllocationCallbacks *allocator, VkInstance *instance);
/* A null device means VR is inactive; an active runtime's device must occur
   in bgfx's enumerated list. Never substitute a different GPU for the headset. */
VkPhysicalDevice vr_openxr_vulkan_physical_device(VkInstance instance);
int vr_openxr_vulkan_prepared(void);
VkResult vr_openxr_vulkan_create_device(PFN_vkGetInstanceProcAddr get_proc, VkPhysicalDevice physical,
	const VkDeviceCreateInfo *create, const VkAllocationCallbacks *allocator, VkDevice *device);
void vr_openxr_vulkan_bind(VkInstance instance, VkPhysicalDevice physical, VkDevice device,
	uint32_t queue_family, uint32_t queue_index);
/* Once GPU use of runtime images has finished, before bgfx destroys Vulkan. */
void vr_openxr_vulkan_shutdown(void);
/* The returned runtime-owned image is valid only until release. The renderer
   must submit its writes and leave COLOR_ATTACHMENT_OPTIMAL before release. */
VkImage vr_openxr_vulkan_acquire(int eye);
void vr_openxr_vulkan_release(int eye);
/* For bgfx's existing external-image queue handling: 0 is not an XR image,
   1 is acquired and waited, -1 belongs to XR but is not owned by the app. */
int vr_openxr_vulkan_image_state(VkImage image);
#ifdef __cplusplus
}
#endif
#endif
