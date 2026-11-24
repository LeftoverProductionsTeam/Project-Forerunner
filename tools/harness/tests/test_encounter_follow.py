"""Native encounter migration chooses a live target, then existing firing groups.

The real follow function runs against bounded world adapters, not a second AI.
Native navigation, multiplayer and gameplay acceptance remain separate.
"""
import pytest

from harness import CHECK_FAILED, build, constant, enum_with, function, mutated, read, run


def generated(fault=None):
    source = read('source/ai/encounters.c')
    candidates = function(source, 'encounter_operation_follow_player')
    follow = function(source, 'encounter_update_follow')
    if fault == 'last':
        candidates = mutated(candidates, 'best_distance_squared = distance_squared;', '(void)0;')
    elif fault == 'dead':
        candidates = mutated(candidates, 'TEST_FLAG(unit->object.damage_flags, _object_dead_bit)', 'FALSE')
    elif fault == 'enemy':
        candidates = mutated(candidates, 'game_team_is_enemy(encounter->team_index, unit->object.owner_team_index)', 'FALSE')
    definitions = read('source/ai/ai_scenario_definitions.h')
    return (
        ('limits.inc', '#define MAXIMUM_FOLLOW_TARGET_UNITS %d\n#define NUMBER_OF_FIRING_POSITION_GROUP_INDICES %d\n' % (
            constant(source, 'MAXIMUM_FOLLOW_TARGET_UNITS'), constant(source, 'NUMBER_OF_FIRING_POSITION_GROUP_INDICES')) +
         enum_with(definitions, 'NUMBER_OF_SQUAD_FLAGS') + '\n' +
         enum_with(definitions, 'MAXIMUM_NUMBER_OF_FIRING_POSITION_GROUPS') + '\n' +
         enum_with(source, '_follow_target_ai')),
        ('under_test.inc', candidates + '\n' + follow + '\n' +
         function(read('source/scenario/scenario.c'), 'scenario_uses_regional_gameplay') + '\n' +
         '\n'.join(function(source, name) for name in
                   ('encounter_activate', 'encounter_deactivate', 'encounter_force_deactivate',
                    'encounters_reconnect_to_structure_bsp'))),
    )


@pytest.mark.parametrize('case', ['nearest', 'reordered', 'sixth', 'tie', 'death-respawn',
                                 'quit-stale', 'empty', 'explicit', 'hysteresis', 'outside-squad',
                                 'opposing-nearer', 'only-opposition', 'ninth', 'incapacitated-nearer',
                                 'legacy', 'legacy-dead', 'legacy-ninth', 'regional-reconnect'])
def test_encounter_follow(case):
    status, output = run(build('encounter_follow', generated()), case)
    assert status == 0, output


@pytest.mark.parametrize('fault,case', [('last', 'nearest'), ('dead', 'death-respawn'), ('enemy', 'opposing-nearer')])
def test_encounter_follow_negative(fault, case):
    status, output = run(build('encounter_follow', generated(fault)), case)
    assert status == CHECK_FAILED, output
