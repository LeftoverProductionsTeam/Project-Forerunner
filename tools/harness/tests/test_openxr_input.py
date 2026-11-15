"""The shared Touch/Index bridge into native Halo actions; no hardware claims."""
import re
import sys
from pathlib import Path

import pytest

sys.path.insert(0, str(Path(__file__).resolve().parents[2]))
from harness import CHECK_FAILED, build, enum_with, function, macro, mutated, read, run, structure


def generated(fault=None):
    header = read('port/linux/src/vr_openxr.h')
    pad = read('port/include/xdk/xdk_pdb.h')
    buttons = read('port/include/xdk/xdk_xbox.h')
    source = read('port/linux/src/xinput_sdl.c')
    types = '\n'.join([
        enum_with(header, '_vr_button_primary ='),
        *(structure(header, name) for name in ['vr_openxr_pose', 'vr_openxr_hand', 'vr_openxr_frame']),
        structure(pad, '_XINPUT_GAMEPAD'), 'typedef struct _XINPUT_GAMEPAD XINPUT_GAMEPAD;',
        *(macro(buttons, name) for name in re.findall(r'^#define (XINPUT_GAMEPAD_\w+) ', buttons, re.M)),
    ])
    code = '\n'.join(function(source, name) for name in
        ['merge_button', 'vr_openxr_weapon_hand', 'vr_openxr_turn_hand', 'openxr_stick', 'openxr_trigger', 'openxr_gamepad_read'])
    if fault:
        code = mutated(code, *fault)
    types += '\n' + enum_with(pad, 'D3DCULL_NONE =')
    render = read('port/linux/game/vr_render.c')
    geometry = '\n'.join(function(render, name) for name in
        ['wrapped_yaw', 'quaternion_rotate', 'world_axes', 'weapon_offset', 'prepare_weapon_camera',
         'vr_render_weapon_nodes', 'vr_render_weapon_draw_begin', 'vr_render_weapon_draw_end',
         'controller_turn_delta', 'vr_render_player_aim'])
    cull = '\n'.join(function(read('port/linux/src/d3d8_gl.c'), name) for name in
                     ['xgpu_openxr_mirrored_draws', 'draw_cull_mode'])
    return (('types.inc', types), ('under_test.inc', code), ('pose.inc', geometry), ('cull.inc', cull))


@pytest.mark.parametrize('case', ['mapping', 'focus', 'pause', 'ptt', 'reference', 'privacy', 'axes',
                                 'left-handed', 'stick-choice', 'layout-change', 'physical-ptt',
                                 'left-pose', 'reflection', 'snap-hand'])
def test_native_actions(case):
    status, output = run(build('openxr_input', generated()), case)
    assert status == 0, output


def test_focus_negative_control():
    status, output = run(build('openxr_input', generated(('if (!frame.focused)', 'if (FALSE)'))), 'focus')
    assert status == CHECK_FAILED, output


@pytest.mark.parametrize('component,before,after,case', [
    ('under_test.inc', '!strcmp(config_string("vr.dominant_hand"), "left")', 'FALSE', 'left-handed'),
    ('pose.inc', 'frame.hand[hand].aim', 'frame.hand[1].aim', 'left-pose'),
    ('pose.inc', 'xgpu_openxr_mirrored_draws(FALSE);', '(void)0;', 'reflection'),
])
def test_handedness_negative_control(component, before, after, case):
    files = tuple((name, mutated(text, before, after)) if name == component else (name, text)
                  for name, text in generated())
    status, output = run(build('openxr_input', files), case)
    assert status == CHECK_FAILED, output
