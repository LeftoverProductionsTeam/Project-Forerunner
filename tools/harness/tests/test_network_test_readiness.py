"""Debug launches use the game's real start and authoritative player APIs."""
import sys
from pathlib import Path

import pytest

sys.path.insert(0, str(Path(__file__).resolve().parents[2]))
from harness import CHECK_FAILED, build, function, mutated, read, run  # noqa: E402

TEST = 'port/linux/game/network_test.c'
MANAGER = 'source/networking/network_server_manager.c'
CLIENT = 'source/networking/network_client_manager.c'
CASES = ['host-delay', 'host-reset', 'host-local-acknowledgement',
         'menu-regional-admission', 'client-transition', 'client-readiness', 'client-membership']
CONTROLS = {
    'unpaused': ('network_game_server_pause_countdown(server, TRUE);',
                 'network_game_server_pause_countdown(server, FALSE);', 'host-delay'),
    'no-profile-gate': (' ||\n\t\tfilesystem_check_thread_is_active()', '', 'client-readiness'),
    'write-is-ack': ('network_test.player_added = all_added;',
                     'network_test.player_added = TRUE;', 'client-transition'),
    'menu-only': ('if (state != 2 && state != 3)', 'if (state != 2)', 'client-transition'),
    'host-unacknowledged': ('(network_test.local_players <= 1 || network_test_host_local_players_ready())',
                            'TRUE', 'host-local-acknowledgement'),
}


def generated(control=None):
    test, manager, client = read(TEST), read(MANAGER), read(CLIENT)
    code = '\n'.join((function(client, 'network_game_client_has_local_player'),
                       function(manager, 'network_game_server_pause_countdown'),
                       function(manager, 'network_game_server_port_start_sent'),
                       function(test, 'network_test_join_players'),
                       function(test, 'network_test_host_local_players_ready'),
                       function(test, 'network_test_start_host_if_ready')))
    if control:
        before, after, _ = CONTROLS[control]
        code = mutated(code, before, after)
    return (('under_test.inc', code),)


@pytest.mark.parametrize('case', CASES)
def test_case(case):
    status, output = run(build('network_test_readiness', generated()), case)
    assert status == 0, output


@pytest.mark.parametrize('control', CONTROLS)
def test_negative_control(control):
    status, output = run(build('network_test_readiness', generated(control)), CONTROLS[control][2])
    assert status == CHECK_FAILED, f'{control} was not caught: {output}'


def test_update_calls_before_the_menu_gate():
    update = function(read(TEST), 'network_test_update')
    assert update.index('network_test_join_players(seconds);') < update.index('if (!main_menu_loaded')
    # Both the initial setup and the running setup timer enforce the start setting.
    assert update.count('network_test_start_host_if_ready();') == 2
    assert 'network_game_server_pause_countdown(global_network_game_server_get(), TRUE);' in update


def test_membership_query_available_to_native_builds():
    source = read(CLIENT)
    query = source.index('boolean network_game_client_has_local_player(')
    browser_start = source.rfind('#ifdef HALO_BROWSER', 0, query)
    assert browser_start < source.rfind('#endif', 0, query)
