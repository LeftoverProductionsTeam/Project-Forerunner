/* Optional OpenXR views through the existing native renderer. UNVERIFIED. */
#ifndef HALO_VR_RENDER_H
#define HALO_VR_RENDER_H

#ifdef HALO_OPENXR
#include "math/real_math.h"
#include "../src/vr_openxr.h"
struct render_camera;
struct render_frustum;
struct render_window;
struct observer_result;

/* Returns the existing views unchanged when VR cannot render this frame. */
short vr_render_prepare(struct render_window *windows, short count, struct render_window **prepared);
boolean vr_render_window_begin(struct render_window *window, short *native_window_index);
void vr_render_window_end(struct render_window *window);
void vr_render_finish(void);
boolean vr_render_eye_pass(void);
/* Only changes position/orientation while native first-person nodes build. */
boolean vr_render_weapon_camera(struct render_camera *camera);
/* OpenH1's dominant-hand reflection, after native render interpolation.
   Draw winding is scoped to the native first-person draw only. */
void vr_render_weapon_nodes(short local_player_index, real_matrix4x3 *nodes, short count);
void vr_render_weapon_draw_begin(void);
void vr_render_weapon_draw_end(void);
/* Scoped to native crosshairs_draw. One world target per frame, projected
   per eye; FALSE from place hides only an aiming bitmap outside its view. */
void vr_render_reticle_begin(short local_player_index, long weapon_index,
	struct render_camera const *camera, struct render_frustum const *frustum);
boolean vr_render_reticle_place(point2d *point, real *reticle_scale);
void vr_render_reticle_end(void);
boolean vr_render_frustum_bounds(struct render_camera const *camera, real_rectangle2d *bounds);
/* Center of the actual eye cameras, for the existing sound listener only.
   Does not change the observer, simulation or XR frame ownership. */
boolean vr_render_listener(short local_player_index, struct observer_result const *base,
	struct observer_result *listener);
/* Optional on-foot controller aim through native facing; no new wire fields.
   permitted is the native camera/menu/input ownership decision. */
boolean vr_render_player_aim(short local_player_index, long unit_index, boolean permitted,
	real previous_yaw, real turn_delta, real_euler_angles2d *facing, real_vector2d *throttle);
void vr_render_reset_aim(void);
/* Optional dominant-grip pickup of the native offered weapon; native actions
   and host admission remain unchanged. No ammunition or inventory grant. */
void vr_render_player_grab(short local, long unit_index, boolean permitted, unsigned long *flags);
#endif
#endif
