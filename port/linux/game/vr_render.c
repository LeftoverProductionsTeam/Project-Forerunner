/*
 * OpenXR stereo camera adapter, using Halo's existing render_frame/window
 * traversal and bgfx Vulkan eye targets. Headset gameplay UNVERIFIED.
 * Quaternion rotation, OpenXR -> Halo axes, metre scale and asymmetric
 * frustum normalization adapted from OpenH1/goath1 port/linux/src/vr.c and
 * port/linux/game/vr_render.c, baed77571d53abe32b27434b4ba869f11db00fe9,
 * AGPL-3.0-or-later. The OpenH1 renderer/host is not duplicated here.
 * Controller weapon-camera placement and vr.weapon_offset defaults also
 * adapt that pin's vr_render_weapon_camera and port_config.c. UNVERIFIED.
 * Per-eye world-hit reticle projection adapts vr_render_crosshairs, using
 * this runtime's native firing preview and HUD instead of a second HUD layer.
 * Audio head clearance adapts that pin's game_sound.c::sound_vr_ears_get.
 * Optional snap turning adapts vr.c's snap latch and threshold policy.
 * Offered-weapon grip pickup adapts vr_grab.c's native swap input and vr.c's
 * grip thresholds/reach. Native ownership/range and multiplayer stay authoritative.
 * Two-handed bracing adapts vr.c::two_handed_update/weapon_pointing and
 * vr_render.c::vr_render_long_gun at the same pin, through native input/posing.
 */
#include "cseries.h"
#ifdef HALO_OPENXR
#include "cseries/cseries_windows.h"
#include "../../include/halo_math.h"
#include "vr_render.h"
#include "camera/observer.h"
#include "camera/director.h"
#include "cutscene/cinematics.h"
#include "game/players.h"
#include "game/game.h"
#include "items/weapons.h"
#include "objects/objects.h"
#include "physics/collisions.h"
#include "physics/collision_usage.h"
#include "render/render_cameras.h"
#include "render/render_cameras_internal.h"
#include "scenario/scenario.h"
#include "tag_files/tag_files.h"
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
	boolean listener_path_valid, listener_path_clear;
	long listener_path_tick;
	short listener_path_bsp;
	real_point3d listener_path_eye, listener_path_head;
	short eye;
	unsigned int reference_serial;
	real origin[3], yaw_cos, yaw_sin, origin_yaw;
	boolean aim_synced;
	boolean snap_armed;
	long aim_unit;
	int aim_hand, turn_hand;
	real body_yaw;
	boolean weapon_camera_valid, weapon_mirrored;
	struct render_camera weapon_camera;
	boolean reticle_checked, reticle_valid, reticle_scoped, reticle_visible;
	long reticle_unit, reticle_weapon;
	real_point3d reticle_target;
	real_point2d reticle_offset;
	boolean grab_armed;
	long grab_unit, grab_object, grab_until;
	unsigned int grab_reference;
	int grab_hand;
	boolean support_armed, support_held;
	long support_unit, support_weapon, support_tick;
	unsigned int support_reference;
	int support_hand;
	real support_weight;
	real_vector3d support_line;
} stereo;

static real wrapped_yaw(real yaw)
{
	while (yaw < 0.0f) yaw += 2.0f * _pi;
	while (yaw >= 2.0f * _pi) yaw -= 2.0f * _pi;
	return yaw;
}

static void controller_support_reset(void)
{
	stereo.support_armed = stereo.support_held = FALSE;
	stereo.support_weight = 0.f;
	stereo.support_unit = stereo.support_weapon = NONE;
}

void vr_render_reset_aim(void)
{
	stereo.aim_synced = FALSE;
	stereo.snap_armed = FALSE;
	stereo.aim_unit = NONE;
	stereo.weapon_camera_valid = FALSE;
	stereo.weapon_mirrored = FALSE;
	stereo.reticle_checked = stereo.reticle_scoped = FALSE;
	stereo.listener_valid = stereo.listener_path_valid = FALSE;
	stereo.grab_armed = FALSE;
	stereo.grab_unit = stereo.grab_object = NONE;
	controller_support_reset();
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

	stereo.snap_armed = FALSE;
	controller_support_reset();
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

/* Only the existing input tick advances the latch/filter. Lost rendering can
 * cancel it; two eyes or extra draw passes cannot advance it twice. */
static void controller_support_update(struct vr_openxr_frame const *frame, long unit_index, struct unit_datum const *unit)
{
	static char const *const one_handed[] = { "pistol", "needler", "flag", "ball" };
	static float const xr_forward[3] = { 0.f, 0.f, -1.f };
	struct object_datum *weapon;
	char const *name;
	real_vector3d line, pointing;
	float rotated[3];
	real along, across, length, seconds, amount;
	long weapon_index, now;
	unsigned long elapsed;
	short index;
	int hand = vr_openxr_weapon_hand(), offhand = 1 - hand;
	boolean fresh;

	if (!config_boolean("vr.two_handed") ||
		(!strcmp(config_string("audio.voice_chat"), "push_to_talk") &&
		 !strcmp(config_string("vr.push_to_talk"), offhand ? "right_grip" : "left_grip")) ||
		!frame->hand[hand].grip.valid || !frame->hand[offhand].grip.valid ||
		!(frame->hand[offhand].squeeze >= 0.f && frame->hand[offhand].squeeze <= 1.f) ||
		!VALID_INDEX(unit->unit.current_weapon_index, MAXIMUM_WEAPONS_PER_UNIT)) goto unavailable;
	weapon_index = unit->unit.weapon_object_indices[unit->unit.current_weapon_index];
	weapon = object_try_and_get_and_verify_type(weapon_index, _object_mask_weapon);
	if (!weapon || weapon->object.parent_object_index != unit_index) goto unavailable;
	name = tag_get_name(weapon->definition_index);
	if (!name) goto unavailable;
	for (index = 0; index < NUMBEROF(one_handed); index++)
		if (strstr(name, one_handed[index])) goto unavailable;
	quaternion_rotate(frame->hand[hand].aim.orientation, xr_forward, rotated);
	pointing.i = rotated[0]; pointing.j = rotated[1]; pointing.k = rotated[2];
	line.i = frame->hand[offhand].grip.position[0] - frame->hand[hand].grip.position[0];
	line.j = frame->hand[offhand].grip.position[1] - frame->hand[hand].grip.position[1];
	line.k = frame->hand[offhand].grip.position[2] - frame->hand[hand].grip.position[2];
	if (!valid_real_vector3d(&pointing) || !valid_real_vector3d(&line) || normalize3d(&pointing) <= 0.f) goto unavailable;
	along = dot_product3d(&line, &pointing);
	across = (real)sqrt(MAX(0.f, magnitude_squared3d(&line) - along * along));
	length = normalize3d(&line);
	now = game_time_get();
	elapsed = (unsigned long)now - (unsigned long)stereo.support_tick;
	fresh = stereo.support_unit != unit_index || stereo.support_weapon != weapon_index ||
		stereo.support_hand != hand || stereo.support_reference != frame->reference_serial || elapsed > 3;
	if (fresh)
	{
		controller_support_reset();
		stereo.support_unit = unit_index; stereo.support_weapon = weapon_index;
		stereo.support_hand = hand; stereo.support_reference = frame->reference_serial;
		stereo.support_line = pointing;
		elapsed = 1;
	}
	stereo.support_tick = now;
	/* A held grip cannot reattach after tracking/body/weapon/layout changes. */
	if (frame->hand[offhand].squeeze <= 0.3f)
	{
		stereo.support_armed = TRUE; stereo.support_held = FALSE;
	}
	else if (stereo.support_armed && !stereo.support_held && frame->hand[offhand].squeeze > 0.6f)
	{
		stereo.support_armed = FALSE;
		if (along > 0.08f && along < 0.70f && across < 0.18f) stereo.support_held = TRUE;
	}
	seconds = (real)elapsed / TICKS_PER_SECOND;
	if (stereo.support_held)
	{
		if (length > 0.04f)
		{
			amount = (real)(1.0 - exp(-seconds / 0.05f));
			stereo.support_line.i += (line.i - stereo.support_line.i) * amount;
			stereo.support_line.j += (line.j - stereo.support_line.j) * amount;
			stereo.support_line.k += (line.k - stereo.support_line.k) * amount;
			if (normalize3d(&stereo.support_line) <= 0.f) goto unavailable;
		}
		stereo.support_weight = MIN(1.f, stereo.support_weight + seconds / 0.15f);
	}
	else stereo.support_weight = MAX(0.f, stereo.support_weight - seconds / 0.15f);
	return;
unavailable:
	controller_support_reset();
}

static void controller_weapon_forward(struct vr_openxr_frame const *frame, float rotated[3])
{
	static float const xr_forward[3] = { 0.f, 0.f, -1.f };
	real weight = stereo.support_weight;
	quaternion_rotate(frame->hand[vr_openxr_weapon_hand()].aim.orientation, xr_forward, rotated);
	if (weight > 0.f && stereo.support_hand == vr_openxr_weapon_hand() &&
		stereo.support_reference == frame->reference_serial && frame->focused &&
		frame->hand[1 - vr_openxr_weapon_hand()].grip.valid)
	{
		weight = weight * weight * (3.f - 2.f * weight);
		rotated[0] += (stereo.support_line.i - rotated[0]) * weight;
		rotated[1] += (stereo.support_line.j - rotated[1]) * weight;
		rotated[2] += (stereo.support_line.k - rotated[2]) * weight;
	}
}

static void prepare_weapon_camera(struct render_camera const *base, struct vr_openxr_frame const *frame)
{
	static const float xr_up[3] = { 0.f, 1.f, 0.f };
	struct unit_datum *unit;
	long player_index = local_player_get_player_index(0);
	struct player_datum *player = player_index != NONE ? player_try_and_get(player_index) : NULL;
	real_vector3d body, left, up = *global_up3d, translation, forward, hand_up, right;
	real_vector3d const *offset;
	float rotated[3], relative[3];
	short axis;
	int hand = vr_openxr_weapon_hand();
	real across = hand == 0 ? -1.f : 1.f;

	stereo.weapon_mirrored = FALSE;
	if (!player || cinematic_in_progress() || director_get_perspective(0) != _director_perspective_first_person)
		return;
	unit = (struct unit_datum *)object_try_and_get_and_verify_type(player->unit_index, _object_mask_biped);
	if (!unit || unit->unit.player_index != player_index ||
		unit->object.parent_object_index != NONE || TEST_FLAG(unit->object.damage_flags, _object_dead_bit))
		return;
	if (strcmp(config_string("vr.aim"), "controller"))
	{
		/* OpenH1's gamepad path poses the weapon from the native body camera,
		   before headset turning/leaning, so looking aside does not aim it. */
		stereo.weapon_camera = *base;
		stereo.weapon_camera_valid = TRUE;
		return;
	}
	if (!stereo.aim_synced || stereo.aim_unit != player->unit_index || stereo.aim_hand != hand ||
		!frame->focused || !frame->hand[hand].aim.valid)
		return;
	body.i = cosine(stereo.body_yaw);
	body.j = sine(stereo.body_yaw);
	body.k = 0.f;
	cross_product3d(&up, &body, &left);
	for (axis = 0; axis < 3; axis++)
		relative[axis] = frame->hand[hand].aim.position[axis] - stereo.origin[axis];
	world_axes(relative, &body, &left, &up, &translation);
	controller_weapon_forward(frame, rotated);
	world_axes(rotated, &body, &left, &up, &forward);
	quaternion_rotate(frame->hand[hand].aim.orientation, xr_up, rotated);
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
		right.i * offset->i * across - hand_up.i * offset->j - forward.i * offset->k) / METRES_PER_WORLD_UNIT;
	stereo.weapon_camera.position.y += (translation.j -
		right.j * offset->i * across - hand_up.j * offset->j - forward.j * offset->k) / METRES_PER_WORLD_UNIT;
	stereo.weapon_camera.position.z += (translation.k -
		right.k * offset->i * across - hand_up.k * offset->j - forward.k * offset->k) / METRES_PER_WORLD_UNIT;
	stereo.weapon_camera_valid = valid_real_point3d(&stereo.weapon_camera.position);
	stereo.weapon_mirrored = stereo.weapon_camera_valid && hand == 0;
}

/* OpenH1 vr_hands.c::vr_hands_mirror at the retained pin: reflect each
 * finished native node about the weapon camera's forward/up plane. Keep the
 * negative handedness; rebuilding left from forward/up would undo reflection. */
void vr_render_weapon_nodes(short local_player_index, real_matrix4x3 *nodes, short count)
{
	real_vector3d normal, relative;
	short index, axis;
	if (local_player_index != 0 || !stereo.active || stereo.eye == NONE ||
		!stereo.weapon_camera_valid || !stereo.weapon_mirrored || !nodes) return;
	cross_product3d(&stereo.weapon_camera.up, &stereo.weapon_camera.forward, &normal);
	if (normalize3d(&normal) <= 0.f) return;
	for (index = 0; index < count; index++)
	{
		real_matrix4x3 *node = &nodes[index];
		real along;
		real_vector3d *basis[3];
		vector_from_points3d(&stereo.weapon_camera.position, &node->position, &relative);
		along = 2.f * dot_product3d(&relative, &normal);
		node->position.x -= along * normal.i;
		node->position.y -= along * normal.j;
		node->position.z -= along * normal.k;
		basis[0] = &node->forward; basis[1] = &node->left; basis[2] = &node->up;
		for (axis = 0; axis < 3; axis++)
		{
			along = 2.f * dot_product3d(basis[axis], &normal);
			basis[axis]->i -= along * normal.i;
			basis[axis]->j -= along * normal.j;
			basis[axis]->k -= along * normal.k;
		}
	}
}

void vr_render_weapon_draw_begin(void)
{
	xgpu_openxr_mirrored_draws(stereo.active && stereo.eye != NONE &&
		stereo.weapon_camera_valid && stereo.weapon_mirrored);
}

void vr_render_weapon_draw_end(void)
{
	xgpu_openxr_mirrored_draws(FALSE);
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

static boolean reticle_target(long unit_index, long weapon_index)
{
	real_point3d origin;
	real_vector3d direction, ray;
	struct collision_result collision;
	struct weapon_datum *weapon;
	boolean hit;

	if (global_current_collision_user_depth <= 0 ||
		global_current_collision_user_depth >= MAXIMUM_COLLISION_USER_STACK_DEPTH)
		return FALSE;
	if (weapon_index != NONE)
	{
		weapon = weapon_try_and_get(weapon_index);
		if (!weapon || weapon->object.parent_object_index != unit_index ||
			!weapon_get_projectile_ray(weapon_index, 0, &origin, &direction))
			return FALSE;
	}
	else
		director_camera_deterministic(unit_index, &origin, &direction);
	if (!valid_real_point3d(&origin) || !valid_real_vector3d(&direction) || normalize3d(&direction) <= 0.f)
		return FALSE;
	scale_vector3d(&direction, 100.f, &ray);
	global_current_collision_users[global_current_collision_user_depth++] = _collision_user_aim_assist;
	hit = collision_test_vector(_collision_test_for_projectiles_flags, &origin, &ray,
		object_get_ultimate_parent(unit_index), &collision);
	--global_current_collision_user_depth;
	if (hit)
		stereo.reticle_target = collision.point;
	else
		point_from_line3d(&origin, &direction, 100.f, &stereo.reticle_target);
	return valid_real_point3d(&stereo.reticle_target);
}

void vr_render_reticle_begin(short local_player_index, long weapon_index,
	struct render_camera const *camera, struct render_frustum const *frustum)
{
	long player_index;
	struct player_datum *player;
	struct unit_datum *unit;
	real_point2d screen;

	stereo.reticle_scoped = stereo.reticle_visible = FALSE;
	if (!vr_render_eye_pass() || local_player_index != 0 || !camera || !frustum ||
		cinematic_in_progress() || director_get_perspective(0) != _director_perspective_first_person ||
		player_control_get_zoom_level(0) != NONE)
		return;
	player_index = local_player_get_player_index(0);
	player = player_index != NONE ? player_try_and_get(player_index) : NULL;
	unit = player ? (struct unit_datum *)object_try_and_get_and_verify_type(player->unit_index, _object_mask_biped) : NULL;
	if (!unit || unit->unit.player_index != player_index || unit->object.parent_object_index != NONE ||
		TEST_FLAG(unit->object.damage_flags, _object_dead_bit))
		return;
	stereo.reticle_scoped = TRUE;
	if (!stereo.reticle_checked || stereo.reticle_unit != player->unit_index || stereo.reticle_weapon != weapon_index)
	{
		stereo.reticle_unit = player->unit_index;
		stereo.reticle_weapon = weapon_index;
		stereo.reticle_valid = reticle_target(player->unit_index, weapon_index);
		stereo.reticle_checked = TRUE;
	}
	if (!stereo.reticle_valid || !render_camera_world_to_screen(camera, frustum, &stereo.reticle_target, &screen))
		return;
	stereo.reticle_offset.x = screen.x - (real)((camera->window_bounds.x0 + camera->window_bounds.x1) / 2);
	stereo.reticle_offset.y = screen.y - (real)((camera->window_bounds.y0 + camera->window_bounds.y1) / 2);
	stereo.reticle_visible = TRUE;
}

boolean vr_render_reticle_place(point2d *point, real *reticle_scale)
{
	if (!stereo.reticle_scoped)
		return TRUE;
	if (!stereo.reticle_visible)
		return FALSE;
	point->x = (short)PIN((real)point->x + stereo.reticle_offset.x, SHORT_MIN, SHORT_MAX);
	point->y = (short)PIN((real)point->y + stereo.reticle_offset.y, SHORT_MIN, SHORT_MAX);
	/* Headset projection already places the world; flat FOV scaling would
	   move the projected reticle a second time about the HUD's centre. */
	*reticle_scale = 1.f;
	return TRUE;
}

void vr_render_reticle_end(void)
{
	stereo.reticle_scoped = FALSE;
}

short vr_render_prepare(struct render_window *windows, short count, struct render_window **prepared)
{
	struct vr_openxr_frame frame;
	short eye;

	stereo.active = FALSE;
	stereo.listener_valid = FALSE;
	stereo.weapon_camera_valid = FALSE;
	stereo.reticle_checked = stereo.reticle_scoped = FALSE;
	stereo.eye = NONE;
	*prepared = windows;
	if (!xgpu_openxr_frame_begin(&frame))
	{
		stereo.snap_armed = FALSE;
		controller_support_reset();
		return count;
	}
	if (!frame.focused || !frame.hand[0].grip.valid || !frame.hand[1].grip.valid)
		controller_support_reset();
	if (!frame.focused)
		stereo.snap_armed = FALSE;
	/* A single local 3D view and its existing console/interface view. Keep
	   split-screen and screenshot paths intact; never submit half an eye pair. */
	if (!frame.should_render || count != 2 || windows[0].console_window || !windows[1].console_window)
	{
		stereo.snap_armed = FALSE;
		controller_support_reset();
		return count;
	}
	if (stereo.aim_synced)
	{
		long player_index = windows[0].local_player_index != NONE ?
			local_player_get_player_index(windows[0].local_player_index) : NONE;
		struct player_datum *player = player_index != NONE ? player_try_and_get(player_index) : NULL;

		if (!player || player->unit_index != stereo.aim_unit)
		{
			stereo.aim_synced = FALSE;
			controller_support_reset();
		}
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
	stereo.reticle_scoped = FALSE;
}

/* OpenH1's eye-to-head clearance, shared by audio positioning and obstruction.
   Also key the native tick/BSP so moving doors and changed geometry are retried. */
static boolean listener_head_clear(real_point3d const *eye, real_point3d const *head)
{
	real_vector3d eye_to_head;
	struct collision_result collision;
	long now = game_time_get();

	if (stereo.listener_path_valid && stereo.listener_path_tick == now &&
		stereo.listener_path_bsp == global_structure_bsp_index &&
		distance_squared3d(eye, &stereo.listener_path_eye) == 0.f &&
		distance_squared3d(head, &stereo.listener_path_head) == 0.f)
		return stereo.listener_path_clear;
	if (global_current_collision_user_depth <= 0 ||
		global_current_collision_user_depth >= MAXIMUM_COLLISION_USER_STACK_DEPTH)
		return FALSE;
	vector_from_points3d(eye, head, &eye_to_head);
	global_current_collision_users[global_current_collision_user_depth++] = _collision_user_sounds;
	stereo.listener_path_clear = magnitude_squared3d(&eye_to_head) == 0.f ||
		!collision_test_vector(FLAG(_collision_test_front_facing_surfaces_bit) | FLAG(_collision_test_structure_bit) |
			FLAG(_collision_test_objects_bit) | FLAG(_collision_test_objects_scenery_bit) |
			FLAG(_collision_test_objects_machines_bit), eye, &eye_to_head, NONE, &collision);
	--global_current_collision_user_depth;
	stereo.listener_path_valid = TRUE;
	stereo.listener_path_tick = now;
	stereo.listener_path_bsp = global_structure_bsp_index;
	stereo.listener_path_eye = *eye;
	stereo.listener_path_head = *head;
	return stereo.listener_path_clear;
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
	if (listener->location.cluster_index == NONE || base->location.cluster_index == NONE)
		return FALSE;
	if (!listener_head_clear(&base->position, &listener->position))
	{
		/* Retain head orientation, but do not hear from across a solid wall. */
		listener->position = base->position;
		listener->location = base->location;
	}
	return TRUE;
}

/* OpenH1's physical-right-stick hysteresis. Native look remains available
   when the XR stick is centered; a deflected stick owns snap-mode turning. */
static real controller_turn_delta(struct vr_openxr_frame const *frame, real native_delta)
{
	int hand = vr_openxr_turn_hand();
	real x = frame->hand[hand].stick[0];
	double degrees;
	if (hand != stereo.turn_hand) stereo.snap_armed = FALSE;
	stereo.turn_hand = hand;

	if (strcmp(config_string("vr.turn"), "snap"))
	{
		stereo.snap_armed = FALSE;
		return native_delta;
	}
	if (!(x >= -1.f && x <= 1.f))
	{
		stereo.snap_armed = FALSE;
		return 0.f;
	}
	if (ABS(x) < 0.3f)
		stereo.snap_armed = TRUE;
	if (ABS(x) <= 0.001f)
		return native_delta;
	if (ABS(x) > 0.7f && stereo.snap_armed)
	{
		stereo.snap_armed = FALSE;
		degrees = config_real("vr.snap_turn_angle");
		if (!(degrees >= 1.0 && degrees <= 180.0))
			degrees = 30.0;
		return (real)(degrees * (_pi / 180.0)) * (x > 0.f ? -1.f : 1.f);
	}
	return 0.f;
}

/* OpenH1's offered-weapon swap policy, through the existing native input.
 * Only the current offered world weapon qualifies, including on a client.
 * Reaching for another object never broadens host pickup admission. */
void vr_render_player_grab(short local, long unit_index, boolean permitted, unsigned long *flags)
{
	struct vr_openxr_frame frame;
	struct player_datum *player;
	struct object_datum *weapon;
	real_point3d eye, hand_position, nearest;
	real_vector3d forward, left, up = *global_up3d, offset, delta, line;
	float translation[3];
	real along, half_length;
	long now;
	int hand, axis;
	char const *talk;
	boolean reserved;

	if (local != 0) return;
	if (!flags || !permitted || !config_boolean("vr.weapon_grab"))
	{
		stereo.grab_armed = FALSE; stereo.grab_unit = stereo.grab_object = NONE; return;
	}
	hand = vr_openxr_weapon_hand();
	talk = config_string("vr.push_to_talk");
	reserved = !strcmp(config_string("audio.voice_chat"), "push_to_talk") &&
		!strcmp(talk, hand ? "right_grip" : "left_grip");
	now = game_time_get();
	if (reserved ||
		!stereo.aim_synced || stereo.aim_unit != unit_index ||
		!vr_openxr_tracking_read(&frame) || !frame.focused || !frame.should_render ||
		frame.reference_serial != stereo.reference_serial || !frame.hand[hand].grip.valid ||
		!(frame.hand[hand].squeeze >= 0.f && frame.hand[hand].squeeze <= 1.f) ||
		!(frame.hand[hand].trigger >= 0.f && frame.hand[hand].trigger <= 0.05f))
	{
		stereo.grab_armed = FALSE; stereo.grab_unit = stereo.grab_object = NONE; return;
	}
	if (stereo.grab_unit != unit_index || stereo.grab_reference != frame.reference_serial || stereo.grab_hand != hand)
	{
		stereo.grab_armed = FALSE; stereo.grab_object = NONE;
		stereo.grab_unit = unit_index; stereo.grab_reference = frame.reference_serial; stereo.grab_hand = hand;
	}
	if (frame.hand[hand].squeeze <= 0.3f) stereo.grab_armed = TRUE;
	player = player_try_and_get(local_player_get_player_index(local));
	if (!player || player->unit_index != unit_index ||
		(player->action_result != _player_action_result_pickup_weapon &&
		 player->action_result != _player_action_result_swap_for_weapon &&
		 player->action_result != _player_action_result_add_weapon_to_inventory)) goto unavailable;
	weapon = object_try_and_get_and_verify_type(player->action_object_index, _object_mask_weapon);
	if (!weapon || weapon->object.parent_object_index != NONE) goto unavailable;
	forward.i = cosine(stereo.body_yaw); forward.j = sine(stereo.body_yaw); forward.k = 0.f;
	cross_product3d(&up, &forward, &left);
	director_camera_deterministic(unit_index, &eye, &line);
	for (axis = 0; axis < 3; axis++) translation[axis] = frame.hand[hand].grip.position[axis] - stereo.origin[axis];
	world_axes(translation, &forward, &left, &up, &offset);
	hand_position.x = eye.x + offset.i / METRES_PER_WORLD_UNIT;
	hand_position.y = eye.y + offset.j / METRES_PER_WORLD_UNIT;
	hand_position.z = eye.z + offset.k / METRES_PER_WORLD_UNIT;
	line = weapon->object.forward;
	if (!valid_real_point3d(&hand_position) || !valid_real_point3d(&weapon->object.bounding_sphere_center) ||
		!valid_real_vector3d(&line) || !(weapon->object.bounding_sphere_radius >= 0.f) || normalize3d(&line) <= 0.f) goto unavailable;
	/* OpenH1's short capsule through the actual weapon, in native world units. */
	half_length = weapon->object.bounding_sphere_radius * 0.8f;
	vector_from_points3d(&weapon->object.bounding_sphere_center, &hand_position, &delta);
	along = PIN(delta.i * line.i + delta.j * line.j + delta.k * line.k, -half_length, half_length);
	nearest.x = weapon->object.bounding_sphere_center.x + along * line.i;
	nearest.y = weapon->object.bounding_sphere_center.y + along * line.j;
	nearest.z = weapon->object.bounding_sphere_center.z + along * line.k;
	if (!(distance3d(&nearest, &hand_position) <= 0.24f / METRES_PER_WORLD_UNIT)) goto unavailable;
	if (stereo.grab_armed && frame.hand[hand].squeeze > 0.6f)
	{
		stereo.grab_armed = FALSE; stereo.grab_object = player->action_object_index; stereo.grab_until = now + 3;
	}
	if (stereo.grab_object == player->action_object_index && now >= stereo.grab_until - 3 && now < stereo.grab_until)
		*flags |= FLAG(_unit_control_action_bit) | FLAG(_unit_control_swap_weapons_bit);
	return;
unavailable:
	stereo.grab_object = NONE;
	if (frame.hand[hand].squeeze > 0.6f) stereo.grab_armed = FALSE;
}

/* Adapted from OpenH1's halo_vr_aim and vr_aim_converge at the pin above.
   Native shots still originate at Halo's deterministic eye. Converge that
   facing on the controller's hit point, without moving the simulated body. */
boolean vr_render_player_aim(short local_player_index, long unit_index, boolean permitted,
	real previous_yaw, real turn_delta, real_euler_angles2d *facing, real_vector2d *throttle)
{
	struct vr_openxr_frame frame;
	struct unit_datum *unit;
	struct collision_result collision;
	real_point3d eye, origin, point;
	real_vector3d forward, left, up = *global_up3d, offset, direction, ray, from_line, aim, eye_forward;
	float rotated[3], translation[3];
	real distance = 100.0f, along, across, nearest, turn, turn_cos, turn_sin, move_forward;
	short axis;
	boolean hit, allowed;
	int hand = vr_openxr_weapon_hand();

	if (local_player_index != 0)
		return FALSE;
	unit = (struct unit_datum *)object_try_and_get_and_verify_type(unit_index, _object_mask_biped);
	if (stereo.aim_synced && stereo.aim_hand != hand)
	{
		if (stereo.aim_unit == unit_index) facing->yaw = previous_yaw = stereo.body_yaw;
		stereo.aim_synced = FALSE;
		stereo.snap_armed = FALSE;
	}
	allowed = permitted && !strcmp(config_string("vr.aim"), "controller") && unit &&
		unit->object.parent_object_index == NONE && !TEST_FLAG(unit->object.damage_flags, _object_dead_bit) &&
		unit->unit.player_index == local_player_get_player_index(local_player_index) &&
		director_get_perspective(local_player_index) == _director_perspective_first_person &&
		stereo.origin_valid && vr_openxr_tracking_read(&frame) && frame.focused && frame.should_render &&
		frame.hand[hand].aim.valid && frame.reference_serial == stereo.reference_serial;
	if (allowed)
	{
		if (!stereo.aim_synced || stereo.aim_unit != unit_index)
			stereo.snap_armed = FALSE;
		turn_delta = controller_turn_delta(&frame, turn_delta);
	}
	else
		stereo.snap_armed = FALSE;
	if (permitted && stereo.aim_synced && stereo.aim_unit == unit_index)
		stereo.body_yaw = wrapped_yaw(stereo.body_yaw + turn_delta);
	if (!allowed)
		goto release;
	if (!stereo.aim_synced || stereo.aim_unit != unit_index)
		stereo.body_yaw = wrapped_yaw(previous_yaw + turn_delta);
	stereo.aim_unit = unit_index;
	stereo.aim_hand = hand;
	stereo.aim_synced = TRUE;
	controller_support_update(&frame, unit_index, unit);
	forward.i = cosine(stereo.body_yaw);
	forward.j = sine(stereo.body_yaw);
	forward.k = 0.0f;
	cross_product3d(&up, &forward, &left);
	director_camera_deterministic(unit_index, &eye, &eye_forward);
	for (axis = 0; axis < 3; axis++)
		translation[axis] = frame.hand[hand].aim.position[axis] - stereo.origin[axis];
	world_axes(translation, &forward, &left, &up, &offset);
	origin.x = eye.x + offset.i / METRES_PER_WORLD_UNIT;
	origin.y = eye.y + offset.j / METRES_PER_WORLD_UNIT;
	origin.z = eye.z + offset.k / METRES_PER_WORLD_UNIT;
	controller_weapon_forward(&frame, rotated);
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
	stereo.snap_armed = FALSE;
	controller_support_reset();
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
