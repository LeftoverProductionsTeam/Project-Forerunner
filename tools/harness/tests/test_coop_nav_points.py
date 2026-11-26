"""Native per-viewer guidance and the existing co-op marker cache; no quest engine."""
import pytest

from harness import CHECK_FAILED, build, constant, enum_with, function, mutated, port_limits, read, run, structure


def generated(fault=None):
    port = read('port/linux/game/network_coop.c')
    native = read('source/interface/hud_nav_points.c')
    cache = enum_with(port, 'MAXIMUM_NAV_POINTS =')
    code = '\n'.join(function(native, name) for name in (
        'hud_clear_player_nav_points', 'hud_activate_nav_point',
        'hud_activate_nav_point_with_flag', 'hud_activate_nav_point_with_object',
        'hud_activate_team_nav_point', 'hud_activate_team_nav_point_with_flag',
        'hud_activate_team_nav_point_with_object',
        'hud_deactivate_nav_point', 'hud_deactivate_team_nav_point',
        'hud_deactivate_team_nav_point_with_flag', 'hud_deactivate_team_nav_point_with_object',
        'hud_unit_activate_nav_point_with_flag', 'hud_unit_activate_nav_point_with_object',
        'hud_unit_deactivate_nav_point_with_flag', 'hud_unit_deactivate_nav_point_with_object'))
    code += '\n' + '\n'.join(function(port, name) for name in (
        'send_nav_point', 'coop_remove_nav_point', 'network_coop_nav_point_object_deleted',
        'network_coop_note_nav_point', 'coop_refresh_nav_points', 'host_send_hud_state', 'object_find', 'client_apply_nav_point',
        'network_coop_checkpoint_saved', 'client_clear_nav_points', 'network_coop_checkpoint_reset',
        'network_coop_reset_recovery', 'network_coop_reverted'))
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
    elif fault == 'cache':
        cache = mutated(cache,
            '(HALO_PORT_MAXIMUM_NETWORK_PLAYERS + NUMBER_OF_SOLO_CAMPAIGN_TEAMS) * MAXIMUM_NUMBER_OF_NAV_POINTS', '16')
    elif fault == 'retry':
        code = mutated(code, '\n\t\t\tbreak;\n\t\thost_hud_state.nav_refresh_next++;',
                       '\n\t\t\t(void)0;\n\t\thost_hud_state.nav_refresh_next++;')
    elif fault == 'budget':
        cache = mutated(cache, 'NAV_POINT_REFRESHES_PER_TICK = 8', 'NAV_POINT_REFRESHES_PER_TICK = 128')
    elif fault == 'lifetime-handle':
        code = mutated(code, 'host_hud_state.nav_points[index].target == object_index',
                       'DATUM_INDEX_TO_ABSOLUTE_INDEX(host_hud_state.nav_points[index].target) == '
                       'DATUM_INDEX_TO_ABSOLUTE_INDEX(object_index)')
    elif fault == 'lifetime-replay':
        code = mutated(code, 'if (host_hud_state.nav_refresh_remaining)\n'
                       '\t\thost_hud_state.nav_refresh_remaining = host_hud_state.nav_point_count;',
                       '(void)host_hud_state.nav_point_count;')
    elif fault == 'ordering':
        code = mutated(code, 'if (!flag && activate && !object_try_and_get_and_verify_type(marker, _object_mask_all))',
                       'if (FALSE)')
    elif fault == 'deactivation':
        code = mutated(code, 'if (!flag && activate && !object_try_and_get_and_verify_type(marker, _object_mask_all))',
                       'if (!flag && !object_try_and_get_and_verify_type(marker, _object_mask_all))')
    return (
        ('types.inc', '#define MAXIMUM_NUMBER_OF_NAV_POINTS %d\n#define HALO_PORT_MAXIMUM_NETWORK_PLAYERS %d\n#define MAXIMUM_QUEUED_EVENTS %d\n' % (
            constant(read('source/interface/hud.h'), 'MAXIMUM_NUMBER_OF_NAV_POINTS'),
            constant(port_limits(), 'HALO_PORT_MAXIMUM_NETWORK_PLAYERS'), constant(port, 'MAXIMUM_QUEUED_EVENTS')) +
         enum_with(read('source/game/game.h'), 'NUMBER_OF_SOLO_CAMPAIGN_TEAMS,') + '\n' + cache + '\n' +
         enum_with(native, '_hud_nav_point_type_flag') + '\n' +
         enum_with(read('port/linux/game/network_coop.h'), '_coop_nav_point_team_flag') + '\n' +
         structure(native, 'hud_nav_point_datum') + '\n' + structure(native, 'hud_nav_point_player_datum') + '\n' +
         structure(port, 'coop_checkpoint_players')),
        ('under_test.inc', code),
        ('deleted.inc', '\n'.join(line for line in read('source/objects/object_deleted_procs.c').splitlines()
                                  if not line.startswith('#include'))),
    )


@pytest.mark.parametrize('case', ['host', 'client', 'switch', 'respawn', 'unit',
                                 'deactivate', 'stale', 'deleted', 'unchanged', 'capacity', 'legacy', 'restore',
                                 'squads-host', 'squads-client', 'replay', 'congested', 'shrinking', 'removed'])
def test_coop_nav_points(case):
    status, output = run(build('coop_nav_points', generated()), case)
    assert status == 0, output


@pytest.mark.parametrize('fault,case', [('client', 'client'), ('clear', 'switch'), ('team', 'host'), ('restore', 'restore'),
                                     ('cache', 'squads-host'), ('retry', 'congested'), ('budget', 'replay')])
def test_coop_nav_points_negative(fault, case):
    status, output = run(build('coop_nav_points', generated(fault)), case)
    assert status == CHECK_FAILED, output


@pytest.mark.parametrize('case', ['host', 'client', 'objects', 'handles', 'replay', 'ordinary', 'progress'])
def test_deleted_marker_lifecycle(case):
    status, output = run(build('coop_nav_points', generated()), 'lifetime-' + case)
    assert status == 0, output


@pytest.mark.parametrize('fault,case', [('lifetime-handle', 'handles'), ('lifetime-replay', 'progress'), ('callback', 'host')])
def test_deleted_marker_negative(fault, case):
    files = generated(None if fault == 'callback' else fault)
    if fault == 'callback':
        files = tuple((name, mutated(code, '\tnetwork_coop_nav_point_object_deleted',
                                    '\tobjects_fix_for_deleted_object')) if name == 'deleted.inc' else (name, code)
                      for name, code in files)
    status, output = run(build('coop_nav_points', files), 'lifetime-' + case)
    assert status == CHECK_FAILED, output


@pytest.mark.parametrize('case', ['pending', 'deactivate', 'named', 'item'])
def test_replica_marker_ordering(case):
    status, output = run(build('coop_nav_points', generated()), 'ordering-' + case)
    assert status == 0, output


@pytest.mark.parametrize('fault,case', [('ordering', 'pending'), ('deactivation', 'deactivate')])
def test_replica_marker_ordering_negative(fault, case):
    status, output = run(build('coop_nav_points', generated(fault)), 'ordering-' + case)
    assert status == CHECK_FAILED, output


@pytest.mark.parametrize('case', ['host', 'client', 'modern-client', 'roster'])
def test_checkpoint_guidance(case):
    status, output = run(build('coop_nav_points', generated()), 'checkpoint-' + case)
    assert status == 0, output
