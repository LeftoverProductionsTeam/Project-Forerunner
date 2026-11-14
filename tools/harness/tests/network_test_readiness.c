#include "harness.h"

#define MAXIMUM_NUMBER_OF_LOCAL_PLAYERS 4
#define MAXIMUM_NUMBER_OF_PLAYERS 16
#define csmemset memset
#define _network_test_off 0
#define _network_test_join 2

struct network_player { boolean valid; char machine_index, controller_index; };
struct network_game_client
{
	short state, machine_index;
	struct { struct network_player players[MAXIMUM_NUMBER_OF_PLAYERS]; } game;
};
struct network_game_server
{
	struct { boolean paused, active; int timer; } countdown_state;
	boolean sent_start_game_message;
};
static struct
{
	short mode, local_players;
	boolean set_up, player_added, started, map_set, lobby_map_set;
	real player_request_seconds, setup_seconds, start_delay;
	char lobby_map[32];
} network_test;
static struct network_game_client client;
static struct network_game_server server;
static boolean have_client = TRUE, have_server = TRUE, initializing;
static int requests[4], request_states[5], starts;

static boolean network_player_is_valid(struct network_player *player) { return player->valid; }
static struct network_game_client *global_network_game_client_get(void) { return have_client ? &client : NULL; }
static struct network_game_server *global_network_game_server_get(void) { return have_server ? &server : NULL; }
static short network_game_client_get_state(struct network_game_client *c, short *data)
{
	(void)data;
	return c->state;
}
static boolean filesystem_check_thread_is_active(void) { return initializing; }
static boolean network_game_client_add_player(struct network_game_client *c, short controller)
{
	CHECK(!initializing, "requested a profile before enumeration finished");
	requests[controller]++;
	request_states[c->state]++;
	return TRUE; /* A successful reliable write is not authoritative admission. */
}
static void network_game_client_request_immediate_start(void)
{
	CHECK(!server.countdown_state.paused, "start was requested while paused");
	starts++;
}
static void platform_log(const char *text) { (void)text; }

#include "under_test.inc"

static void acknowledge(int slot, int machine, int controller)
{
	client.game.players[slot].valid = TRUE;
	client.game.players[slot].machine_index = machine;
	client.game.players[slot].controller_index = controller;
}

int main(int argc, char **argv)
{
	const char *case_name = argc > 1 ? argv[1] : "";
	int index;

	network_test.mode = _network_test_join;
	network_test.set_up = TRUE;
	network_test.local_players = 2;
	network_test.start_delay = 15.0f;
	client.state = 2;
	client.machine_index = 1;

	CASE("host-delay")
	{
		network_test.map_set = TRUE;
		server.countdown_state.active = TRUE;
		server.countdown_state.timer = 3;
		for (index = 0; index < 15; index++)
		{
			network_test.setup_seconds = (real)index;
			network_test_start_host_if_ready();
			CHECK(server.countdown_state.paused && !server.countdown_state.active && !starts,
				"automatic countdown ran before the explicit 15 second setup time");
		}
		network_test.setup_seconds = 15.0f;
		network_test_start_host_if_ready();
		CHECK(starts == 1 && network_test.started && !server.countdown_state.paused,
			"configured start did not release countdown and request start");
		return 0;
	}
	CASE("host-reset")
	{
		network_test.started = FALSE;
		network_test.setup_seconds = 0.0f;
		network_test_start_host_if_ready();
		CHECK(server.countdown_state.paused && !starts, "next game counted down during setup");
		network_test.setup_seconds = 15.0f;
		network_test_start_host_if_ready();
		CHECK(!starts, "unconfigured map started");
		network_test.map_set = TRUE;
		strcpy(network_test.lobby_map, "beavercreek");
		network_test_start_host_if_ready();
		CHECK(!starts, "lobby-map transition was skipped");
		network_test.lobby_map_set = TRUE;
		network_test_start_host_if_ready();
		CHECK(starts == 1, "next game did not start after setup");
		server.sent_start_game_message = TRUE;
		network_test_start_host_if_ready();
		CHECK(starts == 1, "loading game was started twice");
		return 0;
	}
	CASE("client-transition")
	{
		network_test_join_players(0.25f);
		CHECK(!requests[0] && !network_test.player_added, "write/elapsed time counted as acknowledgement");
		network_test_join_players(0.25f);
		CHECK(requests[0] == 1 && requests[1] == 1 && request_states[2] == 2, "pregame requests missing");
		CHECK(!network_test.player_added, "reliable writes counted as players");
		acknowledge(0, 1, 0);
		client.state = 3;
		network_test_join_players(0.25f);
		CHECK(requests[1] == 1, "retry was not bounded to half a second");
		network_test_join_players(0.25f);
		CHECK(requests[0] == 1 && requests[1] == 2 && request_states[3] == 1,
			"missing controller was not retried in-game, or acknowledged controller repeated");
		acknowledge(1, 1, 1);
		network_test_join_players(10.0f);
		CHECK(network_test.player_added && requests[1] == 2, "authoritative acknowledgement was ignored");
		return 0;
	}
	CASE("client-readiness")
	{
		initializing = TRUE;
		network_test_join_players(10.0f);
		CHECK(!requests[0] && !network_test.player_request_seconds, "profile initialization was bypassed");
		initializing = FALSE;
		for (index = 0; index <= 4; index++)
		{
			if (index == 2 || index == 3) continue;
			client.state = index;
			network_test_join_players(10.0f);
		}
		CHECK(!requests[0], "searching/joining/postgame requested a player");
		client.state = 2;
		have_client = FALSE;
		network_test_join_players(10.0f);
		have_client = TRUE;
		network_test.set_up = FALSE;
		network_test_join_players(10.0f);
		network_test.set_up = TRUE;
		network_test.mode = _network_test_off;
		network_test_join_players(10.0f);
		CHECK(!requests[0], "inactive test requested a player");
		return 0;
	}
	CASE("client-membership")
	{
		acknowledge(0, 2, 0);
		acknowledge(1, 1, 1);
		CHECK(!network_game_client_has_local_player(&client, 0), "other machine counted as local");
		CHECK(network_game_client_has_local_player(&client, 1), "own controller absent");
		CHECK(!network_game_client_has_local_player(NULL, 0) &&
			!network_game_client_has_local_player(&client, -1) &&
			!network_game_client_has_local_player(&client, 4), "invalid controller accepted");
		client.game.players[1].valid = FALSE;
		CHECK(!network_game_client_has_local_player(&client, 1), "invalid player counted as local");
		return 0;
	}
	CHECK(FALSE, "unknown case %s", case_name);
	return 1;
}
