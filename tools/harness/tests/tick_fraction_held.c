/* The frames drawn between ticks while the clock waits (source/game/
   game_time.c, #477): a client's clock held for its host (Online Games or
   System Link searching from the main menu) runs no ticks, so every frame
   is drawn on the last tick. Before, the fraction ran on from 0 to 1 over
   and over toward a tick that never came, drawing the last two ticks'
   poses back and forth: the menu background juddered where the Xbox's
   stood still. Once ticks run again, the frames between them blend again. */
#include <math.h>
#include "harness.h"
#define TICKS_PER_SECOND 30
#define MIN(a, b) ((a) < (b) ? (a) : (b))
#define MAX(a, b) ((a) > (b) ? (a) : (b))
#define PIN(x, lo, hi) ((x) < (lo) ? (lo) : (x) > (hi) ? (hi) : (x))
#define valid_real(x) (!isnan(x) && !isinf(x))
#define HALO_PERF_BEGIN(x) ((void)0)
#define HALO_PERF_END(x) ((void)0)
#define HALO_PERF_COUNT(x, n) ((void)0)
#define HALO_CONSOLE_NO_NETWORK_TEST
/* (game.h's connections) */
#include "connections.inc"

struct network_game_client { int unused; };
struct network_game_server { int unused; };

static short connection;
static long distributed_host_time = NONE;
static unsigned long distributed_host_time_received, milliseconds;
static real distributed_own_round_trip;
static unsigned long system_milliseconds(void) { return milliseconds; }
static boolean host_started;
static long ticks_run;
static struct network_game_client client;

static short game_connection(void) { return connection; }
static struct network_game_client *global_network_game_client_get(void) { return &client; }
static struct network_game_server *global_network_game_server_get(void) { return NULL; }
static boolean network_game_client_server_has_started_game(struct network_game_client *c) { (void)c; return host_started; }
static boolean network_game_distributed_client(void) { return connection == _game_connection_network_client; }
static void update_client_local_ticks(long ticks) { (void)ticks; }
static void network_game_server_update_ticks(struct network_game_server *s, short ticks) { (void)s; (void)ticks; }
static long update_client_get_maximum_possible_server_time(void) { return 0x7FFFFFFF; }
static void game_tick(void) { ticks_run++; }
static void render_interpolation_tick(void) {}
static void network_distributed_tick(void) {}
static void render_interpolation_frame_begin(void) {}
static void render_interpolation_frame_end(void) {}
static void game_frame(real seconds) { (void)seconds; }
/* port: chester's MCC game speed (mcc_game_speed.c): off without an MCC map */
static real mcc_game_speed_client(real speed) { return speed; }
static void mcc_game_speed_frame(void) {}
/* (port, this fork: the profiler's sections (profile_sections.h), debug.fixed_step
and the main menu's own clock, none of them in these cases) */
#define profile_scope(variable, statement) { statement }
static boolean halo_fixed_step(void) { return FALSE; }
static boolean menu_scene;
static boolean game_time_menu_scene(void) { return menu_scene; }

#include "clock.inc"
#include "under_test.inc"

static struct game_time_globals_struct globals;

/* a frame of dt seconds, and the fraction it is drawn at */
static real frame(real dt)
{
	game_time_update(dt);
	return game_time_get_tick_fraction();
}

int main(int argc, char **argv)
{
	const char *case_name = argc > 1 ? argv[1] : "";
	int index;
	real fraction, previous;

	game_time_globals = &globals;
	game_time_initialize_for_new_map();
	globals.active = TRUE;
	globals.speed = 1.0f;
	if (!strncmp(case_name,"ahead-",6) || !strncmp(case_name,"clock-",6) || !strcmp(case_name,"behind-catches"))
	{
		connection = _game_connection_network_client; host_started = TRUE;
		milliseconds = distributed_host_time_received = 5000;
		distributed_host_time = 100;
		globals.local_time = globals.server_time = 130;
		if (!strcmp(case_name,"ahead-holds"))
		{
			fraction = frame(1.f/30);
			CHECK(!ticks_run && globals.local_time == 130 && fraction == 1.f,
				"ahead clock advanced or rewound rather than holding the last tick");
		}
		else if (!strcmp(case_name,"ahead-recovers"))
		{
			long previous_time = globals.local_time;
			for (index=0;index<90;index++)
			{
				milliseconds+=34; distributed_host_time++; distributed_host_time_received=milliseconds;
				frame(1.f/30);
				CHECK(globals.local_time>=previous_time && globals.local_time<=MAX(previous_time,distributed_host_time+2),
					"recovery rewound a tick or exceeded the fresh host clock");
				previous_time=globals.local_time;
			}
			CHECK(ticks_run>0 && ticks_run<90 && globals.local_time>=distributed_host_time,
				"ahead client never rejoined normal host pacing");
		}
		else if (!strcmp(case_name,"clock-latency"))
		{
			distributed_own_round_trip=8.f;
			CHECK(distributed_host_tick_limit(30.f)==109,"measured latency allowance lost");
			distributed_own_round_trip=10000.f;
			CHECK(distributed_host_tick_limit(30.f)==115,"latency allowance unbounded");
			distributed_own_round_trip=nanf("");
			CHECK(distributed_host_tick_limit(30.f)==102 && distributed_host_tick_limit(nanf(""))==NONE,
				"invalid timing produced an invalid client clock");
		}
		else if (!strcmp(case_name,"clock-stale"))
		{
			milliseconds+=2001;
			CHECK(distributed_host_tick_limit(30.f)==NONE,"stale host evidence retained a ceiling");
			frame(1.f/30);CHECK(ticks_run>0,"stale clock froze a live client");
		}
		else if (!strcmp(case_name,"clock-wrap"))
		{
			distributed_host_time_received=0xfffffff0UL;milliseconds=0x20;
			CHECK(distributed_host_tick_limit(30.f)==103,"receipt-time wrap broke pacing");
		}
		else if (!strcmp(case_name,"clock-contexts"))
		{
			short contexts[3]={_game_connection_local,_game_connection_network_server,_game_connection_network_client};
			for(index=0;index<3;index++)
			{
				connection=contexts[index];menu_scene=index==2;
				globals.local_time=globals.server_time=130;globals.leftover_dt=0;
				frame(.034f);
				CHECK(globals.local_time==131,"host/local/menu clock was held by client pacing");
			}
		}
		else if (!strcmp(case_name,"behind-catches"))
		{
			globals.local_time=globals.server_time=90;game_time_distributed_target=100;
			frame(1.f/30);CHECK(ticks_run==2,"existing bounded catch-up was lost");
		}
		else CHECK(FALSE,"unknown clock case");
		return 0;
	}
	CASE("local-blends")
	{
		/* (a local game at 120 frames a second: frames between ticks) */
		connection = _game_connection_local;
		frame(1.0f / 30);
		fraction = frame(1.0f / 120);
		CHECK(fraction > 0.1f && fraction < 0.9f, "a frame between ticks drawn at %f", fraction);
		return 0;
	}
	CASE("held-stays")
	{
		/* (searching from the main menu: no host, so no ticks) */
		connection = _game_connection_network_client;
		host_started = FALSE;
		/* (the frames of the first tick's time ease on toward it; from the
		tick that never comes on, they stay there and never step back) */
		previous = 0.0f;
		for (index = 0; index < 600; index++)
		{
			fraction = frame(1.0f / 144);
			CHECK(fraction >= previous, "frame %d stepped back from %f to %f with the clock held", index,
				previous, fraction);
			CHECK(index < 5 || fraction == 1.0f, "frame %d drawn at %f with the clock held", index, fraction);
			previous = fraction;
		}
		CHECK(ticks_run == 0, "%ld ticks ran with the clock held", ticks_run);
		return 0;
	}
	CASE("released-blends")
	{
		connection = _game_connection_network_client;
		host_started = FALSE;
		for (index = 0; index < 100; index++)
			frame(1.0f / 144);
		host_started = TRUE;
		frame(1.0f / 30);
		CHECK(ticks_run > 0, "no tick once the host started");
		fraction = frame(1.0f / 144);
		CHECK(fraction < 1.0f, "a frame between ticks after the hold drawn at %f", fraction);
		return 0;
	}
	CHECK(FALSE, "no such case: %s", case_name);
	return 0;
}
