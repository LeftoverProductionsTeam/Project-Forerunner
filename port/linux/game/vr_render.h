/* Optional OpenXR views through the existing native renderer. UNVERIFIED. */
#ifndef HALO_VR_RENDER_H
#define HALO_VR_RENDER_H

#ifdef HALO_OPENXR
#include "math/real_math.h"
#include "../src/vr_openxr.h"
struct render_camera;
struct render_window;

/* Returns the existing views unchanged when VR cannot render this frame. */
short vr_render_prepare(struct render_window *windows, short count, struct render_window **prepared);
boolean vr_render_window_begin(struct render_window *window, short *native_window_index);
void vr_render_window_end(struct render_window *window);
void vr_render_finish(void);
boolean vr_render_frustum_bounds(struct render_camera const *camera, real_rectangle2d *bounds);
#endif
#endif
