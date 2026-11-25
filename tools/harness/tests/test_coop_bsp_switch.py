"""Real BSP load/recovery and player reconnect code; cache IO and physics are fixture boundaries."""
import sys
from pathlib import Path

import pytest

sys.path.insert(0, str(Path(__file__).resolve().parents[2]))
from harness import CHECK_FAILED, build, constant, enum_with, function, mutated, read, run, structure


def generated(fault=None):
    players = read('source/game/players.c')
    reconnect = function(players, 'players_reconnect_to_structure_bsp')
    tick_rate = constant(read('source/cseries/cseries.h'), 'TICKS_PER_SECOND')
    rescue_timeout = next(line for line in players.splitlines() if line.startswith('#define COOP_STRANDED_TICKS '))
    rescue = f'#define TICKS_PER_SECOND {tick_rate}\n{rescue_timeout}\n' + function(players, 'players_coop_rescue_stranded')
    if fault == 'destination':
        reconnect = mutated(reconnect,
                            'bsp_switch->destination_structure_bsp_index != global_structure_bsp_index',
                            'FALSE')
    objects = function(read('source/objects/objects.c'), 'object_pvs_clear') + '\n' + function(
        read('source/objects/objects.c'), 'objects_disconnect_from_structure_bsp')
    if fault == 'activation':
        objects = mutated(objects, '\t\tobject_pvs_clear();', '\t\t;')
    mcc_cache = read('port/linux/game/mcc_cache.c')
    cache = read('source/cache/cache_files.c')
    cache_header = read('source/cache/cache_files.h')
    bsp_limits = [('MCC_SCENARIO_NETGAME_EQUIPMENT_MAXIMUM', 'port/linux/game/mcc_cache_format.h'),
                  ('MAXIMUM_CLUSTERS_PER_STRUCTURE', 'source/structures/structures.h'),
                  ('MAXIMUM_SURFACES_PER_STRUCTURE', 'source/render/render.h')]
    bsp_verifier = function(read('source/structures/structure_bsp_definitions.c'), 'structure_bsp_port_verify')
    breakables = read('source/physics/breakable_surfaces.c')
    breakables_header = read('source/physics/breakable_surfaces.h')
    coop = read('port/linux/game/network_coop.c')
    network = read('port/linux/game/network_distributed.c')
    damage = read('port/linux/game/network_damage.c')
    return (
        ('mcc_bsp.inc', structure(read('port/linux/game/mcc_cache_format.h'), 'mcc_cache_bsp') + '\n' +
         '\n'.join(function(mcc_cache, name) for name in
                   ('mcc_runtime_linked_bsp_limit', 'mcc_cache_bsp_reference', 'mcc_cache_bsp_header_fits', 'mcc_cache_bsp_reference_checked',
                    'mcc_cache_bsp_resident_address', 'mcc_cache_bsp_in_use', 'mcc_cache_bsp_set_in_use',
                    'mcc_cache_has_bsp_in_use_elsewhere', 'mcc_cache_bsp_keep_two_neighbors', 'mcc_cache_bsp_retained_slot',
                    'mcc_cache_bsp_overlap_end', 'mcc_cache_bsp_address_with_neighbors', 'mcc_cache_bsp_prepare_address',
                    'mcc_cache_bsp_release', 'mcc_cache_remove_retained_slot',
                    'mcc_cache_bsp_retained_structure', 'mcc_cache_bsp_load', 'mcc_cache_bsp_retain',
                    'mcc_cache_bsp_discard_prepared', 'mcc_cache_bsp_unload')) + '\n' +
         enum_with(cache_header, '_scenario_bsp_prepare_deferred') + '\n' +
         structure(cache_header, 'scenario_bsp_prepared') + '\n' +
         function(read('source/scenario/scenario.c'), 'scenario_uses_regional_gameplay') + '\n' +
         '\n'.join(function(cache, name) for name in ('scenario_structure_bsp_reference_is_valid',
             'scenario_structure_bsp_prepare', 'scenario_structure_bsp_activate_prepared',
             'cache_files_unbind_mcc_bsp', 'scenario_structure_bsp_retain'))),
        ('bsp_limits.inc', '\n'.join(f'#define {name} {constant(read(path), name)}' for name, path in bsp_limits) +
         '\n' + bsp_verifier.replace('structure_bsp_port_verify', 'native_bsp_limits', 1)),
        ('surface_breaks.inc',
         enum_with(breakables_header, 'MAXIMUM_BREAKABLE_SURFACES_PER_MAP') + '\n' +
         structure(breakables_header, 'structure_breakable_surface') + '\n' +
         structure(breakables_header, 'breakable_surface_datum') + '\n' +
         structure(breakables, 'breakable_surface_globals') + '\n' +
         structure(coop, 'distributed_coop_event') + '\n' +
         'static struct breakable_surface_globals surface_state, *globals = &surface_state;\n' +
         'static struct distributed_coop_event sent_surfaces[16];\n' +
         'static struct distributed_coop_event *event_new(byte kind)\n'
         '{ CHECK(kind == _coop_event_surface_broken && surface_events < 16, "invalid fracture event");'
         ' return &sent_surfaces[surface_events++]; }\n' +
         function(read('source/scenario/scenario.c'), 'scenario_structure_bsp_try_and_get') + '\n' +
         '\n'.join(function(breakables, name) for name in (
             'breakable_surface_indices_valid', 'breakable_surface_extant_in_bsp',
             'breakable_surface_port_break')) + '\n' +
         '\n'.join(function(coop, name) for name in (
             'network_coop_note_surface_broken', 'host_send_broken_surfaces'))),
        ('objects.inc', objects),
        ('item_structure_support.inc', '\n'.join(function(read('source/items/items.c'), name)
            for name in ('item_rested_surface_index', 'item_rested_surface', 'item_recheck_structure_support'))),
        ('bsp_activation.inc', function(read('source/objects/objects.c'), 'objects_update_cluster_activation')),
        ('switch_type.inc', structure(players, 'scenario_bsp_switch_trigger_volume')),
        ('regional_start.inc', function(players, 'player_starting_location_in_active_bsp')),
        ('regional_equipment.inc', enum_with(read('source/game/game_engine.c'), 'REGIONAL_STATIC_DEVICE_TYPES') +
         '\n' + '\n'.join(function(read('source/game/game_engine.c'), name) for name in
            ('game_engine_equipment_item_at_pad', 'game_engine_equipment_item_parked', 'game_engine_vehicle_empty',
             'game_engine_record_regional_object', 'game_engine_object_parked', 'game_engine_update_item_spawn'))),
        ('device_touch.inc', function(read('source/devices/devices.c'), 'device_touched')),
        ('netgame_flags.inc', enum_with(read('source/scenario/scenario_definitions.h'),
             '_netgame_flag_teleporter_source') + '\n' +
         '\n'.join(function(read('source/game/game_engine.c'), name)
            for name in ('find_netgame_flags_in_structure', 'find_netgame_flags', 'game_engine_find_teleporter'))),
        ('players.inc', structure(read('source/game/players.h'), 'players_bsp_crossing_clearance') + '\n' +
         '\n'.join(function(players, name) for name in (
            'player_teleport_with_policy', 'player_teleport', 'players_coop_teleport_stacked', 'is_player_in_trigger',
            'player_teleport_on_bsp_switch', 'players_cancel_bsp_switch_gather',
            'player_in_bsp_switch_volume', 'players_coop_bsp_switch_trigger',
            'player_bsp_continuous_route_contains', 'player_bsp_crossing_root',
            'players_coop_pending_continuous_bsp_switch', 'player_root_bsp_contacts',
            'players_capture_bsp_crossing_clearance', 'players_continuous_bsp_crossing_valid',
            'players_coop_can_activate_structure_bsp',
            'players_coop_vehicle_passenger_crossing_bsp')) + '\n' + reconnect + '\n' + rescue),
        ('region_location.inc', '\n'.join(function(read('source/scenario/scenario.c'), name) for name in
            ('scenario_location_in_bsp_tree', 'scenario_structure_bsp_contains_point',
             'scenario_structure_bsp_contains_placement'))),
        ('loader.inc', function(read('source/scenario/scenario.c'), 'scenario_switch_structure_bsp')),
        ('scripted_bsp.inc', function(read('source/hs/hs.c'), 'scenario_switch_structure_bsp_evaluate')),
        ('queue_bsp_switch.inc', function(players, 'player_queue_structure_bsp_switch')),
        ('majority_bsp_vote.inc', '\n'.join(function(players, name) for name in
            ('players_coop_bsp_switch_majority', 'players_coop_finish_bsp_switch'))),
        ('bsp_receive.inc', '\n'.join(function(read('source/main/main.c'), name) for name in
            ('main_switch_structure_bsp', 'main_sync_structure_bsp', 'main_switch_to_structure_bsp_private')) +
            '\n' + structure(network, 'distributed_structure_bsp_message') +
            '\n' + '\n'.join(function(network, name) for name in
                            ('distributed_bsp_world_ready', 'distributed_can_apply_bsp_snapshot',
                             'distributed_handle_actor_states', 'distributed_handle_bsp_world_snapshot',
                             'distributed_handle_structure_bsp', 'distributed_handle_unit_states',
                             'distributed_combat_sample_matches_bsp', 'distributed_hit_report_matches_structure_bsp',
                             'distributed_send_structure_bsp'))),
        ('bsp_damage_history.inc', '\n'.join(structure(damage, name) for name in (
            'damage_sight', 'damage_history_tick', 'damage_trail_point')) + '\n' +
         'static struct damage_history_tick damage_history[TARGET_HISTORY_TICKS];\n' +
         'static struct damage_trail_point damage_trails[MAXIMUM_TRACKED_PLAYERS][PLAYER_TRAIL_POINTS];\n' +
         function(damage, 'distributed_sight_at')),
        ('bsp_actor_controls.inc', function(read('port/linux/game/network_actors.c'), 'network_actors_client_bsp_changed')),
        ('bsp_world_state.inc', '\n'.join(function(read('port/linux/game/network_objects.c'), name) for name in
            ('objects_client_reset_own_vehicle_history', 'network_objects_client_bsp_changed')) + '\n' +
         function(read('port/linux/game/network_coop.c'), 'network_coop_note_player_structure_bsp')),
    )


@pytest.mark.parametrize('case', [
    'host-superseded', 'regional-host-superseded', 'regional-late-join', 'host-refused-neighbor', 'host-snapshot-before-load', 'host-snapshot-after-rollback', 'host-actor-snapshot-boundary', 'host-object-snapshot-boundary', 'regional-hit-boundary', 'reference-refused', 'mcc-reference-refused', 'load-refused',
    'verify-refused', 'split-refused', 'reverse-refused',
    'mcc-neighbor-geometry', 'mcc-neighbor-visibility', 'mcc-neighbor-truncated', 'mcc-neighbor-range',
    'mcc-prepare-space-above', 'mcc-prepare-space-below', 'mcc-prepare-cold-crossing', 'mcc-prepare-cold-regional-clearance', 'mcc-prepare-cold-vehicle-clearance',
    'mcc-prepare-success', 'mcc-prepare-relocated', 'mcc-prepare-fractures', 'mcc-prepare-crossing',
    'mcc-prepare-regional-crossing', 'mcc-prepare-three-regions', 'mcc-prepare-fourth-region', 'mcc-prepare-occupied-regions', 'mcc-prepare-evict', 'mcc-prepare-refused', 'mcc-prepare-limits', 'mcc-prepare-overlap',
    'scripted-selection', 'regional-teleporters', 'regional-starts', 'regional-equipment', 'regional-loose-objects', 'regional-devices', 'overlapping-coop-routes', 'majority-overlapping-routes',
    'forward', 'reverse', 'split-forward', 'same-vehicle', 'restore-refused', 'retry-refused',
    'connected-vehicles', 'connected-reverse', 'connected-foot', 'connected-offset-seats',
    'connected-stranded', 'connected-stranded-host', 'connected-unrelated', 'connected-ai-passenger',
    'connected-parallel-vehicles', 'connected-parallel-foot', 'connected-parallel-marked',
    'connected-parallel-other-destination', 'connected-parallel-other-source',
    'no-pending', 'negative-pending', 'past-end-pending', 'crossing-root-trigger',
    'activation-cluster', 'activation-object', 'activation-destination', 'item-rested-surface-bsp',
    'crowded-stranded', 'crowded-solid',
    'crowded-inside', 'crowded-operation', 'crowded-enemy', 'rescue-crowded', 'rescue-solid',
])
def test_bsp_reconnect(case):
    status, output = run(build('coop_bsp_switch', generated()), case)
    assert status == 0, output


def test_destination_negative_control():
    status, output = run(build('coop_bsp_switch', generated('destination')), 'load-refused')
    assert status == CHECK_FAILED, output


def test_stale_activation_negative_control():
    status, output = run(build('coop_bsp_switch', generated('activation')), 'activation-cluster')
    assert status == CHECK_FAILED, output
