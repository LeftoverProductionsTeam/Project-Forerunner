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
#include "networking/network_game_globals.h"
#include "networking/network_game_manager.h"
#include "../../port/linux/src/posix.h"
void platform_translate_path(char const *xbox_path, char *host_path, unsigned long host_path_size);
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

void cheats_forge_reset(void)
{
	forge_selection = NONE;
	forge_last_copy = NONE;
	forge_last_move.object_index = NONE;
	memset(forge_copies, 0xff, sizeof(forge_copies));
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

static struct object_datum *forge_placeable_object_get(long object_index)
{
	struct object_header_datum *header = object_header_try_and_get(object_index);
	struct object_datum *object = object_try_and_get(object_index);
	if (!header || TEST_FLAG(header->flags, _object_header_being_deleted_bit) ||
		!object || object->object.parent_object_index != NONE ||
		TEST_FLAG(object->object.damage_flags, _object_dead_bit) ||
		!TEST_FLAG(_object_mask_scenery | _object_mask_weapon | _object_mask_equipment | _object_mask_vehicle,
			object->object.type))
		return NULL;
	return object;
}

/* Port: Faber's Forge 3D selection uses the native world collision ray.
 * Use the living local player so console selection cannot target through
 * BSP geometry or from another player's camera. */
static boolean forge_aim_collision(struct collision_result *hit)
{
	long unit_index;
	struct unit_datum *unit;
	real_point3d origin;
	real_vector3d ray;

	if (!terminal_command_running || network_game_distributed_client() ||
		network_game_host_cheats_refused() || !game_in_progress())
		return FALSE;
	unit_index = player_control_get_unit_index(0);
	unit = unit_try_and_get(unit_index);
	if (!unit || unit->object.parent_object_index != NONE ||
		TEST_FLAG(unit->object.damage_flags, _object_dead_bit))
		return FALSE;

	unit_get_camera_position(unit_index, &origin);
	unit_get_aiming_vector(unit_index, &ray);
	scale_vector3d(&ray, 20.0f, &ray);
	return collision_test_vector(_collision_test_for_line_of_sight_flags |
		FLAG(_collision_test_objects_weapons_bit) | FLAG(_collision_test_objects_equipment_bit),
		&origin, &ray, unit_index, hit);
}

long cheat_forge_target(void)
{
	struct collision_result hit;
	struct object_datum *object;

	if (!forge_aim_collision(&hit) || hit.type != _collision_result_object)
		return NONE;

	object = forge_placeable_object_get(hit.object_index);
	if (!object || (object->object.type == _object_type_weapon && weapon_is_flag(hit.object_index)))
		return NONE;
	return hit.object_index;
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
	if (global_network_game_server_get() && object->object.type == _object_type_scenery)
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
	/* object_new may move the source's storage; retain no pointer across it. */
	return forge_create_copy(&placement, object->object.scale);
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

/* A move may overlap the same instance. Any other object blocks it. */
static boolean forge_space_occupied(struct location const *location, real_point3d const *center,
	real radius, long ignore_object_index)
{
	long blockers[2];
	short count = objects_in_sphere(0, _object_mask_all & ~_object_mask_projectile,
		location, center, radius, blockers, NUMBEROF(blockers));
	return count > 1 || (count == 1 && blockers[0] != ignore_object_index);
}

/* Faber's aim-based placement uses the native collision contact. Keep the
 * source's orientation and let the usual physics settle the object. */
static boolean forge_aim_placement(long object_index, struct object_datum const *object,
	long ignore_object_index, real_point3d *position)
{
	struct collision_result hit;
	struct location location;
	real_point3d center;
	real_vector3d offset;
	real radius;

	if (!forge_aim_collision(&hit) || hit.type != _collision_result_structure || hit.plane.n.k < 0.7f)
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

long cheat_forge_place(void)
{
	struct object_datum const *object = forge_placeable_object_get(forge_selection);
	real_point3d position;

	if (!object)
	{
		console_warning("forge_place needs an object selected with forge_pick");
		return NONE;
	}
	if (!forge_aim_placement(forge_selection, object, NONE, &position))
		return NONE;
	return forge_clone_at_position(forge_selection, &position);
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
		object->object.first_child_object_index != NONE)
		return NULL;

	item = item_try_and_get(object_index);
	if (item && (item->item.flags & (FLAG(_item_attached_to_unit_bit) | FLAG(_item_belongs_to_player_bit))))
		return NULL;
	if (object->object.type == _object_type_weapon && weapon_is_flag(object_index))
		return NULL;

	return object;
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
}

/* Adapt OpenCE-Forge's before-transform undo snapshot (SutlabSol/OpenCE-Forge). */
static boolean forge_remember_transform(long object_index, struct object_datum const *object)
{
	if (object->object.type == _object_type_vehicle)
	{
		struct physics_instance vehicle;
		if (!physics_instance_new(&vehicle, object_index))
			return FALSE;
		forge_last_move.vehicle_pose = vehicle.world_matrix;
	}
	forge_last_move.object_index = object_index;
	forge_last_move.bsp_index = global_structure_bsp_index;
	forge_last_move.position = object->object.position;
	forge_last_move.center = object->object.bounding_sphere_center;
	forge_last_move.forward = object->object.forward;
	forge_last_move.up = object->object.up;
	forge_last_move.radius = object->object.bounding_sphere_radius;
	forge_last_move.scale = object->object.scale;
	return TRUE;
}

boolean cheat_forge_move_selected(void)
{
	struct object_datum *object = forge_editable_copy(forge_selection);
	real_point3d position;

	if (!object || !forge_aim_placement(forge_selection, object, forge_selection, &position) ||
		!forge_remember_transform(forge_selection, object))
		return FALSE;
	forge_move_copy(forge_selection, object, &position, NULL, NULL);
	return TRUE;
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

boolean cheat_forge_rotate_selected(void)
{
	struct object_datum *object = forge_editable_copy(forge_selection);
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
		if (!physics_instance_new(&vehicle, forge_selection))
			return FALSE;
		vector_from_points3d(&position, &vehicle.world_matrix.position, &offset);
		offset = forge_quarter_turn(offset);
		point_from_line3d(&position, &offset, 1.f, &vehicle.world_matrix.position);
		vehicle.world_matrix.forward = forward;
		vehicle.world_matrix.up = up;
		cross_product3d(&up, &forward, &vehicle.world_matrix.left);
		if (!physics_placement_clear(&vehicle, forge_selection, TRUE, &blocker_index) &&
			(blocker_index != NONE || !TEST_FLAG(object->object.flags, _object_on_ground_bit) ||
			!forge_settle_turned_vehicle(&vehicle, forge_selection, &position, &center, radius, &blocker_index)))
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
			forge_space_occupied(&location, &center, radius, forge_selection))
			return FALSE;
	}
	/* Connect the turned bounding center to the BSP, as object placement does. */
	scenario_location_from_point(&location, &center);
	if (location.cluster_index == NONE || !forge_remember_transform(forge_selection, object))
		return FALSE;
	forge_move_copy(forge_selection, object, &position, &forward, &up);
	return TRUE;
}

boolean cheat_forge_undo_move(void)
{
	long object_index = forge_last_move.object_index;
	struct object_datum *object = forge_editable_copy(object_index);
	struct location location;

	if (!object || forge_last_move.bsp_index != global_structure_bsp_index ||
		object->object.scale != forge_last_move.scale || !valid_real_point3d(&forge_last_move.center) ||
		!valid_real_point3d(&forge_last_move.position))
	{
		console_warning("Forge move undo needs the same unused copy in its original BSP");
		return FALSE;
	}

	/* Match object_reconnect_to_map. A vehicle resting on a slope may have its
	 * origin inside the ground while its bounding center is in a valid leaf. */
	scenario_location_from_point(&location, &forge_last_move.center);
	if (location.cluster_index == NONE)
		scenario_location_from_point(&location, &forge_last_move.position);
	if (location.cluster_index == NONE)
	{
		console_warning("Forge move undo needs a previous position within the active BSP");
		return FALSE;
	}
	if (object->object.type == _object_type_vehicle)
	{
		struct physics_instance vehicle;
		long blocker_index;

		if (!physics_instance_new(&vehicle, object_index))
			return FALSE;
		vehicle.world_matrix = forge_last_move.vehicle_pose;
		if (!physics_placement_clear(&vehicle, object_index, FALSE, &blocker_index))
		{
			struct object_datum const *blocker = object_try_and_get(blocker_index);
			char const *name = blocker ? tag_get_name(blocker->definition_index) : NULL;
			console_warning("Forge needs clear space for the vehicle at its previous position");
			if (name) console_warning("Forge undo blocker: %08lx %s", (unsigned long)blocker_index, name);
			return FALSE;
		}
	}
	else if (forge_space_occupied(&location, &forge_last_move.center, forge_last_move.radius, object_index))
	{
		console_warning("Forge needs clear space around the copy's previous position");
		return FALSE;
	}

	forge_move_copy(object_index, object, &forge_last_move.position, &forge_last_move.forward, &forge_last_move.up);
	forge_last_move.object_index = NONE;
	return TRUE;
}

static boolean forge_remove_copy(long object_index)
{
	if (!forge_editable_copy(object_index))
		return FALSE;
	object_delete(object_index);
	if (forge_last_copy == object_index) forge_last_copy = NONE;
	if (forge_selection == object_index) forge_selection = NONE;
	if (forge_last_move.object_index == object_index) forge_last_move.object_index = NONE;
	return TRUE;
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
enum { FORGE_SIDECAR_OBJECTS = 128, FORGE_SIDECAR_BYTES = 128 * 512 + 512 };
static char const forge_header[] = "[OpenCE Forge Map]";
static char const forge_author[] = "Author: Forerunner";
static char const forge_object_scale[] = "OBJECT_SCALE|";

struct forge_sidecar_placement
{
	struct object_placement_data object;
	real scale;
};

static char const *forge_sidecar_leaf(char const *name)
{
	char const *a = strrchr(name, '\\'), *b = strrchr(name, '/');
	if (b && (!a || b > a)) a = b;
	return a ? a + 1 : name;
}

static boolean forge_sidecar_slot(char const **base, char reference[144], char path[1100], boolean saving)
{
	struct network_game const *game = network_game_get_game();
	char folder[1024];
	char const *family;

	if (!game || !network_game_forge_hosting() || !game_in_progress() ||
		!terminal_command_running || network_game_distributed_client() || network_game_host_cheats_refused())
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

static boolean forge_sidecar_object(char const *line, struct object_placement_data *placement)
{
	char *fields;
	unsigned long group = strtoul(line, &fields, 10);
	real_point3d position;
	real_euler_angles3d angles;
	short team;
	char path[256];
	long definition;
	struct location location;

	if ((group != 'vehi' && group != 'weap' && group != 'eqip' && group != 'scen') ||
		(group == 'scen' && global_network_game_server_get()) ||
		strncmp(fields, "|0|0|", 5) ||
		sscanf(fields + 5, "%f|%f|%f|%f|%f|%f|%3hd|%255[^\r\n]",
			&position.x, &position.y, &position.z, &angles.yaw, &angles.pitch, &angles.roll, &team, path) != 8 ||
		strchr(path, '|') || team < NONE || team >= NUMBER_OF_SOLO_CAMPAIGN_TEAMS ||
		!valid_real_point3d(&position) || !valid_real(angles.yaw) || !valid_real(angles.pitch) || !valid_real(angles.roll) ||
		angles.yaw < -2.f * M_PI || angles.yaw > 2.f * M_PI ||
		angles.pitch < -2.f * M_PI || angles.pitch > 2.f * M_PI || angles.roll < -2.f * M_PI || angles.roll > 2.f * M_PI)
		return FALSE;
	scenario_location_from_point(&location, &position);
	definition = tag_loaded(group, path);
	if (location.cluster_index == NONE || definition == NONE ||
		(group == 'weap' && TEST_FLAG(weapon_definition_get(definition)->weapon.flags, _weapon_must_be_readied_bit)))
		return FALSE;
	object_placement_data_new(placement, definition, NONE);
	placement->position = position;
	placement->owner_team_index = team;
	/* OpenCE-Forge stores radians using this forward/up convention. */
	{
		real cy = cosine(angles.yaw), sy = sine(angles.yaw);
		real cp = cosine(angles.pitch), sp = sine(angles.pitch);
		real cr = cosine(angles.roll), sr = sine(angles.roll);
		placement->forward.i = cy * cp;
		placement->forward.j = sy * cp;
		placement->forward.k = sp;
		placement->up.i = -cy * sp * cr + sy * sr;
		placement->up.j = -sy * sp * cr - cy * sr;
		placement->up.k = cp * cr;
	}
	return valid_real_vector3d_axes2(&placement->forward, &placement->up);
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
		struct object_datum *object;
		char const *path;
		unsigned long group;
		real pitch, yaw, roll, cp, sp, cy, sy;
		struct location location;
		char scale_row[48] = "";

		struct object_header_datum *header = handle != NONE ? object_header_try_and_get(handle) : NULL;

		if (handle != NONE)
			had_copies = TRUE;
		if (!header || TEST_FLAG(header->flags, _object_header_being_deleted_bit))
			continue;
		object = object_try_and_get(handle);
		if (!object || TEST_FLAG(object->object.damage_flags, _object_dead_bit))
			continue;
		if (!forge_placeable_object_get(handle) || object->object.name_index != NONE ||
			object->object.owner_player_index != NONE ||
			object->object.owner_team_index < NONE || object->object.owner_team_index >= NUMBER_OF_SOLO_CAMPAIGN_TEAMS)
			return FALSE;
		group = object->object.type == _object_type_vehicle ? 'vehi' :
			object->object.type == _object_type_weapon ? 'weap' :
			object->object.type == _object_type_equipment ? 'eqip' : 'scen';
		path = tag_get_name(object->definition_index);
		if (++count > FORGE_SIDECAR_OBJECTS || !path || !path[0] || strlen(path) > 255 ||
			strpbrk(path, "|\r\n") || tag_loaded(group, path) != object->definition_index ||
			object->object.variant_number != 0 || !forge_copy_scale_supported(object->object.scale) ||
			(object->object.scale != 0.f && !(object->object.scale > 0.f && object->object.scale < 100.f)) ||
			(group == 'scen' && global_network_game_server_get()) ||
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
		/* Keep the upstream OBJECT row readable. Older editors ignore the
		 * optional property and place these objects at their default scale. */
		if (object->object.scale != 0.f && object->object.scale != 1.f)
			snprintf(scale_row, sizeof(scale_row), "%s%.9g\r\n", forge_object_scale, object->object.scale);
		written = snprintf(text + *size, FORGE_SIDECAR_BYTES - *size,
			"OBJECT|%lu|0|0|%.6f|%.6f|%.6f|%.6f|%.6f|%.6f|%d|%s\r\n%s", group,
			object->object.position.x, object->object.position.y, object->object.position.z,
			yaw, pitch, roll, (int)object->object.owner_team_index, path, scale_row);
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
		success = forge_sidecar_restore(placements, count);
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
