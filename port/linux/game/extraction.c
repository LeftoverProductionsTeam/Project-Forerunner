/* Native timerless Extraction and finite-Survival gear, using native controls,
 * co-op authority and the shared record. No client claims or second director.
 * Native/hardware gameplay acceptance remains separate from source presence.
 * SPDX-License-Identifier: AGPL-3.0-or-later */
#include "cseries.h"
#include "devices/device_controls.h"
#include "devices/devices.h"
#include "game/game.h"
#include "game/players.h"
#include "networking/network_game_globals.h"
#include "game/game_allegiance.h"
#include "cutscene/cinematics.h"
#include "game_modes.h"
#include "gear.h"
#include "interface/hud_messaging.h"
#include "interface/player_ui.h"
#include "objects/objects.h"
#include "objects/damage.h"
#include "physics/collisions.h"
#include "scenario/scenario.h"
#include "scenario/scenario_definitions.h"
#include "tag_files/files.h"
#include "tag_files/tag_groups.h"
#include "units/units.h"
#include "extraction.h"
#include "network_coop.h"
#include "network_distributed.h"
#include "network_rules.h"
#include "coop_enemies.h"
#include "../include/halo_port_limits.h"
#include "../src/port_config.h"
#ifdef HALO_GAME_BROWSER
#include "../src/delta_peer.h"
#endif
#include <string.h>
#include <stdio.h>

/* Existing native notice/ownership interfaces; no new packet or wire state. */
short distributed_client_machines(long *machines, short maximum);
long distributed_player_machine(short player_index);
void distributed_send_notice_to_machine(long machine_index, char const *text);
void platform_log(char const *format, ...);

enum extraction_phase
{
	_extraction_hub, _extraction_deployed, _extraction_activating,
	_extraction_ready, _extraction_settling, _extraction_returned,
	_extraction_losing, _extraction_cancelling, _extraction_lost
};

struct extraction_run
{
	long player, unit, activation_ticks, retry_tick;
	short local_player;
	enum extraction_phase phase;
	boolean blocked, loadout_seen;
	byte owner[GEAR_ID_BYTES];
	byte discard_id[GEAR_ID_BYTES];
	unsigned long discard_generation;
	long discard_unit, discard_after, discard_deadline;
	struct gear_session gear;
};

static struct
{
	boolean bound, admitted, suspended, suspension_announced;
	boolean survival;
	short deploy_name, activate_name, loadout_name, zone, return_zone;
	long last_tick;
} extraction;
static struct extraction_run extraction_runs[HALO_PORT_MAXIMUM_NETWORK_PLAYERS];

/* Medical state is transient host authority, not a second save or inventory.
 * A native death is never cleared. New bodies/full player handles get new state. */
static struct
{
	long player, unit, deadline, last_damage, healer, healer_unit, progress, last_control, received_tick;
	unsigned int epoch;
	boolean downed, used;
} medical_players[HALO_PORT_MAXIMUM_NETWORK_PLAYERS];
static boolean medical_enabled, medical_received, medical_restored;
static long medical_tick = NONE, medical_seconds = 5, medical_bleed_seconds = 30;
static void extraction_medical_reset(void);
static void extraction_medical_update(void);

static boolean extraction_requested(void)
{
	return (config_boolean("game.extraction") || coop_enemies_firefight_gear_active()) && global_scenario &&
		global_scenario->type == _scenario_type_solo &&
		(game_connection() == _game_connection_local ||
		(game_connection() == _game_connection_network_server && network_coop_active()));
}

boolean extraction_owns_map(void)
{
	return config_boolean("game.extraction") && !extraction.survival && extraction_requested() && extraction.bound && extraction.admitted;
}

static void extraction_local_notice(short local, char const *text)
{
	wchar_t wide[128];
	short index;
	for (index = 0; text[index] && index < NUMBEROF(wide)-1; index++) wide[index] = (wchar_t)(unsigned char)text[index];
	wide[index] = 0;
	hud_print_message(local, wide);
}

static void extraction_notice(char const *text)
{
	long machines[HALO_PORT_MAXIMUM_NETWORK_MACHINES];
	short local, index, count;
	for (local = local_player_get_next(NONE); local != NONE; local = local_player_get_next(local))
		extraction_local_notice((short)local, text);
	if (game_connection() == _game_connection_network_server)
	{
		count = distributed_client_machines(machines, NUMBEROF(machines));
		for (index = 0; index < count; index++) distributed_send_notice_to_machine(machines[index], text);
	}
}

static void extraction_player_notice(long player_index, char const *text)
{
	struct player_datum *player = player_try_and_get(player_index);
	long machine;
	char notice[128];
	if (!player || player->quit_out_of_game) return;
	if (player->local_player_index >= 0 && player->local_player_index < MAXIMUM_NUMBER_OF_LOCAL_PLAYERS)
		extraction_local_notice(player->local_player_index, text);
	else
	{
		machine = distributed_player_machine((short)DATUM_INDEX_TO_ABSOLUTE_INDEX(player_index));
		if (machine != NONE)
		{
			snprintf(notice, sizeof(notice), "P%d | %s", (int)player->network_player_data.controller_index + 1, text);
			distributed_send_notice_to_machine(machine, notice);
		}
	}
}

static void extraction_tell(struct extraction_run const *run, char const *text)
{
	extraction_player_notice(run->player, text);
}

static void extraction_block(struct extraction_run *run, char const *text)
{
	if (!run->blocked) extraction_tell(run, text);
	run->blocked = TRUE;
}

static void extraction_run_close(struct extraction_run *run)
{
	if (run->gear.opened)
	{
		/* A committed escape was already consumed from the world. All other
		 * unfinished departures lose the deployed reservation, not the stash. */
		gear_lose(&run->gear);
		gear_close(&run->gear);
	}
	memset(run, 0, sizeof(*run));
	run->player = run->unit = NONE;
	run->local_player = NONE;
	run->gear.descriptor = -1;
}

void extraction_reset(void)
{
	short index;
	for (index = 0; index < NUMBEROF(extraction_runs); index++) extraction_run_close(&extraction_runs[index]);
	memset(&extraction, 0, sizeof(extraction));
	extraction.deploy_name = extraction.activate_name = extraction.loadout_name = extraction.zone = extraction.return_zone = NONE;
	extraction.last_tick = NONE;
	gear_world_reset();
	extraction_medical_reset();
}

void extraction_restored(void)
{
	extraction_reset();
	/* Saved world images can resurrect already recovered pickups. They may
	 * never produce another bank transaction; a fresh map starts a new world. */
	extraction.suspended = TRUE;
	medical_restored = medical_enabled;
}

static boolean extraction_owner(struct player_datum const *player, long player_index, byte owner[GEAR_ID_BYTES])
{
	if (player->local_player_index >= 0 && player->local_player_index < MAXIMUM_NUMBER_OF_LOCAL_PLAYERS)
	{
		char directory[MAXIMUM_FILENAME_LENGTH + 1];
		if (player_ui_get_active_player_profile_index(player->local_player_index) == NONE ||
			!player_ui_get_path_to_local_player_profile_directory(player->local_player_index, directory) || !directory[0]) return FALSE;
		gear_profile_owner(directory, owner);
		return TRUE;
	}
#ifdef HALO_GAME_BROWSER
	if (game_connection() == _game_connection_network_server)
	{
		struct delta_peer_identity identity;
		long machine = distributed_player_machine((short)DATUM_INDEX_TO_ABSOLUTE_INDEX(player_index));
		if (machine == NONE || !delta_peer_game_player_identity((int)machine,
			player->network_player_data.controller_index, &identity) || !identity.bound) return FALSE;
		gear_account_owner(identity.account, owner);
		return TRUE;
	}
#endif
	return FALSE;
}

static boolean extraction_open_gear(struct extraction_run *run, struct player_datum const *player)
{
	byte owner[GEAR_ID_BYTES];
	if (!extraction_owner(player, run->player, owner))
	{
		extraction_block(run, "DEPLOYMENT BLOCKED | Select a saved profile locally, or verify your multiplayer identity.");
		return FALSE;
	}
	if (memcmp(run->owner, owner, GEAR_ID_BYTES)) run->discard_deadline = 0;
	if (run->gear.opened && memcmp(run->owner, owner, GEAR_ID_BYTES))
	{
		gear_lose(&run->gear); gear_close(&run->gear);
	}
	if (!run->gear.opened && !gear_open(&run->gear, owner))
	{
		extraction_block(run, "STASH UNAVAILABLE | Gear is locked, unreadable or awaiting recovery. Retry at the terminal.");
		return FALSE;
	}
	memcpy(run->owner, owner, GEAR_ID_BYTES);
	run->blocked = FALSE;
	return TRUE;
}

static void extraction_lose_run(struct extraction_run *run)
{
	run->phase = _extraction_losing;
	run->activation_ticks = 0;
	run->retry_tick = game_time_get();
	if (gear_lose(&run->gear))
	{
		run->phase = _extraction_lost;
		run->blocked = FALSE;
		extraction_tell(run, extraction.survival ?
			"RUN LOST | Deployed gear lost. Remaining stash is safe; respawn follows the encounter's shared lives." :
			"RUN LOST | Deployed gear lost. Your remaining stash is safe. Return to the hub to redeploy.");
	}
	else extraction_block(run, "LOSS SETTLEMENT PENDING | Redeployment waits for the stash write to complete.");
}

boolean extraction_respawn_after_loss(void)
{
	short index;
	if (!extraction_owns_map() || !players_are_all_dead()) return FALSE;
	for (index = 0; index < NUMBEROF(extraction_runs); index++)
		if (extraction_runs[index].phase == _extraction_deployed || extraction_runs[index].phase == _extraction_activating ||
			extraction_runs[index].phase == _extraction_ready) extraction_lose_run(&extraction_runs[index]);
	players_respawn_at_start();
	return TRUE;
}

static void extraction_bind(void)
{
	short index;
	boolean unique = TRUE;
	struct control_datum *deploy, *activate, *loadout;

	extraction.bound = TRUE;
	extraction.admitted = FALSE;
	extraction.deploy_name = extraction.activate_name = extraction.loadout_name = NONE;
	extraction.zone = extraction.return_zone = NONE;
	if (extraction.survival)
	{
		/* The already active director checked authored-script ownership and
		 * owns its actual success/lives. It does not need an Extraction hub. */
		extraction.admitted = TRUE;
		extraction_notice("SURVIVAL GEAR | Compatible saved gear is at risk. Complete the finite encounter alive to recover loot.");
		return;
	}
	if (config_boolean("network.firefight_encounter"))
	{
		extraction_notice("Extraction unavailable: disable the built-in Firefight director. Authored native AI scripts can keep running.");
		return;
	}
	/* These bindings use native short indices. Refuse ambiguity instead of
	   owning a campaign whose terminals/volumes cannot identify one action. */
	if (global_scenario->object_names.count > SHORT_MAX || global_scenario->trigger_volumes.count > SHORT_MAX)
		goto finished;
	for (index = 0; index < global_scenario->object_names.count; index++)
	{
		struct scenario_object_name *name = TAG_BLOCK_GET_ELEMENT(
			&global_scenario->object_names, index, struct scenario_object_name);

		if (!strcmp(name->name, "extraction_deploy"))
		{
			if (extraction.deploy_name != NONE)
				unique = FALSE;
			extraction.deploy_name = index;
		}
		else if (!strcmp(name->name, "extraction_loadout"))
		{
			if (extraction.loadout_name != NONE) unique = FALSE;
			extraction.loadout_name = index;
		}
		else if (!strcmp(name->name, "extraction_activate"))
		{
			if (extraction.activate_name != NONE)
				unique = FALSE;
			extraction.activate_name = index;
		}
	}
	for (index = 0; index < global_scenario->trigger_volumes.count; index++)
	{
		struct scenario_trigger_volume *volume = TAG_BLOCK_GET_ELEMENT(
			&global_scenario->trigger_volumes, index, struct scenario_trigger_volume);

		if (!strcmp(volume->name, "extraction_zone"))
		{
			if (extraction.zone != NONE)
				unique = FALSE;
			extraction.zone = index;
		}
		else if (!strcmp(volume->name, "extraction_return"))
		{
			if (extraction.return_zone != NONE)
				unique = FALSE;
			extraction.return_zone = index;
		}
	}
	deploy = extraction.deploy_name != NONE ? control_try_and_get(object_index_from_name_index(extraction.deploy_name)) : NULL;
	activate = extraction.activate_name != NONE ? control_try_and_get(object_index_from_name_index(extraction.activate_name)) : NULL;
	loadout = extraction.loadout_name != NONE ? control_try_and_get(object_index_from_name_index(extraction.loadout_name)) : NULL;
	/* Match Spark's native touched-by-player tag requirement. Other trigger
	   kinds never reach extraction_control_touched through control_touched. */
	extraction.admitted = unique && extraction.zone != NONE && extraction.return_zone != NONE &&
		deploy && activate && deploy != activate &&
		control_definition_get(deploy->definition_index)->control.triggers_when == 0 &&
		control_definition_get(activate->definition_index)->control.triggers_when == 0;
	if (extraction.loadout_name != NONE && (!loadout || loadout == deploy || loadout == activate ||
		control_definition_get(loadout->definition_index)->control.triggers_when != 0)) extraction.admitted = FALSE;
	/* Every authored start must return a dead/late-joining player to this hub.
	   A nonempty encounter map with field starts is not an admitted hub. */
	if (!global_scenario->players.count) extraction.admitted = FALSE;
	for (index = 0; extraction.admitted && index < MIN(global_scenario->players.count, SHORT_MAX); index++)
	{
		struct player_starting_location *start = TAG_BLOCK_GET_ELEMENT(&global_scenario->players,
			index, struct player_starting_location);
		if (!scenario_trigger_volume_test_point(extraction.return_zone, &start->position) ||
			scenario_trigger_volume_test_point(extraction.zone, &start->position)) extraction.admitted = FALSE;
	}
	if (global_scenario->players.count > SHORT_MAX) extraction.admitted = FALSE;
finished:
	if (extraction.admitted && !extraction.suspended)
		extraction_notice("FIELD TERMINAL | Use the deployment control to begin. No raid timer.");
	else if (!extraction.admitted)
		extraction_notice("Extraction unavailable: use unique deployment/activation controls (touched by player) and extraction/return zones.");
}


static char const *extraction_item_name(char const *path)
{
	char const *slash = strrchr(path, '\\'), *other = strrchr(path, '/');
	if (!slash || (other && other > slash)) slash = other;
	return slash ? slash + 1 : path;
}

static void extraction_loadout_notice(struct extraction_run *run)
{
	struct game_modes_inventory loadout;
	char text[128];
	if (!game_modes_inventory_grenade_room(run->player, run->gear.grenade_room) || !gear_loadout(&run->gear, &loadout))
	{
		extraction_block(run, "LOADOUT PENDING | Native equipment or stash unavailable. Retry at the terminal."); return;
	}
	run->blocked = FALSE;
	snprintf(text, sizeof(text), "STASH %d/%d | %.20s / %.20s | +%d frag +%d plasma. Use: change; crouch-use: discard.",
		(int)run->gear.record.count, GEAR_CAPACITY,
		loadout.weapon_count > 0 ? extraction_item_name(loadout.weapons[0].tag_path) : "carried gear",
		loadout.weapon_count > 1 ? extraction_item_name(loadout.weapons[1].tag_path) : "-",
		(int)loadout.grenades[0], (int)loadout.grenades[1]);
	extraction_tell(run, text);
}

static boolean extraction_deploy(struct extraction_run *run, struct player_datum const *player)
{
	struct game_modes_inventory loadout;
	long created[GAME_MODES_INVENTORY_WEAPONS];
	if (extraction_medical_incapacitated(player->unit_index)) return FALSE;
	run->discard_deadline = 0;
	if (!extraction_open_gear(run, player) || !game_modes_inventory_grenade_room(run->player, run->gear.grenade_room) ||
		!gear_loadout(&run->gear, &loadout) || !game_modes_inventory_loadout_valid(&loadout) || !gear_reserve(&run->gear))
	{
		extraction_block(run, "DEPLOYMENT PENDING | Select compatible gear and retry; no recovered equipment granted."); return FALSE;
	}
	if (!game_modes_inventory_apply(run->player, &loadout, created))
	{
		if (!gear_cancel_reservation(&run->gear)) run->phase = _extraction_cancelling;
		run->retry_tick = game_time_get();
		extraction_block(run, "DEPLOYMENT BLOCKED | Native loadout could not be placed; reserved gear is returning to the stash."); return FALSE;
	}
	gear_track_reserved(&run->gear, created);
	run->unit = player->unit_index; run->local_player = player->local_player_index;
	run->phase = _extraction_deployed; run->activation_ticks = 0; run->blocked = FALSE; run->loadout_seen = FALSE;
	extraction_tell(run, extraction.survival ?
		"SURVIVAL DEPLOYED | Gear is at risk. Recover carried loot by completing this finite encounter alive." :
		"DEPLOYED | Gear is at risk until escape, including disconnects. No raid timer.");
	return TRUE;
}

static void extraction_discard_choice(struct extraction_run *run, long unit_index)
{
	struct gear_item const *choice;
	long now = game_time_get();
	char text[128];
	boolean confirming = run->discard_deadline && now <= run->discard_deadline && run->discard_unit == unit_index;

	if (unit_get(unit_index)->object.parent_object_index != NONE)
	{
		run->discard_deadline = 0;
		extraction_tell(run, "DISCARD BLOCKED | Leave the vehicle before managing stored gear.");
		return;
	}
	if (confirming && now < run->discard_after) return;
	/* Preserve a pending deletion's exact identity even though that item has
	 * already left the published record. Another choice must not be removed. */
	if (confirming && (run->gear.record.generation == run->discard_generation ||
		run->gear.record.generation == run->discard_generation + 1))
	{
		if (!gear_discard(&run->gear, run->discard_id, run->discard_generation))
		{
			if (!run->gear.pending && run->gear.record.generation != run->discard_generation)
			{
				run->discard_deadline = 0;
				extraction_tell(run, "DISCARD CANCELLED | Stash changed. Use again to review the current weapon.");
				return;
			}
			extraction_block(run, "DISCARD PENDING | Saved gear unchanged or awaiting confirmation. Retry crouch-use; no other item will be removed.");
			return;
		}
		run->discard_deadline = 0; run->blocked = FALSE;
		extraction_tell(run, "DISCARD COMPLETE | One confirmed stored weapon removed. Other gear and grenades are safe.");
		return;
	}
	choice = gear_loadout_choice(&run->gear);
	if (!choice)
	{
		if (run->gear.pending)
		{
			extraction_block(run, "STASH PENDING | Wait for the existing save to finish; no new discard was submitted.");
			return;
		}
		run->discard_deadline = 0;
		extraction_tell(run, "NO STORED WEAPON | No compatible saved weapon is available to discard.");
		return;
	}
	memcpy(run->discard_id, choice->id, GEAR_ID_BYTES);
	run->discard_generation = run->gear.record.generation; run->discard_unit = unit_index;
	run->discard_after = now + TICKS_PER_SECOND; run->discard_deadline = now + 10 * TICKS_PER_SECOND;
	snprintf(text, sizeof(text), "DISCARD %.20s (%d/%d ammo)? Wait 1s; release/use again while crouched within 10s. Stand-use cancels.",
		extraction_item_name(choice->equipment.tag_path), (int)choice->equipment.rounds_loaded[0], (int)choice->equipment.rounds_reserve[0]);
	extraction_tell(run, text);
}

boolean extraction_control_touched(long control_index, long unit_index)
{
	struct object_datum *control;
	struct unit_datum *unit;
	struct player_datum *player;
	struct extraction_run *run;
	long slot;
	boolean in_hub;

	if (!extraction_owns_map()) return FALSE;
	control = object_try_and_get_and_verify_type(control_index, _object_mask_control);
	if (!control || (control->object.name_index != extraction.deploy_name && control->object.name_index != extraction.activate_name &&
		(extraction.loadout_name == NONE || control->object.name_index != extraction.loadout_name))) return FALSE;
	if (extraction.suspended || cinematic_in_progress()) return TRUE;
	/* Recheck authored power, usability and one-use locks on the host before
	 * reserving gear or accepting activation. A queued action may be stale. */
	if (!device_can_change_position(control_index)) return TRUE;
	unit = (struct unit_datum *)object_try_and_get_and_verify_type(unit_index, _object_mask_biped);
	if (!unit || unit->unit.player_index == NONE || TEST_FLAG(unit->object.damage_flags, _object_dead_bit) ||
		extraction_medical_incapacitated(unit_index)) return TRUE;
	player = player_try_and_get(unit->unit.player_index);
	slot = DATUM_INDEX_TO_ABSOLUTE_INDEX(unit->unit.player_index);
	if (!player || player->unit_index != unit_index || player->quit_out_of_game || !VALID_INDEX(slot, NUMBEROF(extraction_runs))) return TRUE;
	run = &extraction_runs[slot];
	if (run->player != unit->unit.player_index)
	{
		extraction_run_close(run); run->player = unit->unit.player_index;
	}
	in_hub = scenario_trigger_volume_test_object(extraction.return_zone, unit_index) &&
		!scenario_trigger_volume_test_object(extraction.zone, unit_index);
	if (control->object.name_index == extraction.loadout_name && in_hub &&
		(run->phase == _extraction_hub || run->phase == _extraction_lost || run->phase == _extraction_returned))
	{
		if (extraction_open_gear(run, player))
		{
			/* Standing cancels a preview, but cannot undo an already published
			 * confirmed deletion. Finish only that exact pending transaction. */
			if (run->discard_deadline && run->gear.pending &&
				run->gear.record.generation == run->discard_generation + 1)
			{
				if (!gear_discard(&run->gear, run->discard_id, run->discard_generation))
				{
					extraction_block(run, "DISCARD PENDING | Already confirmed; retry this terminal to finish saving.");
					return TRUE;
				}
				run->discard_deadline = 0; run->blocked = FALSE;
				extraction_tell(run, "DISCARD COMPLETE | The previously confirmed weapon was removed.");
				extraction_loadout_notice(run);
				return TRUE;
			}
			if (TEST_FLAG(unit->unit.control_flags, _unit_control_crouch_modifier_bit))
			{
				extraction_discard_choice(run, unit_index);
				return TRUE;
			}
			if (run->discard_deadline)
			{
				run->discard_deadline = 0;
				extraction_tell(run, "DISCARD CANCELLED | Your stored gear is safe.");
				extraction_loadout_notice(run);
				return TRUE;
			}
			if (run->loadout_seen && !gear_cycle_loadout(&run->gear))
			{
				extraction_block(run, "LOADOUT CHOICE PENDING | Retry this terminal to confirm the saved choice before deploying.");
				return TRUE;
			}
			run->loadout_seen = TRUE;
			extraction_loadout_notice(run);
		}
		return TRUE;
	}
	if (control->object.name_index == extraction.deploy_name && in_hub &&
		(run->phase == _extraction_hub || run->phase == _extraction_lost || run->phase == _extraction_returned))
	{
		/* Let the native control transition notify HaloScript only after the
		 * host has granted the reserved loadout successfully. */
		return !extraction_deploy(run, player);
	}
	if (control->object.name_index == extraction.activate_name && run->phase == _extraction_deployed && run->unit == unit_index &&
		scenario_trigger_volume_test_object(extraction.zone, unit_index))
	{
		run->phase = _extraction_activating; run->activation_ticks = 10 * TICKS_PER_SECOND;
		extraction_tell(run, "EXTRACTION ACTIVATED | Hold this zone for ten seconds. Leaving cancels activation.");
		return FALSE;
	}
	return TRUE;
}

static void extraction_finish_return(struct extraction_run *run)
{
	char text[128];
	run->retry_tick = game_time_get();
	if (!gear_finish_escape(&run->gear))
	{
		extraction_block(run, "RECOVERY PENDING | Escaped gear secured in escrow; banking will retry."); return;
	}
	run->phase = _extraction_returned; run->blocked = FALSE;
	snprintf(text, sizeof(text), "RUN COMPLETE | Stash %d/%d weapons, %d frag, %d plasma. Compatible recovered gear is ready for your next run.",
		(int)run->gear.record.count, GEAR_CAPACITY, (int)run->gear.record.grenades[0], (int)run->gear.record.grenades[1]);
	extraction_tell(run, text);
}

static void extraction_update_run(struct extraction_run *run, struct player_datum const *player, long now)
{
	struct unit_datum *unit;
	byte owner[GEAR_ID_BYTES];
	if (run->discard_deadline)
	{
		unit = (struct unit_datum *)object_try_and_get_and_verify_type(player->unit_index, _object_mask_biped);
		if (now > run->discard_deadline || !unit || player->unit_index != run->discard_unit ||
			TEST_FLAG(unit->object.damage_flags, _object_dead_bit) || unit->object.parent_object_index != NONE ||
			extraction_medical_incapacitated(player->unit_index) || cinematic_in_progress() ||
			!scenario_trigger_volume_test_object(extraction.return_zone, player->unit_index) ||
			scenario_trigger_volume_test_object(extraction.zone, player->unit_index) ||
			!extraction_owner(player, run->player, owner) || memcmp(owner, run->owner, GEAR_ID_BYTES))
			run->discard_deadline = 0;
	}
	if (run->phase == _extraction_settling || run->phase == _extraction_losing || run->phase == _extraction_cancelling)
	{
		if ((unsigned long)now - (unsigned long)run->retry_tick < TICKS_PER_SECOND) return;
		run->retry_tick = now;
		if (run->phase == _extraction_settling) extraction_finish_return(run);
		else if (run->phase == _extraction_losing) extraction_lose_run(run);
		else if (gear_cancel_reservation(&run->gear))
		{
			run->phase = _extraction_hub; run->blocked = FALSE;
			extraction_tell(run, "LOADOUT RETURNED | Retry deployment when ready.");
		}
		return;
	}
	if (run->phase != _extraction_deployed && run->phase != _extraction_activating && run->phase != _extraction_ready) return;
	unit = (struct unit_datum *)object_try_and_get_and_verify_type(run->unit, _object_mask_biped);
	if (player->unit_index != run->unit || player->local_player_index != run->local_player || !unit ||
		unit->unit.player_index != run->player || TEST_FLAG(unit->object.damage_flags, _object_dead_bit) ||
		!extraction_owner(player, run->player, owner) || memcmp(owner, run->owner, GEAR_ID_BYTES))
	{
		extraction_lose_run(run); return;
	}
	if (extraction_medical_incapacitated(run->unit))
	{
		if (run->phase == _extraction_activating)
		{
			run->phase = _extraction_deployed; run->activation_ticks = 0;
		}
		return; /* Retain owned gear while alive; an incapacitated body cannot escape. */
	}
	if (run->phase == _extraction_activating)
	{
		if (!scenario_trigger_volume_test_object(extraction.zone, run->unit))
		{
			run->phase = _extraction_deployed; run->activation_ticks = 0;
			extraction_tell(run, "ACTIVATION CANCELLED | Return and use the extraction control to retry.");
		}
		else if (--run->activation_ticks <= 0)
		{
			run->phase = _extraction_ready;
			extraction_tell(run, "EXTRACTION SIGNAL READY | Reach the return point alive.");
		}
	}
	if (run->phase == _extraction_ready &&
		((extraction.survival && coop_enemies_firefight_outcome() == 1) ||
		 (!extraction.survival && scenario_trigger_volume_test_object(extraction.return_zone, run->unit) &&
		  !scenario_trigger_volume_test_object(extraction.zone, run->unit))) &&
		unit->unit.grenade_throw_state == _unit_grenade_throw_idle)
	{
		struct game_modes_inventory equipment;
		long objects[GAME_MODES_INVENTORY_WEAPONS];
		if (run->blocked && (unsigned long)now - (unsigned long)run->retry_tick < TICKS_PER_SECOND) return;
		run->retry_tick = now;
		if (!game_modes_inventory_capture(run->player, &equipment))
		{
			extraction_block(run, "RETURN BLOCKED | Native equipment is unavailable; no recovery committed."); return;
		}
		if (run->gear.record.count + equipment.weapon_count > GEAR_CAPACITY)
		{
			extraction_block(run, "STASH FULL | Drop unwanted weapons before returning, or make room on the next deployment."); return;
		}
		memcpy(objects, unit->unit.weapon_object_indices, sizeof(objects));
		gear_prepare_escape(&run->gear, &equipment, objects);
		if (run->gear.record.phase != _gear_escrow)
		{
			extraction_block(run, "RETURN PENDING | Stash write failed; keep your gear and retry here."); return;
		}
		/* One game-thread operation: no simulation/input runs between the
		 * captured escrow and consuming these exact authoritative objects. */
		if (!game_modes_inventory_consume(run->player, &equipment, objects))
		{
			extraction.suspended = TRUE;
			extraction_notice("Recovery stopped: native equipment changed during settlement. Restart this map fresh."); return;
		}
		run->phase = _extraction_settling; run->blocked = FALSE;
		extraction_finish_return(run);
	}
}

void extraction_update(void)
{
	struct data_iterator iterator;
	struct player_datum *player;
	long now, slot;
	short index;
	extraction_medical_update();
	if (!extraction_requested()) return;
	if (!extraction.bound)
	{
		extraction.survival = !config_boolean("game.extraction");
		extraction_bind();
	}
	if (!extraction.admitted) return;
	now = game_time_get();
	if (extraction.last_tick != NONE && now < extraction.last_tick)
	{
		extraction_restored(); return;
	}
	if (now == extraction.last_tick) return;
	extraction.last_tick = now;
	if (extraction.suspended)
	{
		if (!extraction.suspension_announced)
		{
			extraction.suspension_announced = TRUE;
			extraction_notice("Saved-world recovery cannot bank gear. Restart this map fresh to deploy safely.");
		}
		return;
	}
	data_iterator_new(&iterator, player_data);
	while ((player = (struct player_datum *)data_iterator_next(&iterator)) != NULL)
	{
		struct extraction_run *run;
		slot = DATUM_INDEX_TO_ABSOLUTE_INDEX(iterator.datum_index);
		if (!VALID_INDEX(slot, NUMBEROF(extraction_runs))) continue;
		run = &extraction_runs[slot];
		if (run->player != iterator.datum_index)
		{
			extraction_run_close(run); run->player = iterator.datum_index;
		}
		if (!player->quit_out_of_game)
		{
			if (extraction.survival)
			{
				short outcome = coop_enemies_firefight_outcome();
				if (!outcome && (run->phase == _extraction_hub || run->phase == _extraction_lost) &&
					(!run->blocked || (unsigned long)now - (unsigned long)run->retry_tick >= TICKS_PER_SECOND))
				{
					struct game_modes_inventory owned;
					if (game_modes_inventory_capture(run->player, &owned))
					{
						run->retry_tick = now;
						extraction_deploy(run, player);
					}
				}
				if (outcome > 0 && run->phase == _extraction_deployed) run->phase = _extraction_ready;
				else if (outcome < 0 && (run->phase == _extraction_deployed || run->phase == _extraction_ready)) extraction_lose_run(run);
			}
			extraction_update_run(run, player, now);
		}
	}
	for (index = 0; index < NUMBEROF(extraction_runs); index++)
	{
		player = player_try_and_get(extraction_runs[index].player);
		if (extraction_runs[index].player != NONE && (!player || player->quit_out_of_game)) extraction_run_close(&extraction_runs[index]);
	}
}

boolean extraction_survival_settlement_pending(void)
{
	short index;
	if (!extraction.survival || !extraction.admitted || extraction.suspended ||
		!coop_enemies_firefight_gear_active() || coop_enemies_firefight_outcome() != 1) return FALSE;
	for (index = 0; index < NUMBEROF(extraction_runs); index++)
	{
		struct extraction_run const *run = &extraction_runs[index];
		struct player_datum *player = player_try_and_get(run->player);
		if (player && !player->quit_out_of_game && (run->phase == _extraction_deployed ||
			run->phase == _extraction_ready || run->phase == _extraction_settling)) return TRUE;
	}
	return FALSE;
}

static void extraction_medical_reset(void)
{
	short index;
	medical_enabled = config_boolean("game.revive");
	medical_received = medical_restored = FALSE;
	medical_tick = NONE;
	medical_seconds = PIN(config_integer("game.revive_seconds"), 1, 30);
	medical_bleed_seconds = PIN(config_integer("game.revive_bleed_seconds"), 5, 180);
	memset(medical_players, 0, sizeof(medical_players));
	for (index = 0; index < NUMBEROF(medical_players); index++)
	{
		medical_players[index].player = medical_players[index].unit = medical_players[index].healer = NONE;
		medical_players[index].received_tick = NONE;
	}
}

static long extraction_medical_slot(long unit_index)
{
	struct unit_datum *unit = (struct unit_datum *)object_try_and_get_and_verify_type(unit_index, _object_mask_biped);
	struct player_datum *player;
	long slot;
	if (!unit || unit->unit.player_index == NONE) return NONE;
	slot = DATUM_INDEX_TO_ABSOLUTE_INDEX(unit->unit.player_index);
	player = player_try_and_get(unit->unit.player_index);
	if (!VALID_INDEX(slot, NUMBEROF(medical_players)) || !player || player->quit_out_of_game || player->unit_index != unit_index) return NONE;
	if (medical_players[slot].unit != unit_index || medical_players[slot].player != unit->unit.player_index)
	{
		memset(&medical_players[slot], 0, sizeof(medical_players[slot]));
		medical_players[slot].player = unit->unit.player_index;
		medical_players[slot].unit = unit_index;
		medical_players[slot].healer = medical_players[slot].received_tick = NONE;
		medical_players[slot].last_damage = game_time_get() - 2 * TICKS_PER_SECOND;
	}
	return slot;
}

boolean extraction_medical_incapacitated(long unit_index)
{
	struct unit_datum *unit;
	struct player_datum *player;
	long slot;
	if (!medical_enabled && !medical_received) return FALSE;
	unit = (struct unit_datum *)object_try_and_get_and_verify_type(unit_index, _object_mask_biped);
	if (!unit || unit->unit.player_index == NONE || TEST_FLAG(unit->object.damage_flags, _object_dead_bit)) return FALSE;
	player = player_try_and_get(unit->unit.player_index);
	if (!player || player->quit_out_of_game || player->unit_index != unit_index) return FALSE;
	slot = DATUM_INDEX_TO_ABSOLUTE_INDEX(unit->unit.player_index);
	return VALID_INDEX(slot, NUMBEROF(medical_players)) && medical_players[slot].downed &&
		medical_players[slot].player == unit->unit.player_index && medical_players[slot].unit == unit_index &&
		unit->object.body_vitality <= 0.f &&
		(!network_game_distributed_client() || medical_players[slot].epoch == network_distributed_epoch());
}

boolean extraction_medical_predictable(short absolute_player_index)
{
	return !VALID_INDEX(absolute_player_index, NUMBEROF(medical_players)) ||
		!extraction_medical_incapacitated(medical_players[absolute_player_index].unit);
}

boolean extraction_medical_network_enabled(void)
{
	unsigned int epoch = network_distributed_epoch();
	return medical_enabled && game_in_progress() && game_connection() == _game_connection_network_server &&
		network_coop_active() && epoch && epoch <= 65535;
}

static boolean extraction_medical_host(void)
{
	if (!medical_enabled || medical_restored || !game_in_progress() || network_game_distributed_client() ||
		!global_scenario || global_scenario->type != _scenario_type_solo || cinematic_in_progress()) return FALSE;
	if (game_connection() == _game_connection_local) return TRUE;
#ifdef HALO_GAME_BROWSER
	return extraction_medical_network_enabled() && delta_peer_game_room_has(_delta_capability_coop) &&
		delta_peer_game_room_has(_delta_capability_object_sync);
#else
	return FALSE;
#endif
}

void extraction_medical_network_state(long unit_index, boolean downed, unsigned int epoch, unsigned short tick)
{
	long slot, stamp, age;
	if (!game_in_progress() || !network_game_distributed_client() || !network_coop_active() || !epoch ||
		epoch != network_distributed_epoch()) return;
	age = (short)((unsigned short)game_time_get() - tick);
	if (age < -15 || age > NETWORK_RULES_TIME_WINDOW_TICKS) return;
	stamp = game_time_get() - age;
	slot = extraction_medical_slot(unit_index);
	if (slot == NONE || (medical_players[slot].epoch == epoch && medical_players[slot].received_tick != NONE &&
		stamp <= medical_players[slot].received_tick)) return;
	medical_players[slot].epoch = epoch;
	medical_players[slot].received_tick = stamp;
	if (downed != medical_players[slot].downed)
		platform_log("medical: client player %ld incapacitation %d at native epoch %u", slot, downed, epoch);
	medical_players[slot].downed = downed;
	medical_received = TRUE;
}

boolean extraction_medical_lethal(long unit_index)
{
	struct unit_datum *unit;
	long slot;
	if (!extraction_medical_host()) return FALSE;
	unit = (struct unit_datum *)object_try_and_get_and_verify_type(unit_index, _object_mask_biped);
	if (!unit || unit->object.parent_object_index != NONE || !unit_controllable(unit_index) ||
		!(unit->object.body_vitality < 0.f) || unit->object.body_vitality < -1.f ||
		TEST_FLAG(unit->object.damage_flags, _object_dead_bit) ||
		TEST_FLAG(unit->object.damage_flags, _object_die_act_of_god_bit)) return FALSE;
	slot = extraction_medical_slot(unit_index);
	if (slot == NONE || medical_players[slot].used) return FALSE;
	medical_players[slot].used = medical_players[slot].downed = TRUE;
	medical_players[slot].deadline = game_time_get() + medical_bleed_seconds * TICKS_PER_SECOND;
	medical_players[slot].last_damage = game_time_get();
	unit->object.body_vitality = 0.f;
	object_deplete_shield(unit_index);
	platform_log("medical: player %ld incapacitated alive; native unit %ld", slot, unit_index);
	extraction_player_notice(medical_players[slot].player,
		"MJOLNIR EMERGENCY STABILIZATION | Downed, alive. An ally must hold Action nearby to apply biofoam before bleedout.");
	return TRUE;
}

void extraction_medical_damaged(long unit_index)
{
	long slot;
	if (!medical_enabled || network_game_distributed_client()) return;
	slot = extraction_medical_slot(unit_index);
	if (slot != NONE) medical_players[slot].last_damage = game_time_get();
}

static boolean extraction_medical_reach(long healer, long target)
{
	struct unit_datum *from = unit_get(healer), *to = unit_get(target);
	real_vector3d delta;
	struct collision_result collision;
	delta.i = to->object.bounding_sphere_center.x - from->object.bounding_sphere_center.x;
	delta.j = to->object.bounding_sphere_center.y - from->object.bounding_sphere_center.y;
	delta.k = to->object.bounding_sphere_center.z - from->object.bounding_sphere_center.z;
	if (delta.i * delta.i + delta.j * delta.j + delta.k * delta.k > 2.25f ||
		from->object.parent_object_index != NONE || to->object.parent_object_index != NONE ||
		game_team_is_enemy(from->object.owner_team_index, to->object.owner_team_index)) return FALSE;
	return !collision_test_vector(FLAG(_collision_test_structure_bit) | FLAG(_collision_test_objects_bit) |
		FLAG(_collision_test_front_facing_surfaces_bit) | FLAG(_collision_test_back_facing_surfaces_bit),
		&from->object.bounding_sphere_center, &delta, healer, &collision) || collision.object_index == target;
}

void extraction_medical_control(long player_index, struct player_action *action)
{
	struct player_datum *player = player_try_and_get(player_index);
	struct unit_datum *unit;
	long slot, target, now = game_time_get();
	unsigned long combat = FLAG(_unit_control_weapon_primary_trigger_bit) | FLAG(_unit_control_weapon_secondary_trigger_bit) |
		FLAG(_unit_control_throw_grenade_bit) | FLAG(_unit_control_use_equipment_bit) | FLAG(_unit_control_weapon_reload_bit) |
		FLAG(_unit_control_user_animation1_bit);
	if (!player || !action || player->unit_index == NONE) return;
	if (extraction_medical_incapacitated(player->unit_index))
	{
		action->control_flags = FLAG(_unit_control_crouch_modifier_bit);
		action->throttle.i = action->throttle.j = action->primary_trigger = 0.f;
		action->desired_zoom_level = NONE;
		action->desired_weapon_index = unit_get(player->unit_index)->unit.current_weapon_index;
		return;
	}
	/* The native queue latches Action to its first tick. Its unlatched
	 * long-hold swap gesture is the existing continuous Action signal. */
	if (!extraction_medical_host() || !(action->control_flags &
		(FLAG(_unit_control_action_bit) | FLAG(_unit_control_swap_weapons_bit))) ||
		(action->control_flags & combat) || action->primary_trigger > 0.f) return;
	slot = extraction_medical_slot(player->unit_index);
	if (slot == NONE || now - medical_players[slot].last_damage < 2 * TICKS_PER_SECOND) return;
	unit = unit_get(player->unit_index);
	if (TEST_FLAG(unit->object.damage_flags, _object_dead_bit) || !unit_controllable(player->unit_index) ||
		unit_is_busy(player->unit_index) || (action->desired_weapon_index != NONE &&
		action->desired_weapon_index != unit->unit.current_weapon_index)) return;
	for (target = 0; target < NUMBEROF(medical_players); target++)
	{
		if (!medical_players[target].downed || !extraction_medical_incapacitated(medical_players[target].unit) ||
			TEST_FLAG(unit_get(medical_players[target].unit)->object.damage_flags, _object_die_act_of_god_bit) ||
			now - medical_players[target].last_damage < 2 * TICKS_PER_SECOND ||
			!extraction_medical_reach(player->unit_index, medical_players[target].unit)) continue;
		if (!(action->control_flags & FLAG(_unit_control_swap_weapons_bit)))
		{
			/* Consume the initial press near the casualty, before native
			 * reload/pickup handling. Treatment waits for an actual hold. */
			action->control_flags = 0;
			return;
		}
		if (medical_players[target].healer != player_index || medical_players[target].healer_unit != player->unit_index ||
			now - medical_players[target].last_control > 1)
		{
			if (medical_players[target].healer != NONE && now - medical_players[target].last_control <= 1) continue;
			medical_players[target].healer = player_index; medical_players[target].healer_unit = player->unit_index;
			medical_players[target].progress = 0;
			platform_log("medical: player %ld begins biofoam for player %ld", slot, target);
		}
		if (medical_players[target].last_control == now) return;
		medical_players[target].last_control = now;
		/* Held Action also generates the native weapon-swap gesture. Consume
		 * it with treatment; a separate weapon selection above still cancels. */
		action->control_flags = 0; action->throttle.i = action->throttle.j = action->primary_trigger = 0.f;
		medical_players[target].progress++;
		if (medical_players[target].progress % TICKS_PER_SECOND == 0 &&
			medical_players[target].progress < medical_seconds * TICKS_PER_SECOND)
		{
			char message[128];
			snprintf(message, sizeof(message), "APPLYING BIOFOAM | %ld/%ld seconds. Hold Action; stay close and avoid combat.",
				medical_players[target].progress / TICKS_PER_SECOND, medical_seconds);
			extraction_player_notice(player_index, message);
		}
		return;
	}
}

static void extraction_medical_update(void)
{
	long index, now = game_time_get();
	if ((!medical_enabled && !medical_received) || !game_in_progress() || now == medical_tick) return;
	medical_tick = now;
	for (index = 0; index < NUMBEROF(medical_players); index++)
	{
		struct player_datum *player = player_try_and_get(medical_players[index].player);
		struct unit_datum *unit = (struct unit_datum *)object_try_and_get_and_verify_type(medical_players[index].unit, _object_mask_biped);
		if (!medical_players[index].downed) continue;
		if (!unit || TEST_FLAG(unit->object.damage_flags, _object_dead_bit))
		{
			medical_players[index].downed = FALSE; continue;
		}
		if (network_game_distributed_client()) continue;
		/* An authored native repair may help a living Spartan too. It never
		 * clears the dead bit or permits this body another free incapacitation. */
		if (unit->object.body_vitality > 0.f)
		{
			medical_players[index].downed = FALSE; continue;
		}
		if (!player || player->quit_out_of_game || player->unit_index != medical_players[index].unit ||
			unit->unit.player_index != medical_players[index].player || now >= medical_players[index].deadline || medical_restored)
		{
			if (!TEST_FLAG(unit->object.damage_flags, _object_die_act_of_god_bit))
				platform_log("medical: player %ld pending native death; revive refused", index);
			unit_kill(medical_players[index].unit); continue; /* Native death/loot, never resurrect a corpse. */
		}
		if (medical_players[index].healer != NONE)
		{
			struct player_datum *healer = player_try_and_get(medical_players[index].healer);
			long healer_slot = healer ? extraction_medical_slot(healer->unit_index) : NONE;
			/* Resolve after damage/physics: a hit or displacement on the final
			 * treatment tick must cancel, not heal first and hide the interruption. */
			if (!healer || healer_slot == NONE || healer->unit_index != medical_players[index].healer_unit ||
				medical_players[index].last_control != now ||
				now - medical_players[index].last_damage < 2 * TICKS_PER_SECOND ||
				now - medical_players[healer_slot].last_damage < 2 * TICKS_PER_SECOND ||
				extraction_medical_incapacitated(healer->unit_index) ||
				unit_is_busy(healer->unit_index) ||
				TEST_FLAG(unit_get(healer->unit_index)->object.damage_flags, _object_dead_bit) ||
				TEST_FLAG(unit->object.damage_flags, _object_die_act_of_god_bit) ||
				!extraction_medical_reach(healer->unit_index, medical_players[index].unit))
			{
				extraction_player_notice(medical_players[index].healer, "BIOFOAM INTERRUPTED | Hold Action again within reach after combat stops.");
				medical_players[index].healer = NONE; medical_players[index].progress = 0;
			}
			else if (medical_players[index].progress >= medical_seconds * TICKS_PER_SECOND)
			{
				unit->object.body_vitality = .25f;
				medical_players[index].downed = FALSE;
				platform_log("medical: player %ld revived alive by player %ld; native unit preserved", index,
					DATUM_INDEX_TO_ABSOLUTE_INDEX(medical_players[index].healer));
				extraction_player_notice(medical_players[index].player, "BIOFOAM STABILIZED | Back on your feet. Further lethal injury to this body is fatal.");
				extraction_player_notice(medical_players[index].healer, "BIOFOAM TREATMENT COMPLETE | Ally stabilized. Native health and shield rules resume.");
			}
		}
	}
	if (medical_restored && !network_game_distributed_client())
	{
		struct data_iterator iterator;
		struct player_datum *player;
		data_iterator_new(&iterator, player_data);
		while ((player = data_iterator_next(&iterator)) != NULL)
		{
			struct unit_datum *unit = (struct unit_datum *)object_try_and_get_and_verify_type(player->unit_index, _object_mask_biped);
			if (unit && unit->object.body_vitality <= 0.f && !TEST_FLAG(unit->object.damage_flags, _object_dead_bit)) unit_kill(player->unit_index);
		}
	}
}
