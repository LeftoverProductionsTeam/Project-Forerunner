"""Connected real Extraction, gear transactions and native equipment helpers.

The bounded world/identity/OS adapters are fixtures, not native multiplayer proof.
Real native file/process behavior is covered by tools/test_gear_files.py.
"""
import re
import pytest

from harness import CHECK_FAILED, build, function, mutated, read, run
from harness.tests.test_gear import generated as gear_generated


def generated(fault=None, outcome_fault=False):
    files = list(gear_generated())
    inventory = read('port/linux/game/game_modes.c')
    names = ('capture', 'grenade_room', 'loadout_valid', 'apply', 'consume')
    equipment = '\n'.join(function(inventory, 'game_modes_inventory_' + name) for name in names)
    if fault:
        equipment = mutated(equipment, *fault)
    files.append(('equipment.inc', equipment))
    director = read('port/linux/game/coop_enemies.c')
    survival = '\n'.join(function(director, name) for name in (
        'coop_enemies_host', 'coop_enemies_firefight_active',
        'coop_enemies_firefight_gear_active', 'coop_enemies_firefight_outcome'))
    if outcome_fault:
        survival = mutated(survival, 'firefight.victory ? 1 : -1', '1')
    files.append(('survival.inc', survival))
    files.append(('extraction.inc', re.sub(r'^#include.*\n', '',
        read('port/linux/game/extraction.c'), flags=re.M)))
    return tuple(files)


@pytest.mark.parametrize('case', ['loop', 'transfer', 'grant-failure', 'authority',
                                 'restore', 'pending', 'binding', 'disconnect',
                                 'survival-win', 'survival-loss', 'survival-retry',
                                 'survival-restore', 'survival-respawn'])
def test_extraction_workflow(case):
    status, output = run(build('extraction', generated()), case)
    assert status == 0, output


def test_transfer_negative_control():
    status, output = run(build('extraction', generated((
        'if (!weapon || weapon->object.parent_object_index != player->unit_index)',
        'if (!weapon)'))), 'transfer')
    assert status == CHECK_FAILED, output


def test_failed_survival_negative_control():
    status, output = run(build('extraction', generated(outcome_fault=True)), 'survival-loss')
    assert status == CHECK_FAILED, output
