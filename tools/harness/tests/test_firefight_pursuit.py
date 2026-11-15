"""Faber-derived pursuit must not override native allegiance or body ownership."""
import pytest

from harness import CHECK_FAILED, build, enum_with, function, mutated, read, run


def generated(fault=None):
    types = '\n'.join((
        enum_with(read('source/objects/objects.h'), '_object_dead_bit'),
        enum_with(read('source/objects/object_types.h'), '_object_mask_biped'),
        enum_with(read('source/ai/props.h'), '_prop_state_uninspected_orphan'),
        enum_with(read('source/ai/actors.h'), '_actor_target_acknowledged_enemy'),
    ))
    code = function(read('port/linux/game/coop_enemies.c'), 'firefight_pursue')
    if fault:
        code = mutated(code, *fault)
    return (('types.inc', types), ('under_test.inc', code))


@pytest.mark.parametrize('case', ['hostile', 'ally-nearest', 'allied', 'quit',
                                 'stale', 'dead', 'missing', 'switch', 'swarm', 'inactive'])
def test_pursuit(case):
    status, output = run(build('firefight_pursuit', generated()), case)
    assert status == 0, output


def test_allegiance_negative_control():
    status, output = run(build('firefight_pursuit', generated((
        '!game_team_is_enemy(actor->meta.team_index, candidate->object.owner_team_index)',
        'FALSE'))), 'switch')
    assert status == CHECK_FAILED, output


def test_ownership_negative_control():
    status, output = run(build('firefight_pursuit', generated((
        'candidate->unit.player_index != iterator.datum_index', 'FALSE'))), 'stale')
    assert status == CHECK_FAILED, output
