/* Real lobby admission, ownership check, bounded queue and start transitions.
   The decoder and transport stand in for already validated reliable messages. */
#include "harness.h"
#define HALO_PORT_MAXIMUM_NETWORK_PLAYERS 16
#define HALO_PORT_MAXIMUM_NETWORK_MACHINES 4
#define HALO_PORT_MAXIMUM_NETWORK_MESSAGE_SIZE 0x1000
#define MILLISECONDS_PER_SECOND 1000
#define MAXIMUM_LOCAL_PLAYERS 4
#include "enums.inc"

typedef unsigned short word;
#define TICKS_PER_SECOND 30
#define NUMBEROF(a) (sizeof(a) / sizeof((a)[0]))
#define csmemcpy memcpy
#define match_vassert(file, line, condition, text) CHECK(condition, "%s", text)
#define NETWORK_GAME_MESSAGE_VERSION 1
#define _message_client_add_player_request_pregame 1
#define _network_game_packet_class_client_pregame 3
#define _message_server_begin_game 2

struct network_player { short machine_index, controller_index; boolean valid; };
struct network_game
{
	struct network_player players[MAXIMUM_NETWORK_PLAYER_COUNT];
	struct { boolean game_objects_loaded; } local_data;
};
struct network_game_server_client_machine { short machine_index; unsigned long flags; };
struct network_game_server
{
	short state;
	boolean sent_start_game_message, queued_player_valid;
	long next_update_number;
	unsigned long time_of_first_client_loading_completion;
	struct network_game game;
	struct network_game_server_client_machine client_machines[MAXIMUM_NETWORK_MACHINE_COUNT];
	struct network_player queued_player, waiting_players[MAXIMUM_NETWORK_PLAYER_COUNT];
	short waiting_player_count;
};
struct message_server_begin_game { short unused; };

static struct network_game_server server;
static boolean network_game_server_map_pending[MAXIMUM_NETWORK_MACHINE_COUNT];
static struct network_player network_game_server_start_players[MAXIMUM_NETWORK_PLAYER_COUNT];
static long network_game_server_frequent_updates_until[MAXIMUM_NETWORK_MACHINE_COUNT];
static int immediate_additions, settings_sent, starts_sent;
static boolean decode_ok = TRUE;

static void network_event(const char *format, ...) { (void)format; }
static boolean network_player_is_valid(struct network_player *player) { return player->valid; }
static short network_game_server_get_state(struct network_game_server *s, void *unused)
{
	(void)unused;
	return s->state;
}
static void network_game_server_get_client_machine(struct network_game_server *s,
	struct network_game_server_client_machine *m, long *index) { *index = m - s->client_machines; }
static void network_game_server_clean_player_name(struct network_game_server *s,
	struct network_player *p, long index) { (void)s; (void)p; (void)index; }
static boolean decode_network_game_message(struct network_player *player, word *bytes, short *size,
	short *type, short *version, int packet_class)
{
	CHECK(*type == _message_client_add_player_request_pregame && *version == NETWORK_GAME_MESSAGE_VERSION &&
		packet_class == _network_game_packet_class_client_pregame, "wrong decoding contract");
	if (!decode_ok || *size != sizeof(*player)) return FALSE;
	memcpy(player, bytes, sizeof(*player));
	return TRUE;
}
static boolean network_game_server_add_player_to_game(struct network_game_server *s,
	struct network_game_server_client_machine *m, struct network_player *p)
{
	(void)m;
	immediate_additions++;
	s->game.players[0] = *p;
	return TRUE;
}
static boolean network_game_server_send_game_data_pregame(struct network_game_server *s)
{
	(void)s;
	settings_sent++;
	return TRUE;
}
static boolean network_game_server_client_machine_is_joined_to_game(struct network_game_server *s,
	struct network_game_server_client_machine *m) { (void)s; return m->machine_index != NONE; }
static boolean network_game_server_send_message_to_client_machine(struct network_game_server *s,
	struct network_game_server_client_machine *m, void *message)
{
	(void)s; (void)m; (void)message;
	starts_sent++;
	return TRUE;
}
static void *create_network_game_message(int type, void *data, long size)
{
	(void)type; (void)size;
	return data;
}
static boolean network_game_server_send_game_settings_to_all_machines(struct network_game_server *s,
	void *data, long size) { (void)s; (void)data; (void)size; settings_sent++; return TRUE; }
static struct network_game *network_game_client_get_game(void *client) { (void)client; return &server.game; }
#define mcc_ui_network_map_pending(s) FALSE
#define network_game_port_vehicle_riding_start(g) ((void)0)
#define network_game_server_hold_map_fetcher_players(s) ((void)0)
#define network_game_server_discard_held_players(i) ((void)0)
#define map_share_machine_needs_map(i) FALSE
#define mcc_load_network_start(g) ((void)0)
#define network_game_server_restore_held_players(s) ((void)0)
#define mcc_load_network_disconnect() ((void)0)
#define network_game_server_client_machine_heard(s, m) ((void)0)
#define network_game_server_remove_players_gone_while_loading(s) ((void)0)
#define global_network_game_client_get() (&server)

#include "under_test.inc"

static void request(short machine_index, short controller, boolean valid)
{
	word packet[(sizeof(struct network_player) + sizeof(word)) / sizeof(word)];
	struct network_player player = { machine_index, controller, valid };

	packet[0] = 0;
	memcpy(packet + 1, &player, sizeof(player));
	network_game_server_handle_message_client_add_player_request_pregame(&server,
		&server.client_machines[1], packet, sizeof(packet));
}

static void unchanged_settings(void)
{
	CHECK(!immediate_additions && !settings_sent, "loading request changed the start settings");
	CHECK(!network_player_is_valid(&server.game.players[0]), "player entered the start snapshot");
}

int main(int argc, char **argv)
{
	const char *case_name = argc > 1 ? argv[1] : "";
	int index;

	for (index = 0; index < MAXIMUM_NETWORK_MACHINE_COUNT; index++)
		server.client_machines[index].machine_index = NONE;
	server.client_machines[1].machine_index = 1;
	server.state = _network_game_server_state_pregame;
	server.sent_start_game_message = TRUE;
	server.game.local_data.game_objects_loaded = TRUE;

	CASE("loaded-first") server.client_machines[1].flags |= FLAG(_network_client_machine_level_loaded_bit);
	if (!strcmp(case_name, "loading") || !strcmp(case_name, "loaded-first"))
	{
		request(1, 0, TRUE);
		CHECK(server.queued_player_valid, "request crossing start was discarded");
		CHECK(server.queued_player.machine_index == 1, "wrong owner queued");
		unchanged_settings();
		network_game_server_all_machines_have_loaded(&server);
		CHECK(server.state == _network_game_server_state_ingame && server.queued_player_valid,
			"loading completion lost the request before the in-game drain");
		return 0;
	}
	CASE("ownership")
	{
		request(2, 0, TRUE);
		CHECK(!server.queued_player_valid, "machine queued another machine's player");
		unchanged_settings();
		return 0;
	}
	CASE("duplicates")
	{
		request(1, 0, TRUE);
		request(1, 0, TRUE);
		request(1, 1, TRUE);
		request(1, 1, TRUE);
		CHECK(server.queued_player_valid && server.waiting_player_count == 1, "duplicate queued");
		server.game.players[0] = server.queued_player;
		network_game_server_port_clear_queued_players(&server);
		request(1, 0, TRUE);
		CHECK(!server.queued_player_valid && !server.waiting_player_count, "existing player queued");
		return 0;
	}
	CASE("capacity")
	{
		for (index = 0; index < 2 * MAXIMUM_NETWORK_PLAYER_COUNT; index++) request(1, index, TRUE);
		CHECK(server.queued_player_valid && server.waiting_player_count == MAXIMUM_NETWORK_PLAYER_COUNT,
			"queue exceeded its bounded capacity");
		return 0;
	}
	CASE("rejected")
	{
		decode_ok = FALSE;
		request(1, 0, TRUE);
		decode_ok = TRUE;
		request(1, 0, FALSE);
		server.state = _network_game_server_state_postgame;
		request(1, 0, TRUE);
		server.state = _network_game_server_state_ingame;
		server.client_machines[1].flags |= FLAG(_network_client_machine_level_loaded_bit);
		request(1, 0, TRUE);
		CHECK(!server.queued_player_valid && !server.waiting_player_count, "rejected request queued");
		unchanged_settings();
		return 0;
	}
	CASE("lobby")
	{
		server.sent_start_game_message = FALSE;
		request(1, 0, TRUE);
		CHECK(immediate_additions == 1 && settings_sent == 1 && !server.queued_player_valid,
			"open lobby stopped adding players normally");
		return 0;
	}
	CASE("late-join")
	{
		server.state = _network_game_server_state_ingame;
		request(1, 0, TRUE);
		CHECK(server.queued_player_valid, "ordinary late join was discarded");
		unchanged_settings();
		return 0;
	}
	CASE("start-preserves")
	{
		request(1, 0, TRUE);
		request(1, 1, TRUE);
		server.sent_start_game_message = FALSE;
		CHECK(network_game_server_start_network_game(&server), "start failed");
		CHECK(starts_sent == 1 && settings_sent == 1 && server.sent_start_game_message, "start not sent");
		CHECK(server.queued_player_valid && server.waiting_player_count == 1, "start cleared the queue");
		network_game_server_port_clear_queued_players(&server);
		CHECK(!server.queued_player_valid && !server.waiting_player_count, "fresh-game reset kept the queue");
		return 0;
	}
	CHECK(FALSE, "unknown case %s", case_name);
	return 1;
}
