/*
COOP_ENEMIES.C

Network co-op's extra enemies: Server Setup's EXTRA ENEMIES
(network.coop_enemies_mode). PER PLAYER (network.coop_enemies, a
percentage): for each player past the first, each squad of the players'
enemies that a level places (encounters.c's encounter_create) gets that
much of its count more; at 100%, two players meet twice the squad, four
players four times it, but no squad grows past COOP_ENEMIES_MAXIMUM_GROWTH
times its size (at 100%, 64 players made squads 64 times as large: more
than the ground round them, the clients' objects or the host's ticks could
take). STATIC MULTIPLIER (network.coop_enemies_multiplier):
each squad is that many times as large, for any number of players.

Extra enemies are placed in widening rings around the squad's starting
locations, on a spot on the same floor, reachable without passing through
a wall, crate or machine, with room to stand and nobody already there. A
starting location with no room left passes its enemy to the squad's next
one; if none has room, the enemy goes where extra enemies went before free
ground was looked for (rings around the starting location, whoever stands
there), so a squad still gets every enemy the setting asks for.

A squad a script loads into a dropship (vehicle_load_magic) can be larger
than the dropship's seats. The riders left without a seat are erased, and
kept: once the dropship's own riders get out (vehicle_unload, or any other
way), they are placed beside those riders, a few ticks apart, and drop and
fight as they do. A dropship gone first takes them with it.

Only the host runs the AI, so all of this is the host's.
*/

#include "cseries.h"
#include "ai/actor_placement.h"
#include "ai/actors.h"
#include "ai/actor_activation.h"
#include "ai/actor_definitions.h"
#include "ai/actor_types.h"
#include "ai/props.h"
#include "ai/ai_scenario_definitions.h"
#include "ai/encounters.h"
#include "game/game.h"
#include "game/game_allegiance.h"
#include "game/players.h"
#include "main/main.h"
#include "cutscene/cinematics.h"
#include "interface/hud_messaging.h"
#include "networking/network_game_globals.h"
#include "networking/network_game_manager.h"
#include "networking/network_server_manager.h"
#include "objects/objects.h"
#include "physics/collisions.h"
#include "scenario/scenario.h"
#include "scenario/scenario_definitions.h"
#include "tag_files/tag_groups.h"
#include "units/units.h"
#include "units/bipeds.h"
#include "units/unit_definitions.h"

#include "coop_enemies.h"
#include "network_coop.h"
#include "network_distributed.h"

#include <math.h>
#include <string.h>
#include <stdio.h>

/* port_config.c's */
long config_integer(char const *name);
char const *config_string(char const *name);
int config_boolean(char const *name);
void platform_log(char const *format, ...);

/* ---------- constants */

enum
{
	/* the actors a level keeps for its own placements, which the extra
	enemies never take: as many as the Xbox's whole pool */
	COOP_ENEMIES_LEVEL_ACTORS = 256,
	/* EXTRA ENEMIES' amounts, as Server Setup has them: 25 to 200% per
	player, or 2 to 32 times (none is its NONE) */
	COOP_ENEMIES_MINIMUM_PERCENT = 25,
	COOP_ENEMIES_MAXIMUM_PERCENT = 200,
	COOP_ENEMIES_MINIMUM_MULTIPLIER = 2,
	COOP_ENEMIES_MAXIMUM_MULTIPLIER = 32,
	/* the largest PER PLAYER makes a squad, in times its size */
	COOP_ENEMIES_MAXIMUM_GROWTH = 8,
	/* the rings tried around a starting location, the first holding
	SPREAD_PLACES_PER_RING places and each further one that many more */
	SPREAD_RINGS = 5,
	SPREAD_PLACES_PER_RING = 6,
	/* the rings coop_enemies_fallback_position tries, a place on each */
	FALLBACK_RINGS = 8,
	MAXIMUM_RIDING_VEHICLES = 32,
	MAXIMUM_SEATED_RIDERS = 16,
	MAXIMUM_KEPT_RIDERS = 512,
	/* a kept rider placed every this many ticks, as the seated ones
	jumped out */
	RIDER_RELEASE_TICKS = 4,
};

/* (coop_enemies.h's count of the places on all the rings) */
typedef char coop_enemies_spread_spots_assert[
	COOP_ENEMIES_SPREAD_SPOTS == SPREAD_PLACES_PER_RING * SPREAD_RINGS * (SPREAD_RINGS + 1) / 2 ? 1 : -1];

/* distance between rings */
#define SPREAD_SPACING 0.8f
/* height above the floor at which the way to a spot is checked */
#define SPREAD_STEP_HEIGHT 0.6f
/* how far a spot's floor may be above or below the starting location's:
a step, not a ledge */
#define SPREAD_FLOOR_DIFFERENCE 0.5f
/* room a spot needs above its floor */
#define SPREAD_HEADROOM 1.6f
/* how close across the ground a spot may be to a biped already there
(about two bipeds' collision radius), and how far around it to look */
#define SPREAD_CLEARANCE 0.6f
#define SPREAD_SEARCH_RADIUS 1.0f
/* how far below a fallback place the ground is looked for */
#define FALLBACK_GROUND_DEPTH 2.0f
/* a kept rider's place beside a rider who got out, each a little apart */
#define RIDER_SPACING 0.5f

/* EXTRA ENEMIES' choices (network.coop_enemies_mode's values) */
enum
{
	_coop_enemies_none,
	_coop_enemies_per_player,
	_coop_enemies_multiplier,
};

/* ---------- structures */

/* a vehicle a script loaded riders into: its seated ones, and whether they
have begun to get out */
struct coop_riding_vehicle
{
	long vehicle_index;
	long seated_unit_indices[MAXIMUM_SEATED_RIDERS];
	short seat_indices[MAXIMUM_SEATED_RIDERS];
	short seated_count;
	boolean releasing;
	short released_count;
	long next_release_time;
};

/* a rider left without a seat, to place once its vehicle's get out */
struct coop_kept_rider
{
	long vehicle_index;
	long variant_definition_index;
	long encounter_index;
	short squad_index;
};

/* ---------- globals */

static struct
{
	/* EXTRA ENEMIES, as the game began with it: its choice, and the amount
	of PER PLAYER (a percentage) and of STATIC MULTIPLIER */
	short mode;
	short percent;
	short multiplier;
	struct coop_riding_vehicle vehicles[MAXIMUM_RIDING_VEHICLES];
	short vehicle_count;
	struct coop_kept_rider riders[MAXIMUM_KEPT_RIDERS];
	short rider_count;
} coop_enemies;

/* One finite encounter, using existing campaign tags, navigation and co-op
replication. The actor creation and clear-all-wave progression are adapted from
SutlabSol's OpenCE-Faber e4ffe0fe9874870b4fca1fca76beb83514c793f4,
firefight_spawner.c:spawn_single_actor_at and firefight.c:firefight_update
(AGPL-3.0-or-later). No Faber ranks, menus or private network packet are needed. */
static struct
{
	boolean enabled;
	short phase;
	short wave;
	long score;
	long deadline;
	short lives;
	long wipe_deadline;
	long units[2];
	boolean killed[2];
	long variant;
	short team;
	real facing;
	boolean selection_valid;
	char actor_path[256];
	char spawn_encounter[TAG_STRING_LENGTH+1];
	real_point3d origin;
} firefight;

/* ---------- private code */

static boolean coop_enemies_host(
	void)
{
	return game_connection() == _game_connection_network_server && network_coop_active();
}

boolean coop_enemies_firefight_active(void)
{
	return firefight.enabled && firefight.phase != 0 && coop_enemies_host();
}

/* Faber's shared pool: deaths consume a life; at zero the living players
continue, but nobody respawns. The host owns both the pool and spawning. */
boolean coop_enemies_firefight_out_of_lives(void)
{
	return coop_enemies_firefight_active() && (firefight.lives == 0 || firefight.phase >= 4);
}

static void firefight_notice(char const *text)
{
	long machines[HALO_PORT_MAXIMUM_NETWORK_MACHINES];
	short count = distributed_client_machines(machines, NUMBEROF(machines));
	short index;
	long local;
	wchar_t wide[128];

	for (index = 0; text[index] && index < NUMBEROF(wide) - 1; index++)
		wide[index] = (wchar_t)(unsigned char)text[index];
	wide[index] = 0;
	for (local = local_player_get_next(NONE); local != NONE; local = local_player_get_next(local))
		hud_print_message((short)local, wide);
	for (index = 0; index < count; index++)
		distributed_send_notice_to_machine(machines[index], text);
	platform_log("firefight: %s", text);
}

void coop_enemies_firefight_player_died(long unit_index)
{
	struct unit_datum *unit;
	char text[128];

	if (!coop_enemies_firefight_active() || firefight.phase >= 4)
		return;
	unit = (struct unit_datum *)object_try_and_get_and_verify_type(unit_index, _object_mask_unit);
	/* player_died also handles live-unit replacement/deletion. Only an actual
	owned dead body consumes a life, before player_died clears its binding. */
	if (!unit || unit->unit.player_index == NONE || !TEST_FLAG(unit->object.damage_flags, _object_dead_bit))
		return;
	if (firefight.lives > 0)
		firefight.lives--;
	snprintf(text, sizeof(text), "Firefight: shared lives remaining %d", (int)firefight.lives);
	firefight_notice(text);
}

/* Faber's firefight_notify_actor_or_unit_killed records a native death rather
than waiting for a corpse. Keep this encounter's exact handles and one award;
destruction damage may delete the body before the next Firefight tick. */
void coop_enemies_firefight_enemy_died(long unit_index)
{
	struct unit_datum *unit;
	short index;

	if (!coop_enemies_firefight_active() || firefight.phase != 2)
		return;
	unit = (struct unit_datum *)object_try_and_get_and_verify_type(unit_index, _object_mask_biped);
	if (!unit || !TEST_FLAG(unit->object.damage_flags, _object_dead_bit) ||
		unit->unit.feign_death_timer != 0 || unit->unit.actor_index != NONE ||
		unit->unit.swarm_actor_index != NONE)
		return;
	for (index = 0; index < NUMBEROF(firefight.units); index++)
	{
		if (firefight.units[index] == unit_index && !firefight.killed[index])
		{
			firefight.killed[index] = TRUE;
			firefight.score += 100;
			platform_log("firefight: enemy died %08lx", (unsigned long)unit_index);
			break;
		}
	}
}

/* Adapted from Faber's firefight_count_living_players and wipe check: quit
players are not survivors, and an exhausted team must stay dead for 3 seconds. */
static boolean firefight_check_defeat(long now)
{
	struct data_iterator iterator;
	struct player_datum *player;
	short total = 0, alive = 0;

	data_iterator_new(&iterator, player_data);
	while ((player = (struct player_datum *)data_iterator_next(&iterator)) != NULL)
	{
		struct unit_datum *unit;

		if (player->quit_out_of_game)
			continue;
		total++;
		unit = (struct unit_datum *)object_try_and_get_and_verify_type(player->unit_index, _object_mask_unit);
		if (unit && !TEST_FLAG(unit->object.damage_flags, _object_dead_bit))
			alive++;
	}
	if (total > 0 && alive == 0 && firefight.lives == 0)
	{
		if (!firefight.wipe_deadline)
			firefight.wipe_deadline = now + 3 * TICKS_PER_SECOND;
		return now >= firefight.wipe_deadline;
	}
	firefight.wipe_deadline = 0;
	return FALSE;
}

/* Faber's discover_map_spawn_points reads these same native squad locations.
An explicit encounter supplies geometry only; it is not a JSON rules importer. */
static boolean firefight_authored_origin(real_point3d const *player_position)
{
	struct scenario *scenario = global_scenario_get();
	boolean found = FALSE;
	real nearest = 0.0f;
	long encounter_index, squad_index, location_index;

	for (encounter_index = 0; encounter_index < scenario->ai_encounters.count; encounter_index++)
	{
		struct encounter_definition *encounter = TAG_BLOCK_GET_ELEMENT(&scenario->ai_encounters,
			encounter_index, struct encounter_definition);

		if (strncmp(encounter->name, firefight.spawn_encounter, sizeof(encounter->name)))
			continue;
		if (encounter->manual_structure_bsp_reference_index != NONE &&
			encounter->manual_structure_bsp_reference_index != global_structure_bsp_index_get())
			continue;
		if (encounter->runtime_structure_bsp_reference_index != NONE &&
			encounter->runtime_structure_bsp_reference_index != global_structure_bsp_index_get())
			continue;
		for (squad_index = 0; squad_index < encounter->squads.count; squad_index++)
		{
			struct squad_definition *squad = TAG_BLOCK_GET_ELEMENT(&encounter->squads, squad_index, struct squad_definition);

			for (location_index = 0; location_index < squad->starting_locations.count; location_index++)
			{
				struct actor_starting_location *start = TAG_BLOCK_GET_ELEMENT(&squad->starting_locations,
					location_index, struct actor_starting_location);
				struct location location;
				real distance;

				if (!valid_real_point3d(&start->position) || !valid_real(start->facing) ||
					scenario_leaf_index_from_point(&start->position) == NONE)
					continue;
				scenario_location_from_point(&location, &start->position);
				if (location.cluster_index == NONE)
					continue;
				distance = distance_squared3d(player_position, &start->position);
				if (!found || distance < nearest)
				{
					found = TRUE;
					nearest = distance;
					firefight.origin = start->position;
					firefight.facing = start->facing;
				}
			}
		}
	}
	return found;
}

/* Default: an existing nearby Grunt supplies the tag and origin. Explicit
selection uses the map's loaded actor variant and optional native spawn encounter;
missing authored data never silently becomes a different stock enemy. */
static boolean firefight_prepare(void)
{
	struct data_iterator iterator;
	struct player_datum *player;
	struct actor_datum *actor;
	struct object_iterator enemies;
	real_point3d player_position;
	short players = 0;
	real nearest = 20.0f * 20.0f;
	boolean explicit_actor = firefight.actor_path[0] != 0;
	boolean found_origin = FALSE;

	if (cinematic_in_progress())
		return FALSE;
	data_iterator_new(&iterator, player_data);
	while ((player = (struct player_datum *)data_iterator_next(&iterator)) != NULL)
	{
		struct unit_datum *unit;

		if (player->quit_out_of_game)
			continue;
		unit = (struct unit_datum *)object_try_and_get_and_verify_type(player->unit_index, _object_mask_biped);
		if (!unit || TEST_FLAG(unit->object.damage_flags, _object_dead_bit) || unit->object.parent_object_index != NONE)
			return FALSE;
		if (!players)
			player_position = unit->object.position;
		players++;
	}
	if (players < 2)
		return FALSE;
	if (!firefight.selection_valid || (firefight.spawn_encounter[0] && !explicit_actor))
	{
		firefight.phase = 4;
		firefight.deadline = game_time_get() + 5 * TICKS_PER_SECOND;
		firefight_notice("Firefight: invalid actor/spawn selection; encounter aborted");
		return FALSE;
	}
	firefight.variant = NONE;
	if (explicit_actor)
	{
		struct actor_variant_definition *variant;

		firefight.variant = tag_loaded(ACTOR_VARIANT_DEFINITION_TAG, firefight.actor_path);
		if (firefight.variant == NONE)
		{
			firefight.phase = 4;
			firefight.deadline = game_time_get() + 5 * TICKS_PER_SECOND;
			firefight_notice("Firefight: configured actor is not loaded; encounter aborted");
			return FALSE;
		}
		variant = actor_variant_definition_get(firefight.variant);
		if (variant->actor_reference.index == NONE || variant->unit_reference.index == NONE)
		{
			firefight.phase = 4;
			firefight.deadline = game_time_get() + 5 * TICKS_PER_SECOND;
			firefight_notice("Firefight: configured actor has missing dependencies; encounter aborted");
			return FALSE;
		}
		firefight.team = unit_definition_get(variant->unit_reference.index)->unit.default_team;
		if (!VALID_INDEX(firefight.team, NUMBER_OF_SOLO_CAMPAIGN_TEAMS) || !game_team_is_enemy(_game_team_player, firefight.team))
		{
			firefight.phase = 4;
			firefight.deadline = game_time_get() + 5 * TICKS_PER_SECOND;
			firefight_notice("Firefight: configured actor is not hostile; encounter aborted");
			return FALSE;
		}
	}
	/* Swarm actors have no single meta.unit_index. Inspect each real biped
	and its native actor binding so infection forms keep per-body identity. */
	object_iterator_new(&enemies, _object_mask_biped, 0);
	while (object_iterator_next(&enemies))
	{
		struct unit_datum *unit = unit_get(enemies.index);
		struct actor_variant_definition *variant;
		long actor_index = unit->unit.actor_index != NONE ? unit->unit.actor_index : unit->unit.swarm_actor_index;
		real x, y, z, distance;

		if (unit->unit.player_index != NONE || actor_index == NONE || !game_team_is_enemy(_game_team_player, unit->object.owner_team_index) ||
			TEST_FLAG(unit->object.damage_flags, _object_dead_bit) || unit->object.parent_object_index != NONE)
			continue;
		actor = actor_try_and_get(actor_index);
		if (!actor || actor->meta.variant_definition_index == NONE)
			continue;
		variant = actor_variant_definition_get(actor->meta.variant_definition_index);
		x = unit->object.position.x - player_position.x;
		y = unit->object.position.y - player_position.y;
		z = unit->object.position.z - player_position.z;
		distance = x*x + y*y + z*z;
		if (variant->actor_reference.index != NONE && distance < nearest &&
			(explicit_actor ? actor->meta.variant_definition_index == firefight.variant :
			actor_definition_get(variant->actor_reference.index)->type == _actor_grunt))
		{
			found_origin = TRUE;
			nearest = distance;
			firefight.variant = actor->meta.variant_definition_index;
			firefight.team = unit->object.owner_team_index;
			firefight.facing = (real)atan2(unit->object.forward.j, unit->object.forward.i);
			firefight.origin = unit->object.position;
		}
	}
	if (firefight.spawn_encounter[0])
	{
		found_origin = firefight_authored_origin(&player_position);
		if (!found_origin)
		{
			firefight.phase = 4;
			firefight.deadline = game_time_get() + 5 * TICKS_PER_SECOND;
			firefight_notice("Firefight: no valid configured spawn in this BSP; encounter aborted");
			return FALSE;
		}
	}
	if (firefight.variant == NONE || !found_origin)
		return FALSE;
	if (!VALID_INDEX(firefight.team, NUMBER_OF_SOLO_CAMPAIGN_TEAMS))
		return FALSE;
	/* The campaign encounter gives way to the announced wave. These removals
	are setup, never credited as wave deaths or score. Walk the bodies again:
	a swarm may contain more bodies than the actor table has entries. */
	object_iterator_new(&enemies, _object_mask_biped, 0);
	while (object_iterator_next(&enemies))
	{
		struct unit_datum *unit = unit_get(enemies.index);
		long actor_index = unit->unit.actor_index != NONE ? unit->unit.actor_index : unit->unit.swarm_actor_index;

		if (unit->unit.player_index == NONE && actor_index != NONE && actor_try_and_get(actor_index) &&
			game_team_is_enemy(_game_team_player, unit->object.owner_team_index) &&
			!TEST_FLAG(unit->object.damage_flags, _object_dead_bit) && unit->object.parent_object_index == NONE)
			object_delete(enemies.index);
	}
	/* Reuse co-op's in-memory spawn positions for a team recovery. A full
	checkpoint revert would resurrect waves and restore spent lives. */
	players_note_checkpoint();
	return TRUE;
}

static long firefight_spawn(short number)
{
	struct actor_variant_definition *variant = actor_variant_definition_get(firefight.variant);
	struct object_placement_data placement;
	struct actor_datum *actor;
	real_point3d position;
	long unit_index, actor_index;
	boolean swarm;

	if (!coop_enemies_spread_position(&firefight.origin, number + 1, NULL, &position))
		return NONE;
	object_placement_data_new(&placement, variant->unit_reference.index, NONE);
	placement.position = position;
	vector3d_from_angle(&placement.forward, firefight.facing);
	placement.owner_team_index = firefight.team;
	unit_index = object_new(&placement);
	if (unit_index == NONE)
		return NONE;
	if (object_get(unit_index)->object.type != _object_type_biped)
	{
		object_delete(unit_index);
		return NONE;
	}
	if (!biped_fix_position(unit_index, NONE, &position, NULL, 1.0f, TRUE, FALSE, FALSE))
	{
		object_delete(unit_index);
		return NONE;
	}
	actor_customize_unit(firefight.variant, unit_index);
	/* Keep the native type/flag contract: Flood infection forms use real swarm
	attachment, while combat forms remain individual actors. Never rewrite tags. */
	swarm = TEST_FLAG(actor_definition_get(variant->actor_reference.index)->flags, _actor_definition_swarm_actor_bit);
	actor_index = actor_create_for_unit(swarm, unit_index, firefight.variant, NONE, NONE,
		FALSE, NONE, FALSE, 2, 0, NONE, 0);
	if (actor_index == NONE)
	{
		object_delete(unit_index);
		return NONE;
	}
	actor = actor_get(actor_index);
	actor->meta.force_active = TRUE;
	actor->meta.dormant_desire = FALSE;
	if (!actor->meta.active)
		actor_set_active(actor_index, TRUE);
	if (actor->meta.active && actor->meta.dormant)
		actor_set_dormant(actor_index, FALSE);
	actor_set_team(actor_index, firefight.team);
	object_set_automatic_deactivation(unit_index, FALSE);
	actor_verify_activation(actor_index);
	return unit_index;
}

/* Faber's direct_actor_toward_unit, limited to the player target needed by
this encounter. Use the actor's perception/path machinery, not unit teleports. */
static void firefight_pursue(long unit_index)
{
	struct unit_datum *unit = unit_get(unit_index);
	struct actor_datum *actor;
	struct data_iterator iterator;
	struct player_datum *player;
	long target = NONE, prop_index;
	long actor_index;
	real nearest = 0.0f;

	actor_index = unit->unit.actor_index != NONE ? unit->unit.actor_index : unit->unit.swarm_actor_index;
	if (actor_index == NONE || TEST_FLAG(unit->object.damage_flags, _object_dead_bit))
		return;
	actor = actor_get(actor_index);
	data_iterator_new(&iterator, player_data);
	while ((player = (struct player_datum *)data_iterator_next(&iterator)) != NULL)
	{
		struct unit_datum *candidate = (struct unit_datum *)object_try_and_get_and_verify_type(player->unit_index, _object_mask_biped);
		real distance;

		if (!candidate || TEST_FLAG(candidate->object.damage_flags, _object_dead_bit))
			continue;
		distance = distance_squared3d(&unit->object.position, &candidate->object.position);
		if (target == NONE || distance < nearest)
		{
			target = player->unit_index;
			nearest = distance;
		}
	}
	if (target == NONE || !actor->meta.active)
		return;
	actor->external_orders.defending = FALSE;
	actor->emotions.currently_defending = FALSE;
	actor->emotions.forced_to_charge = TRUE;
	actor->state.artificial_combat_status = MAX(actor->state.artificial_combat_status, 4);
	prop_index = prop_get_base_by_unit_index(actor_index, target, TRUE, TRUE);
	if (prop_index != NONE)
	{
		struct prop_datum *prop = prop_try_and_get(prop_index);

		if (prop && prop->state >= _prop_state_uninspected_orphan && prop->state <= _prop_state_inspected_orphan)
		{
			prop_index = prop->parent_prop_index;
			prop = prop_index != NONE ? prop_try_and_get(prop_index) : NULL;
		}
		if (prop && (actor->target.target_prop_index != prop_index || prop->state != _prop_state_acknowledged))
		{
			/* Faber's AI_UNIT_EFFECT_MAGIC_SIGHT. */
			actor_handle_unit_effect(actor_index, prop_index, 3);
			prop = prop_try_and_get(prop_index);
		}
		if (prop && prop->state >= _prop_state_becoming_unacknowledged && prop->state <= _prop_state_acknowledged)
		{
			prop->enemy = TRUE;
			prop->ally = FALSE;
			prop->preferred_target = TRUE;
			prop->ineffability = 3;
			actor->target.target_prop_index = prop_index;
			actor->target.target_type = _actor_target_acknowledged_enemy;
		}
	}
	if (!actor->meta.swarm && !actor->control.path.path.valid && nearest > 3.5f * 3.5f)
	{
		if (prop_index != NONE)
			actor_move_to_prop(actor_index, prop_index, 5.0f);
		else
			actor_move_to_point(actor_index, &object_get(target)->object.position, NONE, NONE);
	}
}

static void firefight_update(void)
{
	long now = game_time_get();
	short index, dead = 0;

	if (!firefight.enabled)
		return;
	if (!firefight.phase)
	{
		if (firefight_prepare())
		{
			firefight.phase = 1;
			firefight.wave = 1;
			firefight.deadline = now + 5 * TICKS_PER_SECOND;
			firefight_notice("Firefight: prepare for the wave");
			{
				char text[128];

				snprintf(text, sizeof(text), "Firefight: shared lives remaining %d", (int)firefight.lives);
				firefight_notice(text);
			}
		}
		return;
	}
	if (firefight.phase < 4 && firefight_check_defeat(now))
	{
		firefight.phase = 4;
		firefight.deadline = now + 5 * TICKS_PER_SECOND;
		firefight_notice("Firefight: defeat - team eliminated, no shared lives");
	}
	if (firefight.phase == 1 && now >= firefight.deadline)
	{
		for (index = 0; index < NUMBEROF(firefight.units); index++)
		{
			firefight.units[index] = firefight_spawn(index);
			if (firefight.units[index] == NONE)
			{
				firefight.phase = 4;
				firefight.deadline = now + 5 * TICKS_PER_SECOND;
				firefight_notice("Firefight: spawn failed; encounter aborted");
				return;
			}
			platform_log("firefight: spawned %08lx", (unsigned long)firefight.units[index]);
		}
		firefight.phase = 2;
		{
			char text[128];

			snprintf(text, sizeof(text), "Firefight: wave %d of 3 - eliminate both enemies", (int)firefight.wave);
			firefight_notice(text);
		}
	}
	if (firefight.phase == 2)
	{
		for (index = 0; index < NUMBEROF(firefight.units); index++)
		{
			struct unit_datum *unit = (struct unit_datum *)object_try_and_get_and_verify_type(firefight.units[index], _object_mask_biped);

			if (!firefight.killed[index] && !unit)
			{
				firefight.phase = 4;
				firefight.deadline = now + 5 * TICKS_PER_SECOND;
				firefight_notice("Firefight: enemy lost; encounter aborted");
				return;
			}
			if (!firefight.killed[index] && now % 15 == 0)
				firefight_pursue(firefight.units[index]);
			dead += firefight.killed[index] ? 1 : 0;
		}
		if (dead == NUMBEROF(firefight.units))
		{
			firefight.phase = 3;
			firefight.deadline = now + 3 * TICKS_PER_SECOND;
			{
				char text[128];

				snprintf(text, sizeof(text), "Firefight: wave %d cleared - team score %ld", (int)firefight.wave, firefight.score);
				firefight_notice(text);
			}
		}
	}
	if (firefight.phase == 3 && now >= firefight.deadline)
	{
		if (firefight.wave < 3)
		{
			firefight.wave++;
			csmemset(firefight.killed, 0, sizeof(firefight.killed));
			firefight.phase = 1;
			firefight.deadline = now + 5 * TICKS_PER_SECOND;
			firefight_notice("Firefight: reinforcements incoming");
		}
		else
		{
			firefight.phase = 4;
			firefight.deadline = now + 5 * TICKS_PER_SECOND;
			{
				char text[128];

				snprintf(text, sizeof(text), "Firefight: victory - three waves cleared, team score %ld", firefight.score);
				firefight_notice(text);
			}
		}
	}
	if (firefight.phase == 4 && now >= firefight.deadline)
	{
		firefight.phase = 5;
		/* The main loop owns the world/lobby transition. */
		main_won_map();
	}
}

/* whether this game has extra enemies (it is a co-op host's, with some
chosen) */
static boolean coop_enemies_on(
	void)
{
	return coop_enemies.mode != _coop_enemies_none && coop_enemies_host();
}

/* the players in the game, every machine's: the network game's (a level
places its first enemies before the players' units are made, as it loads:
game_initialize_for_new_map) */
static short coop_enemies_player_count(
	void)
{
	struct network_game *game = network_game_get_game();
	short count = 0;
	short index;

	for (index = 0; game && index < HALO_PORT_MAXIMUM_NETWORK_PLAYERS; index++)
	{
		if (network_player_is_valid(&game->players[index]))
			count++;
	}
	return count;
}

/* what a spot is tested against: the level, and the objects that block
the way (crates and other scenery, machines, vehicles) */
#define SPREAD_COLLISION_FLAGS (FLAG(_collision_test_structure_bit) | FLAG(_collision_test_objects_bit) | \
	_collision_test_objects_sight_blocking_flags | FLAG(_collision_test_front_facing_surfaces_bit) | \
	FLAG(_collision_test_back_facing_surfaces_bit))

/* whether anything is in the way from `from`, along `vector` */
static boolean coop_enemies_blocked(
	real_point3d const *from,
	real_vector3d const *vector)
{
	struct collision_result collision;

	return collision_test_vector(SPREAD_COLLISION_FLAGS, from, vector, NONE, &collision);
}

/* the floor under a point, looked for from SPREAD_STEP_HEIGHT above it to
SPREAD_FLOOR_DIFFERENCE below it */
static boolean coop_enemies_floor(
	real_point3d const *point,
	real_point3d *floor)
{
	struct collision_result collision;
	real_point3d from = *point;
	real_vector3d down = { 0.0f, 0.0f, -(2.0f * SPREAD_STEP_HEIGHT + SPREAD_FLOOR_DIFFERENCE) };

	from.z += SPREAD_STEP_HEIGHT;
	if (!collision_test_vector(SPREAD_COLLISION_FLAGS, &from, &down, NONE, &collision))
		return FALSE;
	*floor = collision.point;
	return TRUE;
}

/* whether a biped stands within SPREAD_CLEARANCE of a spot on the floor,
or the spot is inside a vehicle */
static boolean coop_enemies_occupied(
	real_point3d const *floor)
{
	real_point3d body = { floor->x, floor->y, floor->z + SPREAD_STEP_HEIGHT };
	struct location location;
	long object_indices[32];
	short object_count;
	short index;

	scenario_location_from_point(&location, &body);
	if (location.cluster_index == NONE)
		return TRUE;
	object_count = objects_in_sphere(0, _object_mask_biped | _object_mask_vehicle, &location, &body,
		SPREAD_SEARCH_RADIUS, object_indices, NUMBEROF(object_indices));
	for (index = 0; index < object_count; index++)
	{
		struct object_datum *object = object_get(object_indices[index]);

		if (object->object.type == _object_type_vehicle)
		{
			if (point_in_sphere(&body, &object->object.bounding_sphere_center, object->object.bounding_sphere_radius))
				return TRUE;
		}
		else
		{
			real dx = object->object.position.x - floor->x;
			real dy = object->object.position.y - floor->y;

			if (dx * dx + dy * dy < SPREAD_CLEARANCE * SPREAD_CLEARANCE)
				return TRUE;
		}
	}
	return FALSE;
}

/* Whether an enemy can stand at `spot`: reachable from `from` (beside the
starting location) with nothing in the way, a floor within a step of
`floor_height` (the starting location's), room above it, and nobody there.
The floor goes in `position`. */
static boolean coop_enemies_spot_free(
	real_point3d const *from,
	real_point3d const *spot,
	real floor_height,
	real_point3d *position)
{
	/* (level, at step height: down to the starting location's floor, a spot
	whose floor is a little higher would end the look inside it) */
	real_vector3d way = { spot->x - from->x, spot->y - from->y, 0.0f };
	real_vector3d up = { 0.0f, 0.0f, SPREAD_HEADROOM };
	real_point3d floor;
	real_point3d feet;

	if (coop_enemies_blocked(from, &way) || !coop_enemies_floor(spot, &floor) ||
		fabsf(floor.z - floor_height) > SPREAD_FLOOR_DIFFERENCE)
	{
		return FALSE;
	}
	/* (headroom is checked from just above the floor, so the floor itself
	doesn't count as in the way) */
	feet = floor;
	feet.z += 0.1f;
	if (coop_enemies_blocked(&feet, &up) || coop_enemies_occupied(&floor))
		return FALSE;
	*position = floor;
	return TRUE;
}

static struct coop_riding_vehicle *coop_enemies_riding_vehicle(
	long vehicle_index,
	boolean create)
{
	short index;

	for (index = 0; index < coop_enemies.vehicle_count; index++)
	{
		if (coop_enemies.vehicles[index].vehicle_index == vehicle_index)
			return &coop_enemies.vehicles[index];
	}
	if (!create || coop_enemies.vehicle_count >= MAXIMUM_RIDING_VEHICLES)
		return NULL;
	csmemset(&coop_enemies.vehicles[coop_enemies.vehicle_count], 0, sizeof(coop_enemies.vehicles[0]));
	coop_enemies.vehicles[coop_enemies.vehicle_count].vehicle_index = vehicle_index;
	return &coop_enemies.vehicles[coop_enemies.vehicle_count++];
}

/* the riders kept for a vehicle forgotten, and the vehicle with them */
static void coop_enemies_forget_vehicle(
	short vehicle_number)
{
	long vehicle_index = coop_enemies.vehicles[vehicle_number].vehicle_index;
	short index, kept = 0;

	for (index = 0; index < coop_enemies.rider_count; index++)
	{
		if (coop_enemies.riders[index].vehicle_index != vehicle_index)
			coop_enemies.riders[kept++] = coop_enemies.riders[index];
	}
	coop_enemies.rider_count = kept;
	coop_enemies.vehicles[vehicle_number] = coop_enemies.vehicles[--coop_enemies.vehicle_count];
}

/* whether a vehicle's seated riders have begun to get out: one alive and
out of it (one killed in its seat is not getting out) */
static boolean coop_enemies_riders_getting_out(
	struct coop_riding_vehicle const *vehicle)
{
	short index;

	for (index = 0; index < vehicle->seated_count; index++)
	{
		struct object_datum *unit = object_try_and_get_and_verify_type(vehicle->seated_unit_indices[index],
			_object_mask_unit);

		if (unit && !TEST_FLAG(unit->object.damage_flags, _object_dead_bit) &&
			unit->object.parent_object_index != vehicle->vehicle_index)
		{
			return TRUE;
		}
	}
	return FALSE;
}

/* where a vehicle's `number`th kept rider is placed: beside one of the
riders who got out of it (where it is now: out of the vehicle, in the
open), a ring further about them each round; else at a seat's marker, else
the vehicle's place */
static void coop_enemies_rider_place(
	struct coop_riding_vehicle const *vehicle,
	short number,
	real_point3d *position,
	real *facing)
{
	struct object_datum *object = object_get(vehicle->vehicle_index);
	struct unit_definition *definition = unit_definition_get(object->definition_index);
	long out_unit_indices[MAXIMUM_SEATED_RIDERS];
	short out_count = 0;
	short index, round;
	real angle;
	real_point3d spread;

	*position = object->object.position;
	*facing = (real)atan2(object->object.forward.j, object->object.forward.i);
	for (index = 0; index < vehicle->seated_count; index++)
	{
		struct object_datum *unit = object_try_and_get_and_verify_type(vehicle->seated_unit_indices[index],
			_object_mask_unit);

		if (unit && !TEST_FLAG(unit->object.damage_flags, _object_dead_bit) &&
			unit->object.parent_object_index != vehicle->vehicle_index)
		{
			out_unit_indices[out_count++] = vehicle->seated_unit_indices[index];
		}
	}
	if (out_count > 0)
	{
		*position = object_get(out_unit_indices[number % out_count])->object.position;
		round = (short)(number / out_count);
	}
	else
	{
		struct object_marker marker;

		round = number;
		if (vehicle->seated_count > 0 && VALID_INDEX(vehicle->seat_indices[0], definition->unit.seats.count))
		{
			struct unit_seat *seat = TAG_BLOCK_GET_ELEMENT(&definition->unit.seats, vehicle->seat_indices[0],
				struct unit_seat);

			if (seat->marker_name[0] &&
				object_get_marker_by_name(vehicle->vehicle_index, seat->marker_name, &marker, 1) > 0)
			{
				*position = marker.matrix.position;
			}
		}
	}
	/* free ground around that spot, as the squad's own extra enemies get;
	failing that, a ring around it, further out each round */
	if (coop_enemies_spread_position(position, (short)(number + 1), NULL, &spread))
	{
		*position = spread;
		return;
	}
	angle = (real)number * (_pi * 2.0f / SPREAD_PLACES_PER_RING);
	position->x += (real)cos(angle) * RIDER_SPACING * (real)(round / SPREAD_PLACES_PER_RING + 1);
	position->y += (real)sin(angle) * RIDER_SPACING * (real)(round / SPREAD_PLACES_PER_RING + 1);
}

/* ---------- public code */

void coop_enemies_new_game(
	void)
{
	char const *mode = config_string("network.coop_enemies_mode");
	char const *actor = config_string("network.firefight_actor");
	char const *spawn = config_string("network.firefight_spawn_encounter");

	csmemset(&coop_enemies, 0, sizeof(coop_enemies));
	csmemset(&firefight, 0, sizeof(firefight));
	firefight.enabled = config_boolean("network.firefight_encounter") != 0;
	firefight.lives = (short)PIN(config_integer("network.firefight_lives"), 0, 99);
	firefight.selection_valid = strlen(actor) < sizeof(firefight.actor_path) && strlen(spawn) < sizeof(firefight.spawn_encounter);
	if (firefight.selection_valid)
	{
		strcpy(firefight.actor_path, actor);
		strcpy(firefight.spawn_encounter, spawn);
	}
	/* (an amount past Server Setup's, as it shows it: its nearest choice's
	end) */
	coop_enemies.percent = (short)PIN(config_integer("network.coop_enemies"), COOP_ENEMIES_MINIMUM_PERCENT,
		COOP_ENEMIES_MAXIMUM_PERCENT);
	coop_enemies.multiplier = (short)PIN(config_integer("network.coop_enemies_multiplier"),
		COOP_ENEMIES_MINIMUM_MULTIPLIER, COOP_ENEMIES_MAXIMUM_MULTIPLIER);
	coop_enemies.mode = !strcmp(mode, "per_player") ? _coop_enemies_per_player :
		!strcmp(mode, "multiplier") ? _coop_enemies_multiplier : _coop_enemies_none;
}

void coop_enemies_reset(
	void)
{
	coop_enemies.vehicle_count = 0;
	coop_enemies.rider_count = 0;
	/* A checkpoint restored another set of object handles. */
	firefight.phase = 0;
	firefight.wipe_deadline = 0;
	csmemset(firefight.killed, 0, sizeof(firefight.killed));
}

short coop_enemies_extra_count(
	long encounter_index,
	short count)
{
	struct encounter_datum *encounter;
	long extra, room;
	short players;

	if (count <= 0 || !coop_enemies_on())
		return 0;
	encounter = encounter_get(encounter_index);
	if (!game_team_is_enemy(_game_team_player, encounter->team_index))
		return 0;
	if (coop_enemies.mode == _coop_enemies_multiplier)
		extra = (long)count * (coop_enemies.multiplier - 1);
	else
	{
		players = coop_enemies_player_count();
		if (players <= 1)
			return 0;
		extra = ((long)count * coop_enemies.percent * (players - 1) + 50) / 100;
		extra = MIN(extra, (long)count * (COOP_ENEMIES_MAXIMUM_GROWTH - 1));
	}
	room = MAXIMUM_ACTORS - COOP_ENEMIES_LEVEL_ACTORS - actor_data->actual_count;
	return (short)PIN(extra, 0, MAX(room, 0));
}

boolean coop_enemies_spread_position(
	real_point3d const *origin,
	short number,
	long *taken,
	real_point3d *position)
{
	real_point3d from = *origin;
	real_point3d floor;
	real floor_height = coop_enemies_floor(origin, &floor) ? floor.z : origin->z;
	short ring, first_spot = 0;

	from.z = floor_height + SPREAD_STEP_HEIGHT;
	for (ring = 0; ring < SPREAD_RINGS; ring++)
	{
		short place_count = (short)(SPREAD_PLACES_PER_RING * (ring + 1));
		real radius = SPREAD_SPACING * (real)(ring + 1);
		short place;

		/* (each enemy starts at a different place on the ring, so a squad
		spreads all the way round rather than filling one side first) */
		for (place = 0; place < place_count; place++)
		{
			short around = (short)((number + place) % place_count);
			short spot_index = (short)(first_spot + around);
			real angle = (real)around * (_pi * 2.0f / (real)place_count);
			real_point3d spot = from;

			if (taken && BIT_VECTOR_TEST_FLAG(taken, spot_index))
				continue;
			spot.x += (real)cos(angle) * radius;
			spot.y += (real)sin(angle) * radius;
			spot.z = floor_height;
			if (coop_enemies_spot_free(&from, &spot, floor_height, position))
				return TRUE;
			if (taken)
				BIT_VECTOR_SET_FLAG(taken, spot_index, TRUE);
		}
		first_spot = (short)(first_spot + place_count);
	}
	return FALSE;
}

boolean coop_enemies_fallback_position(
	real_point3d const *origin,
	short number,
	real_point3d *position)
{
	real_point3d from = *origin;
	short place = (short)((number - 1) % SPREAD_PLACES_PER_RING);
	short ring = (short)((number - 1) / SPREAD_PLACES_PER_RING);
	short attempt;

	from.z += SPREAD_STEP_HEIGHT;
	for (attempt = 0; attempt < FALLBACK_RINGS; attempt++, ring++)
	{
		/* (each ring turned half a place from the last, so its places fall
		between the last's) */
		real angle = ((real)place + 0.5f * (real)(ring % 2)) * (_pi * 2.0f / SPREAD_PLACES_PER_RING);
		real radius = SPREAD_SPACING * (real)(ring + 1);
		real_point3d to = from;
		real_vector3d way = { 0.0f, 0.0f, 0.0f };
		real_vector3d down = { 0.0f, 0.0f, -(SPREAD_STEP_HEIGHT + FALLBACK_GROUND_DEPTH) };
		struct collision_result collision;

		way.i = (real)cos(angle) * radius;
		way.j = (real)sin(angle) * radius;
		to.x += way.i;
		to.y += way.j;
		/* (the level only: the way open, and ground below) */
		if (!collision_test_vector(FLAG(_collision_test_structure_bit) | FLAG(_collision_test_front_facing_surfaces_bit) |
				FLAG(_collision_test_back_facing_surfaces_bit), &from, &way, NONE, &collision) &&
			collision_test_vector(FLAG(_collision_test_structure_bit) | FLAG(_collision_test_front_facing_surfaces_bit),
				&to, &down, NONE, &collision))
		{
			*position = collision.point;
			return TRUE;
		}
	}
	return FALSE;
}

void coop_enemies_rider_seated(
	long vehicle_index,
	long unit_index)
{
	struct coop_riding_vehicle *vehicle;
	struct unit_datum *unit;

	if (!coop_enemies_on())
		return;
	vehicle = coop_enemies_riding_vehicle(vehicle_index, TRUE);
	unit = unit_get(unit_index);
	if (!vehicle)
		return;
	/* (a vehicle loaded again: the riders that got out of it before are not
	this load's) */
	{
		short index, kept = 0;

		for (index = 0; index < vehicle->seated_count; index++)
		{
			struct object_datum *seated = object_try_and_get_and_verify_type(vehicle->seated_unit_indices[index],
				_object_mask_unit);

			if (seated && seated->object.parent_object_index == vehicle_index)
			{
				vehicle->seated_unit_indices[kept] = vehicle->seated_unit_indices[index];
				vehicle->seat_indices[kept++] = vehicle->seat_indices[index];
			}
		}
		vehicle->seated_count = kept;
	}
	if (vehicle->seated_count >= MAXIMUM_SEATED_RIDERS)
		return;
	vehicle->seated_unit_indices[vehicle->seated_count] = unit_index;
	vehicle->seat_indices[vehicle->seated_count] = unit->unit.parent_seat_index;
	vehicle->seated_count++;
	/* (a vehicle loaded again, after its riders got out: they get out again) */
	vehicle->releasing = FALSE;
}

boolean coop_enemies_rider_unseated(
	long vehicle_index,
	long unit_index)
{
	struct unit_datum *unit;
	struct actor_datum *actor;
	struct coop_kept_rider *rider;
	struct coop_riding_vehicle *vehicle;

	if (!coop_enemies_on() || coop_enemies.rider_count >= MAXIMUM_KEPT_RIDERS)
		return FALSE;
	unit = unit_get(unit_index);
	vehicle = coop_enemies_riding_vehicle(vehicle_index, FALSE);
	/* (kept only for a vehicle with riders seated, whose getting out places
	it: one of a load that seated nobody is left where it is) */
	if (unit->object.type != _object_type_biped || unit->unit.actor_index == NONE || !vehicle ||
		vehicle->seated_count == 0)
	{
		return FALSE;
	}
	actor = actor_get(unit->unit.actor_index);
	if (actor->meta.swarm || actor->meta.encounter_index == NONE ||
		!game_team_is_enemy(_game_team_player, encounter_get(actor->meta.encounter_index)->team_index))
	{
		return FALSE;
	}
	rider = &coop_enemies.riders[coop_enemies.rider_count++];
	rider->vehicle_index = vehicle_index;
	rider->variant_definition_index = actor->meta.variant_definition_index;
	rider->encounter_index = actor->meta.encounter_index;
	rider->squad_index = actor->meta.squad_index;
	return TRUE;
}

void coop_enemies_update(
	void)
{
	long now = game_time_get();
	short vehicle_number;

	if (!coop_enemies_host())
		return;
	firefight_update();
	for (vehicle_number = coop_enemies.vehicle_count - 1; vehicle_number >= 0; vehicle_number--)
	{
		struct coop_riding_vehicle *vehicle = &coop_enemies.vehicles[vehicle_number];
		struct object_datum *object = object_try_and_get_and_verify_type(vehicle->vehicle_index, _object_mask_unit);
		short index;

		/* (the vehicle's next kept rider) */
		for (index = 0; index < coop_enemies.rider_count; index++)
		{
			if (coop_enemies.riders[index].vehicle_index == vehicle->vehicle_index)
				break;
		}
		/* (a vehicle gone, or destroyed, takes its kept riders with it; one
		with none kept, every one loaded seated or all placed, is done with) */
		if (!object || TEST_FLAG(object->object.damage_flags, _object_dead_bit) || index >= coop_enemies.rider_count)
		{
			coop_enemies_forget_vehicle(vehicle_number);
			continue;
		}
		if (!vehicle->releasing)
		{
			if (!coop_enemies_riders_getting_out(vehicle))
				continue;
			vehicle->releasing = TRUE;
			vehicle->released_count = 0;
			vehicle->next_release_time = now;
		}
		if (now < vehicle->next_release_time)
			continue;
		{
			struct coop_kept_rider rider = coop_enemies.riders[index];
			struct encounter_definition *encounter_definition;
			struct squad_definition *squad_definition;
			struct actor_starting_location location;

			coop_enemies.riders[index] = coop_enemies.riders[--coop_enemies.rider_count];
			encounter_definition = TAG_BLOCK_GET_ELEMENT(&global_scenario_get()->ai_encounters,
				DATUM_INDEX_TO_ABSOLUTE_INDEX(rider.encounter_index), struct encounter_definition);
			squad_definition = TAG_BLOCK_GET_ELEMENT(&encounter_definition->squads, rider.squad_index,
				struct squad_definition);
			/* (as the squad's: its states, no command list of a starting
			location's) */
			csmemset(&location, 0, sizeof(location));
			coop_enemies_rider_place(vehicle, vehicle->released_count++, &location.position, &location.facing);
			location.cluster_index = NONE;
			location.noncombat_sequence_id = NONE;
			location.default_state = squad_definition->default_state;
			location.initial_state = squad_definition->initial_state;
			location.actor_variant_index = NONE;
			location.command_list_index = NONE;
			actor_place(rider.variant_definition_index, rider.encounter_index, rider.squad_index, &location, FALSE, 0);
			encounter_update_status(rider.encounter_index);
		}
		vehicle->next_release_time = now + RIDER_RELEASE_TICKS;
	}
}
