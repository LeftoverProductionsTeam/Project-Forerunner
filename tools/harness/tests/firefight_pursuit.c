/* Actual pursuit over bounded native-world adapters. This is not played AI. */
#include "harness.h"
#include "types.inc"
#define MAX(a,b) ((a)>(b)?(a):(b))

struct unit_datum {
 struct { real_point3d position; unsigned long damage_flags; short owner_team_index; } object;
 struct { long actor_index,swarm_actor_index,player_index; } unit;
};
struct actor_datum {
 struct { boolean active,swarm;short team_index; } meta;
 struct { boolean defending; } external_orders;
 struct { boolean currently_defending,forced_to_charge; } emotions;
 struct { short artificial_combat_status; } state;
 struct { long target_prop_index;short target_type; } target;
 struct { struct { struct { boolean valid; } path; } path; } control;
};
struct prop_datum { short state;long parent_prop_index;boolean enemy,ally,preferred_target;short ineffability; };
struct player_datum { long unit_index;boolean quit_out_of_game; };
struct data_iterator { short next;long datum_index; };
static struct unit_datum self,units[2];
static struct actor_datum actor;
static struct prop_datum props[2];
static struct player_datum players[2];
static long const handles[2]={0x20010,0x20011},owners[2]={0x10000,0x10001};
static void *player_data;
static long requested=NONE;
static int requests,moves,effects;

static void data_iterator_new(struct data_iterator *it,void *data) {it->next=0;}
static void *data_iterator_next(struct data_iterator *it) {
 short index=it->next++;
 if(index>=2)return NULL;
 it->datum_index=owners[index];return &players[index];
}
static struct unit_datum *unit_get(long handle) {
 short index;
 if(handle==0x20000)return &self;
 for(index=0;index<2;index++)if(handle==handles[index])return &units[index];
 return NULL;
}
static void *object_try_and_get_and_verify_type(long handle,long mask) {return unit_get(handle);}
static struct unit_datum *object_get(long handle) {return unit_get(handle);}
static struct actor_datum *actor_get(long handle) {CHECK(handle==0x30000,"invalid actor");return &actor;}
/* Native hostility is directional; body ownership and the AI's team differ. */
static boolean game_team_is_enemy(short ours,short theirs) {return ours==2&&theirs!=2&&theirs!=3;}
static real distance_squared3d(real_point3d const *a,real_point3d const *b) {
 real x=a->x-b->x,y=a->y-b->y,z=a->z-b->z;return x*x+y*y+z*z;
}
static long prop_get_base_by_unit_index(long ai,long target,boolean create,boolean update) {
 requests++;requested=target;return target==handles[0]?10:11;
}
static struct prop_datum *prop_try_and_get(long handle) {return handle==10?&props[0]:handle==11?&props[1]:NULL;}
static void actor_handle_unit_effect(long ai,long prop,short effect) {CHECK(effect==3,"wrong native sight effect");effects++;}
static void actor_move_to_prop(long ai,long prop,real radius) {moves++;}
static void actor_move_to_point(long ai,real_point3d const *point,long a,long b) {moves++;}
#include "under_test.inc"

int main(int argc,char **argv) {
 char const *name=argv[1];short i;long expected=handles[0];
 self.unit.actor_index=0x30000;self.unit.swarm_actor_index=NONE;
 self.object.owner_team_index=3;
 actor.meta.active=TRUE;actor.meta.team_index=2;actor.target.target_prop_index=NONE;
 actor.external_orders.defending=actor.emotions.currently_defending=TRUE;
 for(i=0;i<2;i++) {
  players[i].unit_index=handles[i];units[i].unit.player_index=owners[i];
  units[i].object.position.x=i?8.f:1.f;units[i].object.owner_team_index=i;
  props[i].state=_prop_state_acknowledged;props[i].parent_prop_index=NONE;
 }
 if(!strcmp(name,"ally-nearest")) {units[0].object.owner_team_index=2;expected=handles[1];}
 else if(!strcmp(name,"allied")) {units[0].object.owner_team_index=units[1].object.owner_team_index=2;expected=NONE;}
 else if(!strcmp(name,"quit")) {players[0].quit_out_of_game=TRUE;expected=handles[1];}
 else if(!strcmp(name,"stale")) {units[0].unit.player_index=owners[0]+0x10000;expected=handles[1];}
 else if(!strcmp(name,"dead")) {units[0].object.damage_flags=FLAG(_object_dead_bit);expected=handles[1];}
 else if(!strcmp(name,"missing")) {players[0].unit_index=NONE;expected=handles[1];}
 else if(!strcmp(name,"inactive")) {actor.meta.active=FALSE;expected=NONE;}
 else if(!strcmp(name,"swarm")) {
  actor.meta.swarm=TRUE;self.unit.actor_index=NONE;self.unit.swarm_actor_index=0x30000;
  units[0].object.owner_team_index=2;expected=handles[1];
 } else if(!strcmp(name,"switch")) {
  firefight_pursue(0x20000);CHECK(requested==handles[0]&&props[0].enemy,"initial hostile not pursued");
  /* Native ai_update_team_status owns reclassification; pursuit must keep it. */
  units[0].object.owner_team_index=2;props[0].enemy=FALSE;props[0].ally=TRUE;
  requested=NONE;requests=moves=effects=0;expected=handles[1];
 } else if(strcmp(name,"hostile"))return 2;
 firefight_pursue(0x20000);
 CHECK(requested==expected,"pursuit chose an allied, departed or unowned body");
 if(expected==NONE) {
  CHECK(!requests&&!moves&&!effects&&!actor.emotions.forced_to_charge,"no-target path forced combat");
 } else {
  struct prop_datum *chosen=&props[expected==handles[0]?0:1];
  CHECK(requests==1&&chosen->enemy&&!chosen->ally&&chosen->preferred_target&&
   actor.target.target_type==_actor_target_acknowledged_enemy,"hostile target lost native pursuit");
  if(!strcmp(name,"switch"))CHECK(!props[0].enemy&&props[0].ally,"pursuit reversed native alliance change");
 }
 return 0;
}
