"""Firefight reserve/battery refill must respect native weapon parent ownership."""
import sys
from pathlib import Path

import pytest

sys.path.insert(0, str(Path(__file__).resolve().parents[2]))
from harness import CHECK_FAILED, build, enum_with, function, mutated, read, run


def generated(fault=None):
    types = '\n'.join([
        enum_with(read('source/units/units.h'), 'MAXIMUM_WEAPONS_PER_UNIT ='),
        enum_with(read('source/objects/objects.h'), '_object_dead_bit'),
        enum_with(read('source/objects/object_types.h'), '_object_mask_unit'),
        enum_with(read('source/items/items.h'), '_item_attached_to_unit_bit'),
    ])
    code = function(read('port/linux/game/coop_enemies.c'), 'firefight_resupply_players')
    if fault:
        code = mutated(code, *fault)
    return (('types.inc', types), ('under_test.inc', code))


@pytest.mark.parametrize('case', ['owned', 'transferred', 'dropped', 'battery', 'ineligible'])
def test_resupply(case):
    status, output = run(build('firefight_resupply', generated()), case)
    assert status == 0, output


def test_ownership_negative_control():
    status, output = run(build('firefight_resupply', generated((
        'weapon->object.parent_object_index != player->unit_index ||', ''))), 'transferred')
    assert status == CHECK_FAILED, output
