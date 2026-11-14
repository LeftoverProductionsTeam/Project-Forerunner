/*
 * OpenXR stereo camera adapter, using Halo's existing render_frame/window
 * traversal and bgfx Vulkan eye targets. UNVERIFIED source only.
 * Quaternion rotation, OpenXR -> Halo axes, metre scale and asymmetric
 * frustum normalization adapted from OpenH1/goath1 port/linux/src/vr.c and
 * port/linux/game/vr_render.c, baed77571d53abe32b27434b4ba869f11db00fe9,
 * AGPL-3.0-or-later. The OpenH1 renderer/host is not duplicated here.
 */
#include "cseries.h"
#ifdef HALO_OPENXR
#include "cseries/cseries_windows.h"
#include "../../include/halo_math.h"
#include "vr_render.h"
#include "camera/observer.h"
#include "render/render_cameras.h"
#include "scenario/scenario.h"
#include "../src/vr_openxr.h"
#include <string.h>

#define METRES_PER_WORLD_UNIT 3.048f

static struct
{
	struct render_window windows[4];
	real_rectangle2d bounds[2];
	boolean active, origin_valid;
	boolean listener_valid;
	unsigned long listener_time;
	short eye;
	unsigned int reference_serial;
	real origin[3], yaw_cos, yaw_sin;
} stereo;

static void quaternion_rotate(float const q[4], float const v[3], float out[3])
{
	float tx = 2.0f * (q[1] * v[2] - q[2] * v[1]);
	float ty = 2.0f * (q[2] * v[0] - q[0] * v[2]);
	float tz = 2.0f * (q[0] * v[1] - q[1] * v[0]);

	out[0] = v[0] + q[3] * tx + q[1] * tz - q[2] * ty;
	out[1] = v[1] + q[3] * ty + q[2] * tx - q[0] * tz;
	out[2] = v[2] + q[3] * tz + q[0] * ty - q[1] * tx;
}

/* OpenH1's game_axes, using the game's maths and a cached recenter yaw. */
static void world_axes(float const v[3], real_vector3d const *forward,
	real_vector3d const *left, real_vector3d const *up, real_vector3d *out)
{
	real x = v[0] * stereo.yaw_cos + v[2] * stereo.yaw_sin;
	real z = -v[0] * stereo.yaw_sin + v[2] * stereo.yaw_cos;

	out->i = -z * forward->i - x * left->i + v[1] * up->i;
	out->j = -z * forward->j - x * left->j + v[1] * up->j;
	out->k = -z * forward->k - x * left->k + v[1] * up->k;
}

static void recenter(struct vr_openxr_frame const *frame)
{
	static const float xr_forward[3] = { 0.0f, 0.0f, -1.0f };
	float forward[2][3];
	real yaw;
	short axis;

	quaternion_rotate(frame->eye[0].orientation, xr_forward, forward[0]);
	quaternion_rotate(frame->eye[1].orientation, xr_forward, forward[1]);
	yaw = ABS(forward[0][0] + forward[1][0]) + ABS(forward[0][2] + forward[1][2]) > _real_epsilon ?
		arctangent(-(forward[0][0] + forward[1][0]), -(forward[0][2] + forward[1][2])) : 0.0f;
	stereo.yaw_cos = cosine(-yaw);
	stereo.yaw_sin = sine(-yaw);
	for (axis = 0; axis < 3; axis++)
		stereo.origin[axis] = (frame->eye[0].position[axis] + frame->eye[1].position[axis]) * 0.5f;
	stereo.reference_serial = frame->reference_serial;
	stereo.origin_valid = TRUE;
}

static void eye_camera(struct render_camera const *base, struct vr_openxr_frame const *frame,
	short eye, boolean preserve_base_orientation, struct render_camera *camera)
{
	static const float xr_forward[3] = { 0.0f, 0.0f, -1.0f };
	static const float xr_up[3] = { 0.0f, 1.0f, 0.0f };
	real_vector3d forward = base->forward, up = *global_up3d, left, offset;
	float rotated[3], translation[3];
	real tangents[4], half_fov_tangent, aspect;
	short axis;

	/* Native input supplies body heading; headset pitch/roll are independent
	   local rendering. Tracked weapon/game input is a later integration. */
	if (preserve_base_orientation)
		up = base->up;
	else
	{
		forward.k = 0.0f;
		if (ABS(forward.i) + ABS(forward.j) < _real_epsilon)
			forward = *global_forward3d;
	}
	normalize3d(&forward);
	cross_product3d(&up, &forward, &left);
	normalize3d(&left);
	cross_product3d(&forward, &left, &up);
	normalize3d(&up);
	*camera = *base;
	for (axis = 0; axis < 3; axis++)
		translation[axis] = frame->eye[eye].position[axis] - stereo.origin[axis];
	world_axes(translation, &forward, &left, &up, &offset);
	camera->position.x += offset.i / METRES_PER_WORLD_UNIT;
	camera->position.y += offset.j / METRES_PER_WORLD_UNIT;
	camera->position.z += offset.k / METRES_PER_WORLD_UNIT;
	quaternion_rotate(frame->eye[eye].orientation, xr_forward, rotated);
	world_axes(rotated, &forward, &left, &up, &camera->forward);
	quaternion_rotate(frame->eye[eye].orientation, xr_up, rotated);
	world_axes(rotated, &forward, &left, &up, &camera->up);
	normalize3d(&camera->forward);
	normalize3d(&camera->up);
	for (axis = 0; axis < 4; axis++)
		tangents[axis] = tangent(frame->fov[eye][axis]);
	half_fov_tangent = MAX(tangents[2], -tangents[3]);
	camera->vertical_field_of_view = 2.0f * arctangent(half_fov_tangent, 1.0f);
	aspect = (real)(camera->viewport_bounds.x1 - camera->viewport_bounds.x0) /
		(real)(camera->viewport_bounds.y1 - camera->viewport_bounds.y0);
	stereo.bounds[eye].x0 = tangents[0] / (aspect * half_fov_tangent);
	stereo.bounds[eye].x1 = tangents[1] / (aspect * half_fov_tangent);
	stereo.bounds[eye].y0 = tangents[3] / half_fov_tangent;
	stereo.bounds[eye].y1 = tangents[2] / half_fov_tangent;
	camera->mirrored = FALSE;
	camera->z_near = MIN(camera->z_near, 0.05f / METRES_PER_WORLD_UNIT);
}

short vr_render_prepare(struct render_window *windows, short count, struct render_window **prepared)
{
	struct vr_openxr_frame frame;
	short eye;

	stereo.active = FALSE;
	stereo.listener_valid = FALSE;
	stereo.eye = NONE;
	*prepared = windows;
	if (!xgpu_openxr_frame_begin(&frame))
		return count;
	/* A single local 3D view and its existing console/interface view. Keep
	   split-screen and screenshot paths intact; never submit half an eye pair. */
	if (!frame.should_render || count != 2 || windows[0].console_window || !windows[1].console_window)
		return count;
	if (!stereo.origin_valid || stereo.reference_serial != frame.reference_serial)
		recenter(&frame);
	for (eye = 0; eye < 2; eye++)
	{
		stereo.windows[eye * 2] = windows[0];
		stereo.windows[eye * 2 + 1] = windows[1];
		eye_camera(&windows[0].render_camera, &frame, eye, windows[0].local_player_index == NONE,
			&stereo.windows[eye * 2].render_camera);
		stereo.windows[eye * 2].rasterizer_camera = stereo.windows[eye * 2].render_camera;
	}
	stereo.active = TRUE;
	stereo.listener_valid = windows[0].local_player_index != NONE;
	stereo.listener_time = system_milliseconds();
	*prepared = stereo.windows;
	return 4;
}

static short window_number(struct render_window const *window)
{
	short index;

	if (stereo.active)
		for (index = 0; index < 4; index++)
			if (window == &stereo.windows[index])
				return index;
	return NONE;
}

boolean vr_render_window_begin(struct render_window *window, short *native_window_index)
{
	short index = window_number(window);

	if (index == NONE)
		return TRUE;
	/* Both eyes are the same local player, not invented split-screen slots.
	   Native dynamic geometry requires its index < main_get_window_count(). */
	*native_window_index = index & 1;
	if (!(index & 1))
		stereo.eye = xgpu_openxr_eye_begin(index / 2) ? index / 2 : NONE;
	return stereo.eye == index / 2;
}

void vr_render_window_end(struct render_window *window)
{
	short index = window_number(window);

	if (index != NONE && (index & 1))
	{
		xgpu_openxr_eye_end();
		stereo.eye = NONE;
	}
}

void vr_render_finish(void)
{
	xgpu_openxr_eye_end();
	stereo.active = FALSE;
	stereo.eye = NONE;
}

boolean vr_render_listener(short local_player_index, struct observer_result const *base,
	struct observer_result *listener)
{
	struct render_camera const *left = &stereo.windows[0].render_camera;
	struct render_camera const *right = &stereo.windows[2].render_camera;
	real_vector3d side;

	if (!base || !listener || !stereo.listener_valid || !vr_openxr_running() ||
		local_player_index != stereo.windows[0].local_player_index ||
		system_milliseconds() - stereo.listener_time > 250)
		return FALSE;
	*listener = *base;
	listener->position.x = (left->position.x + right->position.x) * 0.5f;
	listener->position.y = (left->position.y + right->position.y) * 0.5f;
	listener->position.z = (left->position.z + right->position.z) * 0.5f;
	listener->forward.i = left->forward.i + right->forward.i;
	listener->forward.j = left->forward.j + right->forward.j;
	listener->forward.k = left->forward.k + right->forward.k;
	listener->up.i = left->up.i + right->up.i;
	listener->up.j = left->up.j + right->up.j;
	listener->up.k = left->up.k + right->up.k;
	if (magnitude_squared3d(&listener->forward) < _real_epsilon ||
		magnitude_squared3d(&listener->up) < _real_epsilon)
		return FALSE;
	normalize3d(&listener->forward);
	cross_product3d(&listener->up, &listener->forward, &side);
	if (magnitude_squared3d(&side) < _real_epsilon)
		return FALSE;
	normalize3d(&side);
	cross_product3d(&listener->forward, &side, &listener->up);
	normalize3d(&listener->up);
	/* Never pair the moved head with the body's old acoustic/underwater
	   location. Outside the loaded BSP, retain the native listener. */
	scenario_location_from_point(&listener->location, &listener->position);
	return listener->location.cluster_index != NONE;
}

boolean vr_render_frustum_bounds(struct render_camera const *camera, real_rectangle2d *bounds)
{
	struct render_camera const *eye;

	if (!stereo.active || stereo.eye == NONE || camera->mirrored)
		return FALSE;
	eye = &stereo.windows[stereo.eye * 2].render_camera;
	/* Match copies of the actual eye camera, without changing native struct
	   layouts or applying headset projection to shadow/interface cameras. */
	if (memcmp(&camera->position, &eye->position, sizeof(camera->position)) ||
		memcmp(&camera->forward, &eye->forward, sizeof(camera->forward)) ||
		memcmp(&camera->up, &eye->up, sizeof(camera->up)) ||
		memcmp(&camera->viewport_bounds, &eye->viewport_bounds, sizeof(camera->viewport_bounds)) ||
		memcmp(&camera->window_bounds, &eye->window_bounds, sizeof(camera->window_bounds)) ||
		camera->vertical_field_of_view != eye->vertical_field_of_view)
		return FALSE;
	*bounds = stereo.bounds[stereo.eye];
	return TRUE;
}
#endif
