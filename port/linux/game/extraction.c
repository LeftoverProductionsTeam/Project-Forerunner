/* Native timerless Extraction: the first interaction/state slice.
   Uses existing control actions, scenario volumes and simulation ticks.
   This does not yet implement a loot stash, durable rewards or the hub UI.
   It is opt-in/local only until multiplayer admission and durable results are wired.
   SPDX-License-Identifier: AGPL-3.0-or-later */

#include "cseries.h"
#include "game/game.h"
#include "game/players.h"
#include "game_modes.h"
#include "interface/hud_messaging.h"
#include "interface/player_ui.h"
#include "objects/objects.h"
#include "scenario/scenario.h"
#include "scenario/scenario_definitions.h"
#include "tag_files/tag_groups.h"
#include "units/units.h"
#include "extraction.h"

#include <string.h>

enum extraction_phase
{
	_extraction_hub,
	_extraction_deployed,
	_extraction_activating,
	_extraction_ready,
	_extraction_returned,
	_extraction_lost
};

static struct
{
	boolean bound, admitted;
	short deploy_name, activate_name, zone, return_zone;
	long player, unit, last_tick, activation_ticks;
	long profile;
	short local_player;
	enum extraction_phase phase;
	boolean equipment_valid, equipment_blocked;
	struct game_modes_inventory equipment;
} extraction;

static boolean extraction_requested(void)
{
	/* No new session kind is advertised and no stock peer is told it can
	   understand Extraction. Multiplayer remains an explicit unfinished gate. */
	return config_boolean("game.extraction") && game_connection() == _game_connection_local &&
		global_scenario && global_scenario->type == _scenario_type_solo;
}

boolean extraction_owns_map(void)
{
	return extraction_requested() && extraction.bound && extraction.admitted;
}

static void extraction_notice(char const *text)
{
	wchar_t wide[128];
	long local;
	short index;

	for (index = 0; text[index] && index < NUMBEROF(wide) - 1; index++)
		wide[index] = (wchar_t)(unsigned char)text[index];
	wide[index] = 0;
	for (local = local_player_get_next(NONE); local != NONE; local = local_player_get_next(local))
		hud_print_message((short)local, wide);
}

void extraction_reset(void)
{
	csmemset(&extraction, 0, sizeof(extraction));
	extraction.player = extraction.unit = NONE;
	extraction.profile = NONE;
	extraction.local_player = NONE;
	extraction.deploy_name = extraction.activate_name = extraction.zone = extraction.return_zone = NONE;
	extraction.last_tick = NONE;
}

static void extraction_bind(void)
{
	short index;

	extraction.bound = TRUE;
	extraction.deploy_name = scenario_object_name_index_from_string(global_scenario, "extraction_deploy");
	extraction.activate_name = scenario_object_name_index_from_string(global_scenario, "extraction_activate");
	extraction.zone = extraction.return_zone = NONE;
	for (index = 0; index < global_scenario->trigger_volumes.count; index++)
	{
		struct scenario_trigger_volume *volume = TAG_BLOCK_GET_ELEMENT(
			&global_scenario->trigger_volumes, index, struct scenario_trigger_volume);

		if (!strcmp(volume->name, "extraction_zone"))
		{
			extraction.zone = index;
		}
		else if (!strcmp(volume->name, "extraction_return"))
			extraction.return_zone = index;
	}
	extraction.admitted = extraction.deploy_name != NONE && extraction.activate_name != NONE &&
		extraction.zone != NONE && extraction.return_zone != NONE &&
		object_try_and_get_and_verify_type(object_index_from_name_index(extraction.deploy_name), _object_mask_control) &&
		object_try_and_get_and_verify_type(object_index_from_name_index(extraction.activate_name), _object_mask_control);
	if (extraction.admitted)
		extraction_notice("FIELD TERMINAL | Use the deployment control to begin. No raid timer.");
	else
		extraction_notice("Extraction unavailable: map needs deployment/activation controls and extraction/return zones.");
}

boolean extraction_control_touched(long control_index, long unit_index)
{
	struct object_datum *control;
	struct unit_datum *unit;
	struct player_datum *player;

	if (!extraction_requested() || !extraction.bound || !extraction.admitted)
		return FALSE;
	control = object_try_and_get_and_verify_type(control_index, _object_mask_control);
	unit = (struct unit_datum *)object_try_and_get_and_verify_type(unit_index, _object_mask_biped);
	if (!control || !unit || unit->unit.player_index == NONE || TEST_FLAG(unit->object.damage_flags, _object_dead_bit))
		return FALSE;
	player = player_try_and_get(unit->unit.player_index);
	if (!player || player->unit_index != unit_index || player->quit_out_of_game ||
		player->local_player_index < 0 || player->local_player_index >= MAXIMUM_NUMBER_OF_LOCAL_PLAYERS)
		return FALSE;
	if (control->object.name_index == extraction.deploy_name)
	{
		if ((extraction.phase == _extraction_hub || extraction.phase == _extraction_lost ||
			extraction.phase == _extraction_returned) &&
			scenario_trigger_volume_test_object(extraction.return_zone, unit_index) &&
			!scenario_trigger_volume_test_object(extraction.zone, unit_index))
		{
			extraction.player = unit->unit.player_index;
			extraction.unit = unit_index;
			extraction.local_player = player->local_player_index;
			extraction.profile = player_ui_get_active_player_profile_index(extraction.local_player);
			extraction.phase = _extraction_deployed;
			extraction.equipment_valid = FALSE;
			extraction.equipment_blocked = FALSE;
			csmemset(&extraction.equipment, 0, sizeof(extraction.equipment));
			extraction_notice("DEPLOYED | Return to the extraction control when ready. No raid timer.");
		}
		return TRUE;
	}
	if (control->object.name_index == extraction.activate_name)
	{
		if (extraction.phase == _extraction_deployed && extraction.player == unit->unit.player_index &&
			extraction.unit == unit_index && scenario_trigger_volume_test_object(extraction.zone, unit_index))
		{
			extraction.phase = _extraction_activating;
			extraction.activation_ticks = 10 * TICKS_PER_SECOND;
			extraction_notice("EXTRACTION ACTIVATED | Hold this zone for ten seconds. Leaving cancels activation.");
		}
		return TRUE;
	}
	return FALSE;
}

void extraction_update(void)
{
	struct player_datum *player;
	struct unit_datum *unit;
	long now;

	if (!extraction_requested())
		return;
	if (!extraction.bound)
		extraction_bind();
	if (!extraction.admitted)
		return;
	now = game_time_get();
	/* The mode's source state is not in a native campaign checkpoint yet.
	   A rewind invalidates this run instead of reviving a pending result. */
	if (extraction.last_tick != NONE && now < extraction.last_tick)
	{
		extraction_reset();
		extraction_notice("Run discarded after a checkpoint rewind. Redeploy from the field terminal.");
		return;
	}
	extraction.last_tick = now;
	if (extraction.phase != _extraction_deployed && extraction.phase != _extraction_activating &&
		extraction.phase != _extraction_ready)
		return;
	player = player_try_and_get(extraction.player);
	unit = (struct unit_datum *)object_try_and_get_and_verify_type(extraction.unit, _object_mask_biped);
	if (!player || player->quit_out_of_game || player->unit_index != extraction.unit || !unit ||
		player->local_player_index != extraction.local_player ||
		player_ui_get_active_player_profile_index(extraction.local_player) != extraction.profile ||
		unit->unit.player_index != extraction.player ||
		TEST_FLAG(unit->object.damage_flags, _object_dead_bit))
	{
		extraction.phase = _extraction_lost;
		extraction.activation_ticks = 0;
		extraction.equipment_valid = FALSE;
		csmemset(&extraction.equipment, 0, sizeof(extraction.equipment));
		extraction_notice("RUN LOST | No extraction result committed.");
		return;
	}
	if (extraction.phase == _extraction_activating)
	{
		if (!scenario_trigger_volume_test_object(extraction.zone, extraction.unit))
		{
			extraction.phase = _extraction_deployed;
			extraction.activation_ticks = 0;
			extraction_notice("ACTIVATION CANCELLED | Return and use the extraction control to try again.");
		}
		else if (--extraction.activation_ticks <= 0)
		{
			extraction.phase = _extraction_ready;
			extraction_notice("EXTRACTION SIGNAL READY | Reach the return point alive.");
		}
	}
	/* The authored escape route leads physically back to the hub. Preserve
	   the same player, vehicle and native inventory; no map switch or teleport.
	   Overlapping zones cannot turn activation alone into successful escape. */
	if (extraction.phase == _extraction_ready &&
		scenario_trigger_volume_test_object(extraction.return_zone, extraction.unit) &&
		!scenario_trigger_volume_test_object(extraction.zone, extraction.unit))
	{
		char report[128];

		extraction.equipment_valid = game_modes_inventory_capture(extraction.player, &extraction.equipment);
		if (!extraction.equipment_valid)
		{
			if (!extraction.equipment_blocked)
				extraction_notice("RETURN BLOCKED | Equipment could not be secured. No recovery committed.");
			extraction.equipment_blocked = TRUE;
			return;
		}
		extraction.equipment_blocked = FALSE;
		extraction.phase = _extraction_returned;
		snprintf(report, sizeof(report), "RUN COMPLETE | Carried: %d weapons, %d frag, %d plasma. Terminal: redeploy.",
			(int)extraction.equipment.weapon_count, (int)extraction.equipment.grenades[0],
			(int)extraction.equipment.grenades[1]);
		/* This report records the live loadout, not an awarded/persisted stash. */
		extraction_notice(report);
	}
}
