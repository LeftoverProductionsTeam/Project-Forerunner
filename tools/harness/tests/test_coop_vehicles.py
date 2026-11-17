"""Production vehicle budgets, lifecycle, host authority and checkpoint restore."""
import re
import sys
from pathlib import Path
import pytest

sys.path.insert(0, str(Path(__file__).resolve().parents[2]))
from harness import build, read, run, mutated, CHECK_FAILED, function, enum_with, structure

def generated(fault=None):
    source = re.sub(r'^#include.*\n', '', read('port/linux/game/coop_vehicles.c'), flags=re.M)
    if fault:
        source = mutated(source, *fault)
    riders = read('port/linux/game/coop_enemies.c')
    release = '\n'.join(function(riders, name) for name in (
        'coop_enemies_forget_vehicle', 'coop_enemies_riders_getting_out', 'coop_enemies_update'))
    return (('under_test.inc', source),
            ('rider_types.inc', enum_with(riders, 'MAXIMUM_KEPT_RIDERS =') + '\n' +
             structure(riders, 'coop_riding_vehicle') + '\n' + structure(riders, 'coop_kept_rider') + '\n' +
             structure(read('source/ai/ai_scenario_definitions.h'), 'actor_starting_location')),
            ('rider_release.inc', release))


@pytest.mark.parametrize('case', ['retry', 'seated', 'destroyed', 'authority', 'recycled'])
def test_dropship_rider_release(case):
    status, output = run(build('coop_vehicles', generated()), 'riders-' + case)
    assert status == 0, output


@pytest.mark.parametrize('before,after', [
    ('coop_enemies.riders[index] = coop_enemies.riders[--coop_enemies.rider_count];', '(void)coop_enemies.rider_count;'),
    ('if (now < vehicle->next_release_time)', 'if (FALSE)'),
])
def test_dropship_rider_release_negative(before, after):
    files = tuple((name, mutated(text, before, after)) if name == 'rider_release.inc' else (name, text)
                  for name, text in generated())
    status, output = run(build('coop_vehicles', files), 'riders-retry')
    assert status == CHECK_FAILED, output

@pytest.mark.parametrize('case', ['counts', 'authority', 'driving', 'delivery', 'chain', 'blocked', 'checkpoint', 'recreated'])
def test_case(case):
    status, output = run(build('coop_vehicles', generated()), case)
    assert status == 0, output

@pytest.mark.parametrize('fault,case', [
    (('#define VEHICLE_SCALING_NUMERATOR 2', '#define VEHICLE_SCALING_NUMERATOR 3'), 'counts'),
    (('game_connection() == _game_connection_network_server', 'TRUE'), 'authority'),
    (('#define VEHICLE_CLEARANCE 1.15f', '#define VEHICLE_CLEARANCE 1.0f'), 'driving'),
    (('(home->radius + vehicle->object.bounding_sphere_radius)', '(home->radius)'), 'driving'),
    (('home->copies >= budget', '((void)budget, FALSE)'), 'chain'),
    (('!home->entered ||', 'FALSE ||'), 'driving'),
    (('vehicle->object.parent_object_index != NONE', 'FALSE'), 'delivery'),
    (('coop_vehicle_homes = saved_vehicles;', '(void)saved_vehicles;'), 'checkpoint'),
    (('|| !vehicles_home_clear(home)', '|| FALSE'), 'blocked'),
    (('!collision_model_test_sphere(&instance, &center, radius)', 'FALSE'), 'blocked'),
])
def test_negative_control(fault, case):
    status, output = run(build('coop_vehicles', generated(fault)), case)
    assert status == CHECK_FAILED, output

def test_menu():
    import port_settings
    rows = port_settings.COOP_SETUP_SCREENS[0][3]
    names = [row[0] for row in rows]
    assert names[names.index('coop_extra_pickups') + 1] == 'coop_extra_vehicles'
    assert names[names.index('coop_extra_vehicles') + 1] == 'coop_enemies_multiplier'
    row = rows[names.index('coop_extra_vehicles')]
    assert row[2] == ['PER PLAYER', 'STATIC MULTIPLIER', 'NONE']
    assert row[3][0].replace('\\n', ' ') == (
        "Vehicles respawn after being driven a short distance based on the number of players: "
        "by two thirds the value of 'Per Player' for each player past the first.")
    assert '"network.coop_vehicles", _config_string' in read('port/linux/src/port_config.c')
    assert 'coop_vehicles_register(result, scenario_object);' in read('source/objects/objects.c')
