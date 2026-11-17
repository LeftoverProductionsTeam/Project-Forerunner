"""Faber-derived pursuit must not override native allegiance or body ownership."""
import pytest

from harness import CHECK_FAILED, build, constant, enum_with, function, mutated, read, run


def generated(fault=None):
    types = '\n'.join((
        enum_with(read('source/objects/objects.h'), '_object_dead_bit'),
        enum_with(read('source/objects/object_types.h'), '_object_mask_biped'),
        enum_with(read('source/ai/props.h'), '_prop_state_uninspected_orphan'),
        enum_with(read('source/ai/actors.h'), '_actor_target_acknowledged_enemy'),
        enum_with(read('source/units/units.h'), '_unit_spawned_actors_bit'),
        enum_with(read('source/units/unit_definitions.h'), '_unit_is_destroyed_after_dying_bit'),
        enum_with(read('source/game/game.h'), 'NUMBER_OF_SOLO_CAMPAIGN_TEAMS'),
    ))
    source = read('port/linux/game/coop_enemies.c')
    code = function(source, 'firefight_pursue')
    children = '\n'.join(function(source, name) for name in (
        'firefight_spawn_pending', 'coop_enemies_firefight_tracks_unit',
        'coop_enemies_firefight_track_spawned_unit', 'coop_enemies_firefight_enemy_damaged',
        'coop_enemies_firefight_enemy_died', 'coop_enemies_firefight_spawn_finished',
        'firefight_format_status'))
    if fault:
        if fault[0] in code:
            code = mutated(code, *fault)
        else:
            children = mutated(children, *fault)
    return (('types.inc', types + '\n#define FIREFIGHT_MAX_TRACKED_ENEMIES ' +
             str(constant(source, 'FIREFIGHT_MAX_TRACKED_ENEMIES'))),
            ('under_test.inc', code), ('children.inc', children))


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


@pytest.mark.parametrize('case', ['reclaim', 'pending', 'parent', 'full', 'invalid', 'append', 'repeated', 'partial', 'collected'])
def test_native_children(case):
    status, output = run(build('firefight_pursuit', generated()), 'child-' + case)
    assert status == 0, output


@pytest.mark.parametrize('before,after,case', [
    ('firefight.units[index] != parent_index && firefight.killed[index] &&',
     'firefight.killed[index] &&', 'parent'),
    ('!firefight_spawn_pending((struct unit_datum *)object_try_and_get_and_verify_type(\n'
     '\t\t\t\t\tfirefight.units[index], _object_mask_biped))', 'TRUE', 'pending'),
])
def test_native_child_reuse_negative(before, after, case):
    status, output = run(build('firefight_pursuit', generated((before, after))), 'child-' + case)
    assert status == CHECK_FAILED, output
