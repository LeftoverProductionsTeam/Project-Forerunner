"""Authoritative no-engine co-op replaces the preceding multiplayer variant."""
import sys
from pathlib import Path

import pytest

sys.path.insert(0, str(Path(__file__).resolve().parents[2]))
from harness import CHECK_FAILED, build, function, mutated, read, run  # noqa: E402


def generated(old_guard=False):
    code = function(read('source/networking/network_game_manager.c'), 'network_game_create_game_objects')
    if old_guard:
        code = mutated(code,
            '\tgame_set_game_variant(&game->variant);\n\tgame_set_game_variant_options(&game->variant_options);',
            '\tif (game->variant.game_engine_index)\n\t{\n'
            '\t\tgame_set_game_variant(&game->variant);\n'
            '\t\tgame_set_game_variant_options(&game->variant_options);\n\t}')
    return (('under_test.inc', code),)


@pytest.mark.parametrize('case', ['coop-after-slayer', 'coop-after-multiplayer', 'multiplayer-after-coop',
                                 'failed-precache', 'failed-load', 'failed-player', 'film'])
def test_case(case):
    status, output = run(build('network_game_variant', generated()), case)
    assert status == 0, output


@pytest.mark.parametrize('case', ['coop-after-slayer', 'coop-after-multiplayer'])
def test_old_guard_negative_control(case):
    status, output = run(build('network_game_variant', generated(True)), case)
    assert status == CHECK_FAILED, f'old guard retained the stale multiplayer variant: {output}'
