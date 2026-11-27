/* Native encounter follow/migration regression world; no assets or AI replacement. */
#include "harness.h"
#include <float.h>
#include <math.h>
#include <stdarg.h>
#include <stdlib.h>
#include "limits.inc"

#define REAL_MAX FLT_MAX
#define REAL_MIN (-FLT_MAX)
#define MAXIMUM_SQUADS_PER_ENCOUNTER 64
#define MIN(a,b) ((a)<(b)?(a):(b))
#define VALID_INDEX(i,n) ((i)>=0&&(i)<(n))
#define BIT_VECTOR_SIZE_IN_LONGS(n) (((n)+31)/32)
#define BIT_VECTOR_SET_FLAG(v,i,b) ((v)[(i)/32] = ((v)[(i)/32]&~(1UL<<((i)%32)))|((b)?(1UL<<((i)%32)):0))
#define BIT_VECTOR_TEST_FLAG(v,i) (((v)[(i)/32]&(1UL<<((i)%32)))!=0)
#define csmemset memset
#define square_root sqrtf
#define _error_silent 0
#define _object_mask_unit 1
#define _object_dead_bit 0
enum {_scenario_type_multiplayer=1};

struct tag_block { long count; void *address; };
#define TAG_BLOCK_GET_ELEMENT(b,i,t) (&((t *)(b)->address)[i])
struct squad_definition { char name[32]; unsigned long flags; short platoon_index; unsigned long firing_position_groups[8]; };
struct firing_position_definition { real_point3d position; short group_index; };
struct encounter_definition { char name[32]; struct tag_block squads, platoons, firing_positions; short runtime_structure_bsp_reference_index; };
struct scenario { struct tag_block ai_encounters, structure_bsp_references; short type; };
struct squad_datum { short current_count; boolean automatic_migration_target; };
struct platoon_datum { boolean defending; };
struct encounter_datum { short follow_target_type, squad_count, team_index, current_count; long follow_target_unit_index, follow_target_ai_index; real follow_target_distance; short remain_active_timer; long last_active_time; boolean active; };
struct actor_datum {
 struct { short squad_index,disconnected_squad_index; long unit_index,swarm_unit_index,encounter_index,disconnected_encounter_index; boolean active,encounterless,swarm,erase_next_tick; short swarm_unit_count; long last_active_time; } meta;
 struct { short mode; } state;
 struct { struct { real_point3d body_position; } position; } input;
};
struct unit_datum {
 struct { unsigned long damage_flags; real_point3d position; short owner_team_index; long parent_object_index,owner_player_index; struct {short cluster_index;} location; } object;
 struct { long player_index,actor_index,swarm_actor_index,swarm_next_unit_index; } unit;
};
struct player_datum { long unit_index; boolean quit_out_of_game; };
struct data_iterator { long datum_index, next; };
struct encounter_actor_iterator { long index, next; };
struct encounter_iterator {long index;};
struct ai_script_actor_reference_iterator { long next; };

static struct scenario scenario;
static struct encounter_definition definition;
static struct encounter_datum encounter;
static struct squad_definition squad_definitions[3];
static struct squad_datum squads[3];
static struct firing_position_definition firing_positions[2];
static struct player_datum players[10];
static struct unit_datum units[11];
static long unit_handles[11];
static short player_count=6;
static long incapacitated_unit=NONE;
static struct actor_datum actors[2];
static short actor_count=1, migrations;
static boolean operation=TRUE;
static void *player_data;
static struct { boolean print_automatic_migration; } ai_debug;

static struct scenario *global_scenario_get(void) { return &scenario; }
#define global_scenario_try_and_get global_scenario_get
static struct { boolean valid; } fake_actor_data, *actor_data=&fake_actor_data;
static struct actor_datum *actor_try_and_get(long index) { return index==0x20000 ? &actors[0] : NULL; }
static boolean extraction_owns_map(void) { return operation; }
static boolean game_team_is_enemy(short team,short other) { return team!=other; }
static boolean extraction_medical_incapacitated(long unit_index) { return unit_index==incapacitated_unit; }
static struct encounter_datum *encounter_get(long index) { CHECK(index==0,"unexpected encounter");return &encounter; }
static struct squad_datum *encounter_get_squad(struct encounter_datum *e,short index) { (void)e;return &squads[index]; }
static struct platoon_datum *encounter_get_platoon(struct encounter_datum *e,short index) { (void)e;(void)index;return NULL; }
static void data_iterator_new(struct data_iterator *i,void *data) { (void)data;i->next=0; }
static void *data_iterator_next(struct data_iterator *i) { if(i->next==player_count)return NULL;i->datum_index=0x10000+i->next;return &players[i->next++]; }
static struct unit_datum *unit_try_and_get(long index) { short i;for(i=0;i<11;i++)if(index==unit_handles[i])return &units[i];return NULL; }
static void *object_try_and_get_and_verify_type(long index,long mask) { (void)mask;return unit_try_and_get(index); }
static void object_get_origin(long index,real_point3d *position) { struct unit_datum *u=unit_try_and_get(index);CHECK(u,"stale unit was sampled");*position=u->object.position; }
static real distance_squared3d(real_point3d const *a,real_point3d const *b) { real x=a->x-b->x,y=a->y-b->y,z=a->z-b->z;return x*x+y*y+z*z; }
static void encounter_actor_iterator_new(struct encounter_actor_iterator *i,long index) { (void)index;i->next=0; }
static struct actor_datum *encounter_actor_iterator_next(struct encounter_actor_iterator *i) { if(i->next==actor_count)return NULL;i->index=i->next;return &actors[i->next++]; }
static void ai_index_actor_iterator_new(long index,struct ai_script_actor_reference_iterator *i) { (void)index;i->next=0; }
static struct actor_datum *ai_index_actor_iterator_next(struct ai_script_actor_reference_iterator *i) { (void)i;return NULL; }
static void actor_change_encounter(long actor,long index,short squad) { (void)index;squads[actors[actor].meta.squad_index].current_count--;squads[squad].current_count++;actors[actor].meta.squad_index=squad;migrations++; }
static void test_error(long severity,char const *format,...) { (void)severity;(void)format; }
static void console_printf(boolean warning,char const *format,...) { (void)warning;(void)format; }
#define error test_error
static short global_structure_bsp_index;
static short global_structure_bsp_index_get(void) { return global_structure_bsp_index; }
static boolean mcc_loaded;
static boolean mcc_cache_tags_loaded(void) { return mcc_loaded; }
static long test_game_time=100;
static long game_time_get(void) { return test_game_time; }
static void encounter_clear_pursuit(long index) { (void)index; }
static void actor_verify_activation(long index) { CHECK(VALID_INDEX(index,actor_count),"invalid actor"); }
static void actor_set_active(long index,boolean active) { actors[index].meta.active=active; }
static boolean regional_ready;
static short unit_bsp = NONE, resident_bsp = NONE;
static unsigned long occupied_override;
static boolean use_occupied_override;
#define MAXIMUM_CLUSTERS_PER_STRUCTURE 32
#define MAXIMUM_STRUCTURE_BSPS_PER_SCENARIO 16
#ifndef BIT_VECTOR_SIZE_IN_LONGS
#define BIT_VECTOR_SIZE_IN_LONGS(n) (((n)+31)/32)
#endif
#ifndef BIT_VECTOR_TEST_FLAG
#define BIT_VECTOR_TEST_FLAG(vector,i) TEST_FLAG((vector)[(i)/32],(i)%32)
#endif
static struct structure_bsp { struct {long count;} clusters; } test_bsp = {{8}};
static unsigned long remote_mask;
static short remote_pvs_calls;
static struct structure_bsp const *global_structure_bsp_get(void) { return &test_bsp; }
static long object_get_ultimate_parent(long unit) { return unit; }
static void players_compute_regional_pvs(short bsp,struct structure_bsp const *structure,unsigned long *pvs)
{ CHECK(bsp==1&&structure==&test_bsp,"computed visibility for the wrong region"); *pvs=remote_mask; remote_pvs_calls++; }
static unsigned long players_occupied_regional_bsps(void) { return use_occupied_override ? occupied_override : resident_bsp==NONE ? 0 : FLAG(resident_bsp); }
static boolean network_regional_independent_bsp_ready(void) { return regional_ready; }
static struct structure_bsp const *scenario_structure_bsp_try_and_get(short bsp) { return bsp==resident_bsp ? &test_bsp : NULL; }
static struct structure_bsp const *encounter_remote_structure(short bsp,unsigned long occupied)
{return bsp>=0 && TEST_FLAG(occupied,bsp) ? scenario_structure_bsp_try_and_get(bsp) : NULL;}
static struct actor_datum *actor_get(long index) { CHECK(index==0x20000,"unexpected actor");return &actors[0]; }
static void *object_try_and_get(long unit) { return unit_try_and_get(unit); }
static boolean case_name_is_actor_tick, radar_fixture;
static short radar_regions[4];
static short object_get_regional_bsp(long unit) {
 if(radar_fixture)
 {
  short i;
  for(i=0;i<4;i++)
   if(unit==unit_handles[i])
    return radar_regions[i];
  CHECK(FALSE,"unexpected radar contact");
 }
 if(unit==unit_handles[1] && case_name_is_actor_tick) return 0;
 CHECK(unit==unit_handles[0],"unexpected actor body");
 return unit_bsp;
}
static void encounterless_detach_actor(long actor) { actor_get(actor)->meta.encounterless=FALSE; }
static void encounter_attach_actor(long actor,long encounter_index,short squad,boolean previous_team)
{
 struct actor_datum *data=actor_get(actor);
 CHECK(!data->meta.encounterless&&previous_team,"native reattach lost prior state");
 data->meta.encounter_index=encounter_index;
 data->meta.squad_index=squad;
 data->meta.disconnected_encounter_index=NONE;
}
enum {_game_connection_network_server=1, _actor_mode_braindead=0};
static struct {short last_highest_service_timer,current_highest_service_timer; boolean time_given_this_frame;} ai_state, *ai_globals=&ai_state;
struct actor_iterator {long index;};
static short actor_updates, actor_erasures;
static unsigned long actors_reclaimed;
static short connection=_game_connection_network_server;
static short game_connection(void) {return connection;}
static void actor_iterator_new(struct actor_iterator *i,boolean active) {(void)active;i->index=NONE;}
static struct actor_datum *actor_iterator_next(struct actor_iterator *i) {return ++i->index<actor_count ? &actors[i->index] : NULL;}
static void actor_update(long index) {CHECK(index==0,"wrong actor updated");actor_updates++;}
static void actor_erase(long index,boolean delete_unit)
{
 CHECK(VALID_INDEX(index,actor_count),"invalid actor erased");
 if (delete_unit)
  actors_reclaimed|=FLAG(index);
 else
  actor_erasures++;
}
static void encounter_iterator_new(struct encounter_iterator *iterator,boolean active)
{ (void)active; iterator->index=NONE; }
static struct encounter_datum *encounter_iterator_next(struct encounter_iterator *iterator)
{ return ++iterator->index==0 ? &encounter : NULL; }
#define object_datum unit_datum
#include "under_test.inc"
typedef unsigned long tag;
enum {OBJECT_DEFINITION_TAG=1,DAMAGE_EFFECT_DEFINITION_TAG,SOUND_DEFINITION_TAG};
enum {_effect_environment_anywhere,_effect_environment_air,_effect_environment_water,_effect_environment_vacuum};
struct location {short cluster_index;};
struct effect_datum {struct {short identifier;unsigned long flags;} header;short fixed_bsp,local_player_index;long object_index,owner_object_index,definition_index;
 struct location location;long location_datum_indices[1];};
struct effect_part_definition {tag runtime_base_class_tag;struct {long index;} reference;};
struct effect_event_definition {struct tag_block parts;};
struct effect_definition {unsigned long flags;struct tag_block events,locations;};
struct damage_data {long owner_player_index,owner_object_index;short owner_team_index;real scale;
 struct location location;real_point3d origin,epicenter;real_vector3d direction;};
static struct damage_data damage_readback;
static short damage_bsp,damage_delivered;
static void damage_data_new(struct damage_data *data,long definition_index)
{ CHECK(definition_index==7,"wrong damage tag");memset(data,0,sizeof(*data)); }
static void area_of_effect_cause_damage_in_bsp(short bsp,struct damage_data *data)
{damage_bsp=bsp;damage_readback=*data;damage_delivered++;}
static void scenario_location_from_point_in_structure(struct location *location,
 struct structure_bsp const *structure,real_point3d const *point)
{
 CHECK(structure==&test_bsp&&point->x==37,"damage located in the wrong collision scene");
 location->cluster_index=4;
}
static boolean scenario_location_underwater_in_structure(struct structure_bsp const *structure,
 struct location const *location,real_point3d const *point,short *weather)
{
 CHECK(structure==&test_bsp&&location->cluster_index==4&&!weather&&point->x==37,
       "water was checked using the foreground cluster");
 return TRUE;
}
static boolean scenario_location_underwater(struct location const *location,real_point3d const *point,short *weather)
{ (void)location;(void)point;(void)weather;return FALSE; }
struct effect_location_datum {struct datum_header header;long next_instance_location_index;};
static struct effect_definition effect_definitions[3];
static struct data_array *effect_data,*effect_location_data;
static boolean effects_port_harmless;
#define NUMBEROF(a) (sizeof(a)/sizeof((a)[0]))
static boolean game_in_editor(void) {return FALSE;}
#define effect_get(index) ((struct effect_datum *)datum_get(effect_data,index))
#define effect_definition_get(index) (&effect_definitions[index])
#define effect_location_get(index) ((struct effect_location_datum *)datum_get(effect_location_data,index))
static struct effect_datum *effect_try_and_get(long index)
{
 struct effect_datum *effect=effect_get(index);
 return (unsigned short)effect->header.identifier==((unsigned long)index>>16) ? effect : NULL;
}
static long data_next_index(struct data_array *array,long index)
{
 long slot=(index==NONE ? 0 : DATUM_INDEX_TO_ABSOLUTE_INDEX(index)+1);
 for (;slot<array->maximum_count;slot++)
 {
  struct datum_header *header=datum_get(array,slot);
  if (header->identifier)
   return ((long)(unsigned short)header->identifier<<16)|slot;
 }
 return NONE;
}
static void effect_set_event(long index,short event)
{
 CHECK(effect_get(index)&&event==0,"effect did not start its first native event");
}
#define match_vassert(file,line,condition,description) CHECK(condition,"native assertion at line %d",line)
#define SET_FLAG(flags,bit,value) ((flags)=((flags)&~FLAG(bit))|((value)?FLAG(bit):0))
#include "regional_combat_effects.inc"
#undef SET_FLAG
#undef object_datum

typedef struct {real x,y;} real_point2d;
typedef unsigned short word;
enum {MAXIMUM_LOCAL_PLAYERS=4};
static short local_player_count(void) {return 2;}
static short local_player_get_next(short index) {return index==NONE ? 0 : index==0 ? 2 : NONE;}
static long local_player_get_player_index(short index) {return index==0 ? 0 : index==2 ? 1 : NONE;}
static struct player_datum *player_get(long index) {return &players[index];}
static boolean game_engine_running(void) {return TRUE;}
static long get_return_eip(void) {return 0;}
static void game_engine_motion_sensor_viewer(short index) {(void)index;}
static void unit_get_camera_position(long unit,real_point3d *point) {*point=unit_try_and_get(unit)->object.position;}
static boolean should_track_object(long unit) {return unit_try_and_get(unit)!=NULL;}
static boolean should_draw_object(long unit) {return should_track_object(unit);}
static void object_get_bounding_sphere(long unit,real_point3d *point,real *radius)
{unit_get_camera_position(unit,point);*radius=1;}
static real magnitude_squared3d(real_vector3d const *vector)
{return vector->i*vector->i+vector->j*vector->j+vector->k*vector->k;}
struct object_iterator {long index; short next;};
static void object_iterator_new(struct object_iterator *iterator,long mask,boolean active)
{(void)mask;(void)active;iterator->next=0;}
static boolean object_iterator_next(struct object_iterator *iterator)
{
 if(iterator->next==4)return FALSE;
 iterator->index=unit_handles[iterator->next++];
 return TRUE;
}
static struct {struct {real motion_sensor_range;} defaults;} radar_hud={{15}}, *hud_globals=&radar_hud;
#define csmemcpy memcpy
#include "regional_motion_sensor.inc"
#undef csmemcpy

int main(int argc,char **argv)
{
 char const *case_name=argc>1?argv[1]:"";short i,expected=0;
 scenario.ai_encounters.count=1;scenario.ai_encounters.address=&definition;
 if(!strcmp(case_name,"regional-motion-sensor"))
 {
  struct motion_sensor_datum *first,*second;
  radar_fixture=regional_ready=TRUE;
  test_game_time=0;
  for(i=0;i<4;i++)
  {
   unit_handles[i]=0x30000+i;
   radar_regions[i]=i%2;
   units[i].object.position=(real_point3d){0,0,(real)(10*(i%2))};
  }
  players[0].unit_index=unit_handles[0];
  players[1].unit_index=unit_handles[1];
  motion_sensor_update();
  first=&sensor_storage.players[0].sensor_data[sensor_storage.active_sensor_index];
  second=&sensor_storage.players[2].sensor_data[sensor_storage.active_sensor_index];
  CHECK(first->blip_count==2&&second->blip_count==2&&first->region==1&&second->region==2,
        "a split-screen tracker admitted contacts on another floor");
  CHECK(sensor_storage.players[0].unit_indices[1]==unit_handles[2]&&
        sensor_storage.players[2].unit_indices[1]==unit_handles[3],"contacts reached the wrong viewport");
  radar_regions[1]=0;
  test_game_time=1;
  motion_sensor_update();
  second=&sensor_storage.players[2].sensor_data[sensor_storage.active_sensor_index];
  CHECK(!motion_sensor_regions_match(TRUE,motion_sensor_unit_region(TRUE,unit_handles[1]),second->region),
        "a crossing accepted a previous region's sweep");
  CHECK(!motion_sensor_regions_match(TRUE,0,0),"unqualified contacts were displayed");
  regional_ready=FALSE;
  test_game_time=15;
  motion_sensor_update();
  first=&sensor_storage.players[0].sensor_data[sensor_storage.active_sensor_index];
  CHECK(first->blip_count==4,"classic multiplayer lost its native height-independent tracker");
  return 0;
 }
 if(!strcmp(case_name,"regional-effect-pool"))
 {
  struct effect_part_definition damaged={DAMAGE_EFFECT_DEFINITION_TAG,{7}};
  struct effect_event_definition gameplay_event={.parts={1,&damaged}};
  struct effect_event_definition cosmetic_event={0};
  long old_gameplay, old_cosmetic, gameplay_location, cosmetic_location, replacement;
  effect_data=game_state_data_new("effects",2,sizeof(struct effect_datum));
  effect_location_data=game_state_data_new("effect locations",2,sizeof(struct effect_location_datum));
  old_gameplay=datum_new(effect_data);
  old_cosmetic=datum_new(effect_data);
  gameplay_location=datum_new(effect_location_data);
  cosmetic_location=datum_new(effect_location_data);
  effect_location_get(gameplay_location)->next_instance_location_index=NONE;
  effect_location_get(cosmetic_location)->next_instance_location_index=NONE;
  effect_get(old_gameplay)->definition_index=0;
  effect_get(old_cosmetic)->definition_index=1;
  effect_get(old_gameplay)->location_datum_indices[0]=gameplay_location;
  effect_get(old_cosmetic)->location_datum_indices[0]=cosmetic_location;
  effect_definitions[0]=(struct effect_definition){.events={1,&gameplay_event},.locations={1,NULL}};
  effect_definitions[1]=(struct effect_definition){.events={1,&cosmetic_event},.locations={1,NULL}};
  effect_definitions[2]=effect_definitions[0];

  regional_ready=FALSE;
  CHECK(effect_allocate(2,NONE,TRUE)==NONE&&effect_location_data->count==2,
        "classic lost its allocation policy");
  regional_ready=TRUE;
  replacement=effect_allocate(2,NONE,TRUE);
  CHECK(replacement!=NONE&&effect_get(replacement)==effect_get(old_cosmetic),
        "regional gameplay did not reclaim a presentation slot");
  CHECK(effect_try_and_get(old_gameplay)&&!effect_try_and_get(old_cosmetic)&&
        effect_location_get(gameplay_location)->header.identifier&&effect_location_data->count==1,
        "regional eviction lost gameplay or leaked the cosmetic effect's locations");
  CHECK(effect_allocate(2,NONE,TRUE)==NONE&&effect_data->count==2,
        "a full regional gameplay pool discarded an active gameplay effect");
  return 0;
 }
 if(!strcmp(case_name,"regional-effect-content"))
 {
  struct effect_part_definition parts[]={{SOUND_DEFINITION_TAG,{7}},{DAMAGE_EFFECT_DEFINITION_TAG,{NONE}}};
  struct effect_event_definition event={.parts={2,parts}};
  struct effect_definition definition={.flags=0,.events={1,&event}};
  struct effect_datum cosmetic={0}, gameplay={0};
  CHECK(!effect_has_regional_gameplay(&cosmetic,&definition),"sound or missing damage was classified as gameplay");
  parts[1].reference.index=7;
  CHECK(effect_has_regional_gameplay(&gameplay,&definition),"unflagged damage in a map effect was dropped");
  CHECK(!effect_has_regional_gameplay(&cosmetic,&definition),"effect rescanned its definition after classification");
  CHECK(effect_definition_has_gameplay(&definition),"checkpoint cleanup missed the same gameplay part");
  regional_ready=TRUE;
  resident_bsp=1;
  use_occupied_override=TRUE;
  occupied_override=FLAG(1);
  CHECK(effect_remote_simulation_structure(1,effect_has_regional_gameplay(&gameplay,&definition))==&test_bsp,
        "host skipped an unflagged map effect in an occupied region");
  CHECK(!effect_remote_simulation_structure(1,effect_has_regional_gameplay(&cosmetic,&definition)),
        "cosmetic effect advanced in the gameplay pass");
  return 0;
 }
 if(!strcmp(case_name,"regional-effect-damage"))
 {
  struct effect_datum effect={.object_index=0x30000,.owner_object_index=0x30001,.location={1}};
  struct effect_part_definition part={.runtime_base_class_tag=DAMAGE_EFFECT_DEFINITION_TAG,.reference={7}};
  real_point3d point={37,0,0};
  real_vector3d direction={1,0,0};
  short bsp;
  unit_handles[0]=effect.object_index;
  unit_handles[1]=effect.owner_object_index;
  units[1].object.owner_player_index=100;
  units[1].object.owner_team_index=2;
  regional_ready=TRUE;
  unit_bsp=resident_bsp=1;
  use_occupied_override=TRUE;
  CHECK(effect_simulation_bsp(&effect)==1,"attached effect inherited foreground BSP");
  CHECK(!effect_remote_simulation_structure(1,TRUE),"empty BSP ran remote gameplay");
  occupied_override=FLAG(1);
  resident_bsp=NONE;
  CHECK(!effect_remote_simulation_structure(1,TRUE),"nonresident BSP ran remote gameplay");
  resident_bsp=1;
  CHECK(!effect_remote_simulation_structure(1,FALSE),"presentation effect advanced as gameplay");
  connection=0;
  CHECK(!effect_remote_simulation_structure(1,TRUE),"guest ran authoritative effect");
  connection=_game_connection_network_server;
  CHECK(effect_part_is_regional_gameplay(OBJECT_DEFINITION_TAG)&&
        effect_part_is_regional_gameplay(DAMAGE_EFFECT_DEFINITION_TAG)&&
        !effect_part_is_regional_gameplay(SOUND_DEFINITION_TAG),"remote pass used a foreground-only presentation list");
  CHECK(effect_allowed_by_environment(_effect_environment_water,&effect.location,&point,&test_bsp)&&
        !effect_allowed_by_environment(_effect_environment_air,&effect.location,&point,&test_bsp)&&
        effect_allowed_by_environment(_effect_environment_air,&effect.location,&point,NULL),
        "regional effect did not honor its own water/air limits");
  bsp=effect_simulation_bsp(&effect);
  dispatch_damage_effect(&effect,&part,&point,&direction,1,bsp,effect_remote_simulation_structure(bsp,TRUE));
  CHECK(damage_delivered==1&&damage_bsp==1&&damage_readback.location.cluster_index==4&&
        damage_readback.owner_object_index==effect.owner_object_index&&damage_readback.owner_player_index==100&&
        damage_readback.owner_team_index==2,"damage did not retain its effect region and attacker");
  effect.header.flags=FLAG(_effect_port_fixed_bsp_bit);
  effect.fixed_bsp=0;
  CHECK(effect_simulation_bsp(&effect)==0,"fixed effect followed an attached object");
  effect.header.flags=FLAG(_effect_port_harmless_bit);
  dispatch_damage_effect(&effect,&part,&point,&direction,1,1,&test_bsp);
  CHECK(damage_delivered==1,"harmless copy dealt authoritative damage");
  effect.header.flags=0;
  regional_ready=FALSE;
  CHECK(effect_simulation_bsp(&effect)==NONE,"legacy attachment changed BSP routing");
  return 0;
 }
 if(!strcmp(case_name,"regional-pressure"))
 {
  struct potentially_releasable_storage pending;
  char description[64];
  boolean more;
  scenario.type=_scenario_type_multiplayer;
  scenario.structure_bsp_references.count=2;
  mcc_loaded=TRUE;
  definition.runtime_structure_bsp_reference_index=1;
  actor_count=2;
  for(i=0;i<actor_count;i++)
  {
   actors[i].meta.encounterless=TRUE;
   actors[i].meta.encounter_index=NONE;
   actors[i].meta.disconnected_encounter_index=NONE;
   actors[i].meta.last_active_time=10+i;
  }
  actors[0].meta.disconnected_encounter_index=0;
  ai_find_inactive_encounters((byte *)&pending,sizeof(pending));
  CHECK(pending.count==1&&pending.entities[0].is_actor&&pending.entities[0].entity_index==1,
        "pressure ranked a detached authored actor for deletion");
  actors[0].meta.swarm=actors[1].meta.swarm=TRUE;
  actors[0].meta.swarm_unit_count=actors[1].meta.swarm_unit_count=1;
  CHECK(ai_release_inactive_swarms(description,&more,NULL,0)&&actors_reclaimed==FLAG(1),
        "swarm pressure lost an authored actor or spared disposable actors");
  actors_reclaimed=0;
  mcc_loaded=FALSE;
  ai_find_inactive_encounters((byte *)&pending,sizeof(pending));
  CHECK(pending.count==2,"classic inactive-actor reclamation changed");
  CHECK(ai_release_inactive_swarms(description,&more,NULL,0)&&actors_reclaimed==(FLAG(0)|FLAG(1)),
        "classic swarm reclamation changed");
  return 0;
 }
 if(!strcmp(case_name,"regional-actor-tick"))
 {
  case_name_is_actor_tick=TRUE;
  scenario.type=_scenario_type_multiplayer;
  scenario.structure_bsp_references.count=2;
  mcc_loaded=regional_ready=TRUE;
  unit_handles[0]=0x30000;
  unit_handles[1]=0x30001;
  actors[0].meta.unit_index=unit_handles[0];
  actors[0].state.mode=1;
  unit_bsp=resident_bsp=1;
  use_occupied_override=TRUE;
  actors_update();
  CHECK(actor_updates==0,"actor advanced in a warm but empty region");
  occupied_override=FLAG(1);
  resident_bsp=NONE;
  actors_update();
  CHECK(actor_updates==0,"actor advanced without resident collision");
  resident_bsp=1;
  actors_update();
  CHECK(actor_updates==1,"returning player did not resume the same actor");
  actors[0].meta.swarm=TRUE;
  actors[0].meta.swarm_unit_index=unit_handles[1];
  actors[0].meta.swarm_unit_count=2;
  units[1].unit.swarm_next_unit_index=unit_handles[0];
  units[0].unit.swarm_next_unit_index=NONE;
  occupied_override=0;
  actors_update();
  CHECK(actor_updates==1,"a swarm advanced orders with a member in an empty region");
  regional_ready=FALSE;
  actors_update();
  CHECK(actor_updates==2,"unnegotiated simulation changed");
  regional_ready=TRUE;
  actors[0].meta.erase_next_tick=TRUE;
  actors_update();
  CHECK(actor_updates==2&&actor_erasures==1,"pending cleanup was blocked by region gating");
  return 0;
 }
 if(!strcmp(case_name,"regional-encounterless-pvs"))
 {
  unsigned long foreground=FLAG(2);
  struct encounter_activation_visibility visibility;
  memset(&visibility,0,sizeof(visibility));
  visibility.regional=TRUE;
  visibility.foreground=&foreground;
  visibility.occupied=FLAG(1);
  global_structure_bsp_index=0;
  resident_bsp=1;
  unit_handles[0]=0x10001;
  units[0].object.location.cluster_index=2;
  unit_bsp=1;
  CHECK(!encounterless_unit_is_visible(unit_handles[0],&visibility),"foreground cluster leaked into the remote actor's visibility");
  remote_mask=FLAG(2);
  visibility.computed=0;
  CHECK(encounterless_unit_is_visible(unit_handles[0],&visibility),"remote player could not wake the actor");
  CHECK(encounterless_unit_is_visible(unit_handles[0],&visibility)&&remote_pvs_calls==2,"regional PVS was rebuilt for an actor in the same region");
  visibility.occupied=0;
  CHECK(!encounterless_unit_is_visible(unit_handles[0],&visibility),"a warm BSP kept an unoccupied actor visible");
  unit_bsp=0;
  CHECK(encounterless_unit_is_visible(unit_handles[0],&visibility),"foreground visibility was lost");
  units[0].object.location.cluster_index=33;
  CHECK(!encounterless_unit_is_visible(unit_handles[0],&visibility),"invalid cluster read outside the visibility vector");
  return 0;
 }
 if(!strcmp(case_name,"regional-script-reattach"))
 {
  scenario.type=_scenario_type_multiplayer;
  scenario.structure_bsp_references.count=2;
  mcc_loaded=regional_ready=TRUE;
  global_structure_bsp_index=0;
  definition.runtime_structure_bsp_reference_index=1;
  unit_handles[0]=0x10001;
  actors[0].meta.unit_index=unit_handles[0];
  actors[0].meta.swarm_unit_index=NONE;
  actors[0].meta.encounterless=TRUE;
  actors[0].meta.disconnected_encounter_index=0;
  actors[0].meta.disconnected_squad_index=2;
  unit_bsp=0;
  resident_bsp=1;
  CHECK(!ai_try_reconnect_disconnected_actor(0x20000),"actor attached to encounter across physical regions");
  unit_bsp=1;
  resident_bsp=NONE;
  CHECK(!ai_try_reconnect_disconnected_actor(0x20000),"actor attached to an unloaded encounter");
  resident_bsp=1;
  CHECK(ai_try_reconnect_disconnected_actor(0x20000),"resident remote actor waited for the host foreground");
  CHECK(actors[0].meta.squad_index==2&&actors[0].meta.disconnected_encounter_index==NONE,"reattach lost native squad or left pending state");
  CHECK(!ai_try_reconnect_disconnected_actor(0x20000),"actor reattached twice");
  return 0;
 }
 if(!strcmp(case_name,"regional-actor-ownership"))
 {
  struct unit_datum *unit=&units[0];
  long handle=0x10000;
  unit_handles[0]=handle;
  unit->object.parent_object_index=NONE;
  unit->unit.player_index=NONE;
  unit->unit.actor_index=0x20000;
  unit->unit.swarm_actor_index=NONE;
  actors[0].meta.encounter_index=0;
  definition.runtime_structure_bsp_reference_index=0;
  scenario.type=_scenario_type_multiplayer;
  scenario.structure_bsp_references.count=2;
  actor_data->valid=TRUE;

  global_structure_bsp_index=1;
  CHECK(!ai_unit_belongs_to_other_bsp(handle),"classic actor collision changed");
  mcc_loaded=TRUE;
  CHECK(ai_unit_belongs_to_other_bsp(handle),"the parked actor can collide in the wrong BSP");
  global_structure_bsp_index=0;
  CHECK(!ai_unit_belongs_to_other_bsp(handle),"the actor did not return to its home BSP");
  global_structure_bsp_index=1;

  unit->object.parent_object_index=0x30000;
  CHECK(!ai_unit_belongs_to_other_bsp(handle),"an AI passenger stopped following its vehicle");
  unit->object.parent_object_index=NONE;
  actors[0].meta.encounterless=TRUE;
  CHECK(!ai_unit_belongs_to_other_bsp(handle),"detached AI could not follow the crossing");
  actors[0].meta.encounterless=FALSE;
  unit->unit.player_index=0;
  CHECK(!ai_unit_belongs_to_other_bsp(handle),"a player-controlled unit was parked");
  unit->unit.player_index=NONE;
  unit->unit.swarm_actor_index=unit->unit.actor_index;
  unit->unit.actor_index=NONE;
  CHECK(ai_unit_belongs_to_other_bsp(handle),"a swarm body ignored its encounter");
  definition.runtime_structure_bsp_reference_index=2;
  CHECK(!ai_unit_belongs_to_other_bsp(handle),"an unassigned encounter pinned collision to a missing region");
  return 0;
 }
 if(!strcmp(case_name,"regional-reconnect"))
 {
  mcc_loaded=TRUE;
  scenario.type=_scenario_type_multiplayer;
  scenario.structure_bsp_references.count=2;
  definition.runtime_structure_bsp_reference_index=0;
  encounter.active=actors[0].meta.active=TRUE;
  encounter.remain_active_timer=62;
  encounter_force_deactivate(0,TRUE);
  CHECK(!encounter.active&&!actors[0].meta.active&&encounter.remain_active_timer==62,
        "parking reset the activation window or left an actor active");
  global_structure_bsp_index=1;
  encounters_reconnect_to_structure_bsp();
  CHECK(!encounter.active&&!actors[0].meta.active&&encounter.remain_active_timer==62,
        "woke the encounter in a foreign BSP");
  global_structure_bsp_index=0;
  encounters_reconnect_to_structure_bsp();
  CHECK(encounter.active&&actors[0].meta.active&&encounter.remain_active_timer==62,
        "the original actor did not resume with its remaining time");
  encounter_force_deactivate(0,FALSE);
  encounters_reconnect_to_structure_bsp();
  CHECK(!encounter.active&&!actors[0].meta.active&&!encounter.remain_active_timer,
        "woke an encounter with no activation time remaining");
  return 0;
 }
 definition.squads.count=3;definition.squads.address=squad_definitions;
 definition.firing_positions.count=2;definition.firing_positions.address=firing_positions;
 encounter.squad_count=3;encounter.follow_target_type=_follow_target_players;
 for(i=0;i<3;i++) {
  squad_definitions[i].platoon_index=NONE;
  if(i<2) { squad_definitions[i].flags=FLAG(_squad_automatic_migration_bit);
   squad_definitions[i].firing_position_groups[_firing_position_group_attacking]=FLAG(i);
   squads[i].automatic_migration_target=TRUE; }
 }
 for(i=0;i<11;i++) { unit_handles[i]=0x20000+i;units[i].unit.player_index=0x10000+i;units[i].object.position.x=100+i*20.f; }
 for(i=0;i<10;i++)players[i].unit_index=unit_handles[i];
 firing_positions[0].group_index=0;firing_positions[1].group_index=1;firing_positions[1].position.x=100;
 actors[0].meta.squad_index=1;squads[1].current_count=1;
 units[0].object.position.x=1;units[5].object.position.x=99;
 if(!strcmp(case_name,"reordered")||!strcmp(case_name,"sixth")) {units[0].object.position.x=99;units[5].object.position.x=1;}
 if(!strcmp(case_name,"tie")) {actors[0].input.position.body_position.x=50;}
 if(!strcmp(case_name,"outside-squad")) {actor_count=2;actors[1].meta.squad_index=2;actors[1].input.position.body_position.x=99;}
 if(!strcmp(case_name,"death-respawn")||!strcmp(case_name,"quit-stale")) {
  actors[0].meta.squad_index=0;squads[0].current_count=1;squads[1].current_count=0;expected=1;
  if(!strcmp(case_name,"death-respawn"))units[0].object.damage_flags=FLAG(_object_dead_bit);
  else { players[0].quit_out_of_game=TRUE;players[1].unit_index+=0x10000;units[2].unit.player_index=NONE;
   units[2].object.position.x=1;units[3].object.damage_flags=FLAG(_object_dead_bit);players[4].unit_index=NONE; }
 }
 if(!strcmp(case_name,"empty")) {for(i=0;i<6;i++)units[i].object.damage_flags=FLAG(_object_dead_bit);expected=1;}
 if(!strcmp(case_name,"opposing-nearer")) {units[0].object.owner_team_index=3;expected=1;}
 if(!strcmp(case_name,"only-opposition")) {for(i=0;i<6;i++)units[i].object.owner_team_index=3;expected=1;}
 if(!strcmp(case_name,"ninth")||!strcmp(case_name,"legacy-ninth")) {
  player_count=10;
  units[0].object.position.x=98;
  units[8].object.position.x=1;
 }
 if(!strcmp(case_name,"incapacitated-nearer")) {incapacitated_unit=unit_handles[0];expected=1;}

 if(!strcmp(case_name,"explicit")) {encounter.follow_target_type=_follow_target_unit;encounter.follow_target_unit_index=unit_handles[0];units[0].object.damage_flags=FLAG(_object_dead_bit);}
 if(!strcmp(case_name,"hysteresis")) {units[0].object.position.x=4;firing_positions[1].position.x=10;encounter.follow_target_distance=3;expected=1;}
 if(!strncmp(case_name,"legacy",6)) {
  operation=FALSE;expected=1;
  if(!strcmp(case_name,"legacy-dead")) {
   for(i=0;i<6;i++)units[i].object.damage_flags=FLAG(_object_dead_bit);
   actors[0].meta.squad_index=0;squads[0].current_count=1;squads[1].current_count=0;
  }
 }
 encounter_update_follow(0);
 CHECK(actors[0].meta.squad_index==expected,"native follow chose wrong firing group for %s",case_name);
 if(!strcmp(case_name,"empty")||!strcmp(case_name,"hysteresis"))CHECK(!migrations,"native no-target/tolerance policy changed");
 if(!strcmp(case_name,"outside-squad"))CHECK(actors[1].meta.squad_index==2,"unrelated actor migrated with encounter followers");
 if(!strcmp(case_name,"death-respawn")) {
  units[6].object.position.x=1;units[6].unit.player_index=0x10000;players[0].unit_index=unit_handles[6];
  encounter_update_follow(0);CHECK(actors[0].meta.squad_index==0&&migrations==2,"fresh native respawn did not retarget migration");
 }
 return 0;
}
