"""Connected real Extraction, gear transactions and native equipment helpers.

The bounded world/identity/OS adapters are fixtures, not native multiplayer proof.
Real native file/process behavior is covered by tools/test_gear_files.py.
"""
import re
import pytest

from harness import CHECK_FAILED, build, constant, enum_with, function, mutated, read, run, structure
from harness.tests.test_gear import generated as gear_generated


def generated(fault=None, outcome_fault=False, medical_fault=False):
    files = list(gear_generated())
    files.append(('reload_queue.inc', function(read('source/game/player_queues_new.c'), 'update_client_loadout_modifier_held')))
    inventory = read('port/linux/game/game_modes.c')
    names = ('capture', 'grenade_room', 'weapon_compatible', 'loadout_valid', 'starting_profile', 'stage', 'weapon_for_unit', 'apply', 'revoke', 'consume')
    equipment = '\n'.join(function(inventory, 'game_modes_inventory_' + name) for name in names)
    equipment += '\n' + function(inventory, 'game_modes_player_change_biped')
    if fault:
        equipment = mutated(equipment, *fault)
    files.append(('equipment.inc', equipment))
    files.append(('equipment_flags.inc', enum_with(read('source/items/weapon_definitions.h'), '_weapon_must_be_readied_bit')))
    files.append(('native_teams.inc', enum_with(read('source/game/game.h'), 'NUMBER_OF_SOLO_CAMPAIGN_TEAMS,')))
    files.append(('profile_types.inc', structure(read('source/scenario/scenario_definitions.h'), 'scenario_starting_profile_weapon') +
                  '\n' + structure(read('source/scenario/scenario_definitions.h'), 'scenario_starting_profile')))
    files.append(('body_types.inc', enum_with(read('source/game/players.h'), 'NUMBER_OF_PLAYER_POWERUPS') + '\n' +
                  enum_with(read('source/game/players.h'), '_player_action_result_reload') + '\n' +
                  enum_with(read('source/units/units.h'), '_unit_active_camouflaged_bit') + '\n' +
                  enum_with(read('source/scenario/scenario_definitions.h'), '_scenario_object_placement_not_automatic_bit') + '\n' +
                  structure(read('source/scenario/scenario_definitions.h'), 'scenario_object_datum') + '\n' +
                  structure(read('source/scenario/scenario_definitions.h'), 'scenario_object_palette_entry')))
    files.append(('body_binding.inc', '\n'.join(function(read('source/game/players.c'), name) for name in (
        'network_player_reconcile_coop_team', 'network_player_attach_unit', 'network_player_detach_unit', 'players_set_local_player_unit'))))
    files.append(('equipment_ownership.inc', enum_with(read('source/items/items.h'), '_item_attached_to_unit_bit') +
                  '\n' + enum_with(read('source/objects/objects.h'), '_object_connected_to_map_bit')))
    files.append(('mission_types.inc', enum_with(read('source/hs/hs_scenario_definitions.h'), '_hs_script_startup') +
                  '\n' + enum_with(read('source/hs/hs.h'), '_hs_unparsed') +
                  '\n' + structure(read('source/hs/hs_scenario_definitions.h'), 'hs_script')))
    director = read('port/linux/game/coop_enemies.c')
    survival = '\n'.join(function(director, name) for name in (
        'coop_enemies_host', 'coop_enemies_firefight_active',
        'coop_enemies_firefight_gear_active', 'coop_enemies_firefight_endless', 'coop_enemies_firefight_outcome'))
    if outcome_fault:
        survival = mutated(survival, 'firefight.victory ? 1 : -1', '1')
    files.append(('survival.inc', survival))
    files.append(('survival_lives.inc', '\n'.join(function(director, name) for name in (
        'firefight_respawn_reserved', 'coop_enemies_firefight_can_spawn', 'coop_enemies_firefight_out_of_lives',
        'coop_enemies_firefight_player_died', 'coop_enemies_firefight_player_spawned', 'firefight_check_defeat'))))
    files.append(('survival_notices.inc', function(director, 'firefight_notice') + '\n' + function(director, 'firefight_equipment_reports')))
    extraction = re.sub(r'^#include.*\n', '',
        read('port/linux/game/extraction.h') + '\n' + read('port/linux/game/extraction.c'), flags=re.M)
    if medical_fault:
        extraction = mutated(extraction,
            'now - medical_players[healer_slot].last_damage < 2 * TICKS_PER_SECOND ||', '')
    files.append(('extraction.inc', extraction))
    files.append(('evacuation.inc', function(director, 'firefight_try_evacuation')))
    hud = read('source/interface/hud_nav_points.c')
    files.append(('medical_hud_types.inc', structure(hud, 'hud_waypoint_arrow') +
                  '\n#define MAXIMUM_NUMBER_OF_NAV_POINTS %d' % constant(read('source/interface/hud.h'), 'MAXIMUM_NUMBER_OF_NAV_POINTS')))
    files.append(('medical_hud.inc', function(hud, 'hud_render_medical_nav_points')))
    devices = read('source/devices/devices.c')
    controls = read('source/devices/device_controls.c')
    files.append(('device_binding.inc', structure(devices, 'scenario_device_group') + '\n' +
                  function(devices, 'device_group_find_named')))
    files.append(('controls_types.inc', enum_with(devices, '_device_not_usable_bit')))
    files.append(('vehicle_landing_types.inc', enum_with(read('source/units/vehicles.h'), '_vehicle_hovering_bit')))
    files.append(('vehicle_landing.inc', function(read('source/units/vehicles.c'), 'vehicle_is_landed')))
    files.append(('native_seat_animation_types.inc', enum_with(read('source/units/units.h'), '_unit_seat_animation_airborne_dead')))
    files.append(('native_seats.inc', '\n'.join(function(read('source/units/units.c'), name) for name in (
        'unit_can_enter_seat', 'unit_get_seat_entrance_point'))))
    files.append(('controls.inc', '\n'.join((function(devices, 'device_can_change_position'),
                                            function(controls, 'control_toggle'), function(controls, 'control_touched')))))
    return tuple(files)


@pytest.mark.parametrize('case', ['loop', 'loadout-choice', 'loadout-secondary', 'loadout-secondary-pending', 'loadout-secondary-unavailable', 'loadout-secondary-release', 'stash-discard', 'discard-pending', 'discard-state', 'discard-identity',
                                 'notice-complete', 'notice-routing', 'notice-controller', 'notice-broadcast', 'notice-fallback',
                                 'stowed-loop', 'stowed-owner', 'stowed-ground', 'stowed-unmarked', 'stowed-nonplayer', 'stowed-parent',
                                 'biped-first', 'biped-second', 'biped-survival',
                                 'initial-ammo', 'refund-disconnect', 'refund-recycled', 'refund-map-exit', 'refund-retry',
                                 'refund-disconnect-outage', 'refund-recycled-outage', 'refund-map-outage',
                                 'confirmation-outage', 'confirmation-published',
                                 'reservation-published',
                                 'transfer', 'pending-deploy', 'pending-escape', 'grant-failure', 'authority', 'native-controls',
                                 'account-host', 'account-host-unbound', 'account-host-role',
                                 'mission-complete', 'mission-pending', 'mission-signature', 'mission-return-type', 'mission-restore',
                                 'shared-cache', 'shared-cache-reject', 'shared-cache-failure', 'shared-cache-discard',
                                 'restore', 'pending', 'binding', 'disconnect',
                                 'survival-win', 'survival-loss', 'survival-retry',
                                 'survival-restore', 'survival-respawn',
                                 'evac-loop', 'evac-pending', 'evac-presence', 'evac-binding',
                                 'evac-finite-win', 'evac-finite-loss', 'evac-no-survivors',
                                 'medical-authority', 'medical-revive', 'medical-interrupt',
                                 'medical-target-nearest', 'medical-target-continue',
                                 'medical-target-busy', 'medical-target-interrupt',
                                 'medical-pending-target', 'medical-pending-healer',
                                 'medical-pending-lethal', 'medical-pending-final',
                                 'medical-guidance', 'medical-guidance-client', 'medical-guidance-budget',
                                 'medical-guidance-styles', 'medical-guidance-countdown',
                                 'medical-bleed', 'medical-replica', 'medical-restore', 'medical-gear'])
def test_extraction_workflow(case):
    status, output = run(build('extraction', generated()), case)
    assert status == 0, output


@pytest.mark.parametrize('case', ['paid', 'downed', 'pending', 'eliminated', 'owner', 'living', 'recycled'])
def test_survival_wipe(case):
    status, output = run(build('extraction', generated()), 'ff-wipe-' + case)
    assert status == 0, output


@pytest.mark.parametrize('before,after,case', [
    ('firefight.wipe_deadline = NONE;', 'firefight.wipe_deadline = 0;', 'downed'),
    ('firefight.wipe_deadline <= 0', 'firefight.wipe_deadline == 0', 'eliminated'),
    ('unit->unit.player_index == iterator.datum_index &&', '', 'owner'),
])
def test_survival_wipe_negative(before, after, case):
    files = tuple((name, mutated(text, before, after)) if name == 'survival_lives.inc' else (name, text)
                  for name, text in generated())
    status, output = run(build('extraction', files), 'ff-wipe-' + case)
    assert status == CHECK_FAILED, output


@pytest.mark.parametrize('case', ['chat', 'fallback', 'outcome'])
def test_survival_notices(case):
    status, output = run(build('extraction', generated()), 'ff-notice-' + case)
    assert status == 0, output


@pytest.mark.parametrize('case', ['state', 'faction', 'same', 'separate', 'recycled', 'dead', 'leave', 'restored'])
def test_native_squad_radio(case):
    status, output = run(build('extraction', generated()), 'squad-radio-' + case)
    assert status == 0, output


@pytest.mark.parametrize('case', [
    'squad-loop', 'squad-independent', 'squad-pending', 'squad-loss', 'squad-restore',
    'squad-capacity', 'squad-state', 'squad-identity', 'squad-faction', 'squad-leave',
    'squad-duplicate', 'squad-alias', 'squad-missing',
])
def test_native_squad_workflow(case):
    status, output = run(build('extraction', generated()), case)
    assert status == 0, output


def test_secondary_current_input_negative_control():
    files = tuple((name, mutated(text,
        'update_client_loadout_modifier_held(unit->unit.player_index) ? 1 : 0',
        'TEST_FLAG(unit->unit.control_flags, _unit_control_weapon_reload_bit) ? 1 : 0'))
                  if name == 'extraction.inc' else (name, text) for name, text in generated())
    status, output = run(build('extraction', files), 'loadout-secondary')
    assert status == CHECK_FAILED, output


@pytest.mark.parametrize('case', ['bank', 'loss', 'disconnect', 'pending', 'storage'])
def test_starter_deployment(case):
    status, output = run(build('extraction', generated()), 'starter-run-' + case)
    assert status == 0, output


@pytest.mark.parametrize('case', ['unsc', 'covenant', 'refund'])
def test_starter_faction_kit(case):
    status, output = run(build('extraction', generated()), 'starter-kit-' + case)
    assert status == 0, output


@pytest.mark.parametrize('case', [
    'loadout-secondary', 'loadout-secondary-pending', 'loadout-secondary-release', 'loadout-secondary-unavailable',
    'starter-run-bank', 'starter-run-loss', 'starter-run-disconnect', 'starter-run-pending',
    'starter-kit-unsc', 'starter-kit-covenant', 'starter-kit-refund',
])
def test_controller_loadout_modifier(case):
    status, output = run(build('extraction', generated()), 'light:' + case)
    assert status == 0, output


def test_controller_loadout_negative_control():
    files = tuple((name, mutated(text,
        'FLAG(_unit_control_weapon_reload_bit) | FLAG(_unit_control_integrated_light_bit)',
        'FLAG(_unit_control_weapon_reload_bit)')) if name == 'reload_queue.inc' else (name, text)
                  for name, text in generated())
    status, output = run(build('extraction', files), 'light:loadout-secondary')
    assert status == CHECK_FAILED, output


def test_starter_deployment_negative_control():
    files = tuple((name, mutated(text,
        '!update_client_loadout_modifier_held(unit->unit.player_index)', 'TRUE'))
                  if name == 'extraction.inc' else (name, text) for name, text in generated())
    status, output = run(build('extraction', files), 'starter-run-loss')
    assert status == CHECK_FAILED, output


@pytest.mark.parametrize('case', [
    'squad-query-loop', 'squad-query-state', 'squad-query-policy', 'squad-query-storage',
    'squad-objective-loop', 'squad-objective-escrow', 'squad-objective-alias', 'squad-objective-duplicate',
])
def test_squad_native_objectives(case):
    status, output = run(build('extraction', generated()), case)
    assert status == 0, output


@pytest.mark.parametrize('before,after,case', [
    ('if (run->squad != squad || run->squad_operation != extraction.squads[squad].operation ||',
     'if (FALSE || run->squad_operation != extraction.squads[squad].operation ||', 'squad-query-loop'),
    ('if (control->object.name_index == extraction.activate_name && !extraction_squad_objectives_ready(run))',
     'if (FALSE)', 'squad-objective-loop'),
])
def test_squad_objective_negative_control(before, after, case):
    files = tuple((name, mutated(text, before, after)) if name == 'extraction.inc' else (name, text)
                  for name, text in generated())
    status, output = run(build('extraction', files), case)
    assert status == CHECK_FAILED, output


@pytest.mark.parametrize('before,after,case', [
    ('members >= EXTRACTION_SQUAD_MEMBERS', 'members > EXTRACTION_SQUAD_MEMBERS', 'squad-capacity'),
    ('if (run->player != iterator.datum_index || run->squad != squad ||',
     'if (run->player != iterator.datum_index || FALSE ||', 'squad-independent'),
])
def test_native_squad_negative_control(before, after, case):
    files = tuple((name, mutated(text, before, after)) if name == 'extraction.inc' else (name, text)
                  for name, text in generated())
    status, output = run(build('extraction', files), case)
    assert status == CHECK_FAILED, output


@pytest.mark.parametrize('case', [
    'transport-loop', 'transport-covenant', 'transport-binding', 'transport-duplicate',
    'transport-landed-loop', 'transport-landed-alias',
    'transport-admission-factions', 'transport-admission-npc', 'transport-admission-authority',
    'transport-admission-owner', 'transport-admission-death', 'transport-admission-ordinary', 'transport-admission-occupied',
    'transport-missing-faction', 'transport-seat', 'transport-stale', 'transport-wrong-faction',
    'transport-dead', 'transport-disembark', 'transport-team-change', 'transport-replaced',
    'transport-crash-pending', 'transport-crash-silent', 'transport-crash-no-statistics', 'transport-crash-hub',
    'transport-callback-other', 'transport-callback-field', 'transport-callback-crew',
    'transport-callback-pending', 'transport-callback-signature', 'transport-callback-replaced',
    'transport-callback-continuous', 'transport-recall', 'transport-recall-state', 'transport-recall-missing',
    'transport-boarded-hub', 'transport-boarded-lz', 'transport-boarded-other',
    'transport-boarded-pending', 'transport-boarded-ship', 'transport-boarded-identity', 'transport-boarded-duplicate',
    'transport-boarded-capacity', 'transport-boarded-missing', 'transport-boarded-alias',
    'transport-boarded-squad-field', 'transport-boarded-squad-hub',
    'transport-boarded-squad-wait', 'transport-boarded-squad-mixed',
    'transport-boarded-squad-stale',
])
def test_native_transport_settlement(case):
    status, output = run(build('extraction', generated()), case)
    assert status == 0, output


def test_native_transport_crash_negative_control():
    files = tuple((name, mutated(text, '\n\t\textraction_unit_pending_death(vehicle) ||', ''))
                  if name == 'extraction.inc' else (name, text) for name, text in generated())
    status, output = run(build('extraction', files), 'transport-crash-hub')
    assert status == CHECK_FAILED, output


@pytest.mark.parametrize('case', [
    'transport-called-loop', 'transport-called-ownership', 'transport-called-duplicate',
    'transport-called-capacity', 'transport-called-missing', 'transport-called-alias',
])
def test_native_faction_transport_calls(case):
    status, output = run(build('extraction', generated()), case)
    assert status == 0, output


@pytest.mark.parametrize('before,after,case', [
    ('extraction_faction((short)player->team_index) != faction) continue;', 'FALSE) continue;', 'transport-called-ownership'),
    ('(run->phase != _extraction_activating && run->phase != _extraction_ready) || run->gear.pending',
     '(run->phase != _extraction_ready) || run->gear.pending', 'transport-called-loop'),
])
def test_native_faction_transport_call_negative_control(before, after, case):
    files = tuple((name, mutated(text, before, after)) if name == 'extraction.inc' else (name, text)
                  for name, text in generated())
    status, output = run(build('extraction', files), case)
    assert status == CHECK_FAILED, output


@pytest.mark.parametrize('before,after,case', [
    ('if (!returned || pending) continue;', 'if (!returned || (pending && FALSE)) continue;', 'transport-callback-pending'),
    ('if (run->dropship != ship) continue;', 'if (FALSE) continue;', 'transport-callback-other'),
])
def test_native_transport_release_negative_control(before, after, case):
    files = tuple((name, mutated(text, before, after)) if name == 'extraction.inc' else (name, text)
                  for name, text in generated())
    status, output = run(build('extraction', files), case)
    assert status == CHECK_FAILED, output


@pytest.mark.parametrize('file,before,after,case', [
    ('extraction.inc', 'extraction_player_dropship(unit) != ship ||', 'FALSE ||', 'transport-boarded-hub'),
    ('extraction.inc', 'if (at_lz && run->phase == _extraction_activating) return FALSE;',
     'if (FALSE) return FALSE;', 'transport-boarded-lz'),
    ('device_binding.inc', 'found != NONE || index >= device_groups_data->maximum_count',
     'index >= device_groups_data->maximum_count', 'transport-boarded-duplicate'),
])
def test_native_transport_boarding_negative_control(file, before, after, case):
    files = tuple((name, mutated(text, before, after)) if name == file else (name, text)
                  for name, text in generated())
    status, output = run(build('extraction', files), case)
    assert status == CHECK_FAILED, output


@pytest.mark.parametrize('case', [
    'seat-entrance-exit', 'seat-entrance-normal', 'seat-entrance-normal-marker', 'seat-entrance-npc',
    'seat-entrance-corrupt', 'seat-entrance-missing', 'seat-entrance-zero', 'seat-entrance-marker',
])
def test_native_seat_entrance_workflow(case):
    status, output = run(build('extraction', generated()), case)
    assert status == 0, output


@pytest.mark.parametrize('case', ['view', 'discard', 'cancel', 'pending', 'expired', 'stale', 'fallback'])
def test_native_stash_browser(case):
    status, output = run(build('extraction', generated()), 'stash-browser-' + case)
    assert status == 0, output


def test_native_stash_preview_negative_control():
    files = tuple((name, mutated(text, 'run->discard_generation != run->gear.record.generation ||', ''))
                  if name == 'extraction.inc' else (name, text) for name, text in generated())
    status, output = run(build('extraction', files), 'stash-browser-stale')
    assert status == CHECK_FAILED, output


@pytest.mark.parametrize('case', [
    'debrief-empty', 'debrief-success', 'debrief-loss', 'debrief-pending', 'debrief-auto',
    'debrief-stats-reset', 'debrief-owner', 'debrief-remote', 'debrief-restart',
    'debrief-redeploy', 'debrief-field', 'debrief-battery', 'debrief-fallback',
])
def test_native_debrief_workflow(case):
    status, output = run(build('extraction', generated()), case)
    assert status == 0, output


@pytest.mark.parametrize('before,after,case', [
    ('if (run->gear.pending || run->phase == _extraction_settling || run->phase == _extraction_losing ||\n'
     '\t\trun->phase == _extraction_cancelling)', 'if (FALSE)', 'debrief-pending'),
    ('if (report->staged && !memcmp(report->run, run->gear.run, GEAR_ID_BYTES)) return;',
     'if (FALSE) return;', 'debrief-pending'),
    ('if (definition != NONE &&\n\t\t\t\tTEST_FLAG',
     'if (definition != NONE && !weapon_definition_get(definition)->weapon.magazines.count &&\n\t\t\t\tTEST_FLAG',
     'debrief-battery'),
])
def test_native_debrief_negative_control(before, after, case):
    files = tuple((name, mutated(text, before, after)) if name == 'extraction.inc' else (name, text)
        for name, text in generated())
    status, output = run(build('extraction', files), case)
    assert status == CHECK_FAILED, output


def test_native_seat_entrance_negative_control():
    files = tuple((name, mutated(text,
        'if (animation_index == NONE && unit->unit.player_index != NONE &&',
        'if (FALSE &&')) if name == 'native_seats.inc' else (name, text)
        for name, text in generated())
    status, output = run(build('extraction', files), 'seat-entrance-exit')
    assert status == CHECK_FAILED, output


def test_native_transport_admission_negative_control():
    files = tuple((name, mutated(text,
        'if (!extraction_transport_can_board(unit_index, target_unit_index))',
        'if (FALSE)')) if name == 'native_seats.inc' else (name, text)
        for name, text in generated())
    status, output = run(build('extraction', files), 'transport-admission-factions')
    assert status == CHECK_FAILED, output


@pytest.mark.parametrize('before,after,case', [
    ('control->object.name_index == extraction.activate_name && run->phase == _extraction_ready',
     'control->object.name_index == extraction.activate_name && FALSE', 'transport-recall'),
    ('(reusable && script->script_type == _hs_script_continuous)',
     '(script->script_type == _hs_script_continuous)', 'mission-signature'),
])
def test_native_transport_cycle_negative_control(before, after, case):
    files = tuple((name, mutated(text, before, after)) if name == 'extraction.inc' else (name, text)
                  for name, text in generated())
    status, output = run(build('extraction', files), case)
    assert status == CHECK_FAILED, output


@pytest.mark.parametrize('case', [
    'faction-choice', 'faction-state', 'faction-kit', 'faction-saved',
    'faction-confirm', 'faction-reserve', 'faction-invalid',
    'faction-binding', 'faction-duplicate', 'faction-alias', 'faction-transport', 'faction-transport-missing',
])
def test_native_faction_workflow(case):
    status, output = run(build('extraction', generated()), case)
    assert status == 0, output


@pytest.mark.parametrize('case', [
    'body-loop', 'body-local', 'body-allocation', 'body-preview', 'body-transfer',
    'body-binding', 'body-placement', 'body-palette', 'body-duplicate', 'body-profile',
    'body-spawn', 'body-flag', 'body-item', 'body-stash',
])
def test_native_faction_body_workflow(case):
    status, output = run(build('extraction', generated()), case)
    assert status == 0, output


def test_native_faction_body_owner_negative_control():
    status, output = run(build('extraction', generated(fault=(
        'item_in_unit_inventory(next->unit.weapon_object_indices[index], replacement);',
        '/* lost ownership refresh */'))), 'body-loop')
    assert status == CHECK_FAILED, output


def test_native_faction_body_stash_negative_control():
    files = tuple((name, mutated(text,
        'if (session->loadout_unit != NONE && !game_modes_inventory_weapon_for_unit(session->loadout_unit, &item->equipment)) return FALSE;',
        '/* body admission omitted */')) if name == 'under_test.inc' else (name, text)
        for name, text in generated())
    status, output = run(build('extraction', files), 'body-stash')
    assert status == CHECK_FAILED, output


def test_faction_starter_grenade_stacking_negative_control():
    status, output = run(build('extraction', generated(fault=(
        'next.grenades[index] = MAX(next.grenades[index], starter);',
        'next.grenades[index] += starter;'))), 'faction-saved')
    assert status == CHECK_FAILED, output


def test_native_transport_cinematic_negative_control():
    files = tuple((name, mutated(text,
        'return !cinematic_in_progress() && extraction_at_return(run, run->unit);',
        'return extraction_at_return(run, run->unit);')) if name == 'extraction.inc' else (name, text)
        for name, text in generated())
    status, output = run(build('extraction', files), 'transport-loop')
    assert status == CHECK_FAILED, output


def test_stowed_owner_negative_control():
    status, output = run(build('extraction', generated((
        'weapon->object.owner_player_index != player_index', 'FALSE'))), 'stowed-owner')
    assert status == CHECK_FAILED, output


def test_notice_tail_negative_control():
    files = tuple((name, mutated(text,
        '"P%d | %.127s"',
        '"P%d | %.57s"')) if name == 'extraction.inc' else (name, text)
        for name, text in generated())
    status, output = run(build('extraction', files), 'notice-routing')
    assert status == CHECK_FAILED, output


def test_notice_hud_tail_negative_control():
    files = tuple((name, mutated(text,
        'while (count > 0) hud_print_message(local, lines[--count]);',
        'hud_print_message(local, lines[0]);')) if name == 'extraction.inc' else (name, text)
        for name, text in generated())
    status, output = run(build('extraction', files), 'notice-fallback')
    assert status == CHECK_FAILED, output


def test_transfer_negative_control():
    status, output = run(build('extraction', generated((
        'weapon->object.parent_object_index != player->unit_index &&',
        'FALSE &&'))), 'transfer')
    assert status == CHECK_FAILED, output


def test_mission_completion_negative_control():
    files = tuple((name, mutated(text,
                  'extraction_runs[slot].phase != _extraction_returned', 'FALSE'))
                  if name == 'extraction.inc' else (name, text) for name, text in generated())
    status, output = run(build('extraction', files), 'mission-complete')
    assert status == CHECK_FAILED, output


@pytest.mark.parametrize('file,before,after,case', [
    ('equipment.inc', 'FLAG(_object_dead_bit) | FLAG(_object_die_act_of_god_bit) |\n'
     '\t\t FLAG(_object_die_act_of_god_silent_bit) | FLAG(_object_die_act_of_god_no_statistics_bit)',
     'FLAG(_object_dead_bit)', 'pending-escape'),
    ('extraction.inc', 'FLAG(_object_die_act_of_god_silent_bit) | FLAG(_object_die_act_of_god_no_statistics_bit)',
     '0', 'medical-pending-lethal'),
    ('extraction.inc', 'extraction_unit_pending_death(unit_get(healer->unit_index)) || ',
     '', 'medical-pending-final'),
])
def test_pending_native_death_negative_control(file, before, after, case):
    files = tuple((name, mutated(text, before, after)) if name == file else (name, text)
                  for name, text in generated())
    status, output = run(build('extraction', files), case)
    assert status == CHECK_FAILED, output


def test_biped_admission_negative_control():
    status, output = run(build('extraction', generated((
        '!unit_can_use_weapon(unit_index, created[index])',
        'FALSE'))), 'biped-second')
    assert status == CHECK_FAILED, output


@pytest.mark.parametrize('file,before,after,case', [
    ('under_test.inc', 'if (session->record.phase == _gear_reserved && !gear_cancel_reservation(session)) goto failed;',
     '', 'refund-disconnect-outage'),
    ('extraction.inc', 'game_modes_inventory_revoke(player->unit_index, &loadout, created);',
     '', 'confirmation-outage'),
    ('under_test.inc', 'return session->record.phase == _gear_deployed;',
     'return !session->pending && session->record.phase == _gear_deployed;', 'confirmation-published'),
])
def test_native_confirmation_negative_control(file, before, after, case):
    files = tuple((name, mutated(text, before, after)) if name == file else (name, text)
                  for name, text in generated())
    status, output = run(build('extraction', files), case)
    assert status == CHECK_FAILED, output


@pytest.mark.parametrize('file,before,after,case', [
    ('extraction.inc', '!(viewer->object.body_vitality >= 0.f)',
     '!(viewer->object.body_vitality > 0.f)', 'medical-guidance-client'),
    ('extraction.inc', '!game_team_is_enemy(viewer->object.owner_team_index, target->object.owner_team_index)',
     'TRUE', 'medical-guidance'),
    ('medical_hud_types.inc', '#define MAXIMUM_NUMBER_OF_NAV_POINTS 4',
     '#define MAXIMUM_NUMBER_OF_NAV_POINTS 8', 'medical-guidance-budget'),
    ('medical_hud.inc', 'custom_render_nav_point(local_player_index, &nearest[index].position, arrow, screen);',
     '(void)screen;', 'medical-guidance'),
    ('extraction.inc', '(remaining <= 5 * TICKS_PER_SECOND || remaining % (5 * TICKS_PER_SECOND) == 0)',
     'TRUE', 'medical-guidance-countdown'),
])
def test_medical_guidance_negative_control(file, before, after, case):
    files = tuple((name, mutated(text, before, after)) if name == file else (name, text)
                  for name, text in generated())
    status, output = run(build('extraction', files), case)
    assert status == CHECK_FAILED, output


@pytest.mark.parametrize('before,after,case', [
    ('distance >= nearest', 'distance >= nearest - 100.f', 'medical-target-nearest'),
    ('continuing = medical_players[target].healer == player_index',
     'continuing = FALSE && medical_players[target].healer == player_index', 'medical-target-continue'),
])
def test_medical_target_negative_control(before, after, case):
    files = tuple((name, mutated(text, before, after)) if name == 'extraction.inc' else (name, text)
                  for name, text in generated())
    status, output = run(build('extraction', files), case)
    assert status == CHECK_FAILED, output


def test_initial_ammo_negative_control():
    status, output = run(build('extraction', generated((
        'MAX(capacity->rounds_total_maximum, capacity->rounds_total_initial)',
        'capacity->rounds_total_maximum'))), 'initial-ammo')
    assert status == CHECK_FAILED, output


def test_departed_refund_negative_control():
    files = tuple((name, mutated(text,
        'if (run->phase == _extraction_cancelling)\n\t\t{\n'
        '\t\t\tif (!gear_cancel_reservation(&run->gear))\n'
        '\t\t\t\tplatform_log("extraction: ungranted loadout refund failed before session close");\n'
        '\t\t}\n\t\telse\n\t\t\tgear_lose(&run->gear);',
        'gear_lose(&run->gear);')) if name == 'extraction.inc' else
        # gear_lose also protects unissued reservations. Remove both refund
        # paths to model loss; removing only the caller is now harmless.
        (name, mutated(text, 'if (session->record.phase == _gear_reserved) return gear_cancel_reservation(session);', ''))
        if name == 'under_test.inc' else (name, text)
        for name, text in generated())
    status, output = run(build('extraction', files), 'refund-disconnect')
    assert status == CHECK_FAILED, output


def test_native_deployment_signal_negative_control():
    files = tuple((name, mutated(text,
        'return !extraction_deploy(run, player, !update_client_loadout_modifier_held(unit->unit.player_index));',
        'extraction_deploy(run, player, !update_client_loadout_modifier_held(unit->unit.player_index)); return TRUE;'))
        if name == 'extraction.inc' else (name, text) for name, text in generated())
    status, output = run(build('extraction', files), 'native-controls')
    assert status == CHECK_FAILED, output


def test_shared_cache_native_admission_negative_control():
    files = tuple((name, mutated(text,
        'return game_modes_inventory_weapon_compatible(&item->equipment);',
        'return TRUE;')) if name == 'under_test.inc' else (name, text)
        for name, text in generated())
    status, output = run(build('extraction', files), 'shared-cache-reject')
    assert status == CHECK_FAILED, output


def test_failed_survival_negative_control():
    status, output = run(build('extraction', generated(outcome_fault=True)), 'survival-loss')
    assert status == CHECK_FAILED, output


@pytest.mark.parametrize('before,after,case', [
    ('extraction_runs[slot].phase != _extraction_returned', 'FALSE', 'evac-loop'),
    ('if (!extraction.admitted && !coop_enemies_firefight_endless())',
     'if (!extraction.admitted)', 'evac-binding'),
])
def test_survival_evacuation_negative_control(before, after, case):
    files = tuple((name, mutated(text, before, after)) if name == 'extraction.inc' else (name, text)
                  for name, text in generated())
    status, output = run(build('extraction', files), case)
    assert status == CHECK_FAILED, output


def test_medical_combat_negative_control():
    status, output = run(build('extraction', generated(medical_fault=True)), 'medical-interrupt')
    assert status == CHECK_FAILED, output
