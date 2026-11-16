/* Production diagnostic code over a bounded world: no simulated network pass. */
#include "harness.h"
#include <stdarg.h>
#define MAXIMUM_NETWORK_ACTORS 4
#define TICKS_PER_SECOND 30
#define _game_connection_network_server 1
#define _game_connection_network_client 2
#define _object_mask_unit 1
#define _object_mask_biped 1
#define _object_dead_bit 0
#define MAX(a,b) ((a)>(b)?(a):(b))
#define PIN(a,b,c) ((a)<(b)?(b):((a)>(c)?(c):(a)))
#define csmemcpy memcpy
struct unit_datum
{
	long definition_index;
	struct {real_point3d position;real body_vitality,shield_vitality;unsigned long damage_flags;long parent_object_index;} object;
	struct {long player_index,actor_index;short parent_seat_index,current_weapon_index;long weapon_object_indices[4];} unit;
};
static int actor_data=1;
static struct actor_datum {
 struct {long unit_index;short active,dormant,squad_index;} meta;
 struct {short action,default_state;} state;
 struct {boolean moving;real_point3d moving_towards_point;
  struct {boolean at_destination;real destination_distance;struct {boolean valid;short step_index,step_count;} path;} path;} control;
 struct {struct {struct {short destination_type;} destination;} move;} orders;
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
struct object_iterator {long index;};
struct data_iterator {long datum_index;};
struct player_datum {short local_player_index,action_result;long action_object_index;};
struct object_datum {struct {short name_index;real_point3d bounding_sphere_center;real bounding_sphere_radius;} object;};
static int player_data;
static void object_iterator_new(struct object_iterator *i,int mask,int flags) {(void)i;(void)mask;(void)flags;}
static int object_iterator_next(struct object_iterator *i) {(void)i;return FALSE;}
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
	CASE("host-control")
	{
		host_actors[0].unit_index=add(0);host_actor_count=1;world[0].unit.actor_index=0x56780000;
		native_actor.meta.unit_index=world_ids[0];native_actor.meta.active=1;native_actor.meta.squad_index=2;
		native_actor.state.action=3;native_actor.state.default_state=9;native_actor.control.path.path.valid=1;
		native_actor.control.path.path.step_count=2;native_actor.control.path.destination_distance=6;
		native_actor.orders.move.destination.destination_type=1;native_actor.control.moving_towards_point.x=12;
		network_actors_log_units();expect("ai control 30 12340000 active 1 dormant 0 action 3 default 9 squad 2 moving 0 destination 1 arrived 0 path 1/0/2 distance 6.000 target 12.000");
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
		clear_output();connection=0;network_actors_log_units();CHECK(!used,"local play emitted network evidence");
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
