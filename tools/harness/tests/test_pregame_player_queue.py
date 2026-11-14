"""A reliable player request crossing the lobby's start keeps its place in the game."""
import sys
from pathlib import Path

import pytest

sys.path.insert(0, str(Path(__file__).resolve().parents[2]))
from harness import CHECK_FAILED, build, enum_with, function, mutated, read, run  # noqa: E402

MANAGER = 'source/networking/network_server_manager.c'
HANDLER = 'source/networking/network_server_message_handler.c'
HANDLER_NAME = 'network_game_server_handle_message_client_add_player_request_pregame'
CASES = ['loading', 'loaded-first', 'ownership', 'duplicates', 'capacity', 'rejected', 'lobby', 'late-join',
         'start-preserves']


def generated(old_condition=False):
    manager, handler = read(MANAGER), read(HANDLER)
    code = '\n'.join(function(manager, name) for name in (
        'network_game_server_lobby_is_open', 'network_game_server_client_machine_is_loaded',
        'network_game_server_queue_player_for_addition', 'network_game_server_port_clear_queued_players',
        'network_game_server_start_network_game', 'network_game_server_all_machines_have_loaded'))
    code += '\n' + function(handler, 'network_game_server_queue_client_player')
    admission = function(handler, HANDLER_NAME)
    if old_condition:
        admission = mutated(admission,
            'else if (network_game_server_get_state(server, NULL) == _network_game_server_state_pregame ||\n'
            '\t\t(network_game_server_get_state(server, NULL) == _network_game_server_state_ingame &&\n'
            '\t\t\t!network_game_server_client_machine_is_loaded(server, client_machine)))',
            'else if (network_game_server_get_state(server, NULL) == _network_game_server_state_ingame &&\n'
            '\t\t!network_game_server_client_machine_is_loaded(server, client_machine))')
    enums = '\n'.join((enum_with(manager, '_network_game_server_state_pregame'),
                       enum_with(manager, '_network_client_machine_connected_bit')))
    return (('enums.inc', enums), ('under_test.inc', code + '\n' + admission))


@pytest.mark.parametrize('case', CASES)
def test_case(case):
    status, output = run(build('pregame_player_queue', generated()), case)
    assert status == 0, output


@pytest.mark.parametrize('case', ['loading', 'loaded-first'])
def test_old_condition_negative_control(case):
    status, output = run(build('pregame_player_queue', generated(True)), case)
    assert status == CHECK_FAILED, f'old admission condition was not caught: {output}'
