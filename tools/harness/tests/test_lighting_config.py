"""Existing light effects: preset compatibility and custom saved selection."""
import sys
from pathlib import Path

import pytest

sys.path.insert(0, str(Path(__file__).resolve().parents[2]))
from harness import build, read, run


def generated(target=None):
    source = read('port/linux/game/lighting_config.c').replace('#include "lighting_config.h"', '')
    if target:
        source = '#define ' + target + '\n' + source
    return (('under_test.inc', source),)


@pytest.mark.parametrize('case', ['stock-modes', 'custom', 'reload', 'cache', 'fallback'])
def test_lighting_choices(case):
    status, output = run(build('lighting_config', generated()), case)
    assert status == 0, output


@pytest.mark.parametrize('target', ['HALO_ANDROID', 'HALO_GL_ES', 'HALO_SERVER'])
def test_unsupported_target(target):
    status, output = run(build('lighting_config', generated(target)), 'unsupported')
    assert status == 0, output
