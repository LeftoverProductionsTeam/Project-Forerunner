"""Native wave resupply uses the same actual ownership code as recovery."""
import pytest

from harness import CHECK_FAILED, build, mutated, run
from harness.tests.test_extraction import generated


@pytest.mark.parametrize('case', [
    'owned', 'stowed', 'two-owned', 'battery', 'stowed-battery', 'transferred',
    'dropped', 'stowed-owner', 'stowed-ground', 'stowed-unmarked',
    'stowed-nonplayer', 'mixed-stale', 'quit', 'dead', 'pending',
    'pending-silent', 'pending-no-stats', 'wrong-body', 'disabled', 'objective', 'full',
])
def test_resupply(case):
    status, output = run(build('extraction', generated()), 'resupply-' + case)
    assert status == 0, output


@pytest.mark.parametrize('case', ['native', 'classic', 'explicit', 'shield-only',
                                   'unshielded', 'incapacitated', 'additive'])
def test_equipment_only_respawn(case):
    status, output = run(build('extraction', generated()), 'resupply-profile-' + case)
    assert status == 0, output


def test_equipment_only_respawn_negative_control():
    files = tuple((name, mutated(text, '!coop_enemies_firefight_active() ||', 'TRUE ||'))
                  if name == 'spawn_equipment.inc' else (name, text) for name, text in generated())
    status, output = run(build('extraction', files), 'resupply-profile-native')
    assert status == CHECK_FAILED, output


def test_stowed_negative_control():
    files = tuple((name, mutated(text, 'if (!weapon)',
                  'if (!weapon || weapon->object.parent_object_index != player->unit_index)'))
                  if name == 'resupply.inc' else (name, text) for name, text in generated())
    status, output = run(build('extraction', files), 'resupply-stowed')
    assert status == CHECK_FAILED, output


def test_ownership_negative_control():
    files = tuple((name, mutated(text,
                  '!game_modes_inventory_capture(iterator.datum_index, &inventory)',
                  '(inventory.weapon_count = 1, inventory.weapons[0].slot = 0, FALSE)'))
                  if name == 'resupply.inc' else (name, text) for name, text in generated())
    status, output = run(build('extraction', files), 'resupply-transferred')
    assert status == CHECK_FAILED, output
