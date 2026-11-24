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
    bsp_limits = [('MAXIMUM_CLUSTERS_PER_STRUCTURE', 'source/structures/structures.h'),
                  ('MAXIMUM_SURFACES_PER_STRUCTURE', 'source/render/render.h')]
    bsp_verifier = function(read('source/structures/structure_bsp_definitions.c'), 'structure_bsp_port_verify')
    return (
        ('mcc_bsp.inc', structure(read('port/linux/game/mcc_cache_format.h'), 'mcc_cache_bsp') + '\n' +
         '\n'.join(function(mcc_cache, name) for name in
                   ('mcc_runtime_linked_bsp_limit', 'mcc_cache_bsp_reference', 'mcc_cache_bsp_header_fits', 'mcc_cache_bsp_reference_checked',
                    'mcc_cache_bsp_resident_address', 'mcc_cache_bsp_prepare_address', 'mcc_cache_bsp_release', 'mcc_cache_drop_retained_bsp',
                    'mcc_cache_bsp_load', 'mcc_cache_bsp_retain', 'mcc_cache_bsp_unload')) + '\n' +
         enum_with(cache_header, '_scenario_bsp_prepare_deferred') + '\n' +
         structure(cache_header, 'scenario_bsp_prepared') + '\n' +
         '\n'.join(function(cache, name) for name in ('scenario_structure_bsp_reference_is_valid',
             'scenario_structure_bsp_prepare', 'scenario_structure_bsp_activate_prepared',
             'cache_files_unbind_mcc_bsp', 'scenario_structure_bsp_retain'))),
        ('bsp_limits.inc', '\n'.join(f'#define {name} {constant(read(path), name)}' for name, path in bsp_limits) +
         '\n' + bsp_verifier.replace('structure_bsp_port_verify', 'native_bsp_limits', 1)),
        ('objects.inc', objects),
        ('item_structure_support.inc', '\n'.join(function(read('source/items/items.c'), name)
            for name in ('item_rested_surface_index', 'item_rested_surface', 'item_recheck_structure_support'))),
        ('bsp_activation.inc', function(read('source/objects/objects.c'), 'objects_update_cluster_activation')),
        ('switch_type.inc', structure(players, 'scenario_bsp_switch_trigger_volume')),
        ('players.inc', '\n'.join(function(players, name) for name in (
            'player_teleport_with_policy', 'player_teleport', 'players_coop_teleport_stacked', 'is_player_in_trigger',
            'player_teleport_on_bsp_switch', 'players_cancel_bsp_switch_gather',
            'player_in_bsp_switch_volume', 'players_coop_bsp_switch_trigger',
            'player_bsp_continuous_route_contains', 'player_bsp_crossing_root',
            'players_coop_pending_continuous_bsp_switch', 'players_coop_can_activate_structure_bsp',
            'players_coop_vehicle_passenger_crossing_bsp')) + '\n' + reconnect + '\n' + rescue),
        ('region_location.inc', '\n'.join(function(read('source/scenario/scenario.c'), name) for name in
            ('scenario_location_in_bsp_tree', 'scenario_structure_bsp_contains_point'))),
        ('loader.inc', function(read('source/scenario/scenario.c'), 'scenario_switch_structure_bsp')),
        ('scripted_bsp.inc', function(read('source/hs/hs.c'), 'scenario_switch_structure_bsp_evaluate')),
        ('queue_bsp_switch.inc', function(players, 'player_queue_structure_bsp_switch')),
        ('majority_bsp_vote.inc', '\n'.join(function(players, name) for name in
            ('players_coop_bsp_switch_majority', 'players_coop_finish_bsp_switch'))),
        ('bsp_receive.inc', '\n'.join(function(read('source/main/main.c'), name) for name in
            ('main_switch_structure_bsp', 'main_sync_structure_bsp', 'main_switch_to_structure_bsp_private')) +
            '\n' + '\n'.join(function(read('port/linux/game/network_distributed.c'), name) for name in
                            ('distributed_bsp_world_ready', 'distributed_can_apply_bsp_snapshot',
                             'distributed_handle_actor_states', 'distributed_handle_bsp_world_snapshot',
                             'distributed_handle_structure_bsp', 'distributed_handle_unit_states'))),
        ('bsp_actor_controls.inc', function(read('port/linux/game/network_actors.c'), 'network_actors_client_bsp_changed')),
        ('bsp_world_state.inc', '\n'.join(function(read('port/linux/game/network_objects.c'), name) for name in
            ('objects_client_reset_own_vehicle_history', 'network_objects_client_bsp_changed')) + '\n' +
         function(read('port/linux/game/network_coop.c'), 'network_coop_note_player_structure_bsp')),
    )


@pytest.mark.parametrize('case', [
    'host-superseded', 'host-refused-neighbor', 'host-snapshot-before-load', 'host-snapshot-after-rollback', 'host-actor-snapshot-boundary', 'host-object-snapshot-boundary', 'reference-refused', 'mcc-reference-refused', 'load-refused',
    'verify-refused', 'split-refused', 'reverse-refused',
    'mcc-neighbor-geometry', 'mcc-neighbor-visibility', 'mcc-neighbor-truncated', 'mcc-neighbor-range',
    'mcc-prepare-success', 'mcc-prepare-relocated', 'mcc-prepare-crossing', 'mcc-prepare-evict', 'mcc-prepare-refused', 'mcc-prepare-limits', 'mcc-prepare-overlap',
    'scripted-selection', 'overlapping-coop-routes', 'majority-overlapping-routes',
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
