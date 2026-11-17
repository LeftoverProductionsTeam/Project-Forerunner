"""Native encounter migration chooses a live target, then existing firing groups.

The real follow function runs against bounded world adapters, not a second AI.
Native navigation, multiplayer and gameplay acceptance remain separate.
"""
import pytest

from harness import CHECK_FAILED, build, constant, enum_with, function, mutated, read, run


def generated(fault=None):
    source = read('source/ai/encounters.c')
    follow = function(source, 'encounter_update_follow')
    if fault == 'last':
        follow = mutated(follow, 'best_target_distance_squared = target_distances_squared[target_index];',
                         '(void)0;')
    elif fault == 'dead':
        follow = mutated(follow, 'TEST_FLAG(unit->object.damage_flags, _object_dead_bit)', 'FALSE')
    elif fault == 'enemy':
        follow = mutated(follow, 'game_team_is_enemy(encounter->team_index, unit->object.owner_team_index)', 'FALSE')
    definitions = read('source/ai/ai_scenario_definitions.h')
    return (
        ('limits.inc', '#define MAXIMUM_FOLLOW_TARGET_UNITS %d\n#define NUMBER_OF_FIRING_POSITION_GROUP_INDICES %d\n' % (
            constant(source, 'MAXIMUM_FOLLOW_TARGET_UNITS'), constant(source, 'NUMBER_OF_FIRING_POSITION_GROUP_INDICES')) +
         enum_with(definitions, 'NUMBER_OF_SQUAD_FLAGS') + '\n' +
         enum_with(definitions, 'MAXIMUM_NUMBER_OF_FIRING_POSITION_GROUPS') + '\n' +
         enum_with(source, '_follow_target_ai')),
        ('under_test.inc', follow),
    )


@pytest.mark.parametrize('case', ['nearest', 'reordered', 'sixth', 'tie', 'death-respawn',
                                 'quit-stale', 'empty', 'explicit', 'hysteresis', 'outside-squad',
                                 'opposing-nearer', 'only-opposition', 'legacy', 'legacy-dead'])
def test_encounter_follow(case):
    status, output = run(build('encounter_follow', generated()), case)
    assert status == 0, output


@pytest.mark.parametrize('fault,case', [('last', 'nearest'), ('dead', 'death-respawn'), ('enemy', 'opposing-nearer')])
def test_encounter_follow_negative(fault, case):
    status, output = run(build('encounter_follow', generated(fault)), case)
    assert status == CHECK_FAILED, output
