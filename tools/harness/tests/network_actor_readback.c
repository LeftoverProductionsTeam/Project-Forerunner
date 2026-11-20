/* Production diagnostic code over a bounded world: no simulated network pass. */
#include "harness.h"
#include <stdarg.h>
#include <math.h>
#define MAXIMUM_NETWORK_ACTORS 4
#define TICKS_PER_SECOND 30
#define _game_connection_local 0
#define _game_connection_network_server 1
#define _game_connection_network_client 2
#define _object_mask_unit 1
#define _object_mask_biped 1
#define _object_dead_bit 0
#define _object_on_ground_bit 1
#define _object_at_rest_bit 2
#define _unit_control_jump_bit 0
#define _vehicle_type_human_plane 1
#define vehicle_datum unit_datum
#define MAX(a,b) ((a)>(b)?(a):(b))
#define MIN(a,b) ((a)<(b)?(a):(b))
#define PIN(a,b,c) ((a)<(b)?(b):((a)>(c)?(c):(a)))
#define csmemcpy memcpy
struct unit_datum
{
	long definition_index;
	struct {real_point3d position;real body_vitality,shield_vitality;unsigned long damage_flags,flags;long parent_object_index;real_vector3d forward,translational_velocity;} object;
	struct {long player_index,actor_index,driver_object_index;short parent_seat_index,current_weapon_index;long weapon_object_indices[4];
		unsigned long control_flags;real_vector3d throttle;real seat_power[2];} unit;
	struct {real speed,slide,turn;} vehicle;
};
struct obey_individual_complex_control {
 boolean destination_valid,destination_keep_moving;
 real_point3d destination_point;
};
static int actor_data=1;
static struct actor_datum {
 struct {long unit_index;short active,dormant,squad_index;boolean frozen;} meta;
 struct {short action,default_state;
  struct {struct {struct obey_individual_complex_control complex_control;} obey;} action_data;
 } state;
 struct {boolean moving;real_point3d moving_towards_point;
  struct {boolean at_destination;real destination_distance;struct {boolean valid;short step_index,step_count;} path;} path;} control;
 struct {struct {struct {short destination_type;} destination;} move;} orders;
 struct {short vehicle_driver_type;long vehicle_index;real_vector3d facing_vector;
  struct {real_point3d body_position,head_position;real_vector3d body_facing,velocity;} position;} input;
 struct {real_vector3d throttle,facing_vector;} output;
} native_actor;
static struct actor_datum *actor_try_and_get(long index) { return index==0x56780000 ? &native_actor : NULL; }
static struct unit_datum world[8];
static long world_ids[8];
static struct {long unit_index;} host_actors[MAXIMUM_NETWORK_ACTORS],client_actors[MAXIMUM_NETWORK_ACTORS];
static short host_actor_count,client_actor_count,logged_actor_count;
static long logged_actor_units[MAXIMUM_NETWORK_ACTORS];
static struct {real actors_interval;long actors_logged_time;} network_test;
static int connection=1,cinematic,bsp=2;
static long now=30;
static char output[8192];
static size_t used;
static int summaries;
/* The existing shared diagnostic also observes terminals/player actions.
   This actor-only world has none; keep those iteration boundaries explicit. */
#define _object_mask_control 2
struct object_iterator {long index, next;};
struct data_iterator {long datum_index;};
struct player_datum {short local_player_index,action_result;long action_object_index;};
struct object_datum {struct {short name_index;real_point3d bounding_sphere_center;real bounding_sphere_radius;} object;};
static int player_data;
static long local_world_unit=NONE;
static void object_iterator_new(struct object_iterator *i,int mask,int flags)
{
	(void)flags;
	i->index=NONE;
	i->next=mask==_object_mask_biped ? local_world_unit : NONE;
}
static int object_iterator_next(struct object_iterator *i)
{
	if (i->next==NONE)
		return FALSE;
	i->index=i->next;
	i->next=NONE;
	return TRUE;
}
static struct object_datum *object_get(long index) {(void)index;CHECK(FALSE,"no terminals in actor world");return NULL;}
static int device_can_change_position(long index) {(void)index;return FALSE;}
static void data_iterator_new(struct data_iterator *i,int data) {(void)i;(void)data;}
static void *data_iterator_next(struct data_iterator *i) {(void)i;return NULL;}
static int game_connection(void){return connection;}
static long game_time_get(void){return now;}
static int cinematic_in_progress(void){return cinematic;}
static short global_structure_bsp_index_get(void){return (short)bsp;}
static void *object_try_and_get_and_verify_type(long id,int mask)
{
	int i;(void)mask;
	for(i=0;i<8;i++)if(world_ids[i]==id&&id!=NONE)return &world[i];
	return NULL;
}
static struct unit_datum *unit_get(long id)
{
	struct unit_datum *u=object_try_and_get_and_verify_type(id,1);
	CHECK(u!=NULL,"stale or reused network handle read");return u;
}
static void platform_log(const char *format,...)
{
	va_list args;int written;
	if(strstr(format,"network test: actors "))summaries++;
	va_start(args,format);written=vsnprintf(output+used,sizeof(output)-used,format,args);va_end(args);
	CHECK(written>0&&(size_t)written+used+2<sizeof(output),"diagnostic output overflow");
	used+=(size_t)written;output[used++]='\n';output[used]=0;
}
/* Native transport sampling over the same bounded actor/world adapters. */
struct vehicle_definition {real ai_steering_max_throttle,ai_avoidance_distance,ai_pathfinding_radius;unsigned long flags;short vehicle_type;};
static struct vehicle_definition plane_tag={.65f,4.f,1.8f,0,_vehicle_type_human_plane};
static boolean human_plane=TRUE;
static struct actor_datum *actor_get(long index) { struct actor_datum *a=actor_try_and_get(index);CHECK(a,"unknown actor");return a; }
static struct unit_datum *vehicle_get(long id) {return unit_get(id);}
static struct unit_datum *unit_try_and_get(long id) {return object_try_and_get_and_verify_type(id,_object_mask_unit);}
static boolean vehicle_is_landed(long id) {return TEST_FLAG(vehicle_get(id)->object.flags,_object_on_ground_bit);}
static boolean vehicle_is_human_plane(long id) {(void)id;return human_plane;}
static struct vehicle_definition *vehicle_specific_definition_get(long index) {(void)index;return &plane_tag;}
static void object_get_origin(long id,real_point3d *p) {*p=unit_get(id)->object.position;}
static void object_get_orientation(long id,real_vector3d *f,real_vector3d *u) {(void)u;*f=unit_get(id)->object.forward;}
static void object_get_velocities(long id,real_vector3d *v,real_vector3d *a) {(void)a;*v=unit_get(id)->object.translational_velocity;}
static real actor_destination_tolerance(long index) {(void)index;return 1.8f;}
static real distance_squared3d(real_point3d const *a,real_point3d const *b)
{
	real x=a->x-b->x,y=a->y-b->y,z=a->z-b->z;
	return x*x+y*y+z*z;
}
static real normalize3d(real_vector3d *vector)
{
	real length = sqrtf(vector->i * vector->i + vector->j * vector->j + vector->k * vector->k);
	if (length > 0.f)
	{
		vector->i /= length;
		vector->j /= length;
		vector->k /= length;
	}
	return length;
}
#include "under_test.inc"
static long add(int slot)
{
	long id=0x12340000L+slot;
	world_ids[slot]=id;world[slot].definition_index=0x43210000L+slot;
	world[slot].object.position=(real_point3d){1.25f+slot,2.5f,3.75f};
	world[slot].object.body_vitality=.625f;world[slot].object.shield_vitality=.25f;
	world[slot].object.parent_object_index=NONE;world[slot].unit.parent_seat_index=NONE;
	world[slot].unit.player_index=world[slot].unit.actor_index=NONE;return id;
}
static void clear_output(void){used=0;output[0]=0;summaries=0;}
static void expect(const char *text){CHECK(strstr(output,text)!=NULL,"missing '%s' in %s",text,output);}
int main(int argc,char **argv)
{
	const char *case_name;int i;
	CHECK(argc==2,"case required");case_name=argv[1];
	for(i=0;i<8;i++)world_ids[i]=NONE;
	CASE("plane-waypoint-turn")
	{
		CHECK(actor_move_plane_approach_scale(0.9f, -0.2f, TRUE) == 0.15f,
			"continuing waypoint kept cruise thrust while turning back");
		CHECK(actor_move_plane_approach_scale(0.9f, 0.5f, TRUE) == 0.25f,
			"turn thrust did not follow heading");
		CHECK(actor_move_plane_approach_scale(0.2f, 1.f, TRUE) == 0.5f,
			"aligned climb lost the cruise floor");
		CHECK(actor_move_plane_approach_scale(0.1f, 1.f, FALSE) == 0.15f &&
			actor_move_plane_approach_scale(2.f, 1.f, TRUE) == 1.f,
			"landing or full cruise changed");
	}
	else CASE("plane-parked-look")
	{
		real_vector3d facing = {-.947f, -.306f, -.102f};
		native_actor.state.action = _actor_action_alert;
		native_actor.input.vehicle_index = add(1);
		native_actor.input.vehicle_driver_type = _actor_vehicle_driver_directional_flying;
		native_actor.input.position.body_facing = (real_vector3d){.971f, -.19f, .143f};
		CHECK(!actor_move_plane_parked_facing(&native_actor, &facing) && facing.i < 0.f,
			"airborne pilot lost native steering");
		world[1].object.flags = FLAG(_object_on_ground_bit);
		CHECK(actor_move_plane_parked_facing(&native_actor, &facing) && facing.i > .98f && facing.k == 0.f,
			"look override steered the grounded hull");
		facing.i = -.947f;
		native_actor.control.path.path.valid = TRUE;
		CHECK(!actor_move_plane_parked_facing(&native_actor, &facing) && facing.i < 0.f,
			"active native route lost its heading");
		native_actor.control.path.path.valid = FALSE;
		native_actor.state.action = _actor_action_obey;
		CHECK(!actor_move_plane_parked_facing(&native_actor, &facing) && facing.i < 0.f,
			"authored stop lost its facing order");
		native_actor.state.action = _actor_action_alert;
		human_plane = FALSE;
		CHECK(!actor_move_plane_parked_facing(&native_actor, &facing) && facing.i < 0.f,
			"ordinary vehicle lost native steering");
	}
	else CASE("plane-parked-rest")
	{
		long ship_index = add(1), pilot_index = add(2);
		struct unit_datum *ship = vehicle_get(ship_index), *pilot = unit_get(pilot_index);
		ship->object.flags = FLAG(_object_at_rest_bit) | FLAG(_object_on_ground_bit);
		ship->unit.driver_object_index = pilot_index;
		ship->unit.seat_power[0] = 1.f;
		ship->vehicle.turn = .012f;
		plane_tag.flags = FLAG(1) | FLAG(2);
		pilot->object.parent_object_index = ship_index;
		pilot->unit.actor_index = 0x56780000;
		native_actor.input.vehicle_index = ship_index;
		native_actor.input.vehicle_driver_type = _actor_vehicle_driver_directional_flying;
		native_actor.state.action = _actor_action_alert;
		CHECK(!vehicle_controls_wake_physics(ship_index, ship, &plane_tag),
			"a powered idle pilot woke an already settled plane");
		native_actor.meta.frozen = TRUE;
		native_actor.state.action = _actor_action_none;
		CHECK(!vehicle_controls_wake_physics(ship_index, ship, &plane_tag),
			"suspending a seated pilot woke an already settled plane");
		native_actor.input.vehicle_index = NONE;
		native_actor.input.vehicle_driver_type = _actor_vehicle_driver_none;
		ship->object.flags = 0;
		CHECK(vehicle_plane_pilot_power(ship_index, ship) == 0.f, "initially suspended pilot applied lift or steering");
		ship->object.flags = FLAG(_object_at_rest_bit) | FLAG(_object_on_ground_bit);
		CHECK(!vehicle_controls_wake_physics(ship_index, ship, &plane_tag),
			"seating an already suspended pilot woke an already settled plane");
		native_actor.input.vehicle_index = ship_index;
		native_actor.input.vehicle_driver_type = _actor_vehicle_driver_directional_flying;
		native_actor.meta.frozen = FALSE;
		native_actor.state.action = _actor_action_alert;
		CHECK(vehicle_plane_pilot_power(ship_index, ship) == 1.f, "resumed pilot lost native power");
		native_actor.control.path.path.valid = TRUE;
		CHECK(vehicle_controls_wake_physics(ship_index, ship, &plane_tag), "new route failed to wake the plane");
		native_actor.control.path.path.valid = FALSE;
		native_actor.state.action = _actor_action_obey;
		CHECK(vehicle_controls_wake_physics(ship_index, ship, &plane_tag), "a scripted facing order was held asleep");
		native_actor.state.action = _actor_action_alert;
		ship->unit.throttle.i = .5f;
		CHECK(vehicle_controls_wake_physics(ship_index, ship, &plane_tag), "native throttle failed to wake the plane");
		ship->unit.throttle.i = 0.f;
		ship->object.flags &= ~FLAG(_object_at_rest_bit);
		CHECK(vehicle_controls_wake_physics(ship_index, ship, &plane_tag), "collision wake was suppressed");
		ship->object.flags |= FLAG(_object_at_rest_bit);
		ship->unit.control_flags = FLAG(_unit_control_jump_bit);
		CHECK(vehicle_controls_wake_physics(ship_index, ship, &plane_tag), "lift control failed to wake the plane");
		ship->unit.control_flags = 0;
		pilot->unit.player_index = 1;
		CHECK(vehicle_controls_wake_physics(ship_index, ship, &plane_tag), "player pilot acquired AI parking");
	}
	else CASE("plane-landing-arrival")
	{
		long actor=0x56780000;
		native_actor.input.vehicle_index=add(1);
		native_actor.input.vehicle_driver_type=_actor_vehicle_driver_directional_flying;
		native_actor.state.action=_actor_action_obey;
		native_actor.state.action_data.obey.complex_control.destination_valid=TRUE;
		native_actor.state.action_data.obey.complex_control.destination_point=(real_point3d){79,-124,1.85f};
		native_actor.input.position.body_position=(real_point3d){77.023f,-115.954f,1.856f};
		native_actor.control.path.path.valid=FALSE;
		native_actor.control.path.at_destination=TRUE;
		CHECK(!actor_path_at_destination(actor),"cleared path completed the landing order outside its native radius");
		native_actor.input.position.body_position=(real_point3d){78.747f,-125.707f,1.992f};
		CHECK(actor_path_at_destination(actor),"authored physical arrival did not complete");
		native_actor.control.path.path.valid=TRUE;
		native_actor.control.path.at_destination=FALSE;
		CHECK(!actor_path_at_destination(actor),"physical range completed an active native path");
		native_actor.control.path.path.valid=FALSE;
		native_actor.input.position.body_position=(real_point3d){77.023f,-115.954f,1.856f};
		native_actor.state.action_data.obey.complex_control.destination_keep_moving=TRUE;
		CHECK(actor_path_at_destination(actor),"a keep-moving waypoint changed semantics");
		native_actor.state.action_data.obey.complex_control.destination_keep_moving=FALSE;
		human_plane=FALSE;
		CHECK(actor_path_at_destination(actor),"legacy aircraft arrival changed");
	}
	else CASE("plane-waypoint-position")
	{
		long ship=add(1);
		native_actor.input.vehicle_index=ship;
		native_actor.input.vehicle_driver_type=_actor_vehicle_driver_directional_flying;
		native_actor.input.position.body_position=(real_point3d){99,98,97};
		native_actor.input.position.head_position=(real_point3d){9,8,7};
		world[1].object.forward=(real_vector3d){-1,0,0};
		world[1].object.translational_velocity=(real_vector3d){.1f,.2f,.3f};
		actor_move_sample_plane_position(0x56780000);
		CHECK(native_actor.input.position.body_position.x==world[1].object.position.x &&
		      native_actor.input.position.body_position.z==world[1].object.position.z,"transport navigated from pilot seat");
		CHECK(native_actor.input.position.body_facing.i==-1 &&
		      native_actor.input.position.velocity.j==.2f,"transport kept seat motion/facing");
		CHECK(native_actor.input.position.head_position.x==9 &&
		      native_actor.input.position.head_position.z==7,"pilot sight was replaced by aircraft position");
		world[1].object.position=(real_point3d){50,-110,3};
		world[1].object.forward=(real_vector3d){0,1,0};
		actor_move_sample_plane_position(0x56780000);
		CHECK(native_actor.input.position.body_position.y==-110 &&
		      native_actor.input.position.body_facing.j==1,"turn changed waypoint reference to the offset seat");
		native_actor.input.position.body_position.x=99;human_plane=FALSE;
		actor_move_sample_plane_position(0x56780000);
		CHECK(native_actor.input.position.body_position.x==99,"legacy fighter sampling changed");
		human_plane=TRUE;plane_tag.ai_avoidance_distance=0;
		actor_move_sample_plane_position(0x56780000);
		CHECK(native_actor.input.position.body_position.x==99,"unconfigured plane sampling changed");
		plane_tag.ai_avoidance_distance=4;native_actor.input.vehicle_driver_type=_actor_vehicle_driver_none;
		actor_move_sample_plane_position(0x56780000);
		CHECK(native_actor.input.position.body_position.x==99,"passenger/on-foot sampling changed");
	}
	else CASE("host-control")
	{
		host_actors[0].unit_index=add(0);host_actor_count=1;world[0].unit.actor_index=0x56780000;
		native_actor.meta.unit_index=world_ids[0];native_actor.meta.active=1;native_actor.meta.squad_index=2;
		native_actor.state.action=3;native_actor.state.default_state=9;native_actor.control.path.path.valid=1;
		native_actor.control.path.path.step_count=2;native_actor.control.path.destination_distance=6;
		native_actor.orders.move.destination.destination_type=1;native_actor.control.moving_towards_point.x=12;
		native_actor.input.vehicle_driver_type=4;native_actor.output.throttle.i=.5f;native_actor.output.facing_vector.k=-.25f;
		network_actors_log_units();expect("ai control 30 12340000 active 1 dormant 0 action 3 default 9 squad 2 moving 0 destination 1 arrived 0 path 1/0/2 distance 6.000 target 12.000");
		expect("driver 4 throttle 0.500 0.000 0.000 facing 0.000 0.000 -0.250");
		clear_output();native_actor.meta.unit_index+=0x10000;network_actors_log_units();
		CHECK(!strstr(output,"ai control"),"reused unit ownership emitted stale AI state");
		clear_output();native_actor.meta.unit_index=world_ids[0];world[0].unit.actor_index+=0x10000;network_actors_log_units();
		CHECK(!strstr(output,"ai control"),"reused actor salt emitted stale AI state");
		clear_output();world[0].unit.actor_index=0x56780000;connection=2;network_actors_log_units();
		CHECK(!strstr(output,"ai control"),"client interpreted a host actor index locally");
	}
	else CASE("host-world")
	{
		host_actors[0].unit_index=add(0);host_actor_count=1;cinematic=1;
		world[0].object.parent_object_index=0x98760001L;world[0].unit.parent_seat_index=2;
		network_actors_log_units();
		expect("actor 30 host 12340000 43210000 1.250000 2.500000 3.750000 0.625000 0.250000 0 98760001 2 1");
		expect("actors 30 host 1 cinematic 1 bsp 2");
		clear_output();world[0].object.position.x=9.5f;network_actors_log_units();expect("9.500000 2.500000");
	}
	else CASE("client-world")
	{
		connection=2;client_actors[0].unit_index=add(1);client_actor_count=1;
		network_actors_log_units();expect("actor 30 client 12340001 43210001");expect("actors 30 client 1");
	}
	else CASE("remember-death")
	{
		host_actors[0].unit_index=add(0);host_actor_count=1;network_actors_log_units();
		host_actor_count=0;world[0].object.damage_flags=FLAG(_object_dead_bit);world[0].object.body_vitality=0;
		clear_output();now=60;network_actors_log_units();
		expect("actor 60 host 12340000 43210000 1.250000 2.500000 3.750000 0.000000 0.250000 1 ffffffff -1 0");
	}
	else CASE("deleted-reused")
	{
		host_actors[0].unit_index=add(0);host_actor_count=1;network_actors_log_units();host_actor_count=0;
		world_ids[0]=0x56780000L;clear_output();network_actors_log_units();
		CHECK(strstr(output,"network test: actor ")==NULL,"deleted/reused handle reported");expect("actors 30 host 0");
	}
	else CASE("players-duplicates")
	{
		host_actors[0].unit_index=add(0);host_actors[1].unit_index=add(1);world[1].unit.player_index=2;
		host_actors[2].unit_index=host_actors[0].unit_index;host_actor_count=3;
		network_actors_log_units();expect("actors 30 host 1");CHECK(logged_actor_count==1,"player/duplicate retained");
	}
	else CASE("capacity-priority")
	{
		for(i=0;i<4;i++)host_actors[i].unit_index=add(i);host_actor_count=4;network_actors_log_units();
		for(i=0;i<4;i++)host_actors[i].unit_index=add(i+4);clear_output();network_actors_log_units();
		expect("actors 30 host 4");expect("actor 30 host 12340007");
		CHECK(strstr(output,"actor 30 host 12340000")==NULL,"corpse crowded out current control entry");
	}
	else CASE("local-reset")
	{
		host_actors[0].unit_index=add(0);host_actor_count=1;network_actors_log_units();
		network_actors_new_game();CHECK(!logged_actor_count&&!host_actor_count&&!client_actor_count,"map reset kept units");
		clear_output();connection=_game_connection_local;local_world_unit=world_ids[0];network_actors_log_units();
		expect("actor 30 local 12340000 43210000 1.250000 2.500000 3.750000 0.625000 0.250000 0 ffffffff -1 0");
		expect("actors 30 local 1");
	}
	else CASE("disabled")
	{
		network_test.actors_logged_time=NONE;network_test.actors_interval=0;network_test_log_actors();
		CHECK(!used,"default setting emitted evidence");network_test.actors_interval=-1;network_test_log_actors();CHECK(!used,"negative enabled");
	}
	else CASE("cadence-rewind")
	{
		network_test.actors_logged_time=NONE;network_test.actors_interval=.5f;network_test_log_actors();CHECK(summaries==1,"no first snapshot");
		now=44;network_test_log_actors();CHECK(summaries==1,"interval ignored");now=45;network_test_log_actors();CHECK(summaries==2,"interval missed");
		now=5;network_test_log_actors();CHECK(summaries==3,"checkpoint rewind suppressed snapshot");
	}
	else CASE("interval-bounds")
	{
		network_test.actors_logged_time=NONE;network_test.actors_interval=.00001f;network_test_log_actors();network_test_log_actors();CHECK(summaries==1,"multiple snapshots in one tick");
		now++;network_test_log_actors();CHECK(summaries==2,"subtick interval missed next tick");
		network_test.actors_interval=1e30f;now+=108000;network_test_log_actors();CHECK(summaries==3,"huge interval overflowed or exceeded bound");
	}
	else CASE("cadence-delayed")
	{
		network_test.actors_logged_time=NONE;network_test.actors_interval=1;network_test_log_actors();
		now=64;network_test_log_actors();CHECK(summaries==2,"delayed snapshot missing");expect("actors 64 host");
		now=90;network_test_log_actors();CHECK(summaries==3,"delayed frame shifted future sample boundaries");expect("actors 90 host");
		now=95;network_test_log_actors();CHECK(summaries==3,"multiple samples in one interval");
		now=120;network_test_log_actors();CHECK(summaries==4,"cadence did not recover");
	}
	else CHECK(FALSE,"unknown case");
	return 0;
}
