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
        ['merge_button', 'openxr_stick', 'openxr_trigger', 'openxr_gamepad_read'])
    if fault:
        code = mutated(code, *fault)
    return (('types.inc', types), ('under_test.inc', code))


@pytest.mark.parametrize('case', ['mapping', 'focus', 'pause', 'ptt', 'reference', 'privacy', 'axes'])
def test_native_actions(case):
    status, output = run(build('openxr_input', generated()), case)
    assert status == 0, output


def test_focus_negative_control():
    status, output = run(build('openxr_input', generated(('if (!frame.focused)', 'if (FALSE)'))), 'focus')
    assert status == CHECK_FAILED, output
