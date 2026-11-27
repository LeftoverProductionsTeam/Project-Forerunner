/* A sound's wall test is made once a tick (sound_manager.c's source_audible
and its obstruction cache): the tick's other frames reuse it even though a
sound on a moving object is at a new interpolated position each frame; the
next tick, another listener or another sound in the same slot tests again;
and a sound past its range is never tested. */
#include "harness.h"
#include "types.inc"

#define REAL_MAX 3.4028235e38f
static struct sound_listener listeners[MAXIMUM_NUMBER_OF_LOCAL_PLAYERS];
static long now;
static short global_structure_bsp_index;
static boolean tracked_listener;
static boolean listener_uses_tracking(short index) { (void)index; return tracked_listener; }
static int tests;
static long game_time_get(void) { return now; }
static struct sound_listener *listener_get(short index) { return &listeners[index]; }
static real square_root(real x) { return x; }
/* the distance is the source's x, so a source that moves stays in range */
static real source_distance_squared(short listener_index, struct sound_source const *source)
{
	(void)listener_index;
	return source->location.position.x * source->location.position.x;
}
static void compute_sound_obstruction(short listener_index, struct sound_source *source, real distance)
{
	(void)listener_index; (void)distance;
	tests++;
	source->obstruction = 0.25f * (real)tests;
	source->occlusion = 0.f;
}
/* Three native scene slots: foreground, render preview, and simulation hold. */
#define MAXIMUM_STRUCTURE_BSPS_PER_SCENARIO 3
#define MIN(a,b) ((a) < (b) ? (a) : (b))
struct structure_bsp { real min_x, max_x; };
struct scenario { struct { long count; } structure_bsp_references; };
static struct scenario camera_scenario = {{3}};
static struct structure_bsp camera_regions[] = {{0.f, 1.f}, {1.f, 2.f}, {2.f, 3.f}};
static short followed_bsp = NONE;
static boolean regional_scene = TRUE;
static struct scenario const *global_scenario_try_and_get(void) { return &camera_scenario; }
static boolean scenario_uses_regional_gameplay(struct scenario const *scenario) { return scenario && regional_scene; }
static short global_structure_bsp_index_get(void) { return global_structure_bsp_index; }
static struct structure_bsp const *global_structure_bsp_get(void) { return &camera_regions[0]; }
static long director_get_followed_unit(short player) { (void)player; return followed_bsp == NONE ? NONE : 1; }
static boolean object_try_and_get(long unit) { return unit != NONE; }
static short object_get_regional_bsp(long unit) { (void)unit; return followed_bsp; }
static struct structure_bsp const *scenario_structure_bsp_preview(short bsp, boolean prefetch)
{
	CHECK(!prefetch, "a camera query loaded a BSP");
	return &camera_regions[bsp];
}
static struct structure_bsp const *scenario_structure_bsp_try_and_get(short bsp)
{
	return bsp == 1 ? NULL : &camera_regions[bsp];
}
static boolean scenario_structure_bsp_contains_point(struct structure_bsp const *structure, real_point3d const *point)
{
	return structure && point->x >= structure->min_x && point->x <= structure->max_x;
}
#include "under_test.inc"

static struct sound_source at(real x)
{
	struct sound_source source;

	memset(&source, 0, sizeof(source));
	source.spatialization_mode = _sound_spatialization_mode_absolute;
	source.location.position.x = x;
	return source;
}

int main(int argc, char **argv)
{
	char const *case_name = argc > 1 ? argv[1] : "";
	struct sound_source source;
	long sound = (0x8001L << 16) | 5, other = (0x8002L << 16) | 5;
	int frame;

	listeners[0].valid = TRUE;
	now = 10;
	CASE("detached-listener")
	{
		real_point3d position = {.x = 2.5f};
		CHECK(observer_get_camera_bsp(0, &position, FALSE) == 2, "detached audio stayed in foreground");
		CHECK(observer_get_camera_bsp(0, &position, TRUE) == 2, "detached rendering disagrees with loaded audio scene");
		position.x = 1.5f;
		CHECK(observer_get_camera_bsp(0, &position, FALSE) == 0, "audio used a render preview");
		CHECK(observer_get_camera_bsp(0, &position, TRUE) == 1, "detached rendering lost the preview");
		regional_scene = FALSE;
		CHECK(observer_get_camera_bsp(0, &position, TRUE) == 0, "classic camera lost foreground");
	}
	CASE("unloaded-followed-unit")
	{
		real_point3d position = {.x = 2.5f};
		followed_bsp = 1;
		CHECK(observer_get_camera_bsp(0, &position, FALSE) == 1, "unloaded follower adopted another BSP's audio");
		followed_bsp = 0;
		CHECK(observer_get_camera_bsp(0, &position, FALSE) == 0, "foreground follower lost its own BSP");
	}
	CASE("moving")
	{
		/* four frames of a tick, the source's interpolated position moving */
		for (frame = 0; frame < 4; frame++)
		{
			source = at(2.f + 0.1f * (real)frame);
			CHECK(source_audible(&source, 10.f, _sound_obstruction_cache_sound, sound) == 0, "the sound is unheard");
			CHECK(source.obstruction == 0.25f, "frame %d has obstruction %f", frame, source.obstruction);
		}
		CHECK(tests == 1, "a tick of a moving sound made %d wall tests", tests);
	}
	CASE("next-tick")
	{
		source = at(2.f);
		source_audible(&source, 10.f, _sound_obstruction_cache_looping_sound, sound);
		now++;
		source_audible(&source, 10.f, _sound_obstruction_cache_looping_sound, sound);
		CHECK(tests == 2, "two ticks made %d wall tests", tests);
	}
	CASE("listener")
	{
		source = at(2.f);
		source_audible(&source, 10.f, _sound_obstruction_cache_sound, sound);
		listeners[0].valid = FALSE;
		listeners[1].valid = TRUE;
		source_audible(&source, 10.f, _sound_obstruction_cache_sound, sound);
		CHECK(tests == 2, "another listener made %d wall tests", tests);
	}
	CASE("region")
	{
		source = at(2.f);
		source_audible(&source, 10.f, _sound_obstruction_cache_sound, sound);
		global_structure_bsp_index++;
		source_audible(&source, 10.f, _sound_obstruction_cache_sound, sound);
		CHECK(tests == 2, "a new BSP reused the old obstruction");
	}
	CASE("regional-listeners")
	{
		/* Both listeners are equally close, but only the second shares the BSP. */
		listeners[0].bsp_plus_one = 1;
		listeners[1].valid = TRUE;
		listeners[1].bsp_plus_one = 9;
		source = at(2.f);
		source.bsp_plus_one = 9;
		CHECK(source_audible(&source, 10.f, _sound_obstruction_cache_sound, sound) == 1,
			"the foreground listener intercepted a remote sound");
		listeners[1].valid = FALSE;
		CHECK(source_audible(&source, 10.f, _sound_obstruction_cache_sound, sound) == NONE,
			"a positional sound leaked between BSPs");
		CHECK(tests == 1, "a BSP-mismatched source ran an obstruction test");
	}
	CASE("regional-source-move")
	{
		/* The sound datum and listener index survive a transfer in the same tick. */
		source = at(2.f);
		listeners[0].bsp_plus_one = source.bsp_plus_one = 9;
		CHECK(source_audible(&source, 10.f, _sound_obstruction_cache_sound, sound) == 0,
			"the first regional sound is unheard");
		listeners[0].bsp_plus_one = source.bsp_plus_one = 12;
		CHECK(source_audible(&source, 10.f, _sound_obstruction_cache_sound, sound) == 0,
			"the transferred regional sound is unheard");
		CHECK(tests == 2, "a moved source reused the former BSP obstruction");
	}
	CASE("tracked-origin")
	{
		source = at(2.f);
		source_audible(&source, 10.f, _sound_obstruction_cache_sound, sound);
		tracked_listener = TRUE;
		source_audible(&source, 10.f, _sound_obstruction_cache_sound, sound);
		CHECK(tests == 2, "the tracked listener reused the old camera obstruction");
	}
	CASE("slot")
	{
		/* another sound in the same slot is not given the first one's result */
		source = at(2.f);
		source_audible(&source, 10.f, _sound_obstruction_cache_sound, sound);
		source_audible(&source, 10.f, _sound_obstruction_cache_sound, other);
		CHECK(tests == 2, "another sound in the slot made %d wall tests", tests);
	}
	CASE("impulse")
	{
		/* a new impulse has no datum yet: every call tests */
		source = at(2.f);
		source_audible(&source, 10.f, _sound_obstruction_cache_none, NONE);
		source_audible(&source, 10.f, _sound_obstruction_cache_none, NONE);
		CHECK(tests == 2, "two impulses made %d wall tests", tests);
	}
	CASE("out-of-range")
	{
		source = at(20.f);
		CHECK(source_audible(&source, 10.f, _sound_obstruction_cache_sound, sound) == NONE, "the sound is heard");
		CHECK(tests == 0, "a sound past its range made %d wall tests", tests);
	}
	return 0;
}
