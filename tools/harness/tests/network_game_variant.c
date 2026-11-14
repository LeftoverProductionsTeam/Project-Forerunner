/* Real network map-load handoff; game initialization observes the selected
   variant before any campaign scripts or multiplayer callbacks run. */
#include "harness.h"

#define NETWORK_GAME_PLAYER_SLOTS 4
#define _game_connection_network_client 1
#define _game_connection_network_server 2
#define _game_connection_film_playback 3
#define csstrncpy strncpy

struct game_variant { long game_engine_index; long rules; };
struct game_variant_options { long rules; };
struct network_player { boolean valid; char player_list_index; };
struct network_game
{
	struct { char name[64]; } map;
	short difficulty;
	long random_seed;
	struct game_variant variant;
	struct game_variant_options variant_options;
	struct { boolean game_objects_loaded; } local_data;
	struct network_player players[NETWORK_GAME_PLAYER_SLOTS];
};
struct game_options { char map_name[64]; short difficulty; long random_seed; };

static struct game_variant current_variant;
static struct game_variant_options current_options;
static struct network_game game;
static boolean precache_ok = TRUE, load_ok = TRUE, spawn_ok = TRUE;
static boolean old_active = TRUE, old_loaded = TRUE;
static int connection = _game_connection_network_server;
static int disposed, unloaded, initialized, spawns, variant_calls, option_calls, failures;

static void game_options_new(struct game_options *options) { memset(options, 0, sizeof(*options)); }
static int game_connection(void) { return connection; }
static long network_game_get_random_seed(void) { return 1234; }
static boolean game_precache_new_map(char const *map_name, boolean network)
{
	CHECK(!strcmp(map_name, game.map.name) && network, "wrong precache");
	return precache_ok;
}
static boolean mcc_main_network_load_failed(char const *name)
{
	(void)name;
	failures++;
	return TRUE;
}
static void main_menu_unload(void) {}
static boolean game_in_progress(void) { return old_active; }
static boolean game_map_loaded(void) { return old_loaded; }
static void game_dispose_from_old_map(void) { disposed++; old_active = FALSE; }
static void game_unload(void) { unloaded++; old_loaded = FALSE; }
static void game_set_game_variant(struct game_variant *variant)
{
	CHECK(!old_active && !old_loaded, "old map still owns variant");
	current_variant = *variant;
	variant_calls++;
}
static void game_set_game_variant_options(struct game_variant_options const *options)
{
	current_options = *options;
	option_calls++;
}
static boolean game_load(struct game_options *options)
{
	CHECK(current_variant.game_engine_index == game.variant.game_engine_index,
		"stale engine %ld instead of authoritative engine %ld", current_variant.game_engine_index,
		game.variant.game_engine_index);
	CHECK(current_variant.rules == game.variant.rules && current_options.rules == game.variant_options.rules,
		"variant options were not replaced together");
	CHECK(options->difficulty == game.difficulty && !strcmp(options->map_name, game.map.name), "wrong options");
	CHECK(options->random_seed == (connection == _game_connection_film_playback ? game.random_seed : 1234),
		"wrong random seed");
	return load_ok;
}
static void game_initialize_for_new_map(void) { initialized++; }
static boolean network_player_is_valid(struct network_player *player) { return player->valid; }
static boolean network_game_spawn_player(struct network_player *player)
{
	CHECK(player->player_list_index == 0 || player->player_list_index == 2, "player slots changed");
	spawns++;
	return spawn_ok;
}
static void error(int level, char const *text) { (void)level; (void)text; }

#include "under_test.inc"

int main(int argc, char **argv)
{
	char const *name = argc > 1 ? argv[1] : "";
	boolean result;

	strcpy(game.map.name, "levels\\a10\\a10");
	game.difficulty = 2;
	game.random_seed = 7654;
	game.variant.game_engine_index = 0;
	game.variant.rules = 12;
	game.variant_options.rules = 34;
	game.players[0].valid = game.players[2].valid = TRUE;
	current_variant.game_engine_index = 2;
	current_variant.rules = 99;
	current_options.rules = 99;
	if (!strcmp(name, "coop-after-multiplayer")) current_variant.game_engine_index = 5;
	else if (!strcmp(name, "multiplayer-after-coop"))
	{
		current_variant.game_engine_index = 0;
		game.variant.game_engine_index = 2;
	}
	else if (!strcmp(name, "failed-precache")) precache_ok = FALSE;
	else if (!strcmp(name, "failed-load")) load_ok = FALSE;
	else if (!strcmp(name, "failed-player")) spawn_ok = FALSE;
	else if (!strcmp(name, "film")) connection = _game_connection_film_playback;
	else CHECK(!strcmp(name, "coop-after-slayer"), "unknown case %s", name);
	result = network_game_create_game_objects(&game);
	if (!precache_ok)
	{
		CHECK(!result && !disposed && !unloaded && !variant_calls && !option_calls && failures == 1,
			"failed precache changed the current map or variant");
		return 0;
	}
	CHECK(disposed == 1 && unloaded == 1 && variant_calls == 1 && option_calls == 1, "incorrect map handoff");
	CHECK(result == (load_ok && spawn_ok) && game.local_data.game_objects_loaded == result, "wrong load result");
	CHECK(initialized == (load_ok ? 1 : 0), "initialized an unloaded map");
	CHECK(spawns == (load_ok ? (spawn_ok ? 2 : 1) : 0), "incorrect spawn processing");
	return 0;
}
