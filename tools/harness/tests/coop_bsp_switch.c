/* The existing scenario loader calls the real player reconnect/teleport code.
   IO, tag storage, collision lookup and movement are small fixture boundaries;
   this is not native map traversal or a multiplayer session. */
#include "harness.h"
#include <limits.h>

#undef match_assert
#define match_assert(f,l,c) CHECK(c,"engine assertion")
#define match_vassert(f,l,c,m) CHECK(c,"%s",m)
#define _error_silent 0
#define _error_immediate 1
#define error(...) ((void)0)
#define MAXIMUM_STRUCTURE_BSPS_PER_SCENARIO 16
enum {_game_connection_network_server=2};
enum {_collision_test_front_facing_surfaces_bit, _collision_test_ignore_invisible_surfaces_bit,
      _collision_test_structure_bit, _collision_test_objects_scenery_bit};

struct tag_block {long count;void *elements;};
static void *element(struct tag_block *block,long index,size_t size)
{
    CHECK(index>=0&&index<block->count,"invalid tag access %ld/%ld",index,block->count);
    return (byte *)block->elements+index*size;
}
#define TAG_BLOCK_GET_ELEMENT(b,i,t) ((t *)element(b,i,sizeof(t)))
#include "switch_type.inc"
struct scenario_cutscene_flag {real_point3d position;};
struct scenario_structure_bsp_reference {struct {long index;} structure_bsp;};
struct scenario {struct tag_block bsp_switch_trigger_volumes,cutscene_flags,structure_bsp_references;};
struct collision_bsp {int placeholder;};
struct bsp3d {int placeholder;};
struct structure_leaf {long cluster_index;};
struct structure_bsp {struct tag_block leaves,collision_bsp;};
struct data_iterator {long datum_index;int slot;};
struct player_datum {long unit_index,cluster_index;short local_player_index;};
struct object_datum {struct {long parent_object_index;real_point3d position,bounding_sphere_center;} object;
                     long weapon;short ammunition;};
#define biped_datum object_datum

static struct scenario scenario;
static struct scenario *global_scenario=&scenario;
static struct {short structure_bsp_index;} scenario_state;
static __typeof__(scenario_state) *scenario_globals=&scenario_state;
static struct {short pending_teleport_starting_location_index,local_player_count;boolean respawn_failed;} player_state;
static __typeof__(player_state) *players_globals=&player_state;
static struct player_datum people[2];
static struct object_datum objects[4];
static long units[4]={0x10000,0x20001,0x30002,0x40003};
static long players[2]={0x50000,0x60001};
static void *player_data;
static struct scenario_bsp_switch_trigger_volume switches[1];
static struct scenario_cutscene_flag flags[1];
static struct scenario_structure_bsp_reference references[2];
static struct structure_bsp structures[2],*global_structure_bsp;
static struct collision_bsp collisions[2],*global_collision_bsp;
static struct bsp3d *global_bsp3d;
static struct structure_leaf leaves[2];
static short global_structure_bsp_index;
static int coop=TRUE,connection=_game_connection_network_server;
static int reject_load[2],reject_verify[2],loads[2],unloads[2];
static int teleports,ejections,reconnects,disconnects,menu,clock_stops,clock_starts,collision_log=TRUE,culling;

static int slot(long unit)
{
    for(int i=0;i<4;i++)if(units[i]==unit)return i;
    CHECK(FALSE,"invalid full unit handle %lx",unit);return 0;
}
static struct player_datum *player_get(long player)
{for(int i=0;i<2;i++)if(players[i]==player)return &people[i];CHECK(FALSE,"invalid full player handle");return NULL;}
static struct object_datum *object_try_and_get(long unit) {return unit==NONE?NULL:&objects[slot(unit)];}
static struct object_datum *biped_try_and_get(long unit) {return object_try_and_get(unit);}
static struct object_datum *biped_get(long unit) {return &objects[slot(unit)];}
static boolean network_coop_active(void) {return coop;}
static int game_connection(void) {return connection;}
static struct scenario *global_scenario_get(void) {return &scenario;}
static struct structure_bsp *global_structure_bsp_get(void) {return global_structure_bsp;}
static void data_iterator_new(struct data_iterator *it,void *data) {it->slot=-1;it->datum_index=NONE;}
static struct player_datum *data_iterator_next(struct data_iterator *it)
{it->slot++;if(it->slot==2)return NULL;it->datum_index=players[it->slot];return &people[it->slot];}
static boolean scenario_trigger_volume_test_object(short trigger,long unit) {return slot(unit)==0;}
static short players_coop_bsp_switch_trigger(long unit) {return slot(unit)==0?0:NONE;}
static long scenario_leaf_index_from_point(real_point3d const *p) {return p->x<200?0:NONE;}
static void biped_get_physics_pill(long unit,real_point3d *p,real *height,real *width)
{*p=objects[slot(unit)].object.position;*height=1;*width=.2f;}
static boolean collision_test_point(unsigned long mask,real_point3d const *p,long ignore) {return FALSE;}
static void unit_exit_seat_end(long unit) {ejections++;objects[slot(unit)].object.parent_object_index=NONE;}
static boolean player_teleport_internal(long player,long source,real_point3d const *p)
{teleports++;objects[slot(player_get(player)->unit_index)].object.position=*p;return TRUE;}
static boolean player_place_beside_teammate(long player) {CHECK(FALSE,"unexpected placement retry");return FALSE;}
/* The unchanged classic reconnect loop uses assignment as its condition. */
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wparentheses"
#include "players.inc"
#pragma clang diagnostic pop

static void main_stop_time(void) {clock_stops++;}
static void main_start_time(void) {clock_starts++;}
static void collision_log_enable(boolean value) {collision_log=value;}
static void scenario_call_disconnect_from_structure_bsp_procs(void) {disconnects++;}
static void scenario_call_reconnect_to_structure_bsp_procs(void) {reconnects++;players_reconnect_to_structure_bsp();}
static boolean scenario_structure_bsp_load(struct scenario_structure_bsp_reference *ref)
{loads[ref->structure_bsp.index]++;return !reject_load[ref->structure_bsp.index];}
static void scenario_structure_bsp_unload(struct scenario_structure_bsp_reference *ref) {unloads[ref->structure_bsp.index]++;}
static struct structure_bsp *structure_bsp_definition_get(long index) {return &structures[index];}
static boolean structure_bsp_port_verify(struct structure_bsp *bsp) {return !reject_verify[bsp-structures];}
static boolean mcc_cache_tags_loaded(void) {return FALSE;}
static void main_goto_main_menu(void) {menu++;}
static void structure_culling_load_settings(void) {culling++;}
static void structure_culling_structure_bsp_loaded(void) {culling++;}
#include "loader.inc"

static void setup(short old,short destination)
{
    scenario.bsp_switch_trigger_volumes=(struct tag_block){1,switches};
    scenario.cutscene_flags=(struct tag_block){1,flags};
    scenario.structure_bsp_references=(struct tag_block){2,references};
    switches[0]=(struct scenario_bsp_switch_trigger_volume){0,old,destination,0};
    flags[0].position=(real_point3d){100,20,2};
    for(int i=0;i<2;i++)
    {
        references[i].structure_bsp.index=i;
        leaves[i].cluster_index=2+i;
        structures[i].leaves=(struct tag_block){1,&leaves[i]};
        structures[i].collision_bsp=(struct tag_block){1,&collisions[i]};
        people[i]=(struct player_datum){units[i],8+i,i?NONE:0};
    }
    for(int i=0;i<4;i++)
    {
        objects[i].object.parent_object_index=NONE;
        objects[i].object.position=(real_point3d){10+i,5+i,1};
        objects[i].object.bounding_sphere_center=objects[i].object.position;
        objects[i].weapon=0x70007+i;
        objects[i].ammunition=30+i;
    }
    objects[0].object.parent_object_index=units[2];
    objects[1].object.parent_object_index=units[3];
    global_structure_bsp_index=scenario_globals->structure_bsp_index=old;
    global_structure_bsp=&structures[old];
    global_collision_bsp=&collisions[old];
    global_bsp3d=(struct bsp3d *)global_collision_bsp;
    players_globals->pending_teleport_starting_location_index=0;
    players_globals->local_player_count=1;
}

int main(int argc,char **argv)
{
    char const *name=argc>1?argv[1]:"";
    short old=!strcmp(name,"reverse")||!strcmp(name,"reverse-refused")?1:0,destination=1-old;
    struct object_datum before[4];
    setup(old,destination);
    if(!strcmp(name,"split-refused")||!strcmp(name,"split-forward"))
    {coop=FALSE;players_globals->local_player_count=2;people[1].local_player_index=1;}
    if(!strcmp(name,"same-vehicle"))objects[1].object.parent_object_index=units[2];
    memcpy(before,objects,sizeof(before));
    if(!strcmp(name,"no-pending")||!strcmp(name,"negative-pending")||!strcmp(name,"past-end-pending"))
    {
        players_globals->pending_teleport_starting_location_index=!strcmp(name,"negative-pending")?-2:
            !strcmp(name,"past-end-pending")?1:NONE;
        players_reconnect_to_structure_bsp();
    }
    else
    {
        boolean refused=strstr(name,"refused")!=NULL;
        if(refused)reject_load[destination]=TRUE;
        if(!strcmp(name,"verify-refused")){reject_load[destination]=FALSE;reject_verify[destination]=TRUE;}
        if(!strcmp(name,"restore-refused"))reject_load[old]=TRUE;
        CHECK(scenario_switch_structure_bsp(destination)==!refused,"incorrect load result");
        CHECK(clock_stops==1&&clock_starts==1&&collision_log,"time/collision state not restored");
        CHECK(disconnects==1&&loads[destination]==1&&unloads[old]==1,"loader lifecycle changed");
        if(!strcmp(name,"restore-refused"))
        {
            CHECK(menu==1&&!reconnects&&global_structure_bsp_index==NONE&&!global_structure_bsp&&
                  !global_collision_bsp&&!global_bsp3d,"unusable world did not return to menu");
            return 0;
        }
        CHECK(reconnects==1&&culling==2,"missing reconnect/culling lifecycle");
        CHECK(global_structure_bsp_index==(refused?old:destination)&&
              scenario_globals->structure_bsp_index==global_structure_bsp_index&&
              global_structure_bsp==&structures[global_structure_bsp_index]&&
              global_collision_bsp==&collisions[global_structure_bsp_index],"wrong loaded world");
        if(!refused)
        {
            if(!strcmp(name,"same-vehicle"))
                CHECK(!teleports&&!ejections&&objects[1].object.parent_object_index==units[2],"shared passengers separated");
            else
                CHECK(teleports==1&&ejections==1&&objects[1].object.position.x==100&&
                      objects[1].object.parent_object_index==NONE,"successful classic gather changed");
            CHECK(players_globals->pending_teleport_starting_location_index==NONE,"completed request not consumed");
            return 0;
        }
        CHECK(loads[old]==1&&!menu,"previous BSP not restored");
        if(!strcmp(name,"verify-refused"))CHECK(unloads[destination]==1,"refused BSP resources leaked");
    }
    CHECK(!teleports&&!ejections&&!memcmp(before,objects,sizeof(before)),"rejected transition moved/ejected a player or changed owned gear/vehicle");
    CHECK(players_globals->pending_teleport_starting_location_index==NONE,"stale transition not consumed");
    CHECK(people[0].cluster_index==NONE&&people[1].cluster_index==NONE,"restored clusters were not invalidated");
    if(!strcmp(name,"retry-refused"))
    {
        reject_load[destination]=FALSE;
        players_globals->pending_teleport_starting_location_index=0;
        CHECK(scenario_switch_structure_bsp(destination)&&global_structure_bsp_index==destination,
              "restored world could not retry the transition");
        CHECK(teleports==1&&ejections==1&&objects[1].object.position.x==100&&
              players_globals->pending_teleport_starting_location_index==NONE,
              "successful retry lost the normal gather");
    }
    return 0;
}
