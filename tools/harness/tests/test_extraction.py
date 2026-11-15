"""Connected real Extraction, gear transactions and native equipment helpers.

The bounded world/identity/OS adapters are fixtures, not native multiplayer proof.
Real native file/process behavior is covered by tools/test_gear_files.py.
"""
import re
import pytest

from harness import CHECK_FAILED, build, enum_with, function, mutated, read, run
from harness.tests.test_gear import generated as gear_generated


def generated(fault=None, outcome_fault=False, medical_fault=False):
    files = list(gear_generated())
    inventory = read('port/linux/game/game_modes.c')
    names = ('capture', 'grenade_room', 'weapon_compatible', 'loadout_valid', 'apply', 'consume')
    equipment = '\n'.join(function(inventory, 'game_modes_inventory_' + name) for name in names)
    if fault:
        equipment = mutated(equipment, *fault)
    files.append(('equipment.inc', equipment))
    files.append(('equipment_flags.inc', enum_with(read('source/items/weapon_definitions.h'), '_weapon_must_be_readied_bit')))
    director = read('port/linux/game/coop_enemies.c')
    survival = '\n'.join(function(director, name) for name in (
        'coop_enemies_host', 'coop_enemies_firefight_active',
        'coop_enemies_firefight_gear_active', 'coop_enemies_firefight_outcome'))
    if outcome_fault:
        survival = mutated(survival, 'firefight.victory ? 1 : -1', '1')
    files.append(('survival.inc', survival))
    extraction = re.sub(r'^#include.*\n', '',
        read('port/linux/game/extraction.h') + '\n' + read('port/linux/game/extraction.c'), flags=re.M)
    if medical_fault:
        extraction = mutated(extraction,
            'now - medical_players[healer_slot].last_damage < 2 * TICKS_PER_SECOND ||', '')
    files.append(('extraction.inc', extraction))
    devices = read('source/devices/devices.c')
    controls = read('source/devices/device_controls.c')
    files.append(('controls_types.inc', enum_with(devices, '_device_not_usable_bit')))
    files.append(('controls.inc', '\n'.join((function(devices, 'device_can_change_position'),
                                            function(controls, 'control_toggle'), function(controls, 'control_touched')))))
    return tuple(files)


@pytest.mark.parametrize('case', ['loop', 'loadout-choice', 'stash-discard', 'discard-pending', 'discard-state', 'discard-identity',
                                 'transfer', 'grant-failure', 'authority', 'native-controls',
                                 'shared-cache', 'shared-cache-reject', 'shared-cache-failure', 'shared-cache-discard',
                                 'restore', 'pending', 'binding', 'disconnect',
                                 'survival-win', 'survival-loss', 'survival-retry',
                                 'survival-restore', 'survival-respawn',
                                 'medical-authority', 'medical-revive', 'medical-interrupt',
                                 'medical-bleed', 'medical-replica', 'medical-restore', 'medical-gear'])
def test_extraction_workflow(case):
    status, output = run(build('extraction', generated()), case)
    assert status == 0, output


def test_transfer_negative_control():
    status, output = run(build('extraction', generated((
        'if (!weapon || weapon->object.parent_object_index != player->unit_index)',
        'if (!weapon)'))), 'transfer')
    assert status == CHECK_FAILED, output


def test_native_deployment_signal_negative_control():
    files = tuple((name, mutated(text,
        'return !extraction_deploy(run, player);',
        'extraction_deploy(run, player); return TRUE;'))
        if name == 'extraction.inc' else (name, text) for name, text in generated())
    status, output = run(build('extraction', files), 'native-controls')
    assert status == CHECK_FAILED, output


def test_shared_cache_native_admission_negative_control():
    files = tuple((name, mutated(text,
        'return game_modes_inventory_weapon_compatible(&item->equipment);',
        'return TRUE;')) if name == 'under_test.inc' else (name, text)
        for name, text in generated())
    status, output = run(build('extraction', files), 'shared-cache-reject')
    assert status == CHECK_FAILED, output


def test_failed_survival_negative_control():
    status, output = run(build('extraction', generated(outcome_fault=True)), 'survival-loss')
    assert status == CHECK_FAILED, output


def test_medical_combat_negative_control():
    status, output = run(build('extraction', generated(medical_fault=True)), 'medical-interrupt')
    assert status == CHECK_FAILED, output
