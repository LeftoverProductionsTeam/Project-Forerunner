/*
 * OpenXR stereo camera adapter, using Halo's existing render_frame/window
 * traversal and bgfx Vulkan eye targets. UNVERIFIED source only.
 * Quaternion rotation, OpenXR -> Halo axes, metre scale and asymmetric
 * frustum normalization adapted from OpenH1/goath1 port/linux/src/vr.c and
 * port/linux/game/vr_render.c, baed77571d53abe32b27434b4ba869f11db00fe9,
 * AGPL-3.0-or-later. The OpenH1 renderer/host is not duplicated here.
 * Controller weapon-camera placement and vr.weapon_offset defaults also
 * adapt that pin's vr_render_weapon_camera and port_config.c. UNVERIFIED.
 */
#include "cseries.h"
#ifdef HALO_OPENXR
#include "cseries/cseries_windows.h"
#include "../../include/halo_math.h"
#include "vr_render.h"
#include "camera/observer.h"
#include "camera/director.h"
#include "game/players.h"
#include "objects/objects.h"
#include "physics/collisions.h"
#include "physics/collision_usage.h"
#include "render/render_cameras.h"
#include "scenario/scenario.h"
#include "units/units.h"
#include "../src/vr_openxr.h"
#include "../src/port_config.h"
#include <string.h>
#include <stdio.h>

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
	real origin[3], yaw_cos, yaw_sin, origin_yaw;
	boolean aim_synced;
	long aim_unit;
	real body_yaw;
	boolean weapon_camera_valid;
	struct render_camera weapon_camera;
} stereo;

static real wrapped_yaw(real yaw)
{
	while (yaw < 0.0f) yaw += 2.0f * _pi;
	while (yaw >= 2.0f * _pi) yaw -= 2.0f * _pi;
	return yaw;
}

void vr_render_reset_aim(void)
{
	stereo.aim_synced = FALSE;
	stereo.aim_unit = NONE;
	stereo.weapon_camera_valid = FALSE;
}

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
	/* OpenH1 keeps world heading stable when the XR reference is recentered. */
	if (stereo.origin_valid && stereo.aim_synced)
		stereo.body_yaw = wrapped_yaw(stereo.body_yaw + yaw - stereo.origin_yaw);
	stereo.origin_yaw = yaw;
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

	/* Keep body turning separate from the native weapon's tracked facing. */
	if (preserve_base_orientation)
		up = base->up;
	else
	{
		if (stereo.aim_synced)
		{
			forward.i = cosine(stereo.body_yaw);
			forward.j = sine(stereo.body_yaw);
		}
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

/* OpenH1's game-hands offset is the grip from the original animation camera,
   in metres, right/up/forward. Invalid/out-of-range settings use its defaults. */
static real_vector3d const *weapon_offset(void)
{
	static boolean initialized;
	static unsigned long read_at;
	static real_vector3d offset;
	unsigned long changes = config_changes();
	real_vector3d parsed;
	char extra;

	if (!initialized || changes != read_at)
	{
		offset.i = 0.15f;
		offset.j = -0.22f;
		offset.k = 0.30f;
		if (sscanf(config_string("vr.weapon_offset"), " %f , %f , %f %c", &parsed.i, &parsed.j, &parsed.k, &extra) == 3 &&
			parsed.i >= -1.f && parsed.i <= 1.f && parsed.j >= -1.f && parsed.j <= 1.f &&
			parsed.k >= -1.f && parsed.k <= 1.f)
			offset = parsed;
		initialized = TRUE;
		read_at = changes;
	}
	return &offset;
}

static void prepare_weapon_camera(struct render_camera const *base, struct vr_openxr_frame const *frame)
{
	static const float xr_forward[3] = { 0.f, 0.f, -1.f };
	static const float xr_up[3] = { 0.f, 1.f, 0.f };
	struct unit_datum *unit;
	real_vector3d body, left, up = *global_up3d, translation, forward, hand_up, right;
	real_vector3d const *offset;
	float rotated[3], relative[3];
	short axis;

	if (!stereo.aim_synced || !frame->focused || !frame->hand[1].aim.valid ||
		strcmp(config_string("vr.aim"), "controller") ||
		director_get_perspective(0) != _director_perspective_first_person)
		return;
	unit = (struct unit_datum *)object_try_and_get_and_verify_type(stereo.aim_unit, _object_mask_biped);
	if (!unit || unit->unit.player_index != local_player_get_player_index(0) ||
		unit->object.parent_object_index != NONE || TEST_FLAG(unit->object.damage_flags, _object_dead_bit))
		return;
	body.i = cosine(stereo.body_yaw);
	body.j = sine(stereo.body_yaw);
	body.k = 0.f;
	cross_product3d(&up, &body, &left);
	for (axis = 0; axis < 3; axis++)
		relative[axis] = frame->hand[1].aim.position[axis] - stereo.origin[axis];
	world_axes(relative, &body, &left, &up, &translation);
	quaternion_rotate(frame->hand[1].aim.orientation, xr_forward, rotated);
	world_axes(rotated, &body, &left, &up, &forward);
	quaternion_rotate(frame->hand[1].aim.orientation, xr_up, rotated);
	world_axes(rotated, &body, &left, &up, &hand_up);
	if (!valid_real_vector3d(&translation) || !valid_real_vector3d(&forward) ||
		!valid_real_vector3d(&hand_up) || normalize3d(&forward) <= 0.f)
		return;
	cross_product3d(&forward, &hand_up, &right);
	if (normalize3d(&right) <= 0.f)
		return;
	cross_product3d(&right, &forward, &hand_up);
	offset = weapon_offset();
	stereo.weapon_camera = *base;
	stereo.weapon_camera.forward = forward;
	stereo.weapon_camera.up = hand_up;
	stereo.weapon_camera.position.x += (translation.i -
		right.i * offset->i - hand_up.i * offset->j - forward.i * offset->k) / METRES_PER_WORLD_UNIT;
	stereo.weapon_camera.position.y += (translation.j -
		right.j * offset->i - hand_up.j * offset->j - forward.j * offset->k) / METRES_PER_WORLD_UNIT;
	stereo.weapon_camera.position.z += (translation.k -
		right.k * offset->i - hand_up.k * offset->j - forward.k * offset->k) / METRES_PER_WORLD_UNIT;
	stereo.weapon_camera_valid = valid_real_point3d(&stereo.weapon_camera.position);
}

boolean vr_render_eye_pass(void)
{
	return stereo.active && stereo.eye != NONE;
}

boolean vr_render_weapon_camera(struct render_camera *camera)
{
	if (!camera || !vr_render_eye_pass() || !stereo.weapon_camera_valid)
		return FALSE;
	camera->position = stereo.weapon_camera.position;
	camera->forward = stereo.weapon_camera.forward;
	camera->up = stereo.weapon_camera.up;
	return TRUE;
}

short vr_render_prepare(struct render_window *windows, short count, struct render_window **prepared)
{
	struct vr_openxr_frame frame;
	short eye;

	stereo.active = FALSE;
	stereo.listener_valid = FALSE;
	stereo.weapon_camera_valid = FALSE;
	stereo.eye = NONE;
	*prepared = windows;
	if (!xgpu_openxr_frame_begin(&frame))
		return count;
	/* A single local 3D view and its existing console/interface view. Keep
	   split-screen and screenshot paths intact; never submit half an eye pair. */
	if (!frame.should_render || count != 2 || windows[0].console_window || !windows[1].console_window)
		return count;
	if (stereo.aim_synced)
	{
		long player_index = windows[0].local_player_index != NONE ?
			local_player_get_player_index(windows[0].local_player_index) : NONE;
		struct player_datum *player = player_index != NONE ? player_try_and_get(player_index) : NULL;

		if (!player || player->unit_index != stereo.aim_unit)
			stereo.aim_synced = FALSE;
	}
	if (!stereo.origin_valid || stereo.reference_serial != frame.reference_serial)
		recenter(&frame);
	if (windows[0].local_player_index == 0)
		prepare_weapon_camera(&windows[0].render_camera, &frame);
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

/* Adapted from OpenH1's halo_vr_aim and vr_aim_converge at the pin above.
   Native shots still originate at Halo's deterministic eye. Converge that
   facing on the controller's hit point, without moving the simulated body. */
boolean vr_render_player_aim(short local_player_index, long unit_index, boolean permitted,
	real previous_yaw, real turn_delta, real_euler_angles2d *facing, real_vector2d *throttle)
{
	static const float xr_forward[3] = { 0.0f, 0.0f, -1.0f };
	struct vr_openxr_frame frame;
	struct unit_datum *unit;
	struct collision_result collision;
	real_point3d eye, origin, point;
	real_vector3d forward, left, up = *global_up3d, offset, direction, ray, from_line, aim, eye_forward;
	float rotated[3], translation[3];
	real distance = 100.0f, along, across, nearest, turn, turn_cos, turn_sin, move_forward;
	short axis;
	boolean hit;

	if (local_player_index != 0)
		return FALSE;
	if (permitted && stereo.aim_synced && stereo.aim_unit == unit_index)
		stereo.body_yaw = wrapped_yaw(stereo.body_yaw + turn_delta);
	unit = (struct unit_datum *)object_try_and_get_and_verify_type(unit_index, _object_mask_biped);
	if (!permitted || strcmp(config_string("vr.aim"), "controller") || !unit ||
		unit->object.parent_object_index != NONE || TEST_FLAG(unit->object.damage_flags, _object_dead_bit) ||
		unit->unit.player_index != local_player_get_player_index(local_player_index) ||
		director_get_perspective(local_player_index) != _director_perspective_first_person ||
		!stereo.origin_valid || !vr_openxr_tracking_read(&frame) || !frame.focused || !frame.should_render ||
		!frame.hand[1].aim.valid || frame.reference_serial != stereo.reference_serial)
		goto release;
	if (!stereo.aim_synced || stereo.aim_unit != unit_index)
		stereo.body_yaw = wrapped_yaw(previous_yaw + turn_delta);
	stereo.aim_unit = unit_index;
	stereo.aim_synced = TRUE;
	forward.i = cosine(stereo.body_yaw);
	forward.j = sine(stereo.body_yaw);
	forward.k = 0.0f;
	cross_product3d(&up, &forward, &left);
	director_camera_deterministic(unit_index, &eye, &eye_forward);
	for (axis = 0; axis < 3; axis++)
		translation[axis] = frame.hand[1].aim.position[axis] - stereo.origin[axis];
	world_axes(translation, &forward, &left, &up, &offset);
	origin.x = eye.x + offset.i / METRES_PER_WORLD_UNIT;
	origin.y = eye.y + offset.j / METRES_PER_WORLD_UNIT;
	origin.z = eye.z + offset.k / METRES_PER_WORLD_UNIT;
	quaternion_rotate(frame.hand[1].aim.orientation, xr_forward, rotated);
	world_axes(rotated, &forward, &left, &up, &direction);
	if (!valid_real_point3d(&origin) || !valid_real_vector3d(&direction) || normalize3d(&direction) <= 0.0f ||
		global_current_collision_user_depth >= MAXIMUM_COLLISION_USER_STACK_DEPTH)
		goto release;
	scale_vector3d(&direction, distance, &ray);
	global_current_collision_users[global_current_collision_user_depth++] = _collision_user_aim_assist;
	hit = collision_test_vector(_collision_test_for_line_of_sight_flags | FLAG(_collision_test_objects_bipeds_bit),
		&origin, &ray, object_get_ultimate_parent(unit_index), &collision);
	--global_current_collision_user_depth;
	if (hit)
		distance = distance3d(&origin, &collision.point);
	vector_from_points3d(&origin, &eye, &from_line);
	along = dot_product3d(&from_line, &direction);
	across = (real)sqrt(MAX(magnitude_squared3d(&from_line) - along * along, 0.0f));
	nearest = MAX(1.5f / METRES_PER_WORLD_UNIT, 3.0f * across + along);
	distance = MAX(distance, nearest);
	point.x = origin.x + direction.i * distance;
	point.y = origin.y + direction.j * distance;
	point.z = origin.z + direction.k * distance;
	vector_from_points3d(&eye, &point, &aim);
	if (normalize3d(&aim) <= 0.0f)
		goto release;
	facing->yaw = arctangent(aim.j, aim.i);
	facing->pitch = arctangent(aim.k, (real)sqrt(aim.i * aim.i + aim.j * aim.j));
	facing->yaw = wrapped_yaw(facing->yaw);
	/* Native biped throttle is relative to desired facing (positive j is
	   left). Preserve body-directed walking while the hand aims elsewhere. */
	turn = stereo.body_yaw - facing->yaw;
	turn_cos = cosine(turn);
	turn_sin = sine(turn);
	move_forward = throttle->i;
	throttle->i = turn_cos * move_forward - turn_sin * throttle->j;
	throttle->j = turn_sin * move_forward + turn_cos * throttle->j;
	return TRUE;
release:
	/* Return native view heading to the body on loss/menu/seat transitions. */
	if (stereo.aim_synced && stereo.aim_unit == unit_index)
	{
		facing->yaw = stereo.body_yaw;
	}
	stereo.aim_synced = FALSE;
	return FALSE;
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
