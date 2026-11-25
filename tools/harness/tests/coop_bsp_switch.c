/* The existing scenario loader calls the real player reconnect/teleport code.
   IO, tag storage, collision lookup and movement are small fixture boundaries;
   this is not native map traversal or a multiplayer session. */
#include "harness.h"
#include <limits.h>
#include <stdint.h>
#include <math.h>

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
#define NETWORK_GAME_MAXIMUM_PLAYER_COUNT 16
#include <limits.h>
#define SHORT_MAX SHRT_MAX
#define MIN(a, b) ((a) < (b) ? (a) : (b))
#define MAX(a, b) ((a) > (b) ? (a) : (b))
#define VALID_INDEX(index, count) (index>=0 && index<count)
enum {_game_connection_local=0, _game_connection_network_server=2};
enum {_collision_test_front_facing_surfaces_bit, _collision_test_ignore_invisible_surfaces_bit,
      _collision_test_structure_bit, _collision_test_objects_scenery_bit,
      _collision_test_back_facing_surfaces_bit, _collision_test_ignore_two_sided_surfaces_bit,
      _collision_test_ignore_breakable_surfaces_bit};

struct tag_block {long count;void *elements;};
static void *element(struct tag_block const *block,long index,size_t size)
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
struct scenario {
    struct tag_block bsp_switch_trigger_volumes,cutscene_flags,structure_bsp_references,netgame_flags,netgame_equipment;
    short type;
};
struct player_starting_location {real_point3d position;short structure_bsp_reference_index;};
enum {_scenario_type_multiplayer=1};
struct scenario_netgame_flag {real_point3d position; short type,team_index;};
struct collision_surface {long plane_designator;};
struct collision_bsp {struct tag_block surfaces;};
struct bsp3d {int placeholder;};
struct structure_leaf {long cluster_index;};
struct location {long leaf_index; short cluster_index,bonus;};
struct structure_bsp {
    struct tag_block leaves,collision_bsp,clusters,surfaces,breakable_surfaces;
    struct {long size;} cluster_data,sound_cluster_data;
};
struct data_iterator {long datum_index;int slot;};
struct player_datum {long unit_index,cluster_index;short local_player_index,identifier;};
struct object_datum {
    struct {long parent_object_index,first_child_object_index,next_object_index;short type;
        real_point3d position,bounding_sphere_center;real bounding_sphere_radius;
        unsigned long flags,damage_flags;struct location location;} object;
    long weapon;short ammunition;
    struct {long player_index,driver_object_index,gunner_object_index;short parent_seat_index;} unit;
};
#define biped_datum object_datum
#define unit_datum object_datum
#define _object_mask_vehicle FLAG(_object_type_vehicle)
#define _object_mask_biped FLAG(_object_type_biped)
enum {_object_dead_bit=0, _object_type_biped=0, _object_type_vehicle=4, _object_type_machine=5, _object_type_control=6, _object_type_scenery=7, _object_type_light_fixture=8, _object_type_sound_scenery=9, _object_type_projectile=10};
#define _object_mask_projectile FLAG(_object_type_projectile)
#define _object_mask_machine FLAG(_object_type_machine)
#define _object_mask_control FLAG(_object_type_control)
#define _object_mask_scenery FLAG(_object_type_scenery)
#define _object_mask_light_fixture FLAG(_object_type_light_fixture)
#define _object_mask_sound_scenery FLAG(_object_type_sound_scenery)
#define _object_mask_device (_object_mask_machine | _object_mask_control | _object_mask_light_fixture)

static struct scenario scenario;
static struct scenario *global_scenario=&scenario;
static struct {short structure_bsp_index;} scenario_state;
static __typeof__(scenario_state) *scenario_globals=&scenario_state;
enum {_local_player_triggered_switch_none = -2};
static struct {
    short pending_teleport_starting_location_index,local_player_count;
    short local_player_triggered_switch,bsp_check_recursive_switch_ticks;
    boolean respawn_failed;
    unsigned long combined_pvs[1], combined_pvs_local[1];
} player_state;
static __typeof__(player_state) *players_globals=&player_state;
static struct player_datum people[2];
static struct object_datum objects[4];
static long units[4]={0x10000,0x20001,0x30002,0x40003};
static long players[2]={0x50000,0x60001};
static void *player_data;
static struct scenario_bsp_switch_trigger_volume switches[3];
static struct scenario_cutscene_flag flags[1];
static struct scenario_structure_bsp_reference references[4];
static struct structure_bsp structures[2],*global_structure_bsp;
static struct collision_bsp collisions[4],*global_collision_bsp;
static struct bsp3d *global_bsp3d;
static struct structure_leaf leaves[2];
static short global_structure_bsp_index;
static int coop=TRUE,connection=_game_connection_network_server;
static int reject_reference[4],reject_load[4],reject_verify[4],loads[4],unloads[4];
static int teleports,ejections,reconnects,disconnects,menu,clock_stops,clock_starts,collision_log=TRUE,culling;
static boolean crowded,solid,ignore_players,operations,opposed,connected,dedicated_vote_routes,parallel_routes;
static int stacked_calls,beside_retries;
static long ticks;
static struct {
    long stranded_since[2],bsp_switch_time;
    short bsp_crossing_from,bsp_crossing_to,bsp_crossing_count;
    long bsp_crossing_players[NETWORK_GAME_MAXIMUM_PLAYER_COUNT];
} players_coop_state;
#define csmemcpy memcpy

static int slot(long unit)
{
    for(int i=0;i<4;i++)if(units[i]==unit)return i;
    CHECK(FALSE,"invalid full unit handle %lx",unit);return 0;
}
static struct player_datum *player_get(long player)
{for(int i=0;i<2;i++)if(players[i]==player)return &people[i];CHECK(FALSE,"invalid full player handle");return NULL;}
static struct object_datum *object_try_and_get(long unit) {return unit==NONE?NULL:&objects[slot(unit)];}
static void *object_try_and_get_and_verify_type(long object_index, unsigned long types);
static struct object_datum *unit_try_and_get(long unit)
{return unit==units[0]||unit==units[1]?object_try_and_get(unit):NULL;}
static struct object_datum *biped_try_and_get(long unit) {return object_try_and_get(unit);}
static struct object_datum *biped_get(long unit) {return &objects[slot(unit)];}
static struct object_datum *object_get(long unit) {return &objects[slot(unit)];}
static boolean regional_match, regional_room_ready, regional_host_agreed;
static boolean delta_peer_game_host_agreed_regional_bsp(void) {return regional_host_agreed;}
static boolean network_coop_active(void) {return coop;}
static boolean network_regional_bsp_active(void) {return regional_match;}
static boolean network_synchronizes_structure_bsp(void) {return coop||regional_match;}
static boolean splitscreen_local;
enum {_delta_capability_regional_bsp};
static boolean network_game_is_splitscreen_local(void) {return splitscreen_local;}
static int delta_peer_game_room_has(int capability)
{
    CHECK(capability==_delta_capability_regional_bsp,"incorrect capability used for BSP transitions");
    return regional_room_ready;
}
#include "regional_room.inc"
static boolean extraction_owns_map(void) {return operations;}
static boolean players_coop_teammate(long player,long other)
{return other!=NONE&&player!=other&&!opposed;}
static void biped_fix_ignore_players(boolean ignore) {ignore_players=ignore;}
static int game_connection(void) {return connection;}
static struct scenario *global_scenario_get(void) {return &scenario;}
static struct scenario *global_scenario_try_and_get(void);
static boolean scenario_uses_regional_gameplay(struct scenario const *scenario);
static struct structure_bsp *global_structure_bsp_get(void) {return global_structure_bsp;}
static short global_structure_bsp_index_get(void);
static struct collision_bsp *global_collision_bsp_get(void) {return global_collision_bsp;}
struct item_datum {
    struct {unsigned long flags;real_point3d position;long parent_object_index;} object;
    struct {unsigned long flags;short rested_surface_index,bsp_index,netgame_equipment_index;long last_owned_time,item_on_rest_object_index;} item;
};
struct item_definition {struct {unsigned long flags;} item;};
enum {_item_on_structure_bit, _item_does_not_accelerate_bit, _item_on_object_bit};
enum {_object_at_rest_bit, _object_outside_of_map_bit};
enum {_item_definition_antigravity_bit};
static void data_iterator_new(struct data_iterator *it,void *data) {it->slot=-1;it->datum_index=NONE;}
static struct player_datum *data_iterator_next(struct data_iterator *it)
{it->slot++;if(it->slot==2)return NULL;it->datum_index=players[it->slot];return &people[it->slot];}
static boolean scenario_trigger_volume_test_object(short trigger,long unit)
{
    if (dedicated_vote_routes)
        return trigger == 2 ? unit == units[3] : unit == units[0];
    if (parallel_routes)
        return trigger == 1 ? unit == units[3] || unit == units[1] : unit == units[2] || unit == units[0];
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
static struct structure_bsp const *scenario_structure_bsp_try_and_get(short index);
static long regional_blocker=NONE;
static int regional_pill_probes, regional_vehicle_probes;
static boolean collision_object_blocks_pill(long blocker,real_point3d const *base,real height,real width)
{
    CHECK(height==1.f&&width==.2f,"destination used the wrong player collision capsule");
    regional_pill_probes++;
    return blocker==regional_blocker;
}
static boolean physics_vehicle_blocked_by_object(long vehicle,long blocker)
{
    regional_vehicle_probes++;
    return blocker==regional_blocker;
}
static boolean bsp_pill_blocked[2];
static unsigned long bsp_vehicle_contacts[2];
static unsigned long physics_structure_contact_mask(long object_index,struct structure_bsp const *structure,short index)
{
    CHECK((object_index==units[2]||object_index==units[3])&&structure&&VALID_INDEX(index,2),
          "invalid vehicle BSP contact query");
    return bsp_vehicle_contacts[index];
}
static boolean collision_structure_blocks_pill(struct structure_bsp const *structure,short index,
                                              real_point3d const *base,real height,real width)
{
    CHECK(VALID_INDEX(index,2)&&height==1.f&&width==.2f,"seam used incorrect native biped shape");
    CHECK(structure && base->x==objects[0].object.position.x,"seam used incorrect biped position");
    CHECK((index==0)==(structure==global_structure_bsp),"tested capsule in the wrong BSP");
    return bsp_pill_blocked[index];
}
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
static void players_compute_combined_pvs(unsigned long *pvs, boolean local)
{
    CHECK(pvs==(local ? players_globals->combined_pvs_local : players_globals->combined_pvs),
        "wrong destination visibility buffer");
    CHECK(people[0].cluster_index==NONE&&people[1].cluster_index==NONE,
        "existing player clusters used to build destination visibility");
    pvs[0]=2;
}
/* The unchanged classic reconnect loop uses assignment as its condition. */
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wparentheses"
static long prepared_crossing_leaf;
static boolean cold_crossing;
static boolean test_region_lookup,test_region_overlap;
static long bsp3d_test_point(struct bsp3d const *tree, long node, real_point3d const *point)
{
    if (test_region_lookup)
    {
        CHECK(node==0&&tree==(struct bsp3d const *)&collisions[global_structure_bsp_index],"incorrect flag collision tree");
        boolean shared=test_region_overlap&&point->x>=199&&point->x<=201;
        return point->z >= 0 && (shared || (point->x < 200)==(global_structure_bsp_index==0)) ? 0 : NONE;
    }
    CHECK(tree==(struct bsp3d const *)&collisions[1]&&node==0&&global_collision_bsp==&collisions[0],
          "destination check used the wrong collision tree");
    if (cold_crossing)
        CHECK(global_structure_bsp_index==NONE&&disconnects==reconnects+1,
              "cold validation happened after publishing the destination");
    else
        CHECK(global_structure_bsp_index==0&&!disconnects,"destination test replaced active collision");
    return point->x < 200 ? prepared_crossing_leaf : NONE;
}
#include "region_location.inc"
#define HALO_PORT_MAXIMUM_NETWORK_PLAYERS 128
#define csmemset memset
#define NUMBEROF(array) ((int)(sizeof(array)/sizeof((array)[0])))
struct object_iterator {short slot;long index;unsigned long mask;};
static void object_iterator_new(struct object_iterator *it,unsigned long mask,long flags) {it->slot=0;it->mask=mask;}
static struct object_datum *object_iterator_next(struct object_iterator *it)
{
    while(it->slot<4)
    {
        short slot=it->slot++;
        if(TEST_FLAG(it->mask,objects[slot].object.type))
        {
            it->index=units[slot];
            return &objects[slot];
        }
    }
    return NULL;
}
static short game_engine_object_recorded_bsp(long index,short type);
static real distance_squared3d(real_point3d const *a, real_point3d const *b);
static boolean scenario_trigger_volume_test_point(short volume, real_point3d const *point);
#include "players.inc"
#undef NUMBEROF
#undef csmemset
#pragma clang diagnostic pop

static void main_stop_time(void) {clock_stops++;}
static void main_start_time(void) {clock_starts++;}
static void collision_log_enable(boolean value) {collision_log=value;}
enum {_pvs_activation_normal, _pvs_activation_object, _pvs_activation_cluster};
enum {_object_connected_to_map_bit};
#define _object_mask_all (~0UL)
static struct {
    short pvs_activation_type;
    union {short cluster_index;long object_index;} pvs_activation;
    unsigned long last_active_cluster_bits[1],active_cluster_bits[1];
} object_state;
static __typeof__(object_state) *object_globals=&object_state;
static void object_disconnect_from_map(long index) {objects[slot(index)].object.flags=0;}
static void object_type_disconnect_from_structure_bsp(long index) {}
#define SET_FLAG(value,bit,on) ((value)=(on)?(value)|FLAG(bit):(value)&~FLAG(bit))
static struct scenario *global_scenario_try_and_get(void) {return global_scenario;}
static boolean network_game_distributed_client(void);
static void game_engine_record_regional_object(long index,struct object_datum const *object,short bsp);
#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wparentheses"
static short object_disconnected_bsp=NONE;
#include "objects.inc"
#pragma clang diagnostic pop
static void scenario_call_disconnect_from_structure_bsp_procs(void) {disconnects++;objects_disconnect_from_structure_bsp();}
static void scenario_call_reconnect_to_structure_bsp_procs(void) {reconnects++;players_reconnect_to_structure_bsp();}
static boolean object_bsp_activation_pending;
enum {_object_header_automatically_deactivate_bit, _object_header_connected_to_map_bit,
      _object_header_active_bit, _object_header_child_bit};
enum {_object_deleted_when_deactivated_bit=1};
struct object_header_datum {
    short identifier,cluster_index;
    unsigned long flags;
    struct object_datum *datum;
};
static struct object_header_datum activation_headers[4];
static struct {void *data;short count;} activation_table={activation_headers,4}, *object_header_data=&activation_table;
static unsigned activations,deactivations,deletions,decals;
#define csmemcmp memcmp
#define BIT_VECTOR_SIZE_IN_BYTES(count) (sizeof(unsigned long))
#define BIT_VECTOR_TEST_FLAG(bits,index) TEST_FLAG((bits)[0],index)
static unsigned long const *players_get_combined_pvs(void) {return players_globals->combined_pvs;}
static void object_activate(long index) {activations|=FLAG(index);}
static void object_deactivate(long index) {deactivations|=FLAG(index);}
static void object_delete_initial_recursive(long index,boolean siblings) {deletions|=FLAG(index);}
static void structure_decals_update(unsigned long *old,unsigned long *current,short clusters)
{
    CHECK(*old==0&&*current==2&&clusters==2,"old region visibility used for new decals");
    decals++;
}
#define DATUM_INDEX_NEW(i,salt) ((long)(((unsigned long)(unsigned short)(salt)<<16)|(i)))
boolean game_engine_object_parked(long object_index, short type);
#include "item_structure_support.inc"
#include "bsp_activation.inc"
static int test_destination_activation(void)
{
    unsigned long flags=FLAG(_object_header_automatically_deactivate_bit) |
        FLAG(_object_header_connected_to_map_bit);
    short i;
    global_structure_bsp=&structures[0];
    structures[0].clusters.count=2;
    object_globals->active_cluster_bits[0]=2;
    players_globals->combined_pvs[0]=2; /* Same bit as the previous BSP. */
    for(i=0;i<4;i++) {
        activation_headers[i].identifier=1;
        activation_headers[i].datum=&objects[i];
        activation_headers[i].flags=flags;
    }
    activation_headers[0].cluster_index=1;
    objects[0].object.flags=FLAG(_object_deleted_when_deactivated_bit);
    activation_headers[1].cluster_index=0;
    objects[1].object.flags=FLAG(_object_deleted_when_deactivated_bit);
    activation_headers[2].cluster_index=1;
    activation_headers[2].flags|=FLAG(_object_header_active_bit);
    objects[2].object.flags=FLAG(_object_deleted_when_deactivated_bit);
    activation_headers[3].cluster_index=NONE;
    activation_headers[3].flags|=FLAG(_object_header_active_bit)|FLAG(_object_header_child_bit);
    object_bsp_activation_pending=TRUE;
    objects_update_cluster_activation();
    CHECK(activations==1&&deletions==2&&!deactivations,
        "destination visibility discarded a visible object or an attached child");
    CHECK(decals==1&&!object_bsp_activation_pending,"destination visibility did not finish");
    objects_update_cluster_activation();
    CHECK(decals==1&&activations==1&&deletions==2&&!deactivations,
        "steady-state activation did not retain its previous fast path");
    return 0;
}
#undef BIT_VECTOR_SIZE_IN_BYTES

static boolean mcc_loaded;
struct mcc_cache_bsp;
static struct mcc_runtime {
    struct {uint32_t bsp_count,tag_base;struct {uint16_t scenario_type;} identity;struct mcc_cache_bsp const *bsps;} report;
    uint32_t capacity,used,bsp_addresses[4];
} mcc_loaded_cache;
static struct mcc_cache_bsp const *mcc_retained_bsps[2];
static struct mcc_cache_bsp const *mcc_active_bsp;
static struct mcc_cache_bsp const *mcc_preview_bsp;
static uint32_t mcc_failed_bsp_previews;
static uint32_t mcc_bsp_preview_tick;
static uint32_t mcc_bsps_in_use;
static struct __attribute__((aligned(16))) {
    byte header[24];
    struct structure_bsp structure;
} mcc_regions[4];
static boolean mcc_prepare_fixture,mcc_region_live[4];
static struct structure_bsp mcc_file_structure;
static boolean mcc_file_read_failed;
static int mcc_file_reads;
static int mcc_runtime_read(struct mcc_runtime *runtime,uint32_t offset,uint32_t size,void *out)
{
    CHECK(runtime==&mcc_loaded_cache&&size==sizeof(mcc_file_structure)&&
          (offset==0x1018||(mcc_prepare_fixture&&(offset==0x2018||offset==0x3018||offset==0x4018))),"unexpected BSP header read");
    mcc_file_reads++;
    if (mcc_file_read_failed) return FALSE;
    memcpy(out,&mcc_file_structure,size);
    return TRUE;
}
static boolean structure_bsp_port_verify(struct structure_bsp const *bsp);
static boolean mcc_cache_tags_loaded(void) {return mcc_loaded;}
#define TAG_CACHE_SIZE 1
static struct {struct {long tag_data_size;} header;void *structure_bsp_header;} cache_file_globals;
static struct {void *base_address;} test_tag_instances[4];
static int mcc_prepared_loads,mcc_prepared_releases,mcc_prepared_validations;
#define cache_get_tag_instance(index) (&test_tag_instances[index])
static int mcc_geometry_bsp_load(struct mcc_runtime *runtime, uint32_t handle, uint32_t offset,
                                 uint32_t size, void *address, void **structure)
{
    short active=global_structure_bsp_index;
    uint32_t window=(uint32_t)(uintptr_t)address;
    uint32_t slot_offset=window-runtime->report.tag_base;

    CHECK(runtime==&mcc_loaded_cache&&handle<runtime->report.bsp_count&&size==sizeof(mcc_regions[handle])&&
          slot_offset%size==0&&slot_offset+size<=runtime->capacity&&offset==references[handle].file_offset,
          "loaded outside a checked fixture cache window");
    CHECK(active!=NONE&&active!=handle&&
          cache_file_globals.structure_bsp_header==xbox_pointer(runtime->bsp_addresses[active])&&
          test_tag_instances[active].base_address==xbox_pointer(runtime->bsp_addresses[active]+24),
          "preparation detached the active BSP");
    for (uint32_t index=0;index<runtime->report.bsp_count;index++)
        CHECK(!mcc_region_live[index]||runtime->bsp_addresses[index]!=window,
              "preparation wrote over live geometry");
    CHECK(!test_tag_instances[handle].base_address&&!mcc_region_live[handle]&&
          mcc_region_live[0]+mcc_region_live[1]+mcc_region_live[2]+mcc_region_live[3]<=
          (runtime->report.identity.scenario_type==_scenario_type_multiplayer?runtime->report.bsp_count-1:1),
          "conversion exceeded the resident limit or published the target early");
    mcc_prepared_loads++;
    if (reject_load[handle]) return FALSE;
    mcc_region_live[handle]=TRUE;
    runtime->bsp_addresses[handle]=window;
    *structure=xbox_pointer(window+24);
    if (*structure!=&mcc_regions[handle].structure)
        memcpy(*structure,&mcc_regions[handle].structure,sizeof(mcc_regions[handle].structure));
    return TRUE;
}
static void mcc_geometry_bsp_unload(struct mcc_runtime *runtime, uint32_t handle)
{
    CHECK(runtime==&mcc_loaded_cache&&handle<runtime->report.bsp_count&&mcc_region_live[handle],"released missing geometry");
    mcc_region_live[handle]=FALSE;
    runtime->bsp_addresses[handle]=0;
    mcc_prepared_releases++;
}
static int mcc_bsp_validate(struct mcc_runtime *runtime, long handle, void *address, long size, unsigned long linked_address)
{
    CHECK(runtime==&mcc_loaded_cache&&handle>=0&&handle<(long)runtime->report.bsp_count&&mcc_region_live[handle]&&
          address==xbox_pointer(runtime->bsp_addresses[handle])&&size==sizeof(mcc_regions[handle]),
          "validated a different cache window");
    CHECK(linked_address==references[handle].base_address,"lost the linked address");
    mcc_prepared_validations++;
    return TRUE;
}
static void mcc_bsp_release_claims(struct mcc_runtime *runtime, void const *address, unsigned long size)
{
    CHECK(runtime==&mcc_loaded_cache&&size==sizeof(mcc_regions[0])&&
          (address==mcc_regions[0].header||address==mcc_regions[1].header||address==mcc_regions[2].header||address==mcc_regions[3].header),
          "released claims outside the cache window");
}
static boolean cache_file_structure_bsp_tag_valid(struct scenario_structure_bsp_reference const *ref)
{return !reject_reference[ref->structure_bsp.index];}
#define cache_file_structure_bsp_reference_verify cache_file_structure_bsp_tag_valid
#define NUMBEROF(array) ((int)(sizeof(array)/sizeof((array)[0])))
#include "mcc_bsp.inc"
#undef NUMBEROF
static boolean scenario_structure_bsp_load(struct scenario_structure_bsp_reference *ref)
{loads[ref->structure_bsp.index]++;return !reject_reference[ref->structure_bsp.index] && !reject_load[ref->structure_bsp.index];}
static void scenario_structure_bsp_unload(struct scenario_structure_bsp_reference *ref)
{
    long index=ref->structure_bsp.index;

    unloads[index]++;
    if (mcc_loaded)
    {
        cache_files_unbind_mcc_bsp(ref);
        mcc_cache_bsp_unload(ref);
    }
}
static struct structure_bsp *structure_bsp_definition_get(long index)
{
    return mcc_prepare_fixture && test_tag_instances[index].base_address ?
        test_tag_instances[index].base_address :
        (mcc_prepare_fixture ? &mcc_regions[index].structure : &structures[index]);
}
#define BIT_VECTOR_SIZE_IN_BYTES(count) (4u * (((count) + 31u) / 32u))
#include "bsp_limits.inc"
static boolean structure_bsp_port_verify(struct structure_bsp const *bsp)
{
    if (bsp==&structures[0]) return !reject_verify[0];
    if (bsp==&structures[1]) return !reject_verify[1];
    for (short i=0;i<4;i++)
        if (bsp==&mcc_regions[i].structure) return !reject_verify[i];
    return native_bsp_limits(bsp);
}
static void main_goto_main_menu(void) {menu++;}
static void structure_culling_load_settings(void) {culling++;}
static void structure_culling_structure_bsp_loaded(void) {culling++;}
static short object_partition_bsp = NONE;
static void objects_bind_cluster_partition_bsp(short bsp) {object_partition_bsp=bsp;}
#include "loader.inc"

static long cluster_partition_port_no_cluster_first_reference;
static long reference_list_get_next_datum_index(struct data_array *data,long *reference_index)
{
    if (*reference_index==NONE) return NONE;
    *reference_index=NONE;
    return *(long *)data->data;
}
#include "object_cluster_keys.inc"

static struct {short switch_to_structure_bsp_index;} main_globals={NONE};
static short distributed_host_structure_bsp_index = NONE;
static long distributed_host_structure_bsp_start = NONE;
static short global_structure_bsp_index_get(void) {return global_structure_bsp_index;}
static real distance_squared3d(real_point3d const *a, real_point3d const *b)
{
    real x=a->x-b->x,y=a->y-b->y,z=a->z-b->z;
    return x*x+y*y+z*z;
}
#include "netgame_flags.inc"
#include "regional_start.inc"
struct distributed_structure_bsp {short structure_bsp_index,pad;};
static boolean loading_hud;
static void hud_load(boolean value) {loading_hud=value;}
#define console_warning(...) ((void)0)
typedef unsigned short word;
static short scripted_multiplayer_bsp = NONE;
static boolean scripted_multiplayer_bsp_reported;
static boolean hs_menu_script_running;
static struct hs_arguments_word {word value;} bsp_argument;
static boolean game_engine_running(void) {return TRUE;}
static void const *hs_macro_function_evaluate(short function_index, long thread_index, boolean initialize)
{
    return &bsp_argument;
}
static void hs_return(long thread_index, long value) {CHECK(!value, "invalid switch_bsp result");}
#include "scripted_bsp.inc"
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
struct client_actor {
    long unit_index, speech_sound, user_animation_graph;
    short impulse;
    byte played_impulse_number;
    boolean driven, control_invalidated;
};
static struct client_actor client_actors[1];
static short client_actor_count;
static int actor_controls_released, actor_snapshots_applied;
#define _object_mask_unit 1
static void *object_try_and_get_and_verify_type(long object_index, unsigned long types)
{
    if (types==_object_mask_vehicle)
        return object_index==units[2]||object_index==units[3]?object_try_and_get(object_index):NULL;
    CHECK(types==_object_mask_unit,"unexpected actor release type");
    return object_try_and_get(object_index);
}
static void unit_set_actively_controlled(long object_index, boolean controlled)
{
    CHECK(object_index==units[0]&&!controlled,"region switch released the wrong actor");
    actor_controls_released++;
}
static void network_actors_handle_states(void const *entries, short count)
{
    CHECK(entries&&count==1,"invalid actor snapshot");
    actor_snapshots_applied++;
}
#include "bsp_actor_controls.inc"

#define MAXIMUM_TRACKED_OBJECTS 4
#define MAXIMUM_LOCAL_PLAYERS 2
#define OWN_VEHICLE_POSITION_TICKS 3
static struct {long object_index,time,interval;} objects_client_unsteered_path[MAXIMUM_TRACKED_OBJECTS];
static struct {long time,vehicle_index;} objects_client_own_vehicles[MAXIMUM_LOCAL_PLAYERS][OWN_VEHICLE_POSITION_TICKS];
static short host_player_structure_bsps[2]={NONE,NONE};
static struct {boolean scene_refresh,refresh;} host_resend;
#define NUMBEROF(array) (sizeof(array) / sizeof(array[0]))
#define VALID_INDEX(index, count) (index>=0 && index<count)
#include "bsp_world_state.inc"

enum {_coop_event_surface_broken, MAXIMUM_QUEUED_EVENTS=128};
static struct {short count;} coop_events;
struct damage_data {real_point3d epicenter; struct location location;};
static short surface_events, surface_effects;
static boolean warned_about_breakable_surfaces;
static void scenario_location_from_point(struct location *location, real_point3d const *point)
{
    (void)point;
    *location = (struct location){0,0,NONE};
}
static void breakable_surface_effect(short index, struct damage_data *damage, long collision_surface)
{
    CHECK(index==0&&damage->epicenter.x==20.f&&collision_surface==8,"effect used a foreign BSP");
    surface_effects++;
}
#define csmemset memset
#define BIT_VECTOR_SET_FLAG(bits,index,value) SET_FLAG((bits)[(index)/32],(index)%32,value)
#include "surface_breaks.inc"

#include <float.h>
#define REAL_MAX FLT_MAX
#define HALO_PERF_BEGIN(section) ((void)0)
#define HALO_PERF_END(section) ((void)0)
enum {_collision_function_vector_structure};
static struct {struct {long QuadPart;} vector_structure;} collision_usage_times;
static void collision_log_usage(int function) {}
#define collision_log_start_time(state) ((void)(state))
#define collision_log_end_time(function, elapsed) ((void)(elapsed))
struct collision_bsp_test_vector_result {int unused;};
struct collision_result {int unused;};
static boolean debug_collision_skip_vectors;
static int terrain_traces, legacy_traces;
static boolean collision_bsp_test_vector(unsigned long flags, struct collision_bsp const *bsp,
    short surface_count, byte const *surface_flags, real_point3d const *from,
    real_vector3d const *vector, real maximum_t, struct collision_bsp_test_vector_result *result)
{
    short region=bsp==&collisions[0]?0:1;
    CHECK(bsp==&collisions[region]&&surface_count==MAXIMUM_BREAKABLE_SURFACES_PER_MAP&&
          surface_flags==surface_state.breakable_surface_flags[region],
          "terrain and fracture state did not both belong to the requested BSP");
    CHECK((flags&FLAG(_collision_test_front_facing_surfaces_bit))&&
          !(flags&FLAG(_collision_test_objects_scenery_bit))&&
          from->x==0.0f&&vector->i==4.0f&&maximum_t==REAL_MAX,
          "the trace did not retain native structure-only flags or endpoints");
    terrain_traces++;
    return region==0 && TEST_FLAG(surface_flags[0],0);
}
static boolean collision_test_line(unsigned long flags,real_point3d const *from,real_point3d const *to,
    long ignored,struct collision_result *result)
{
    legacy_traces++;
    return FALSE;
}
#include "bsp_terrain.inc"

enum {
    _distributed_message_object_states,
    _distributed_message_coop_object_transforms,
    _distributed_message_coop_device_states
};
static unsigned received_scene_types;
static void network_objects_handle_states(void const *entries,short count,long host_time)
{
    CHECK(entries&&count==1&&host_time>=90,"invalid object state");
    received_scene_types|=1;
}
static void network_coop_handle_object_transforms(void const *entries,short count)
{
    CHECK(entries&&count==1,"invalid scene transform");
    received_scene_types|=2;
}
static void network_coop_handle_device_states(void const *entries,short count)
{
    CHECK(entries&&count==1,"invalid device state");
    received_scene_types|=4;
}

enum {_distributed_message_structure_bsp=10, _distributed_to_clients, _distributed_to_clients_reliably};
struct distributed_message_header {long game_time;};
static short distributed_sent_structure_bsp_index=NONE;
static int bsp_announcement_targeted,bsp_announcement_broadcast;
static struct player_datum *distributed_player(short slot)
{return slot==0?&people[0]:NULL;}
static boolean distributed_machine_has_player(long machine, short slot)
{return machine==7&&slot==0;}
static boolean network_coop_player_has_structure_bsp(long player);
static boolean check_report_region;
static void distributed_send_to_machine_reliably(long machine,void const *message,byte type,word count,word size)
{
    CHECK(machine==7&&message&&type==_distributed_message_structure_bsp&&count==1&&size,
          "invalid targeted BSP message");
    bsp_announcement_targeted++;
}
static void distributed_send(void *message, byte type, word count, word size, int delivery)
{
    CHECK(message&&type==_distributed_message_structure_bsp&&count==1&&size,
          "invalid broadcast BSP message");
    bsp_announcement_broadcast++;
}
#define TARGET_HISTORY_TICKS 4
#define MAXIMUM_TRACKED_PLAYERS 2
#define MAXIMUM_SIGHT_POINTS 2
#define PLAYER_TRAIL_POINTS 4
#define PLAYER_TRAIL_INTERVAL_TICKS 10

void players_coop_finish_bsp_switch(short from, short to, boolean loaded);
#include "bsp_receive.inc"
#undef DATUM_INDEX_NEW
#include "bsp_damage_history.inc"

static boolean players_coop_queue_bsp_switch(long unit, short trigger, short destination)
{
    return unit == units[0] && trigger >= 0 && main_switch_structure_bsp(destination);
}

static boolean preview_available;
static int preview_requests;
static struct structure_bsp const *scenario_structure_bsp_preview(short index, boolean prefetch)
{
    CHECK(prefetch&&index==1,"incorrect destination preview");
    preview_requests++;
    return preview_available?&structures[index]:NULL;
}
#include "queue_regional_bsp_switch.inc"
#include "queue_bsp_switch.inc"

static long vote_excluded_player = NONE, vote_crossers, voted_players[2];
static short vote_source, vote_destination;
static int vote_announced, vote_waiting;
static long vote_waiting_player;
static boolean network_coop_player_has_structure_bsp(long player)
{
    if (!check_report_region)
        return TRUE;
    CHECK(player==players[0],"hit report lost the current player identity");
    return host_player_structure_bsps[0]==global_structure_bsp_index;
}
static boolean network_coop_player_away(long player) {return player == vote_excluded_player;}
static void network_coop_tell_bsp_switch_waiting(long player, long more, long in, long total)
{
    CHECK(more == 1 && in == 1 && total == 2, "invalid waiting count");
    vote_waiting_player = player;
    vote_waiting++;
}
static void network_coop_note_bsp_switch(short from, short to, long const *crossers, long count)
{
    CHECK(to == global_structure_bsp_index && count > 0 && count <= 2, "invalid route announcement");
    vote_announced++;
    vote_source = from;
    vote_destination = to;
    vote_crossers = count;
    memcpy(voted_players, crossers, (size_t)count * sizeof(long));
}
#include "majority_bsp_vote.inc"

enum {MAXIMUM_SCENARIO_NETGAME_EQUIPMENT_PER_SCENARIO=200, _equipment_created_at_rest_bit=0,
      _item_attached_to_unit_bit=2, _object_type_weapon=1, pickup_handle=0x40000};
struct scenario_netgame_equipment {
    unsigned long flags;
    short game_type[4],spawn_time;
    long run_time_spawned_item_index;
    real_point3d position;
    real facing;
    struct {long index;} item_collection;
};
struct item_collection_definition {short spawn_time;};
struct object_placement_data {real_point3d position;real_vector3d forward;};
static struct {struct {short type;} object;} test_equipment_definition;
static struct {boolean no_map_weapons;} test_equipment_options;
static struct item_datum placed_pickups[8];
static int placed_pickup_count;
static boolean test_equipment_client;
static unsigned long game_engine_equipment_regions_seen;
static short game_engine_equipment_home_bsp[MCC_SCENARIO_NETGAME_EQUIPMENT_MAXIMUM];
static boolean network_game_distributed_client(void) {return test_equipment_client;}
static long game_engine_get_type(void) {return 2;}
static boolean match_game_type(long type,long count,short const *kinds) {return count>0&&type==kinds[0];}
static struct item_collection_definition *item_collection_definition_get(long index)
{static struct item_collection_definition collection={1};return &collection;}
static long random_item(long index) {return index;}
static void object_placement_data_new(struct object_placement_data *placement,long definition,long owner)
{memset(placement,0,sizeof(*placement));}
static void vector3d_from_angle(real_vector3d *forward,real angle) {}
static long object_new(struct object_placement_data const *placement)
{
    long index=placed_pickup_count++;
    CHECK(VALID_INDEX(index,NUMBEROF(placed_pickups)),"too many pickups");
    placed_pickups[index].object.position=placement->position;
    placed_pickups[index].object.parent_object_index=NONE;
    placed_pickups[index].item.last_owned_time=ticks;
    placed_pickups[index].item.netgame_equipment_index=NONE;
    return pickup_handle+index;
}
static struct item_datum *item_try_and_get(long index)
{return index>=pickup_handle&&index-pickup_handle<placed_pickup_count?&placed_pickups[index-pickup_handle]:NULL;}
#define item_get(index) item_try_and_get(index)
#define object_definition_get(index) (&test_equipment_definition)
#define game_variant_options_get() (&test_equipment_options)
static void object_set_garbage(long index,boolean value)
{CHECK(!value&&item_try_and_get(index),"unexpected pickup garbage assignment");}
static struct game_engine_regional_object {
    long object_index;
    short bsp_index;
} game_engine_regional_objects[8];
#define _object_mask_item FLAG(_object_type_weapon)
static struct object_datum *unit_get(long index) {return object_get(index);}
static boolean game_engine_vehicle_parked(long index) {return FALSE;}
static long parked_ai_unit=NONE;
static boolean ai_unit_belongs_to_other_bsp(long index) {return index==parked_ai_unit;}
#include "regional_equipment.inc"

static boolean scenario_trigger_volume_test_point(short volume, real_point3d const *point)
{
    return volume==0 && point->x>0.f && point->x<2.f;
}
#define scenario_trigger_volume_test_object native_trigger_volume_test_object
#include "regional_trigger.inc"
#undef scenario_trigger_volume_test_object

struct device_datum { struct { short type; } object; };
static struct device_datum *device_get(long index)
{
    static struct device_datum device;
    device.object.type=object_get(index)->object.type;
    return &device;
}
static int regional_machine_touches, regional_control_touches;
static void machine_bumped(long device,long unit) {regional_machine_touches++;}
static void control_touched(long device,long unit) {regional_control_touches++;}
#include "device_touch.inc"

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
        people[i]=(struct player_datum){units[i],8+i,i?NONE:0,0};
    }
    for(int i=0;i<4;i++)
    {
        objects[i].object.parent_object_index=NONE;
        objects[i].object.type=i<2?_object_type_biped:_object_type_vehicle;
        objects[i].object.position=(real_point3d){10+i,5+i,1};
        objects[i].object.bounding_sphere_center=objects[i].object.position;
        objects[i].weapon=0x70007+i;
        objects[i].ammunition=30+i;
        objects[i].unit.player_index=i<2?players[i]:NONE;
        objects[i].unit.parent_seat_index=NONE;
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
    boolean extra_below=!strcmp(name,"mcc-prepare-space-below");
    boolean relocated=!strcmp(name,"mcc-prepare-relocated")||extra_below;
    short old=relocated||!strcmp(name,"reverse")||!strcmp(name,"reverse-refused")||!strcmp(name,"connected-reverse")?1:0,destination=1-old;
    struct object_datum before[4];
    setup(old,destination);
    if (!strcmp(name, "qualified-object-clusters"))
    {
        long head=7, cursor=NONE;
        long entry=units[0], reverse_key=NONE;
        struct data_array entries={.data=(unsigned char *)&entry}, reverse={.data=(unsigned char *)&reverse_key};
        struct cluster_partition partition={
            .cluster_first_data_references=&head,
            .data_reference_data=&entries,
            .cluster_reference_data=&reverse,
            .port_structure_bsp_index=NONE,
        };
        global_structure_bsp->clusters.count=1;
        CHECK(cluster_partition_key(&partition,0)==0&&
              cluster_partition_get_first_datum(&partition,&cursor,0)==units[0],
              "classic cluster key or traversal changed");
        partition.port_structure_bsp_index=old;
        reverse_key=cluster_partition_key(&partition,0);
        CHECK(reverse_key==MAXIMUM_CLUSTERS_PER_STRUCTURE&&
              cluster_partition_get_first_cluster(&partition,&cursor,head)==0,
              "native membership did not store a qualified key or expose the local cluster");
        CHECK(cluster_partition_get_first_datum_in_bsp(&partition,&cursor,destination,0)==NONE&&cursor==NONE&&
              cluster_partition_get_first_datum_in_bsp(&partition,&cursor,old,0)==units[0],
              "equal cluster numbers made two BSP lookups alias");

        global_structure_bsp_index=destination;
        partition.port_structure_bsp_index=destination;
        entry=units[1];
        CHECK(cluster_partition_key(&partition,0)!=reverse_key&&
              cluster_partition_get_first_cluster(&partition,&cursor,head)==NONE&&cursor==NONE,
              "a reverse reference exposed a cluster from the previous BSP");
        CHECK(cluster_partition_get_first_datum_in_bsp(&partition,&cursor,old,0)==NONE&&
              cluster_partition_get_first_datum(&partition,&cursor,0)==units[1],
              "binding the next region changed the qualified lookup contract");
        reverse_key=0;
        CHECK(cluster_partition_get_first_cluster(&partition,&cursor,head)==0,
              "an older raw cluster key could not be read after restore");
        return 0;
    }
    if (!strcmp(name, "regional-hit-boundary"))
    {
        struct damage_history_tick *original=&damage_history[10 & (TARGET_HISTORY_TICKS - 1)];
        struct damage_trail_point *old_trail=&damage_trails[0][1];
        struct damage_trail_point *new_trail=&damage_trails[0][110 / PLAYER_TRAIL_INTERVAL_TICKS & (PLAYER_TRAIL_POINTS - 1)];

        regional_match=regional_room_ready=check_report_region=TRUE;
        people[0].identifier=(unsigned long)players[0]>>16;
        ticks=10;
        original->time=old_trail->time=ticks;
        original->structure_bsp_index=old_trail->structure_bsp_index=old;
        original->sight[0].count=old_trail->sight.count=1;
        distributed_send_structure_bsp(NONE);
        CHECK(distributed_hit_report_matches_structure_bsp(7,0,10)&&
              distributed_sight_at(0,10)==&original->sight[0],
              "initial BSP combat was refused");

        global_structure_bsp_index=destination;
        ticks=100;
        CHECK(!distributed_hit_report_matches_structure_bsp(7,0,99),
              "report crossed ahead of the host announcement");
        distributed_send_structure_bsp(7);
        CHECK(bsp_announcement_targeted==1&&!distributed_hit_report_matches_structure_bsp(7,0,101),
              "a late join request advanced the room's combat boundary");
        distributed_send_structure_bsp(NONE);
        CHECK(original->time==10&&old_trail->time==10&&!distributed_sight_at(0,10),
              "crossing destroyed the retained history or used sight from the previous BSP");
        CHECK(!distributed_hit_report_matches_structure_bsp(7,0,101),
              "report was accepted before the shooter loaded the BSP");

        network_coop_note_player_structure_bsp(0,destination);
        CHECK(!distributed_hit_report_matches_structure_bsp(7,0,99)&&
              !distributed_hit_report_matches_structure_bsp(7,0,100)&&
              distributed_hit_report_matches_structure_bsp(7,0,101),
              "combat resumed with a hit from the previous BSP or the ambiguous switch tick");
        CHECK(!distributed_combat_sample_matches_bsp(destination,100)&&
              !distributed_combat_sample_matches_bsp(old,101)&&
              distributed_combat_sample_matches_bsp(destination,101),
              "the rewind boundary ignored the sample's tick or BSP");

        ticks=111;
        new_trail->time=110;
        new_trail->structure_bsp_index=destination;
        new_trail->sight.count=1;
        distributed_send_structure_bsp(NONE);
        CHECK(distributed_hit_report_matches_structure_bsp(7,0,101)&&
              distributed_sight_at(0,111)==&new_trail->sight,
              "a periodic announcement moved the combat boundary or blocked fresh sight history");

        global_structure_bsp_index=old;
        ticks=120;
        distributed_send_structure_bsp(NONE);
        network_coop_note_player_structure_bsp(0,old);
        CHECK(bsp_announcement_broadcast==4&&!distributed_hit_report_matches_structure_bsp(7,0,119)&&
              distributed_hit_report_matches_structure_bsp(7,0,121),
              "return to the same BSP did not establish a new combat boundary");
        CHECK(original->time==10&&new_trail->time==110&&
              !distributed_sight_at(0,10)&&!distributed_sight_at(0,111)&&
              distributed_combat_sample_matches_bsp(old,121),
              "returning to the original BSP restored an earlier visit or dropped stored history");

        regional_room_ready=FALSE;
        CHECK(distributed_hit_report_matches_structure_bsp(7,0,110),
              "legacy report behavior changed without negotiation");
        regional_match=FALSE;
        global_structure_bsp_index=destination;
        distributed_send_structure_bsp(NONE);
        CHECK(distributed_sight_at(0,10)==&original->sight[0]&&
              distributed_sight_at(0,111)==&new_trail->sight,
              "classic co-op unexpectedly filtered spatial history");
        return 0;
    }
    if (!strcmp(name, "regional-devices"))
    {
        long machine=units[2], control=units[3], scenery=units[1];
        struct object_datum *door=&objects[2], *button=&objects[3], *barrier=&objects[1];
        memset(game_engine_regional_objects,NONE,sizeof(game_engine_regional_objects));
        scenario.type=_scenario_type_multiplayer;
        scenario.structure_bsp_references.count=2;
        door->object.type=_object_type_machine;
        button->object.type=_object_type_control;
        barrier->object.type=_object_type_scenery;
        door->object.parent_object_index=button->object.parent_object_index=barrier->object.parent_object_index=NONE;
        door->object.flags=button->object.flags=barrier->object.flags=FLAG(_object_connected_to_map_bit);
        door->object.location.cluster_index=button->object.location.cluster_index=barrier->object.location.cluster_index=0;

        objects_disconnect_from_structure_bsp();
        global_structure_bsp_index=1;
        CHECK(!game_engine_object_parked(machine,_object_type_machine)&&
              !game_engine_object_parked(scenery,_object_type_scenery),"legacy static object was parked");

        global_structure_bsp_index=0;
        mcc_loaded=TRUE;
        test_equipment_client=TRUE;
        objects_disconnect_from_structure_bsp();
        global_structure_bsp_index=1;
        CHECK(game_engine_object_parked(machine,_object_type_machine)&&
              game_engine_object_parked(control,_object_type_control),
              "a peer moved its native devices into a different BSP");
        CHECK(game_engine_object_parked(scenery,_object_type_scenery),
              "a peer moved its native scenery into a different BSP");
        device_touched(machine,units[0]);
        device_touched(control,units[0]);
        CHECK(!regional_machine_touches&&!regional_control_touches,
              "touch in foreign geometry reached a parked device");

        global_structure_bsp_index=0;
        CHECK(!game_engine_object_parked(machine,_object_type_machine)&&
              !game_engine_object_parked(control,_object_type_control),
              "the device was not returned to its home BSP");
        CHECK(!game_engine_object_parked(scenery,_object_type_scenery),
              "the scenery was not returned to its home BSP");
        device_touched(machine,units[0]);
        device_touched(control,units[0]);
        CHECK(regional_machine_touches==1&&regional_control_touches==1,
              "the same devices did not receive native actions after returning");
        return 0;
    }
    if (!strcmp(name, "regional-environment"))
    {
        struct object_datum *emitter=&objects[2], *fixture=&objects[3];
        memset(game_engine_regional_objects,NONE,sizeof(game_engine_regional_objects));
        scenario.type=_scenario_type_multiplayer;
        scenario.structure_bsp_references.count=2;
        mcc_loaded=TRUE;
        test_equipment_client=TRUE;
        emitter->object.type=_object_type_sound_scenery;
        fixture->object.type=_object_type_light_fixture;
        emitter->object.parent_object_index=fixture->object.parent_object_index=NONE;
        emitter->object.flags=fixture->object.flags=FLAG(_object_connected_to_map_bit);
        emitter->object.location.cluster_index=fixture->object.location.cluster_index=0;

        objects_disconnect_from_structure_bsp();
        global_structure_bsp_index=1;
        CHECK(game_engine_object_parked(units[2],_object_type_sound_scenery),
              "an ambient sound emitter followed the client into an overlapping BSP");
        CHECK(game_engine_object_parked(units[3],_object_type_light_fixture),
              "a light fixture followed the client into an overlapping BSP");

        global_structure_bsp_index=0;
        CHECK(!game_engine_object_parked(units[2],_object_type_sound_scenery)&&
              !game_engine_object_parked(units[3],_object_type_light_fixture),
              "environment objects failed to resume in their recorded BSP");
        return 0;
    }
    if (!strcmp(name, "regional-bodies"))
    {
        long handle=units[2];
        struct object_datum *body=&objects[2];

        memset(game_engine_regional_objects,NONE,sizeof(game_engine_regional_objects));
        body->object.type=_object_type_biped;
        body->object.flags=FLAG(_object_connected_to_map_bit);
        body->object.parent_object_index=NONE;
        body->object.location.cluster_index=0;
        test_equipment_client=TRUE;

        game_engine_record_regional_object(handle,body,2);
        CHECK(game_engine_object_recorded_bsp(handle,_object_type_biped)==NONE,
              "a live unit was pinned to the scene it crossed from");
        body->object.damage_flags=FLAG(_object_dead_bit);
        game_engine_record_regional_object(handle,body,2);

        global_structure_bsp_index=1;
        CHECK(game_engine_object_parked(handle,_object_type_biped),
              "a corpse reconnected in overlapping geometry");
        body->object.flags|=FLAG(_object_outside_of_map_bit);
        game_engine_record_regional_object(handle,body,1);
        CHECK(game_engine_object_recorded_bsp(handle,_object_type_biped)==2,
              "the inactive scene replaced corpse ownership");

        global_structure_bsp_index=2;
        CHECK(!game_engine_object_parked(handle,_object_type_biped),
              "the corpse did not return to its home scene");

        global_structure_bsp_index=1;
        body->object.parent_object_index=units[0];
        CHECK(!game_engine_object_parked(handle,_object_type_biped),
              "a carried corpse used its old scene instead of its parent's");
        body->object.parent_object_index=NONE;
        body->object.damage_flags=0;
        CHECK(!game_engine_object_parked(handle,_object_type_biped),
              "revival inherited the corpse's region");
        parked_ai_unit=handle;
        CHECK(object_simulation_is_parked(handle,body),
              "a disconnected encounter actor still runs physics in the active BSP");
        body->object.parent_object_index=units[0];
        CHECK(!object_simulation_is_parked(handle,body),
              "a seated actor lost its native parent update");
        body->object.parent_object_index=NONE;
        parked_ai_unit=NONE;
        CHECK(!object_simulation_is_parked(handle,body),
              "an active or unbound actor cannot resume native object updates");
        return 0;
    }
    if (!strcmp(name, "regional-local-room"))
    {
        CHECK(!network_regional_bsp_room_ready(),"an unnegotiated remote room enabled regional play");
        splitscreen_local=TRUE;
        CHECK(network_regional_bsp_room_ready(),"local split screen required a Delta room");
        splitscreen_local=FALSE;
        regional_room_ready=TRUE;
        CHECK(network_regional_bsp_room_ready(),"a negotiated remote room lost regional play");
        return 0;
    }
    if (!strcmp(name, "regional-preview-admission"))
    {
        regional_match=TRUE;
        main_globals.switch_to_structure_bsp_index=NONE;
        CHECK(!players_queue_regional_bsp_switch(&switches[0])&&!preview_requests,
              "the host prepared a marked transition");
        switches[0].cutscene_flag_index=NONE;
        CHECK(!players_queue_regional_bsp_switch(&switches[0])&&!preview_requests,
              "an unnegotiated room prepared a neighbor");

        regional_room_ready=TRUE;
        objects[0].object.parent_object_index=NONE;
        objects[1].object.parent_object_index=NONE;
        prepared_crossing_leaf=NONE;
        preview_available=TRUE;
        CHECK(!players_queue_regional_bsp_switch(&switches[0])&&preview_requests==1,
              "first arrival did not warm the neighbor or forced the scene to switch");
        CHECK(main_globals.switch_to_structure_bsp_index==NONE,
              "the host queued a switch with a living player still away");

        connected=TRUE;
        CHECK(!players_queue_regional_bsp_switch(&switches[0])&&preview_requests==2,
              "a resident destination with no valid crossing leaf was queued");
        CHECK(main_globals.switch_to_structure_bsp_index==NONE,
              "an obstructed resident destination was allowed to switch");

        preview_available=FALSE;
        CHECK(players_queue_regional_bsp_switch(&switches[0])&&preview_requests==3,
              "memory pressure prevented use of the ordinary cold-switch path");
        CHECK(main_globals.switch_to_structure_bsp_index==destination,
              "the valid cold crossing was not queued");

        connection=_game_connection_local;
        coop=regional_match=regional_room_ready=FALSE;
        mcc_loaded=TRUE;
        scenario.type=_scenario_type_multiplayer;
        struct mcc_cache_bsp checked={.file_offset=4096,.size=1024,.address=0x40001000,
            .tag_handle=(uint32_t)destination,.structure_address=0x40001018};
        struct scenario_structure_bsp_reference *ref=&references[destination];
        mcc_loaded_cache.report.bsp_count=1;
        mcc_loaded_cache.report.bsps=&checked;
        ref->file_offset=checked.file_offset;
        ref->file_size=checked.size;
        ref->base_address=checked.address;
        mcc_file_structure.collision_bsp.count=1;
        mcc_file_structure.clusters.count=2;
        mcc_file_structure.cluster_data.size=8;
        mcc_file_structure.sound_cluster_data.size=1;
        main_globals.switch_to_structure_bsp_index=NONE;
        preview_available=TRUE;
        player_queue_structure_bsp_switch(&people[0], units[0]);
        CHECK(main_globals.switch_to_structure_bsp_index==NONE&&preview_requests==4,
              "offline play entered the retained BSP without a valid destination leaf");
        preview_available=FALSE;
        player_queue_structure_bsp_switch(&people[0], units[0]);
        CHECK(main_globals.switch_to_structure_bsp_index==destination&&preview_requests==5,
              "an offline cold crossing incorrectly required a network capability");
        return 0;
    }
    if (!strcmp(name, "regional-projectiles"))
    {
        struct object_datum *round=&objects[2], *parked=&objects[3];
        struct location destination_location={0,1,0};
        struct location obstructed={NONE,NONE,0};
        memset(game_engine_regional_objects,NONE,sizeof(game_engine_regional_objects));
        round->object.type=parked->object.type=_object_type_projectile;
        round->object.flags=parked->object.flags=FLAG(_object_connected_to_map_bit);
        round->object.location.cluster_index=parked->object.location.cluster_index=0;
        round->object.bounding_sphere_center.x=1.f;
        parked->object.bounding_sphere_center.x=3.f;
        parked->object.flags|=FLAG(_object_outside_of_map_bit);
        parked->object.location.cluster_index=NONE;
        game_engine_set_regional_projectile_bsp(units[3],old);
        scenario.type=_scenario_type_multiplayer;
        mcc_loaded=TRUE;
        global_structure_bsp_index=old;
        objects_disconnect_from_structure_bsp();
        CHECK(object_disconnected_bsp==old &&
              game_engine_object_recorded_bsp(units[2],_object_type_projectile)==old &&
              game_engine_object_recorded_bsp(units[3],_object_type_projectile)==old,
              "disconnect failed to record projectile ownership");
        global_structure_bsp_index=destination;

        object_transfer_regional_projectile(units[2],round,object_disconnected_bsp,destination,&destination_location);
        CHECK(game_engine_object_parked(units[2],_object_type_projectile),
              "a marked player transition moved a projectile");
        switches[0].cutscene_flag_index=NONE;
        object_transfer_regional_projectile(units[2],round,old,destination,&obstructed);
        CHECK(game_engine_object_parked(units[2],_object_type_projectile),
              "a projectile crossed without a destination leaf");
        /* The swept round has left the source leaf, but its center and the
         * destination leaf still meet at this authored seam. */
        round->object.flags|=FLAG(_object_outside_of_map_bit);
        round->object.location.cluster_index=NONE;
        object_transfer_regional_projectile(units[2],round,old,destination,&destination_location);
        CHECK(game_engine_object_recorded_bsp(units[2],_object_type_projectile)==destination,
              "a projectile in the native continuous seam lost its destination ownership");

        object_transfer_regional_projectile(units[3],parked,old,destination,&destination_location);
        parked->object.flags|=FLAG(_object_outside_of_map_bit)|FLAG(_object_deleted_when_deactivated_bit);
        CHECK(object_simulation_is_parked(units[3],parked),
              "an off-route projectile resumed in overlapping destination geometry");
        activation_headers[3].identifier=(short)(units[3]>>16);
        activation_headers[3].cluster_index=NONE;
        activation_headers[3].datum=parked;
        activation_headers[3].flags=FLAG(_object_header_automatically_deactivate_bit)|FLAG(_object_header_connected_to_map_bit);
        global_structure_bsp=&structures[0];
        structures[0].clusters.count=2;
        players_globals->combined_pvs[0]=2;
        object_bsp_activation_pending=TRUE;
        objects_update_cluster_activation();
        CHECK(!deletions&&!activations,"destination PVS expired a parked projectile");
        global_structure_bsp_index=old;
        activation_headers[3].cluster_index=1;
        object_bsp_activation_pending=TRUE;
        objects_update_cluster_activation();
        CHECK(!object_simulation_is_parked(units[3],parked)&&activations==FLAG(3)&&!deletions,
              "a parked projectile did not resume in its source region");
        return 0;
    }
    if (!strcmp(name, "regional-triggers"))
    {
        struct object_datum *actor=&objects[2], *carried=&objects[3];
        mcc_loaded=TRUE;
        scenario.type=_scenario_type_multiplayer;
        scenario.structure_bsp_references.count=2;
        global_structure_bsp_index=0;
        actor->object.type=_object_type_biped;
        actor->object.parent_object_index=NONE;
        actor->object.bounding_sphere_center.x=1.f;
        carried->object.type=_object_type_weapon;
        carried->object.parent_object_index=units[2];
        carried->object.bounding_sphere_center.x=1.f;

        CHECK(native_trigger_volume_test_object(0,units[2])&&
              native_trigger_volume_test_object(0,units[3]), "active objects did not enter their native volume");
        parked_ai_unit=units[2];
        CHECK(!native_trigger_volume_test_object(0,units[2])&&
              !native_trigger_volume_test_object(0,units[3]), "an inactive encounter or its carried equipment tripped the active trigger");
        mcc_loaded=FALSE;
        CHECK(native_trigger_volume_test_object(0,units[2])&&
              native_trigger_volume_test_object(0,units[3]), "classic trigger membership changed");
        CHECK(!native_trigger_volume_test_object(0,NONE), "an absent script object tripped a trigger");
        return 0;
    }
    if (!strcmp(name, "regional-loose-objects"))
    {
        struct object_datum item={.object={
            .type=_object_type_weapon,.parent_object_index=NONE,.flags=FLAG(_object_connected_to_map_bit),
            .location={.cluster_index=0},
        }};
        struct object_datum *vehicle=&objects[2];
        long index=pickup_handle;
        long vehicle_index=units[2];

        memset(game_engine_regional_objects,NONE,sizeof(game_engine_regional_objects));
        placed_pickup_count=1;
        placed_pickups[0].item.netgame_equipment_index=NONE;
        vehicle->object.type=_object_type_vehicle;
        vehicle->object.parent_object_index=NONE;
        vehicle->object.first_child_object_index=NONE;
        vehicle->object.flags=FLAG(_object_connected_to_map_bit);
        vehicle->object.location.cluster_index=0;
        vehicle->unit.driver_object_index=NONE;
        vehicle->unit.gunner_object_index=NONE;

        game_engine_record_regional_object(index,&item,0);
        game_engine_record_regional_object(vehicle_index,vehicle,0);
        global_structure_bsp_index=1;
        CHECK(game_engine_object_parked(index,_object_type_weapon)&&
              game_engine_object_parked(vehicle_index,_object_type_vehicle),
              "loose objects in overlapping geometry moved to the destination");

        item.object.flags|=FLAG(_object_outside_of_map_bit);
        vehicle->object.flags|=FLAG(_object_outside_of_map_bit);
        CHECK(object_simulation_is_parked(index,&item)&&
              object_simulation_is_parked(vehicle_index,vehicle),
              "parked equipment or vehicle could run physics in the destination BSP");
        item.object.parent_object_index=units[0];
        CHECK(!object_simulation_is_parked(index,&item),"attached equipment cannot follow its parent");
        item.object.parent_object_index=NONE;
        game_engine_record_regional_object(index,&item,1);
        game_engine_record_regional_object(vehicle_index,vehicle,1);
        CHECK(game_engine_object_parked(index,_object_type_weapon)&&
              game_engine_object_parked(vehicle_index,_object_type_vehicle),
              "an inactive BSP replaced an object's owner");
        CHECK(!game_engine_object_parked(vehicle_index+0x10000,_object_type_vehicle),
              "a recycled datum inherited the previous vehicle's region");

        global_structure_bsp_index=0;
        CHECK(!game_engine_object_parked(index,_object_type_weapon)&&
              !game_engine_object_parked(vehicle_index,_object_type_vehicle),
              "returning to the original BSP did not restore its objects");
        CHECK(!object_simulation_is_parked(index,&item)&&
              !object_simulation_is_parked(vehicle_index,vehicle),
              "returning to the original BSP still suppressed physics");

        vehicle->object.flags&=~FLAG(_object_outside_of_map_bit);
        vehicle->object.first_child_object_index=units[3];
        vehicle->unit.driver_object_index=units[3];
        objects[3].object.type=_object_type_biped;
        objects[3].object.parent_object_index=vehicle_index;
        objects[3].object.next_object_index=NONE;
        objects[3].unit.parent_seat_index=0;
        objects[3].unit.player_index=NONE;
        game_engine_record_regional_object(vehicle_index,vehicle,0);
        global_structure_bsp_index=1;
        CHECK(game_engine_object_parked(vehicle_index,_object_type_vehicle),
              "an AI-only vehicle followed players through an unrelated seam");
        global_structure_bsp_index=0;
        CHECK(!game_engine_object_parked(vehicle_index,_object_type_vehicle),
              "an AI-only vehicle did not return in its original scene");

        placed_pickups[0].item.flags=FLAG(_item_attached_to_unit_bit);
        vehicle->unit.driver_object_index=NONE;
        vehicle->object.first_child_object_index=units[0];
        objects[0].object.type=_object_type_biped;
        objects[0].object.parent_object_index=vehicle_index;
        objects[0].object.next_object_index=units[3];
        objects[0].unit.parent_seat_index=1;
        game_engine_record_regional_object(index,&item,0);
        game_engine_record_regional_object(vehicle_index,vehicle,0);
        global_structure_bsp_index=1;
        CHECK(!game_engine_object_parked(index,_object_type_weapon)&&
              !game_engine_object_parked(vehicle_index,_object_type_vehicle),
              "carried gear or a passenger-only vehicle failed to follow the crossing");
        return 0;
    }
    if (!strcmp(name, "regional-equipment"))
    {
        enum { DONOR_EQUIPMENT_ROW = 484 };
        struct scenario_netgame_equipment equipment[DONOR_EQUIPMENT_ROW + 1]={
            {.game_type={2},.spawn_time=1,.position={50,0,-.01f},.item_collection={1}},
            {.game_type={2},.spawn_time=1,.position={200,0,0},.item_collection={1}},
            [DONOR_EQUIPMENT_ROW]={.game_type={2},.spawn_time=1,.position={250,0,0},.item_collection={1}},
        };

        scenario.type=_scenario_type_multiplayer;
        scenario.netgame_equipment=(struct tag_block){NUMBEROF(equipment),equipment};
        test_region_lookup=test_region_overlap=mcc_loaded=TRUE;
        structures[0].clusters.count=structures[1].clusters.count=4;
        memset(game_engine_equipment_home_bsp,NONE,sizeof(game_engine_equipment_home_bsp));

        ticks=1;
        test_equipment_client=TRUE;
        game_engine_update_item_spawn();
        CHECK(!placed_pickup_count&&!game_engine_equipment_regions_seen,"a client initialized pickups");
        test_equipment_client=FALSE;
        game_engine_update_item_spawn();
        CHECK(placed_pickup_count==2&&game_engine_equipment_home_bsp[DONOR_EQUIPMENT_ROW]==NONE,
              "spawned outside the region or skipped first activation");

        ticks=TICKS_PER_SECOND;
        game_engine_update_item_spawn();
        CHECK(placed_pickup_count==2&&placed_pickups[0].item.last_owned_time==ticks,
              "duplicated an unclaimed pickup or left it eligible for cleanup");
        placed_pickups[0].object.parent_object_index=units[0];
        ticks+=TICKS_PER_SECOND;
        game_engine_update_item_spawn();
        CHECK(placed_pickup_count==3&&equipment[0].run_time_spawned_item_index==pickup_handle+2&&
              placed_pickups[0].object.parent_object_index==units[0],"did not restock while preserving the carried item");

        global_structure_bsp_index=1;
        global_structure_bsp=&structures[1];
        global_collision_bsp=&collisions[1];
        ticks++;
        game_engine_update_item_spawn();
        CHECK(placed_pickup_count==4&&game_engine_equipment_home_bsp[1]==0&&
              equipment[DONOR_EQUIPMENT_ROW].run_time_spawned_item_index==pickup_handle+3&&
              placed_pickups[3].item.netgame_equipment_index==DONOR_EQUIPMENT_ROW,
              "reassigned a shared seam or skipped the expanded equipment row");
        CHECK(placed_pickups[2].item.last_owned_time==ticks,"parked pad became eligible for cleanup");
        CHECK(game_engine_equipment_item_parked(pickup_handle+2)&&
              game_engine_equipment_item_parked(pickup_handle+1)&&
              !game_engine_equipment_item_parked(pickup_handle+3),
              "pad ownership did not separate the active and parked region");
        placed_pickups[0].object.parent_object_index=NONE;
        CHECK(!game_engine_equipment_item_parked(pickup_handle),
              "a superseded pad item still belonged to the old region");
        placed_pickups[1].object.position.x+=1.f;
        CHECK(!game_engine_equipment_item_parked(pickup_handle+1),
              "a displaced item could not leave its home region");
        placed_pickups[1].object.position.x-=1.f;
        placed_pickups[1].item.flags|=FLAG(_item_attached_to_unit_bit);
        CHECK(!game_engine_equipment_item_parked(pickup_handle+1),
              "a carried item was parked outside the player's region");
        placed_pickups[1].item.flags&=~FLAG(_item_attached_to_unit_bit);

        global_structure_bsp_index=0;
        global_structure_bsp=&structures[0];
        global_collision_bsp=&collisions[0];
        ticks=3*TICKS_PER_SECOND;
        game_engine_update_item_spawn();
        CHECK(placed_pickup_count==4&&equipment[0].run_time_spawned_item_index==pickup_handle+2&&
              !game_engine_equipment_item_parked(pickup_handle+2),
              "duplicated the pickup on a round trip");
        return 0;
    }
    if (!strcmp(name, "regional-starts"))
    {
        struct player_starting_location local={{100,0,0},0};
        struct player_starting_location foreign={{100,0,0},1};
        struct player_starting_location outside={{210,0,0},0};
        struct player_starting_location unset={{105,0,0},NONE};
        struct player_starting_location floor={{100,0,-.01f},0};
        struct player_starting_location buried={{100,0,-1},0};

        scenario.type=_scenario_type_multiplayer;
        test_region_lookup=mcc_loaded=TRUE;
        structures[0].clusters.count=structures[1].clusters.count=4;
        CHECK(player_starting_location_in_active_bsp(&local),"rejected a valid authored start");
        CHECK(!player_starting_location_in_active_bsp(&foreign),"admitted a foreign authored start inside shared geometry");
        CHECK(!player_starting_location_in_active_bsp(&outside),"admitted a start without valid collision");
        CHECK(player_starting_location_in_active_bsp(&unset),"rejected an unassigned start in valid geometry");
        CHECK(player_starting_location_in_active_bsp(&floor)&&floor.position.z==-.01f,
              "rejected a floor start or moved its authored position");
        CHECK(!player_starting_location_in_active_bsp(&buried),"admitted a buried start");
        global_structure_bsp_index=1;
        global_structure_bsp=&structures[1];
        CHECK(!player_starting_location_in_active_bsp(&local),"did not follow the active BSP");
        outside.structure_bsp_reference_index=1;
        CHECK(player_starting_location_in_active_bsp(&outside),"rejected a destination BSP start");
        global_structure_bsp_index=NONE;
        CHECK(!player_starting_location_in_active_bsp(&unset),"admitted a start with no active BSP");
        scenario.type=0;
        CHECK(player_starting_location_in_active_bsp(&outside),"changed campaign spawn rules");
        scenario.type=_scenario_type_multiplayer;
        mcc_loaded=FALSE;
        CHECK(player_starting_location_in_active_bsp(&outside),"changed classic multiplayer spawn rules");
        return 0;
    }
    if (!strcmp(name, "regional-teleporters"))
    {
        struct scenario_netgame_flag markers[5]={0};
        short const channel=8;
        real_point3d border={199.8f,0,0};
        long first,second;

        scenario.type=_scenario_type_multiplayer;
        scenario.netgame_flags=(struct tag_block){NUMBEROF(markers),markers};
        markers[0]=(struct scenario_netgame_flag){{201,0,0},_netgame_flag_teleporter_target,channel};
        markers[1]=(struct scenario_netgame_flag){{200.1f,0,0},_netgame_flag_teleporter_source,channel};
        markers[2]=(struct scenario_netgame_flag){{50,0,-1},_netgame_flag_teleporter_target,channel};
        markers[3]=(struct scenario_netgame_flag){{50,0,-.01f},_netgame_flag_teleporter_target,channel};
        markers[4]=(struct scenario_netgame_flag){{199.6f,0,-.01f},_netgame_flag_teleporter_source,channel};
        test_region_lookup=mcc_loaded=TRUE;
        structures[0].clusters.count=structures[1].clusters.count=4;

        CHECK(game_engine_find_teleporter(&border,.5f,_netgame_flag_teleporter_source,NONE)==4,
              "the source in the next BSP suppressed the local source");
        CHECK(game_engine_find_teleporter(NULL,0,_netgame_flag_teleporter_target,channel)==3,
              "the destination in the next BSP shadowed the local channel");
        global_structure_bsp_index=1;
        global_structure_bsp=&structures[1];
        CHECK(game_engine_find_teleporter(NULL,0,_netgame_flag_teleporter_target,channel)==0,
              "the second BSP did not use its own channel");
        global_structure_bsp_index=NONE;
        CHECK(game_engine_find_teleporter(NULL,0,_netgame_flag_teleporter_target,channel)==NONE,
              "selected a regional teleporter without an active BSP");

        first=find_netgame_flags(NULL,0,0,_netgame_flag_teleporter_target,channel,1,&second);
        CHECK(first==1&&second==0,"changed the legacy global flag query");
        scenario.structure_bsp_references.count=1;
        CHECK(game_engine_find_teleporter(NULL,0,_netgame_flag_teleporter_target,channel)==0,
              "restricted teleporters on a single-BSP MCC map");
        scenario.structure_bsp_references.count=2;
        scenario.type=0;
        CHECK(game_engine_find_teleporter(NULL,0,_netgame_flag_teleporter_target,channel)==0,
              "restricted campaign teleporters");
        scenario.type=_scenario_type_multiplayer;
        mcc_loaded=FALSE;
        CHECK(game_engine_find_teleporter(NULL,0,_netgame_flag_teleporter_target,channel)==0,
              "changed the legacy global teleporter contract");
        return 0;
    }
    if (!strncmp(name, "mcc-prepare-",12))
    {
        boolean extra_above=!strcmp(name,"mcc-prepare-space-above");
        boolean cold_vehicle=!strcmp(name,"mcc-prepare-cold-vehicle-clearance");
        boolean cold_regional=!strcmp(name,"mcc-prepare-cold-regional-clearance")||cold_vehicle;
        boolean overlap=!strcmp(name,"mcc-prepare-overlap")||!strcmp(name,"mcc-prepare-cold-crossing")||cold_regional;
        boolean limits=!strcmp(name,"mcc-prepare-limits");
        boolean refused=!strcmp(name,"mcc-prepare-refused");
        boolean evict=!strcmp(name,"mcc-prepare-evict");
        boolean occupied_regions=!strcmp(name,"mcc-prepare-occupied-regions");
        boolean fourth_region=occupied_regions||!strcmp(name,"mcc-prepare-fourth-region");
        boolean preview=!strcmp(name,"mcc-prepare-preview");
        boolean three_regions=fourth_region||!strcmp(name,"mcc-prepare-three-regions");
        short region_count=fourth_region?4:3;
        boolean regional_crossing=!strcmp(name,"mcc-prepare-regional-crossing");
        boolean crossing=!strcmp(name,"mcc-prepare-crossing")||regional_crossing;
        boolean fractures=!strcmp(name,"mcc-prepare-fractures");
        struct mcc_cache_bsp regions[4];
        uint32_t const offsets[]={0x4000,0x1000,0x2000,0x3000};

        mcc_loaded=mcc_prepare_fixture=TRUE;
        mcc_loaded_cache.report.bsp_count=scenario.structure_bsp_references.count=region_count;
        mcc_loaded_cache.report.bsps=regions;
        if (three_regions||preview) mcc_loaded_cache.report.identity.scenario_type=_scenario_type_multiplayer;
        mcc_loaded_cache.report.tag_base=(uint32_t)(uintptr_t)mcc_regions[0].header;
        cold_crossing=!strcmp(name,"mcc-prepare-cold-crossing")||cold_regional;
        mcc_loaded_cache.capacity=(overlap?2:region_count)*sizeof(mcc_regions[0]);
        for (short i=0;i<region_count;i++)
        {
            mcc_regions[i].structure=structures[i<2?i:0];
            regions[i]=(struct mcc_cache_bsp){
                .file_offset=offsets[i],.size=sizeof(mcc_regions[i]),.tag_handle=i,
                .address=(uint32_t)(uintptr_t)mcc_regions[i].header,
                .structure_address=(uint32_t)(uintptr_t)&mcc_regions[i].structure
            };
            if ((overlap||relocated||extra_above)&&i==destination)
            {
                regions[i].address=(uint32_t)(uintptr_t)mcc_regions[old].header+(extra_above?0:4);
                regions[i].structure_address=regions[i].address+24;
            }
            if (((extra_below||three_regions)&&i==2)||(fourth_region&&i==3))
            {
                regions[i].address=(uint32_t)(uintptr_t)mcc_regions[0].header;
                regions[i].structure_address=regions[i].address+24;
            }
            references[i].structure_bsp.index=i;
            references[i].base_address=regions[i].address;
            references[i].file_offset=regions[i].file_offset;
            references[i].file_size=regions[i].size;
        }
        mcc_file_structure.collision_bsp.count=1;
        mcc_file_structure.leaves.count=1;
        mcc_file_structure.clusters.count=1;
        mcc_file_structure.cluster_data.size=4;
        mcc_region_live[old]=TRUE;
        mcc_loaded_cache.bsp_addresses[old]=regions[old].address;
        global_structure_bsp=&mcc_regions[old].structure;
        cache_file_globals.structure_bsp_header=mcc_regions[old].header;
        test_tag_instances[old].base_address=global_structure_bsp;
        reject_load[destination]=refused;
        reject_verify[destination]=limits;
        if (preview)
        {
            struct scenario_bsp_prepared prepared;
            struct structure_bsp const *geometry;
            mcc_cache_bsp_set_active(&references[old],TRUE);
            mcc_cache_bsp_set_in_use(&references[old],TRUE);
            CHECK(!mcc_cache_bsp_preview(&references[destination],FALSE)&&!mcc_prepared_loads,
                  "a view loaded terrain during submission");
            geometry=mcc_cache_bsp_preview(&references[destination],TRUE);
            CHECK(geometry&&mcc_prepared_loads==1&&mcc_preview_bsp==&regions[destination],
                  "the spare warm slot did not prepare its checked geometry");
            CHECK(mcc_cache_bsp_preview(&references[destination],FALSE)==geometry&&
                  !mcc_cache_bsp_preview(&references[2],TRUE)&&mcc_prepared_loads==1,
                  "an immediate camera turn replaced the preview without a cooldown");
            ticks+=30;
            CHECK(!mcc_cache_bsp_preview(&references[2],FALSE)&&
                  mcc_cache_bsp_preview(&references[destination],FALSE)==geometry,
                  "a viewport replaced the warmed region during submission");
            {
                uint32_t capacity=mcc_loaded_cache.capacity;
                mcc_loaded_cache.capacity=sizeof(mcc_regions[0]);
                regions[2].address=regions[old].address;
                regions[2].structure_address=regions[2].address+24;
                references[2].base_address=regions[2].address;
                CHECK(!mcc_cache_bsp_preview(&references[2],TRUE)&&
                      mcc_cache_bsp_preview(&references[destination],FALSE)==geometry&&
                      !mcc_prepared_releases,
                      "a preview that could not fit evicted an existing preview");
                mcc_loaded_cache.capacity=capacity;
                regions[2].address=(uint32_t)(uintptr_t)mcc_regions[2].header;
                regions[2].structure_address=(uint32_t)(uintptr_t)&mcc_regions[2].structure;
                references[2].base_address=regions[2].address;
            }
            geometry=mcc_cache_bsp_preview(&references[2],TRUE);
            CHECK(geometry&&mcc_preview_bsp==&regions[2]&&mcc_prepared_loads==2&&mcc_prepared_releases==1,
                  "the frame could not replace the preview with another route");
            CHECK(!mcc_cache_bsp_retained_structure(&references[2])&&
                  global_structure_bsp_index==old&&!test_tag_instances[2].base_address&&
                  mcc_cache_bsp_pinned(&regions[old])&&mcc_region_live[old],
                  "the camera exposed preview geometry to simulation or evicted a live region");
            CHECK(scenario_structure_bsp_prepare(&references[old],&references[2],&prepared)==_scenario_bsp_prepare_ready&&
                  prepared.structure==geometry&&!mcc_preview_bsp&&mcc_prepared_loads==2,
                  "the BSP switch failed to reuse the already validated preview");
            return 0;
        }
        if (occupied_regions)
        {
            uint32_t capacity=mcc_loaded_cache.capacity;
            struct scenario_bsp_prepared prepared;
            mcc_cache_bsp_set_active(&references[old],TRUE);
            mcc_cache_bsp_set_in_use(&references[old],TRUE);
            mcc_loaded_cache.capacity=sizeof(mcc_regions[0]);
            CHECK(scenario_structure_bsp_prepare(&references[old],&references[3],&prepared)==_scenario_bsp_prepare_failed&&
                  !mcc_prepared_loads&&!disconnects,
                  "a sole foreground simulation hold permitted a cold switch");
            mcc_loaded_cache.capacity=capacity;
            mcc_cache_bsp_set_in_use(&references[old],FALSE);
        }
        if (cold_crossing)
        {
            long passenger=objects[0].object.parent_object_index;
            parallel_routes=TRUE;
            switches[0].cutscene_flag_index=NONE;
            mcc_regions[destination].structure.clusters.count=4;
            objects[0].object.position.x=260;
            objects[2].object.bounding_sphere_center.x=250;
            objects[2].object.position.x=190;
            prepared_crossing_leaf=cold_regional?0:1;
            if (cold_regional)
            {
                coop=FALSE;
                regional_match=regional_room_ready=TRUE;
                objects[1].object.parent_object_index=units[2];
                if (cold_vehicle)
                {
                    bsp_vehicle_contacts[old]=FLAG(0);
                    bsp_vehicle_contacts[destination]=FLAG(0)|FLAG(2);
                }
                else
                {
                    objects[0].object.parent_object_index=passenger=NONE;
                    objects[0].object.position.x=190;
                    bsp_pill_blocked[destination]=TRUE;
                }
            }

            CHECK(!scenario_switch_structure_bsp(destination),
                  "cold crossing accepted an invalid leaf or a new obstruction");
            CHECK(global_structure_bsp_index==old&&global_collision_bsp==&collisions[old]&&
                  disconnects==1&&reconnects==1&&loads[destination]==1&&loads[old]==1&&
                  unloads[destination]==1&&!teleports&&!ejections&&!menu&&
                  objects[0].object.parent_object_index==passenger&&
                  players_globals->pending_teleport_starting_location_index==NONE,
                  "cold rollback failed to restore the source and passenger");

            prepared_crossing_leaf=0;
            bsp_pill_blocked[old]=cold_regional&&!cold_vehicle; /* Shared contact permits the retry. */
            bsp_vehicle_contacts[destination]=bsp_vehicle_contacts[old];
            players_globals->pending_teleport_starting_location_index=0;
            /* The cache boundary stub needs the reloaded source's resident address. */
            mcc_region_live[old]=TRUE;
            mcc_loaded_cache.bsp_addresses[old]=regions[old].address;
            CHECK(scenario_switch_structure_bsp(destination),"a valid cold crossing was refused");
            CHECK(global_structure_bsp_index==destination&&global_collision_bsp==&collisions[destination]&&
                  disconnects==2&&reconnects==2&&!mcc_prepared_loads&&
                  objects[0].object.parent_object_index==passenger&&clock_starts==2&&collision_log,
                  "cold retry did not preserve the occupied crossing root");
            return 0;
        }
        if (crossing)
        {
            parallel_routes=TRUE;
            switches[0].cutscene_flag_index=NONE;
            mcc_regions[1].structure.clusters.count=4;
            objects[0].object.position.x=260;
            objects[2].object.bounding_sphere_center.x=250;
            objects[2].object.position.x=190;
            prepared_crossing_leaf=1;

            if (regional_crossing)
            {
                coop=FALSE;
                regional_match=TRUE;
                scenario.type=_scenario_type_multiplayer;
                prepared_crossing_leaf=0;
                CHECK(!players_coop_can_activate_structure_bsp(old,destination,&mcc_regions[destination].structure,NULL),
                      "crossing accepted without negotiated peers");
                regional_room_ready=TRUE;
                CHECK(!players_coop_can_activate_structure_bsp(old,destination,&mcc_regions[destination].structure,NULL),
                      "crossing would leave a living player behind");
                objects[1].object.parent_object_index=units[2];
                objects[0].object.parent_object_index=NONE;
                objects[0].object.position.x=190;
                bsp_pill_blocked[1]=TRUE;
                CHECK(!players_coop_can_activate_structure_bsp(old,destination,&mcc_regions[destination].structure,NULL),
                      "the destination obstructed a biped that fit in the source");
                bsp_pill_blocked[0]=TRUE;
                CHECK(players_coop_can_activate_structure_bsp(old,destination,&mcc_regions[destination].structure,NULL),
                      "shared terrain contact refused an otherwise valid crossing");
                bsp_pill_blocked[0]=bsp_pill_blocked[1]=FALSE;

                struct object_datum *door=&objects[3];
                door->object.type=_object_type_machine;
                door->object.parent_object_index=NONE;
                door->object.flags=FLAG(_object_connected_to_map_bit);
                door->object.location.cluster_index=0;
                door->object.bounding_sphere_center=objects[0].object.bounding_sphere_center;
                door->object.bounding_sphere_radius=1.f;
                regional_blocker=units[3];
                game_engine_record_regional_object(regional_blocker,door,old);
                CHECK(players_coop_can_activate_structure_bsp(old,destination,&mcc_regions[destination].structure,NULL)&&
                      !regional_pill_probes&&!regional_vehicle_probes,
                      "source collision geometry blocked a destination crossing");
                game_engine_record_regional_object(regional_blocker,door,destination);
                CHECK(!players_coop_can_activate_structure_bsp(old,destination,&mcc_regions[destination].structure,NULL)&&
                      regional_pill_probes==1,
                      "the on-foot player could cross into a known parked door");
                objects[0].object.parent_object_index=units[2];
                door->object.bounding_sphere_center=objects[2].object.bounding_sphere_center;
                CHECK(!players_coop_can_activate_structure_bsp(old,destination,&mcc_regions[destination].structure,NULL)&&
                      regional_vehicle_probes==1,
                      "the occupied vehicle could cross into a known parked door");
                door->object.bounding_sphere_center.x+=10.f;
                CHECK(players_coop_can_activate_structure_bsp(old,destination,&mcc_regions[destination].structure,NULL)&&
                      regional_vehicle_probes==1,
                      "an out-of-reach blocker reached the vehicle collision test");

                door->object.type=_object_type_scenery;
                door->object.bounding_sphere_center=objects[2].object.bounding_sphere_center;
                game_engine_record_regional_object(regional_blocker,door,destination);
                CHECK(!players_coop_can_activate_structure_bsp(old,destination,&mcc_regions[destination].structure,NULL)&&
                      regional_vehicle_probes==2,
                      "the occupied vehicle could cross into destination scenery");
                objects[0].object.parent_object_index=NONE;
                door->object.bounding_sphere_center=objects[0].object.bounding_sphere_center;
                CHECK(!players_coop_can_activate_structure_bsp(old,destination,&mcc_regions[destination].structure,NULL)&&
                      regional_pill_probes==2,
                      "the on-foot player could cross into destination scenery");
                door->object.type=_object_type_light_fixture;
                game_engine_record_regional_object(regional_blocker,door,destination);
                CHECK(!players_coop_can_activate_structure_bsp(old,destination,&mcc_regions[destination].structure,NULL)&&
                      regional_pill_probes==3,
                      "the on-foot player could cross into a solid destination light fixture");
                objects[0].object.parent_object_index=units[2];
                regional_blocker=NONE;

                bsp_vehicle_contacts[old]=FLAG(0);
                bsp_vehicle_contacts[destination]=FLAG(0)|FLAG(2);
                CHECK(!players_coop_can_activate_structure_bsp(old,destination,&mcc_regions[destination].structure,NULL),
                      "a new contact at a separate mass point was accepted");
                bsp_vehicle_contacts[destination]=FLAG(0);
                CHECK(players_coop_can_activate_structure_bsp(old,destination,&mcc_regions[destination].structure,NULL),
                      "shared wheel contact prevented a native crossing");
                bsp_vehicle_contacts[old]=bsp_vehicle_contacts[destination]=0;
                prepared_crossing_leaf=1;
            }

            CHECK(!scenario_switch_structure_bsp(destination),"crossed to an invalid destination leaf");
            CHECK(global_structure_bsp_index==old&&global_collision_bsp==&collisions[old]&&
                  cache_file_globals.structure_bsp_header==mcc_regions[old].header&&!disconnects&&!reconnects&&
                  clock_stops==1&&clock_starts==1&&collision_log&&mcc_retained_bsps[0]==&regions[destination]&&
                  players_globals->pending_teleport_starting_location_index==NONE,
                  "refused seam dropped the active scene or its prepared neighbor");

            prepared_crossing_leaf=0;
            players_globals->pending_teleport_starting_location_index=0;
            CHECK(scenario_switch_structure_bsp(destination),"a valid vehicle root failed to cross");
            CHECK(disconnects==1&&reconnects==1&&objects[0].object.parent_object_index==units[2]&&
                  global_collision_bsp==&collisions[destination]&&mcc_retained_bsps[0]==&regions[old]&&
                  mcc_prepared_loads==1&&mcc_prepared_validations==1&&mcc_prepared_releases==0,
                  "crossing reloaded its prepared BSP or removed the passenger");
            if (regional_crossing)
                CHECK(objects[1].object.parent_object_index==units[2]&&!teleports&&!ejections&&
                      object_partition_bsp==destination,
                      "competitive crossing lost its regional partition or unboarded a player");
            return 0;
        }

        CHECK(scenario_switch_structure_bsp(destination)==(!limits&&!refused),"wrong outcome for staged BSP");
        CHECK(clock_stops==1&&clock_starts==1&&collision_log&&!menu,"staging did not restore the clock");
        CHECK(mcc_prepared_loads==!overlap&&mcc_prepared_releases==(limits||overlap),
              "unexpected preparation lifetime");
        if (refused || limits)
        {
            CHECK(!disconnects&&!reconnects&&!unloads[old]&&!loads[destination]&&!ejections&&!teleports&&
                  global_structure_bsp_index==old&&global_structure_bsp==&mcc_regions[old].structure&&
                  cache_file_globals.structure_bsp_header==mcc_regions[old].header&&
                  test_tag_instances[old].base_address==global_structure_bsp&&
                  mcc_region_live[old]&&!mcc_region_live[destination]&&!mcc_retained_bsps[0]&&
                  players_globals->pending_teleport_starting_location_index==NONE,
                  "refused staging mutated the active BSP or its players");
        }
        else
        {
            CHECK(disconnects==1&&reconnects==1&&unloads[old]==overlap&&loads[destination]==overlap&&
                  global_structure_bsp_index==destination&&global_collision_bsp==&collisions[destination],
                  "activation bypassed native reconnect or used the wrong load path");
            if (!overlap)
            {
                CHECK(cache_file_globals.structure_bsp_header==mcc_regions[destination].header&&
                      test_tag_instances[destination].base_address==&mcc_regions[destination].structure&&
                      !test_tag_instances[old].base_address&&mcc_retained_bsps[0]==&regions[old]&&
                      mcc_region_live[old]&&mcc_region_live[destination],
                      "the inactive BSP lost its geometry or remained published");
                if (relocated||extra_above)
                    CHECK(mcc_loaded_cache.bsp_addresses[destination]==(uint32_t)(uintptr_t)mcc_regions[destination].header&&
                          mcc_loaded_cache.bsp_addresses[destination]!=regions[destination].address,
                          "an overlapping BSP did not use the free slot");
                if (!strcmp(name,"mcc-prepare-terrain-sight"))
                {
                    real_point3d eye={0,0,0}, target={4,0,0};
                    unsigned long flags=FLAG(_collision_test_objects_scenery_bit);
                    surface_state.breakable_surface_flags[0][0]=1;

                    CHECK(!distributed_report_sight_clear(old,flags,&eye,&target,&target)&&
                          distributed_report_sight_clear(destination,flags,&eye,&target,&target),
                          "identical world positions did not query independent resident geometry");
                    CHECK(!distributed_report_sight_clear(2,flags,&eye,&target,&target),
                          "missing geometry looked like a clear line");
                    CHECK(distributed_report_sight_clear(NONE,flags,&eye,&target,&target)&&legacy_traces==1,
                          "classic combat did not keep the native shared scene trace");

                    surface_state.breakable_surface_flags[0][0]=0;
                    CHECK(distributed_report_sight_clear(old,flags,&eye,&target,&target)&&terrain_traces==4&&
                          global_structure_bsp_index==destination&&global_collision_bsp==&collisions[destination],
                          "the retained trace changed the session BSP or read its neighbor's fracture state");
                    return 0;
                }
                if (fractures)
                {
                    struct structure_breakable_surface panes[2] = {
                        {.centroid={10.f,0,0},.collision_surface_index=7},
                        {.centroid={20.f,0,0},.collision_surface_index=8}
                    };
                    mcc_regions[old].structure.breakable_surfaces=(struct tag_block){1,&panes[old]};
                    mcc_regions[destination].structure.breakable_surfaces=(struct tag_block){1,&panes[destination]};
                    CHECK(scenario_structure_bsp_try_and_get(old)==&mcc_regions[old].structure&&
                          scenario_structure_bsp_try_and_get(destination)==&mcc_regions[destination].structure&&
                          !scenario_structure_bsp_try_and_get(2),"resident lookup changed the active tag binding");

                    memset(surface_state.breakable_surface_flags,0xff,sizeof(surface_state.breakable_surface_flags));
                    breakable_surface_port_break(old,0,&panes[old].centroid);
                    CHECK(!breakable_surface_extant_in_bsp(old,0)&&breakable_surface_extant_in_bsp(destination,0)&&
                          !surface_effects,"a retained BSP break changed the live surface or played an effect");
                    breakable_surface_port_break(destination,0,&panes[destination].centroid);
                    breakable_surface_port_break(destination,0,&panes[destination].centroid);
                    breakable_surface_port_break(old,1,&panes[old].centroid);
                    CHECK(surface_effects==1&&!breakable_surface_extant_in_bsp(destination,0)&&
                          breakable_surface_extant_in_bsp(old,1),"surface validation or duplicate suppression failed");
                    host_resend.refresh=TRUE;
                    host_send_broken_surfaces();
                    CHECK(surface_events==2&&sent_surfaces[0].frame==old&&sent_surfaces[1].frame==destination&&
                          sent_surfaces[0].reals[0]==10.f&&sent_surfaces[1].reals[0]==20.f,
                          "late join refresh lost a BSP frame or used the wrong geometry");
                    host_send_broken_surfaces();
                    CHECK(surface_events==4,"a dropped fracture update could not be resent");
                    mcc_loaded=FALSE;
                    host_send_broken_surfaces();
                    CHECK(surface_events==5&&sent_surfaces[4].frame==destination,
                          "legacy cache sent off-BSP fracture state");
                    mcc_loaded=TRUE;
                    coop_events.count=MAXIMUM_QUEUED_EVENTS-1;
                    host_send_broken_surfaces();
                    CHECK(surface_events==6&&sent_surfaces[5].frame==old,
                          "fracture refresh ignored the last available queue slot");
                    coop_events.count=MAXIMUM_QUEUED_EVENTS-1;
                    host_send_broken_surfaces();
                    CHECK(surface_events==7&&sent_surfaces[6].frame==destination,
                          "fracture refresh starved the next BSP when capacity was limited");
                    CHECK(scenario_switch_structure_bsp(2)&&!scenario_structure_bsp_try_and_get(old),
                          "an evicted region still exposed geometry");
                    surface_state.breakable_surface_flags[old][0]=0xff;
                    breakable_surface_port_break(old,0,&panes[old].centroid);
                    CHECK(breakable_surface_extant_in_bsp(old,0),
                          "legacy campaign accepted a fracture for unloaded geometry");
                    scenario.type=_scenario_type_multiplayer;
                    breakable_surface_port_break(3,0,&panes[old].centroid);
                    CHECK(breakable_surface_extant_in_bsp(3,0),
                          "a BSP with no scenario reference accepted a fracture");
                    breakable_surface_port_break(old,0,&panes[old].centroid);
                    CHECK(!breakable_surface_extant_in_bsp(old,0)&&surface_effects==1,
                          "an authoritative break received before load was lost or played an off-region effect");
                    CHECK(scenario_switch_structure_bsp(old)&&global_structure_bsp_index==old&&
                          !breakable_surface_extant_in_bsp(old,0)&&surface_effects==1,
                          "loading a region restored glass that was already broken");
                    return 0;
                }
                if (three_regions)
                {
                    unsigned long address;
                    mcc_loaded_cache.capacity=2*sizeof(mcc_regions[0]);
                    address=mcc_cache_bsp_prepare_address(&references[destination],&references[2]);
                    CHECK(address==references[2].base_address&&mcc_region_live[old],
                          "two-window fallback evicted its neighbor during address selection");
                    mcc_loaded_cache.capacity=region_count*sizeof(mcc_regions[0]);
                    CHECK(scenario_switch_structure_bsp(2), "third regional BSP did not load");
                    CHECK(mcc_prepared_loads==2&&mcc_prepared_releases==0&&
                          mcc_loaded_cache.bsp_addresses[2]!=(uint32_t)references[2].base_address&&
                          scenario_structure_bsp_try_and_get(old)==&mcc_regions[old].structure&&
                          scenario_structure_bsp_try_and_get(destination)==&mcc_regions[destination].structure&&
                          !test_tag_instances[old].base_address&&!test_tag_instances[destination].base_address,
                          "third region overwrote a neighbor or published an inactive tag");
                    if (occupied_regions)
                    {
                        struct scenario_bsp_prepared rejected;
                        int previous_disconnects = disconnects;

                        CHECK(mcc_cache_bsp_in_use(&regions[2]), "the bound native scene did not hold its BSP");
                        mcc_cache_bsp_set_in_use(&references[old],TRUE);
                        mcc_cache_bsp_set_in_use(&references[destination],TRUE);
                        mcc_loaded_cache.capacity=3*sizeof(mcc_regions[0]);

                        CHECK(!scenario_switch_structure_bsp(3)&&global_structure_bsp_index==2&&
                              disconnects==previous_disconnects&&mcc_prepared_loads==2&&mcc_prepared_releases==0,
                              "a full set of live holds fell through to a destructive cold switch");
                        CHECK(!mcc_cache_bsp_load(&references[3],0,&rejected.header,&rejected.structure)&&
                              mcc_region_live[old]&&mcc_region_live[destination]&&mcc_region_live[2],
                              "the cold loader overwrote a held window");
                        CHECK(scenario_structure_bsp_try_and_get(old)==&mcc_regions[old].structure&&
                              scenario_structure_bsp_try_and_get(destination)==&mcc_regions[destination].structure,
                              "retained lookup lost an occupied region after rejection");

                        mcc_cache_bsp_set_in_use(&references[2],TRUE);
                        mcc_loaded_cache.capacity=4*sizeof(mcc_regions[0]);
                        CHECK(scenario_switch_structure_bsp(3)&&global_structure_bsp_index==3&&
                              mcc_prepared_loads==3&&mcc_prepared_releases==0,
                              "three simulation holds could not coexist with a foreground within budget");
                        CHECK(scenario_structure_bsp_try_and_get(old)==&mcc_regions[old].structure&&
                              scenario_structure_bsp_try_and_get(destination)==&mcc_regions[destination].structure&&
                              scenario_structure_bsp_try_and_get(2)==&mcc_regions[2].structure&&
                              mcc_cache_bsp_pinned(&regions[2])&&mcc_active_bsp==&regions[3],
                              "foreground activation hid or released an occupied BSP");
                        CHECK(scenario_switch_structure_bsp(old)&&global_structure_bsp_index==old&&
                              mcc_prepared_loads==3&&mcc_prepared_releases==0&&mcc_region_live[3],
                              "returning to a held BSP reloaded it or dropped the unoccupied neighbor");

                        mcc_cache_bsp_set_in_use(&references[old],FALSE);
                        CHECK(mcc_region_live[old]&&mcc_active_bsp==&regions[old],
                              "releasing a simulation hold unloaded the active BSP");
                        mcc_cache_bsp_set_in_use(&references[destination],FALSE);
                        CHECK(!mcc_region_live[destination]&&!scenario_structure_bsp_try_and_get(destination)&&
                              mcc_region_live[2]&&mcc_region_live[3],
                              "releasing an inactive hold leaked or evicted another BSP");
                        scenario_structure_bsp_unload(&references[old]);
                        CHECK(!mcc_region_live[old]&&mcc_region_live[2]&&!mcc_region_live[3]&&!mcc_active_bsp,
                              "unloading the foreground disturbed held geometry or kept an unheld neighbor");
                        mcc_cache_bsp_set_in_use(&references[2],FALSE);
                        CHECK(!mcc_region_live[2]&&!mcc_bsps_in_use&&mcc_prepared_releases==4,
                              "the final release leaked a BSP or hold");
                        return 0;
                    }
                    if (fourth_region)
                    {
                        reject_verify[3]=TRUE;
                        CHECK(!scenario_switch_structure_bsp(3)&&global_structure_bsp_index==2,
                              "failed candidate activated a new BSP");
                        CHECK(mcc_prepared_loads==3&&mcc_prepared_releases==1&&
                              mcc_region_live[old]&&mcc_region_live[destination]&&mcc_region_live[2]&&!mcc_region_live[3]&&
                              scenario_structure_bsp_try_and_get(old)==&mcc_regions[old].structure&&
                              scenario_structure_bsp_try_and_get(destination)==&mcc_regions[destination].structure,
                              "a rejected candidate evicted unrelated checked geometry");
                        reject_verify[3]=FALSE;
                        CHECK(scenario_switch_structure_bsp(3)&&global_structure_bsp_index==3,
                              "valid fourth region did not activate");
                        CHECK(mcc_prepared_loads==4&&mcc_prepared_releases==2&&
                              !mcc_region_live[old]&&mcc_region_live[destination]&&mcc_region_live[2]&&mcc_region_live[3],
                              "successful switch did not evict only the oldest neighbor");
                        scenario_structure_bsp_unload(&references[3]);
                        CHECK(mcc_prepared_releases==5&&!mcc_region_live[1]&&!mcc_region_live[2]&&!mcc_region_live[3],
                              "cold unload leaked retained geometry");
                        return 0;
                    }
                    CHECK(scenario_switch_structure_bsp(old)&&mcc_prepared_loads==2&&mcc_prepared_releases==0&&
                          scenario_structure_bsp_try_and_get(2)==&mcc_regions[2].structure&&
                          scenario_structure_bsp_try_and_get(destination)==&mcc_regions[destination].structure,
                          "return reloaded geometry or lost the third resident BSP");
                    scenario_structure_bsp_unload(&references[old]);
                    CHECK(!mcc_region_live[0]&&!mcc_region_live[1]&&!mcc_region_live[2]&&
                          !mcc_retained_bsps[0]&&!mcc_retained_bsps[1]&&mcc_prepared_releases==3,
                          "unload leaked a third resident BSP");
                    return 0;
                }
                if (evict)
                {
                    CHECK(scenario_switch_structure_bsp(2),"third region did not load");
                    CHECK(!mcc_region_live[old]&&mcc_region_live[destination]&&mcc_region_live[2]&&
                          mcc_prepared_loads==2&&mcc_prepared_releases==1&&mcc_retained_bsps[0]==&regions[destination],
                          "third region failed to evict only the inactive neighbor");
                }
                else
                {
                    CHECK(scenario_switch_structure_bsp(old),"cached return did not activate");
                    CHECK(mcc_prepared_loads==1&&mcc_prepared_validations==1&&mcc_prepared_releases==0&&
                          mcc_retained_bsps[0]==&regions[destination]&&test_tag_instances[old].base_address==
                          &mcc_regions[old].structure&&!test_tag_instances[destination].base_address,
                          "the cached return reloaded geometry or published both tags");
                    scenario_structure_bsp_unload(&references[old]);
                    CHECK(!mcc_region_live[old]&&!mcc_region_live[destination]&&!mcc_retained_bsps[0]&&
                          mcc_prepared_releases==2,"cold unload leaked retained geometry");
                }
            }
        }
        return 0;
    }
    if (!strcmp(name, "item-rested-surface-bsp"))
    {
        struct collision_surface old_floor={4},new_floor={9};
        struct item_datum item={0};
        struct item_definition definition={0};
        collisions[old].surfaces=(struct tag_block){1,&old_floor};
        collisions[destination].surfaces=(struct tag_block){1,&new_floor};
        item.item.bsp_index=old;
        item.item.flags=FLAG(_item_on_structure_bit);
        item.object.flags=FLAG(_object_at_rest_bit);
        item_recheck_support(&item,&definition);
        CHECK(item_rested_surface(&item)==&old_floor&&TEST_FLAG(item.object.flags,_object_at_rest_bit),
              "a rested weapon lost its own support plane");
        CHECK(scenario_switch_structure_bsp(destination),"destination test BSP failed to load");
        CHECK(!item_rested_surface(&item),"an impact could snap the weapon to another BSP's ground plane");
        item.object.flags|=FLAG(_object_outside_of_map_bit);
        item_recheck_support(&item,&definition);
        CHECK(item.item.bsp_index==old&&TEST_FLAG(item.object.flags,_object_at_rest_bit),
              "a pickup outside the loaded BSP lost its previous support");
        item.object.flags&=~FLAG(_object_outside_of_map_bit);
        definition.item.flags=FLAG(_item_definition_antigravity_bit);
        item_recheck_support(&item,&definition);
        CHECK(TEST_FLAG(item.object.flags,_object_at_rest_bit),"an antigravity pickup started falling");
        definition.item.flags=0;
        item.item.flags|=FLAG(_item_does_not_accelerate_bit);
        item_recheck_support(&item,&definition);
        CHECK(TEST_FLAG(item.object.flags,_object_at_rest_bit),"an immobile pickup started falling");
        item.item.flags&=~FLAG(_item_does_not_accelerate_bit);
        item_recheck_support(&item,&definition);
        CHECK(item.item.bsp_index==NONE&&item.item.rested_surface_index==NONE&&
              !TEST_FLAG(item.item.flags,_item_on_structure_bit)&&!TEST_FLAG(item.object.flags,_object_at_rest_bit),
              "a mobile pickup inside the new BSP could not settle again");

        /* The prop is still a valid datum, but its support belongs to the
         * region just left. The pickup is now inside the active region. */
        scenario.type=_scenario_type_multiplayer;
        mcc_loaded=TRUE;
        objects[2].object.type=_object_type_scenery;
        objects[2].object.parent_object_index=NONE;
        objects[2].object.flags=FLAG(_object_connected_to_map_bit);
        objects[2].object.location.cluster_index=0;
        game_engine_record_regional_object(units[2],&objects[2],old);
        item.item.flags=FLAG(_item_on_object_bit);
        item.item.item_on_rest_object_index=units[2];
        item.object.flags=FLAG(_object_at_rest_bit);
        global_structure_bsp_index=old;
        item_recheck_support(&item,&definition);
        CHECK(TEST_FLAG(item.object.flags,_object_at_rest_bit)&&item.item.item_on_rest_object_index==units[2],
              "an in-region prop stopped supporting a pickup");
        global_structure_bsp_index=destination;
        item.object.flags|=FLAG(_object_outside_of_map_bit);
        item_recheck_support(&item,&definition);
        CHECK(TEST_FLAG(item.object.flags,_object_at_rest_bit)&&item.item.item_on_rest_object_index==units[2],
              "an inactive pickup lost the prop it rests on");
        item.object.flags&=~FLAG(_object_outside_of_map_bit);
        CHECK(game_engine_object_parked(units[2],_object_type_scenery),"fixture support did not park");
        item_recheck_support(&item,&definition);
        CHECK(!TEST_FLAG(item.object.flags,_object_at_rest_bit)&&!TEST_FLAG(item.item.flags,_item_on_object_bit)&&
              item.item.item_on_rest_object_index==NONE,
              "an active pickup hung in the air on a prop in another BSP");
        return 0;
    }
    if (!strcmp(name, "connected-ai-passenger"))
    {
        connected = TRUE;
        switches[0].cutscene_flag_index = NONE;
        people[1].unit_index = NONE;
        objects[0].unit.parent_seat_index = 0;
        objects[1].unit.parent_seat_index = 1;
        CHECK(!players_coop_vehicle_passenger_crossing_bsp(units[1]), "AI in a different vehicle joined the crossing");
        objects[1].object.parent_object_index = units[2];
        CHECK(players_coop_vehicle_passenger_crossing_bsp(units[1]), "seated AI missed the player's crossing");
        connected = FALSE;
        CHECK(!players_coop_vehicle_passenger_crossing_bsp(units[1]), "the vehicle was outside the crossing trigger");
        connected = TRUE;
        objects[0].unit.parent_seat_index = NONE;
        CHECK(!players_coop_vehicle_passenger_crossing_bsp(units[1]), "unseated player was counted as a rider");
        objects[0].unit.parent_seat_index = 0;
        objects[0].object.damage_flags = FLAG(_object_dead_bit);
        CHECK(!players_coop_vehicle_passenger_crossing_bsp(units[1]), "dead player was counted as a rider");
        objects[0].object.damage_flags = 0;
        switches[0].cutscene_flag_index = 0;
        CHECK(!players_coop_vehicle_passenger_crossing_bsp(units[1]), "classic scripted gather changed AI behavior");
        switches[0].cutscene_flag_index = NONE;
        players_globals->pending_teleport_starting_location_index = NONE;
        CHECK(!players_coop_vehicle_passenger_crossing_bsp(units[1]), "an unqueued route carried AI");

        players_globals->pending_teleport_starting_location_index = 0;
        parallel_routes = TRUE;
        scenario.bsp_switch_trigger_volumes.count = 2;
        switches[0].trigger_volume_index = 1;
        switches[1]=(struct scenario_bsp_switch_trigger_volume){0,old,destination,NONE};
        CHECK(players_coop_vehicle_passenger_crossing_bsp(units[1]), "AI missed the player's alternate route to the same BSP");
        switches[1].destination_structure_bsp_index = 2;
        CHECK(!players_coop_vehicle_passenger_crossing_bsp(units[1]), "an unrelated route carried AI");
        return 0;
    }
    if (!strcmp(name, "majority-overlapping-routes"))
    {
        dedicated_vote_routes = TRUE;
        scenario.bsp_switch_trigger_volumes.count = 3;
        switches[0]=(struct scenario_bsp_switch_trigger_volume){0,old,2,0};
        switches[1]=(struct scenario_bsp_switch_trigger_volume){1,old,destination,0};
        switches[2]=(struct scenario_bsp_switch_trigger_volume){2,old,destination,NONE};
        CHECK(!players_coop_bsp_switch_majority(0) && !vote_announced &&
              vote_waiting == 1 && vote_waiting_player == players[0],
              "an unrelated route received another destination's votes");
        reject_reference[destination] = TRUE;
        CHECK(!players_coop_bsp_switch_majority(1) && !vote_announced &&
              main_globals.switch_to_structure_bsp_index == NONE,
              "an invalid destination queued or announced a crossing");
        reject_reference[destination] = FALSE;
        vote_excluded_player = players[1];
        CHECK(players_coop_bsp_switch_majority(1) && players_coop_state.bsp_crossing_count == 1 &&
              players_coop_state.bsp_crossing_players[0] == players[0],
              "away teammate was counted in the destination's voting population");
        CHECK(!vote_announced, "players were told they crossed before the native BSP load");
        reject_load[destination] = TRUE;
        main_switch_to_structure_bsp_private();
        CHECK(!vote_announced && global_structure_bsp_index == old &&
              !players_coop_state.bsp_crossing_count,
              "rolled-back load sent or retained a crossing announcement");

        reject_load[destination] = FALSE;
        vote_excluded_player = NONE;
        CHECK(players_coop_bsp_switch_majority(1) && !vote_announced,
              "overlap hid a valid vote or announced before the load");
        players_globals->pending_teleport_starting_location_index = 1;
        main_switch_to_structure_bsp_private();
        CHECK(vote_announced == 1 && vote_source == old && vote_destination == destination &&
              vote_crossers == 2 && voted_players[0] == players[0] && voted_players[1] == players[1] &&
              !players_coop_state.bsp_crossing_count,
              "successful load did not announce exactly one crossing with both players");
        return 0;
    }
    if (!strcmp(name, "overlapping-coop-routes"))
    {
        scenario.bsp_switch_trigger_volumes.count = 2;
        scenario.structure_bsp_references.count = 3;
        references[2].structure_bsp.index = 2;
        switches[1] = (struct scenario_bsp_switch_trigger_volume){1,old,2,0};
        ticks = 90;

        player_queue_structure_bsp_switch(&people[0], units[2]);
        CHECK(main_globals.switch_to_structure_bsp_index == destination &&
            players_globals->pending_teleport_starting_location_index == 0 &&
            players_coop_state.bsp_switch_time == ticks,
            "a second overlapping route replaced the accepted co-op destination and gather");

        main_globals.switch_to_structure_bsp_index = NONE;
        reject_reference[destination] = TRUE;
        player_queue_structure_bsp_switch(&people[0], units[2]);
        CHECK(main_globals.switch_to_structure_bsp_index == 2 &&
            players_globals->pending_teleport_starting_location_index == 1,
            "a refused first route stopped a valid alternative");

        main_globals.switch_to_structure_bsp_index = NONE;
        reject_reference[destination] = FALSE;
        coop = FALSE;
        player_queue_structure_bsp_switch(&people[0], units[2]);
        CHECK(main_globals.switch_to_structure_bsp_index == 2 &&
            players_globals->pending_teleport_starting_location_index == 1,
            "local multiplayer changed legacy route priority");
        return 0;
    }
    if (!strcmp(name, "scripted-selection"))
    {
        reject_load[destination] = TRUE;
        bsp_argument.value = destination;
        scenario_switch_structure_bsp_evaluate(0, 0, TRUE);
        CHECK(global_structure_bsp_index == old && scripted_multiplayer_bsp == NONE,
              "refused first choice locked the multiplayer BSP selector");

        bsp_argument.value = old;
        scenario_switch_structure_bsp_evaluate(0, 0, TRUE);
        CHECK(scripted_multiplayer_bsp == old && loads[destination] == 1,
              "explicit selection of the resident BSP should lock without reloading");

        bsp_argument.value = destination;
        hs_menu_script_running = TRUE;
        scenario_switch_structure_bsp_evaluate(0, 0, TRUE);
        CHECK(global_structure_bsp_index == old && scripted_multiplayer_bsp == old,
              "refused menu choice replaced the active BSP selection");

        reject_load[destination] = FALSE;
        scenario_switch_structure_bsp_evaluate(0, 0, TRUE);
        CHECK(global_structure_bsp_index == destination && scripted_multiplayer_bsp == destination,
              "valid menu choice did not update the selected BSP");

        hs_menu_script_running = FALSE;
        bsp_argument.value = old;
        scenario_switch_structure_bsp_evaluate(0, 0, TRUE);
        CHECK(global_structure_bsp_index == destination && scripted_multiplayer_bsp_reported,
              "background script could supersede the menu choice");
        return 0;
    }
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
    if(!strcmp(name,"host-superseded")||!strcmp(name,"regional-host-superseded"))
    {
        struct distributed_structure_bsp message={destination,0};
        if(!strcmp(name,"regional-host-superseded"))
        {
            coop=FALSE;
            regional_match=TRUE;
        }
        distributed_handle_structure_bsp(1, &message);
        CHECK(main_globals.switch_to_structure_bsp_index==destination&&loading_hud,"host BSP transition did not queue");
        message.structure_bsp_index=2;
        distributed_handle_structure_bsp(2, &message);
        CHECK(main_globals.switch_to_structure_bsp_index==destination&&loading_hud,"invalid update changed the pending switch");
        message.structure_bsp_index=old;
        distributed_handle_structure_bsp(3, &message);
        CHECK(main_globals.switch_to_structure_bsp_index==NONE&&!loading_hud,"client retained superseded BSP switch");
        CHECK(!disconnects&&!ejections&&!teleports&&global_structure_bsp_index==old,"client disturbed the active world");
        message.structure_bsp_index=destination;
        distributed_handle_structure_bsp(4, &message);
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
        distributed_handle_structure_bsp(1, &message);
        distributed_handle_unit_states(1,state,state+sizeof(state),1);
        CHECK(unit_snapshots_applied==0,"applied destination snapshot to the old BSP");
        message.structure_bsp_index=old;
        distributed_handle_structure_bsp(2, &message);
        distributed_handle_unit_states(2,state,state+sizeof(state),1);
        CHECK(unit_snapshots_applied==1,"snapshot did not resume after the host canceled the switch");
        message.structure_bsp_index=destination;
        distributed_handle_structure_bsp(3, &message);
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
        distributed_handle_structure_bsp(1, &message);
        main_switch_to_structure_bsp_private();
        CHECK(global_structure_bsp_index == old && !loading_hud, "failed load did not restore the source BSP");

        distributed_handle_unit_states(1, state, state + sizeof(state), 1);
        CHECK(unit_snapshots_applied == 0, "rollback accepted a player snapshot from the destination BSP");

        reject_load[destination] = FALSE;
        distributed_handle_structure_bsp(2, &message);
        main_switch_to_structure_bsp_private();
        CHECK(global_structure_bsp_index == destination, "client did not retry the failed destination");
        distributed_handle_unit_states(2, state, state + sizeof(state), 1);
        CHECK(unit_snapshots_applied == 1, "snapshots did not resume after the destination loaded");
        return 0;
    }
    if(!strcmp(name,"host-actor-snapshot-boundary"))
    {
        struct distributed_structure_bsp message={destination,0};
        struct client_actor *actor=&client_actors[0];
        byte entry=0;

        client_actor_count=1;
        *actor=(struct client_actor){
            .unit_index=units[0], .driven=TRUE, .impulse=3, .speech_sound=4,
            .user_animation_graph=5, .played_impulse_number=8
        };
        distributed_handle_actor_states(88,&entry,1);
        CHECK(actor_snapshots_applied==1,"actor gate changed unswitched gameplay");
        distributed_handle_structure_bsp(90,&message);
        CHECK(actor_controls_released==1&&actor->control_invalidated&&!actor->driven&&
            actor->impulse==NONE&&actor->speech_sound==NONE&&actor->user_animation_graph==NONE&&
            actor->played_impulse_number==8,"old actor controls survived or lost deduplication");
        distributed_handle_actor_states(90,&entry,1);
        CHECK(actor_snapshots_applied==1,"actor applied before the destination loaded");
        main_switch_to_structure_bsp_private();
        CHECK(global_structure_bsp_index==destination,"client did not reach the destination");
        distributed_handle_actor_states(89,&entry,1);
        distributed_handle_unit_states(89,&entry,&entry+1,1);
        CHECK(actor_snapshots_applied==1&&!unit_snapshots_applied,"source snapshots crossed the BSP boundary");
        distributed_handle_actor_states(90,&entry,1);
        CHECK(actor_snapshots_applied==2,"destination actors did not resume on the transition tick");
        distributed_handle_structure_bsp(110,&message);
        distributed_handle_actor_states(109,&entry,1);
        distributed_handle_unit_states(109,&entry,&entry+1,1);
        CHECK(actor_snapshots_applied==3&&unit_snapshots_applied==1&&
            actor_controls_released==1,"periodic BSP confirmation moved the boundary");
        return 0;
    }
    if(!strcmp(name,"regional-late-join"))
    {
        struct distributed_structure_bsp message={destination,0};
        byte state=0;
        coop=FALSE;
        regional_match=TRUE;
        CHECK(distributed_bsp_world_ready(),"a legacy host would block joining BSP 0");
        regional_host_agreed=TRUE;

        distributed_handle_unit_states(89,&state,&state+1,1);
        distributed_handle_actor_states(89,&state,1);
        distributed_handle_bsp_world_snapshot(_distributed_message_object_states,89,&state,1);
        CHECK(!unit_snapshots_applied&&!actor_snapshots_applied&&!received_scene_types&&
              !distributed_bsp_world_ready(),"processed snapshots before the first announced BSP");

        distributed_handle_structure_bsp(90,&message);
        CHECK(!distributed_bsp_world_ready(),"expected geometry was marked ready before load");
        main_switch_to_structure_bsp_private();
        CHECK(distributed_bsp_world_ready()&&global_structure_bsp_index==destination,
              "the announced region did not become ready");
        distributed_handle_unit_states(89,&state,&state+1,1);
        CHECK(!unit_snapshots_applied,"loaded region accepted pre-announcement sample");
        distributed_handle_unit_states(90,&state,&state+1,1);
        CHECK(unit_snapshots_applied==1,"post-load samples did not resume");
        return 0;
    }
    if(!strcmp(name,"host-object-snapshot-boundary"))
    {
        struct distributed_structure_bsp message={destination,0};
        byte entry=0;

        objects_client_unsteered_path[0].object_index=units[2];
        objects_client_unsteered_path[0].time=87;
        objects_client_own_vehicles[0][1].vehicle_index=units[2];
        objects_client_own_vehicles[0][1].time=87;
        distributed_handle_structure_bsp(90,&message);
        CHECK(objects_client_unsteered_path[0].object_index==NONE&&
              objects_client_unsteered_path[0].time==NONE&&
              objects_client_own_vehicles[0][1].vehicle_index==NONE&&
              objects_client_own_vehicles[0][1].time==NONE,
              "reused the old region's vehicle interpolation");
        distributed_handle_bsp_world_snapshot(_distributed_message_object_states,90,&entry,1);
        distributed_handle_bsp_world_snapshot(_distributed_message_coop_object_transforms,90,&entry,1);
        distributed_handle_bsp_world_snapshot(_distributed_message_coop_device_states,90,&entry,1);
        CHECK(!received_scene_types,"applied geometry before the destination loaded");

        main_switch_to_structure_bsp_private();
        CHECK(network_coop_note_player_structure_bsp(0,old)&&!host_resend.scene_refresh,
              "resends started before the client had the current BSP");
        CHECK(network_coop_note_player_structure_bsp(0,destination)&&host_resend.scene_refresh,
              "the loaded guest missed the scene refresh");
        host_resend.scene_refresh=FALSE;
        CHECK(!network_coop_note_player_structure_bsp(0,destination)&&!host_resend.scene_refresh,
              "duplicate acknowledgement caused another refresh");

        distributed_handle_bsp_world_snapshot(_distributed_message_object_states,89,&entry,1);
        distributed_handle_bsp_world_snapshot(_distributed_message_coop_object_transforms,89,&entry,1);
        distributed_handle_bsp_world_snapshot(_distributed_message_coop_device_states,89,&entry,1);
        CHECK(!received_scene_types,"applied a late object from the source BSP");
        distributed_handle_bsp_world_snapshot(_distributed_message_object_states,90,&entry,1);
        distributed_handle_bsp_world_snapshot(_distributed_message_coop_object_transforms,90,&entry,1);
        distributed_handle_bsp_world_snapshot(_distributed_message_coop_device_states,90,&entry,1);
        CHECK(received_scene_types==7,"destination objects and devices did not resume");
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
        if (!strncmp(name,"connected-parallel-",19))
        {
            parallel_routes = TRUE;
            scenario.bsp_switch_trigger_volumes.count = 2;
            switches[1]=(struct scenario_bsp_switch_trigger_volume){1,old,destination,NONE};
            if (!strcmp(name,"connected-parallel-foot"))
                objects[1].object.parent_object_index=NONE;
            else if (!strcmp(name,"connected-parallel-marked"))
                switches[1].cutscene_flag_index=0;
            else if (!strcmp(name,"connected-parallel-other-destination"))
                switches[1].destination_structure_bsp_index=2;
            else if (!strcmp(name,"connected-parallel-other-source"))
                switches[1].source_structure_bsp_index=2;
        }
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
    if(!strcmp(name,"activation-destination"))
        return test_destination_activation();

    memcpy(before,objects,sizeof(before));
    if(!strcmp(name,"reference-refused")||!strncmp(name,"mcc-",4))
    {
        struct mcc_cache_bsp checked={.file_offset=4096,.size=1024,.address=0x40001000,.tag_handle=(uint32_t)destination,
                                      .structure_address=0x40001018};
        if(!strncmp(name,"mcc-",4))
        {
            struct scenario_structure_bsp_reference *ref=&references[destination];
            mcc_loaded=TRUE;
            mcc_loaded_cache.report.bsp_count=1;
            mcc_loaded_cache.report.bsps=&checked;
            ref->file_offset=checked.file_offset;
            ref->file_size=checked.size;
            ref->base_address=checked.address;
            mcc_file_structure.collision_bsp.count=1;
            mcc_file_structure.clusters.count=2;
            mcc_file_structure.cluster_data.size=8;
            mcc_file_structure.sound_cluster_data.size=1;
            CHECK(scenario_structure_bsp_reference_is_valid(ref),"matching MCC reference refused");
            if (!strcmp(name,"mcc-neighbor-geometry")) {
                mcc_file_structure.clusters.count=MAXIMUM_CLUSTERS_PER_STRUCTURE+1;
            } else if (!strcmp(name,"mcc-neighbor-visibility")) {
                mcc_file_structure.cluster_data.size=7;
            } else if (!strcmp(name,"mcc-neighbor-truncated")) {
                mcc_file_read_failed=TRUE;
            } else if (!strcmp(name,"mcc-neighbor-range")) {
                checked.structure_address=checked.address+checked.size;
            } else {
                ref->base_address++;
                CHECK(!mcc_cache_bsp_reference_checked(ref),"changed MCC address accepted");
                ref->base_address--;
                ref->file_size++;
                CHECK(!mcc_cache_bsp_reference_checked(ref),"changed MCC size accepted");
                ref->file_size--;
                ref->file_offset++;
            }
        }
        else
            reject_reference[destination]=TRUE;
        object_globals->pvs_activation_type=_pvs_activation_cluster;
        object_globals->pvs_activation.cluster_index=2;
        CHECK(!scenario_switch_structure_bsp(destination),"invalid BSP reference was accepted");
        if (!strcmp(name,"mcc-neighbor-range"))
            CHECK(mcc_file_reads==1,"out-of-range header caused a file read");
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
                else if(!strcmp(name,"connected-stranded")||!strcmp(name,"connected-unrelated")||
                        !strcmp(name,"connected-parallel-marked")||!strncmp(name,"connected-parallel-other-",25))
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
