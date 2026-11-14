/*
 * Shared OpenXR backend for Quest 2 PCVR (Touch) and Vive/Index (Knuckles).
 * Adapted from OpenH1/goath1 port/linux/host/host_vr.c and host_vr_vulkan.h,
 * baed77571d53abe32b27434b4ba869f11db00fe9, AGPL-3.0-or-later.
 * OpenH1 attribution and the repository's upstream licences remain in force.
 * Uses the installed Windows OpenXR loader and Forerunner's existing Vulkan
 * device; no OpenH1 guest host, second renderer or private runtime loader.
 * UNVERIFIED: no compilation, runtime or hardware validation authorized.
 */
#include <windows.h>
#include <stdlib.h>
#include <string.h>
#include "vr_openxr_vulkan.h"
#include "vr_openxr.h"
#include "port_config.h"
#define XR_NO_PROTOTYPES
#define XR_USE_GRAPHICS_API_VULKAN
#include <openxr/openxr.h>
#include <openxr/openxr_platform.h>

extern void platform_log(const char *format, ...);

#define XR_FUNCTIONS(X) \
	X(xrDestroyInstance) X(xrGetSystem) X(xrGetVulkanGraphicsRequirements2KHR) \
	X(xrCreateVulkanInstanceKHR) X(xrGetVulkanGraphicsDevice2KHR) X(xrCreateVulkanDeviceKHR) \
	X(xrCreateSession) X(xrDestroySession) X(xrBeginSession) X(xrEndSession) X(xrPollEvent) \
	X(xrCreateReferenceSpace) X(xrDestroySpace) X(xrLocateSpace) X(xrLocateViews) \
	X(xrEnumerateViewConfigurationViews) X(xrEnumerateEnvironmentBlendModes) \
	X(xrEnumerateSwapchainFormats) X(xrCreateSwapchain) X(xrDestroySwapchain) \
	X(xrEnumerateSwapchainImages) X(xrAcquireSwapchainImage) X(xrWaitSwapchainImage) \
	X(xrReleaseSwapchainImage) X(xrWaitFrame) X(xrBeginFrame) X(xrEndFrame) \
	X(xrStringToPath) X(xrCreateActionSet) X(xrDestroyActionSet) X(xrCreateAction) \
	X(xrSuggestInteractionProfileBindings) X(xrAttachSessionActionSets) X(xrCreateActionSpace) \
	X(xrSyncActions) X(xrGetActionStateBoolean) X(xrGetActionStateFloat) \
	X(xrGetActionStateVector2f) X(xrGetActionStatePose) X(xrApplyHapticFeedback)
#define DECLARE_XR(name) static PFN_##name name;
XR_FUNCTIONS(DECLARE_XR)
#undef DECLARE_XR
static PFN_xrGetInstanceProcAddr get_xr_proc;
static PFN_xrCreateInstance xrCreateInstance;
static PFN_xrEnumerateInstanceExtensionProperties xrEnumerateInstanceExtensionProperties;

enum xr_action
{
	_action_primary, _action_secondary, _action_stick_click, _action_menu,
	_action_stick, _action_trigger, _action_squeeze, _action_aim, _action_grip,
	_action_haptic, NUMBER_OF_ACTIONS
};

static struct
{
	HMODULE loader;
	int attempted, prepared, running, unavailable, frame_begun, views_valid;
	XrInstance instance;
	XrSystemId system;
	XrSession session;
	XrSessionState state;
	XrSpace space, aim[2], grip[2];
	XrActionSet action_set;
	XrAction actions[NUMBER_OF_ACTIONS];
	XrPath hands[2];
	XrGraphicsRequirementsVulkan2KHR requirements;
	XrSwapchain swapchain[2];
	XrSwapchainImageVulkanKHR *images[2];
	uint32_t count[2], width[2], height[2], acquired[2];
	int owned[2], waited[2], released[2];
	VkFormat format;
	XrFrameState frame;
	XrView views[2];
} vr;

static int check(XrResult result, const char *operation)
{
	if (XR_FAILED(result))
	{
		platform_log("OpenXR: %s failed (%d); VR source remains unverified", operation, (int)result);
		return 0;
	}
	return 1;
}

int vr_openxr_requested(void)
{
	return config_boolean("vr.enabled");
}

void vr_openxr_vulkan_shutdown(void)
{
	int eye;

	/* The renderer has finished with runtime images before this call. On
	   session/device loss destroy the session rather than reusing stale handles. */
	if (vr.session && vr.frame_begun && !vr.owned[0] && !vr.owned[1])
		vr_openxr_frame_end(0);
	for (eye = 0; eye < 2; eye++)
	{
		if (vr.aim[eye]) xrDestroySpace(vr.aim[eye]);
		if (vr.grip[eye]) xrDestroySpace(vr.grip[eye]);
		if (vr.swapchain[eye]) xrDestroySwapchain(vr.swapchain[eye]);
		free(vr.images[eye]);
	}
	if (vr.space) xrDestroySpace(vr.space);
	if (vr.session) xrDestroySession(vr.session);
	if (vr.action_set) xrDestroyActionSet(vr.action_set);
	if (vr.instance && xrDestroyInstance) xrDestroyInstance(vr.instance);
	if (vr.loader) FreeLibrary(vr.loader);
	memset(&vr, 0, sizeof(vr));
#define CLEAR_XR(name) name = NULL;
	XR_FUNCTIONS(CLEAR_XR)
#undef CLEAR_XR
	get_xr_proc = NULL;
	xrCreateInstance = NULL;
	xrEnumerateInstanceExtensionProperties = NULL;
}

static int prepare(void)
{
	XrInstanceCreateInfo create = { XR_TYPE_INSTANCE_CREATE_INFO };
	XrSystemGetInfo system = { XR_TYPE_SYSTEM_GET_INFO };
	XrExtensionProperties *extensions = NULL;
	const char *extension = XR_KHR_VULKAN_ENABLE2_EXTENSION_NAME;
	uint32_t count = 0, index;
	int found = 0;

	if (vr.attempted) return vr.prepared;
	vr.attempted = 1;
	/* Exclude the working directory from DLL search. The installed Khronos
	   loader selects the user's active SteamVR or Meta PC runtime. */
	vr.loader = LoadLibraryExW(L"openxr_loader.dll", NULL, LOAD_LIBRARY_SEARCH_DEFAULT_DIRS);
	if (!vr.loader)
	{
		platform_log("OpenXR: installed loader unavailable; flat-screen Vulkan remains available");
		goto failed;
	}
	get_xr_proc = (PFN_xrGetInstanceProcAddr)GetProcAddress(vr.loader, "xrGetInstanceProcAddr");
	if (!get_xr_proc) goto failed;
	if (XR_FAILED(get_xr_proc(XR_NULL_HANDLE, "xrCreateInstance", (PFN_xrVoidFunction *)&xrCreateInstance)) ||
		XR_FAILED(get_xr_proc(XR_NULL_HANDLE, "xrEnumerateInstanceExtensionProperties",
			(PFN_xrVoidFunction *)&xrEnumerateInstanceExtensionProperties))) goto failed;
	if (!check(xrEnumerateInstanceExtensionProperties(NULL, 0, &count, NULL), "enumerate extensions") ||
		!count || count > 4096) goto failed;
	extensions = calloc(count, sizeof(*extensions));
	if (!extensions) goto failed;
	for (index = 0; index < count; index++) extensions[index].type = XR_TYPE_EXTENSION_PROPERTIES;
	if (!check(xrEnumerateInstanceExtensionProperties(NULL, count, &count, extensions), "read extensions")) goto failed;
	for (index = 0; index < count; index++)
		if (!strcmp(extensions[index].extensionName, extension)) found = 1;
	free(extensions);
	extensions = NULL;
	if (!found)
	{
		platform_log("OpenXR: active runtime lacks XR_KHR_vulkan_enable2; flat-screen fallback");
		goto failed;
	}
	strcpy(create.applicationInfo.applicationName, "Forerunner");
	strcpy(create.applicationInfo.engineName, "Forerunner OpenCE");
	create.applicationInfo.apiVersion = XR_MAKE_VERSION(1, 0, 0);
	create.enabledExtensionCount = 1;
	create.enabledExtensionNames = &extension;
	if (!check(xrCreateInstance(&create, &vr.instance), "create instance")) goto failed;
#define LOAD_XR(name) if (XR_FAILED(get_xr_proc(vr.instance, #name, (PFN_xrVoidFunction *)&name)) || !name) goto failed;
	XR_FUNCTIONS(LOAD_XR)
#undef LOAD_XR
	system.formFactor = XR_FORM_FACTOR_HEAD_MOUNTED_DISPLAY;
	if (!check(xrGetSystem(vr.instance, &system, &vr.system), "select headset")) goto failed;
	vr.requirements.type = XR_TYPE_GRAPHICS_REQUIREMENTS_VULKAN2_KHR;
	if (!check(xrGetVulkanGraphicsRequirements2KHR(vr.instance, vr.system, &vr.requirements),
		"Vulkan graphics requirements")) goto failed;
	vr.prepared = 1;
	platform_log("OpenXR: shared Vulkan device negotiation prepared (UNVERIFIED)");
	return 1;
failed:
	free(extensions);
	vr_openxr_vulkan_shutdown();
	vr.attempted = 1;
	return 0;
}

int vr_openxr_vulkan_prepared(void) { return vr.prepared; }

VkResult vr_openxr_vulkan_create_instance(PFN_vkGetInstanceProcAddr get_proc,
	const VkInstanceCreateInfo *create, const VkAllocationCallbacks *allocator, VkInstance *instance)
{
	XrVulkanInstanceCreateInfoKHR xr = { XR_TYPE_VULKAN_INSTANCE_CREATE_INFO_KHR };
	VkResult result = VK_ERROR_INITIALIZATION_FAILED;
	uint32_t version = create->pApplicationInfo ? create->pApplicationInfo->apiVersion : VK_API_VERSION_1_0;
	XrVersion requested = XR_MAKE_VERSION(VK_VERSION_MAJOR(version), VK_VERSION_MINOR(version), VK_VERSION_PATCH(version));

	*instance = VK_NULL_HANDLE;
	if (!vr_openxr_requested() || !prepare())
	{
		PFN_vkCreateInstance create_instance = (PFN_vkCreateInstance)get_proc(VK_NULL_HANDLE, "vkCreateInstance");
		return create_instance ? create_instance(create, allocator, instance) : result;
	}
	if (requested < vr.requirements.minApiVersionSupported)
	{
		platform_log("OpenXR: backend Vulkan version is below the runtime minimum");
		return VK_ERROR_INCOMPATIBLE_DRIVER;
	}
	xr.systemId = vr.system;
	xr.pfnGetInstanceProcAddr = get_proc;
	xr.vulkanCreateInfo = create;
	xr.vulkanAllocator = allocator;
	if (!check(xrCreateVulkanInstanceKHR(vr.instance, &xr, instance, &result), "create Vulkan instance"))
		return VK_ERROR_INITIALIZATION_FAILED;
	return result;
}

VkPhysicalDevice vr_openxr_vulkan_physical_device(VkInstance instance)
{
	XrVulkanGraphicsDeviceGetInfoKHR get = { XR_TYPE_VULKAN_GRAPHICS_DEVICE_GET_INFO_KHR };
	VkPhysicalDevice physical = VK_NULL_HANDLE;

	if (!vr.prepared) return physical;
	get.systemId = vr.system;
	get.vulkanInstance = instance;
	if (!check(xrGetVulkanGraphicsDevice2KHR(vr.instance, &get, &physical), "headset Vulkan GPU"))
		return VK_NULL_HANDLE;
	return physical;
}

VkResult vr_openxr_vulkan_create_device(PFN_vkGetInstanceProcAddr get_proc, VkPhysicalDevice physical,
	const VkDeviceCreateInfo *create, const VkAllocationCallbacks *allocator, VkDevice *device)
{
	XrVulkanDeviceCreateInfoKHR xr = { XR_TYPE_VULKAN_DEVICE_CREATE_INFO_KHR };
	VkResult result = VK_ERROR_INITIALIZATION_FAILED;

	*device = VK_NULL_HANDLE;
	if (!vr.prepared)
	{
		return VK_ERROR_INITIALIZATION_FAILED;
	}
	xr.systemId = vr.system;
	xr.pfnGetInstanceProcAddr = get_proc;
	xr.vulkanPhysicalDevice = physical;
	xr.vulkanCreateInfo = create;
	xr.vulkanAllocator = allocator;
	if (!check(xrCreateVulkanDeviceKHR(vr.instance, &xr, device, &result), "create Vulkan device"))
		return VK_ERROR_INITIALIZATION_FAILED;
	return result;
}

struct binding { int action; const char *path; };

/* OpenH1's Touch mapping, shared actions with Knuckles. The left Touch X/Y
   and left Index A/B are primary/secondary; the game can map them to X/Y.
   Index system buttons are runtime-owned, so no system/menu binding is invented. */
#define HAND_BINDINGS(side) \
	{ _action_stick_click, "/user/hand/" side "/input/thumbstick/click" }, \
	{ _action_stick, "/user/hand/" side "/input/thumbstick" }, \
	{ _action_trigger, "/user/hand/" side "/input/trigger/value" }, \
	{ _action_squeeze, "/user/hand/" side "/input/squeeze/value" }, \
	{ _action_aim, "/user/hand/" side "/input/aim/pose" }, \
	{ _action_grip, "/user/hand/" side "/input/grip/pose" }, \
	{ _action_haptic, "/user/hand/" side "/output/haptic" }
#define RIGHT_BUTTONS \
	{ _action_primary, "/user/hand/right/input/a/click" }, \
	{ _action_secondary, "/user/hand/right/input/b/click" }
static const struct binding touch_bindings[] = {
	HAND_BINDINGS("left"), HAND_BINDINGS("right"), RIGHT_BUTTONS,
	{ _action_primary, "/user/hand/left/input/x/click" },
	{ _action_secondary, "/user/hand/left/input/y/click" },
	{ _action_menu, "/user/hand/left/input/menu/click" }
};
static const struct binding index_bindings[] = {
	HAND_BINDINGS("left"), HAND_BINDINGS("right"), RIGHT_BUTTONS,
	{ _action_primary, "/user/hand/left/input/a/click" },
	{ _action_secondary, "/user/hand/left/input/b/click" }
};

static void bindings_suggest(const char *profile, const struct binding *bindings, uint32_t count)
{
	XrActionSuggestedBinding suggested[24];
	XrInteractionProfileSuggestedBinding suggestion = { XR_TYPE_INTERACTION_PROFILE_SUGGESTED_BINDING };
	uint32_t index;

	if (count > 24 || XR_FAILED(xrStringToPath(vr.instance, profile, &suggestion.interactionProfile))) return;
	for (index = 0; index < count; index++)
	{
		suggested[index].action = vr.actions[bindings[index].action];
		if (XR_FAILED(xrStringToPath(vr.instance, bindings[index].path, &suggested[index].binding))) return;
	}
	suggestion.countSuggestedBindings = count;
	suggestion.suggestedBindings = suggested;
	/* A runtime may support just one of these profiles. Failure leaves its
	   other profile intact; never strip mandatory bindings by trial and error. */
	check(xrSuggestInteractionProfileBindings(vr.instance, &suggestion), profile);
}

static int actions_create(void)
{
	static const char *names[NUMBER_OF_ACTIONS] = {
		"primary", "secondary", "stick_click", "menu", "stick", "trigger", "squeeze", "aim", "grip", "haptic"
	};
	XrActionSetCreateInfo set = { XR_TYPE_ACTION_SET_CREATE_INFO };
	XrSessionActionSetsAttachInfo attach = { XR_TYPE_SESSION_ACTION_SETS_ATTACH_INFO };
	int action, hand;

	strcpy(set.actionSetName, "gameplay");
	strcpy(set.localizedActionSetName, "Gameplay");
	if (!check(xrCreateActionSet(vr.instance, &set, &vr.action_set), "create action set")) return 0;
	if (XR_FAILED(xrStringToPath(vr.instance, "/user/hand/left", &vr.hands[0])) ||
		XR_FAILED(xrStringToPath(vr.instance, "/user/hand/right", &vr.hands[1]))) return 0;
	for (action = 0; action < NUMBER_OF_ACTIONS; action++)
	{
		XrActionCreateInfo create = { XR_TYPE_ACTION_CREATE_INFO };
		create.actionType = action <= _action_menu ? XR_ACTION_TYPE_BOOLEAN_INPUT :
			action == _action_stick ? XR_ACTION_TYPE_VECTOR2F_INPUT :
			action <= _action_squeeze ? XR_ACTION_TYPE_FLOAT_INPUT :
			action == _action_haptic ? XR_ACTION_TYPE_VIBRATION_OUTPUT : XR_ACTION_TYPE_POSE_INPUT;
		create.countSubactionPaths = 2;
		create.subactionPaths = vr.hands;
		strcpy(create.actionName, names[action]);
		strcpy(create.localizedActionName, names[action]);
		if (!check(xrCreateAction(vr.action_set, &create, &vr.actions[action]), "create action")) return 0;
	}
	bindings_suggest("/interaction_profiles/oculus/touch_controller", touch_bindings,
		(uint32_t)(sizeof(touch_bindings) / sizeof(touch_bindings[0])));
	bindings_suggest("/interaction_profiles/valve/index_controller", index_bindings,
		(uint32_t)(sizeof(index_bindings) / sizeof(index_bindings[0])));
	for (hand = 0; hand < 2; hand++)
	{
		XrActionSpaceCreateInfo space = { XR_TYPE_ACTION_SPACE_CREATE_INFO };
		space.poseInActionSpace.orientation.w = 1.0f;
		space.subactionPath = vr.hands[hand];
		space.action = vr.actions[_action_aim];
		if (!check(xrCreateActionSpace(vr.session, &space, &vr.aim[hand]), "create aim space")) return 0;
		space.action = vr.actions[_action_grip];
		if (!check(xrCreateActionSpace(vr.session, &space, &vr.grip[hand]), "create grip space")) return 0;
	}
	attach.countActionSets = 1;
	attach.actionSets = &vr.action_set;
	return check(xrAttachSessionActionSets(vr.session, &attach), "attach actions");
}

static int swapchains_create(void)
{
	XrViewConfigurationView views[2] = {{ XR_TYPE_VIEW_CONFIGURATION_VIEW }, { XR_TYPE_VIEW_CONFIGURATION_VIEW }};
	int64_t formats[128];
	XrEnvironmentBlendMode modes[16];
	uint32_t count, index;
	int eye, opaque = 0;

	if (!check(xrEnumerateEnvironmentBlendModes(vr.instance, vr.system, XR_VIEW_CONFIGURATION_TYPE_PRIMARY_STEREO,
		16, &count, modes), "blend modes")) return 0;
	for (index = 0; index < count; index++) if (modes[index] == XR_ENVIRONMENT_BLEND_MODE_OPAQUE) opaque = 1;
	if (!opaque) return 0;
	if (!check(xrEnumerateViewConfigurationViews(vr.instance, vr.system, XR_VIEW_CONFIGURATION_TYPE_PRIMARY_STEREO,
		2, &count, views), "stereo views") || count != 2) return 0;
	if (!check(xrEnumerateSwapchainFormats(vr.session, 128, &count, formats), "swapchain formats")) return 0;
	/* sRGB runtime attachments accept linear renderer output. No CPU readback
	   or second device. The eventual renderer import must preserve this format. */
	for (index = 0; index < count; index++)
		if (formats[index] == VK_FORMAT_R8G8B8A8_SRGB || formats[index] == VK_FORMAT_B8G8R8A8_SRGB)
		{
			vr.format = (VkFormat)formats[index];
			break;
		}
	if (vr.format == VK_FORMAT_UNDEFINED) return 0;
	for (eye = 0; eye < 2; eye++)
	{
		XrSwapchainCreateInfo create = { XR_TYPE_SWAPCHAIN_CREATE_INFO };
		create.usageFlags = XR_SWAPCHAIN_USAGE_COLOR_ATTACHMENT_BIT;
		create.format = vr.format;
		create.sampleCount = 1;
		create.width = views[eye].recommendedImageRectWidth;
		create.height = views[eye].recommendedImageRectHeight;
		create.faceCount = create.arraySize = create.mipCount = 1;
		if (!create.width || !create.height) return 0;
		if (!check(xrCreateSwapchain(vr.session, &create, &vr.swapchain[eye]), "create eye swapchain")) return 0;
		vr.width[eye] = create.width;
		vr.height[eye] = create.height;
		if (!check(xrEnumerateSwapchainImages(vr.swapchain[eye], 0, &vr.count[eye], NULL), "eye image count") ||
			!vr.count[eye] || vr.count[eye] > 64) return 0;
		vr.images[eye] = calloc(vr.count[eye], sizeof(*vr.images[eye]));
		if (!vr.images[eye]) return 0;
		for (index = 0; index < vr.count[eye]; index++) vr.images[eye][index].type = XR_TYPE_SWAPCHAIN_IMAGE_VULKAN_KHR;
		if (!check(xrEnumerateSwapchainImages(vr.swapchain[eye], vr.count[eye], &vr.count[eye],
			(XrSwapchainImageBaseHeader *)vr.images[eye]), "eye images")) return 0;
	}
	return 1;
}

void vr_openxr_vulkan_bind(VkInstance instance, VkPhysicalDevice physical, VkDevice device,
	uint32_t queue_family, uint32_t queue_index)
{
	XrGraphicsBindingVulkan2KHR binding = { XR_TYPE_GRAPHICS_BINDING_VULKAN2_KHR };
	XrSessionCreateInfo create = { XR_TYPE_SESSION_CREATE_INFO };
	XrReferenceSpaceCreateInfo space = { XR_TYPE_REFERENCE_SPACE_CREATE_INFO };

	if (!vr.prepared) return;
	binding.instance = instance;
	binding.physicalDevice = physical;
	binding.device = device;
	binding.queueFamilyIndex = queue_family;
	binding.queueIndex = queue_index;
	create.next = &binding;
	create.systemId = vr.system;
	if (!check(xrCreateSession(vr.instance, &create, &vr.session), "create shared Vulkan session")) goto failed;
	space.referenceSpaceType = XR_REFERENCE_SPACE_TYPE_LOCAL;
	space.poseInReferenceSpace.orientation.w = 1.0f;
	if (!check(xrCreateReferenceSpace(vr.session, &space, &vr.space), "create local space") ||
		!actions_create() || !swapchains_create()) goto failed;
	platform_log("OpenXR: Vulkan session, Touch and Index actions created (UNVERIFIED; stereo game wiring pending)");
	return;
failed:
	/* Vulkan is still owned by bgfx, so leave its device alive. */
	vr.unavailable = 1;
}

static void pose_copy(const XrPosef *in, struct vr_openxr_pose *out)
{
	out->position[0] = in->position.x; out->position[1] = in->position.y; out->position[2] = in->position.z;
	out->orientation[0] = in->orientation.x; out->orientation[1] = in->orientation.y;
	out->orientation[2] = in->orientation.z; out->orientation[3] = in->orientation.w;
	out->valid = 1;
}

static void locate_hand(XrSpace space, XrAction action, XrPath hand, struct vr_openxr_pose *out)
{
	XrActionStateGetInfo get = { XR_TYPE_ACTION_STATE_GET_INFO };
	XrActionStatePose state = { XR_TYPE_ACTION_STATE_POSE };
	XrSpaceLocation location = { XR_TYPE_SPACE_LOCATION };
	XrSpaceLocationFlags valid = XR_SPACE_LOCATION_POSITION_VALID_BIT | XR_SPACE_LOCATION_ORIENTATION_VALID_BIT;

	get.action = action;
	get.subactionPath = hand;
	if (XR_SUCCEEDED(xrGetActionStatePose(vr.session, &get, &state)) && state.isActive &&
		XR_SUCCEEDED(xrLocateSpace(space, vr.space, vr.frame.predictedDisplayTime, &location)) &&
		(location.locationFlags & valid) == valid) pose_copy(&location.pose, out);
}

static void input_read(struct vr_openxr_frame *frame)
{
	XrActiveActionSet active = { vr.action_set, XR_NULL_PATH };
	XrActionsSyncInfo sync = { XR_TYPE_ACTIONS_SYNC_INFO };
	int hand, action;

	if (!frame->focused) return;
	sync.countActiveActionSets = 1;
	sync.activeActionSets = &active;
	if (XR_FAILED(xrSyncActions(vr.session, &sync))) return;
	for (hand = 0; hand < 2; hand++)
	{
		XrActionStateGetInfo get = { XR_TYPE_ACTION_STATE_GET_INFO };
		XrActionStateVector2f stick = { XR_TYPE_ACTION_STATE_VECTOR2F };
		get.subactionPath = vr.hands[hand];
		for (action = _action_primary; action <= _action_menu; action++)
		{
			XrActionStateBoolean state = { XR_TYPE_ACTION_STATE_BOOLEAN };
			get.action = vr.actions[action];
			if (XR_SUCCEEDED(xrGetActionStateBoolean(vr.session, &get, &state)) && state.isActive && state.currentState)
				frame->hand[hand].buttons |= 1u << action;
		}
		get.action = vr.actions[_action_stick];
		if (XR_SUCCEEDED(xrGetActionStateVector2f(vr.session, &get, &stick)) && stick.isActive)
		{
			frame->hand[hand].stick[0] = stick.currentState.x;
			frame->hand[hand].stick[1] = stick.currentState.y;
		}
		for (action = _action_trigger; action <= _action_squeeze; action++)
		{
			XrActionStateFloat state = { XR_TYPE_ACTION_STATE_FLOAT };
			get.action = vr.actions[action];
			if (XR_SUCCEEDED(xrGetActionStateFloat(vr.session, &get, &state)) && state.isActive)
			{
				if (action == _action_trigger) frame->hand[hand].trigger = state.currentState;
				else frame->hand[hand].squeeze = state.currentState;
			}
		}
		locate_hand(vr.aim[hand], vr.actions[_action_aim], vr.hands[hand], &frame->hand[hand].aim);
		locate_hand(vr.grip[hand], vr.actions[_action_grip], vr.hands[hand], &frame->hand[hand].grip);
	}
}

int vr_openxr_frame_begin(struct vr_openxr_frame *frame)
{
	XrEventDataBuffer event = { XR_TYPE_EVENT_DATA_BUFFER };
	XrFrameWaitInfo wait = { XR_TYPE_FRAME_WAIT_INFO };
	XrFrameBeginInfo begin = { XR_TYPE_FRAME_BEGIN_INFO };
	XrViewLocateInfo locate = { XR_TYPE_VIEW_LOCATE_INFO };
	XrViewState state = { XR_TYPE_VIEW_STATE };
	uint32_t count;
	int eye;

	memset(frame, 0, sizeof(*frame));
	if (!vr.session || vr.unavailable || vr.frame_begun) return 0;
	while (xrPollEvent(vr.instance, &event) == XR_SUCCESS)
	{
		if (event.type == XR_TYPE_EVENT_DATA_INSTANCE_LOSS_PENDING) vr.unavailable = 1;
		if (event.type == XR_TYPE_EVENT_DATA_SESSION_STATE_CHANGED)
		{
			const XrEventDataSessionStateChanged *changed = (const XrEventDataSessionStateChanged *)&event;
			if (changed->session == vr.session)
			{
				vr.state = changed->state;
				if (vr.state == XR_SESSION_STATE_READY && !vr.running)
				{
					XrSessionBeginInfo session = { XR_TYPE_SESSION_BEGIN_INFO };
					session.primaryViewConfigurationType = XR_VIEW_CONFIGURATION_TYPE_PRIMARY_STEREO;
					vr.running = check(xrBeginSession(vr.session, &session), "begin session");
				}
				if (vr.state == XR_SESSION_STATE_STOPPING && vr.running)
				{
					check(xrEndSession(vr.session), "end session");
					vr.running = 0;
				}
				if (vr.state == XR_SESSION_STATE_EXITING || vr.state == XR_SESSION_STATE_LOSS_PENDING)
					vr.unavailable = 1;
			}
		}
		event.type = XR_TYPE_EVENT_DATA_BUFFER;
		event.next = NULL;
	}
	if (!vr.running || vr.unavailable) return 0;
	vr.frame.type = XR_TYPE_FRAME_STATE;
	if (!check(xrWaitFrame(vr.session, &wait, &vr.frame), "wait frame") ||
		!check(xrBeginFrame(vr.session, &begin), "begin frame"))
	{
		vr.unavailable = 1;
		return 0;
	}
	vr.frame_begun = 1;
	vr.views_valid = 0;
	vr.released[0] = vr.released[1] = 0;
	frame->focused = vr.state == XR_SESSION_STATE_FOCUSED;
	frame->display_period = (float)((double)vr.frame.predictedDisplayPeriod / 1e9);
	locate.viewConfigurationType = XR_VIEW_CONFIGURATION_TYPE_PRIMARY_STEREO;
	locate.displayTime = vr.frame.predictedDisplayTime;
	locate.space = vr.space;
	for (eye = 0; eye < 2; eye++) vr.views[eye].type = XR_TYPE_VIEW;
	if (XR_SUCCEEDED(xrLocateViews(vr.session, &locate, &state, 2, &count, vr.views)) && count == 2 &&
		(state.viewStateFlags & (XR_VIEW_STATE_POSITION_VALID_BIT | XR_VIEW_STATE_ORIENTATION_VALID_BIT)) ==
		(XR_VIEW_STATE_POSITION_VALID_BIT | XR_VIEW_STATE_ORIENTATION_VALID_BIT))
	{
		vr.views_valid = 1;
		for (eye = 0; eye < 2; eye++)
		{
			pose_copy(&vr.views[eye].pose, &frame->eye[eye]);
			frame->fov[eye][0] = vr.views[eye].fov.angleLeft;
			frame->fov[eye][1] = vr.views[eye].fov.angleRight;
			frame->fov[eye][2] = vr.views[eye].fov.angleUp;
			frame->fov[eye][3] = vr.views[eye].fov.angleDown;
		}
	}
	frame->should_render = vr.frame.shouldRender && vr.views_valid;
	input_read(frame);
	return 1;
}

VkFormat vr_openxr_vulkan_format(void) { return vr.format; }

int vr_openxr_vulkan_size(int eye, uint32_t *width, uint32_t *height)
{
	if (eye < 0 || eye > 1 || !vr.swapchain[eye] || vr.unavailable) return 0;
	*width = vr.width[eye]; *height = vr.height[eye];
	return 1;
}

VkImage vr_openxr_vulkan_acquire(int eye)
{
	XrSwapchainImageAcquireInfo acquire = { XR_TYPE_SWAPCHAIN_IMAGE_ACQUIRE_INFO };
	XrSwapchainImageWaitInfo wait = { XR_TYPE_SWAPCHAIN_IMAGE_WAIT_INFO };
	XrResult result;

	if (eye < 0 || eye > 1 || !vr.frame_begun || !vr.frame.shouldRender || !vr.views_valid ||
		vr.unavailable || vr.owned[eye] || vr.released[eye]) return VK_NULL_HANDLE;
	if (!check(xrAcquireSwapchainImage(vr.swapchain[eye], &acquire, &vr.acquired[eye]), "acquire eye")) goto failed;
	vr.owned[eye] = 1;
	wait.timeout = XR_INFINITE_DURATION;
	result = xrWaitSwapchainImage(vr.swapchain[eye], &wait);
	if (!check(result, "wait eye") || result != XR_SUCCESS) goto failed;
	vr.waited[eye] = 1;
	if (vr.acquired[eye] >= vr.count[eye]) goto failed;
	return vr.images[eye][vr.acquired[eye]].image;
failed:
	vr.unavailable = 1;
	return VK_NULL_HANDLE;
}

void vr_openxr_vulkan_release(int eye)
{
	XrSwapchainImageReleaseInfo release = { XR_TYPE_SWAPCHAIN_IMAGE_RELEASE_INFO };

	if (eye < 0 || eye > 1 || !vr.owned[eye] || !vr.waited[eye]) return;
	if (!check(xrReleaseSwapchainImage(vr.swapchain[eye], &release), "release eye"))
	{
		vr.unavailable = 1;
		return;
	}
	vr.owned[eye] = vr.waited[eye] = 0;
	vr.released[eye] = 1;
}

void vr_openxr_frame_end(int submit_stereo)
{
	XrFrameEndInfo end = { XR_TYPE_FRAME_END_INFO };
	XrCompositionLayerProjection projection = { XR_TYPE_COMPOSITION_LAYER_PROJECTION };
	XrCompositionLayerProjectionView views[2] = {{ XR_TYPE_COMPOSITION_LAYER_PROJECTION_VIEW },
		{ XR_TYPE_COMPOSITION_LAYER_PROJECTION_VIEW }};
	const XrCompositionLayerBaseHeader *layer = (const XrCompositionLayerBaseHeader *)&projection;
	int eye;

	if (!vr.frame_begun) return;
	end.displayTime = vr.frame.predictedDisplayTime;
	end.environmentBlendMode = XR_ENVIRONMENT_BLEND_MODE_OPAQUE;
	if (submit_stereo && !vr.unavailable && vr.frame.shouldRender && vr.views_valid &&
		vr.released[0] && vr.released[1] && !vr.owned[0] && !vr.owned[1])
	{
		for (eye = 0; eye < 2; eye++)
		{
			views[eye].pose = vr.views[eye].pose;
			views[eye].fov = vr.views[eye].fov;
			views[eye].subImage.swapchain = vr.swapchain[eye];
			views[eye].subImage.imageRect.extent.width = (int32_t)vr.width[eye];
			views[eye].subImage.imageRect.extent.height = (int32_t)vr.height[eye];
		}
		projection.space = vr.space;
		projection.viewCount = 2;
		projection.views = views;
		end.layerCount = 1;
		end.layers = &layer;
	}
	if (!check(xrEndFrame(vr.session, &end), "end frame")) vr.unavailable = 1;
	vr.frame_begun = 0;
}

void vr_openxr_haptic(int hand, float amplitude, float seconds)
{
	XrHapticActionInfo info = { XR_TYPE_HAPTIC_ACTION_INFO };
	XrHapticVibration vibration = { XR_TYPE_HAPTIC_VIBRATION };

	if (hand < 0 || hand > 1 || !vr.running || vr.unavailable || vr.state != XR_SESSION_STATE_FOCUSED ||
		!(amplitude > 0.0f) || !(seconds > 0.0f)) return;
	info.action = vr.actions[_action_haptic];
	info.subactionPath = vr.hands[hand];
	vibration.amplitude = amplitude > 1.0f ? 1.0f : amplitude;
	vibration.duration = (XrDuration)((seconds > 1.0f ? 1.0f : seconds) * 1e9);
	vibration.frequency = XR_FREQUENCY_UNSPECIFIED;
	xrApplyHapticFeedback(vr.session, &info, (const XrHapticBaseHeader *)&vibration);
}
