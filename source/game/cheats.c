/*
CHEATS.C

symbols in this file:
00094C50 0020:
	_cheats_initialize (0000)
00094C70 0010:
	_cheats_dispose (0000)
00094C80 0010:
	_cheats_dispose_from_old_map (0000)
00094C90 00b0:
	_cheats_update (0000)
00094D40 0090:
	_cheats_load (0000)
00094DD0 0060:
	_cheat_active_camouflage_local_player (0000)
00094E30 0060:
	_cheat_player_index (0000)
00094E90 0010:
	_cheats_initialize_for_new_map (0000)
00094EA0 00b0:
	_cheat_teleport_to_camera (0000)
00094F50 0050:
	_cheat_active_camouflage (0000)
00094FA0 0150:
	_cheat_objects (0000)
000950F0 00d0:
	_cheat_all_weapons (0000)
000951C0 0050:
	_cheat_all_powerups (0000)
00095210 0050:
	_cheat_all_vehicles (0000)
0025AC64 003b:
	??_C@_0DL@LCJIJOPI@Cannot?5execute?5cheats?5attached?5t@ (0000)
0025ACA0 0005:
	??_C@_04JNJDPIIA@?$AN?6?7?$DL?$AA@ (0000)
0025ACA8 000e:
	??_C@_0O@KAFNGCBO@d?3?2cheats?4txt?$AA@ (0000)
0025ACB8 003a:
	??_C@_0DK@OACLODJP@Camera?5is?5outside?5BSP?4?4?4?5cannot?5@ (0000)
0025ACF4 001d:
	??_C@_0BN@DPPGFJCA@c?3?2halo?2SOURCE?2game?2cheats?4c?$AA@ (0000)
0025AD14 0004:
	__real@3ec90fdb (0000)
0043D808 0c81:
	_cheat_strings (0000)
*/

/* ---------- headers */

#include "cseries.h"
#include "cseries/errors.h"
#include "cheats.h"
#include "cache/cache_files.h"
#include "camera/director.h"
#include "camera/observer.h"
#include "game/game.h" /* port: game_connection, cheat_objects */
#include "game/game_globals.h"
#include "game/players.h"
#include "network_coop.h" /* port: existing co-op appearance replication */
#include "hs/hs.h"
#include "input/input.h"
#include "interface/terminal.h"
#include "items/items.h"
#include "items/weapon_definitions.h"
#include "items/weapons.h" /* port: objective weapons are not Forge clones */
#include "main/console.h"
#include "math/real_math.h"
#include "objects/objects.h"
#include "physics/collisions.h"
#include "physics/collision_features.h"
#include "physics/physics.h"
#include "rasterizer/rasterizer_console_vars.h"
#include "scenario/scenario.h"
#include "tag_files/tag_groups.h"
#include "units/units.h"

#if (defined(_WIN32) || defined(__linux__) || defined(__APPLE__)) && \
	!defined(HALO_XBOX_CONSOLE) && !defined(HALO_CONSOLE) && !defined(HALO_ANDROID) && \
	!defined(HALO_WEB) && !defined(HALO_BROWSER)
#define NATIVE_FORGE_SIDECARS
#include "forge_files.h"
#include "networking/network_game_globals.h"
#include "networking/network_game_manager.h"
#ifdef HALO_GAME_BROWSER
#include "../../port/linux/src/delta_peer.h"
#endif
#include "../../port/linux/src/posix.h"
void platform_translate_path(char const *xbox_path, char *host_path, unsigned long host_path_size);
char const *platform_save_root(void);
#endif
#ifdef HALO_64BIT
#include "cseries/errors.h"
#endif

/* ---------- constants */

enum
{
	MAXIMUM_CHEATS = NUMBER_OF_GAMEPAD_BUTTONS,
	MAXIMUM_CHEAT_LENGTH = 200
};

#define CHEATS_FILENAME "d:\\cheats.txt"

/* ---------- macros */

#define TAG_BLOCK_TRY_AND_GET_ELEMENT(block_address, index, type) \
	((block_address)->count ? TAG_BLOCK_GET_ELEMENT((block_address), (index), type) : NULL)

/* ---------- structures */

/* ---------- prototypes */

/* ---------- globals */

static char cheat_strings[MAXIMUM_CHEATS][MAXIMUM_CHEAT_LENGTH] = {0};
/* January emits this otherwise unreferenced byte after cheat_strings.
 * Its original name and purpose are unknown; the name is descriptive only. */
static boolean cheats_unused_flag = FALSE;

/* ---------- code */

void cheats_initialize(
	void)
{
	csmemset(cheat_strings, 0, sizeof(cheat_strings));

	return;
}

void cheats_dispose(
	void)
{
	return;
}

static long forge_selection = NONE;
static long forge_last_copy = NONE;
static long forge_copies[MAXIMUM_OBJECTS_PER_MAP];

enum { FORGE_SIDECAR_OBJECTS = 128 };
#ifdef NATIVE_FORGE_SIDECARS
enum { FORGE_SIDECAR_BYTES = FORGE_SIDECAR_OBJECTS * 512 + 512 };

struct forge_fixed_pose
{
	long object_index;
	real_point3d position;
	real_vector3d forward, up;
};

static struct forge_fixed_pose forge_fixed_poses[FORGE_SIDECAR_OBJECTS];
static short forge_fixed_count;
static boolean forge_remote_physics_active;

static struct
{
	char title[FORGE_FILE_TITLE_SIZE];
	char author[FORGE_FILE_AUTHOR_SIZE];
	char description[FORGE_FILE_DESCRIPTION_SIZE];
	char date[FORGE_FILE_DATE_SIZE];
	posix_ulong saved_size;
	unsigned int saved_crc;
} forge_hmv_metadata;
#endif

struct forge_move_snapshot
{
	long object_index;
	short bsp_index;
	real_point3d position, center;
	real_vector3d forward, up;
	real radius, scale;
	real_matrix4x3 vehicle_pose;
};

static struct forge_move_snapshot forge_last_move = { NONE };
static struct forge_move_snapshot forge_last_destination = { NONE };
static boolean forge_move_undone;

void cheats_forge_reset(void)
{
	forge_selection = NONE;
	forge_last_copy = NONE;
	forge_last_move.object_index = NONE;
	forge_last_destination.object_index = NONE;
	forge_move_undone = FALSE;
	memset(forge_copies, 0xff, sizeof(forge_copies));
#ifdef NATIVE_FORGE_SIDECARS
	forge_fixed_count = 0;
	forge_remote_physics_active = FALSE;
	memset(&forge_hmv_metadata, 0, sizeof(forge_hmv_metadata));
#endif
}

void cheats_dispose_from_old_map(
	void)
{
	cheats_forge_reset();
}

/* network_game_globals.c's */
boolean network_game_distributed_client(void);
boolean network_game_host_cheats_refused(void);
/* bipeds.c's, player_control.c's */
extern boolean rider_ejection;
extern boolean player_autoaim_flag;
extern boolean player_magnetism_flag;
/* game_time.c's */
boolean game_time_reset_speed(void);
/* network_game_globals.c's, port_config.c's, delta_peer_game.c's */
#ifndef NATIVE_FORGE_SIDECARS
struct network_game_server *global_network_game_server_get(void);
#endif
long config_integer(const char *name);
#ifdef HALO_GAME_BROWSER
int delta_peer_game_hosting_xbox(void);
#endif

/* port: the objects a network game's host made from its console (its
cheats, its object_create), this game: past network.host_console_objects
(32 unless set; 8 while an original Xbox machine plays, whose object array
is the PC's eighth) the next is refused, and none is kept at an index past
the Xbox's own half (1,024: one there takes the place of the console's own
objects, port/linux/game/network_objects.c). A map's own scripts are not
counted: the objects they make by name are the map's, one each at most */
enum
{
	CONSOLE_OBJECTS_DEFAULT = 32,
	CONSOLE_OBJECTS_WITH_XBOX = 8,
	CONSOLE_OBJECTS_XBOX_INDEX_LIMIT = 1024
};

static long cheats_console_objects_made = 0;
static boolean cheats_console_objects_refusal_logged = FALSE;

/* whether the quota counts this object: the host of a network game, a
command typed or a cheat button's (terminal_command_running) */
static boolean cheats_console_objects_counted(
	void)
{
	return terminal_command_running && global_network_game_server_get() != NULL &&
		!network_game_distributed_client();
}

static boolean cheats_console_objects_xbox(
	void)
{
#ifdef HALO_GAME_BROWSER
	return delta_peer_game_hosting_xbox() != 0;
#else
	return FALSE;
#endif
}

static void cheats_console_objects_refused(
	char const *why)
{
	if (!cheats_console_objects_refusal_logged)
	{
		error(_error_log, "the host's console: objects refused (%s; %ld made this game)", why,
			cheats_console_objects_made);
		cheats_console_objects_refusal_logged = TRUE;
	}
	console_warning("no more objects from the console this game (%s)", why);
}

boolean cheats_console_object_allowed(
	void)
{
	long limit;

	if (!cheats_console_objects_counted())
		return TRUE;
	limit = config_integer("network.host_console_objects");
	if (limit < 0)
		limit = CONSOLE_OBJECTS_DEFAULT;
	if (limit > 0 && cheats_console_objects_xbox() && limit > CONSOLE_OBJECTS_WITH_XBOX)
		limit = CONSOLE_OBJECTS_WITH_XBOX;
	if (limit > 0 && cheats_console_objects_made >= limit)
	{
		cheats_console_objects_refused("network.host_console_objects");
		return FALSE;
	}
	return TRUE;
}

boolean cheats_console_object_made(
	long object_index)
{
	if (object_index == NONE || !cheats_console_objects_counted())
		return TRUE;
	if (cheats_console_objects_xbox() &&
		DATUM_INDEX_TO_ABSOLUTE_INDEX(object_index) >= CONSOLE_OBJECTS_XBOX_INDEX_LIMIT)
	{
		object_delete(object_index);
		cheats_console_objects_refused("an original Xbox plays, and the game has many objects");
		return FALSE;
	}
	cheats_console_objects_made++;
	return TRUE;
}

#ifdef NATIVE_FORGE_SIDECARS
static boolean forge_local_host(void)
{
	return network_game_forge_hosting() && network_game_is_splitscreen_local();
}
static boolean forge_physics_supported(void)
{
#ifdef HALO_GAME_BROWSER
	if (network_game_forge_hosting() && delta_peer_game_room_has(_delta_capability_forge_physics))
		return TRUE;
#endif
	return forge_local_host();
}

/* Adapt ChupathingyCE forge_physics.inc (1b732f52, GPL-3.0) to the
 * existing Forge handles. A negotiated match uses the same fixed poses. */
static struct forge_fixed_pose *forge_fixed_pose_get(long object_index)
{
	short index;
	for (index = 0; index < forge_fixed_count; index++)
		if (forge_fixed_poses[index].object_index == object_index)
			return &forge_fixed_poses[index];
	return NULL;
}

static void forge_fixed_pose_capture(struct forge_fixed_pose *pose, long object_index)
{
	struct object_datum const *object = object_get(object_index);
	pose->object_index = object_index;
	pose->position = object->object.position;
	pose->forward = object->object.forward;
	pose->up = object->object.up;
}

static unsigned char forge_placement_physics(long object_index, struct object_datum const *object)
{
	if (TEST_FLAG(object->object.flags, _object_forge_phased_bit))
		return FORGE_RECORD_PHYSICS_PHASED;
	return forge_fixed_pose_get(object_index) ? FORGE_RECORD_PHYSICS_FIXED : 0;
}

static void forge_apply_placement_physics(long object_index, unsigned char physics)
{
	if (physics == FORGE_RECORD_PHYSICS_PHASED)
		SET_FLAG(object_get(object_index)->object.flags, _object_forge_phased_bit, TRUE);
	else if (physics == FORGE_RECORD_PHYSICS_FIXED)
		forge_fixed_pose_capture(&forge_fixed_poses[forge_fixed_count++], object_index);

	if (physics && !forge_local_host())
		forge_remote_physics_active = TRUE;
}
#endif

boolean cheats_forge_remote_physics_active(void)
{
#ifdef NATIVE_FORGE_SIDECARS
	return forge_remote_physics_active;
#else
	return FALSE;
#endif
}

enum forge_physics_mode cheats_forge_object_physics(long object_index)
{
#ifdef NATIVE_FORGE_SIDECARS
	struct object_datum const *object = object_try_and_get(object_index);
	unsigned char physics;

	if (!object)
		return _forge_physics_refused;
	physics = forge_placement_physics(object_index, object);
	if (physics == FORGE_RECORD_PHYSICS_FIXED)
		return _forge_physics_fixed;
	if (physics == FORGE_RECORD_PHYSICS_PHASED)
		return _forge_physics_phased;
	return _forge_physics_normal;
#else
	(void)object_index;
	return _forge_physics_refused;
#endif
}

enum forge_physics_mode cheats_forge_network_physics(long object_index)
{
#ifdef NATIVE_FORGE_SIDECARS
	/* Ordinary objects keep their original network description. */
	if (forge_remote_physics_active && forge_copies[DATUM_INDEX_TO_ABSOLUTE_INDEX(object_index)] == object_index)
		return cheats_forge_object_physics(object_index);
#else
	(void)object_index;
#endif
	return _forge_physics_refused;
}

void cheats_forge_receive_physics(long object_index, enum forge_physics_mode mode,
	real_point3d const *position, real_vector3d const *forward, real_vector3d const *up)
{
#ifdef NATIVE_FORGE_SIDECARS
	struct object_datum *object = object_get(object_index);
	struct forge_fixed_pose *fixed;

	if (mode < _forge_physics_normal || mode > _forge_physics_phased ||
		!TEST_FLAG(_object_mask_vehicle | _object_mask_weapon | _object_mask_equipment, object->object.type))
		return;
	fixed = forge_fixed_pose_get(object_index);
	if (mode == _forge_physics_fixed)
	{
		if (!fixed)
		{
			if (forge_fixed_count == NUMBEROF(forge_fixed_poses))
				return;
			fixed = &forge_fixed_poses[forge_fixed_count++];
			fixed->object_index = object_index;
		}
		/* Native object replication carries the edited home pose. While an
		 * item is carried the host omits its pose from world state updates. */
		fixed->position = *position;
		fixed->forward = *forward;
		fixed->up = *up;
	}
	else if (fixed)
		*fixed = forge_fixed_poses[--forge_fixed_count];
	SET_FLAG(object->object.flags, _object_forge_phased_bit, mode == _forge_physics_phased);
#else
	(void)object_index;
	(void)mode;
	(void)position;
	(void)forward;
	(void)up;
#endif
}

void cheats_forge_update(void)
{
#ifdef NATIVE_FORGE_SIDECARS
	short index = 0;
	while (index < forge_fixed_count)
	{
		struct forge_fixed_pose const *pose = &forge_fixed_poses[index];
		long handle = pose->object_index;
		struct object_header_datum *header = object_header_try_and_get(handle);
		struct object_datum *object = object_try_and_get(handle);
		if (!header || TEST_FLAG(header->flags, _object_header_being_deleted_bit) ||
			!object || TEST_FLAG(object->object.damage_flags, _object_dead_bit))
		{
			forge_fixed_poses[index] = forge_fixed_poses[--forge_fixed_count];
			continue;
		}
		index++;
		if (object->object.parent_object_index != NONE)
			continue;
		if (distance_squared3d(&object->object.position, &pose->position) > 0.005f * 0.005f ||
			dot_product3d(&object->object.forward, &pose->forward) < 0.99999f ||
			dot_product3d(&object->object.up, &pose->up) < 0.99999f ||
			magnitude_squared3d(&object->object.translational_velocity) > 0.f ||
			magnitude_squared3d(&object->object.angular_velocity) > 0.f)
		{
			object_set_position(handle, &pose->position, &pose->forward, &pose->up);
			object = object_get(handle);
			object->object.translational_velocity = *global_zero_vector3d;
			object->object.angular_velocity = *global_zero_vector3d;
			SET_FLAG(object->object.flags, _object_at_rest_bit, TRUE);
		}
	}
#endif
}

static boolean forge_has_remote_players(void)
{
#ifdef NATIVE_FORGE_SIDECARS
	if (forge_local_host())
		return FALSE;
#endif
	return global_network_game_server_get() != NULL;
}

static struct object_datum *forge_placeable_object_get(long object_index)
{
	struct object_header_datum *header = object_header_try_and_get(object_index);
	struct object_datum *object = object_try_and_get(object_index);
	if (!header || TEST_FLAG(header->flags, _object_header_being_deleted_bit) ||
		!object || object->object.parent_object_index != NONE ||
		TEST_FLAG(object->object.damage_flags, _object_dead_bit))
		return NULL;
	if (TEST_FLAG(_object_mask_scenery | _object_mask_weapon | _object_mask_equipment | _object_mask_vehicle,
		object->object.type))
		return object;
#ifdef NATIVE_FORGE_SIDECARS
	/* Exact HMV-created devices can be edited. Native scenario bindings
	 * and live linked switches stay with the original map. */
	if (forge_local_host() && TEST_FLAG(_object_mask_machine | _object_mask_control, object->object.type) &&
		forge_copies[DATUM_INDEX_TO_ABSOLUTE_INDEX(object_index)] == object_index)
		return object;
#endif
	return NULL;
}

/* Port: Faber's Forge 3D selection uses the native world collision ray.
 * Guests and hosts both use the aiming vector of a living native unit. */
static boolean forge_unit_aim_collision(long unit_index, struct collision_result *hit, boolean select_phased)
{
	struct unit_datum *unit = unit_try_and_get(unit_index);
	real_point3d origin;
	real_vector3d ray;
	if (!unit || unit->object.parent_object_index != NONE ||
		TEST_FLAG(unit->object.damage_flags, _object_dead_bit))
		return FALSE;

	unit_get_camera_position(unit_index, &origin);
	unit_get_aiming_vector(unit_index, &ray);
	scale_vector3d(&ray, 20.0f, &ray);
	return collision_test_vector(_collision_test_for_line_of_sight_flags |
		FLAG(_collision_test_objects_weapons_bit) | FLAG(_collision_test_objects_equipment_bit) |
		(select_phased ? FLAG(_collision_test_forge_phased_objects_bit) : 0),
		&origin, &ray, unit_index, hit);
}

static boolean forge_aim_collision(struct collision_result *hit, boolean select_phased)
{
	return terminal_command_running && !network_game_distributed_client() &&
		!network_game_host_cheats_refused() && game_in_progress() &&
		forge_unit_aim_collision(player_control_get_unit_index(0), hit, select_phased);
}

static long forge_target_from_hit(struct collision_result const *hit)
{
	struct object_datum *object;

	if (hit->type != _collision_result_object)
		return NONE;

	object = forge_placeable_object_get(hit->object_index);
	if (!object || (object->object.type == _object_type_weapon && weapon_is_flag(hit->object_index)))
		return NONE;
	return hit->object_index;
}

long cheats_forge_aimed_object(long unit_index)
{
	struct collision_result hit;
	return game_in_progress() && forge_unit_aim_collision(unit_index, &hit, TRUE) ?
		forge_target_from_hit(&hit) : NONE;
}

long cheat_forge_target(void)
{
	struct collision_result hit;
	return forge_aim_collision(&hit, TRUE) ? forge_target_from_hit(&hit) : NONE;
}

static boolean forge_copy_scale_supported(real scale)
{
	return !global_network_game_server_get() || scale == 0.f || scale == 1.f ||
		(network_coop_active() && !cheats_console_objects_xbox() && scale > 0.f && scale < 100.f);
}

static long forge_create_copy(struct object_placement_data *placement, real scale)
{
	long clone_index;

	if (!cheats_console_object_allowed())
		return NONE;
	SET_FLAG(placement->flags, _new_object_never_automatically_delete_bit, TRUE);
	clone_index = object_new(placement);
	if (clone_index == NONE || !cheats_console_object_made(clone_index))
		return NONE;
	objects_scripting_set_scale(clone_index, scale, 0);
	forge_copies[DATUM_INDEX_TO_ABSOLUTE_INDEX(clone_index)] = clone_index;
	forge_last_copy = clone_index;
	return clone_index;
}

/* port: Faber's native placement recipe (forge_3d_mode.c, e4ffe0fe), without
tag injection or an editor session. Copy placement, never live ownership,
inventory, AI, children or scenario-name bindings. */
static long forge_clone_at_position(
	long object_index,
	real_point3d const *position)
{
	struct object_datum *object;
	struct object_placement_data placement;
	long copy;
#ifdef NATIVE_FORGE_SIDECARS
	unsigned char physics;
#endif

	if (!terminal_command_running || network_game_distributed_client() || network_game_host_cheats_refused())
	{
		console_warning("forge_clone requires an allowed local or host console");
		return NONE;
	}
	object = forge_placeable_object_get(object_index);
	if (!object)
	{
		console_warning("forge_clone needs a live, unattached scenery, weapon, equipment or vehicle object");
		return NONE;
	}
	if (object->object.type == _object_type_weapon && weapon_is_flag(object_index))
	{
		console_warning("forge_clone cannot duplicate an objective weapon");
		return NONE;
	}
	/* port: native object creation does not replicate scenery. A host-only
	copy would leave the other machines without its geometry/collision. */
	if (forge_has_remote_players() && object->object.type == _object_type_scenery)
	{
		console_warning("forge_clone needs local play for scenery; network creation does not carry scenery");
		return NONE;
	}
	/* Co-op's existing object-looks message carries initial scale and refreshes
	missed creation. Native zero means the unscaled model, as does one.
	Ordinary multiplayer/Xbox has no scaled equivalent. Keep exactly
	the receiving scale bounds; never create a differently sized replica. */
	if (!forge_copy_scale_supported(object->object.scale))
	{
		console_warning("forge_clone needs supported native co-op for scaled network objects (scale >0 and <100; no Xbox)");
		return NONE;
	}
	if (position)
	{
		struct location location;
		if (!game_in_progress() || !valid_real_point3d(position))
			return NONE;
		scenario_location_from_point(&location, position);
		if (location.cluster_index == NONE)
		{
			console_warning("forge_clone_at needs a position inside the active BSP");
			return NONE;
		}
	}
	object_placement_data_new(&placement, object->definition_index, NONE);
	placement.position = position ? *position : object->object.position;
	placement.forward = object->object.forward;
	placement.up = object->object.up;
	placement.owner_team_index = object->object.owner_team_index;
	placement.variant_number = object->object.variant_number;
	csmemcpy(placement.change_colors, object->object.base_change_colors, sizeof(placement.change_colors));
#ifdef NATIVE_FORGE_SIDECARS
	physics = forge_placement_physics(object_index, object);
	if (physics && !forge_physics_supported())
		return NONE;
	if (physics == FORGE_RECORD_PHYSICS_FIXED && forge_fixed_count == NUMBEROF(forge_fixed_poses))
		return NONE;
#endif
	/* object_new may move the source's storage; retain no pointer across it. */
	copy = forge_create_copy(&placement, object->object.scale);
#ifdef NATIVE_FORGE_SIDECARS
	if (copy != NONE && physics)
		forge_apply_placement_physics(copy, physics);
#endif
	return copy;
}

long cheat_forge_clone(long object_index)
{
	return forge_clone_at_position(object_index, NULL);
}

long cheat_forge_clone_at(long object_index, real_point3d const *position)
{
	return position ? forge_clone_at_position(object_index, position) : NONE;
}

boolean cheat_forge_pick(void)
{
	forge_selection = cheat_forge_target();
	if (forge_selection != NONE)
		return TRUE;

	console_warning("forge_pick needs a visible Forge object within 20 units");
	return FALSE;
}

/* A solid copy can share space with Phased pieces. Keep checking past
 * those pieces so they cannot hide a solid object behind them. */
static boolean forge_space_occupied(struct location const *location, real_point3d const *center,
	real radius, long ignore_object_index)
{
	long blockers[FORGE_SIDECAR_OBJECTS + 2];
	short index, count = objects_in_sphere(0, _object_mask_all & ~_object_mask_projectile,
		location, center, radius, blockers, NUMBEROF(blockers));

	for (index = 0; index < count; index++)
	{
		long object_index = blockers[index];
		if (object_index != ignore_object_index &&
			!TEST_FLAG(object_get(object_index)->object.flags, _object_forge_phased_bit))
			return TRUE;
	}
	/* A layout beyond the saved-object limit can fill this list. */
	return count == NUMBEROF(blockers);
}

/* Faber's aim-based placement uses the native collision contact. Keep the
 * source's orientation and let the usual physics settle the object. */
static boolean forge_aim_placement(long unit_index, long object_index, struct object_datum const *object,
	long ignore_object_index, real_point3d *position)
{
	struct collision_result hit;
	struct location location;
	real_point3d center;
	real_vector3d offset;
	real radius;

	if (!forge_unit_aim_collision(unit_index, &hit, FALSE) || hit.type != _collision_result_structure || hit.plane.n.k < 0.7f)
	{
		console_warning("Forge needs visible ground within 20 units");
		return FALSE;
	}
	radius = object->object.bounding_sphere_radius;
	if (!valid_real(radius) || radius < 0.0f)
		return FALSE;

	if (object->object.type == _object_type_vehicle)
	{
		if (!physics_resting_placement(object_index, &hit.point, &hit.plane.n, ignore_object_index, position))
		{
			console_warning("Forge needs clear space for the vehicle");
			return FALSE;
		}
		scenario_location_from_point(&location, position);
		return location.cluster_index != NONE;
	}

	point_from_line3d(&hit.point, &hit.plane.n, radius + 0.06f, &center);
	if (!valid_real_point3d(&center))
		return FALSE;
	scenario_location_from_point(&location, &center);
	if (location.cluster_index == NONE || collision_test_sphere(&center, radius, NONE))
	{
		console_warning("Forge needs clear space around the target");
		return FALSE;
	}
	if (forge_space_occupied(&location, &center, radius, ignore_object_index))
	{
		console_warning("Forge needs clear space around the target");
		return FALSE;
	}

	vector_from_points3d(&object->object.bounding_sphere_center, &object->object.position, &offset);
	point_from_line3d(&center, &offset, 1.0f, position);
	if (!valid_real_point3d(position))
		return FALSE;
	scenario_location_from_point(&location, position);
	return location.cluster_index != NONE;
}

static long forge_place_copy(long unit_index, long object_index)
{
	struct object_datum const *object = forge_placeable_object_get(object_index);
	real_point3d position;

	if (!object)
	{
		console_warning("forge_place needs an object selected with forge_pick");
		return NONE;
	}
	if (!forge_aim_placement(unit_index, object_index, object, NONE, &position))
		return NONE;
	return forge_clone_at_position(object_index, &position);
}

long cheat_forge_place(void)
{
	if (!terminal_command_running || network_game_distributed_client() ||
		network_game_host_cheats_refused() || !game_in_progress())
		return NONE;
	return forge_place_copy(player_control_get_unit_index(0), forge_selection);
}

/* A copied vehicle's factory weapon is created with the vehicle as
 * its owner and is attached when readied. Stock object deletion already
 * removes that attachment. Any rider or independently owned child blocks edits. */
static boolean forge_editable_children(long object_index, struct object_datum const *object)
{
	long child_index = object->object.first_child_object_index;
	while (child_index != NONE)
	{
		struct item_datum const *child = item_try_and_get(child_index);
		if (object->object.type != _object_type_vehicle || !child ||
			child->object.type != _object_type_weapon || child->object.parent_object_index != object_index ||
			child->object.owner_object_index != object_index || child->object.owner_player_index != NONE ||
			child->object.name_index != NONE || child->object.first_child_object_index != NONE ||
			!TEST_FLAG(child->item.flags, _item_attached_to_unit_bit) ||
			TEST_FLAG(child->item.flags, _item_belongs_to_player_bit) || weapon_is_flag(child_index))
			return FALSE;
		child_index = child->object.next_object_index;
	}
	return TRUE;
}

static struct object_datum *forge_editable_copy(long object_index)
{
	long slot;
	struct object_header_datum *header;
	struct object_datum *object;
	struct item_datum *item;

	if (object_index == NONE || !terminal_command_running || !game_in_progress() ||
		network_game_distributed_client() || network_game_host_cheats_refused())
		return NULL;

	slot = DATUM_INDEX_TO_ABSOLUTE_INDEX(object_index);
	if (!VALID_INDEX(slot, NUMBEROF(forge_copies)) || forge_copies[slot] != object_index)
		return NULL;
	header = object_header_try_and_get(object_index);
	if (!header || TEST_FLAG(header->flags, _object_header_being_deleted_bit))
		return NULL;

	/* Native edits cannot disturb riders, owned equipment or scenario objects. */
	object = forge_placeable_object_get(object_index);
	if (!object || object->object.name_index != NONE || object->object.owner_player_index != NONE ||
		!forge_editable_children(object_index, object))
		return NULL;

	item = item_try_and_get(object_index);
	if (item && (item->item.flags & (FLAG(_item_attached_to_unit_bit) | FLAG(_item_belongs_to_player_bit))))
		return NULL;
	if (object->object.type == _object_type_weapon && weapon_is_flag(object_index))
		return NULL;

	return object;
}

boolean cheats_forge_physics_available(void)
{
#ifdef NATIVE_FORGE_SIDECARS
	return forge_physics_supported();
#else
	return FALSE;
#endif
}

#ifdef NATIVE_FORGE_SIDECARS
static boolean forge_phase_exit_clear(long object_index, struct object_datum const *object)
{
	struct location location;
	real_point3d const *center = &object->object.bounding_sphere_center;
	real radius = object->object.bounding_sphere_radius;

	if (object->object.type == _object_type_vehicle)
	{
		struct physics_instance vehicle;
		long blocker_index;

		/* The vehicle is already resting against the BSP. Check other objects. */
		if (!physics_instance_new(&vehicle, object_index))
			return FALSE;
		if (!physics_placement_clear(&vehicle, object_index, FALSE, &blocker_index))
		{
			struct object_datum const *blocker = object_try_and_get(blocker_index);
			if (blocker)
				error(_error_log, "Forge: %s prevents restoring solid collision.", tag_get_name(blocker->definition_index));
			return FALSE;
		}
		return TRUE;
	}

	scenario_location_from_point(&location, center);
	return location.cluster_index != NONE && !forge_space_occupied(&location, center, radius, object_index);
}
#endif

static enum forge_physics_mode forge_cycle_copy_physics(long object_index)
{
#ifdef NATIVE_FORGE_SIDECARS
	struct object_datum *object;
	struct forge_fixed_pose *fixed;

	if (!cheats_forge_physics_available() || !(object = forge_editable_copy(object_index)) ||
		(!forge_local_host() && !TEST_FLAG(_object_mask_vehicle | _object_mask_weapon | _object_mask_equipment,
			object->object.type)))
		return _forge_physics_refused;

	if (TEST_FLAG(object->object.flags, _object_forge_phased_bit))
	{
		if (!forge_phase_exit_clear(object_index, object))
			return _forge_physics_refused;
		SET_FLAG(object->object.flags, _object_forge_phased_bit, FALSE);
		SET_FLAG(object->object.flags, _object_at_rest_bit, FALSE);
		object_activate(object_index);
		return _forge_physics_normal;
	}

	fixed = forge_fixed_pose_get(object_index);
	if (fixed)
	{
		*fixed = forge_fixed_poses[--forge_fixed_count];
		forge_apply_placement_physics(object_index, FORGE_RECORD_PHYSICS_PHASED);
		SET_FLAG(object->object.flags, _object_at_rest_bit, FALSE);
		object_activate(object_index);
		return _forge_physics_phased;
	}

	if (forge_fixed_count == NUMBEROF(forge_fixed_poses))
		return _forge_physics_refused;
	forge_apply_placement_physics(object_index, FORGE_RECORD_PHYSICS_FIXED);
	object->object.translational_velocity = *global_zero_vector3d;
	object->object.angular_velocity = *global_zero_vector3d;
	SET_FLAG(object->object.flags, _object_at_rest_bit, TRUE);
#ifdef HALO_GAME_BROWSER
	if (!forge_local_host())
	{
		extern void network_objects_forge_physics_changed(long object_index);
		network_objects_forge_physics_changed(object_index);
	}
#endif
	return _forge_physics_fixed;
#else
	(void)object_index;
	return _forge_physics_refused;
#endif
}

enum forge_physics_mode cheat_forge_cycle_physics(void)
{
	return forge_cycle_copy_physics(forge_selection);
}

/* The existing object state sender and placement writer see the same handle. */
static void forge_move_copy(long object_index, struct object_datum *object,
	real_point3d const *position, real_vector3d const *forward, real_vector3d const *up)
{
	object->object.translational_velocity = *global_zero_vector3d;
	object->object.angular_velocity = *global_zero_vector3d;
	SET_FLAG(object->object.flags, _object_at_rest_bit, FALSE);
	object_set_position(object_index, position, forward, up);
	object_activate(object_index);
#ifdef NATIVE_FORGE_SIDECARS
	if (forge_fixed_count)
	{
		struct forge_fixed_pose *pose = forge_fixed_pose_get(object_index);
		if (pose)
			forge_fixed_pose_capture(pose, object_index);
	}
#endif
}

/* Adapt OpenCE-Forge's before-transform undo snapshot (SutlabSol/OpenCE-Forge). */
static boolean forge_remember_transform(long object_index, struct object_datum const *object,
	struct forge_move_snapshot *snapshot)
{
	if (object->object.type == _object_type_vehicle)
	{
		struct physics_instance vehicle;
		if (!physics_instance_new(&vehicle, object_index))
			return FALSE;
		snapshot->vehicle_pose = vehicle.world_matrix;
	}
	snapshot->object_index = object_index;
	snapshot->bsp_index = global_structure_bsp_index;
	snapshot->position = object->object.position;
	snapshot->center = object->object.bounding_sphere_center;
	snapshot->forward = object->object.forward;
	snapshot->up = object->object.up;
	snapshot->radius = object->object.bounding_sphere_radius;
	snapshot->scale = object->object.scale;
	return TRUE;
}

/* Like ChupathingyCE's Forge history, retain the edited pose itself rather
 * than capturing the possibly shifted body when undo is requested. */
static void forge_commit_transform(long object_index, struct object_datum *object,
	real_point3d const *position, real_vector3d const *forward, real_vector3d const *up)
{
	forge_move_copy(object_index, object, position, forward, up);
	forge_last_destination.object_index = NONE;
	forge_remember_transform(object_index, object, &forge_last_destination);
	forge_move_undone = FALSE;
}

static boolean forge_move_selected_copy(long unit_index, long object_index)
{
	struct object_datum *object = forge_editable_copy(object_index);
	real_point3d position;

	if (!object || !forge_aim_placement(unit_index, object_index, object, object_index, &position) ||
		!forge_remember_transform(object_index, object, &forge_last_move))
		return FALSE;
	forge_commit_transform(object_index, object, &position, NULL, NULL);
	return TRUE;
}

boolean cheat_forge_move_selected(void)
{
	return forge_move_selected_copy(player_control_get_unit_index(0), forge_selection);
}

static boolean forge_settle_turned_vehicle(struct physics_instance *vehicle, long object_index,
	real_point3d *position, real_point3d *center, real radius, long *blocker_index)
{
	struct collision_result floor;
	real_vector3d down = { 0.f, 0.f, -radius };
	real lift;

	if (!collision_test_vector(_collision_test_environment_flags |
		FLAG(_collision_test_front_facing_surfaces_bit), position, &down, object_index, &floor) ||
		floor.type != _collision_result_structure || floor.plane.n.k < 0.7f ||
		!physics_placement_lift(vehicle, &floor.point, &floor.plane.n, &lift))
		return FALSE;

	/* Apply the support-plane clearance vertically so the pivot keeps its XY. */
	lift = (lift + 0.06f) / floor.plane.n.k;
	if (!valid_real(lift) || lift > radius)
		return FALSE;
	position->z += lift;
	center->z += lift;
	vehicle->world_matrix.position.z += lift;
	return physics_placement_clear(vehicle, object_index, TRUE, blocker_index);
}

static real_vector3d forge_quarter_turn(real_vector3d v)
{
	real_vector3d turned = { -v.j, v.i, v.k };
	return turned;
}

static boolean forge_rotate_copy(long object_index)
{
	struct object_datum *object = forge_editable_copy(object_index);
	real_point3d position, center;
	real_vector3d forward, up, offset;
	real radius;
	struct location location;

	if (!object)
	{
		console_warning("Forge rotation needs a selected, unused copy");
		return FALSE;
	}
	position = object->object.position;
	radius = object->object.bounding_sphere_radius;
	if (!valid_real_point3d(&position) || !valid_real(radius) || radius < 0.f)
		return FALSE;
	forward = forge_quarter_turn(object->object.forward);
	up = forge_quarter_turn(object->object.up);
	vector_from_points3d(&position, &object->object.bounding_sphere_center, &offset);
	offset = forge_quarter_turn(offset);
	point_from_line3d(&position, &offset, 1.f, &center);
	if (!valid_real_point3d(&center))
		return FALSE;
	if (object->object.type == _object_type_vehicle)
	{
		struct physics_instance vehicle;
		long blocker_index;
		if (!physics_instance_new(&vehicle, object_index))
			return FALSE;
		vector_from_points3d(&position, &vehicle.world_matrix.position, &offset);
		offset = forge_quarter_turn(offset);
		point_from_line3d(&position, &offset, 1.f, &vehicle.world_matrix.position);
		vehicle.world_matrix.forward = forward;
		vehicle.world_matrix.up = up;
		cross_product3d(&up, &forward, &vehicle.world_matrix.left);
		if (!physics_placement_clear(&vehicle, object_index, TRUE, &blocker_index) &&
			(blocker_index != NONE || !TEST_FLAG(object->object.flags, _object_on_ground_bit) ||
			!forge_settle_turned_vehicle(&vehicle, object_index, &position, &center, radius, &blocker_index)))
		{
			struct object_datum const *blocker = object_try_and_get(blocker_index);
			char const *name = blocker ? tag_get_name(blocker->definition_index) : NULL;
			console_warning("Forge rotation needs space around the turned vehicle");
			if (name) console_warning("Forge rotation blocker: %s", name);
			return FALSE;
		}
	}
	else
	{
		scenario_location_from_point(&location, &center);
		if (location.cluster_index == NONE || collision_test_sphere(&center, radius, NONE) ||
			forge_space_occupied(&location, &center, radius, object_index))
			return FALSE;
	}
	/* Connect the turned bounding center to the BSP, as object placement does. */
	scenario_location_from_point(&location, &center);
	if (location.cluster_index == NONE || !forge_remember_transform(object_index, object, &forge_last_move))
		return FALSE;
	forge_commit_transform(object_index, object, &position, &forward, &up);
	return TRUE;
}

boolean cheat_forge_rotate_selected(void)
{
	return forge_rotate_copy(forge_selection);
}

int cheats_forge_guest_edits_allowed(void)
{
#if defined(NATIVE_FORGE_SIDECARS) && defined(HALO_GAME_BROWSER)
	extern int config_boolean(char const *name);
	extern int p2p_hosting_public_game(void);
	return network_game_forge_hosting() && !forge_local_host() &&
		config_boolean("network.forge_guest_edits") && !p2p_hosting_public_game() &&
		!network_game_host_cheats_refused();
#else
	return 0;
#endif
}

static boolean forge_restore_transform(boolean redo)
{
	struct forge_move_snapshot const *target = redo ? &forge_last_destination : &forge_last_move;
	long object_index = target->object_index;
	struct object_datum *object = forge_editable_copy(object_index);
	struct location location;

	if (redo != forge_move_undone || !object || target->bsp_index != global_structure_bsp_index ||
		object->object.scale != target->scale || !valid_real_point3d(&target->center) ||
		!valid_real_point3d(&target->position))
	{
		console_warning("Forge transform history needs the same unused copy in its original BSP");
		return FALSE;
	}

	/* Match object_reconnect_to_map. A vehicle resting on a slope may have its
	 * origin inside the ground while its bounding center is in a valid leaf. */
	scenario_location_from_point(&location, &target->center);
	if (location.cluster_index == NONE)
		scenario_location_from_point(&location, &target->position);
	if (location.cluster_index == NONE)
	{
		console_warning("Forge transform history needs a previous position within the active BSP");
		return FALSE;
	}
	if (object->object.type == _object_type_vehicle)
	{
		struct physics_instance vehicle;
		long blocker_index;

		if (!physics_instance_new(&vehicle, object_index))
			return FALSE;
		vehicle.world_matrix = target->vehicle_pose;
		if (!physics_placement_clear(&vehicle, object_index, FALSE, &blocker_index))
		{
			struct object_datum const *blocker = object_try_and_get(blocker_index);
			char const *name = blocker ? tag_get_name(blocker->definition_index) : NULL;
			console_warning("Forge needs clear space for the vehicle at its saved position");
			if (name) console_warning("Forge transform blocker: %08lx %s", (unsigned long)blocker_index, name);
			return FALSE;
		}
	}
	else if (forge_space_occupied(&location, &target->center, target->radius, object_index))
	{
		console_warning("Forge needs clear space around the copy's previous position");
		return FALSE;
	}

	forge_move_copy(object_index, object, &target->position, &target->forward, &target->up);
	forge_move_undone = !redo;
	return TRUE;
}

boolean cheat_forge_undo_move(void)
{
	return forge_restore_transform(FALSE);
}

boolean cheat_forge_redo_move(void)
{
	return forge_restore_transform(TRUE);
}

static boolean forge_remove_copy(long object_index)
{
	if (!forge_editable_copy(object_index))
		return FALSE;
	object_delete(object_index);
	if (forge_last_copy == object_index) forge_last_copy = NONE;
	if (forge_selection == object_index) forge_selection = NONE;
	if (forge_last_move.object_index == object_index)
	{
		forge_last_move.object_index = NONE;
		forge_last_destination.object_index = NONE;
		forge_move_undone = FALSE;
	}
	return TRUE;
}

boolean cheats_forge_guest_edit(enum forge_guest_action action, long unit_index, long object_index)
{
	boolean old_terminal_state = terminal_command_running;
	boolean edited = FALSE;

	if (object_index == NONE || !cheats_forge_guest_edits_allowed())
		return FALSE;
	terminal_command_running = TRUE;
	switch (action)
	{
	case _forge_guest_rotate: edited = forge_rotate_copy(object_index); break;
	case _forge_guest_place: edited = forge_place_copy(unit_index, object_index) != NONE; break;
	case _forge_guest_move: edited = forge_move_selected_copy(unit_index, object_index); break;
	case _forge_guest_remove: edited = forge_remove_copy(object_index); break;
	case _forge_guest_cycle_physics: edited = forge_cycle_copy_physics(object_index) != _forge_physics_refused; break;
	default: break;
	}
	terminal_command_running = old_terminal_state;
	return edited;
}

boolean cheat_forge_undo(void)
{
	return forge_remove_copy(forge_last_copy);
}

boolean cheat_forge_remove_selected(void)
{
	return forge_remove_copy(forge_selection);
}


#ifdef NATIVE_FORGE_SIDECARS
/* Base-map OBJECT rows from OpenCE-Forge (SutlabSol, 91099ce3,
 * forge_runtime_tags.c and forge_3d_mode.c). AGPL-3.0-or-later. */
static char const forge_header[] = "[OpenCE Forge Map]";
static char const forge_author[] = "Author: Forerunner";
static char const forge_object_scale[] = "OBJECT_SCALE|";

struct forge_sidecar_placement
{
	struct object_placement_data object;
	real scale;
	unsigned char physics;
};

static char const *forge_sidecar_leaf(char const *name)
{
	char const *a = strrchr(name, '\\'), *b = strrchr(name, '/');
	if (b && (!a || b > a)) a = b;
	return a ? a + 1 : name;
}

static boolean forge_layout_host(void)
{
	return network_game_get_game() && network_game_forge_hosting() && game_in_progress() &&
		terminal_command_running && !network_game_distributed_client() && !network_game_host_cheats_refused();
}

static boolean forge_sidecar_slot(char const **base, char reference[144], char path[1100], boolean saving)
{
	struct network_game const *game = network_game_get_game();
	char folder[1024];
	char const *family;

	if (!forge_layout_host())
		return FALSE;
	*base = forge_sidecar_leaf(game->map.name);
	if (!(*base)[0] || strlen(*base) >= 128 || strstr(*base, "..") ||
		strspn(*base, "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789_-. @[]()+") != strlen(*base))
		return FALSE;
	family = strchr(*base, '@');
	if (family)
	{
		if (_stricmp(family, "@ce") || family == *base)
			return FALSE;
		snprintf(reference, 144, "custom_maps\\%.*s", (int)(family - *base), *base);
	}
	else
		snprintf(reference, 144, "%s", *base);
	platform_translate_path("z:\\custom_maps", folder, sizeof(folder));
	if (!posix_mcc_share_path_safe(folder))
		return FALSE;
	if (saving)
		posix_make_directory(folder);
	return snprintf(path, 1100, "%s/forge-%s.txt", folder, *base) < 1100 && posix_mcc_share_path_safe(path);
}

static boolean forge_layout_placement(unsigned long group, char const *path, real_point3d const *position,
	real_euler_angles3d const *angles, short team, boolean hmv, struct object_placement_data *placement)
{
	long definition;
	struct location location;

	/* ChupathingyCE Forge creates machines and controls with object_new
	 * and leaves their scenario links to the map. The co-op state sender
	 * assumes those devices already exist on each peer, so host them locally. */
	boolean device = hmv && forge_local_host() && (group == 'mach' || group == 'ctrl');
	if ((!device && group != 'vehi' && group != 'weap' && group != 'eqip' && group != 'scen') ||
		(group == 'scen' && forge_has_remote_players()) ||
		strchr(path, '|') || team < NONE || team >= NUMBER_OF_SOLO_CAMPAIGN_TEAMS ||
		!valid_real_point3d(position) || !valid_real(angles->yaw) || !valid_real(angles->pitch) || !valid_real(angles->roll) ||
		angles->yaw < -2.f * M_PI || angles->yaw > 2.f * M_PI ||
		angles->pitch < -2.f * M_PI || angles->pitch > 2.f * M_PI || angles->roll < -2.f * M_PI || angles->roll > 2.f * M_PI)
		return FALSE;
	scenario_location_from_point(&location, position);
	definition = tag_loaded(group, path);
	if (location.cluster_index == NONE || definition == NONE ||
		(group == 'weap' && TEST_FLAG(weapon_definition_get(definition)->weapon.flags, _weapon_must_be_readied_bit)))
		return FALSE;
	object_placement_data_new(placement, definition, NONE);
	placement->position = *position;
	placement->owner_team_index = team;
	/* OpenCE-Forge and Chupathingy layouts use the same facing convention. */
	{
		real cy = cosine(angles->yaw), sy = sine(angles->yaw);
		real cp = cosine(angles->pitch), sp = sine(angles->pitch);
		real cr = cosine(angles->roll), sr = sine(angles->roll);
		placement->forward.i = cy * cp;
		placement->forward.j = sy * cp;
		placement->forward.k = sp;
		placement->up.i = -cy * sp * cr + sy * sr;
		placement->up.j = -sy * sp * cr - cy * sr;
		placement->up.k = cp * cr;
	}
	return valid_real_vector3d_axes2(&placement->forward, &placement->up);
}

static boolean forge_sidecar_object(char const *line, struct object_placement_data *placement)
{
	char *fields;
	unsigned long group = strtoul(line, &fields, 10);
	real_point3d position;
	real_euler_angles3d angles;
	short team;
	char path[256];

	return !strncmp(fields, "|0|0|", 5) &&
		sscanf(fields + 5, "%f|%f|%f|%f|%f|%f|%3hd|%255[^\r\n]",
			&position.x, &position.y, &position.z, &angles.yaw, &angles.pitch, &angles.roll, &team, path) == 8 &&
		forge_layout_placement(group, path, &position, &angles, team, FALSE, placement);
}

static boolean forge_sidecar_base_matches(char const *name, char const *reference)
{
	static char const custom_maps[] = "custom_maps";
	if (!_strnicmp(reference, "custom_maps\\", sizeof(custom_maps)))
		return !_strnicmp(name, custom_maps, sizeof(custom_maps) - 1) &&
			(name[sizeof(custom_maps) - 1] == '\\' || name[sizeof(custom_maps) - 1] == '/') &&
			!_stricmp(name + sizeof(custom_maps), reference + sizeof(custom_maps));
	if (!_stricmp(name, reference))
		return TRUE;
	return (!_strnicmp(name, "levels\\", 7) || !_strnicmp(name, "levels/", 7)) &&
		!_stricmp(forge_sidecar_leaf(name), reference);
}

static boolean forge_sidecar_parse(char *text, char const *reference, struct forge_sidecar_placement *placements,
	short *count, boolean own)
{
	boolean header = FALSE, base_found = FALSE, author_found = FALSE, state = FALSE;
	*count = 0;
	while (*text)
	{
		char *line = text;
		text += strcspn(text, "\r\n");
		if (*text)
		{
			*text++ = 0;
			text += strspn(text, "\r\n");
		}
		if (!header)
		{
			if (strcmp(line, forge_header))
				return FALSE;
			header = TRUE;
		}
		else if (state)
		{
			if (!strncmp(line, forge_object_scale, sizeof(forge_object_scale) - 1))
			{
				char const *value = line + sizeof(forge_object_scale) - 1;
				char *end;
				real scale = strtof(value, &end);

				if (!*count || placements[*count - 1].scale != 0.f ||
					end == value || *end || !(scale > 0.f && scale < 100.f) ||
					!forge_copy_scale_supported(scale))
					return FALSE;
				placements[*count - 1].scale = scale;
			}
			else
			{
				if (*count >= FORGE_SIDECAR_OBJECTS || strncmp(line, "OBJECT|", 7) ||
					!forge_sidecar_object(line + 7, &placements[*count].object))
					return FALSE;
				placements[*count].scale = 0.f;
				placements[*count].physics = 0;
				(*count)++;
			}
		}
		else if (!strcmp(line, "[ForgeState]"))
			state = TRUE;
		else if (!strncmp(line, "Base Map: ", 10))
		{
			if (base_found || !forge_sidecar_base_matches(line + 10, reference))
				return FALSE;
			base_found = TRUE;
		}
		else if (!strcmp(line, forge_author))
			author_found = TRUE;
	}
	return state && base_found && (!own || author_found);
}

static struct object_datum *forge_layout_copy(long handle)
{
	struct object_header_datum *header = handle != NONE ? object_header_try_and_get(handle) : NULL;
	struct object_datum *object;

	if (!header || TEST_FLAG(header->flags, _object_header_being_deleted_bit))
		return NULL;
	object = object_try_and_get(handle);
	return object && !TEST_FLAG(object->object.damage_flags, _object_dead_bit) ? object : NULL;
}

static boolean forge_layout_pose(long handle, struct object_datum const *object,
	unsigned long *tag_group, char const **tag_path, real_euler_angles3d *angles)
{
	char const *path;
	unsigned long group;
	real pitch, yaw, roll, cp, sp, cy, sy;
	struct location location;

	if (!forge_placeable_object_get(handle) || object->object.name_index != NONE ||
		object->object.owner_player_index != NONE ||
		object->object.owner_team_index < NONE || object->object.owner_team_index >= NUMBER_OF_SOLO_CAMPAIGN_TEAMS)
		return FALSE;
	group = tag_get_group_tag(object->definition_index);
	path = tag_get_name(object->definition_index);
	if (!path || !path[0] || strlen(path) > 255 ||
		strpbrk(path, "|\r\n") || tag_loaded(group, path) != object->definition_index ||
		object->object.variant_number != 0 || !forge_copy_scale_supported(object->object.scale) ||
		(object->object.scale != 0.f && !(object->object.scale > 0.f && object->object.scale < 100.f)) ||
		(group == 'scen' && forge_has_remote_players()) ||
		(group == 'weap' && weapon_is_flag(handle)) ||
		!valid_real_point3d(&object->object.position) ||
		!valid_real_vector3d_axes2(&object->object.forward, &object->object.up))
		return FALSE;
	scenario_location_from_point(&location, &object->object.position);
	if (location.cluster_index == NONE)
		return FALSE;
	pitch = arcsine(PIN(object->object.forward.k, -1.f, 1.f));
	cp = cosine(pitch); sp = sine(pitch);
	if (object->object.forward.k > .9995f || object->object.forward.k < -.9995f)
	{
		real sign = object->object.forward.k > 0.f ? -1.f : 1.f;
		yaw = arctangent(sign * object->object.up.j, sign * object->object.up.i);
		roll = 0.f;
	}
	else
	{
		yaw = arctangent(object->object.forward.j, object->object.forward.i);
		cy = cosine(yaw); sy = sine(yaw);
		roll = arctangent(sy * object->object.up.i - cy * object->object.up.j,
			-cy * sp * object->object.up.i - sy * sp * object->object.up.j + cp * object->object.up.k);
	}
	*tag_group = group;
	*tag_path = path;
	angles->yaw = yaw;
	angles->pitch = pitch;
	angles->roll = roll;
	return TRUE;
}

static boolean forge_sidecar_capture(char *text, char const *base, char const *reference, posix_ulong *size)
{
	boolean had_copies = FALSE;
	short count = 0;
	long slot;
	int written = snprintf(text, FORGE_SIDECAR_BYTES, "%s\r\nForgeSource=UserCreated\r\nMap Name: %s\r\nBase Map: %s\r\n%s\r\n[ForgeState]\r\n",
		forge_header, base, reference, forge_author);

	if (written < 0 || written >= FORGE_SIDECAR_BYTES)
		return FALSE;
	*size = (posix_ulong)written;
	for (slot = 0; slot < NUMBEROF(forge_copies); slot++)
	{
		long handle = forge_copies[slot];
		struct object_datum *object = forge_layout_copy(handle);
		char const *path;
		unsigned long group;
		real_euler_angles3d angles;
		char scale_row[48] = "";

		if (handle != NONE)
			had_copies = TRUE;
		if (!object)
			continue;
		if (++count > FORGE_SIDECAR_OBJECTS || !forge_layout_pose(handle, object, &group, &path, &angles) ||
			group == 'mach' || group == 'ctrl' || forge_placement_physics(handle, object))
			return FALSE;
		/* Keep the upstream OBJECT row readable. Older editors ignore the
		 * optional property and place these objects at their default scale. */
		if (object->object.scale != 0.f && object->object.scale != 1.f)
			snprintf(scale_row, sizeof(scale_row), "%s%.9g\r\n", forge_object_scale, object->object.scale);
		written = snprintf(text + *size, FORGE_SIDECAR_BYTES - *size,
			"OBJECT|%lu|0|0|%.6f|%.6f|%.6f|%.6f|%.6f|%.6f|%d|%s\r\n%s", group,
			object->object.position.x, object->object.position.y, object->object.position.z,
			angles.yaw, angles.pitch, angles.roll, (int)object->object.owner_team_index, path, scale_row);
		if (written < 0 || (posix_ulong)written >= FORGE_SIDECAR_BYTES - *size)
			return FALSE;
		*size += (posix_ulong)written;
	}
	/* Saving after an intentional removal may leave an empty layout.
	 * A freshly opened session cannot accidentally overwrite its save. */
	return had_copies;
}

static boolean forge_sidecar_restore(struct forge_sidecar_placement const *placements, short count)
{
	long created[FORGE_SIDECAR_OBJECTS], slot, previous_count = cheats_console_objects_made;
	short index;

	for (slot = 0; slot < NUMBEROF(forge_copies); slot++)
		if (forge_copies[slot] != NONE && object_try_and_get(forge_copies[slot]))
			return FALSE; /* Load onto a fresh layout, never duplicate existing copies. */
	for (index = 0; index < count; index++)
	{
		struct forge_sidecar_placement const *saved = &placements[index];
		struct object_placement_data placement = saved->object;
		created[index] = forge_create_copy(&placement, saved->scale);
		if (created[index] == NONE)
		{
			while (index > 0)
			{
				long handle = created[--index];
				forge_copies[DATUM_INDEX_TO_ABSOLUTE_INDEX(handle)] = NONE;
				object_delete(handle);
			}
			forge_last_copy = NONE;
			cheats_console_objects_made = previous_count;
			return FALSE;
		}
		/* Native vehicle birth raises the hull above the input position.
		 * Apply the saved pose to the new copy before the next physics tick. */
		object_set_position(created[index], &saved->object.position,
			&saved->object.forward, &saved->object.up);
	}
	/* A fresh layout owns the new handles only after every placement succeeds. */
	forge_fixed_count = 0;
	for (index = 0; index < count; index++)
		forge_apply_placement_physics(created[index], placements[index].physics);
	return TRUE;
}
#endif

boolean cheat_forge_sidecar(boolean saving)
{
#ifdef NATIVE_FORGE_SIDECARS
	char const *base;
	char reference[144], path[1100], lock_path[1105];
	struct forge_sidecar_placement *placements;
	char *text;
	int lock = -1, read;
	posix_ulong size;
	short count;
	boolean success = FALSE;

	if (!forge_sidecar_slot(&base, reference, path, saving))
		return FALSE;
	text = malloc(FORGE_SIDECAR_BYTES + 1);
	placements = malloc(FORGE_SIDECAR_OBJECTS * sizeof(*placements));
	if (!text || !placements)
		goto done;
	if (saving)
	{
		snprintf(lock_path, sizeof(lock_path), "%s.lock", path);
		lock = posix_gear_lock(lock_path);
		if (lock < 0)
			goto done;
	}
	read = posix_gear_read(path, text, FORGE_SIDECAR_BYTES, &size);
	if (read < 0 || (read && memchr(text, 0, size)))
		goto done;
	text[size] = 0;
	if (read && !forge_sidecar_parse(text, reference, placements, &count, saving))
		goto done;
	if (saving)
		success = forge_sidecar_capture(text, base, reference, &size) && posix_gear_replace(path, text, size);
	else if (read)
	{
		success = forge_sidecar_restore(placements, count);
		if (success)
			memset(&forge_hmv_metadata, 0, sizeof(forge_hmv_metadata));
	}
done:
	posix_gear_unlock(lock);
	free(placements);
	free(text);
	return success;
#else
	(void)saving;
	return FALSE;
#endif
}

#ifdef NATIVE_FORGE_SIDECARS
static char const forge_hmv_title[] = "Forerunner placements";
static char const forge_hmv_author[] = "Forerunner";

static boolean forge_hmv_folder(char map[FORGE_FILE_MAP_NAME_SIZE], char folder[1060], boolean saving)
{
	char root[1024];
	char const *scenario;
	size_t i, length;

	if (!forge_layout_host() || global_scenario_index == NONE)
		return FALSE;
	scenario = tag_get_name(global_scenario_index);
	if (!scenario)
		return FALSE;
	scenario = forge_sidecar_leaf(scenario);
	length = strlen(scenario);
	if (length >= FORGE_FILE_MAP_NAME_SIZE)
		return FALSE;
	for (i = 0; i <= length; i++)
		map[i] = scenario[i] >= 'A' && scenario[i] <= 'Z' ? scenario[i] - 'A' + 'a' : scenario[i];
	if (!forge_file_map_name_valid(map))
		return FALSE;

	/* Chupathingy layouts are directly under the save root. Translating
	 * through an emulated Xbox drive would add a separate drive folder. */
	if (snprintf(root, sizeof(root), "%s/forge", platform_save_root()) >= sizeof(root) ||
		snprintf(folder, 1060, "%s/%s", root, map) >= 1060 ||
		!posix_mcc_share_path_safe(root) || !posix_mcc_share_path_safe(folder))
		return FALSE;
	if (saving)
	{
		posix_make_directory(root);
		posix_make_directory(folder);
	}
	return TRUE;
}

static boolean forge_hmv_capture(struct forge_file_layout *layout, char const *map)
{
	boolean had_copies = FALSE;
	long slot;

	memset(layout, 0, sizeof(*layout));
	strcpy(layout->map_name, map);
	if (forge_hmv_metadata.title[0])
	{
		strcpy(layout->title, forge_hmv_metadata.title);
		strcpy(layout->author, forge_hmv_metadata.author);
		strcpy(layout->description, forge_hmv_metadata.description);
		strcpy(layout->date, forge_hmv_metadata.date);
	}
	else
	{
		strcpy(layout->title, forge_hmv_title);
		strcpy(layout->author, forge_hmv_author);
	}
	layout->map_hash = (unsigned int)cache_files_get_checksum();
	for (slot = 0; slot < NUMBEROF(forge_copies); slot++)
	{
		long handle = forge_copies[slot];
		struct object_datum *object = forge_layout_copy(handle);
		struct forge_file_record *record;
		char const *path;
		unsigned long group;
		real_euler_angles3d angles;
		short i;

		if (handle != NONE)
			had_copies = TRUE;
		if (!object)
			continue;
		if (layout->record_count >= FORGE_SIDECAR_OBJECTS ||
			(object->object.scale != 0.f && object->object.scale != 1.f) ||
			!forge_layout_pose(handle, object, &group, &path, &angles) ||
			!forge_file_path_valid(path, (int)strlen(path)))
			return FALSE;
		for (i = 0; i < layout->path_count; i++)
			if (layout->paths[i].group_tag == group && !strcmp(layout->paths[i].path, path))
				break;
		if (i == layout->path_count)
		{
			layout->paths[i].group_tag = (unsigned int)group;
			strcpy(layout->paths[i].path, path);
			layout->path_count++;
		}
		record = &layout->records[layout->record_count++];
		record->path_index = (unsigned short)i;
		record->kind = _forge_record_object;
		record->team = (signed char)object->object.owner_team_index;
		memcpy(record->position, object->object.position.n, sizeof(record->position));
		memcpy(record->angles, angles.n, sizeof(record->angles));
		record->count = 1;
		record->flags = FORGE_RECORD_PLACE_AT_START | forge_placement_physics(handle, object);
	}
	return had_copies;
}

/* Validated layouts may still contain properties that the current native
 * placement path cannot express. Refuse the entire batch before spawning. */
static boolean forge_hmv_placements(struct forge_file_layout const *layout, struct forge_sidecar_placement *placements)
{
	short i;
	if (layout->pack_count || layout->flags || layout->record_count > FORGE_SIDECAR_OBJECTS ||
		layout->map_hash != (unsigned int)cache_files_get_checksum())
		return FALSE;
	for (i = 0; i < layout->record_count; i++)
	{
		struct forge_file_record const *record = &layout->records[i];
		struct forge_file_path const *path;
		real_point3d position;
		real_euler_angles3d angles;
		unsigned char physics = record->flags & FORGE_RECORD_PHYSICS_MASK;
		if (record->kind != _forge_record_object || (record->flags & ~FORGE_RECORD_PHYSICS_MASK) != FORGE_RECORD_PLACE_AT_START ||
			(physics && !forge_physics_supported()) ||
			record->game_types || record->variant || record->respawn_seconds || record->ai_team ||
			record->count != 1 || record->path_index >= layout->path_count)
			return FALSE;
		path = &layout->paths[record->path_index];
		memcpy(position.n, record->position, sizeof(record->position));
		memcpy(angles.n, record->angles, sizeof(record->angles));
		if (!forge_layout_placement(path->group_tag, path->path, &position, &angles, record->team, TRUE, &placements[i].object))
			return FALSE;
		placements[i].scale = 1.f;
		placements[i].physics = physics;
	}
	return TRUE;
}
#endif

boolean cheat_forge_hmv(enum forge_hmv_action action, char const **title, char const **author)
{
#ifdef NATIVE_FORGE_SIDECARS
	char map[FORGE_FILE_MAP_NAME_SIZE], folder[1060], path[1100], lock_path[1105];
	unsigned char *bytes = NULL;
	struct forge_file_layout *layout = NULL;
	struct forge_sidecar_placement *placements = NULL;
	int lock = -1, read, written;
	posix_ulong size;
	boolean success = FALSE;
	boolean saving = action == _forge_hmv_save;
	boolean saved_file = action != _forge_hmv_load;

	*title = *author = NULL;
	if (!forge_hmv_folder(map, folder, saving))
		return FALSE;
	snprintf(path, sizeof(path), "%s/%s", folder, saved_file ? "forerunner.hmv" : "import.hmv");
	if (!posix_mcc_share_path_safe(path))
		return FALSE;
	bytes = malloc(FORGE_FILE_MAXIMUM_SIZE);
	layout = malloc(sizeof(*layout));
	placements = malloc(FORGE_SIDECAR_OBJECTS * sizeof(*placements));
	if (!bytes || !layout || !placements)
		goto done;
	if (saving)
	{
		snprintf(lock_path, sizeof(lock_path), "%s.lock", path);
		lock = posix_gear_lock(lock_path);
		if (lock < 0)
			goto done;
	}
	read = posix_gear_read(path, bytes, FORGE_FILE_MAXIMUM_SIZE, &size);
	if (!read && !saved_file)
	{
		saved_file = TRUE;
		snprintf(path, sizeof(path), "%s/forerunner.hmv", folder);
		if (!posix_mcc_share_path_safe(path))
			goto done;
		read = posix_gear_read(path, bytes, FORGE_FILE_MAXIMUM_SIZE, &size);
	}
	if (read < 0 || (read && (forge_file_decode(bytes, (int)size, map, layout, NULL, 0) != _forge_file_ok ||
		!forge_hmv_placements(layout, placements))))
		goto done;
	if (saving)
	{
		/* An edited import owns the save slot only after it has written it
		 * or explicitly loaded it. Keep existing legacy saves writable. */
		if (read && (forge_hmv_metadata.saved_size != size ||
			forge_file_crc32(0, bytes, (int)size) != forge_hmv_metadata.saved_crc) &&
			(forge_hmv_metadata.title[0] || strcmp(layout->title, forge_hmv_title) ||
			strcmp(layout->author, forge_hmv_author) || layout->description[0] || layout->date[0]))
			goto done;
		if (forge_hmv_capture(layout, map))
		{
			written = forge_file_encode(layout, bytes, FORGE_FILE_MAXIMUM_SIZE, NULL);
			success = written > 0 && posix_gear_replace(path, bytes, (posix_ulong)written);
			if (success)
			{
				forge_hmv_metadata.saved_size = (posix_ulong)written;
				forge_hmv_metadata.saved_crc = forge_file_crc32(0, bytes, written);
			}
		}
	}
	else if (read && forge_sidecar_restore(placements, layout->record_count))
	{
		strcpy(forge_hmv_metadata.title, layout->title);
		strcpy(forge_hmv_metadata.author, layout->author);
		strcpy(forge_hmv_metadata.description, layout->description);
		strcpy(forge_hmv_metadata.date, layout->date);
		forge_hmv_metadata.saved_size = saved_file ? size : 0;
		forge_hmv_metadata.saved_crc = saved_file ? forge_file_crc32(0, bytes, (int)size) : 0;
		*title = forge_hmv_metadata.title;
		*author = forge_hmv_metadata.author;
		success = TRUE;
	}
done:
	posix_gear_unlock(lock);
	free(placements);
	free(layout);
	free(bytes);
	return success;
#else
	(void)action;
	*title = *author = NULL;
	return FALSE;
#endif
}

/* port: what the map's own scripts set rider_ejection to (TRUE until they
set it, and again with each new map). A client in another's game keeps it,
as Halo PC's clients do, which run the map's scripts too: a map that turns
ejection off (Coldsnap's settings script) would otherwise have its clients
throw their players out of an overturned vehicle the host keeps them in
(cheats_network_client_enforce) */
static boolean cheats_map_rider_ejection = TRUE;

void cheats_map_set_rider_ejection(
	boolean value)
{
	cheats_map_rider_ejection = value;

	return;
}

/* port: what the machine draws of the world put back as it draws it for
everyone: nothing seen through walls (wireframe, a drawing mode, the
environment or its parts left out), past fog, or under grass and water;
whether any was other */
static boolean cheats_network_client_rasterizer_enforce(
	void)
{
	boolean changed = FALSE;
	boolean *environment_part;

	if (rasterizer_debug_options.wireframe_enabled || rasterizer_debug_options.drawing_mode != 0 ||
		!rasterizer_debug_options.draw_water || !rasterizer_debug_options.draw_detail_objects ||
		!rasterizer_debug_options.draw_lens_flares || !rasterizer_debug_options.fog_atmospheric_enabled ||
		!rasterizer_debug_options.fog_planar_enabled)
	{
		changed = TRUE;
	}
	rasterizer_debug_options.wireframe_enabled = FALSE;
	rasterizer_debug_options.drawing_mode = 0;
	rasterizer_debug_options.draw_water = TRUE;
	rasterizer_debug_options.draw_detail_objects = TRUE;
	rasterizer_debug_options.draw_lens_flares = TRUE;
	rasterizer_debug_options.fog_atmospheric_enabled = TRUE;
	rasterizer_debug_options.fog_planar_enabled = TRUE;
	/* (the environment and its parts, lightmaps to screen fog, all drawn,
	as rasterizer_frame_begin leaves them from its switch: 2, given) */
	for (environment_part = &rasterizer_debug_options.draw_environment_lightmaps;
		environment_part <= &rasterizer_debug_options.draw_environment_fog_screen;
		environment_part++)
	{
		if (!*environment_part)
			changed = TRUE;
		*environment_part = TRUE;
	}
	if (rasterizer_debug_options.draw_environment != 2)
		changed = TRUE;
	rasterizer_debug_options.draw_environment = 2;

	return changed;
}

/* port: a client in another's game plays by the host's rules: none of its
own cheats, its own game speed or its own changes to how players play, nor
what its drawing shows it that others' does not, set before it joined too
(after, hs_compile_and_evaluate refuses them); each frame and each tick. So
does the host of a public internet game with others in it, unless
network.host_cheats is on (network_game_host_cheats_refused): its cheats
set before (init.txt, a game alone) are put back as the first player
joins */
void cheats_network_client_enforce(
	void)
{
	static struct cheat_globals const none = { 0 };
	boolean changed;
	boolean host = FALSE;

	if (!network_game_distributed_client())
	{
		if (!network_game_host_cheats_refused())
			return;
		host = TRUE;
	}
	changed = csmemcmp(&cheat, &none, sizeof(cheat)) != 0 || rider_ejection != cheats_map_rider_ejection ||
		!player_autoaim_flag || !player_magnetism_flag;
	csmemset(&cheat, 0, sizeof(cheat));
	rider_ejection = cheats_map_rider_ejection;
	player_autoaim_flag = TRUE;
	player_magnetism_flag = TRUE;
	changed |= game_time_reset_speed();
	changed |= cheats_network_client_rasterizer_enforce();
	if (changed && host)
	{
		console_warning("hosting a public game with other players: cheats, game speed and drawing put back "
			"(network.host_cheats = true allows them, and Online Games shows it)");
		error(_error_log, "hosting a public game with other players: cheats, game speed and drawing put back");
	}
	else if (changed)
	{
		console_warning("playing in another's game: its host's rules (cheats, game speed and drawing put back)");
		error(_error_log, "playing in another's game: cheats, game speed and drawing put back to the host's");
	}

	return;
}

void cheats_update(
	void)
{
	cheats_network_client_enforce();
	if (cheat.controller_enabled)
	{
		short local_player_index;

		for (local_player_index = local_player_get_next(NONE);
			local_player_index!=NONE;
			local_player_index = local_player_get_next(local_player_index))
		{
			struct gamepad_state const *gamepad = input_get_gamepad_state(local_player_index);

			if (gamepad && gamepad->buttons[_gamepad_binary_button_back]>0)
			{
				short button_index;

				for (button_index = 0; button_index<MAXIMUM_CHEATS; button_index++)
				{
					if (cheat_strings[button_index][0] && gamepad->buttons[button_index]>0)
					{
						director_inhibit_input(local_player_index);

						if (gamepad->buttons[button_index]==1)
						{
							console_printf(FALSE, cheat_strings[button_index]);
							if (!hs_compile_and_evaluate(cheat_strings[button_index]))
								cheat_strings[button_index][0] = 0;
						}
					}
				}
			}
		}
	}

	return;
}

void cheats_load(
	void)
{
	FILE *file = fopen(CHEATS_FILENAME, "r");

	if (file)
	{
		short cheat_index;

		for (cheat_index = 0; cheat_index<MAXIMUM_CHEATS; cheat_index++)
		{
			if (!fgets(cheat_strings[cheat_index], MAXIMUM_CHEAT_LENGTH-1, file))
				break;

			csstrtok(cheat_strings[cheat_index], "\r\n\t;");

			if ((cheat_index==_gamepad_binary_button_back || cheat_index==_gamepad_binary_button_start)
				&& cheat_strings[cheat_index][0])
			{
				cheat_strings[cheat_index][0] = 0;
				error(2, "Cannot execute cheats attached to the back or start button");
			}
		}

		fclose(file);
	}

	return;
}

void cheat_active_camouflage_local_player(
	short local_player_index)
{
	if (local_player_index>=0 && local_player_index<MAXIMUM_LOCAL_PLAYERS)
	{
		long player_index = local_player_get_player_index(local_player_index);

		if (player_index!=NONE)
		{
			struct player_datum *player = player_get(player_index);
			struct unit_datum *unit = unit_get(player->unit_index);

			unit->unit.active_camouflage = 1.0f;
			if (TEST_FLAG(unit->unit.flags, _unit_active_camouflaged_bit))
				unit->unit.flags |= FLAG(_unit_super_camouflaged_bit);
			unit->unit.flags |= FLAG(_unit_active_camouflaged_bit);
		}
	}

	return;
}

static long cheat_player_index(
	void)
{
	struct data_iterator iterator;
	struct player_datum *player;
	long player_index = NONE;

	data_iterator_new(&iterator, player_data);
	while ((player = (struct player_datum *)data_iterator_next(&iterator))!=NULL)
	{
		if (player->unit_index!=NONE)
		{
			player_index = iterator.datum_index;
			break;
		}
	}

	return player_index;
}

void cheats_initialize_for_new_map(
	void)
{
	/* port: (the new map's scripts set their own) */
	cheats_map_rider_ejection = TRUE;
	/* port: (each game's console objects counted anew) */
	cheats_console_objects_made = 0;
	cheats_console_objects_refusal_logged = FALSE;
	cheats_load();

	return;
}

void cheat_teleport_to_camera(
	void)
{
	long player_index = cheat_player_index();

	if (player_index!=NONE)
	{
		struct player_datum *player = player_get(player_index);

		if (player->local_player_index!=NONE)
		{
			struct observer_result const *result = observer_get_camera(player->local_player_index);

			match_assert("c:\\halo\\SOURCE\\game\\cheats.c", 256, result);

			if (result->location.cluster_index!=NONE)
			{
				long object_index = unit_get(player->unit_index)->object.parent_object_index;

				if (object_index==NONE)
					object_index = player->unit_index;

				object_set_position(object_index, &result->position, NULL, NULL);
			}
			else
			{
				terminal_printf(global_real_argb_orange, "Camera is outside BSP... cannot initiate teleportation...");
			}
		}
	}

	return;
}

void cheat_active_camouflage(
	void)
{
	long player_index = cheat_player_index();

	if (player_index!=NONE)
	{
		struct player_datum *player = player_get(player_index);
		struct unit_datum *unit = unit_get(player->unit_index);

		unit->unit.active_camouflage = 1.0f;
		if (TEST_FLAG(unit->unit.flags, _unit_active_camouflaged_bit))
			unit->unit.flags |= FLAG(_unit_super_camouflaged_bit);
		unit->unit.flags |= FLAG(_unit_active_camouflaged_bit);
	}

	return;
}

static void cheat_objects(
	struct tag_reference *references,
	short reference_count)
{
	long player_index = cheat_player_index();

	if (player_index!=NONE)
	{
		struct player_datum *player = player_get(player_index);
		struct unit_datum *unit = unit_get(player->unit_index);
		real_point3d origin;
		real_vector3d forward, up;
		short reference_index;

		object_get_origin(player->unit_index, &origin);
		object_get_orientation(player->unit_index, &forward, &up);

		for (reference_index = 0; reference_index<reference_count; reference_index++)
		{
			if (references[reference_index].index!=NONE)
			{
				struct object_placement_data data;
				real angle_separation = CEILING(2*_pi/reference_count, _pi/8);
				real angle = arctangent(forward.i, forward.j)+
					(reference_index-reference_count/2)*angle_separation;

				/* port: a network game's host makes no flag or ball (a weapon
				that must be readied): those belong to the game engine, and
				clients refuse one it does not own. Found in chester's GulchCE. */
				if (game_connection() == _game_connection_network_server &&
					object_definition_get(references[reference_index].index)->object.type == _object_type_weapon &&
					TEST_FLAG(weapon_definition_get(references[reference_index].index)->weapon.flags,
						_weapon_must_be_readied_bit))
				{
					continue;
				}

				object_placement_data_new(&data, references[reference_index].index, NONE);

				data.position.x = cosine(angle)*1.5f+origin.x;
				data.position.y = sine(angle)*1.5f+origin.y;
				data.position.z = origin.z+0.8f;
				data.forward = forward;
				data.up = up;

				/* port: a network game's host, no more than its quota */
				if (!cheats_console_object_allowed())
					break;
				cheats_console_object_made(object_new(&data));
			}
		}
	}

	return;
}

/* port: a block's references (the map's count and address): none from a
block with no count or no address, and no more than a short counts (the
count was cast to one, and wrapped) */
static void cheat_objects_from_block(
	struct tag_block const *block)
{
	if (block->count > 0 && block->address)
	{
		cheat_objects(
			XBOX_POINTER(struct tag_reference, block->address),
			(short)MIN(block->count, SHORT_MAX));
	}

	return;
}

void cheat_all_weapons(
	void)
{
	struct game_globals *globals = scenario_get_game_globals();

	if (TAG_BLOCK_TRY_AND_GET_ELEMENT(&globals->weapon_list, 0, struct tag_reference))
	{
		cheat_objects_from_block(&globals->weapon_list);
	}
	else
	{
		struct tag_reference references[MAXIMUM_CHEATS];
		struct tag_iterator iterator;
		long tag_index;
		short reference_count = 0;

		tag_iterator_new(&iterator, WEAPON_DEFINITION_TAG);
		for (tag_index = tag_iterator_next(&iterator);
			tag_index!=NONE && reference_count<NUMBEROF(references);
			tag_index = tag_iterator_next(&iterator))
		{
			references[reference_count].index = tag_index;
			reference_count++;
		}

		cheat_objects(references, reference_count);
	}

	return;
}

void cheat_all_powerups(
	void)
{
	struct game_globals *globals = scenario_get_game_globals();

	cheat_objects_from_block(&globals->cheat_powerups);

	return;
}

void cheat_all_vehicles(
	void)
{
	struct game_globals *globals = scenario_get_game_globals();

	/* port: a block with an address, too */
	if (globals->multiplayer_information.count > 0 && globals->multiplayer_information.address)
	{
		cheat_objects_from_block(
			&TAG_BLOCK_GET_ELEMENT(&globals->multiplayer_information, 0,
				struct game_globals_multiplayer_information)->vehicles);
	}

	return;
}
