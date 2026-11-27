"""Native session/owned-position handoff to the actual Delta voice router."""
import pytest

from harness import CHECK_FAILED, build, enum_with, function, mutated, read, run


def generated(fault=None):
    client = read('source/networking/network_client_manager.c')
    game = read('port/linux/game/voice_game.c')
    legacy_voice = read('port/linux/game/network_voice.c')
    legacy_rules = read('port/linux/game/network_voice.h')
    types = '\n'.join((enum_with(client, '_network_game_client_state_ingame'),
        enum_with(read('source/game/game.h'), '_game_connection_network_server'),
        enum_with(read('source/objects/objects.h'), '_object_dead_bit'),
        enum_with(read('source/objects/object_types.h'), '_object_mask_unit'),
        enum_with(legacy_rules, '_voice_mode_off'),
        enum_with(legacy_rules, '_voice_route_none')))
    code = '\n'.join((function(client, 'network_game_client_in_game'),
        function(read('source/game/game_engine.c'), 'game_engine_running'),
        function(read('port/linux/game/network_coop.c'), 'network_coop_active'),
        function(game, 'voice_in_game'),
        function(legacy_voice, 'voice_in_lobby'), function(legacy_voice, 'voice_route'),
        function(legacy_voice, 'voice_player_position'), function(legacy_voice, 'voice_route_between'),
        function(game, 'player_place'), function(game, 'host_world')))
    if fault:
        code = mutated(code, *fault)
    peer = read('port/linux/src/delta_peer.c')
    route = '\n'.join((function(peer, 'valid_machine'), function(peer, 'delta_peer_voice_route')))
    return (('types.inc', types), ('under_test.inc', code), ('route.inc', route),
            ('delta.h', read('port/linux/include/delta.h')),
            ('halo_console_features.h', read('port/linux/include/halo_console_features.h')))


@pytest.mark.parametrize('case', ['coop-near', 'coop-far', 'pregame', 'postgame',
    'mp-postgame', 'no-session', 'no-world', 'no-players', 'dead', 'stale', 'missing',
    'wrong-type', 'quit', 'invalid-position', 'vehicle', 'enemy-proximity', 'disabled', 'regional-proximity'])
def test_context(case):
    status, output = run(build('voice_context', generated()), case)
    assert status == 0, output


@pytest.mark.parametrize('case', ['same', 'different', 'faction', 'far', 'unknown', 'mixed'])
def test_operation_radio(case):
    status, output = run(build('voice_context', generated()), 'operation-' + case)
    assert status == 0, output


@pytest.mark.parametrize('before,after,case', [
    ('world.coop = 0;', 'world.coop = 1;', 'different'),
    ('if (group != member->team) member->team = (unsigned char)isolated;', '(void)group;', 'mixed'),
])
def test_operation_radio_negative(before, after, case):
    status, output = run(build('voice_context', generated((before, after))), 'operation-' + case)
    assert status == CHECK_FAILED, output


def test_coop_lobby_negative_control():
    status, output = run(build('voice_context', generated((
        'return network_game_client_in_game(global_network_game_client_get()) &&',
        'return game_engine_running() &&'))), 'coop-far')
    assert status == CHECK_FAILED, output


def test_stale_owner_negative_control():
    status, output = run(build('voice_context', generated((
        'unit->unit.player_index != iterator.datum_index ||', ''))), 'stale')
    assert status == CHECK_FAILED, output
