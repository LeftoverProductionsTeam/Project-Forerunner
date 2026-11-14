/* Optional Windows64 OpenXR bridge. Source only, unverified.
   Pose coordinates follow OpenXR: metres, right/up/back, quaternion xyzw.
   Adapted from OpenH1/goath1 port/linux/src/vr_host.h and host/host_vr.c,
   baed77571d53abe32b27434b4ba869f11db00fe9 (AGPL-3.0-or-later).
   No native graphics handles or SDK structures enter game code. */
#ifndef HALO_VR_OPENXR_H
#define HALO_VR_OPENXR_H

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
void vr_openxr_frame_end(int submit_stereo);
void vr_openxr_haptic(int hand, float amplitude, float seconds);

#endif
