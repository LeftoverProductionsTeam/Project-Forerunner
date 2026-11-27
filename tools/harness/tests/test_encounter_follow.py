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
    ai = read('source/ai/ai.c')
    effects = read('source/effects/effects.c')
    effect_part = function(effects, 'effect_generate_part')
    damage_part = effect_part[effect_part.index('case DAMAGE_EFFECT_DEFINITION_TAG:'):
                              effect_part.index('case LIGHT_DEFINITION_TAG:')]
    return (
        ('limits.inc', '#define MAXIMUM_FOLLOW_TARGET_UNITS %d\n#define NUMBER_OF_FIRING_POSITION_GROUP_INDICES %d\n' % (
            constant(source, 'MAXIMUM_FOLLOW_TARGET_UNITS'), constant(source, 'NUMBER_OF_FIRING_POSITION_GROUP_INDICES')) +
         enum_with(definitions, 'NUMBER_OF_SQUAD_FLAGS') + '\n' +
         enum_with(definitions, 'MAXIMUM_NUMBER_OF_FIRING_POSITION_GROUPS') + '\n' +
         enum_with(source, '_follow_target_ai')),
        ('regional_combat_effects.inc', '\n'.join(function(effects, name) for name in (
             'effect_fixed_bsp', 'effect_simulation_bsp', 'effect_remote_simulation_structure',
             'effect_part_is_regional_gameplay', 'effect_allowed_by_environment')) +
         '\nstatic void dispatch_damage_effect(struct effect_datum const *effect,\n'
         ' struct effect_part_definition const *part_definition, real_point3d const *world_point,\n'
         ' real_vector3d const *world_forward, real scale, short bsp,\n'
         ' struct structure_bsp const *remote_structure)\n'
         '{\n switch(part_definition->runtime_base_class_tag) {\n' + damage_part + '\n }\n}\n'),
        ('under_test.inc', candidates + '\n' + follow + '\n' +
         function(read('source/scenario/scenario.c'), 'scenario_uses_regional_gameplay') + '\n' +
         function(read('source/ai/ai.c'), 'ai_unit_belongs_to_other_bsp') + '\n' +
         function(ai, 'ai_try_reconnect_disconnected_actor') + '\n' +
         '#define MAXIMUM_POTENTIALLY_RELEASABLE_ENTITIES %d\n' %
         constant(ai, 'MAXIMUM_POTENTIALLY_RELEASABLE_ENTITIES') +
         ai[ai.index('struct potentially_releasable_entity\n{'):ai.index('struct actor_iterator\n{')] +
         '\n'.join(function(ai, name) for name in (
             'ai_encounter_has_regional_home', 'ai_actor_has_regional_home', 'ai_release_inactive_swarms',
             'compare_potentially_releasable_entities', 'ai_find_inactive_encounters')) + '\n' +
         '\n'.join(function(read('source/ai/actors.c'), name) for name in
                   ('actor_simulation_region_live', 'actor_simulation_scene_live', 'actors_update')) + '\n' +
         source[source.index('struct encounter_activation_visibility\n{'):source.index('static unsigned long const *encounter_activation_pvs(')] +
         '\n'.join(function(source, name) for name in
                   ('encounter_activation_pvs', 'encounterless_unit_is_visible', 'encounter_activate', 'encounter_deactivate', 'encounter_force_deactivate',
                    'encounters_reconnect_to_structure_bsp'))),
    )


@pytest.mark.parametrize('case', ['nearest', 'reordered', 'sixth', 'tie', 'death-respawn',
                                 'quit-stale', 'empty', 'explicit', 'hysteresis', 'outside-squad',
                                 'opposing-nearer', 'only-opposition', 'ninth', 'incapacitated-nearer',
                                 'legacy', 'legacy-dead', 'legacy-ninth', 'regional-reconnect', 'regional-actor-ownership', 'regional-script-reattach', 'regional-encounterless-pvs', 'regional-actor-tick', 'regional-pressure', 'regional-effect-damage'])
def test_encounter_follow(case):
    status, output = run(build('encounter_follow', generated()), case)
    assert status == 0, output


@pytest.mark.parametrize('fault,case', [('last', 'nearest'), ('dead', 'death-respawn'), ('enemy', 'opposing-nearer')])
def test_encounter_follow_negative(fault, case):
    status, output = run(build('encounter_follow', generated(fault)), case)
    assert status == CHECK_FAILED, output
