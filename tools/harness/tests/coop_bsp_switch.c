/* The existing scenario loader calls the real player reconnect/teleport code.
   IO, tag storage, collision lookup and movement are small fixture boundaries;
   this is not native map traversal or a multiplayer session. */
#include "harness.h"
#include <limits.h>
#include <stdint.h>

#undef match_assert
#define match_assert(f,l,c) CHECK(c,"engine assertion")
#define match_vassert(f,l,c,m) CHECK(c,"%s",m)
#define _error_silent 0
#define _error_immediate 1
/* errors.c exits on immediate errors before the caller can recover. */
#define error(priority, ...) do { \
    if ((priority) == _error_immediate) { \
        fprintf(stderr, __VA_ARGS__); \
        fputc('\n', stderr); \
        exit(1); \
    } \
} while (0)
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
struct scenario_structure_bsp_reference {
    struct {long index;} structure_bsp;
    uint32_t base_address;
    long file_offset,file_size;
};
struct scenario {struct tag_block bsp_switch_trigger_volumes,cutscene_flags,structure_bsp_references;};
struct collision_bsp {int placeholder;};
struct bsp3d {int placeholder;};
struct structure_leaf {long cluster_index;};
struct structure_bsp {struct tag_block leaves,collision_bsp;};
struct data_iterator {long datum_index;int slot;};
struct player_datum {long unit_index,cluster_index;short local_player_index;};
struct object_datum {struct {long parent_object_index;real_point3d position,bounding_sphere_center;unsigned long flags;} object;
                     long weapon;short ammunition;struct {long player_index;} unit;};
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
static struct scenario_structure_bsp_reference references[4];
static struct structure_bsp structures[2],*global_structure_bsp;
static struct collision_bsp collisions[2],*global_collision_bsp;
static struct bsp3d *global_bsp3d;
static struct structure_leaf leaves[2];
static short global_structure_bsp_index;
static int coop=TRUE,connection=_game_connection_network_server;
static int reject_reference[4],reject_load[2],reject_verify[2],loads[2],unloads[2];
static int teleports,ejections,reconnects,disconnects,menu,clock_stops,clock_starts,collision_log=TRUE,culling;
static boolean crowded,solid,ignore_players,operations,opposed,connected;
static int stacked_calls,beside_retries;
static long ticks;
static struct {long stranded_since[2];} players_coop_state;

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
static struct object_datum *object_get(long unit) {return &objects[slot(unit)];}
static boolean network_coop_active(void) {return coop;}
static boolean extraction_owns_map(void) {return operations;}
static boolean players_coop_teammate(long player,long other)
{return other!=NONE&&player!=other&&!opposed;}
static void biped_fix_ignore_players(boolean ignore) {ignore_players=ignore;}
static int game_connection(void) {return connection;}
static struct scenario *global_scenario_get(void) {return &scenario;}
static struct structure_bsp *global_structure_bsp_get(void) {return global_structure_bsp;}
static void data_iterator_new(struct data_iterator *it,void *data) {it->slot=-1;it->datum_index=NONE;}
static struct player_datum *data_iterator_next(struct data_iterator *it)
{it->slot++;if(it->slot==2)return NULL;it->datum_index=players[it->slot];return &people[it->slot];}
static boolean scenario_trigger_volume_test_object(short trigger,long unit)
{
    return unit==units[0] || (connected &&
        (unit==units[3] || unit==objects[0].object.parent_object_index ||
            (unit==units[1] && objects[1].object.parent_object_index==NONE)));
}
static long object_get_ultimate_parent(long unit)
{
    while(objects[slot(unit)].object.parent_object_index!=NONE)
        unit=objects[slot(unit)].object.parent_object_index;
    return unit;
}
static short players_coop_bsp_switch_trigger(long unit) {return slot(unit)==0?0:NONE;}
static long scenario_leaf_index_from_point(real_point3d const *p) {return p->x<200?0:NONE;}
static long game_time_get(void) {return ticks;}
static boolean player_input_enabled(void) {return TRUE;}
static boolean players_coop_unit_in_structure(long unit)
{return scenario_leaf_index_from_point(&objects[slot(object_get_ultimate_parent(unit))].object.position)!=NONE;}
static boolean players_coop_unit_grounded(long unit) {return players_coop_unit_in_structure(unit);}
static boolean players_coop_unit_stranded(long unit) {return !players_coop_unit_in_structure(unit);}
static long players_coop_unit_where(long player,boolean (*eligible)(long))
{return player==players[1]&&eligible(units[0])?units[0]:NONE;}
static real_point3d const *players_checkpoint_position(long player) {return NULL;}
static void biped_get_physics_pill(long unit,real_point3d *p,real *height,real *width)
{*p=objects[slot(unit)].object.position;*height=1;*width=.2f;}
static boolean collision_test_point(unsigned long mask,real_point3d const *p,long ignore) {return FALSE;}
static void unit_exit_seat_end(long unit) {ejections++;objects[slot(unit)].object.parent_object_index=NONE;}
static boolean player_teleport_internal(long player,long source,real_point3d const *p,boolean coop_spawn)
{
    CHECK(!coop_spawn,"scripted BSP gather acquired the spawn-only policy");
    teleports++;
    if(ignore_players)stacked_calls++;
    if(solid||(crowded&&!ignore_players))return FALSE;
    objects[slot(player_get(player)->unit_index)].object.position=*p;
    return TRUE;
}
static boolean player_place_beside_teammate(long player)
{
    CHECK(crowded,"unexpected placement retry");
    beside_retries++;
    return FALSE;
}
/* The unchanged classic reconnect loop uses assignment as its condition. */
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wparentheses"
#include "players.inc"
#pragma clang diagnostic pop

static void main_stop_time(void) {clock_stops++;}
static void main_start_time(void) {clock_starts++;}
static void collision_log_enable(boolean value) {collision_log=value;}
enum {_pvs_activation_normal, _pvs_activation_object, _pvs_activation_cluster};
enum {_object_connected_to_map_bit};
#define _object_mask_all (~0UL)
static struct {short pvs_activation_type;union {short cluster_index;long object_index;} pvs_activation;} object_state;
static __typeof__(object_state) *object_globals=&object_state;
struct object_iterator {short slot;long index;};
static void object_iterator_new(struct object_iterator *it,unsigned long mask,long flags) {it->slot=0;}
static struct object_datum *object_iterator_next(struct object_iterator *it)
{if(it->slot==4)return NULL;it->index=units[it->slot];return &objects[it->slot++];}
static void object_disconnect_from_map(long index) {objects[slot(index)].object.flags=0;}
static void object_type_disconnect_from_structure_bsp(long index) {}
#define SET_FLAG(value,bit,on) ((value)=(on)?(value)|FLAG(bit):(value)&~FLAG(bit))
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wparentheses"
#include "objects.inc"
#pragma clang diagnostic pop
static void scenario_call_disconnect_from_structure_bsp_procs(void) {disconnects++;objects_disconnect_from_structure_bsp();}
static void scenario_call_reconnect_to_structure_bsp_procs(void) {reconnects++;players_reconnect_to_structure_bsp();}
static boolean mcc_loaded;
struct mcc_cache_bsp;
static struct {struct {uint32_t bsp_count;struct mcc_cache_bsp const *bsps;} report;} mcc_loaded_cache;
static boolean mcc_cache_tags_loaded(void) {return mcc_loaded;}
#define TAG_CACHE_SIZE 1
static struct {struct {long tag_data_size;} header;} cache_file_globals;
static boolean cache_file_structure_bsp_tag_valid(struct scenario_structure_bsp_reference const *ref)
{return !reject_reference[ref->structure_bsp.index];}
#define cache_file_structure_bsp_reference_verify cache_file_structure_bsp_tag_valid
#include "mcc_bsp.inc"
static boolean scenario_structure_bsp_load(struct scenario_structure_bsp_reference *ref)
{loads[ref->structure_bsp.index]++;return !reject_reference[ref->structure_bsp.index] && !reject_load[ref->structure_bsp.index];}
static void scenario_structure_bsp_unload(struct scenario_structure_bsp_reference *ref) {unloads[ref->structure_bsp.index]++;}
static struct structure_bsp *structure_bsp_definition_get(long index) {return &structures[index];}
static boolean structure_bsp_port_verify(struct structure_bsp *bsp) {return !reject_verify[bsp-structures];}
static void main_goto_main_menu(void) {menu++;}
static void structure_culling_load_settings(void) {culling++;}
static void structure_culling_structure_bsp_loaded(void) {culling++;}
#include "loader.inc"

static struct {short switch_to_structure_bsp_index;} main_globals={NONE};
static short distributed_host_structure_bsp_index = NONE;
static short global_structure_bsp_index_get(void) {return global_structure_bsp_index;}
struct distributed_structure_bsp {short structure_bsp_index,pad;};
static boolean loading_hud;
static void hud_load(boolean value) {loading_hud=value;}
#define console_warning(...) ((void)0)
typedef unsigned short word;
struct distributed_unit_state {byte player_index;};
static int unit_snapshots_applied;
static word distributed_unit_state_read(byte const *data,byte const *end,struct distributed_unit_state *state)
{
    if(data==end)
        return 0;
    state->player_index=*data;
    return 1;
}
static void distributed_handle_unit_state(long host_time,struct distributed_unit_state const *state)
{
    CHECK(host_time>0&&state->player_index==0,"invalid snapshot passed to unit handler");
    unit_snapshots_applied++;
}
#include "bsp_receive.inc"

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
        objects[i].unit.player_index=i<2?players[i]:NONE;
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
    short old=!strcmp(name,"reverse")||!strcmp(name,"reverse-refused")||!strcmp(name,"connected-reverse")?1:0,destination=1-old;
    struct object_datum before[4];
    setup(old,destination);
    if (!strcmp(name, "crossing-root-trigger"))
    {
        switches[0].cutscene_flag_index = NONE;
        CHECK(!player_in_bsp_switch_volume(&switches[0], units[0], units[2]),
              "seat triggered a continuous switch before the vehicle entered");
        connected = TRUE;
        CHECK(player_in_bsp_switch_volume(&switches[0], units[1], units[2]),
              "vehicle entered without detecting its occupied seat");
        CHECK(player_in_bsp_switch_volume(&switches[0], units[0], units[0]),
              "continuous infantry crossing did not trigger");
        switches[0].cutscene_flag_index = 0;
        CHECK(!player_in_bsp_switch_volume(&switches[0], units[1], units[2]),
              "vehicle root overrode a marked transition's native trigger");
        coop = FALSE;
        switches[0].cutscene_flag_index = NONE;
        CHECK(player_in_bsp_switch_volume(&switches[0], units[0], units[2]),
              "local crossing stopped using the native seat trigger");
        return 0;
    }
    if(!strcmp(name,"host-superseded"))
    {
        struct distributed_structure_bsp message={destination,0};
        distributed_handle_structure_bsp(&message);
        CHECK(main_globals.switch_to_structure_bsp_index==destination&&loading_hud,"host BSP transition did not queue");
        message.structure_bsp_index=2;
        distributed_handle_structure_bsp(&message);
        CHECK(main_globals.switch_to_structure_bsp_index==destination&&loading_hud,"invalid update changed the pending switch");
        message.structure_bsp_index=old;
        distributed_handle_structure_bsp(&message);
        CHECK(main_globals.switch_to_structure_bsp_index==NONE&&!loading_hud,"client retained superseded BSP switch");
        CHECK(!disconnects&&!ejections&&!teleports&&global_structure_bsp_index==old,"client disturbed the active world");
        message.structure_bsp_index=destination;
        distributed_handle_structure_bsp(&message);
        CHECK(main_globals.switch_to_structure_bsp_index==destination&&loading_hud,"later valid switch did not queue");
        return 0;
    }
    if(!strcmp(name,"host-refused-neighbor"))
    {
        scenario.structure_bsp_references.count=4;
        references[2].structure_bsp.index=2;
        reject_reference[2]=TRUE;
        main_sync_structure_bsp(destination);
        main_sync_structure_bsp(2);
        CHECK(main_globals.switch_to_structure_bsp_index==destination&&loading_hud,
              "refused neighbor displaced the pending valid route");
        main_switch_to_structure_bsp_private();
        CHECK(global_structure_bsp_index==destination&&!loading_hud,"valid route did not finish");
        main_sync_structure_bsp(2);
        CHECK(main_globals.switch_to_structure_bsp_index==NONE&&!loading_hud,
              "refused neighbor opened a loading screen");
        CHECK(disconnects==1&&global_structure_bsp_index==destination,"refused neighbor disturbed the active BSP");
        return 0;
    }
    if(!strcmp(name,"host-snapshot-before-load"))
    {
        struct distributed_structure_bsp message={destination,0};
        byte state[]={0};
        distributed_handle_structure_bsp(&message);
        distributed_handle_unit_states(1,state,state+sizeof(state),1);
        CHECK(unit_snapshots_applied==0,"applied destination snapshot to the old BSP");
        message.structure_bsp_index=old;
        distributed_handle_structure_bsp(&message);
        distributed_handle_unit_states(2,state,state+sizeof(state),1);
        CHECK(unit_snapshots_applied==1,"snapshot did not resume after the host canceled the switch");
        message.structure_bsp_index=destination;
        distributed_handle_structure_bsp(&message);
        main_switch_to_structure_bsp_private();
        CHECK(global_structure_bsp_index==destination,"client did not load the destination BSP");
        distributed_handle_unit_states(3,state,state+sizeof(state),1);
        CHECK(unit_snapshots_applied==2,"snapshot did not resume after loading the destination BSP");
        return 0;
    }
    if(!strcmp(name,"host-snapshot-after-rollback"))
    {
        struct distributed_structure_bsp message = {destination, 0};
        byte state[] = {0};
        reject_load[destination] = TRUE;
        distributed_handle_structure_bsp(&message);
        main_switch_to_structure_bsp_private();
        CHECK(global_structure_bsp_index == old && !loading_hud, "failed load did not restore the source BSP");

        distributed_handle_unit_states(1, state, state + sizeof(state), 1);
        CHECK(unit_snapshots_applied == 0, "rollback accepted a player snapshot from the destination BSP");

        reject_load[destination] = FALSE;
        distributed_handle_structure_bsp(&message);
        main_switch_to_structure_bsp_private();
        CHECK(global_structure_bsp_index == destination, "client did not retry the failed destination");
        distributed_handle_unit_states(2, state, state + sizeof(state), 1);
        CHECK(unit_snapshots_applied == 1, "snapshots did not resume after the destination loaded");
        return 0;
    }
    if(!strncmp(name,"rescue-",7))
    {
        solid=!strcmp(name,"rescue-solid");
        crowded=TRUE;
        objects[0].object.parent_object_index=objects[1].object.parent_object_index=NONE;
        objects[1].object.position.x=250.f;
        ticks=1;
        players_coop_rescue_stranded();
        CHECK(!teleports,"stranded grace period was skipped");
        ticks+=2*TICKS_PER_SECOND;
        players_coop_rescue_stranded();
        CHECK(teleports==2&&stacked_calls==1&&!ignore_players,"timed rescue did not restore collision policy");
        CHECK(objects[1].object.position.x==(solid?250.f:objects[0].object.bounding_sphere_center.x),
              "timed rescue accepted solid terrain or failed to reach the teammate");
        ticks++;
        players_coop_rescue_stranded();
        CHECK(teleports==2,"timed rescue retried immediately");
        return 0;
    }
    if(!strcmp(name,"activation-cluster")||!strcmp(name,"activation-object"))
    {
        boolean cluster=!strcmp(name,"activation-cluster");
        object_globals->pvs_activation_type=cluster?_pvs_activation_cluster:_pvs_activation_object;
        object_globals->pvs_activation.object_index=units[2];
        if(cluster)object_globals->pvs_activation.cluster_index=42;
        players_globals->pending_teleport_starting_location_index=NONE;
        CHECK(scenario_switch_structure_bsp(destination),"BSP transition failed");
        CHECK(object_globals->pvs_activation_type==(cluster?_pvs_activation_normal:_pvs_activation_object),
              "stale cluster retained or object activation lost");
        if(!cluster)CHECK(object_globals->pvs_activation.object_index==units[2],"activation object changed");
        return 0;
    }
    if(!strcmp(name,"split-refused")||!strcmp(name,"split-forward"))
    {coop=FALSE;players_globals->local_player_count=2;people[1].local_player_index=1;}
    if(!strcmp(name,"same-vehicle"))objects[1].object.parent_object_index=units[2];
    connected=!strncmp(name,"connected-",10);
    if(connected)
    {
        switches[0].cutscene_flag_index=NONE;
        if(!strcmp(name,"connected-foot"))
            objects[0].object.parent_object_index=objects[1].object.parent_object_index=NONE;
        if(!strcmp(name,"connected-stranded"))
            objects[3].object.position.x=objects[3].object.bounding_sphere_center.x=250.f;
        if (!strcmp(name, "connected-stranded-host"))
            objects[2].object.position.x = objects[2].object.bounding_sphere_center.x = 250.f;
        if(!strcmp(name,"connected-offset-seats"))
        {
            objects[0].object.position.x=objects[0].object.bounding_sphere_center.x=250.f;
            objects[1].object.position.x=objects[1].object.bounding_sphere_center.x=250.f;
        }
        if(!strcmp(name,"connected-unrelated"))
        {
            objects[1].object.parent_object_index=units[2];
            objects[0].object.parent_object_index=units[3];
        }
    }
    crowded=!strncmp(name,"crowded-",8);
    if(crowded)
    {
        solid=!strcmp(name,"crowded-solid");
        operations=!strcmp(name,"crowded-operation");
        opposed=!strcmp(name,"crowded-enemy");
        objects[0].object.parent_object_index=objects[1].object.parent_object_index=NONE;
        objects[1].object.position.x=!strcmp(name,"crowded-inside")?50.f:250.f;
        objects[1].object.bounding_sphere_center=objects[1].object.position;
    }
    memcpy(before,objects,sizeof(before));
    if(!strcmp(name,"reference-refused")||!strcmp(name,"mcc-reference-refused"))
    {
        struct mcc_cache_bsp checked={.file_offset=4096,.size=1024,.address=0x40001000,.tag_handle=(uint32_t)destination};
        if(!strcmp(name,"mcc-reference-refused"))
        {
            struct scenario_structure_bsp_reference *ref=&references[destination];
            mcc_loaded=TRUE;
            mcc_loaded_cache.report.bsp_count=1;
            mcc_loaded_cache.report.bsps=&checked;
            ref->file_offset=checked.file_offset;
            ref->file_size=checked.size;
            ref->base_address=checked.address;
            CHECK(scenario_structure_bsp_reference_is_valid(ref),"matching MCC reference refused");
            ref->base_address++;
            CHECK(!mcc_cache_bsp_reference_checked(ref),"changed MCC address accepted");
            ref->base_address--;
            ref->file_size++;
            CHECK(!mcc_cache_bsp_reference_checked(ref),"changed MCC size accepted");
            ref->file_size--;
            ref->file_offset++;
        }
        else
            reject_reference[destination]=TRUE;
        object_globals->pvs_activation_type=_pvs_activation_cluster;
        object_globals->pvs_activation.cluster_index=2;
        CHECK(!scenario_switch_structure_bsp(destination),"invalid BSP reference was accepted");
        CHECK(!clock_stops&&!clock_starts&&collision_log&&!disconnects&&!reconnects&&
              !loads[old]&&!unloads[old]&&!loads[destination]&&!culling&&!menu,
              "invalid reference tore down simulation state before loading");
        CHECK(global_structure_bsp_index==old&&scenario_globals->structure_bsp_index==old&&
              global_structure_bsp==&structures[old]&&global_collision_bsp==&collisions[old],
              "invalid reference displaced the active BSP");
        CHECK(people[0].cluster_index==8&&people[1].cluster_index==9&&
              object_globals->pvs_activation_type==_pvs_activation_cluster&&
              object_globals->pvs_activation.cluster_index==2&&!memcmp(before,objects,sizeof(before)),
              "invalid reference changed cluster activation, vehicles or equipment");
        CHECK(players_globals->pending_teleport_starting_location_index==NONE,"refused switch left a pending gather");
        return 0;
    }
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
            if(crowded)
            {
                boolean can_stack=!operations&&!opposed&&before[1].object.position.x>200.f;
                CHECK(beside_retries==1,"existing nearby placement was skipped");
                CHECK(stacked_calls==can_stack&&teleports==1+can_stack&&!ignore_players,
                      "incorrect crowded placement retry or collision flag leaked");
                CHECK(!ejections&&objects[1].weapon==before[1].weapon&&objects[1].ammunition==before[1].ammunition,
                      "crowd recovery changed player body or equipment");
                CHECK(objects[1].object.position.x==(can_stack&&!solid?before[0].object.bounding_sphere_center.x:before[1].object.position.x),
                      "crowd recovery chose an obstructed or unexpected position");
            }
            else if(!strcmp(name,"same-vehicle"))
                CHECK(!teleports&&!ejections&&objects[1].object.parent_object_index==units[2],"shared passengers separated");
            else if(connected)
            {
                if (!strcmp(name, "connected-stranded-host"))
                {
                    CHECK(ejections == 1 && teleports == 1 && objects[0].object.parent_object_index == NONE,
                          "a stranded host seat displaced the safe remote gather anchor");
                    CHECK(objects[0].object.position.x == before[3].object.position.x &&
                          objects[0].weapon == before[0].weapon && objects[0].ammunition == before[0].ammunition,
                          "stranded host did not recover beside the arrived vehicle with its gear");
                    CHECK(!memcmp(&before[1], &objects[1], sizeof(objects[1])),
                          "the arrived guest was unseated or moved");
                }
                else if(!strcmp(name,"connected-stranded")||!strcmp(name,"connected-unrelated"))
                    CHECK(teleports==1&&ejections==1&&objects[1].object.parent_object_index==NONE,
                          "player without a safe crossing bypassed native recovery");
                else
                    CHECK(!teleports&&!ejections&&!memcmp(before,objects,sizeof(before)),
                          "arrived crews or infantry were displaced during a continuous crossing");
            }
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
