"""Real BSP load/recovery and player reconnect code; cache IO and physics are fixture boundaries."""
import sys
from pathlib import Path

import pytest

sys.path.insert(0, str(Path(__file__).resolve().parents[2]))
from harness import CHECK_FAILED, build, constant, function, mutated, read, run, structure


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
    return (
        ('mcc_bsp.inc', structure(read('port/linux/game/mcc_cache_format.h'), 'mcc_cache_bsp') + '\n' +
         '\n'.join(function(mcc_cache, name) for name in
                   ('mcc_cache_bsp_reference', 'mcc_cache_bsp_reference_checked')) + '\n' +
         function(read('source/cache/cache_files.c'), 'scenario_structure_bsp_reference_is_valid')),
        ('objects.inc', objects),
        ('switch_type.inc', structure(players, 'scenario_bsp_switch_trigger_volume')),
        ('players.inc', '\n'.join(function(players, name) for name in (
            'player_teleport_with_policy', 'player_teleport', 'players_coop_teleport_stacked', 'is_player_in_trigger',
            'player_teleport_on_bsp_switch', 'players_cancel_bsp_switch_gather',
            'player_in_bsp_switch_volume', 'player_bsp_crossing_root')) + '\n' + reconnect + '\n' + rescue),
        ('loader.inc', function(read('source/scenario/scenario.c'), 'scenario_switch_structure_bsp')),
        ('bsp_receive.inc', '\n'.join(function(read('source/main/main.c'), name) for name in
            ('main_switch_structure_bsp', 'main_sync_structure_bsp', 'main_switch_to_structure_bsp_private')) +
            '\n' + '\n'.join(function(read('port/linux/game/network_distributed.c'), name) for name in
                            ('distributed_handle_structure_bsp', 'distributed_handle_unit_states'))),
    )


@pytest.mark.parametrize('case', [
    'host-superseded', 'host-snapshot-before-load', 'host-snapshot-after-rollback', 'reference-refused', 'mcc-reference-refused', 'load-refused',
    'verify-refused', 'split-refused', 'reverse-refused',
    'forward', 'reverse', 'split-forward', 'same-vehicle', 'restore-refused', 'retry-refused',
    'connected-vehicles', 'connected-reverse', 'connected-foot', 'connected-offset-seats',
    'connected-stranded', 'connected-stranded-host', 'connected-unrelated',
    'no-pending', 'negative-pending', 'past-end-pending', 'crossing-root-trigger',
    'activation-cluster', 'activation-object', 'crowded-stranded', 'crowded-solid',
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
