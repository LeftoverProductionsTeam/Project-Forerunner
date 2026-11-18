"""Native actor readback samples real world units; it does not validate gameplay."""
import sys
from pathlib import Path

import pytest

sys.path.insert(0, str(Path(__file__).resolve().parents[2]))
from harness import CHECK_FAILED, build, enum_with, function, mutated, read, run  # noqa: E402

ACTORS = 'port/linux/game/network_actors.c'
TEST = 'port/linux/game/network_test.c'
CASES = ['host-control', 'host-world', 'client-world', 'remember-death', 'deleted-reused',
         'players-duplicates', 'capacity-priority', 'local-reset', 'disabled',
         'cadence-rewind', 'cadence-delayed', 'interval-bounds', 'plane-waypoint-position', 'plane-waypoint-turn',
         'plane-landing-arrival', 'plane-parked-look', 'plane-parked-rest']
CONTROLS = {
    'client-ai-state': ('if (host && actor_data && unit->unit.actor_index != NONE)',
                        'if (actor_data && unit->unit.actor_index != NONE)', 'host-control'),
    'stale-ai-owner': ('if (actor && actor->meta.unit_index == units[index])',
                       'if (actor)', 'host-control'),
    'stale-position': ('unit->object.position.x,', '0.0,', 'host-world'),
    'parked-look-steers': ('*facing = heading;',
                           '/* keep the look override */', 'plane-parked-look'),
    'powered-seat-wakes-parked-plane': ('actor && actor->input.vehicle_index == vehicle_index && actor_move_plane_is_parked(actor)',
                                      'FALSE', 'plane-parked-rest'),
    'wrong-table': ('boolean host = game_connection() == _game_connection_network_server;',
                    'boolean host = TRUE;', 'client-world'),
    'lost-salt': (
        'now, role, (unsigned long)units[index], (unsigned long)unit->definition_index,',
        'now, role, (unsigned long)DATUM_INDEX_TO_ABSOLUTE_INDEX(units[index]), (unsigned long)unit->definition_index,',
        'host-world'),
    'no-corpse-readback': ('index < logged_actor_count;', 'index < 0;', 'remember-death'),
    'includes-player': ('unit->unit.player_index != NONE', 'FALSE', 'players-duplicates'),
    'reduced-capacity': ('*count >= MAXIMUM_NETWORK_ACTORS', '*count >= MAXIMUM_NETWORK_ACTORS - 1', 'capacity-priority'),
    'stale-map': ('logged_actor_count = 0;', '/* no reset */', 'local-reset'),
    'default-enabled': ('if (!(network_test.actors_interval > 0.0f))', 'if (FALSE)', 'disabled'),
    'no-cadence': ('now / interval == network_test.actors_logged_time / interval',
                   '(now / interval == network_test.actors_logged_time / interval && FALSE)', 'cadence-rewind'),
    'shifted-cadence': ('now / interval == network_test.actors_logged_time / interval',
                        'now - network_test.actors_logged_time < interval', 'cadence-delayed'),
}


def generated(control=None):
    actors, test = read(ACTORS), read(TEST)
    code = '\n'.join(function(actors, name) for name in
                     ('network_actors_new_game', 'actor_log_remember', 'network_actors_log_units'))
    code += '\n' + function(test, 'network_test_log_actors')
    moving = read('source/ai/actor_moving.c')
    code += '\n' + enum_with(read('source/ai/actors.h'), '_actor_vehicle_driver_directional_flying')
    code += '\n' + enum_with(read('source/ai/actors.h'), '_actor_action_obey')
    code += '\n' + '\n'.join(function(moving, name) for name in
                                  ('actor_move_plane_definition', 'actor_move_sample_plane_position',
                                   'actor_move_plane_approach_scale', 'actor_path_at_destination',
                                   'actor_move_plane_is_parked', 'actor_move_plane_parked_facing'))
    code += '\n' + function(read('source/units/vehicles.c'), 'vehicle_controls_wake_physics')
    if control:
        before, after, _ = CONTROLS[control]
        code = mutated(code, before, after)
    return (('under_test.inc', code),)


@pytest.mark.parametrize('case', CASES)
def test_case(case):
    status, output = run(build('network_actor_readback', generated()), case)
    assert status == 0, output


@pytest.mark.parametrize('control', CONTROLS)
def test_negative_control(control):
    status, output = run(build('network_actor_readback', generated(control)), CONTROLS[control][2])
    assert status == CHECK_FAILED, f'{control} was not caught: {output}'


def test_configuration_and_update_hook():
    source = read('port/linux/src/port_config.c')
    assert '"debug.network_test_actors", _config_real, "0.0", "HALO_NETWORK_TEST_ACTORS"' in source
    update = function(read(TEST), 'network_test_update')
    assert 'if (game_in_progress() && !main_menu_loaded)\n\t\tnetwork_test_log_actors();' in update
