/* Native encounter follow/migration regression world; no assets or AI replacement. */
#include "harness.h"
#include <float.h>
#include <math.h>
#include <stdarg.h>
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
struct encounter_datum { short follow_target_type, squad_count, team_index; long follow_target_unit_index, follow_target_ai_index; real follow_target_distance; short remain_active_timer; long last_active_time; boolean active; };
struct actor_datum { struct {short squad_index;long unit_index;boolean active;} meta; struct {struct {real_point3d body_position;} position;} input; };
struct unit_datum { struct { unsigned long damage_flags; real_point3d position; short owner_team_index; } object; struct { long player_index; } unit; };
struct player_datum { long unit_index; boolean quit_out_of_game; };
struct data_iterator { long datum_index, next; };
struct encounter_actor_iterator { long index, next; };
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
static boolean mcc_loaded;
static boolean mcc_cache_tags_loaded(void) { return mcc_loaded; }
static long game_time_get(void) { return 100; }
static void encounter_clear_pursuit(long index) { (void)index; }
static void actor_verify_activation(long index) { CHECK(VALID_INDEX(index,actor_count),"invalid actor"); }
static void actor_set_active(long index,boolean active) { actors[index].meta.active=active; }
#include "under_test.inc"

int main(int argc,char **argv)
{
 char const *case_name=argc>1?argv[1]:"";short i,expected=0;
 scenario.ai_encounters.count=1;scenario.ai_encounters.address=&definition;
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
