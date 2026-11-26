/* Native timerless Extraction and finite-Survival gear, using native controls,
 * co-op authority and the shared record. No client claims or second director.
 * Native/hardware gameplay acceptance remains separate from source presence.
 * SPDX-License-Identifier: AGPL-3.0-or-later */
#include "cseries.h"
#include "devices/device_controls.h"
#include "devices/devices.h"
#include "ai/ai.h"
#include "ai/actors.h"
#include "game/game.h"
#include "game/game_globals.h"
#include "game/players.h"
#include "game/player_queues_new.h"
#include "hs/hs.h"
#include "hs/object_lists.h"
#include "hs/hs_scenario_definitions.h"
#include "networking/network_game_globals.h"
#include "networking/network_server_manager.h"
#include "game/game_allegiance.h"
#include "cutscene/cinematics.h"
#include "cutscene/recorded_animations.h"
#include "game_modes.h"
#include "interface/hud_messaging.h"
#include "sound/game_sound.h"
#include "sound/sound_definitions.h"
#include "gear.h"
#include "interface/player_ui.h"
#include "objects/objects.h"
#include "objects/object_types.h"
#include "objects/damage.h"
#include "physics/collisions.h"
#include "scenario/scenario.h"
#include "scenario/scenario_definitions.h"
#include "tag_files/files.h"
#include "tag_files/tag_groups.h"
#include "tag_files/tag_files.h"
#include "units/biped_definitions.h"
#include "items/weapons.h"
#include "items/weapon_definitions.h"
#include "units/units.h"
#include "units/vehicles.h"
#include "units/vehicle_definitions.h"
#include "units/unit_definitions.h"
#include "extraction.h"
#include "network_coop.h"
#include "halo_ui_map_list.h"
#if HALO_MCC && defined(HALO_SPARK_MODE_RECEIPTS) && defined(HALO_64BIT)
#include "mcc_cache_format.h"
#include "mcc_package.h"
#endif
#include "network_chat.h"
#include "network_distributed.h"
#include "network_rules.h"
#include "coop_enemies.h"
#include "coop_scripts.h"
#include "../include/halo_port_limits.h"
#include "../src/port_config.h"
#include "../src/log_address.h"
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

enum
{
	EXTRACTION_SQUAD_MEMBERS = 6,
	EXTRACTION_SQUADS = (HALO_PORT_MAXIMUM_NETWORK_PLAYERS + EXTRACTION_SQUAD_MEMBERS - 1) / EXTRACTION_SQUAD_MEMBERS
};

/* Read-only view of one completed transaction, never a second gear authority.
 * Detailed reports live for this session; persistent outcome/stash remain in gear. */
struct extraction_report
{
	boolean staged, ready, statistics, announced;
	byte run[GEAR_ID_BYTES];
	short outcome, weapon_count, grenades[2], kills, deaths;
	struct game_modes_inventory_weapon weapons[GAME_MODES_INVENTORY_WEAPONS];
};

struct extraction_run
{
	long player, unit, activation_ticks, retry_tick;
	short local_player;
	enum extraction_phase phase;
	boolean blocked, loadout_seen, stash_preview, discard_stash, field_reached, field_exit_ready;
	byte owner[GEAR_ID_BYTES];
	byte discard_id[GEAR_ID_BYTES];
	unsigned long discard_generation;
	long discard_unit, discard_after, discard_deadline;
	long dropship;
	short dropship_team;
	short report_page, start_kills, start_deaths;
	boolean statistics_started, hub_briefed;
	short squad;
	unsigned long squad_operation;
	struct extraction_report report;
	struct gear_session gear;
};

struct extraction_squad_choice
{
	byte owner[GEAR_ID_BYTES];
	byte escrow_run[GEAR_ID_BYTES];
	unsigned long escrow_operation;
	short squad, faction;
	boolean remembered;
};

enum
{
	_extraction_hub_deploy_control,
	_extraction_hub_loadout_control,
	_extraction_hub_debrief_control,
	NUMBER_OF_EXTRACTION_HUB_CONTROLS
};

static char const *const extraction_hub_control_names[2][NUMBER_OF_EXTRACTION_HUB_CONTROLS] = {
	{ "extraction_unsc_deploy", "extraction_unsc_loadout", "extraction_unsc_debrief" },
	{ "extraction_covenant_deploy", "extraction_covenant_loadout", "extraction_covenant_debrief" }
};

static struct
{
	boolean bound, admitted, suspended, suspension_announced, separate_hubs;
	boolean native_mode_checked, authored_extraction, lan_session_gear;
	boolean regional_receipt_checked, regional_receipt_valid;
	boolean survival, evacuation, completion_notified, squad_script_warning, squad_notice_warning, squad_radio_warning;
	short deploy_name, activate_name, loadout_name, debrief_name, zone, return_zone;
	short dropship_return_zones[2];
	short dropship_names[2]; /* native named UNSC/Covenant vehicles; optional */
	short dropship_activate_names[2], dropship_zones[2]; /* optional paired field routes */
	short dropship_boarding_groups[2]; /* optional host-derived native outputs */
	short dropship_landing_groups[2]; /* optional touchdown or transfer output */
	boolean dropship_hover_transfer[2];
	short dropship_call_groups[2]; /* optional faction-specific evacuation requests */
	/* Full native handles for this arrival, cleared by actual LZ boarding. */
	long dropship_evacuating[2], dropship_banked[2], dropship_completed[2];
	boolean dropship_return_stop[2];
	short faction_names[2], faction_profiles[2]; /* optional native hub choices */
	short faction_hubs[2]; /* paired authored start volumes */
	short faction_hub_bsps[2]; /* compiled start ownership on regional maps */
	short hub_controls[2][NUMBER_OF_EXTRACTION_HUB_CONTROLS];
	long faction_bipeds[2];
	short faction_variants[2]; /* optional nonautomatic native biped placements */
	long last_tick;
	/* Optional hub choices only; native scripts still own mission objectives,
	 * and every member keeps their existing independent gear transaction. */
	struct { short name, ready_group; unsigned long operation; boolean notified, banked, unconfirmed_escape; } squads[EXTRACTION_SQUADS];
	/* Map-local choices for verified reconnects. Only live runs occupy seats. */
	struct extraction_squad_choice squad_choices[HALO_PORT_MAXIMUM_NETWORK_PLAYERS];
	short next_squad_choice;
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
static void extraction_bind(void);
static void extraction_medical_reset(void);
static void extraction_medical_update(void);
static boolean extraction_players_returned(void);
static struct extraction_squad_choice *extraction_squad_choice(byte const owner[GEAR_ID_BYTES]);
static char const *extraction_resolve_owner(struct player_datum const *player, long player_index, byte owner[GEAR_ID_BYTES]);

/* Match Halo's three native forced-death requests. Do not consume the flags,
 * clear death, or substitute settlement for native death and loot disposal. */
static boolean extraction_unit_pending_death(struct unit_datum const *unit)
{
	return (unit->object.damage_flags & (FLAG(_object_die_act_of_god_bit) |
		FLAG(_object_die_act_of_god_silent_bit) | FLAG(_object_die_act_of_god_no_statistics_bit))) != 0;
}

static boolean extraction_authoritative_scene(void)
{
	if (!global_scenario)
		return FALSE;
	if (global_scenario->type == _scenario_type_solo)
		return game_connection() == _game_connection_local ||
			(game_connection() == _game_connection_network_server && network_coop_active());
#if HALO_MCC && defined(HALO_SPARK_MODE_RECEIPTS) && defined(HALO_64BIT)
	if (game_connection() == _game_connection_network_server && !config_boolean("network.online") &&
		config_boolean("game.extraction") && network_regional_independent_bsp_ready())
	{
		if (!extraction.regional_receipt_checked)
		{
			char path[128];
			wchar_t description[128] = {0};
			extraction.regional_receipt_checked = TRUE;
			extraction.regional_receipt_valid = mcc_package_map_path(tag_get_name(global_scenario_index), path) &&
				ui_map_list_spark_description(path, MCC_CACHE_VERSION, FALSE, description, NUMBEROF(description)) ==
					_ui_map_mode_regional_extraction;
		}
		return extraction.regional_receipt_valid;
	}
#endif
	return FALSE;
}

static boolean extraction_host_firefight_requested(void)
{
	return game_connection() == _game_connection_network_server && config_boolean("network.firefight_encounter");
}

static boolean extraction_mode_selected(void)
{
	return extraction.authored_extraction || config_boolean("game.extraction");
}

static boolean extraction_requested(void)
{
	return extraction_authoritative_scene() &&
		(extraction_mode_selected() || coop_enemies_firefight_gear_active());
}

static void extraction_check_authored_mode(void)
{
	if (extraction.native_mode_checked || !extraction_authoritative_scene())
		return;
	extraction.native_mode_checked = TRUE;
	if (config_boolean("game.extraction") ||
		extraction_host_firefight_requested() ||
		coop_enemies_authored_round())
		return;

	/* Existing Spark exports identify timerless Extraction through native
	 * control names. The ordinary binder still admits every control, zone,
	 * faction route and spawn before any gear can be reserved. */
	extraction.authored_extraction =
		scenario_object_name_index_from_string(global_scenario, "extraction_deploy") != NONE &&
		scenario_object_name_index_from_string(global_scenario, "extraction_activate") != NONE;
}

static short extraction_faction(short team)
{
	if (team == _game_team_player || team == _game_team_human) return 0;
	return team == _game_team_covenant ? 1 : NONE;
}

boolean extraction_owns_map(void)
{
	return extraction_mode_selected() && !extraction.survival && extraction_requested() && extraction.bound && extraction.admitted;
}

static unsigned short extraction_start_hubs(struct player_starting_location const *start)
{
	unsigned short hubs = 0;
	short faction;

	for (faction = 0; faction < NUMBEROF(extraction.faction_hubs); faction++)
		if (extraction.faction_hubs[faction] != NONE &&
			scenario_trigger_volume_test_point(extraction.faction_hubs[faction], &start->position))
			hubs |= FLAG(faction);
	return hubs;
}

static void extraction_prepare_spawn_binding(void)
{
	/* The first native spawn can run before the regular Extraction tick. */
	if (!extraction.bound && extraction_authoritative_scene())
	{
		extraction_check_authored_mode();
		if (extraction_mode_selected())
			extraction_bind();
	}
}

short extraction_player_spawn_bsp(long player_index)
{
	struct player_datum const *player = player_try_and_get(player_index);
	short faction = player ? extraction_faction((short)player->team_index) : NONE;

	extraction_prepare_spawn_binding();
	return faction != NONE && extraction_owns_map() ? extraction.faction_hub_bsps[faction] : NONE;
}

boolean extraction_player_start_allowed(long player_index, struct player_starting_location const *start)
{
	struct player_datum const *player;
	short faction;

	extraction_prepare_spawn_binding();
	if (!extraction_owns_map() || extraction.faction_hubs[0] == NONE)
		return TRUE;
	player = player_try_and_get(player_index);
	faction = player ? extraction_faction((short)player->team_index) : NONE;
	return faction != NONE && extraction_start_hubs(start) == FLAG(faction) &&
		(extraction.faction_hub_bsps[faction] == NONE ||
		 start->structure_bsp_reference_index == extraction.faction_hub_bsps[faction]);
}

typedef char extraction_voice_groups_fit_byte[
	(2 + EXTRACTION_SQUADS + HALO_PORT_MAXIMUM_NETWORK_MACHINES <= 256) ? 1 : -1];

short extraction_voice_group(long player_index, long machine_index)
{
	struct player_datum *player;
	struct extraction_run const *run;
	short isolated, faction;
	long slot = DATUM_INDEX_TO_ABSOLUTE_INDEX(player_index);
	if (!game_in_progress() || game_connection() != _game_connection_network_server || !extraction_requested() ||
		!VALID_INDEX(machine_index, HALO_PORT_MAXIMUM_NETWORK_MACHINES)) return NONE;
	isolated = (short)(2 + EXTRACTION_SQUADS + machine_index);
	/* Do not broadcast between operations while native admission is pending.
	 * A rejected ordinary map then keeps the inherited co-op radio behavior. */
	if (!extraction.bound) return isolated;
	if (!extraction.admitted) return NONE;
	if (extraction.suspended) return isolated;
	player = player_try_and_get(player_index);
	if (!player || player->quit_out_of_game) return isolated;
	run = VALID_INDEX(slot, NUMBEROF(extraction_runs)) ? &extraction_runs[slot] : NULL;
	if (run && run->player == player_index && VALID_INDEX(run->squad, NUMBEROF(extraction.squads)) &&
		extraction.squads[run->squad].name != NONE) return (short)(2 + run->squad);
	faction = extraction_faction((short)player->team_index);
	return faction != NONE ? faction : isolated;
}

boolean extraction_script_players(char const *name, long *object_list)
{
	short squad, index;
	if (!name || !object_list) return FALSE;
	for (squad = 0; squad < NUMBEROF(extraction.squads); squad++)
	{
		char query[32];
		snprintf(query, sizeof(query), "forerunner_squad_%02d_players", squad + 1);
		if (!strcmp(name, query)) break;
	}
	if (squad == NUMBEROF(extraction.squads)) return FALSE;
	*object_list = NONE;
	if (!extraction_owns_map() || extraction.suspended || extraction.squads[squad].name == NONE ||
		!extraction.squads[squad].operation) return TRUE;
	/* Legacy campaign conveniences can broaden a single player's action to
	 * followers. Authored operations already have an exact native policy. */
	if (!coop_scripts_native_player_actions())
	{
		if (!extraction.squad_script_warning)
		{
			extraction.squad_script_warning = TRUE;
			platform_log("extraction: squad script queries require network.coop_script_adaptations=false at map start");
		}
		return TRUE;
	}
	*object_list = object_list_new();
	if (*object_list == NONE) return TRUE;
	for (index = 0; index < NUMBEROF(extraction_runs); index++)
	{
		struct extraction_run const *run = &extraction_runs[index];
		struct player_datum *player;
		struct unit_datum *unit;
		byte owner[GEAR_ID_BYTES];
		if (run->squad != squad || run->squad_operation != extraction.squads[squad].operation ||
			(run->phase != _extraction_deployed && run->phase != _extraction_activating && run->phase != _extraction_ready &&
			 run->phase != _extraction_settling && run->phase != _extraction_returned)) continue;
		player = player_try_and_get(run->player);
		if (!player || player->quit_out_of_game || player->unit_index != run->unit || !run->gear.opened ||
			extraction_resolve_owner(player, run->player, owner) || memcmp(owner, run->owner, GEAR_ID_BYTES)) continue;
		unit = (struct unit_datum *)object_try_and_get_and_verify_type(run->unit, _object_mask_biped);
		if (!unit || unit->unit.player_index != run->player || TEST_FLAG(unit->object.damage_flags, _object_dead_bit) ||
			extraction_unit_pending_death(unit)) continue;
		if (!object_list_add(*object_list, run->unit))
		{
			/* An incomplete list must never make an ALL objective pass early. */
			object_list_delete(*object_list); *object_list = NONE; return TRUE;
		}
	}
	object_list_mark_players(*object_list);
	return TRUE;
}

boolean extraction_script_notice(char const *name, short message_index)
{
	short squad;
	long list, reference, object;
	char query[32], text[128];
	if (!name) return FALSE;
	for (squad = 0; squad < NUMBEROF(extraction.squads); squad++)
	{
		snprintf(query, sizeof(query), "forerunner_squad_%02d_notice", squad + 1);
		if (!strcmp(name, query)) break;
	}
	if (squad == NUMBEROF(extraction.squads)) return FALSE;
	snprintf(query, sizeof(query), "forerunner_squad_%02d_players", squad + 1);
	/* Reuse admission, authority, current-operation, identity and living-body
	 * checks rather than allowing a script to choose arbitrary recipients. */
	extraction_script_players(query, &list);
	if (list == NONE) return TRUE;
	if (hud_messaging_notice_text(message_index, text, sizeof(text)))
	{
		for (object = object_list_get_first(list, &reference); object != NONE; object = object_list_get_next(list, &reference))
		{
			struct unit_datum *unit = (struct unit_datum *)object_try_and_get_and_verify_type(object, _object_mask_biped);
			if (unit) extraction_player_notice(unit->unit.player_index, text);
		}
	}
	else if (!extraction.squad_notice_warning)
	{
		extraction.squad_notice_warning = TRUE;
		platform_log("extraction: squad notices require a nonempty plain-text HMT message, printable ASCII, at most 127 characters");
	}
	object_list_delete(list);
	return TRUE;
}

boolean extraction_script_radio(char const *name, long sound)
{
	short squad, members = 0, machine_count = 0, index, other;
	long list, reference, object;
	long players[EXTRACTION_SQUAD_MEMBERS], machines[EXTRACTION_SQUAD_MEMBERS];
	boolean play_here = FALSE, delivered = FALSE;
	char query[32];
	if (!name) return FALSE;
	for (squad = 0; squad < NUMBEROF(extraction.squads); squad++)
	{
		snprintf(query, sizeof(query), "forerunner_squad_%02d_radio", squad + 1);
		if (!strcmp(name, query)) break;
	}
	if (squad == NUMBEROF(extraction.squads)) return FALSE;
	snprintf(query, sizeof(query), "forerunner_squad_%02d_players", squad + 1);
	extraction_script_players(query, &list);
	if (list == NONE) return TRUE;
	/* One shared audio output cannot privately address only one split-screen
	 * view. A machine with any other active player must not hear this radio. */
	for (object = object_list_get_first(list, &reference); object != NONE; object = object_list_get_next(list, &reference))
	{
		struct unit_datum *unit = (struct unit_datum *)object_try_and_get_and_verify_type(object, _object_mask_biped);
		struct player_datum *player = unit ? player_try_and_get(unit->unit.player_index) : NULL;
		long machine;
		if (!player || members == NUMBEROF(players)) continue;
		players[members++] = unit->unit.player_index;
		machine = VALID_INDEX(player->local_player_index, MAXIMUM_NUMBER_OF_LOCAL_PLAYERS) ? NONE :
			distributed_player_machine((short)DATUM_INDEX_TO_ABSOLUTE_INDEX(unit->unit.player_index));
		if (machine == NONE && !VALID_INDEX(player->local_player_index, MAXIMUM_NUMBER_OF_LOCAL_PLAYERS)) continue;
		for (index = 0; index < machine_count && machines[index] != machine; index++) ;
		if (index == machine_count) machines[machine_count++] = machine;
	}
	object_list_delete(list);
	if (!members || !distributed_tag_of_group(sound, SOUND_DEFINITION_TAG)) return TRUE;
	for (index = 0; index < machine_count; index++)
	{
		struct data_iterator iterator;
		struct player_datum *player;
		boolean permitted = TRUE;
		data_iterator_new(&iterator, player_data);
		while ((player = data_iterator_next(&iterator)) != NULL)
		{
			boolean local = VALID_INDEX(player->local_player_index, MAXIMUM_NUMBER_OF_LOCAL_PLAYERS);
			if (player->quit_out_of_game || (machines[index] == NONE ? !local :
				(local || distributed_player_machine((short)DATUM_INDEX_TO_ABSOLUTE_INDEX(iterator.datum_index)) != machines[index]))) continue;
			for (other = 0; other < members && players[other] != iterator.datum_index; other++) ;
			if (other == members) { permitted = FALSE; break; }
		}
		if (!permitted)
		{
			if (!extraction.squad_radio_warning)
			{
				extraction.squad_radio_warning = TRUE;
				platform_log("extraction: private squad radio withheld from a machine with other local participants");
			}
			continue;
		}
		if (machines[index] == NONE) play_here = delivered = TRUE;
		else delivered |= network_coop_private_sound(machines[index], sound);
	}
	/* Preserve native script timing, but never play or broadcast on the host
	 * when only a remote operation is listening. Client playback stays native. */
	if (delivered) scripted_sound_new_private(sound, play_here);
	return TRUE;
}

boolean extraction_player_spawn_profile(long player_index, long *biped, short *profile, short *variant)
{
	struct player_datum *player = player_try_and_get(player_index);
	short faction;
	if (!player || !biped || !profile || !variant || !extraction.bound || !extraction.admitted ||
		extraction.suspended || !extraction_requested()) return FALSE;
	faction = extraction_faction((short)player->team_index);
	if (faction == NONE || extraction.faction_profiles[faction] == NONE ||
		(extraction.faction_bipeds[faction] == NONE && extraction.faction_hub_bsps[faction] == NONE))
		return FALSE;
	if (extraction.faction_bipeds[faction] != NONE)
		*biped = extraction.faction_bipeds[faction];
	*profile = extraction.faction_profiles[faction];
	*variant = extraction.faction_variants[faction];
	return TRUE;
}

static void extraction_notice_to_machine(long machine, short controller, char const *text)
{
	char notice[144];
	/* The existing chat/notice layer works after native weapons/HUD are banked.
	 * Its 160-byte notice packet holds the complete existing 127-byte text and
	 * controller prefix. Chat owns wrapping and drawing for every recipient. */
	if (controller == NONE) snprintf(notice, sizeof(notice), "%.127s", text);
	else snprintf(notice, sizeof(notice), "P%d | %.127s", controller + 1, text);
	if (machine == NONE && !network_chat_notice_visible())
	{
		if (controller != NONE) hud_print_notice(controller, notice);
		else
		{
			short local;
			for (local = local_player_get_next(NONE); local != NONE; local = local_player_get_next(local))
				hud_print_notice(local, notice);
		}
		return;
	}
	distributed_send_notice_to_machine(machine, notice);
}

void extraction_notice(char const *text)
{
	long machines[HALO_PORT_MAXIMUM_NETWORK_MACHINES];
	short index, count;
	/* Chat is shared by this machine's views; global notices are shown once. */
	if (local_player_get_next(NONE) != NONE) extraction_notice_to_machine(NONE, NONE, text);
	if (game_connection() == _game_connection_network_server)
	{
		count = distributed_client_machines(machines, NUMBEROF(machines));
		for (index = 0; index < count; index++) extraction_notice_to_machine(machines[index], NONE, text);
	}
}

void extraction_player_notice(long player_index, char const *text)
{
	struct player_datum *player = player_try_and_get(player_index);
	long machine;
	if (!player || player->quit_out_of_game) return;
	if (player->local_player_index >= 0 && player->local_player_index < MAXIMUM_NUMBER_OF_LOCAL_PLAYERS)
		extraction_notice_to_machine(NONE, player->local_player_index, text);
	else
	{
		machine = distributed_player_machine((short)DATUM_INDEX_TO_ABSOLUTE_INDEX(player_index));
		if (machine != NONE)
			extraction_notice_to_machine(machine, player->network_player_data.controller_index, text);
	}
}

static void extraction_tell(struct extraction_run const *run, char const *text)
{
	extraction_player_notice(run->player, text);
	/* Keep native control/countdown decisions observable without treating an
	 * input press as host acknowledgement. No account or gear identity is logged. */
	if (config_string("debug.network_test")[0])
		platform_log("network test: extraction notice player %ld phase %d: %s",
			DATUM_INDEX_TO_ABSOLUTE_INDEX(run->player), run->phase, text);
}

static void extraction_block(struct extraction_run *run, char const *text)
{
	if (!run->blocked)
	{
		extraction_tell(run, text);
		platform_log("extraction: player %ld blocked: %s", DATUM_INDEX_TO_ABSOLUTE_INDEX(run->player), text);
	}
	run->blocked = TRUE;
}

static void extraction_remember_bank(struct extraction_run const *run)
{
	short faction;
	if (run->gear.record.phase != _gear_idle || run->gear.record.outcome != _gear_escaped ||
		memcmp(run->gear.record.settled_run, run->gear.run, GEAR_ID_BYTES)) return;
	/* The durable escape outlives its player slot while the current squad
	 * still has teammates in the field or settling their runs. */
	if (run->squad_operation && VALID_INDEX(run->squad, NUMBEROF(extraction.squads)) &&
		run->squad_operation == extraction.squads[run->squad].operation)
		extraction.squads[run->squad].banked = TRUE;
	if (run->dropship == NONE) return;
	faction = extraction_faction(run->dropship_team);
	if (faction != NONE && extraction.dropship_names[faction] != NONE &&
		object_index_from_name_index(extraction.dropship_names[faction]) == run->dropship)
		extraction.dropship_banked[faction] = run->dropship;
}

static void extraction_reconcile_squad_escrow(struct gear_record const *record)
{
	struct extraction_squad_choice *choice = extraction_squad_choice(record->owner);
	short squad;

	if (!choice || !choice->escrow_operation) return;
	squad = choice->squad;
	/* Only the verified owner's exact receipt can complete this map's
	 * still-current operation. A squad with a durable escape already
	 * reports success once, regardless of other passengers' writes. */
	if (record->phase == _gear_idle && record->outcome == _gear_escaped &&
		!memcmp(record->settled_run, choice->escrow_run, GEAR_ID_BYTES) &&
		VALID_INDEX(squad, NUMBEROF(extraction.squads)) &&
		choice->escrow_operation == extraction.squads[squad].operation && !extraction.squads[squad].banked)
	{
		extraction.squads[squad].banked = TRUE;
		extraction.squads[squad].notified = FALSE;
	}
	choice->escrow_operation = 0;
}

static void extraction_run_close(struct extraction_run *run)
{
	if (run->gear.opened)
	{
		/* A failed native grant removed all newly created equipment. Preserve
		 * its refund on disconnect, slot reuse and map exit as on a live retry.
		 * Actual deployments still lose gear; committed escapes finish escrow. */
		if (run->phase == _extraction_cancelling)
		{
			if (!gear_cancel_reservation(&run->gear))
				platform_log("extraction: ungranted loadout refund failed before session close");
		}
		else if (gear_lose(&run->gear))
		{
			if (run->phase == _extraction_settling) extraction_remember_bank(run);
		}
		else if (run->phase == _extraction_settling && VALID_INDEX(run->squad, NUMBEROF(extraction.squads)) &&
			run->squad_operation && run->squad_operation == extraction.squads[run->squad].operation)
		{
			struct extraction_squad_choice *choice = extraction_squad_choice(run->gear.record.owner);

			/* A disconnected escape escrow may still bank when reopened. */
			extraction.squads[run->squad].unconfirmed_escape = TRUE;
			if (choice && choice->squad == run->squad)
			{
				memcpy(choice->escrow_run, run->gear.run, GEAR_ID_BYTES);
				choice->escrow_operation = run->squad_operation;
			}
		}
		gear_close(&run->gear);
	}
	memset(run, 0, sizeof(*run));
	run->player = run->unit = NONE;
	run->dropship = NONE;
	run->local_player = NONE;
	run->squad = NONE;
	run->gear.descriptor = -1;
}

void extraction_reset(void)
{
	short index;
	for (index = 0; index < NUMBEROF(extraction_runs); index++) extraction_run_close(&extraction_runs[index]);
	memset(&extraction, 0, sizeof(extraction));
	for (index = 0; index < NUMBEROF(extraction.squads); index++)
		extraction.squads[index].name = extraction.squads[index].ready_group = NONE;
	extraction.deploy_name = extraction.activate_name = extraction.loadout_name = extraction.debrief_name = extraction.zone = extraction.return_zone = NONE;
	extraction.dropship_names[0] = extraction.dropship_names[1] = NONE;
	extraction.dropship_activate_names[0] = extraction.dropship_activate_names[1] = NONE;
	extraction.dropship_zones[0] = extraction.dropship_zones[1] = NONE;
	extraction.dropship_return_zones[0] = extraction.dropship_return_zones[1] = NONE;
	extraction.dropship_boarding_groups[0] = extraction.dropship_boarding_groups[1] = NONE;
	extraction.dropship_call_groups[0] = extraction.dropship_call_groups[1] = NONE;
	extraction.dropship_landing_groups[0] = extraction.dropship_landing_groups[1] = NONE;
	extraction.dropship_evacuating[0] = extraction.dropship_evacuating[1] = NONE;
	extraction.dropship_banked[0] = extraction.dropship_banked[1] = NONE;
	extraction.dropship_completed[0] = extraction.dropship_completed[1] = NONE;
	extraction.faction_names[0] = extraction.faction_names[1] = NONE;
	extraction.faction_profiles[0] = extraction.faction_profiles[1] = NONE;
	extraction.faction_hubs[0] = extraction.faction_hubs[1] = NONE;
	memset(extraction.hub_controls, NONE, sizeof(extraction.hub_controls));
	extraction.faction_bipeds[0] = extraction.faction_bipeds[1] = NONE;
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

/* Return the deployment prompt on failure, or fill owner and return NULL. */
static boolean extraction_lan_guest(long machine)
{
	unsigned long address;
	byte octets[4];
	if (!extraction.lan_session_gear || machine == NONE || config_boolean("network.online"))
		return FALSE;
	address = network_game_server_machine_address(machine);
	octets[0] = (byte)(address >> 24); octets[1] = (byte)(address >> 16);
	octets[2] = (byte)(address >> 8); octets[3] = (byte)address;
	/* Zero and Internet Play's virtual range can be treated as local in
	 * diagnostics. Neither establishes a direct private LAN peer. */
	return octets[0] != 0 && !(octets[0] == 100 && (octets[1] & 0xc0) == 64) &&
		log_address_local(octets, sizeof(octets));
}

static char const *extraction_resolve_owner_mode(struct player_datum const *player, long player_index,
	byte owner[GEAR_ID_BYTES], boolean *session_only)
{
	boolean local = player->local_player_index >= 0 && player->local_player_index < MAXIMUM_NUMBER_OF_LOCAL_PLAYERS;
	*session_only = FALSE;
	if (!local && game_connection() == _game_connection_network_server &&
#ifdef HALO_GAME_BROWSER
		!delta_peer_game_on_delta() &&
#endif
		extraction_lan_guest(distributed_player_machine((short)DATUM_INDEX_TO_ABSOLUTE_INDEX(player_index))))
	{
		gear_session_owner(player_index, owner);
		*session_only = TRUE;
		return NULL;
	}
#ifdef HALO_GAME_BROWSER
	/* Hosting and joining must resolve the same verified account to the same
	 * stash. A Delta host without its bound proof waits; it must not fall
	 * back to an unrelated offline profile owner while authentication loads. */
	if (game_connection() == _game_connection_network_server && (!local || delta_peer_game_on_delta()))
	{
		struct delta_peer_identity identity;
		/* The distributed lookup contains remote clients only. The host's
		 * native player already carries its authoritative machine membership. */
		long machine = local ? player->network_player_data.machine_index :
			distributed_player_machine((short)DATUM_INDEX_TO_ABSOLUTE_INDEX(player_index));
		if (machine == NONE || !delta_peer_game_player_identity((int)machine,
			player->network_player_data.controller_index, &identity) || !identity.bound)
			return "DEPLOYMENT BLOCKED | Link your Delta account and enable Share My Profile in Delta Profile > Privacy.";
		gear_account_owner(identity.account, owner);
		return NULL;
	}
#endif
	if (local)
	{
		char directory[MAXIMUM_FILENAME_LENGTH + 1];
		if (player_ui_get_active_player_profile_index(player->local_player_index) == NONE ||
			!player_ui_get_path_to_local_player_profile_directory(player->local_player_index, directory) || !directory[0])
			return "DEPLOYMENT BLOCKED | Select a saved player profile in Settings.";
		gear_profile_owner(directory, owner);
		return NULL;
	}
	return "DEPLOYMENT BLOCKED | This session cannot verify a gear owner.";
}

static char const *extraction_resolve_owner(struct player_datum const *player, long player_index, byte owner[GEAR_ID_BYTES])
{
	boolean session_only;
	return extraction_resolve_owner_mode(player, player_index, owner, &session_only);
}

static boolean extraction_open_gear(struct extraction_run *run, struct player_datum const *player)
{
	byte owner[GEAR_ID_BYTES];
	boolean session_only;
	char const *blocked = extraction_resolve_owner_mode(player, run->player, owner, &session_only);
	if (blocked)
	{
		extraction_block(run, blocked);
		return FALSE;
	}
	if (memcmp(run->owner, owner, GEAR_ID_BYTES))
	{
		run->squad = NONE; run->squad_operation = 0;
		run->discard_deadline = 0;
		run->stash_preview = FALSE;
		memset(&run->report, 0, sizeof(run->report));
		run->report_page = 0; run->statistics_started = FALSE;
	}
	if (run->gear.opened && memcmp(run->owner, owner, GEAR_ID_BYTES))
	{
		gear_lose(&run->gear); gear_close(&run->gear);
	}
	if (!run->gear.opened)
	{
		if (!(session_only ? gear_open_session(&run->gear, owner) : gear_open(&run->gear, owner)))
		{
			extraction_block(run, "STASH UNAVAILABLE | Gear is locked, unreadable or awaiting recovery. Retry at the terminal.");
			return FALSE;
		}
		if (session_only)
			extraction_tell(run, "LAN SESSION STASH | Recovered gear lasts until you leave or the match ends.");
		else
			extraction_reconcile_squad_escrow(&run->gear.record);
	}
	memcpy(run->owner, owner, GEAR_ID_BYTES);
	run->gear.loadout_unit = (extraction.faction_bipeds[0] != NONE || extraction.faction_bipeds[1] != NONE) ? player->unit_index : NONE;
	run->blocked = FALSE;
	return TRUE;
}

static void extraction_report_stage(struct extraction_run *run, short outcome)
{
	struct extraction_report *report = &run->report;
	struct gear_record const *record = &run->gear.record;
	struct player_datum *player;
	short index;
	if (report->staged && !memcmp(report->run, run->gear.run, GEAR_ID_BYTES)) return;
	memset(report, 0, sizeof(*report)); run->report_page = 0;
	if ((outcome == _gear_escaped && record->phase != _gear_escrow) ||
		(outcome == _gear_lost && record->phase != _gear_deployed)) return;
	report->staged = TRUE; report->outcome = outcome;
	memcpy(report->run, run->gear.run, GEAR_ID_BYTES);
	report->weapon_count = outcome == _gear_escaped ? record->escrow_count : record->reserved_count;
	memcpy(report->grenades, outcome == _gear_escaped ? record->escrow_grenades : record->reserved_grenades, sizeof(report->grenades));
	for (index = 0; index < report->weapon_count; index++)
		report->weapons[index] = (outcome == _gear_escaped ? record->escrow[index] : record->reserved[index]).equipment;
	player = player_try_and_get(run->player);
	if (player && run->statistics_started && run->start_kills >= 0 && run->start_deaths >= 0 &&
		player->statistics.kills[0] >= run->start_kills && player->statistics.deaths >= run->start_deaths)
	{
		report->statistics = TRUE;
		report->kills = player->statistics.kills[0] - run->start_kills;
		report->deaths = player->statistics.deaths - run->start_deaths;
	}
}

static void extraction_lose_run(struct extraction_run *run)
{
	/* An aircraft still waits on a connected passenger's unresolved loss. */
	run->phase = _extraction_losing;
	run->activation_ticks = 0;
	run->retry_tick = game_time_get();
	extraction_report_stage(run, _gear_lost);
	if (gear_lose(&run->gear))
	{
		if (run->report.staged && run->report.outcome == _gear_lost &&
			!memcmp(run->report.run, run->gear.run, GEAR_ID_BYTES)) run->report.ready = TRUE;
		run->phase = _extraction_lost;
		run->dropship = NONE;
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

static short extraction_zone_for_faction(short faction)
{
	if (VALID_INDEX(faction, 2) && extraction.dropship_zones[faction] != NONE)
		return extraction.dropship_zones[faction];
	return extraction.zone;
}

static short extraction_transport_return_zone(short faction)
{
	return VALID_INDEX(faction, NUMBEROF(extraction.dropship_return_zones)) &&
		extraction.dropship_return_zones[faction] != NONE
		? extraction.dropship_return_zones[faction] : extraction.return_zone;
}

static boolean extraction_in_field_volume(short zone, short call, long object)
{
	if (zone == NONE || !scenario_trigger_volume_test_object(zone, object))
		return FALSE;
	if (scenario_uses_regional_gameplay(global_scenario))
	{
		short bsp = object_get_regional_bsp(object);
		long control = object_index_from_name_index(call);

		/* The live field control qualifies its trigger when BSPs overlap. */
		return bsp != NONE && control != NONE && object_get_regional_bsp(control) == bsp;
	}
	return TRUE;
}

static boolean extraction_in_owned_volume(short zone, short bsp, long object)
{
	return zone != NONE && (bsp == NONE || object_get_regional_bsp(object) == bsp) &&
		scenario_trigger_volume_test_object(zone, object);
}

static boolean extraction_in_hub_volume(short return_zone, long object)
{
	short faction;
	if (return_zone == NONE || !scenario_trigger_volume_test_object(return_zone, object) ||
		extraction_in_field_volume(extraction.zone, extraction.activate_name, object))
		return FALSE;
	for (faction = 0; faction < 2; faction++)
		if (extraction_in_field_volume(extraction.dropship_zones[faction],
			extraction.dropship_activate_names[faction], object)) return FALSE;
	return TRUE;
}

static boolean extraction_in_faction_hub(short faction, long object)
{
	short bsp = extraction.faction_hub_bsps[faction];

	return (bsp == NONE || object_get_regional_bsp(object) == bsp) &&
		extraction_in_hub_volume(extraction.faction_hubs[faction], object);
}

static boolean extraction_at_hub(long object)
{
	if (extraction.separate_hubs)
		return extraction_in_faction_hub(0, object) || extraction_in_faction_hub(1, object);
	return extraction_in_hub_volume(extraction.return_zone, object);
}

static boolean extraction_transport_at_return(short faction, long ship)
{
	/* Regional return pads belong to the faction's authored hub BSP. */
	short bsp = extraction.faction_hub_bsps[faction];

	return (bsp == NONE || object_get_regional_bsp(ship) == bsp) &&
		extraction_in_hub_volume(extraction_transport_return_zone(faction), ship);
}

static boolean extraction_transport_at_field(short faction, long ship)
{
	short zone = extraction_zone_for_faction(faction);
	short return_zone = extraction_transport_return_zone(faction);
	short call = extraction.dropship_zones[faction] != NONE ?
		extraction.dropship_activate_names[faction] : extraction.activate_name;
	short hub_bsp = extraction.faction_hub_bsps[faction];

	return extraction_in_field_volume(zone, call, ship) &&
		!extraction_in_owned_volume(extraction.return_zone, hub_bsp, ship) &&
		(!extraction.separate_hubs ||
		 (!extraction_in_owned_volume(extraction.faction_hubs[0], extraction.faction_hub_bsps[0], ship) &&
		  !extraction_in_owned_volume(extraction.faction_hubs[1], extraction.faction_hub_bsps[1], ship))) &&
		(return_zone == extraction.return_zone || !extraction_in_owned_volume(return_zone, hub_bsp, ship));
}

/* The compiled declaration opts in to field hover. It is never executed. */
static boolean extraction_bind_field_hover(struct scenario const *scenario, short faction)
{
	char const *name = faction ? "gs_spirit_hover_field" : "gs_pelican_hover_field";
	short index;
	extraction.dropship_hover_transfer[faction] = FALSE;
	for (index = 0; index < MIN(scenario->hs_scripts.count, SHORT_MAX); index++)
	{
		struct hs_script const *script = TAG_BLOCK_GET_ELEMENT(&scenario->hs_scripts, index, struct hs_script);
		if (strcmp(script->name, name)) continue;
		if (extraction.dropship_hover_transfer[faction] || script->script_type != _hs_script_static ||
			script->return_type != _hs_type_boolean || extraction.dropship_landing_groups[faction] == NONE ||
			extraction.dropship_names[faction] == NONE) return FALSE;
		extraction.dropship_hover_transfer[faction] = TRUE;
	}
	return TRUE;
}

/* Resolve the authored object in the scenario. A regional control's live
 * datum may be absent until the player reaches its BSP. */
static boolean extraction_control_placement_in_bsp(struct scenario_object_datum const *placement, short bsp)
{
	struct structure_bsp const *structure;

	if (bsp == NONE)
		return TRUE;
	if (!VALID_INDEX(bsp, MAXIMUM_STRUCTURE_BSPS_PER_SCENARIO))
		return FALSE;
	if (placement->on_bsp_flags)
		return placement->on_bsp_flags == FLAG(bsp);
	/* Native MCC control rows can leave their BSP mask unset. Check the
	 * placement against the hub's scene without switching the host view. */
	structure = scenario_structure_bsp_try_and_get(bsp);
	if (!structure)
		structure = scenario_structure_bsp_preview(bsp, TRUE);
	return structure && scenario_structure_bsp_contains_placement(structure, &placement->position);
}

static boolean extraction_authored_control(short index, short volume, short bsp)
{
	struct scenario_object_name const *name;
	struct scenario_object_datum const *placement;
	struct scenario_device_datum const *device;
	struct scenario_object_palette_entry const *entry;
	struct tag_block *placements, *palette;
	char const *path;
	long size, definition;
	short faction, squad;

	if (!VALID_INDEX(index, global_scenario->object_names.count) || volume == NONE) return FALSE;
	name = TAG_BLOCK_GET_ELEMENT(&global_scenario->object_names, index, struct scenario_object_name);
	if (name->runtime_object_type != _object_type_control) return FALSE;
	placements = scenario_get_object_type_scenario_datums(global_scenario, _object_type_control, &size);
	palette = scenario_get_object_type_scenario_palette(global_scenario, _object_type_control);
	if (!VALID_INDEX(name->runtime_scenario_datum_index, placements->count)) return FALSE;
	placement = tag_block_get_element_with_size(placements, name->runtime_scenario_datum_index, size);
	if (placement->name_index != index ||
		!extraction_control_placement_in_bsp(placement, bsp) ||
		TEST_FLAG(placement->placement_flags, _scenario_object_placement_not_automatic_bit) ||
		!VALID_INDEX(placement->palette_entry_index, palette->count) ||
		!scenario_trigger_volume_test_point(volume, &placement->position)) return FALSE;
	entry = TAG_BLOCK_GET_ELEMENT(palette, placement->palette_entry_index, struct scenario_object_palette_entry);
	device = control_scenario_device_data((struct scenario_control_datum const *)placement);
	for (faction = 0; faction < 2; faction++)
	{
		short groups[] = {extraction.dropship_boarding_groups[faction], extraction.dropship_landing_groups[faction],
			extraction.dropship_call_groups[faction]};
		short group;
		for (group = 0; group < NUMBEROF(groups); group++)
			if (groups[group] != NONE && (device->power_group_index == groups[group] ||
				device->position_group_index == groups[group])) return FALSE;
	}
	for (squad = 0; squad < NUMBEROF(extraction.squads); squad++)
		if (extraction.squads[squad].ready_group != NONE &&
			(device->power_group_index == extraction.squads[squad].ready_group ||
			 device->position_group_index == extraction.squads[squad].ready_group)) return FALSE;
	definition = entry->reference.index;
	path = definition != NONE ? tag_get_name(definition) : NULL;
	return path && tag_loaded(CONTROL_DEFINITION_TAG, path) == definition &&
		control_definition_get(definition)->control.triggers_when == 0;
}

/* A regional aircraft need not have a live object until its BSP is occupied. */
static boolean extraction_authored_dropship(short index, short home_bsp)
{
	struct scenario_object_name const *name;
	struct tag_block *placements, *palette;
	struct scenario_object_datum const *placement;
	struct scenario_object_palette_entry const *entry;
	char const *path;
	long size;

	if (!VALID_INDEX(index, global_scenario->object_names.count)) return FALSE;
	name = TAG_BLOCK_GET_ELEMENT(&global_scenario->object_names, index, struct scenario_object_name);
	if (name->runtime_object_type != _object_type_vehicle) return FALSE;
	placements = scenario_get_object_type_scenario_datums(global_scenario, _object_type_vehicle, &size);
	if (!VALID_INDEX(name->runtime_scenario_datum_index, placements->count)) return FALSE;
	placement = tag_block_get_element_with_size(placements, name->runtime_scenario_datum_index, size);
	palette = scenario_get_object_type_scenario_palette(global_scenario, _object_type_vehicle);
	if (placement->name_index != index || !VALID_INDEX(placement->palette_entry_index, palette->count)) return FALSE;
	/* Regional named creation follows the compiled single-BSP mask. */
	if (!VALID_INDEX(home_bsp, MAXIMUM_STRUCTURE_BSPS_PER_SCENARIO) ||
		!VALID_INDEX(home_bsp, global_scenario->structure_bsp_references.count) ||
		placement->on_bsp_flags != FLAG(home_bsp)) return FALSE;
	entry = TAG_BLOCK_GET_ELEMENT(palette, placement->palette_entry_index, struct scenario_object_palette_entry);
	path = entry->reference.index != NONE ? tag_get_name(entry->reference.index) : NULL;
	return path && tag_loaded(VEHICLE_DEFINITION_TAG, path) == entry->reference.index;
}

/* Both regional flight controllers depend on these host-published signals.
 * Solo maps may still use their existing optional signals or timed routes. */
static boolean extraction_regional_transport_bound(short faction)
{
	return extraction.dropship_boarding_groups[faction] != NONE &&
		extraction.dropship_call_groups[faction] != NONE &&
		extraction.dropship_landing_groups[faction] != NONE &&
		extraction.dropship_zones[faction] != NONE &&
		extraction.dropship_activate_names[faction] != NONE &&
		extraction_authored_dropship(extraction.dropship_names[faction], extraction.faction_hub_bsps[faction]);
}

static boolean extraction_validate_native_starts(boolean regional)
{
	short index, faction;
	unsigned short populated_hubs = 0;

	extraction.faction_hub_bsps[0] = extraction.faction_hub_bsps[1] = NONE;
	if (global_scenario->players.count <= 0 || global_scenario->players.count > SHORT_MAX ||
		(regional && !extraction.separate_hubs))
		return FALSE;

	for (index = 0; index < global_scenario->players.count; index++)
	{
		struct player_starting_location const *start = TAG_BLOCK_GET_ELEMENT(&global_scenario->players,
			index, struct player_starting_location);
		unsigned short hubs = extraction_start_hubs(start);

		if (hubs == (FLAG(0) | FLAG(1)))
			return FALSE;
		if (regional)
		{
			short bsp = start->structure_bsp_reference_index;
			if (!hubs)
				continue; /* Ordinary multiplayer starts are outside the operation. */
			faction = hubs == FLAG(0) ? 0 : 1;
			if (!VALID_INDEX(bsp, global_scenario->structure_bsp_references.count) ||
				(extraction.faction_hub_bsps[faction] != NONE && extraction.faction_hub_bsps[faction] != bsp))
				return FALSE;
			extraction.faction_hub_bsps[faction] = bsp;
		}
		populated_hubs |= hubs;
		if ((extraction.separate_hubs ? !hubs :
			 !scenario_trigger_volume_test_point(extraction.return_zone, &start->position)) ||
			(extraction.zone != NONE && scenario_trigger_volume_test_point(extraction.zone, &start->position)))
			return FALSE;
		for (faction = 0; faction < 2; faction++)
			if (extraction.dropship_zones[faction] != NONE &&
				scenario_trigger_volume_test_point(extraction.dropship_zones[faction], &start->position))
				return FALSE;
	}
	return (extraction.faction_hubs[0] == NONE && extraction.faction_hubs[1] == NONE) ||
		(populated_hubs == (FLAG(0) | FLAG(1)) &&
		 extraction.faction_names[0] != NONE && extraction.faction_names[1] != NONE);
}

static void extraction_bind(void)
{
	short index, faction, squad;
	boolean unique = TRUE;
	struct control_datum *deploy = NULL, *activate = NULL, *loadout = NULL, *debrief = NULL;
	struct control_datum *choices[2] = {NULL, NULL};
	struct control_datum *faction_calls[2] = {NULL, NULL};

	extraction.bound = TRUE;
	extraction.admitted = FALSE;
	extraction.separate_hubs = FALSE;
	extraction.evacuation = FALSE;
	extraction.lan_session_gear = !config_boolean("network.online");
#ifdef HALO_GAME_BROWSER
	extraction.lan_session_gear = extraction.lan_session_gear && !delta_peer_game_on_delta();
#endif
	for (squad = 0; squad < NUMBEROF(extraction.squads); squad++)
		extraction.squads[squad].name = extraction.squads[squad].ready_group = NONE;
	extraction.deploy_name = extraction.activate_name = extraction.loadout_name = extraction.debrief_name = NONE;
	extraction.dropship_names[0] = extraction.dropship_names[1] = NONE;
	extraction.dropship_activate_names[0] = extraction.dropship_activate_names[1] = NONE;
	extraction.dropship_zones[0] = extraction.dropship_zones[1] = NONE;
	extraction.dropship_return_zones[0] = extraction.dropship_return_zones[1] = NONE;
	extraction.dropship_boarding_groups[0] = extraction.dropship_boarding_groups[1] = NONE;
	extraction.dropship_call_groups[0] = extraction.dropship_call_groups[1] = NONE;
	extraction.dropship_landing_groups[0] = extraction.dropship_landing_groups[1] = NONE;
	extraction.dropship_evacuating[0] = extraction.dropship_evacuating[1] = NONE;
	extraction.dropship_banked[0] = extraction.dropship_banked[1] = NONE;
	extraction.dropship_completed[0] = extraction.dropship_completed[1] = NONE;
	extraction.faction_names[0] = extraction.faction_names[1] = NONE;
	extraction.faction_profiles[0] = extraction.faction_profiles[1] = NONE;
	extraction.faction_hubs[0] = extraction.faction_hubs[1] = NONE;
	memset(extraction.hub_controls, NONE, sizeof(extraction.hub_controls));
	extraction.faction_bipeds[0] = extraction.faction_bipeds[1] = NONE;
	extraction.zone = extraction.return_zone = NONE;
	if (!extraction.survival && extraction_host_firefight_requested())
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
		if (!extraction.survival)
			for (squad = 0; squad < NUMBEROF(extraction.squads); squad++)
			{
				char choice[32];
				snprintf(choice, sizeof(choice), "extraction_squad_%02d", squad + 1);
				if (!strcmp(name->name, choice))
				{
					if (extraction.squads[squad].name != NONE) unique = FALSE;
					extraction.squads[squad].name = index;
				}
			}

		for (faction = 0; faction < 2; faction++)
		{
			short service;
			for (service = 0; service < NUMBER_OF_EXTRACTION_HUB_CONTROLS; service++)
				if (!extraction.survival && !strcmp(name->name, extraction_hub_control_names[faction][service]))
				{
					if (extraction.hub_controls[faction][service] != NONE) unique = FALSE;
					extraction.hub_controls[faction][service] = index;
					extraction.separate_hubs = TRUE;
				}
			if (!strcmp(name->name, faction ? "extraction_covenant_body" : "extraction_unsc_body"))
			{
				long size;
				struct tag_block *placements, *palette;
				struct scenario_object_datum *body;
				struct scenario_object_palette_entry *entry;
				char const *path;
				if (extraction.faction_bipeds[faction] != NONE || name->runtime_object_type != _object_type_biped)
				{ unique = FALSE; continue; }
				placements = scenario_get_object_type_scenario_datums(global_scenario, _object_type_biped, &size);
				palette = scenario_get_object_type_scenario_palette(global_scenario, _object_type_biped);
				if (!VALID_INDEX(name->runtime_scenario_datum_index, placements->count)) { unique = FALSE; continue; }
				body = tag_block_get_element_with_size(placements, name->runtime_scenario_datum_index, size);
				if (body->name_index != index || !TEST_FLAG(body->placement_flags, _scenario_object_placement_not_automatic_bit) ||
					!VALID_INDEX(body->palette_entry_index, palette->count)) { unique = FALSE; continue; }
				entry = TAG_BLOCK_GET_ELEMENT(palette, body->palette_entry_index, struct scenario_object_palette_entry);
				if (entry->reference.index == NONE) { unique = FALSE; continue; }
				path = tag_get_name(entry->reference.index);
				if (!path || tag_loaded(BIPED_DEFINITION_TAG, path) != entry->reference.index) { unique = FALSE; continue; }
				extraction.faction_bipeds[faction] = entry->reference.index;
				extraction.faction_variants[faction] = body->variant_number;
			}
			if (!strcmp(name->name, faction ? "extraction_covenant" : "extraction_unsc"))
			{
				if (extraction.faction_names[faction] != NONE) unique = FALSE;
				extraction.faction_names[faction] = index;
			}
			if (!strcmp(name->name, faction ? "extraction_spirit_activate" : "extraction_pelican_activate"))
			{
				if (extraction.dropship_activate_names[faction] != NONE) unique = FALSE;
				extraction.dropship_activate_names[faction] = index;
			}
			if (!strcmp(name->name, faction ? "extraction_spirit" : "extraction_pelican"))
			{
				if (extraction.dropship_names[faction] != NONE || name->runtime_object_type != _object_type_vehicle)
					unique = FALSE;
				extraction.dropship_names[faction] = index;
			}
		}

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
		else if (!strcmp(name->name, "extraction_debrief"))
		{
			if (extraction.debrief_name != NONE) unique = FALSE;
			extraction.debrief_name = index;
		}
		else if (!strcmp(name->name, "extraction_activate"))
		{
			if (extraction.activate_name != NONE)
				unique = FALSE;
			extraction.activate_name = index;
		}
	}
	for (faction = 0; faction < 2; faction++)
	{
		if (!device_group_find_named(faction ? "extraction_spirit_boarded" : "extraction_pelican_boarded",
			&extraction.dropship_boarding_groups[faction]) ||
			(extraction.dropship_boarding_groups[faction] != NONE && extraction.dropship_names[faction] == NONE)) unique = FALSE;
		if (!device_group_find_named(faction ? "extraction_spirit_landed" : "extraction_pelican_landed",
			&extraction.dropship_landing_groups[faction]) ||
			(extraction.dropship_landing_groups[faction] != NONE && extraction.dropship_names[faction] == NONE)) unique = FALSE;
		if (!extraction_bind_field_hover(global_scenario, faction)) unique = FALSE;
		if (!device_group_find_named(faction ? "extraction_spirit_called" : "extraction_pelican_called",
			&extraction.dropship_call_groups[faction]) ||
			(extraction.dropship_call_groups[faction] != NONE && extraction.dropship_names[faction] == NONE)) unique = FALSE;
	}
	if (!extraction.survival)
		for (squad = 0; squad < NUMBEROF(extraction.squads); squad++)
		{
			char group[32];
			snprintf(group, sizeof(group), "extraction_squad_%02d_ready", squad + 1);
			if (!device_group_find_named(group, &extraction.squads[squad].ready_group) ||
				(extraction.squads[squad].ready_group != NONE && extraction.squads[squad].name == NONE)) unique = FALSE;
		}
	if (extraction.faction_names[0] != NONE || extraction.faction_names[1] != NONE)
	{
		if (global_scenario->starting_profiles.count > SHORT_MAX) goto finished;
		for (index = 0; index < global_scenario->starting_profiles.count; index++)
		{
			struct scenario_starting_profile *profile = TAG_BLOCK_GET_ELEMENT(
				&global_scenario->starting_profiles, index, struct scenario_starting_profile);
			for (faction = 0; faction < 2; faction++)
				if (!strcmp(profile->name, faction ? "extraction_covenant" : "extraction_unsc"))
				{
					if (extraction.faction_profiles[faction] != NONE) unique = FALSE;
					extraction.faction_profiles[faction] = index;
				}
		}
	}
	for (index = 0; index < global_scenario->trigger_volumes.count; index++)
	{
		struct scenario_trigger_volume *volume = TAG_BLOCK_GET_ELEMENT(
			&global_scenario->trigger_volumes, index, struct scenario_trigger_volume);

		for (faction = 0; faction < 2; faction++)
		{
			if (!extraction.survival &&
				!strcmp(volume->name, faction ? "extraction_covenant_hub" : "extraction_unsc_hub"))
			{
				if (extraction.faction_hubs[faction] != NONE || volume->type != _scenario_trigger_volume_type_oriented)
					unique = FALSE;
				extraction.faction_hubs[faction] = index;
			}
			else if (!strcmp(volume->name, faction ? "extraction_spirit_zone" : "extraction_pelican_zone"))
			{
				if (extraction.dropship_zones[faction] != NONE) unique = FALSE;
				extraction.dropship_zones[faction] = index;
			}
			else if (extraction.dropship_names[faction] != NONE &&
				!strcmp(volume->name, faction ? "spirit_return" : "pelican_return"))
			{
				if (extraction.dropship_return_zones[faction] != NONE) unique = FALSE;
				extraction.dropship_return_zones[faction] = index;
			}
		}

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
	debrief = extraction.debrief_name != NONE ? control_try_and_get(object_index_from_name_index(extraction.debrief_name)) : NULL;
	/* Match Spark's native touched-by-player tag requirement. Other trigger
	   kinds never reach extraction_control_touched through control_touched. */
	extraction.admitted = unique && extraction_validate_native_starts(scenario_uses_regional_gameplay(global_scenario));
	if (extraction.separate_hubs)
	{
		if (extraction.deploy_name != NONE || extraction.loadout_name != NONE || extraction.debrief_name != NONE)
			extraction.admitted = FALSE;
		for (faction = 0; faction < 2; faction++)
		{
			short service;
			for (service = 0; service < NUMBER_OF_EXTRACTION_HUB_CONTROLS; service++)
			{
				short service_name = extraction.hub_controls[faction][service];
				if ((service == _extraction_hub_deploy_control || service_name != NONE) &&
					!extraction_authored_control(service_name, extraction.faction_hubs[faction], extraction.faction_hub_bsps[faction]))
					extraction.admitted = FALSE;
			}
			if (extraction.dropship_names[faction] != NONE && extraction.dropship_return_zones[faction] == NONE)
				extraction.admitted = FALSE;
		}
	}
	else
		extraction.admitted = extraction.admitted && extraction.return_zone != NONE && deploy &&
			control_definition_get(deploy->definition_index)->control.triggers_when == 0;
	if (extraction.activate_name != NONE || extraction.zone != NONE)
	{
		if (extraction.zone == NONE || !activate || activate == deploy ||
			control_definition_get(activate->definition_index)->control.triggers_when != 0) extraction.admitted = FALSE;
	}
	else if (extraction.dropship_names[0] == NONE && extraction.dropship_names[1] == NONE)
		extraction.admitted = FALSE;
	for (faction = 0; faction < 2; faction++)
	{
		short call_name = extraction.dropship_activate_names[faction];
		short zone = extraction.dropship_zones[faction];
		if (call_name != NONE)
			faction_calls[faction] = control_try_and_get(object_index_from_name_index(call_name));
		if (extraction.dropship_names[faction] != NONE && !activate && call_name == NONE)
			extraction.admitted = FALSE;
		if (call_name != NONE || zone != NONE)
		{
			struct control_datum *call = faction_calls[faction];
			if (extraction.separate_hubs)
			{
				if (extraction.dropship_names[faction] == NONE || !extraction_authored_control(call_name, zone, NONE))
					extraction.admitted = FALSE;
			}
			else if (!call || call->object.name_index != call_name || zone == NONE || extraction.dropship_names[faction] == NONE ||
				call == deploy || call == activate || call == loadout || call == debrief ||
				(faction && call == faction_calls[0]) ||
				control_definition_get(call->definition_index)->control.triggers_when != 0 ||
				!scenario_trigger_volume_test_point(zone, &call->object.position)) extraction.admitted = FALSE;
		}
	}
	if (extraction.loadout_name != NONE && (!loadout || loadout == deploy || loadout == activate ||
		control_definition_get(loadout->definition_index)->control.triggers_when != 0)) extraction.admitted = FALSE;
	if (extraction.debrief_name != NONE && (!debrief || debrief == deploy || debrief == activate || debrief == loadout ||
		control_definition_get(debrief->definition_index)->control.triggers_when != 0)) extraction.admitted = FALSE;
	for (faction = 0; faction < 2; faction++)
		if (extraction.faction_names[faction] != NONE)
		{
			choices[faction] = control_try_and_get(object_index_from_name_index(extraction.faction_names[faction]));
			if (extraction.separate_hubs)
			{
				if (extraction.faction_profiles[faction] == NONE ||
					!extraction_authored_control(extraction.faction_names[faction], extraction.faction_hubs[faction], extraction.faction_hub_bsps[faction]))
					extraction.admitted = FALSE;
			}
			else if (!choices[faction] || choices[faction] == deploy || choices[faction] == activate || choices[faction] == loadout || choices[faction] == debrief ||
				choices[faction] == faction_calls[0] || choices[faction] == faction_calls[1] ||
				(faction && choices[1] == choices[0]) || extraction.faction_profiles[faction] == NONE ||
				control_definition_get(choices[faction]->definition_index)->control.triggers_when != 0) extraction.admitted = FALSE;
		}
	if (global_scenario->type != _scenario_type_solo)
	{
		for (faction = 0; faction < 2; faction++)
			if (extraction.faction_bipeds[faction] == NONE || !extraction_regional_transport_bound(faction))
				extraction.admitted = FALSE;
	}
	/* Optional character templates share the existing faction controls/profiles. */
	for (faction = 0; faction < 2; faction++)
		if (extraction.faction_bipeds[faction] != NONE &&
			(extraction.faction_names[faction] == NONE || extraction.faction_profiles[faction] == NONE)) extraction.admitted = FALSE;
finished:
	if (extraction.admitted)
		for (squad = 0; squad < NUMBEROF(extraction.squads); squad++)
			if (extraction.squads[squad].name != NONE)
			{
				struct control_datum *choice = control_try_and_get(object_index_from_name_index(extraction.squads[squad].name));
				short ready = extraction.squads[squad].ready_group;
				short other;
				if (!choice || choice->object.name_index != extraction.squads[squad].name ||
					choice == deploy || choice == activate || choice == loadout || choice == debrief ||
					choice == choices[0] || choice == choices[1] || choice == faction_calls[0] || choice == faction_calls[1] ||
					control_definition_get(choice->definition_index)->control.triggers_when != 0)
				{ extraction.admitted = FALSE; continue; }
				for (other = 0; other < squad; other++)
					if (extraction.squads[other].name != NONE &&
						object_index_from_name_index(extraction.squads[other].name) == object_index_from_name_index(extraction.squads[squad].name))
						extraction.admitted = FALSE;
				if (ready != NONE)
				{
					struct control_datum *inputs[] = {deploy, activate, loadout, debrief, choices[0], choices[1], faction_calls[0], faction_calls[1]};
					for (other = 0; other < NUMBEROF(inputs); other++)
						if (inputs[other] && (inputs[other]->device.position_group_index == ready ||
							inputs[other]->device.power_group_index == ready)) extraction.admitted = FALSE;
					for (other = 0; other < NUMBEROF(extraction.squads); other++)
					{
						struct control_datum *input = extraction.squads[other].name != NONE ?
							control_try_and_get(object_index_from_name_index(extraction.squads[other].name)) : NULL;
						if ((other != squad && ready == extraction.squads[other].ready_group) ||
							(input && (input->device.position_group_index == ready || input->device.power_group_index == ready)))
							extraction.admitted = FALSE;
					}
				}
				for (faction = 0; faction < 2; faction++)
				{
					short outputs[3] = {extraction.dropship_boarding_groups[faction], extraction.dropship_call_groups[faction],
						extraction.dropship_landing_groups[faction]};
					for (other = 0; other < NUMBEROF(outputs); other++)
						if (outputs[other] != NONE && (ready == outputs[other] || choice->device.position_group_index == outputs[other] ||
							choice->device.power_group_index == outputs[other])) extraction.admitted = FALSE;
				}
			}
	if (extraction.admitted)
		for (faction = 0; faction < 2; faction++)
		{
			short outputs[3] = {extraction.dropship_boarding_groups[faction], extraction.dropship_call_groups[faction],
				extraction.dropship_landing_groups[faction]};
			short output;
			for (output = 0; output < NUMBEROF(outputs); output++)
			if (outputs[output] != NONE)
			{
				struct control_datum *inputs[] = {deploy, activate, loadout, debrief, choices[0], choices[1], faction_calls[0], faction_calls[1]};
				for (index = 0; index < NUMBEROF(inputs); index++)
					if (inputs[index] && (inputs[index]->device.position_group_index == outputs[output] ||
						inputs[index]->device.power_group_index == outputs[output])) extraction.admitted = FALSE;
			}
		}
	if (extraction.admitted)
		for (faction = 0; faction < 2; faction++)
		{
			if (extraction.dropship_boarding_groups[faction] != NONE)
				device_group_set_actual_value(extraction.dropship_boarding_groups[faction], 0.f);
			if (extraction.dropship_call_groups[faction] != NONE)
				device_group_set_actual_value(extraction.dropship_call_groups[faction], 0.f);
			if (extraction.dropship_landing_groups[faction] != NONE)
				device_group_set_actual_value(extraction.dropship_landing_groups[faction], 0.f);
		}
	if (extraction.admitted)
		for (squad = 0; squad < NUMBEROF(extraction.squads); squad++)
			if (extraction.squads[squad].ready_group != NONE)
				device_group_set_actual_value(extraction.squads[squad].ready_group, 0.f);
	platform_log("extraction: native hub %s; deploy %d activate %d return %d zone %d pelican %d spirit %d",
		extraction.admitted ? "admitted" : "unavailable", extraction.deploy_name,
		extraction.activate_name, extraction.return_zone, extraction.zone,
		extraction.dropship_names[0], extraction.dropship_names[1]);
	if (!extraction.admitted)
	{
		platform_log("extraction: rejected hub bindings unique %d loadout %d debrief %d starts %ld",
			unique, loadout != NULL, debrief != NULL, global_scenario->players.count);
		for (faction = 0; faction < 2; faction++)
			platform_log("extraction: faction %d choice %d profile %d body %08lx boarded %d called %d landed %d",
				faction, choices[faction] != NULL, extraction.faction_profiles[faction],
				(unsigned long)extraction.faction_bipeds[faction], extraction.dropship_boarding_groups[faction],
				extraction.dropship_call_groups[faction], extraction.dropship_landing_groups[faction]);
	}
	if (extraction.survival)
	{
		/* Reuse the native controls, volumes and settlement. Finite encounters
		 * retain their old recovery without a hub. Endless play must not
		 * reserve gear without a real escape interface. */
		extraction.evacuation = extraction.admitted;
		if (!extraction.admitted && !coop_enemies_firefight_endless())
		{
			extraction.admitted = TRUE;
			extraction.deploy_name = extraction.activate_name = extraction.loadout_name = extraction.debrief_name = NONE;
			extraction.faction_names[0] = extraction.faction_names[1] = NONE;
			extraction.faction_profiles[0] = extraction.faction_profiles[1] = NONE;
			extraction.faction_bipeds[0] = extraction.faction_bipeds[1] = NONE;
			extraction.dropship_boarding_groups[0] = extraction.dropship_boarding_groups[1] = NONE;
			extraction.dropship_call_groups[0] = extraction.dropship_call_groups[1] = NONE;
			extraction.dropship_landing_groups[0] = extraction.dropship_landing_groups[1] = NONE;
			extraction.zone = extraction.return_zone = NONE;
			extraction.dropship_activate_names[0] = extraction.dropship_activate_names[1] = NONE;
			extraction.dropship_zones[0] = extraction.dropship_zones[1] = NONE;
			extraction.dropship_return_zones[0] = extraction.dropship_return_zones[1] = NONE;
		}
		if (extraction.admitted && !extraction.suspended)
			extraction_notice(extraction.evacuation ?
				"SURVIVAL GEAR | Escape through activation/return points, or complete a finite encounter alive. Deploy at the hub to rejoin." :
				"SURVIVAL GEAR | Compatible saved gear is at risk. Complete the finite encounter alive to recover loot.");
		else if (!extraction.admitted)
			extraction_notice("SURVIVAL GEAR UNAVAILABLE | Endless recovery needs unique native hub controls and extraction/return zones. No stash reserved.");
		return;
	}
	if (extraction.admitted && !extraction.suspended && global_scenario->type == _scenario_type_solo)
		extraction_notice("HUB | Use the deployment control to begin. No raid timer.");
	else if (!extraction.admitted)
		extraction_notice("Extraction unavailable: check native hub controls/zones, starts, dropships and optional faction controls/profiles.");
}


static char const *extraction_item_name(char const *path)
{
	char const *slash = strrchr(path, '\\'), *other = strrchr(path, '/');
	if (!slash || (other && other > slash)) slash = other;
	return slash ? slash + 1 : path;
}

static void extraction_debrief(struct extraction_run *run)
{
	struct extraction_report const *report = &run->report;
	char lines[4][58]; /* Keep the existing remote controller prefix within 62 characters. */
	short page, pages, report_pages, index;
	struct player_datum *player = player_try_and_get(run->player);
	boolean reverse = player && player->local_player_index >= 0 && player->local_player_index < MAXIMUM_NUMBER_OF_LOCAL_PLAYERS &&
		!network_chat_notice_visible();
	/* Retry only an already published save, without advancing its selection.
	 * A preview may have expired while its confirmed deletion awaited flush. */
	if (run->gear.pending) (void)gear_loadout_choice(&run->gear);
	if (run->gear.pending || run->phase == _extraction_settling || run->phase == _extraction_losing ||
		run->phase == _extraction_cancelling)
	{
		extraction_tell(run, "DEBRIEF PENDING | Settlement must finish before results.");
		return;
	}
	run->stash_preview = FALSE;
	report_pages = report->ready ? report->weapon_count + 2 : 1;
	pages = report_pages + run->gear.record.count;
	page = run->report_page % pages;
	memset(lines, 0, sizeof(lines));
	if (page >= report_pages)
	{
		struct gear_item const *item = &run->gear.record.items[page - report_pages];
		snprintf(lines[0], sizeof(lines[0]), "STASH %d/%d | %d/%d weapons stored",
			(int)(page - report_pages + 1), (int)run->gear.record.count, (int)run->gear.record.count, GEAR_CAPACITY);
		snprintf(lines[1], sizeof(lines[1]), "Weapon: %.45s", extraction_item_name(item->equipment.tag_path));
		snprintf(lines[2], sizeof(lines[2]), "%s", gear_item_available(&run->gear, item) ?
			"Compatible with this map and body." : "Unavailable here; safely retained in your stash.");
		snprintf(lines[3], sizeof(lines[3]), "%s", run->gear.record.phase == _gear_idle ?
			"Use: next | Crouch-use twice: confirm discard" : "Use: next | Finish this run before discarding");
		memcpy(run->discard_id, item->id, GEAR_ID_BYTES);
		run->discard_generation = run->gear.record.generation;
		run->discard_unit = player ? player->unit_index : NONE;
		run->stash_preview = TRUE;
	}
	else if (!report->ready)
	{
		snprintf(lines[0], sizeof(lines[0]), "DEBRIEF | %s",
			run->gear.record.outcome == _gear_escaped ? "LAST RUN RECOVERED" :
			run->gear.record.outcome == _gear_lost ? "LAST RUN LOST" : "NO COMPLETED RUN");
		snprintf(lines[1], sizeof(lines[1]), "Current stash: %d/%d weapons | %d frag | %d plasma",
			(int)run->gear.record.count, GEAR_CAPACITY, (int)run->gear.record.grenades[0], (int)run->gear.record.grenades[1]);
		snprintf(lines[2], sizeof(lines[2]), "%s", run->gear.record.outcome == _gear_no_outcome ?
			"No completed operation yet. Deploy from the hub." : "Previous-session item details are unavailable.");
		snprintf(lines[3], sizeof(lines[3]), "Use: browse stash | Loadout: prepare your next run");
	}
	else
	{
		run->report.announced = TRUE;
		snprintf(lines[0], sizeof(lines[0]), "DEBRIEF %d/%d | LAST RUN %s", (int)page + 1, (int)report_pages,
			report->outcome == _gear_escaped ? "RECOVERED" : "PERSONAL GEAR LOST");
		if (!page)
		{
			snprintf(lines[1], sizeof(lines[1]), "%s: %d weapons | %d frag | %d plasma",
				report->outcome == _gear_escaped ? "Secured" : "Owned loadout lost",
				(int)report->weapon_count, (int)report->grenades[0], (int)report->grenades[1]);
			snprintf(lines[2], sizeof(lines[2]), "Current stash: %d/%d weapons", (int)run->gear.record.count, GEAR_CAPACITY);
		}
		else if (page <= report->weapon_count)
		{
			struct game_modes_inventory_weapon const *weapon = &report->weapons[page - 1];
			long definition = tag_loaded(WEAPON_DEFINITION_TAG, weapon->tag_path);
			snprintf(lines[1], sizeof(lines[1]), "Weapon: %.45s", extraction_item_name(weapon->tag_path));
			/* Native plasma weapons can retain a placeholder magazine. Use the
			 * same age-limited flag that native firing checks for battery depletion. */
			if (definition != NONE &&
				TEST_FLAG(weapon_definition_get(definition)->weapon.flags, _weapon_cannot_fire_at_maximum_age_bit))
				snprintf(lines[2], sizeof(lines[2]), "%s battery: %d%%",
					report->outcome == _gear_escaped ? "Recovered" : "Lost",
					(int)PIN((1.0f - weapon->age) * 100.0f, 0.0f, 100.0f));
			else
				snprintf(lines[2], sizeof(lines[2]), "%s ammo: %d/%d loaded, %d/%d spare",
					report->outcome == _gear_escaped ? "Recovered" : "Lost",
					(int)weapon->rounds_loaded[0], (int)weapon->rounds_loaded[1],
					(int)weapon->rounds_reserve[0], (int)weapon->rounds_reserve[1]);
		}
		else if (report->statistics)
		{
			snprintf(lines[1], sizeof(lines[1]), "Personal eliminations: %d | Deaths: %d", (int)report->kills, (int)report->deaths);
			snprintf(lines[2], sizeof(lines[2]), "Your equipment and combat results for this operation.");
		}
		else
		{
			snprintf(lines[1], sizeof(lines[1]), "Combat totals unavailable for this run.");
			snprintf(lines[2], sizeof(lines[2]), "No combat totals are inferred from recovered equipment.");
		}
		snprintf(lines[3], sizeof(lines[3]), "Use: next page/stash | Loadout terminal: prepare run");
	}
	run->report_page = (page + 1) % pages;
	/* The inherited HUD and chat backends have opposite record ordering. */
	for (index = 0; index < 4; index++) extraction_tell(run, lines[reverse ? 3 - index : index]);
}

static boolean extraction_loadout(struct extraction_run *run, struct game_modes_inventory *loadout,
	boolean bring_stash, short *weapons_to_reserve)
{
	struct player_datum *player;
	struct unit_datum *unit;
	short faction;
	if (!gear_loadout(&run->gear, loadout, bring_stash)) return FALSE;
	if (weapons_to_reserve)
		*weapons_to_reserve = run->gear.record.phase == _gear_idle ? loadout->weapon_count : 0;
	if (extraction.faction_names[0] == NONE && extraction.faction_names[1] == NONE) return TRUE;
	player = player_try_and_get(run->player);
	unit = player ? (struct unit_datum *)object_try_and_get_and_verify_type(player->unit_index, _object_mask_biped) : NULL;
	if (!unit || unit->unit.player_index != run->player) return FALSE;
	faction = extraction_faction(unit->object.owner_team_index);
	return faction != NONE && extraction.faction_names[faction] != NONE &&
		game_modes_inventory_starting_profile(extraction.faction_profiles[faction], GEAR_LOADOUT_WEAPONS,
			run->gear.grenade_room, loadout);
}

static void extraction_loadout_notice(struct extraction_run *run)
{
	struct game_modes_inventory loadout;
	char text[128];
	if (!game_modes_inventory_grenade_room(run->player, run->gear.grenade_room) || !extraction_loadout(run, &loadout, TRUE, NULL))
	{
		extraction_block(run, "LOADOUT PENDING | Native equipment or stash unavailable. Retry at the terminal."); return;
	}
	run->blocked = FALSE;
	snprintf(text, sizeof(text), "LOADOUT | Stash %d/%d | %.18s / %.18s | +%d frag +%d plasma.",
		(int)run->gear.record.count, GEAR_CAPACITY,
		loadout.weapon_count > 0 ? extraction_item_name(loadout.weapons[0].tag_path) : "carried gear",
		loadout.weapon_count > 1 ? extraction_item_name(loadout.weapons[1].tag_path) : "-",
		(int)loadout.grenades[0], (int)loadout.grenades[1]);
	extraction_tell(run, text);
	extraction_tell(run, "Use: pair; Reload/Light-use: second; crouch-use: discard.");
	extraction_tell(run, "Deploy: Reload/Light-use keeps stash safe.");
}

static boolean extraction_squad_busy(short squad, struct extraction_run const *except)
{
	short index;
	if (!VALID_INDEX(squad, NUMBEROF(extraction.squads))) return FALSE;
	for (index = 0; index < NUMBEROF(extraction_runs); index++)
	{
		struct extraction_run const *member = &extraction_runs[index];
		if (member == except || member->squad != squad || !member->squad_operation ||
			member->squad_operation != extraction.squads[squad].operation) continue;
		/* A departed slot still belongs to this operation until the update
		 * closes its gear and records the outcome. */
		if (member->phase == _extraction_returned ? member->gear.pending :
			(member->phase != _extraction_hub && member->phase != _extraction_lost))
			return TRUE;
	}
	return FALSE;
}

static void extraction_squad_notice(short squad, char const *text)
{
	short index;
	for (index = 0; index < NUMBEROF(extraction_runs); index++)
		if (extraction_runs[index].squad == squad) extraction_tell(&extraction_runs[index], text);
}

static struct extraction_squad_choice *extraction_squad_choice(byte const owner[GEAR_ID_BYTES])
{
	short index;
	for (index = 0; index < NUMBEROF(extraction.squad_choices); index++)
		if (extraction.squad_choices[index].remembered &&
			!memcmp(extraction.squad_choices[index].owner, owner, GEAR_ID_BYTES))
			return &extraction.squad_choices[index];
	return NULL;
}

static boolean extraction_remember_squads(void)
{
#ifdef HALO_GAME_BROWSER
	return game_connection() == _game_connection_network_server && delta_peer_game_on_delta();
#else
	return FALSE;
#endif
}

static void extraction_remember_squad(struct extraction_run const *run, short faction)
{
	struct extraction_squad_choice *choice;
	short index;
	if (!extraction_remember_squads()) return;
	choice = extraction_squad_choice(run->owner);
	if (run->squad == NONE)
	{
		if (choice) choice->remembered = FALSE;
		return;
	}
	if (!choice)
	{
		for (index = 0; index < NUMBEROF(extraction.squad_choices); index++)
			if (!extraction.squad_choices[index].remembered) break;
		if (index == NUMBEROF(extraction.squad_choices))
		{
			index = extraction.next_squad_choice;
			extraction.next_squad_choice = (short)((index + 1) % NUMBEROF(extraction.squad_choices));
		}
		choice = &extraction.squad_choices[index];
		memset(choice, 0, sizeof(*choice));
	}
	memcpy(choice->owner, run->owner, GEAR_ID_BYTES);
	choice->squad = run->squad;
	choice->faction = faction;
	choice->remembered = TRUE;
}

static boolean extraction_squad_admit(struct extraction_run const *run, short squad, short faction, short *count)
{
	short index, members = 0;
	for (index = 0; index < NUMBEROF(extraction_runs); index++)
	{
		struct extraction_run const *member = &extraction_runs[index];
		struct player_datum *other;
		if (member == run || member->squad != squad) continue;
		other = player_try_and_get(member->player);
		if (!other || other->quit_out_of_game) continue;
		if (extraction_faction((short)other->team_index) != faction)
		{
			extraction_tell(run, "SQUAD FACTION MISMATCH | Join members of your faction or choose another squad."); return FALSE;
		}
		members++;
	}
	if (members >= EXTRACTION_SQUAD_MEMBERS)
	{
		extraction_tell(run, "SQUAD FULL | Six members maximum. Choose another squad."); return FALSE;
	}
	*count = members;
	return TRUE;
}

static boolean extraction_restore_squad(struct extraction_run *run, struct player_datum const *player)
{
	struct extraction_squad_choice const *choice;
	struct unit_datum const *unit;
	short members;
	if (run->squad != NONE || !extraction_remember_squads()) return TRUE;
	choice = extraction_squad_choice(run->owner);
	if (!choice) return TRUE;
	unit = (struct unit_datum const *)object_try_and_get_and_verify_type(player->unit_index, _object_mask_biped);
	if (!unit || extraction_faction((short)player->team_index) != choice->faction ||
		extraction_faction(unit->object.owner_team_index) != choice->faction)
	{
		extraction_tell(run, "SQUAD FACTION MISMATCH | Select your previous faction or choose another squad."); return FALSE;
	}
	if (extraction_squad_busy(choice->squad, run))
	{
		extraction_tell(run, "SQUAD IN FIELD | Wait for your squad to return or choose another squad."); return FALSE;
	}
	if (!extraction_squad_admit(run, choice->squad, choice->faction, &members)) return FALSE;
	run->squad = choice->squad;
	run->squad_operation = 0;
	extraction_tell(run, "SQUAD REJOINED | Your selection has been restored.");
	return TRUE;
}

static void extraction_squad_choose(struct extraction_run *run, struct player_datum const *player,
	short squad, boolean in_hub)
{
	short members;
	short faction = extraction_faction((short)player->team_index);
	char text[128];
	if (!in_hub || unit_get(player->unit_index)->object.parent_object_index != NONE ||
		(run->phase != _extraction_hub && run->phase != _extraction_lost && run->phase != _extraction_returned) ||
		extraction_squad_busy(run->squad, run) ||
		(squad != run->squad && extraction_squad_busy(squad, run)))
	{
		extraction_tell(run, "SQUAD LOCKED | Choose on foot at the hub before the squad deploys, or after its operation ends."); return;
	}
	if (!extraction_open_gear(run, player)) return;
	if (run->squad == squad)
	{
		run->squad = NONE; run->squad_operation = 0;
		extraction_remember_squad(run, faction);
		extraction_tell(run, "SQUAD LEFT | TEAM radio: faction. Gear stays yours. Choose a squad before deployment."); return;
	}
	if (faction == NONE || extraction_faction(unit_get(player->unit_index)->object.owner_team_index) != faction)
	{
		extraction_tell(run, "SQUAD UNAVAILABLE | Select a compatible native faction first."); return;
	}
	if (!extraction_squad_admit(run, squad, faction, &members)) return;
	run->squad = squad; run->squad_operation = 0;
	extraction_remember_squad(run, faction);
	platform_log("extraction: player %ld joined squad %d", DATUM_INDEX_TO_ABSOLUTE_INDEX(run->player), squad + 1);
	snprintf(text, sizeof(text), "SQUAD %02d | %d/6 members. TEAM radio: squad. Gear stays individually owned. Use again at hub to leave.",
		squad + 1, members + 1);
	extraction_squad_notice(squad, text);
}

static boolean extraction_squad_deployment_ready(struct extraction_run *run)
{
	short squad = run->squad;
	if (VALID_INDEX(squad, NUMBEROF(extraction.squads)) && extraction.squads[squad].name != NONE)
	{
		boolean busy = extraction_squad_busy(squad, run);

		if (busy && run->squad_operation == extraction.squads[squad].operation)
		{
			extraction_tell(run, "SQUAD IN FIELD | Wait for your current operation to finish before redeploying."); return FALSE;
		}
		/* A reconnect can finish the previous slot's escrow during this
		 * use action, before the host update reports that operation. */
		if (!busy && extraction.squads[squad].operation && !extraction.squads[squad].notified)
		{
			extraction_tell(run, "SQUAD REPORT PENDING | Retry deployment after the current operation settles."); return FALSE;
		}
		return TRUE;
	}
	for (squad = 0; squad < NUMBEROF(extraction.squads); squad++)
		if (extraction.squads[squad].name != NONE)
		{
			extraction_tell(run, "CHOOSE SQUAD | Use a squad terminal at the hub before deployment."); return FALSE;
		}
	return TRUE; /* Older maps retain their existing ungrouped operation. */
}

static boolean extraction_squad_objectives_ready(struct extraction_run const *run)
{
	short group;
	if (!VALID_INDEX(run->squad, NUMBEROF(extraction.squads))) return TRUE;
	group = extraction.squads[run->squad].ready_group;
	return group == NONE || (run->squad_operation && run->squad_operation == extraction.squads[run->squad].operation &&
		device_group_get_value(group) == 1.f);
}

static void extraction_set_player_team(struct player_datum *player, struct unit_datum *unit, short team)
{
	if (unit->unit.actor_index != NONE) actor_set_team(unit->unit.actor_index, team);
	unit->object.owner_team_index = team;
	player->team_index = team;
	ai_update_team_status();
}

static boolean extraction_wake_mission_script(char const *name, boolean reusable)
{
	short index;
	for (index = 0; index < MIN(global_scenario->hs_scripts.count, SHORT_MAX); index++)
	{
		struct hs_script const *script = TAG_BLOCK_GET_ELEMENT(&global_scenario->hs_scripts, index, struct hs_script);
		if (!strcmp(script->name, name) && script->return_type == _hs_type_void &&
			(script->script_type == _hs_script_dormant || (reusable && script->script_type == _hs_script_continuous)))
			return hs_wake_by_name(name);
	}
	return FALSE;
}

static boolean extraction_deploy(struct extraction_run *run, struct player_datum *player, boolean bring_stash)
{
	struct game_modes_inventory loadout;
	long created[GAME_MODES_INVENTORY_WEAPONS];
	short weapons_to_reserve, faction = extraction_faction((short)player->team_index);
	short starting_squad = NONE;
	if (extraction_medical_incapacitated(player->unit_index)) return FALSE;
	run->discard_deadline = 0;
	if (!extraction_open_gear(run, player) || !extraction_restore_squad(run, player) ||
		!extraction_squad_deployment_ready(run)) return FALSE;
	if (!game_modes_inventory_grenade_room(run->player, run->gear.grenade_room) ||
		!extraction_loadout(run, &loadout, bring_stash, &weapons_to_reserve) ||
		!game_modes_inventory_loadout_valid(&loadout))
	{
		extraction_block(run, "DEPLOYMENT PENDING | Select compatible gear and retry; no recovered equipment granted."); return FALSE;
	}
	/* Authored aircraft can hold the entire crew aboard until everyone banks.
	 * Leave room for the largest native inventory while stash controls are reachable. */
	if (VALID_INDEX(faction, 2) && extraction.dropship_boarding_groups[faction] != NONE)
	{
		int shortfall = run->gear.record.count - weapons_to_reserve + GAME_MODES_INVENTORY_WEAPONS - GEAR_CAPACITY;
		if (shortfall > 0)
		{
			char notice[128];
			snprintf(notice, sizeof(notice), "STASH FULL | Free %d stash slot%s at loadout or bring compatible stored gear.",
				shortfall, shortfall == 1 ? "" : "s");
			extraction_tell(run, notice);
			return FALSE;
		}
	}
	if (!gear_reserve(&run->gear, bring_stash))
	{
		extraction_block(run, "DEPLOYMENT PENDING | Select compatible gear and retry; no recovered equipment granted."); return FALSE;
	}
	if (!game_modes_inventory_apply(run->player, &loadout, created))
	{
		if (!gear_cancel_reservation(&run->gear)) run->phase = _extraction_cancelling;
		run->retry_tick = game_time_get();
		extraction_block(run, "DEPLOYMENT BLOCKED | Native loadout could not be placed; reserved gear is returning to the stash."); return FALSE;
	}
	if (!gear_confirm_deployment(&run->gear))
	{
		/* No tick/input occurs between grant and confirmation. Remove only
		 * this grant before refunding; never leave usable unissued equipment. */
		game_modes_inventory_revoke(player->unit_index, &loadout, created);
		if (!gear_cancel_reservation(&run->gear)) run->phase = _extraction_cancelling;
		run->retry_tick = game_time_get();
		extraction_block(run, "DEPLOYMENT PENDING | Equipment returned; stash confirmation failed. Retry after storage recovers.");
		return FALSE;
	}
	gear_track_reserved(&run->gear, created);
	/* Native campaign Player and Human are distinct teams. On a map with
	 * faction selection, the default UNSC run uses the same team as the
	 * UNSC terminal once the loadout has been durably issued. */
	if (!extraction.survival && extraction.faction_names[0] != NONE && player->team_index == _game_team_player)
	{
		struct unit_datum *unit = unit_get(player->unit_index);
		if (unit->object.owner_team_index == _game_team_player)
			extraction_set_player_team(player, unit, _game_team_human);
	}
	if (VALID_INDEX(run->squad, NUMBEROF(extraction.squads)))
	{
		if (!extraction_squad_busy(run->squad, run))
		{
			starting_squad = run->squad;
			if (!++extraction.squads[run->squad].operation) ++extraction.squads[run->squad].operation;
			extraction.squads[run->squad].notified = FALSE;
			extraction.squads[run->squad].banked = FALSE;
			extraction.squads[run->squad].unconfirmed_escape = FALSE;
			if (extraction.squads[run->squad].ready_group != NONE)
				device_group_set_actual_value(extraction.squads[run->squad].ready_group, 0.f);
		}
		run->squad_operation = extraction.squads[run->squad].operation;
	}
	run->start_kills = player->statistics.kills[0]; run->start_deaths = player->statistics.deaths;
	run->statistics_started = TRUE; run->report_page = 0;
	run->unit = player->unit_index; run->local_player = player->local_player_index;
	run->dropship = NONE;
	run->field_reached = FALSE;
	run->field_exit_ready = FALSE;
	run->phase = _extraction_deployed; run->activation_ticks = 0; run->blocked = FALSE; run->loadout_seen = FALSE;
	run->retry_tick = game_time_get();
	if (!extraction.survival && starting_squad != NONE)
	{
		char script[32];

		/* The new roster and cleared objective gate are visible to the
		 * authored setup. Later members keep this operation's state. */
		snprintf(script, sizeof(script), "forerunner_squad_%02d_begin", starting_squad + 1);
		if (extraction_wake_mission_script(script, TRUE))
			platform_log("extraction: squad %d operation %lu began; authored setup woken",
				starting_squad + 1, extraction.squads[starting_squad].operation);
	}
	if (!extraction.survival && VALID_INDEX(faction, 2) && extraction.dropship_names[faction] != NONE)
		extraction_tell(run, faction == 0 ?
			"DEPLOYED | Board a Pelican passenger seat. Escape alive to recover your carried gear and loot." :
			"DEPLOYED | Board a Spirit passenger seat. Escape alive to recover your carried gear and loot.");
	else extraction_tell(run, !extraction.survival && !run->gear.record.reserved_count && !run->gear.record.reserved_grenades[0] && !run->gear.record.reserved_grenades[1] ?
		"DEPLOYED | Stored gear stays safe. Native starters/carried gear and field loot must escape to be recovered. No raid timer." : extraction.survival ?
		"SURVIVAL DEPLOYED | Gear is at risk. Recover through an available evacuation point or finite victory." :
		"DEPLOYED | Gear is at risk until escape, including disconnects. No raid timer.");
	return TRUE;
}

static void extraction_discard_choice(struct extraction_run *run, long unit_index, boolean stash)
{
	struct gear_item const *choice;
	long now = game_time_get();
	char text[128];
	boolean confirming = run->discard_deadline && now <= run->discard_deadline && run->discard_unit == unit_index &&
		run->discard_stash == stash;
	/* An already published deletion remains the exact same transaction even
	 * after the preview timer. It must not become another item selection. */
	if (run->discard_deadline && run->gear.pending && run->gear.record.generation == run->discard_generation + 1)
		confirming = TRUE;

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
		run->discard_deadline = 0; run->blocked = FALSE; run->stash_preview = FALSE;
		extraction_tell(run, "DISCARD COMPLETE | One confirmed stored weapon removed. Other gear and grenades are safe.");
		return;
	}
	choice = NULL;
	if (stash)
	{
		short index;
		if (!run->stash_preview || run->discard_unit != unit_index ||
			run->discard_generation != run->gear.record.generation || run->gear.record.phase != _gear_idle)
		{
			run->discard_deadline = 0; run->stash_preview = FALSE;
			extraction_tell(run, "STASH PREVIEW EXPIRED | Finish your run, then use the report terminal to review a stored item.");
			return;
		}
		for (index = 0; index < run->gear.record.count; index++)
			if (!memcmp(run->discard_id, run->gear.record.items[index].id, GEAR_ID_BYTES))
				choice = &run->gear.record.items[index];
	}
	else choice = gear_loadout_choice(&run->gear);
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
	run->discard_stash = stash;
	run->discard_generation = run->gear.record.generation; run->discard_unit = unit_index;
	run->discard_after = now + TICKS_PER_SECOND; run->discard_deadline = now + 10 * TICKS_PER_SECOND;
	snprintf(text, sizeof(text), "DISCARD %.20s (%d/%d ammo)? Wait 1s; release/use again while crouched within 10s. Stand-use cancels.",
		extraction_item_name(choice->equipment.tag_path), (int)choice->equipment.rounds_loaded[0], (int)choice->equipment.rounds_reserve[0]);
	extraction_tell(run, text);
}

static boolean extraction_call_owner_current(struct extraction_run const *run, struct player_datum const *player)
{
	byte owner[GEAR_ID_BYTES];

	return !player->quit_out_of_game && player->unit_index == run->unit &&
		player->local_player_index == run->local_player && run->gear.opened && !run->gear.pending &&
		run->gear.record.phase == _gear_deployed && !extraction_resolve_owner(player, run->player, owner) &&
		!memcmp(owner, run->owner, GEAR_ID_BYTES);
}

static boolean extraction_begin_activation(struct extraction_run *run, struct player_datum const *player)
{
	if (run->phase != _extraction_deployed || !extraction_call_owner_current(run, player)) return FALSE;

	run->field_reached = TRUE;
	run->phase = _extraction_activating;
	run->activation_ticks = 10 * TICKS_PER_SECOND;
	extraction_tell(run, "EXTRACTION ACTIVATED | Hold this zone for ten seconds. Leaving cancels activation.");
	return TRUE;
}

static void extraction_begin_squad_activation(struct extraction_run const *caller, short faction, short zone)
{
	short index, squad = caller->squad;

	if (extraction.survival || !VALID_INDEX(faction, NUMBEROF(extraction.dropship_names)) ||
		extraction.dropship_names[faction] == NONE || extraction.dropship_boarding_groups[faction] == NONE ||
		!VALID_INDEX(squad, NUMBEROF(extraction.squads)) || extraction.squads[squad].name == NONE ||
		!caller->squad_operation || caller->squad_operation != extraction.squads[squad].operation) return;

	for (index = 0; index < NUMBEROF(extraction_runs); index++)
	{
		struct extraction_run *member = &extraction_runs[index];
		struct player_datum const *player;
		struct unit_datum const *unit;

		if (member->phase != _extraction_deployed || member->squad != squad ||
			member->squad_operation != caller->squad_operation) continue;
		player = player_try_and_get(member->player);
		if (!player || extraction_faction((short)player->team_index) != faction) continue;
		unit = (struct unit_datum const *)object_try_and_get_and_verify_type(member->unit, _object_mask_biped);
		if (!unit || unit->unit.player_index != member->player || unit->object.parent_object_index != NONE ||
			extraction_faction(unit->object.owner_team_index) != faction ||
			TEST_FLAG(unit->object.damage_flags, _object_dead_bit) || extraction_unit_pending_death(unit) ||
			extraction_medical_incapacitated(member->unit) ||
			!scenario_trigger_volume_test_object(zone, member->unit)) continue;
		extraction_begin_activation(member, player);
	}
}

static short extraction_hub_control(short name, short *faction)
{
	short side, service;

	for (side = 0; side < 2; side++)
		for (service = 0; service < NUMBER_OF_EXTRACTION_HUB_CONTROLS; service++)
			if (name == extraction.hub_controls[side][service])
			{
				*faction = side;
				return service;
			}
	return NONE;
}

static wchar_t const *extraction_control_label(short name, short *squad)
{
	short index, faction, service;

	if (name == NONE || (!extraction_owns_map() &&
		!(extraction.survival && extraction.admitted && extraction.evacuation && extraction_requested()))) return NULL;

	service = extraction_hub_control(name, &faction);
	if (name == extraction.deploy_name || service == _extraction_hub_deploy_control)
		return L"Press %action to prepare for deployment";
	if (name == extraction.activate_name || name == extraction.dropship_activate_names[0] ||
		name == extraction.dropship_activate_names[1]) return L"Press %action to request evacuation";
	if (name == extraction.loadout_name || service == _extraction_hub_loadout_control) return L"Press %action for loadout and stash";
	if (name == extraction.debrief_name || service == _extraction_hub_debrief_control) return L"Press %action for operation report";
	if (name == extraction.faction_names[0]) return L"Press %action to choose UNSC";
	if (name == extraction.faction_names[1]) return L"Press %action to choose Covenant";
	for (index = 0; index < NUMBEROF(extraction.squads); index++)
	{
		if (name == extraction.squads[index].name)
		{
			*squad = index;
			return L"Press %action to choose this squad";
		}
	}
	return NULL;
}

wchar_t const *extraction_control_prompt(long control_index)
{
	struct object_datum const *control = object_try_and_get_and_verify_type(control_index, _object_mask_control);
	short squad = NONE;

	return control ? extraction_control_label(control->object.name_index, &squad) : NULL;
}

boolean extraction_control_touched(long control_index, long unit_index)
{
	struct object_datum *control;
	struct unit_datum *unit;
	struct player_datum *player;
	struct extraction_run *run;
	long slot;
	short faction, squad = NONE, zone, call_name, hub_faction, hub_service;
	boolean in_hub, activation;

	control = object_try_and_get_and_verify_type(control_index, _object_mask_control);
	if (!control || !extraction_control_label(control->object.name_index, &squad)) return FALSE;
	activation = control->object.name_index == extraction.activate_name ||
		control->object.name_index == extraction.dropship_activate_names[0] ||
		control->object.name_index == extraction.dropship_activate_names[1];
	if (extraction.suspended || cinematic_in_progress() ||
		(extraction.survival && coop_enemies_firefight_outcome() != 0 && control->object.name_index != extraction.debrief_name)) return TRUE;
	/* Recheck authored power, usability and one-use locks on the host before
	 * reserving gear or accepting activation. A queued action may be stale. */
	if (!device_can_change_position(control_index)) return TRUE;
	unit = (struct unit_datum *)object_try_and_get_and_verify_type(unit_index, _object_mask_biped);
	if (!unit || unit->unit.player_index == NONE || TEST_FLAG(unit->object.damage_flags, _object_dead_bit) ||
		extraction_unit_pending_death(unit) || extraction_medical_incapacitated(unit_index)) return TRUE;
	if (scenario_uses_regional_gameplay(global_scenario))
	{
		short bsp = object_get_regional_bsp(unit_index);
		if (bsp == NONE || object_get_regional_bsp(control_index) != bsp)
			return TRUE;
	}
	player = player_try_and_get(unit->unit.player_index);
	slot = DATUM_INDEX_TO_ABSOLUTE_INDEX(unit->unit.player_index);
	if (!player || player->unit_index != unit_index || player->quit_out_of_game || !VALID_INDEX(slot, NUMBEROF(extraction_runs))) return TRUE;
	run = &extraction_runs[slot];
	if (run->player != unit->unit.player_index)
	{
		extraction_run_close(run); run->player = unit->unit.player_index;
	}
	in_hub = extraction_at_hub(unit_index);
	hub_service = extraction_hub_control(control->object.name_index, &hub_faction);
	if (hub_service != NONE &&
		(extraction_faction((short)player->team_index) != hub_faction ||
		 extraction_faction(unit->object.owner_team_index) != hub_faction ||
		 !extraction_in_faction_hub(hub_faction, unit_index)))
	{
		extraction_tell(run, "USE YOUR FACTION BASE | This terminal serves the other faction.");
		return TRUE;
	}
	if (squad != NONE)
	{
		extraction_squad_choose(run, player, squad, in_hub);
		return TRUE; /* Membership is per player; do not toggle a shared switch. */
	}
	if (hub_service == _extraction_hub_debrief_control || control->object.name_index == extraction.debrief_name)
	{
		if (!in_hub || unit->object.parent_object_index != NONE)
			extraction_tell(run, "DEBRIEF LOCKED | Read your report on foot at the hub.");
		else if (extraction_open_gear(run, player))
		{
			if (run->discard_deadline && run->gear.pending &&
				run->gear.record.generation == run->discard_generation + 1)
				extraction_discard_choice(run, unit_index, run->discard_stash);
			else if (TEST_FLAG(unit->unit.control_flags, _unit_control_crouch_modifier_bit))
				extraction_discard_choice(run, unit_index, TRUE);
			else
			{
				run->discard_deadline = 0;
				extraction_debrief(run);
			}
		}
		return TRUE; /* Per-player pages never toggle a shared native device. */
	}
	for (faction = 0; faction < 2; faction++)
		if (extraction.faction_names[faction] != NONE && control->object.name_index == extraction.faction_names[faction])
		{
			struct game_modes_inventory loadout;
			short team = faction ? _game_team_covenant : _game_team_human;
			short dropped = 0;
			short variant = extraction.faction_variants[faction];
			long biped = extraction.faction_bipeds[faction];
			boolean native_body = biped == NONE && extraction.faction_bipeds[1 - faction] != NONE;
			if (run->squad != NONE && extraction_faction((short)player->team_index) != faction)
			{
				extraction_tell(run, "LEAVE SQUAD FIRST | Change faction at the hub, then join a compatible squad."); return TRUE;
			}
			if (!in_hub || unit->object.parent_object_index != NONE || unit->unit.grenade_throw_state != _unit_grenade_throw_idle ||
				(run->phase != _extraction_hub && run->phase != _extraction_lost && run->phase != _extraction_returned))
			{
				extraction_tell(run, "FACTION LOCKED | Choose on foot at the hub before deployment or after settling a run.");
				return TRUE;
			}
			if ((extraction.dropship_names[0] != NONE || extraction.dropship_names[1] != NONE) &&
				extraction.dropship_names[faction] == NONE)
			{
				extraction_tell(run, "FACTION UNAVAILABLE | This operation has no extraction transport for that side.");
				return TRUE;
			}
			if (!extraction_open_gear(run, player) || run->gear.record.phase != _gear_idle ||
				!game_modes_inventory_grenade_room(run->player, run->gear.grenade_room) || !gear_loadout(&run->gear, &loadout, TRUE) ||
				!game_modes_inventory_starting_profile(extraction.faction_profiles[faction], GEAR_LOADOUT_WEAPONS,
					run->gear.grenade_room, &loadout))
			{
				extraction_block(run, "FACTION UNAVAILABLE | Starter equipment is unavailable or stash recovery is pending. No equipment granted.");
				return TRUE;
			}
			/* A one-sided body override still needs a way back to the map's
			 * native player body. The ordinary respawn uses this same tag. */
			if (native_body)
			{
				struct game_globals *globals = scenario_get_game_globals();
				if (globals && globals->player_information.count > 0)
				{
					struct game_globals_player_information const *info =
						TAG_BLOCK_GET_ELEMENT(&globals->player_information, 0, struct game_globals_player_information);
					biped = info->player_unit.index;
				}
				variant = 0;
			}
			if ((native_body && biped == NONE) ||
				(biped != NONE &&
				 (!native_body || unit->definition_index != biped ||
				  extraction_faction(unit->object.owner_team_index) != faction) &&
				 !game_modes_player_change_biped(run->player, biped, variant, team,
					extraction.faction_profiles[faction], &run->gear, &dropped)))
			{
				extraction_block(run, "CHARACTER UNAVAILABLE | Check compatible loadout. Carried gear remains with you or beside the terminal.");
				return TRUE;
			}
			unit = unit_get(player->unit_index);
			/* Reuse native allegiance and its existing object replication. Never
			 * attach a new AI controller or change the whole encounter's team. */
			extraction_set_player_team(player, unit, team);
			run->discard_deadline = 0; run->loadout_seen = FALSE; run->blocked = FALSE; run->dropship = NONE;
			extraction_tell(run, faction ?
				(extraction.dropship_names[1] != NONE ?
					"COVENANT SELECTED | Recovered gear takes priority over the starter kit. Evacuate aboard your Spirit." :
					"COVENANT SELECTED | Recovered gear takes priority over the starter kit. This operation returns through the hub.") :
				(extraction.dropship_names[0] != NONE ?
					"UNSC SELECTED | Recovered gear takes priority over the starter kit. Evacuate aboard your Pelican." :
					"UNSC SELECTED | Recovered gear takes priority over the starter kit. This operation returns through the hub."));
			if (dropped) extraction_tell(run, "FACTION SELECTED | Incompatible carried weapons are beside you. Saved gear is unchanged.");
			return TRUE; /* Per-player choice, not a shared device-position flag. */
		}
	if ((hub_service == _extraction_hub_loadout_control || control->object.name_index == extraction.loadout_name) && in_hub &&
		(run->phase == _extraction_hub || run->phase == _extraction_lost || run->phase == _extraction_returned))
	{
		if (extraction_open_gear(run, player))
		{
			run->stash_preview = FALSE;
			if (!run->gear.pending && run->discard_stash) run->discard_deadline = 0;
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
				extraction_discard_choice(run, unit_index, FALSE);
				return TRUE;
			}
			if (run->discard_deadline)
			{
				run->discard_deadline = 0;
				extraction_tell(run, "DISCARD CANCELLED | Your stored gear is safe.");
				extraction_loadout_notice(run);
				return TRUE;
			}
			if (run->loadout_seen && !gear_cycle_loadout(&run->gear,
				update_client_loadout_modifier_held(unit->unit.player_index) ? 1 : 0))
			{
				extraction_block(run, "LOADOUT CHOICE PENDING | Retry this terminal to confirm the saved choice before deploying.");
				return TRUE;
			}
			run->loadout_seen = TRUE;
			extraction_loadout_notice(run);
		}
		return TRUE;
	}
	if ((hub_service == _extraction_hub_deploy_control || control->object.name_index == extraction.deploy_name) && in_hub &&
		(run->phase == _extraction_hub || run->phase == _extraction_lost || run->phase == _extraction_returned))
	{
		/* Let the native control transition notify HaloScript only after the
		 * host has granted the reserved loadout successfully. */
		return !extraction_deploy(run, player, !update_client_loadout_modifier_held(unit->unit.player_index));
	}
	if (!activation) return TRUE;
	faction = extraction_faction(unit->object.owner_team_index);
	call_name = VALID_INDEX(faction, 2) && extraction.dropship_activate_names[faction] != NONE ?
		extraction.dropship_activate_names[faction] : extraction.activate_name;
	if (control->object.name_index != call_name ||
		((extraction.dropship_activate_names[0] != NONE || extraction.dropship_activate_names[1] != NONE) &&
			(faction == NONE || faction != extraction_faction((short)player->team_index) ||
				extraction.dropship_names[faction] == NONE)))
	{
		extraction_tell(run, "WRONG LANDING ZONE | Use your faction's extraction terminal.");
		return TRUE;
	}
	zone = extraction_zone_for_faction(faction);
	if (zone == NONE || !scenario_trigger_volume_test_object(zone, unit_index)) return TRUE;
	if (!extraction_squad_objectives_ready(run))
	{
		extraction_tell(run, "OBJECTIVES INCOMPLETE | Finish your squad's authored mission before calling extraction."); return TRUE;
	}
	if (run->phase == _extraction_deployed && run->unit == unit_index)
	{
		if (!extraction_begin_activation(run, player)) return TRUE;
		extraction_begin_squad_activation(run, faction, zone);
		return FALSE;
	}
	if (run->phase == _extraction_ready && run->unit == unit_index &&
		unit->object.parent_object_index == NONE &&
		(extraction.dropship_names[0] != NONE || extraction.dropship_names[1] != NONE))
	{
		/* An earlier flight may have departed with another crew. Reuse the
		 * authored call control without another countdown or gear transaction. */
		if (faction == NONE || extraction.dropship_names[faction] == NONE)
		{
			extraction_tell(run, "TRANSPORT UNAVAILABLE | This operation has no dropship for your faction. Gear remains at risk.");
			return TRUE;
		}
		if (!extraction_call_owner_current(run, player)) return TRUE;
		run->dropship = NONE;
		extraction_begin_squad_activation(run, faction, zone);
		extraction_tell(run, "TRANSPORT RECALLED | Wait for your assigned dropship, then board at the landing zone. Gear remains at risk.");
		return FALSE;
	}
	return TRUE;
}

static boolean extraction_transport_seat_allowed(struct unit_datum const *vehicle, short seat_index, boolean passenger_only)
{
	struct tag_block const *seats = &unit_definition_get(vehicle->definition_index)->unit.seats;
	if (!VALID_INDEX(seat_index, seats->count)) return FALSE;
	return !passenger_only ||
		!TEST_FLAG(TAG_BLOCK_GET_ELEMENT(seats, seat_index, struct unit_seat)->flags, _unit_seat_driver_bit);
}

/* Native seats, not a trigger/client claim, identify the assigned transport.
 * A named vehicle may be created by HaloScript after the extraction call. */
static long extraction_player_dropship(struct unit_datum const *unit)
{
	short faction, name;
	long index;
	struct unit_datum *vehicle;
	faction = extraction_faction(unit->object.owner_team_index);
	if (faction == NONE) return NONE;
	name = extraction.dropship_names[faction];
	if (name == NONE || unit->unit.parent_seat_index < 0) return NONE;
	index = object_index_from_name_index(name);
	if (index == NONE || unit->object.parent_object_index != index) return NONE;
	vehicle = (struct unit_datum *)object_try_and_get_and_verify_type(index, _object_mask_vehicle);
	if (!vehicle || TEST_FLAG(vehicle->object.damage_flags, _object_dead_bit) || vehicle->object.body_vitality <= 0.0f ||
		extraction_unit_pending_death(vehicle) || !extraction_transport_seat_allowed(vehicle,
			unit->unit.parent_seat_index, extraction.dropship_boarding_groups[faction] != NONE)) return NONE;
	return index;
}

static boolean extraction_transport_hover_ready(long vehicle_index)
{
	struct unit_datum const *vehicle =
		object_try_and_get_and_verify_type(vehicle_index, _object_mask_vehicle);
	return vehicle && vehicle->object.body_vitality > 0.f &&
		!TEST_FLAG(vehicle->object.damage_flags, _object_dead_bit) &&
		!extraction_unit_pending_death(vehicle) && vehicle_is_hovering(vehicle_index);
}

static boolean extraction_transport_landed(short faction, long vehicle_index)
{
	boolean at_field;

	/* The last recorded input can have landed while hover or cleanup is
	 * still pending. Wait for native cleanup before accepting touchdown. */
	if (recorded_animation_has_thread(vehicle_index))
		return FALSE;
	at_field = extraction_transport_at_field(faction, vehicle_index);
	if (extraction.dropship_hover_transfer[faction] && at_field)
		return extraction_transport_hover_ready(vehicle_index);
	if (!vehicle_is_landed(vehicle_index))
		return FALSE;

	/* A field-bound Pelican can briefly pass the speed and attitude limits
	 * during its ground roll. Wait for its native rest flag before opening
	 * passenger exits or publishing touchdown to HaloScript. */
	if (vehicle_is_human_plane(vehicle_index) && at_field)
	{
		struct object_datum const *vehicle = object_try_and_get_and_verify_type(vehicle_index, _object_mask_vehicle);
		return vehicle && TEST_FLAG(vehicle->object.flags, _object_at_rest_bit);
	}
	return TRUE;
}

/* A stopped hull can serve another operation after its verified return.
 * The next touchdown still uses the ordinary landing limits. */
static boolean extraction_transport_completed_return(short faction, long vehicle_index)
{
	short zone = extraction.dropship_return_zones[faction];
	return !extraction.suspended && extraction.dropship_return_stop[faction] &&
		zone != NONE && extraction.dropship_completed[faction] == vehicle_index &&
		!recorded_animation_has_thread(vehicle_index) &&
		extraction_transport_at_return(faction, vehicle_index) && vehicle_is_settled_on_return(vehicle_index);
}

static boolean extraction_transport_landing_signal(short faction, long ship)
{
	/* A safe unscheduled touchdown can allow egress without signaling
	 * scripted arrival at the operation's assigned site. */
	if (scenario_uses_regional_gameplay(global_scenario) &&
		!extraction_transport_at_field(faction, ship) && !extraction_transport_at_return(faction, ship))
		return FALSE;
	return extraction_transport_landed(faction, ship) || extraction_transport_completed_return(faction, ship);
}

static boolean extraction_transport_arrival_ready(short faction, long vehicle_index)
{
	/* Older routes can time their own landing. Settlement and release still
	 * wait until recorded playback relinquishes the aircraft. */
	if (extraction.dropship_landing_groups[faction] != NONE)
		return extraction_transport_landed(faction, vehicle_index);
	return !recorded_animation_has_thread(vehicle_index);
}

boolean extraction_transport_can_board(long unit_index, long vehicle_index, short seat_index)
{
	struct unit_datum *unit, *vehicle;
	struct player_datum *player;
	struct extraction_run const *run;
	long slot;
	short faction;
	if (!extraction_requested() || !extraction.bound || !extraction.admitted) return TRUE;
	for (faction = 0; faction < 2; faction++)
		if (extraction.dropship_names[faction] != NONE &&
			object_index_from_name_index(extraction.dropship_names[faction]) == vehicle_index) break;
	if (faction == 2) return TRUE; /* No restrictions on ordinary native vehicles. */
	unit = (struct unit_datum *)object_try_and_get_and_verify_type(unit_index, _object_mask_unit);
	if (!unit) return FALSE;
	if (unit->unit.player_index == NONE) return TRUE; /* Authored NPC crews. */
	player = player_try_and_get(unit->unit.player_index);
	vehicle = (struct unit_datum *)object_try_and_get_and_verify_type(vehicle_index, _object_mask_vehicle);
	if (!player || player->unit_index != unit_index || player->quit_out_of_game ||
		!vehicle || TEST_FLAG(vehicle->object.damage_flags, _object_dead_bit) || extraction_unit_pending_death(vehicle) ||
		!(vehicle->object.body_vitality > 0.0f) || TEST_FLAG(unit->object.damage_flags, _object_dead_bit) ||
		extraction_unit_pending_death(unit) || extraction_medical_incapacitated(unit_index) ||
		extraction_faction(unit->object.owner_team_index) != faction ||
		extraction_faction((short)player->team_index) != faction) return FALSE;
	/* Crew-only maps still use their authored touchdown rules. Native
	 * playback must release the aircraft before accepting new passengers. */
	if ((extraction.dropship_landing_groups[faction] != NONE || extraction.dropship_boarding_groups[faction] != NONE) &&
		!extraction_transport_arrival_ready(faction, vehicle_index) &&
		!extraction_transport_completed_return(faction, vehicle_index)) return FALSE;
	if (extraction.dropship_boarding_groups[faction] == NONE) return TRUE;
	if (!extraction_transport_seat_allowed(vehicle, seat_index, TRUE)) return FALSE;

	/* Ships with a crew-readiness signal reserve their seats for runs that
	 * can actually depart. Ordinary Halo seating still owns distance,
	 * facing, animation and occupied-seat checks. */
	slot = DATUM_INDEX_TO_ABSOLUTE_INDEX(unit->unit.player_index);
	if (extraction.suspended || cinematic_in_progress() || !VALID_INDEX(slot, NUMBEROF(extraction_runs))) return FALSE;
	run = &extraction_runs[slot];
	if (run->player != unit->unit.player_index || run->unit != unit_index || !run->gear.opened ||
		run->gear.pending || run->gear.record.phase != _gear_deployed) return FALSE;
	if (extraction_transport_at_return(faction, vehicle_index))
		return run->phase == _extraction_deployed;
	return run->phase == _extraction_ready && extraction_transport_at_field(faction, vehicle_index) &&
		extraction_squad_objectives_ready(run);
}

boolean extraction_transport_uses_hover_exit(struct unit_datum const *passenger)
{
	short faction;
	long ship = passenger->object.parent_object_index;
	if (passenger->unit.player_index == NONE || ship == NONE ||
		!extraction_requested() || !extraction.bound || !extraction.admitted) return FALSE;
	for (faction = 0; faction < NUMBEROF(extraction.dropship_names); faction++)
		if (extraction.dropship_hover_transfer[faction] && extraction.dropship_names[faction] != NONE &&
			object_index_from_name_index(extraction.dropship_names[faction]) == ship)
			return extraction_transport_at_field(faction, ship) && extraction_transport_hover_ready(ship);
	return FALSE;
}

boolean extraction_transport_can_disembark(struct unit_datum const *passenger)
{
	long ship = passenger->object.parent_object_index;
	short faction;
	if (passenger->unit.player_index == NONE || ship == NONE ||
		!extraction_requested() || !extraction.bound || !extraction.admitted) return TRUE;
	for (faction = 0; faction < 2; faction++)
		if (extraction.dropship_names[faction] != NONE &&
			(extraction.dropship_landing_groups[faction] != NONE || extraction.dropship_boarding_groups[faction] != NONE) &&
			object_index_from_name_index(extraction.dropship_names[faction]) == ship)
		{
			struct unit_datum *vehicle = (struct unit_datum *)
				object_try_and_get_and_verify_type(ship, _object_mask_vehicle);
			/* A damaged or destroyed craft must still release its passengers. */
			return !vehicle || TEST_FLAG(vehicle->object.damage_flags, _object_dead_bit) ||
				extraction_unit_pending_death(vehicle) || !(vehicle->object.body_vitality > 0.f) ||
				extraction_transport_arrival_ready(faction, ship) ||
				extraction_transport_completed_return(faction, ship);
		}
	return TRUE;
}

wchar_t const *extraction_transport_passenger_prompt(struct unit_datum const *passenger)
{
	struct extraction_run const *run;
	long slot, ship;
	if (passenger->object.parent_object_index == NONE || passenger->unit.player_index == NONE ||
		!extraction_owns_map())
		return NULL;
	ship = extraction_player_dropship(passenger);
	if (ship == NONE) return NULL;

	slot = DATUM_INDEX_TO_ABSOLUTE_INDEX(passenger->unit.player_index);
	run = VALID_INDEX(slot, NUMBEROF(extraction_runs)) ? &extraction_runs[slot] : NULL;
	if (run && run->player == passenger->unit.player_index && run->dropship == ship &&
		(run->phase == _extraction_ready || run->phase == _extraction_settling) &&
		passenger->unit.animation.state != _unit_state_exiting_seat)
		return L"Stay aboard to bank your gear";

	if (passenger->unit.animation.state == _unit_state_exiting_seat)
		return NULL;
	if (!extraction_transport_can_disembark(passenger) ||
		unit_animation_busy(&passenger->unit.animation))
		return L"Stay seated until the exit opens";
	if (extraction_transport_uses_hover_exit(passenger))
		return L"Press %action to deploy";
	return L"Press %action to disembark";
}

void extraction_transport_seat_transition_started(struct unit_datum const *passenger)
{
	long ship = passenger->object.parent_object_index;
	short faction;
	if (!extraction.bound || !extraction.admitted || network_game_distributed_client() ||
		passenger->unit.player_index == NONE || ship == NONE || passenger->unit.parent_seat_index == NONE) return;
	/* Invalidate the attached craft even if its rider or hull lost eligibility
	 * before a forced exit. HaloScript can read the group again this tick. */
	for (faction = 0; faction < NUMBEROF(extraction.dropship_names); faction++)
	{
		short group = extraction.dropship_boarding_groups[faction];
		short name = extraction.dropship_names[faction];
		if (group != NONE && name != NONE && object_index_from_name_index(name) == ship &&
			device_group_get_value(group) != 0.f) device_group_set_actual_value(group, 0.f);
	}
}

void extraction_transport_recording_started(long unit_index)
{
	short faction;
	if (!extraction.bound || !extraction.admitted || network_game_distributed_client()) return;
	for (faction = 0; faction < NUMBEROF(extraction.dropship_names); faction++)
	{
		short landing_group = extraction.dropship_landing_groups[faction];
		short boarding_group = extraction.dropship_boarding_groups[faction];
		if ((landing_group == NONE && boarding_group == NONE) || extraction.dropship_names[faction] == NONE ||
			object_index_from_name_index(extraction.dropship_names[faction]) != unit_index) continue;
		extraction.dropship_return_stop[faction] = FALSE;
		/* HaloScript can read these outputs again before the next tick. */
		if (landing_group != NONE && device_group_get_value(landing_group) != 0.f)
			device_group_set_actual_value(landing_group, 0.f);
		if (boarding_group != NONE && device_group_get_value(boarding_group) != 0.f)
			device_group_set_actual_value(boarding_group, 0.f);
	}
}

static boolean extraction_at_return(struct extraction_run const *run, long unit_index)
{
	struct unit_datum *unit = (struct unit_datum *)object_try_and_get_and_verify_type(unit_index, _object_mask_biped);
	if (!unit) return FALSE;
	/* The ship's authored return pad governs banking while aboard.
	 * On foot, paired bases each accept only their own faction. */
	if (run->dropship != NONE && extraction_player_dropship(unit) == run->dropship)
	{
		short faction = extraction_faction(unit->object.owner_team_index);
		if (faction != NONE && !extraction_transport_arrival_ready(faction, run->dropship)) return FALSE;
		return extraction_transport_at_return(faction, run->dropship);
	}
	if (extraction.separate_hubs)
	{
		short faction = extraction_faction(unit->object.owner_team_index);
		return VALID_INDEX(faction, 2) && extraction_in_faction_hub(faction, unit_index);
	}
	return extraction_at_hub(unit_index);
}

boolean extraction_transport_idle_at_field(long vehicle_index)
{
	short faction;

	if (!extraction_owns_map() || extraction.suspended || recorded_animation_has_thread(vehicle_index) ||
		!vehicle_is_landed(vehicle_index)) return FALSE;
	for (faction = 0; faction < NUMBEROF(extraction.dropship_names); faction++)
		if (extraction.dropship_names[faction] != NONE && extraction.dropship_landing_groups[faction] != NONE &&
			!extraction.dropship_hover_transfer[faction] &&
			object_index_from_name_index(extraction.dropship_names[faction]) == vehicle_index)
			return extraction_transport_at_field(faction, vehicle_index);
	return FALSE;
}

boolean extraction_transport_brake_at_pad(long vehicle_index)
{
	short faction, index;

	if (!extraction_owns_map() || extraction.suspended || recorded_animation_has_thread(vehicle_index))
		return FALSE;
	for (faction = 0; faction < NUMBEROF(extraction.dropship_return_zones); faction++)
	{
		short zone = extraction.dropship_return_zones[faction];
		if (zone == NONE || extraction.dropship_evacuating[faction] != vehicle_index ||
			!extraction_transport_at_return(faction, vehicle_index))
			continue;
		for (index = 0; index < NUMBEROF(extraction_runs); index++)
		{
			struct extraction_run const *run = &extraction_runs[index];
			if (run->phase == _extraction_ready && run->dropship == vehicle_index)
			{
				struct unit_datum *unit = (struct unit_datum *)object_try_and_get_and_verify_type(run->unit, _object_mask_biped);
				if (unit && extraction_player_dropship(unit) == vehicle_index)
					return TRUE;
			}
		}
	}
	return FALSE;
}

static boolean extraction_return_ready(struct extraction_run *run, struct unit_datum const *unit)
{
	long dropship;
	if (extraction.dropship_names[0] == NONE && extraction.dropship_names[1] == NONE)
		return extraction_at_return(run, run->unit); /* existing hub-only maps */
	dropship = extraction_player_dropship(unit);
	if (dropship == NONE || (run->dropship != NONE &&
		(run->dropship != dropship || run->dropship_team != unit->object.owner_team_index)))
	{
		if (run->dropship != NONE)
			extraction_tell(run, "BOARDING CANCELLED | Return to your assigned dropship at the landing zone. Gear remains at risk.");
		run->dropship = NONE;
		return FALSE;
	}
	if (run->dropship == NONE)
	{
		short faction = extraction_faction(unit->object.owner_team_index);
		if (extraction_medical_incapacitated(run->unit) ||
			!extraction_transport_at_field(faction, dropship) ||
			!extraction_transport_arrival_ready(faction, dropship) ||
			(extraction.dropship_boarding_groups[faction] != NONE &&
				unit->unit.animation.state == _unit_state_entering_seat)) return FALSE;
		run->dropship = dropship;
		run->dropship_team = unit->object.owner_team_index;
		extraction.dropship_evacuating[faction] = dropship;
		extraction.dropship_return_stop[faction] = FALSE;
		extraction.dropship_banked[faction] = extraction.dropship_completed[faction] = NONE;
		extraction_tell(run, "BOARDED | Stay aboard through departure. Recovery follows arrival and the authored cinematic.");
		return FALSE;
	}
	/* The authored native pilot/flight and camera scripts own departure. Do
	 * not bank a timer, a walk into the hub, a new ship reusing a slot, or a
	 * passenger who left the transport. Keep it alive at the hub until the
	 * cinematic ends and the existing settlement callback releases it. */
	return !cinematic_in_progress() && extraction_at_return(run, run->unit);
}

static void extraction_finish_return(struct extraction_run *run)
{
	char text[128];
	char const *exit_hint = "";
	run->retry_tick = game_time_get();
	extraction_report_stage(run, _gear_escaped);
	if (!gear_finish_escape(&run->gear))
	{
		extraction_block(run, "RECOVERY PENDING | Escaped gear secured in escrow; banking will retry."); return;
	}
	if (run->report.staged && run->report.outcome == _gear_escaped &&
		!memcmp(run->report.run, run->gear.run, GEAR_ID_BYTES)) run->report.ready = TRUE;
	extraction_remember_bank(run);
	run->phase = _extraction_returned; run->blocked = FALSE;
	if (run->dropship != NONE)
	{
		struct unit_datum const *rider = object_try_and_get_and_verify_type(run->unit, _object_mask_biped);
		if (rider && extraction_player_dropship(rider) == run->dropship &&
			extraction_transport_can_disembark(rider))
			exit_hint = " Use your seat exit to disembark at the hub.";
	}
	if (extraction.debrief_name != NONE)
		snprintf(text, sizeof(text), "GEAR BANKED | Debrief available at the hub.%s", exit_hint);
	else
		snprintf(text, sizeof(text), "%s | Stash %d/%d | Frag %d Plasma %d.%s",
			run->gear.session_only ? "SESSION GEAR BANKED" : "GEAR BANKED",
			(int)run->gear.record.count, GEAR_CAPACITY, (int)run->gear.record.grenades[0], (int)run->gear.record.grenades[1], exit_hint);
	extraction_tell(run, text);
}

static void extraction_update_run(struct extraction_run *run, struct player_datum const *player, long now)
{
	struct unit_datum *unit;
	boolean return_ready = FALSE;
	byte owner[GEAR_ID_BYTES];
	if (run->discard_deadline)
	{
		unit = (struct unit_datum *)object_try_and_get_and_verify_type(player->unit_index, _object_mask_biped);
		if (now > run->discard_deadline || !unit || player->unit_index != run->discard_unit ||
			TEST_FLAG(unit->object.damage_flags, _object_dead_bit) || extraction_unit_pending_death(unit) || unit->object.parent_object_index != NONE ||
			extraction_medical_incapacitated(player->unit_index) || cinematic_in_progress() ||
			!extraction_at_hub(player->unit_index) ||
			extraction_resolve_owner(player, run->player, owner) || memcmp(owner, run->owner, GEAR_ID_BYTES))
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
	if ((run->phase == _extraction_returned || run->phase == _extraction_lost) &&
		extraction.debrief_name != NONE && run->report.ready && !run->report.announced && !run->gear.pending &&
		!cinematic_in_progress() && !extraction_resolve_owner(player, run->player, owner) && !memcmp(owner, run->owner, GEAR_ID_BYTES))
	{
		unit = (struct unit_datum *)object_try_and_get_and_verify_type(player->unit_index, _object_mask_biped);
		if (unit && unit->unit.player_index == run->player && unit->object.parent_object_index == NONE &&
			!TEST_FLAG(unit->object.damage_flags, _object_dead_bit) && !extraction_unit_pending_death(unit) &&
			!extraction_medical_incapacitated(player->unit_index) && extraction_at_return(run, player->unit_index))
			extraction_debrief(run);
	}
	if (run->phase != _extraction_deployed && run->phase != _extraction_activating && run->phase != _extraction_ready) return;
	unit = (struct unit_datum *)object_try_and_get_and_verify_type(run->unit, _object_mask_biped);
	if (player->unit_index != run->unit || player->local_player_index != run->local_player || !unit ||
		unit->unit.player_index != run->player || TEST_FLAG(unit->object.damage_flags, _object_dead_bit) ||
		extraction_resolve_owner(player, run->player, owner) || memcmp(owner, run->owner, GEAR_ID_BYTES))
	{
		extraction_lose_run(run); return;
	}
	if (extraction_unit_pending_death(unit)) return; /* Native death decides the loss next tick. */
	/* The equipment has already been issued. Retry the published confirmation
	 * for this owner without reissuing gear or restarting the operation. */
	if (run->phase == _extraction_deployed && run->gear.pending && run->gear.record.phase == _gear_deployed)
	{
		if ((unsigned long)now - (unsigned long)run->retry_tick >= TICKS_PER_SECOND)
		{
			run->retry_tick = now;
			gear_confirm_deployment(&run->gear);
		}
		if (run->gear.pending) return;
	}
	if (run->phase == _extraction_ready) return_ready = extraction_return_ready(run, unit);
	if (extraction_medical_incapacitated(run->unit))
	{
		if (run->phase == _extraction_activating)
		{
			run->phase = _extraction_deployed; run->activation_ticks = 0;
		}
		return; /* Retain owned gear while alive; an incapacitated body cannot escape. */
	}
	if (run->phase == _extraction_deployed)
	{
		short faction = extraction_faction(unit->object.owner_team_index);
		long ship = extraction_player_dropship(unit);
		boolean exit_ready = ship != NONE && extraction_transport_at_field(faction, ship) &&
			extraction_transport_arrival_ready(faction, ship);
		if (exit_ready)
		{
			run->field_reached = TRUE;
			if (!run->field_exit_ready && !extraction.survival)
				extraction_tell(run, extraction.dropship_hover_transfer[faction]
					? "AT TRANSFER | Follow the seat exit prompt to deploy. Find the extraction control when ready to evacuate."
					: "LANDED | Follow the seat exit prompt to disembark. Find the extraction control when ready to evacuate.");
		}
		else if (run->field_exit_ready && ship != NONE && !extraction.survival &&
			!extraction_transport_can_disembark(unit))
			extraction_tell(run, "WAIT TO DISEMBARK | The dropship is moving. Stay seated until the exit opens.");
		/* Arrival is an operation milestone. Exit guidance follows the live seat gate. */
		run->field_exit_ready = exit_ready;
	}
	if ((run->phase == _extraction_activating || run->phase == _extraction_ready) &&
		!extraction_squad_objectives_ready(run))
	{
		run->phase = _extraction_deployed; run->activation_ticks = 0; run->dropship = NONE;
		extraction_tell(run, "EXTRACTION CANCELLED | Your squad's authored objectives are no longer complete.");
	}
	if (run->phase == _extraction_activating)
	{
		short zone = extraction_zone_for_faction(extraction_faction(unit->object.owner_team_index));
		if (zone == NONE || !scenario_trigger_volume_test_object(zone, run->unit))
		{
			run->phase = _extraction_deployed; run->activation_ticks = 0;
			extraction_tell(run, "ACTIVATION CANCELLED | Return and use the extraction control to retry.");
		}
		else if (--run->activation_ticks <= 0)
		{
			run->phase = _extraction_ready;
			extraction_tell(run, extraction.dropship_names[0] != NONE || extraction.dropship_names[1] != NONE ?
				"EXTRACTION SIGNAL READY | Board your assigned dropship in the landing zone and stay aboard through departure." :
				"EXTRACTION SIGNAL READY | Reach the return point alive.");
			return_ready = extraction_return_ready(run, unit);
		}
	}
	if (run->phase == _extraction_ready &&
		((extraction.survival && coop_enemies_firefight_outcome() == 1) ||
		 ((!extraction.survival || extraction.evacuation) && return_ready)) &&
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

static boolean extraction_transport_crew_ready(short faction, long ship)
{
	short index, passengers = 0;
	unsigned long crew_squads = 0;
	boolean at_hub, at_lz, legacy_crew = FALSE;
	/* Boarding-only maps retain their authored landing rules. During recorded
	 * flight the playback thread owns departure readiness until cleanup. */
	if (cinematic_in_progress() || recorded_animation_has_thread(ship) ||
		!object_try_and_get_and_verify_type(ship, _object_mask_vehicle)) return FALSE;
	at_hub = extraction_transport_at_return(faction, ship);
	at_lz = extraction_transport_at_field(faction, ship);
	if (at_hub == at_lz) return FALSE;
	/* Actual passengers identify participating squads without another flight
	 * registry. Unboarded independent operations cannot veto this departure.
	 * Unsquadded maps retain the existing whole-faction wait. */
	for (index = 0; index < NUMBEROF(extraction_runs); index++)
	{
		struct extraction_run const *run = &extraction_runs[index];
		struct player_datum *player = player_try_and_get(run->player);
		struct unit_datum *unit;
		if (!player || player->quit_out_of_game) continue;
		unit = (struct unit_datum *)object_try_and_get_and_verify_type(player->unit_index, _object_mask_biped);
		if (!unit || unit->object.parent_object_index != ship) continue;
		/* A previous or newly ineligible passenger must physically disembark
		 * before this hull can carry a new crew out of the pad. */
		if (run->phase != (at_lz ? _extraction_ready : _extraction_deployed)) return FALSE;
		/* Preserve the native seat-admission requirements until departure.
		 * A still-attached rider with stale ownership cannot release a crew. */
		if (extraction_player_dropship(unit) != ship || extraction_faction((short)player->team_index) != faction ||
			run->unit != player->unit_index || unit->unit.player_index != run->player ||
			!run->gear.opened || run->gear.pending || run->gear.record.phase != _gear_deployed ||
			TEST_FLAG(unit->object.damage_flags, _object_dead_bit) || extraction_unit_pending_death(unit) ||
			unit->unit.animation.state == _unit_state_entering_seat ||
			unit->unit.animation.state == _unit_state_exiting_seat ||
			extraction_medical_incapacitated(player->unit_index)) return FALSE;
		if (run->squad == NONE) legacy_crew = TRUE;
		else
		{
			if (!VALID_INDEX(run->squad, NUMBEROF(extraction.squads)) || extraction.squads[run->squad].name == NONE ||
				!run->squad_operation || run->squad_operation != extraction.squads[run->squad].operation) return FALSE;
			crew_squads |= FLAG(run->squad);
		}
	}
	for (index = 0; index < NUMBEROF(extraction_runs); index++)
	{
		struct extraction_run const *run = &extraction_runs[index];
		struct player_datum *player = player_try_and_get(run->player);
		struct unit_datum *unit;
		if (!player || player->quit_out_of_game) continue;
		if (run->dropship == ship && (run->phase == _extraction_settling || run->phase == _extraction_losing)) return FALSE;
		unit = (struct unit_datum *)object_try_and_get_and_verify_type(player->unit_index, _object_mask_biped);
		if (!unit || extraction_faction(unit->object.owner_team_index) != faction) continue;
		if (!legacy_crew && (!VALID_INDEX(run->squad, NUMBEROF(extraction.squads)) || !(crew_squads & FLAG(run->squad)) ||
			run->squad_operation != extraction.squads[run->squad].operation)) continue;
		if (at_lz && run->phase == _extraction_activating) return FALSE;
		/* A deployed teammate may approach a seat outside the hub volume.
		 * Only actual field arrival releases a run from the insertion crew. */
		if (at_lz ? run->phase != _extraction_ready :
			(run->phase != _extraction_deployed ||
				(run->field_reached && unit->object.parent_object_index != ship))) continue;
		passengers++;
		if (run->unit != player->unit_index || unit->unit.player_index != run->player ||
			TEST_FLAG(unit->object.damage_flags, _object_dead_bit) || extraction_unit_pending_death(unit) ||
			extraction_medical_incapacitated(player->unit_index) || extraction_player_dropship(unit) != ship ||
			(at_lz && run->dropship != ship)) return FALSE;
	}
	return passengers > 0;
}

static boolean extraction_transport_called(short faction)
{
	short index, zone = extraction_zone_for_faction(faction);
	if (extraction.suspended || extraction.dropship_names[faction] == NONE || zone == NONE) return FALSE;
	/* The shared activation control's position is not a faction request. Derive
	 * requests from current host-owned runs; a stale switch cannot call a ship
	 * for the other faction or survive death, cancellation or settlement. */
	for (index = 0; index < NUMBEROF(extraction_runs); index++)
	{
		struct extraction_run const *run = &extraction_runs[index];
		struct player_datum *player;
		struct unit_datum *unit;
		if ((run->phase != _extraction_activating && run->phase != _extraction_ready) || run->gear.pending) continue;
		player = player_try_and_get(run->player);
		if (!player || player->quit_out_of_game || player->unit_index != run->unit ||
			extraction_faction((short)player->team_index) != faction) continue;
		unit = (struct unit_datum *)object_try_and_get_and_verify_type(run->unit, _object_mask_biped);
		if (!unit || unit->unit.player_index != run->player || extraction_faction(unit->object.owner_team_index) != faction ||
			TEST_FLAG(unit->object.damage_flags, _object_dead_bit) || extraction_unit_pending_death(unit)) continue;
		if (extraction_medical_incapacitated(run->unit)) continue;
		/* A completed managed pickup stays open while living callers approach.
		 * Countdown presence and actual boarding still use the field volume. */
		if (!extraction.survival && extraction.dropship_boarding_groups[faction] != NONE &&
			run->phase == _extraction_ready && run->dropship == NONE &&
			extraction_player_dropship(unit) == NONE && extraction_squad_objectives_ready(run)) return TRUE;
		if (unit->object.parent_object_index == NONE ? scenario_trigger_volume_test_object(zone, run->unit) :
			(extraction_player_dropship(unit) == run->dropship && run->dropship != NONE &&
				scenario_trigger_volume_test_object(zone, run->dropship))) return TRUE;
	}
	return FALSE;
}

static void extraction_transport_readiness(void)
{
	short faction;
	for (faction = 0; faction < 2; faction++)
	{
		short landing_group = extraction.dropship_landing_groups[faction];
		short boarding_group = extraction.dropship_boarding_groups[faction];
		if (landing_group != NONE || boarding_group != NONE)
		{
			long ship = extraction.suspended ? NONE : object_index_from_name_index(extraction.dropship_names[faction]);
			real landed, ready;
			if (extraction.dropship_return_stop[faction])
			{
				struct object_datum const *vehicle = object_try_and_get_and_verify_type(ship, _object_mask_vehicle);
				if (!vehicle || ship != extraction.dropship_completed[faction] ||
					!TEST_FLAG(vehicle->object.flags, _object_on_ground_bit) || recorded_animation_has_thread(ship) ||
					!extraction_transport_at_return(faction, ship))
					extraction.dropship_return_stop[faction] = FALSE;
			}
			landed = landing_group != NONE && ship != NONE &&
				extraction_transport_landing_signal(faction, ship) ? 1.f : 0.f;
			ready = boarding_group != NONE && ship != NONE && (landing_group == NONE || landed > 0.f) &&
				extraction_transport_crew_ready(faction, ship) ? 1.f : 0.f;
			if (landing_group != NONE && device_group_get_value(landing_group) != landed)
				device_group_set_actual_value(landing_group, landed);
			if (boarding_group != NONE && device_group_get_value(boarding_group) != ready)
				device_group_set_actual_value(boarding_group, ready);
		}
		if (extraction.dropship_call_groups[faction] != NONE)
		{
			real called = extraction_transport_called(faction) ? 1.f : 0.f;
			if (device_group_get_value(extraction.dropship_call_groups[faction]) != called)
				device_group_set_actual_value(extraction.dropship_call_groups[faction], called);
		}
	}
}

static void extraction_transport_completions(void)
{
	short faction, index;
	for (faction = 0; faction < 2; faction++)
	{
		struct unit_datum const *vehicle;
		long ship;
		boolean pending = FALSE;
		if (extraction.dropship_names[faction] == NONE) continue;
		ship = object_index_from_name_index(extraction.dropship_names[faction]);
		if (ship == NONE || ship != extraction.dropship_evacuating[faction] || ship == extraction.dropship_completed[faction]) continue;
		vehicle = (struct unit_datum const *)object_try_and_get_and_verify_type(ship, _object_mask_vehicle);
		if (!vehicle || TEST_FLAG(vehicle->object.damage_flags, _object_dead_bit) ||
			extraction_unit_pending_death(vehicle) || !(vehicle->object.body_vitality > 0.0f) ||
			!extraction_transport_at_return(faction, ship) || cinematic_in_progress() ||
			!extraction_transport_arrival_ready(faction, ship)) continue;
		/* The durable bank belongs to the arrival even if its owner leaves.
		 * Other squads and craft do not delay release. A connected boarded
		 * crewmate still settling keeps this aircraft at the hub. */
		for (index = 0; index < NUMBEROF(extraction_runs); index++)
		{
			struct extraction_run const *run = &extraction_runs[index];
			struct player_datum *player;
			if (run->dropship != ship) continue;
			player = player_try_and_get(run->player);
			if (!player || player->quit_out_of_game) continue;
			if (run->phase == _extraction_ready || run->phase == _extraction_settling ||
				run->phase == _extraction_losing) { pending = TRUE; break; }
		}
		if (pending) continue;
		extraction.dropship_completed[faction] = ship;
		if (ship == extraction.dropship_banked[faction])
		{
			extraction.dropship_return_stop[faction] = extraction.dropship_return_zones[faction] != NONE &&
				extraction.dropship_landing_groups[faction] != NONE;
			if (extraction_wake_mission_script(faction ? "forerunner_spirit_complete" : "forerunner_pelican_complete", TRUE))
				platform_log("extraction: %s crew banked; authored transport completion woken", faction ? "Spirit" : "Pelican");
		}
		else if (extraction_wake_mission_script(faction ? "forerunner_spirit_aborted" : "forerunner_pelican_aborted", TRUE))
			platform_log("extraction: %s evacuation aborted; authored transport release woken", faction ? "Spirit" : "Pelican");
	}
}

/* Wait for the faction's live spawn so a late joiner sees the host's
 * instructions at their own hub, after the match-level notice has passed. */
static void extraction_brief_regional_player(struct extraction_run *run, struct player_datum const *player)
{
	struct unit_datum const *unit;
	short faction;

	if (run->hub_briefed || run->phase != _extraction_hub || cinematic_in_progress()) return;
	faction = extraction_faction((short)player->team_index);
	if (faction == NONE) return;
	unit = (struct unit_datum const *)object_try_and_get_and_verify_type(player->unit_index, _object_mask_unit);
	if (!unit || unit->unit.player_index != run->player || TEST_FLAG(unit->object.damage_flags, _object_dead_bit) ||
		!(unit->object.body_vitality > 0.0f) || extraction_unit_pending_death(unit) ||
		extraction_medical_incapacitated(player->unit_index) ||
		!extraction_in_faction_hub(faction, player->unit_index)) return;

	extraction_tell(run, faction == 0 ?
		"UNSC | Use Deploy, then board the Pelican." :
		"Covenant | Use Deploy, then board the Spirit.");
	run->hub_briefed = TRUE;
}

void extraction_update(void)
{
	struct data_iterator iterator;
	struct player_datum *player;
	long now, slot;
	short index;
	extraction_medical_update();
	extraction_check_authored_mode();
	if (!extraction_requested()) return;
	if (!extraction.bound)
	{
		extraction.survival = !extraction_mode_selected();
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
						extraction_deploy(run, player, TRUE);
					}
				}
				if (outcome > 0 && (run->phase == _extraction_deployed || run->phase == _extraction_activating)) run->phase = _extraction_ready;
				else if (outcome < 0 && (run->phase == _extraction_deployed || run->phase == _extraction_activating || run->phase == _extraction_ready)) extraction_lose_run(run);
			}
			else if (global_scenario->type != _scenario_type_solo)
				extraction_brief_regional_player(run, player);
			extraction_update_run(run, player, now);
			if (extraction.suspended)
			{
				/* Clear the published aircraft signals before HaloScript runs
				 * again. Other passengers must keep their live inventory. */
				extraction_transport_readiness();
				return;
			}
		}
	}
	for (index = 0; index < NUMBEROF(extraction_runs); index++)
	{
		player = player_try_and_get(extraction_runs[index].player);
		if (extraction_runs[index].player != NONE && (!player || player->quit_out_of_game)) extraction_run_close(&extraction_runs[index]);
	}
	extraction_transport_readiness();
	extraction_transport_completions();
	for (index = 0; index < NUMBEROF(extraction.squads); index++)
		if (!extraction.survival && extraction.squads[index].operation && !extraction.squads[index].notified &&
			!extraction_squad_busy(index, NULL))
		{
			char script[32], text[128];
			boolean banked = extraction.squads[index].banked;
			extraction.squads[index].notified = TRUE;
			if (!banked && extraction.squads[index].unconfirmed_escape) continue;
			snprintf(script, sizeof(script), "forerunner_squad_%02d_%s", index + 1, banked ? "complete" : "aborted");
			platform_log("extraction: squad %d operation %lu settled; authored %s %s", index + 1,
				extraction.squads[index].operation, banked ? "completion" : "abort",
				extraction_wake_mission_script(script, TRUE) ? "woken" : "absent or unsupported");
			snprintf(text, sizeof(text), "SQUAD %02d READY | %s Redeploy at the hub.", index + 1,
				banked ? "Survivors banked." : "Operation lost.");
			extraction_squad_notice(index, text);
		}
	/* The optional authored continuation runs once per fresh map, after actual
	 * native consumption and durable banking for every living survivor. It
	 * owns objectives only; gear remains owned by the existing settlement. */
	if (!extraction.survival && !extraction.completion_notified && extraction_players_returned())
	{
		extraction.completion_notified = TRUE;
		if (extraction_wake_mission_script("forerunner_extraction_complete", FALSE))
			platform_log("extraction: all survivors banked; authored completion woken");
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
		if (player && !player->quit_out_of_game && (run->phase == _extraction_deployed || run->phase == _extraction_activating ||
			run->phase == _extraction_ready || run->phase == _extraction_settling)) return TRUE;
	}
	return FALSE;
}

static boolean extraction_players_returned(void)
{
	struct data_iterator iterator;
	struct player_datum *player;
	short survivors = 0;
	data_iterator_new(&iterator, player_data);
	while ((player = (struct player_datum *)data_iterator_next(&iterator)) != NULL)
	{
		long slot = DATUM_INDEX_TO_ABSOLUTE_INDEX(iterator.datum_index);
		struct unit_datum *unit;
		if (player->quit_out_of_game) continue;

		if (player->unit_index == NONE) continue;
		unit = (struct unit_datum *)object_try_and_get_and_verify_type(player->unit_index, _object_mask_biped);
		if (!unit || unit->unit.player_index != iterator.datum_index) return FALSE;
		if (TEST_FLAG(unit->object.damage_flags, _object_dead_bit)) continue;
		if (extraction_unit_pending_death(unit)) return FALSE;
		if (!VALID_INDEX(slot, NUMBEROF(extraction_runs)) || extraction_runs[slot].player != iterator.datum_index ||
			extraction_runs[slot].phase != _extraction_returned || extraction_medical_incapacitated(player->unit_index) ||
			!extraction_at_return(&extraction_runs[slot], player->unit_index)) return FALSE;
		survivors++;
	}
	return survivors > 0;
}

boolean extraction_survival_evacuation_ready(void)
{
	return extraction.survival && extraction.admitted && extraction.evacuation && !extraction.suspended &&
		extraction_requested() && coop_enemies_firefight_outcome() == 0 && extraction_players_returned();
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

boolean extraction_medical_ally_incapacitated(long viewer_unit, long target_unit)
{
	struct unit_datum *viewer, *target;
	struct player_datum *player;
	if (!game_in_progress() || cinematic_in_progress() || viewer_unit == target_unit ||
		!extraction_medical_incapacitated(target_unit) || extraction_medical_incapacitated(viewer_unit)) return FALSE;
	viewer = (struct unit_datum *)object_try_and_get_and_verify_type(viewer_unit, _object_mask_biped);
	/* A living client's tiny positive health can quantize to zero. Native
	 * death flags and received incapacitation, not a positive-health guess,
	 * decide whether this owned body can see allies who need assistance. */
	if (!viewer || viewer->unit.player_index == NONE || !(viewer->object.body_vitality >= 0.f) ||
		TEST_FLAG(viewer->object.damage_flags, _object_dead_bit) ||
		extraction_unit_pending_death(viewer)) return FALSE;
	player = player_try_and_get(viewer->unit.player_index);
	if (!player || player->quit_out_of_game || player->unit_index != viewer_unit) return FALSE;
	target = unit_get(target_unit);
	return !extraction_unit_pending_death(target) &&
		!game_team_is_enemy(viewer->object.owner_team_index, target->object.owner_team_index);
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
	struct data_iterator iterator;
	struct player_datum *ally;
	char message[128];
	long slot;
	if (!extraction_medical_host()) return FALSE;
	unit = (struct unit_datum *)object_try_and_get_and_verify_type(unit_index, _object_mask_biped);
	if (!unit || unit->object.parent_object_index != NONE || !unit_controllable(unit_index) ||
		!(unit->object.body_vitality < 0.f) || unit->object.body_vitality < -1.f ||
		TEST_FLAG(unit->object.damage_flags, _object_dead_bit) ||
		extraction_unit_pending_death(unit)) return FALSE;
	slot = extraction_medical_slot(unit_index);
	if (slot == NONE || medical_players[slot].used) return FALSE;
	medical_players[slot].used = medical_players[slot].downed = TRUE;
	medical_players[slot].deadline = game_time_get() + medical_bleed_seconds * TICKS_PER_SECOND;
	medical_players[slot].last_damage = game_time_get();
	unit->object.body_vitality = 0.f;
	object_deplete_shield(unit_index);
	platform_log("medical: player %ld incapacitated alive; native unit %ld", slot, unit_index);
	snprintf(message, sizeof(message), "DOWNED | %lds to bleedout. Ally: hold Action nearby.",
		medical_bleed_seconds);
	extraction_player_notice(medical_players[slot].player, message);
	data_iterator_new(&iterator, player_data);
	while ((ally = (struct player_datum *)data_iterator_next(&iterator)) != NULL)
		if (extraction_medical_ally_incapacitated(ally->unit_index, unit_index))
			extraction_player_notice(iterator.datum_index, "ALLY DOWN | Hold Action nearby for biofoam. Avoid combat.");
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
	long slot, target, choice = NONE, now = game_time_get();
	real nearest = 0.f;
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
	if (TEST_FLAG(unit->object.damage_flags, _object_dead_bit) || extraction_unit_pending_death(unit) ||
		!unit_controllable(player->unit_index) ||
		unit_is_busy(player->unit_index) || (action->desired_weapon_index != NONE &&
		action->desired_weapon_index != unit->unit.current_weapon_index)) return;
	for (target = 0; target < NUMBEROF(medical_players); target++)
	{
		struct unit_datum *casualty;
		real_vector3d delta;
		real distance;
		boolean continuing;
		if (!medical_players[target].downed || !extraction_medical_incapacitated(medical_players[target].unit) ||
			extraction_unit_pending_death(unit_get(medical_players[target].unit)) ||
			now - medical_players[target].last_damage < 2 * TICKS_PER_SECOND) continue;
		continuing = medical_players[target].healer == player_index && medical_players[target].healer_unit == player->unit_index &&
			now - medical_players[target].last_control <= 1;
		if (!continuing && medical_players[target].healer != NONE && now - medical_players[target].last_control <= 1) continue;
		casualty = unit_get(medical_players[target].unit);
		delta.i = casualty->object.bounding_sphere_center.x - unit->object.bounding_sphere_center.x;
		delta.j = casualty->object.bounding_sphere_center.y - unit->object.bounding_sphere_center.y;
		delta.k = casualty->object.bounding_sphere_center.z - unit->object.bounding_sphere_center.z;
		distance = delta.i * delta.i + delta.j * delta.j + delta.k * delta.k;
		if (!(distance <= 2.25f) || (!continuing && choice != NONE && distance >= nearest) ||
			!extraction_medical_reach(player->unit_index, medical_players[target].unit)) continue;
		choice = target; nearest = distance;
		/* Keep a valid treatment stable when another casualty approaches.
		 * A new hold selects the nearest available ally, not the lowest slot. */
		if (continuing) break;
	}
	if (choice == NONE) return;
	target = choice;
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
		snprintf(message, sizeof(message), "BIOFOAM | %ld/%ld seconds. Hold Action; stay close, avoid combat.",
			medical_players[target].progress / TICKS_PER_SECOND, medical_seconds);
		extraction_player_notice(player_index, message);
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
			if (!extraction_unit_pending_death(unit))
				platform_log("medical: player %ld pending native death; revive refused", index);
			unit_kill(medical_players[index].unit); continue; /* Native death/loot, never resurrect a corpse. */
		}
		/* Host-clock reminders use existing private notices, not a client
		 * countdown or another replicated medical timer. */
		{
			long remaining = medical_players[index].deadline - now;
			if (!cinematic_in_progress() && !extraction_unit_pending_death(unit) &&
				remaining > 0 && remaining < medical_bleed_seconds * TICKS_PER_SECOND &&
				remaining % TICKS_PER_SECOND == 0 &&
				(remaining <= 5 * TICKS_PER_SECOND || remaining % (5 * TICKS_PER_SECOND) == 0))
			{
				char message[128];
				snprintf(message, sizeof(message), "BLEEDOUT | %ld seconds. An ally must hold Action nearby.", remaining / TICKS_PER_SECOND);
				extraction_player_notice(medical_players[index].player, message);
			}
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
				extraction_unit_pending_death(unit_get(healer->unit_index)) || extraction_unit_pending_death(unit) ||
				!extraction_medical_reach(healer->unit_index, medical_players[index].unit))
			{
				extraction_player_notice(medical_players[index].healer, "BIOFOAM INTERRUPTED | Hold Action again nearby after combat.");
				medical_players[index].healer = NONE; medical_players[index].progress = 0;
			}
			else if (medical_players[index].progress >= medical_seconds * TICKS_PER_SECOND)
			{
				unit->object.body_vitality = .25f;
				medical_players[index].downed = FALSE;
				platform_log("medical: player %ld revived alive by player %ld; native unit preserved", index,
					DATUM_INDEX_TO_ABSOLUTE_INDEX(medical_players[index].healer));
				extraction_player_notice(medical_players[index].player, "STABILIZED | Back on your feet. Another lethal injury is fatal.");
				extraction_player_notice(medical_players[index].healer, "BIOFOAM COMPLETE | Ally stabilized. Native health rules resume.");
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
