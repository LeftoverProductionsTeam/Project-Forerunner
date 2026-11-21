/* Actual pursuit over bounded native-world adapters. This is not played AI. */
#include "harness.h"
#include <limits.h>
#include "types.inc"
#define SHORT_MAX SHRT_MAX
#define MAX(a,b) ((a)>(b)?(a):(b))
#define NUMBEROF(a) (sizeof(a)/sizeof((a)[0]))
#define VALID_INDEX(i,n) ((unsigned)(i)<(unsigned)(n))
#define TICKS_PER_SECOND 30
#define platform_log(...) ((void)0)

struct unit_datum {
 long definition_index;
 struct { real_point3d position; unsigned long damage_flags; short owner_team_index; } object;
 struct { long actor_index,swarm_actor_index,player_index;unsigned long flags;short feign_death_timer; } unit;
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
struct player_datum { long unit_index;boolean quit_out_of_game;struct {short kills[1];} statistics; };
struct data_iterator { short next;long datum_index; };
static struct unit_datum self,units[2];
static struct actor_datum actor;
static struct prop_datum props[2];
static struct player_datum players[2];
static long const handles[2]={0x20010,0x20011},owners[2]={0x10000,0x10001};
static void *player_data;
static long requested=NONE;
static int requests,moves,effects;
static struct unit_datum children[96];
static long child_handles[96];
static int child_count;

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
 for(index=0;index<child_count;index++)if(handle==child_handles[index])return &children[index];
 return NULL;
}
static void *object_try_and_get_and_verify_type(long handle,long mask) {return unit_get(handle);}
static struct unit_datum *object_get(long handle) {return unit_get(handle);}
static struct actor_datum *actor_get(long handle) {CHECK(handle==0x30000,"invalid actor");return &actor;}
/* Native hostility is directional; body ownership and the AI's team differ. */
static boolean game_team_is_enemy(short ours,short theirs) {
 return (ours==2&&theirs!=2&&theirs!=3)||(ours==_game_team_player&&theirs==_game_team_covenant);
}
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

static struct {
 boolean enabled;short phase,unit_count,lives;long wave,wave_limit,score,deadline;
 long units[FIREFIGHT_MAX_TRACKED_ENEMIES],last_damage_player[FIREFIGHT_MAX_TRACKED_ENEMIES];
 boolean killed[FIREFIGHT_MAX_TRACKED_ENEMIES];
} firefight;
struct unit_definition {struct {unsigned long flags;struct {long index;} spawned_actor_variant;short spawn_actor_upper_bound;} unit;};
static struct unit_definition definitions[2];
static int wakes,notices,garbage_releases;
static long garbage_unit;
static boolean garbage_value;
static boolean coop_enemies_firefight_active(void) {return firefight.enabled&&firefight.phase!=0;}
static struct unit_definition *unit_definition_get(long definition) {return &definitions[definition];}
static long game_time_get(void) {return 100;}
static void firefight_notice(char const *text) {notices++;}
static void firefight_wake_unit(long unit,long ai,short team) {
 CHECK(unit_get(unit)&&ai==0x30000&&team==_game_team_covenant,"invalid native child activation");wakes++;
}
static struct player_datum *player_try_and_get(long player) {
 for(int i=0;i<2;i++)if(player==owners[i])return &players[i];return NULL;
}
static void object_set_garbage(long unit,boolean value) {garbage_unit=unit;garbage_value=value;if(value)garbage_releases++;}
#include "children.inc"

static void child_setup(void) {
 firefight.enabled=TRUE;firefight.phase=2;firefight.wave=1;firefight.lives=1;
 definitions[1].unit.flags=FLAG(_unit_is_destroyed_after_dying_bit);
 definitions[1].unit.spawned_actor_variant.index=10;definitions[1].unit.spawn_actor_upper_bound=4;
 child_count=96;
 for(int i=0;i<child_count;i++) {
  child_handles[i]=0x70000+i;children[i].unit.player_index=NONE;
  children[i].unit.actor_index=0x30000;children[i].unit.swarm_actor_index=NONE;
  children[i].object.owner_team_index=_game_team_covenant;
 }
 self.definition_index=1;self.unit.player_index=NONE;
 self.unit.actor_index=self.unit.swarm_actor_index=NONE;self.object.damage_flags=FLAG(_object_dead_bit);
 firefight.units[0]=0x20000;firefight.unit_count=FIREFIGHT_MAX_TRACKED_ENEMIES;
 for(int i=1;i<firefight.unit_count;i++)firefight.units[i]=child_handles[i-1];
 for(int i=0;i<firefight.unit_count;i++)firefight.last_damage_player[i]=NONE;
 firefight.last_damage_player[0]=owners[0];coop_enemies_firefight_enemy_died(0x20000);
 CHECK(firefight.killed[0]&&firefight.score==100&&!garbage_value,"carrier death not retained pending its burst");
}
static void child_kill(long unit,long owner) {
 struct unit_datum *body=unit_get(unit);
 coop_enemies_firefight_enemy_damaged(unit,owner);
 body->object.damage_flags=FLAG(_object_dead_bit);body->unit.actor_index=body->unit.swarm_actor_index=NONE;
 coop_enemies_firefight_enemy_died(unit);
}
static int native_children(char const *name) {
 long child=child_handles[31],retired;char status[128];short slot=5;
 if(!strcmp(name,"append"))firefight.unit_count=2;
 if(!strcmp(name,"reclaim")||!strcmp(name,"pending")||!strcmp(name,"invalid")||!strcmp(name,"repeated")||
    !strcmp(name,"partial")||!strcmp(name,"collected"))
  child_kill(firefight.units[slot],owners[0]);
 retired=firefight.units[slot];
 if(!strcmp(name,"collected"))child_handles[slot-1]+=0x10000; /* same slot, new salted body */
 if(!strcmp(name,"pending")) {
  unit_get(retired)->definition_index=1;
  slot=9;child_kill(firefight.units[slot],owners[0]);retired=firefight.units[slot];
 }
 if(!strcmp(name,"parent"))self.unit.flags|=FLAG(_unit_spawned_actors_bit);
 if(!strcmp(name,"invalid"))unit_get(child)->unit.player_index=owners[0];
 if(!strcmp(name,"full")||!strcmp(name,"parent")||!strcmp(name,"invalid")) {
  long before[FIREFIGHT_MAX_TRACKED_ENEMIES];memcpy(before,firefight.units,sizeof(before));
  CHECK(!coop_enemies_firefight_track_spawned_unit(0x20000,child)&&firefight.phase==4&&notices==1&&!wakes&&
        !memcmp(before,firefight.units,sizeof(before)),"invalid/capacity request displaced a live or spawning owner");
  self.unit.flags|=FLAG(_unit_spawned_actors_bit);
  coop_enemies_firefight_spawn_finished(child);
  CHECK(!garbage_releases,"an untracked child released a native corpse");
  coop_enemies_firefight_spawn_finished(0x20000);
  CHECK(garbage_unit==0x20000&&garbage_value&&garbage_releases==1,
        "failed native spawn left the carrier held after the round aborted");
  return 0;
 }
 CHECK(coop_enemies_firefight_track_spawned_unit(0x20000,child)&&firefight.phase==2,"completed dead slot refused a native child");
 if(!strcmp(name,"append"))slot=2;
 CHECK(firefight.units[slot]==child&&!firefight.killed[slot]&&firefight.last_damage_player[slot]==NONE&&wakes==1,
       "new child inherited a prior death/damager or used the wrong slot");
 CHECK(firefight.unit_count==(!strcmp(name,"append")?3:FIREFIGHT_MAX_TRACKED_ENEMIES)&&
       coop_enemies_firefight_tracks_unit(0x20000)&&coop_enemies_firefight_tracks_unit(child),"tracker or parent identity changed");
 CHECK(coop_enemies_firefight_track_spawned_unit(0x20000,child)&&wakes==1,"duplicate native child repeated activation");
 if(!strcmp(name,"partial")) {
  CHECK(!coop_enemies_firefight_track_spawned_unit(0x20000,NONE)&&firefight.phase==4&&notices==1&&
        firefight.units[0]==0x20000,"partial native creation lost its parent or failed to abort");
  self.unit.flags|=FLAG(_unit_spawned_actors_bit);
  coop_enemies_firefight_spawn_finished(0x20000);
  CHECK(garbage_unit==0x20000&&garbage_value&&garbage_releases==1,
        "partial native spawn left its completed carrier held");
  return 0;
 }
 if(strcmp(name,"append"))CHECK(!coop_enemies_firefight_tracks_unit(retired),"completed corpse remained tracked");
 if(!strcmp(name,"pending"))CHECK(firefight.units[5]!=child&&firefight.killed[5],"a carrier with an unfinished burst was displaced");
 if(!strcmp(name,"repeated")) {
  for(int i=32;i<96;i++) {
   child_kill(child,owners[1]);child=child_handles[i];
   CHECK(coop_enemies_firefight_track_spawned_unit(0x20000,child)&&firefight.units[slot]==child&&
         firefight.unit_count==FIREFIGHT_MAX_TRACKED_ENEMIES,"repeated offspring grew the tracker or lost ownership");
  }
  CHECK(players[1].statistics.kills[0]==64&&firefight.score==6600,"reclaimed children lost/duplicated native kill credit");
 }
 firefight_format_status(status,sizeof(status));
 CHECK(strstr(status,!strcmp(name,"append")?"enemies 2; carriers pending 1":
       !strcmp(name,"pending")?"enemies 30; carriers pending 2":"enemies 31; carriers pending 1"),"live/pending status counts are wrong");
 child_kill(child,owners[1]);coop_enemies_firefight_enemy_died(child);
 CHECK(players[1].statistics.kills[0]==(!strcmp(name,"repeated")?65:1),"a child death was credited twice");
 self.unit.flags|=FLAG(_unit_spawned_actors_bit);coop_enemies_firefight_spawn_finished(0x20000);
 CHECK(garbage_unit==0x20000&&garbage_value&&garbage_releases==1,"finished parent did not rejoin native garbage collection");
 return 0;
}

int main(int argc,char **argv) {
 char const *name=argv[1];short i;long expected=handles[0];
 if(!strncmp(name,"child-",6)){child_setup();return native_children(name+6);}
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
