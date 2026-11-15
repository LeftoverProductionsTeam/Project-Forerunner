"""Native synthetic settings: account actions and graphics preset previews/save."""
import sys
from pathlib import Path

import pytest

sys.path.insert(0, str(Path(__file__).resolve().parents[2]))
from harness import CHECK_FAILED, build, function, mutated, read, run, structure

NAMES = ('text_is_number', 'spinner_item', 'setting_value_index', 'setting_text', 'setting_write', 'setting_load',
         'setting_changed_save', 'settings_each', 'screen_of', 'graphics_preset_row_show',
         'graphics_preset_row_check', 'graphics_preset_show')


def generated(fault=None):
    source = read('port/linux/game/menu_functions.c')
    start = source.index('/* OpenH1/GOAT H1 baed7757')
    code = source[start:source.index("/* a setting's value as text", start)]
    code += 'static char const *graphics_preset_choice;\nstatic boolean graphics_preset_matches;\n'
    code += '\n'.join(function(source, name) for name in NAMES)
    if fault:
        code = mutated(code, *fault)
    return (('under_test_types.inc', structure(source, 'pc_menu_setting')), ('under_test.inc', code))


@pytest.mark.parametrize('case', ['chosen_already', 'chosen', 'graphics-preview', 'graphics-custom', 'graphics-save'])
def test_case(case):
    status, output = run(build('link_action_menu', generated()), case)
    assert status == 0, output


@pytest.mark.parametrize('fault,case', [
    (('setting_value_index(setting, "none")', 'setting->loaded_index'), 'chosen_already'),
    (('player_link_begin(atoi(value + 5) - 1);', 'player_link_begin(atoi(value + 5));'), 'chosen'),
    (('settings_each(screen, graphics_preset_row_show);',
      'settings_each(screen, graphics_preset_row_show); settings_each(screen, setting_changed_save);'), 'graphics-preview'),
    (('if (!graphics_preset_matches)', 'if (FALSE)'), 'graphics-custom'),
])
def test_negative_control(fault, case):
    status, output = run(build('link_action_menu', generated(fault)), case)
    assert status == CHECK_FAILED, output
