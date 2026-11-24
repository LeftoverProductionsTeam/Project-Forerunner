"""Chaining native aircraft recordings before the previous cleanup tick."""
import pytest
from harness import CHECK_FAILED, build, enum_with, function, mutated, read, run, structure


def generated(fault=None):
    source = read('source/cutscene/recorded_animations.c')
    types = enum_with(source, '_recording_thread_finished_bit')
    types += structure(read('source/cutscene/recorded_animation_definitions.h'), 'recorded_animation_definition')
    for name in ('animation_thread', 'animation_thread_debug', 'animation_playback'):
        types += structure(source, name)
    code = '\n'.join(function(source, name) for name in (
        'get_controlling_thread', 'recorded_animation_controlling_unit', 'recorded_animation_has_thread',
        'recorded_animation_kill', 'recorded_animation_play_internal',
        'recorded_animations_update'))
    if fault:
        code = mutated(code, *fault)
    return (('recording_types.inc', types), ('recording_lifecycle.inc', code))


@pytest.mark.parametrize('case', ['killed', 'finished', 'hover', 'hover-handoff', 'uncontrolled'])
def test_native_flight_handoff(case):
    status, output = run(build('recorded_flight', generated()), case)
    assert status == 0, output


@pytest.mark.parametrize('fault,case', [
    (('thread->flags = extra_flags;', 'thread->flags |= extra_flags;'), 'killed'),
    (('was_controlled);', 'was_controlled && unit_controllable(unit_index));'), 'finished'),
])
def test_native_flight_handoff_negative_control(fault, case):
    status, output = run(build('recorded_flight', generated(fault)), case)
    assert status == CHECK_FAILED, output
