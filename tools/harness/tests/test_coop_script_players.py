"""Script player0/followers must agree with the engine's actual players list."""
import sys
from pathlib import Path
import pytest
sys.path.insert(0, str(Path(__file__).resolve().parents[2]))
from harness import build, read, run, function, inline, constant, structure, mutated, port_limits, CHECK_FAILED


def generated(fault=None):
    port = read('port/linux/game/coop_scripts.c')
    followers = function(port, 'players_following')
    if fault == 'forward':
        followers = mutated(followers, 'players[player_count - 1] != unit_index', 'players[0] != unit_index')
    if fault == 'named':
        followers = mutated(followers, 'player_count - NAMED_PLAYERS - 1', 'player_count - 2')
    policy = 'static boolean coop_scripts_adaptations;\n' + function(port, 'coop_scripts_new_game')
    policy += '\n' + function(port, 'coop_scripts_adaptations_active')
    policy += '\n' + function(port, 'coop_scripts_native_player_actions')
    if fault == 'adaptations':
        policy = mutated(policy, 'config_boolean("network.coop_script_adaptations")', 'TRUE')
    return (
        ('cooperative.inc', function(read('source/game/game.c'), 'game_is_cooperative')),
        ('reference.inc', structure(read('source/objects/reference_lists.h'), 'data_reference') + '\n' +
         inline(read('source/objects/reference_lists.h'), 'reference_list_add')),
        ('players.inc', function(read('source/hs/hs_library_external.c'), 'hs_players')),
        ('followers.inc', '#define HALO_PORT_MAXIMUM_NETWORK_PLAYERS ' +
         str(constant(port_limits(), 'HALO_PORT_MAXIMUM_NETWORK_PLAYERS')) +
         '\n#define NAMED_PLAYERS ' + str(constant(port, 'NAMED_PLAYERS')) + '\n' + policy + '\n' + followers),
        ('actions.inc', '\n'.join(function(port, name) for name in [
            'coop_scripts_teleport_followers', 'coop_scripts_suspend_followers',
            'coop_scripts_exit_followers', 'follower_seat_name',
            'coop_scripts_board_followers', 'coop_scripts_player_add_equipment', 'coop_scripts_safe_to_save'])),
        ('vitality.inc', function(read('port/linux/game/network_coop.c'), 'network_coop_set_players_vitality') + '\n' +
         function(read('source/units/unit_scripting_commands.c'), 'unit_scripting_set_current_vitality')),
    )


@pytest.mark.parametrize('case', ['counts', 'gaps', 'respawn', 'bounded', 'nonhost', 'teleport', 'suspend', 'exit', 'board', 'equipment', 'native-actions', 'native-save',
                                 'native-current-vitality', 'legacy-current-vitality', 'legacy-maximum-vitality', 'cooperative-identity'])
def test_script_players(case):
    status, output = run(build('coop_script_players', generated()), case)
    assert status == 0, output


@pytest.mark.parametrize('fault', ['forward', 'named'])
def test_negative(fault):
    status, output = run(build('coop_script_players', generated(fault)), 'counts')
    assert status == CHECK_FAILED, output


def test_native_actions_negative():
    status, output = run(build('coop_script_players', generated('adaptations')), 'native-actions')
    assert status == CHECK_FAILED, output
