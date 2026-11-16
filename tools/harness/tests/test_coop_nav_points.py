"""Native per-viewer guidance and the existing co-op marker cache; no quest engine."""
import pytest

from harness import CHECK_FAILED, build, constant, enum_with, function, mutated, read, run, structure


def generated(fault=None):
    port = read('port/linux/game/network_coop.c')
    native = read('source/interface/hud_nav_points.c')
    code = '\n'.join(function(native, name) for name in (
        'hud_clear_player_nav_points', 'hud_activate_nav_point',
        'hud_activate_nav_point_with_flag', 'hud_activate_nav_point_with_object',
        'hud_activate_team_nav_point', 'hud_activate_team_nav_point_with_flag',
        'hud_deactivate_nav_point', 'hud_deactivate_team_nav_point',
        'hud_deactivate_team_nav_point_with_flag'))
    code += '\n' + '\n'.join(function(port, name) for name in (
        'network_coop_note_nav_point', 'coop_refresh_nav_points'))
    if fault == 'client':
        code = mutated(code, 'if (!coop_host() && !coop_client())', 'if (!coop_host())')
    elif fault == 'clear':
        code = mutated(code, 'hud_clear_player_nav_points(player_index);', '(void)player_index;')
    elif fault == 'team':
        code = mutated(code, 'if (team ? target != player->team_index : target != player->unit_index) continue;',
                       '(void)team; (void)target;')
    elif fault == 'restore':
        code = mutated(code,
                       'if (!coop_presentation.nav_players[local].seen || coop_presentation.nav_players[local].player != player_index)',
                       'if (FALSE)')
    return (
        ('types.inc', '#define MAXIMUM_NUMBER_OF_NAV_POINTS %d\n#define MAXIMUM_NAV_POINTS %d\n' % (
            constant(native, 'MAXIMUM_NUMBER_OF_NAV_POINTS'), constant(port, 'MAXIMUM_NAV_POINTS')) +
         enum_with(native, '_hud_nav_point_type_flag') + '\n' +
         enum_with(read('port/linux/game/network_coop.h'), '_coop_nav_point_team_flag') + '\n' +
         structure(native, 'hud_nav_point_datum') + '\n' + structure(native, 'hud_nav_point_player_datum')),
        ('under_test.inc', code),
    )


@pytest.mark.parametrize('case', ['host', 'client', 'switch', 'respawn', 'unit',
                                 'deactivate', 'stale', 'deleted', 'unchanged', 'capacity', 'legacy', 'restore'])
def test_coop_nav_points(case):
    status, output = run(build('coop_nav_points', generated()), case)
    assert status == 0, output


@pytest.mark.parametrize('fault,case', [('client', 'client'), ('clear', 'switch'), ('team', 'host'), ('restore', 'restore')])
def test_coop_nav_points_negative(fault, case):
    status, output = run(build('coop_nav_points', generated(fault)), case)
    assert status == CHECK_FAILED, output
