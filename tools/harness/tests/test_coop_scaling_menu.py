"""All 27 enemy/pickup/vehicle modes, row positions and help indices."""
import sys
import re
import xml.etree.ElementTree as ET
from pathlib import Path

import pytest

sys.path.insert(0, str(Path(__file__).resolve().parents[2]))
from harness import CHECK_FAILED, build, enum_with, function, mutated, read, run
import port_settings


def generated(fault=None):
    source = read('port/linux/game/menu_functions.c')
    # (OPENCE OPTIONS' rows name the modes: game_engine.h)
    code = enum_with(read('source/game/game_engine.h'), '_game_mode_none = 0') + '\n' + \
        function(source, 'gametype_option_help') + '\n' + \
        function(source, 'gametype_option_setting_index') + '\n' + \
        function(source, 'gametype_option_setting_write') + '\n' + function(source, 'forerunner_operation_mode')
    if fault:
        code = mutated(code, *fault)
    return (('under_test.inc', code),
            ('solo_start.inc', function(source, 'campaign_start') + '\n' + function(source, 'difficulty_start')))


@pytest.mark.parametrize('case', ['layout', 'help', 'unrelated'])
def test_case(case):
    status, output = run(build('coop_scaling_menu', generated()), case)
    assert status == 0, output


@pytest.mark.parametrize('case', ['mode-loop', 'mode-write-failure', 'mode-unrelated', 'custom-setting', 'entry-modes'])
def test_operations_mode(case):
    status, output = run(build('coop_scaling_menu', generated()), case)
    assert status == 0, output


@pytest.mark.parametrize('case', ['solo-extraction-fresh', 'solo-extraction-missing', 'solo-campaign-continue'])
def test_solo_start(case):
    status, output = run(build('coop_scaling_menu', generated()), case)
    assert status == 0, output


@pytest.mark.parametrize('fault,case', [
    (('row->vertical_offset = y;', 'row->vertical_offset = y; row->child->vertical_offset = y;'), 'layout'),
    (('if (row->visible && !strncmp(row->name, "op_coop_", 8))\n\t\t\t\t{',
      'if (!strncmp(row->name, "op_coop_", 8))\n\t\t\t\t{'), 'layout'),
    (('index += spinner->parameters.list.number_of_items;',
      'if (row->visible) index += spinner->parameters.list.number_of_items;'), 'help'),
    (('struct widget_instance *pickups = named(list, "coop_extra_pickups_spinner", 0);',
      'struct widget_instance *pickups = NULL;'), 'layout'),
    (('struct widget_instance *vehicles = named(list, "coop_extra_vehicles_spinner", 0);',
      'struct widget_instance *vehicles = NULL;'), 'layout'),
])
def test_negative_control(fault, case):
    status, output = run(build('coop_scaling_menu', generated(fault)), case)
    assert status == CHECK_FAILED, output


@pytest.mark.parametrize('control,fault', [
    ('fixed-thirty', ('spacing = MIN(30, (341 - 1 - 73 - 28) / (visible - 1));', 'spacing = 30;')),
])
def test_riding_negative_control(control, fault):
    status, output = run(build('coop_scaling_menu', generated(fault)), 'layout')
    assert status == CHECK_FAILED, output


def test_team_wipe_generated_screen():
    name = 'main_menu.multiplayer_type_select.server_settings.xml'
    base = 'main_menu/multiplayer_type_select/server_settings/coop_options/'
    text = '\n'.join(port_settings.multiplayer_files()[name])
    root = ET.fromstring(text)
    widgets = {widget.get('name'): widget for widget in root.findall('widget')}
    spinner = widgets[base + 'coop_team_wipe_spinner']
    strings = root.find(f"strings[@name='{spinner.get('string_list')}']")
    assert [item.get('text') for item in strings] == ['AUTO', 'REVERT CHECKPOINT', 'RESPAWN ONLY']
    menu = widgets[base + 'coop_options_menu']
    rows = [child for child in menu.findall('child') if '/op_coop_' in child.get('widget', '')]
    assert len(rows) == 11
    assert any(row.get('widget') == base + 'op_coop_team_wipe' for row in rows)
    help_top = int(widgets[base + 'coop_options_help'].get('top'))
    for row in rows:
        assert int(row.get('y')) + int(widgets[row.get('widget')].get('height')) < help_top
    committed = Path(__file__).resolve().parents[3] / 'port/assets/menus/ce' / name
    assert committed.read_text() == text


def test_operations_mode_generated_screen():
    text = '\n'.join(port_settings.multiplayer_files()['main_menu.multiplayer_type_select.server_settings.xml'])
    root = ET.fromstring(text)
    name = 'main_menu/multiplayer_type_select/server_settings/coop_options/coop_firefight_spinner'
    spinner = next(widget for widget in root.findall('widget') if widget.get('name') == name)
    strings = root.find(f"strings[@name='{spinner.get('string_list')}']")
    assert [item.get('text') for item in strings] == ['CLASSIC', 'FIREFIGHT', 'EXTRACTION']


def test_firefight_setup_generated_screen():
    name = 'main_menu.multiplayer_type_select.server_settings.xml'
    text = '\n'.join(port_settings.multiplayer_files()[name])
    root = ET.fromstring(text)
    widgets = {widget.get('name'): widget for widget in root.findall('widget')}
    base = 'main_menu/multiplayer_type_select/server_settings/firefight_options/'
    rows = ['ff_waves', 'ff_enemies', 'ff_growth', 'ff_lives', 'ff_resupply', 'ff_gear']
    menu = widgets[base + 'firefight_options_menu']
    assert [child.get('widget') for child in menu.findall('child')][:-1] == [base + 'op_' + row for row in rows]
    for row in rows:
        spinner = widgets[base + row + '_spinner']
        strings = root.find(f"strings[@name='{spinner.get('string_list')}']")
        option = next(line for line in read('port/linux/game/menu_functions.c').splitlines()
                      if f'"{row}_spinner", _option_setting' in line)
        assert len(strings) == int(re.search(r'_option_setting, 0, 0, (\d+)', option).group(1))
    assert widgets[base + 'op_ff_gear'].get('platform') == 'desktop'
    help_top = int(widgets[base + 'firefight_options_help'].get('top'))
    for child in menu.findall('child')[:-1]:
        assert int(child.get('y')) + int(widgets[child.get('widget')].get('height')) < help_top
    assert (Path(__file__).resolve().parents[3] / 'port/assets/menus/ce' / name).read_text() == text


def test_server_setup_help_indices_follow_generated_options():
    root = ET.fromstring('\n'.join(port_settings.multiplayer_files()['main_menu.multiplayer_type_select.server_settings.xml']))
    strings = root.find("strings[@name='main_menu/multiplayer_type_select/server_settings/help_strings']")
    source = read('port/linux/game/menu_functions.c')
    for macro, key in [('COOPERATIVE_OPTIONS_HELP', 'coop_options'),
                       ('COOPERATIVE_VOICE_OPTIONS_HELP', 'voice_options'),
                       ('COOPERATIVE_FIREFIGHT_OPTIONS_HELP', 'firefight_options')]:
        index = int(re.search(r'#define ' + macro + r' (\d+)', source).group(1))
        expected = next(help_text for name, _, help_text, _ in port_settings.COOP_SETUP_SCREENS if name == key)
        assert strings[index].get('text') == expected
    index = int(re.search(r'#define MAP_SHARE_OPTIONS_HELP (\d+)', source).group(1))
    assert strings[index].get('text') == port_settings.MAP_SHARE_SETUP[2]
