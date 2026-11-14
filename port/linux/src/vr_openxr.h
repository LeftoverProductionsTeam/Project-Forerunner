/* Optional Windows64 OpenXR bridge. Source only, unverified.
   Pose coordinates follow OpenXR: metres, right/up/back, quaternion xyzw.
   Adapted from OpenH1/goath1 port/linux/src/vr_host.h and host/host_vr.c,
   baed77571d53abe32b27434b4ba869f11db00fe9 (AGPL-3.0-or-later).
   Game-facing poses contain no SDK structures. The image interface below is
   used only by the existing platform renderer. */
#ifndef HALO_VR_OPENXR_H
#define HALO_VR_OPENXR_H

#include <stdint.h>

struct vr_openxr_pose
{
	float position[3];
	float orientation[4];
	int valid;
};

enum vr_openxr_button
{
	_vr_button_primary = 1,
	_vr_button_secondary = 2,
	_vr_button_stick = 4,
	_vr_button_menu = 8
};

struct vr_openxr_hand
{
	unsigned int buttons;
	float stick[2];
	float trigger, squeeze;
	struct vr_openxr_pose aim, grip;
};

struct vr_openxr_frame
{
	int should_render, focused;
	/* Camera anchoring resets when a new session/local reference is ready. */
	unsigned int reference_serial;
	float display_period;
	struct vr_openxr_pose eye[2];
	/* Left, right, up, down angles in radians, from xrLocateViews. */
	float fov[2][4];
	struct vr_openxr_hand hand[2];
};

int vr_openxr_requested(void);
/* The render thread owns the complete wait/begin/acquire/release/end sequence.
   End every begun frame, even if tracking is invalid or no image is rendered. */
int vr_openxr_frame_begin(struct vr_openxr_frame *frame);
enum vr_openxr_layer
{
	_vr_layer_none,
	_vr_layer_stereo,
	_vr_layer_menu
};
/* Menu frames reuse eye zero as a head-locked quad; aspect is the native
   desktop canvas width/height, independent of the swapchain's pixel shape. */
void vr_openxr_frame_end(enum vr_openxr_layer layer, float menu_aspect);
void vr_openxr_haptic(int hand, float amplitude, float seconds);
int vr_openxr_running(void);
/* Same-thread input read from the latest located frame, with fresh actions.
   Returns zero without a running XR session; stale/unfocused input is neutral. */
int vr_openxr_input_read(struct vr_openxr_frame *frame);

/* Platform renderer only: the VkImage bits accepted by bgfx's existing
   external-texture argument. Runtime ownership remains acquire -> release. */
struct vr_openxr_image
{
	uint64_t handle;
	uint32_t width, height;
	int bgra;
};
int vr_openxr_eye_acquire(int eye, struct vr_openxr_image *image);
void vr_openxr_eye_release(int eye);

/* Existing renderer entry points for the game-side stereo camera adapter.
   Present submits and closes the frame, including skipped/failed eye pairs. */
int xgpu_openxr_frame_begin(struct vr_openxr_frame *frame);
int xgpu_openxr_eye_begin(int eye);
void xgpu_openxr_eye_end(void);

#endif
