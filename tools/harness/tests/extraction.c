/* Real Extraction + shared gear + native inventory operations in one bounded
 * world. No native rendering, packets, identity proof or AI is simulated here. */
#define GEAR_FIXTURE_ONLY
#include "gear.c"
#include <wchar.h>
#define HALO_GAME_BROWSER 1
#define HALO_PORT_MAXIMUM_NETWORK_PLAYERS 16
#define HALO_PORT_MAXIMUM_NETWORK_MACHINES 16
#define MAXIMUM_NUMBER_OF_LOCAL_PLAYERS 4
#define MAXIMUM_FILENAME_LENGTH 255
#define MAXIMUM_WEAPONS_PER_UNIT 4
#define TICKS_PER_SECOND 30
#define WEAPON_DEFINITION_TAG 1
#define NETWORK_RULES_TIME_WINDOW_TICKS 450
#define csmemset memset
#define TAG_BLOCK_GET_ELEMENT(block,index,type) (&((type *)(block)->address)[index])
enum { _object_mask_biped=1, _object_mask_control=2, _object_mask_weapon=4,
 _object_dead_bit=0, _unit_grenade_throw_idle=0, _unit_add_weapon_starting=0,
 _scenario_type_solo=0, _object_die_act_of_god_bit=1,
 _unit_control_crouch_modifier_bit=0, _unit_control_action_bit=6,
 _unit_control_weapon_reload_bit=10, _unit_control_weapon_primary_trigger_bit=11,
 _unit_control_weapon_secondary_trigger_bit=12, _unit_control_throw_grenade_bit=13,
 _unit_control_use_equipment_bit=7, _unit_control_swap_weapons_bit=14, _unit_control_user_animation1_bit=2,
 _collision_test_structure_bit=0, _collision_test_objects_bit=1,
 _collision_test_front_facing_surfaces_bit=2, _collision_test_back_facing_surfaces_bit=3,
 _delta_capability_coop=6, _delta_capability_object_sync=17 };
struct tag_block { long count; void *address; };
struct object_fields { long parent_object_index; short name_index; long damage_flags; real_point3d position;
 real_point3d bounding_sphere_center; real body_vitality, shield_vitality; short owner_team_index; };
struct object_datum { long definition_index; struct object_fields object; };
struct unit_datum { long definition_index; struct object_fields object;
 struct { long player_index, weapon_object_indices[4]; short current_weapon_index, desired_weapon_index;
  signed char grenade_counts[2]; short grenade_throw_state; } unit; };
struct weapon_datum { long definition_index; struct object_fields object;
 struct { struct { short rounds_loaded, rounds_total; } magazines[2]; float age; } weapon; };
struct control_datum { long definition_index; struct object_fields object; };
struct player_datum { long unit_index; short local_player_index; boolean quit_out_of_game;
 struct { short controller_index; } network_player_data; };
struct weapon_definition { struct { struct tag_block magazines; } weapon; };
struct weapon_magazine_definition { short rounds_loaded_maximum, rounds_total_maximum; };
struct game_globals_grenade { short maximum_count; };
struct game_globals { struct tag_block grenades; };
struct control_definition { struct { short triggers_when; } control; };
struct scenario_object_name { char name[32]; };
struct scenario_trigger_volume { char name[32]; };
struct player_starting_location { real_point3d position; };
struct scenario { short type; struct tag_block object_names, trigger_volumes, players; };
struct object_placement_data { long definition_index; real_point3d position; };
struct delta_peer_identity { byte account[16]; int bound; };
struct data_iterator { short next; long datum_index; };
struct player_action { unsigned long control_flags; real primary_trigger; struct {real i,j;} throttle;
 short desired_zoom_level, desired_weapon_index; };
struct collision_result { long object_index; };

static struct player_datum players[2];
static long player_handles[2] = { 0x10000, 0x10001 };
static long unit_handles[2] = { 0x1000a, 0x1000b };
static struct unit_datum units[2];
static struct { long handle; struct weapon_datum datum; boolean flag; } weapons[96];
static short weapon_count, fail_allocate, fail_attach;
static struct control_datum controls[3];
static long control_handles[3] = { 0x10003, 0x10004, 0x10005 };
static struct control_definition control_tag;
static struct weapon_magazine_definition magazine = { 20, 200 };
static struct weapon_definition weapon_tag = { { { 1, &magazine } } };
static struct game_globals_grenade grenades[2] = { {4}, {4} };
static struct game_globals globals = { { 2, grenades } };
static struct scenario_object_name names[3] = { {"extraction_deploy"}, {"extraction_activate"}, {"extraction_loadout"} };
static struct scenario_trigger_volume volumes[2] = { {"extraction_zone"}, {"extraction_return"} };
static struct player_starting_location starts[2];
static struct scenario scenario = { _scenario_type_solo, {3,names}, {2,volumes}, {2,starts} };
static struct scenario *global_scenario = &scenario;
static void *player_data;
static long tick;
static boolean enabled=TRUE, director, profile=TRUE, verified=TRUE;
static boolean medical_setting, medical_support=TRUE, medical_object_sync=TRUE, wall;
static unsigned int epoch=1;
static int native_kills;
void platform_log(char const *format, ...) { (void)format; }
static byte remote_account[16] = {1};
static int local_notices, remote_notices, respawns;

static boolean config_boolean(char const *name)
{ return !strcmp(name,"game.extraction") ? enabled : !strcmp(name,"game.revive") ? medical_setting : director; }
static long config_integer(char const *name) { return !strcmp(name,"game.revive_seconds") ? 5 : 30; }
static boolean network_coop_active(void) { return TRUE; }
static struct { boolean enabled, gear, victory; short phase; } firefight;
#include "survival.inc"
static long game_time_get(void) { return tick; }
static struct player_datum *player_try_and_get(long handle)
{ short i; for(i=0;i<2;i++) if(player_handles[i]==handle) return &players[i]; return NULL; }
static struct player_datum *player_get(long h) { struct player_datum *p=player_try_and_get(h); CHECK(p,"invalid player"); return p; }
static void *object_try_and_get_and_verify_type(long h,long mask)
{
 short i;
 if(h==NONE) return NULL;
 for(i=0;i<2;i++) if(h==unit_handles[i] && (mask&_object_mask_biped)) return &units[i];
 for(i=0;i<3;i++) if(h==control_handles[i] && (mask&_object_mask_control)) return &controls[i];
 for(i=0;i<weapon_count;i++) if(h==weapons[i].handle && (mask&_object_mask_weapon)) return &weapons[i].datum;
 return NULL;
}
static struct unit_datum *unit_get(long h) { void *p=object_try_and_get_and_verify_type(h,_object_mask_biped); CHECK(p,"invalid unit"); return p; }
static struct weapon_datum *weapon_try_and_get(long h) { return object_try_and_get_and_verify_type(h,_object_mask_weapon); }
static struct weapon_datum *weapon_get(long h) { struct weapon_datum *p=weapon_try_and_get(h); CHECK(p,"invalid weapon"); return p; }
static boolean cinematic_in_progress(void) { return FALSE; }
static boolean unit_controllable(long h) { return !(unit_get(h)->object.damage_flags & 1); }
static boolean unit_is_busy(long h) { return FALSE; }
static void object_deplete_shield(long h) { unit_get(h)->object.shield_vitality=0.f; }
static void unit_kill(long h) { native_kills++; unit_get(h)->object.damage_flags |= FLAG(_object_die_act_of_god_bit); }
static unsigned int network_distributed_epoch(void) { return epoch; }
static boolean delta_peer_game_room_has(int capability) { return capability==_delta_capability_object_sync ? medical_object_sync : medical_support; }
static boolean game_team_is_enemy(short a,short b) { return a!=b; }
static boolean collision_test_vector(unsigned long flags,real_point3d const *from,real_vector3d const *way,long ignore,struct collision_result *result)
{ result->object_index=NONE; return wall; }
static boolean weapon_is_flag(long h)
{ short i; for(i=0;i<weapon_count;i++) if(weapons[i].handle==h) return weapons[i].flag; return FALSE; }
static char const *tag_get_name(long tag) { return tag==1 ? "weapons\\rifle\\rifle" : "weapons\\pistol\\pistol"; }
static long tag_loaded(long group,char const *path)
{ return !strcmp(path,tag_get_name(1)) ? 1 : !strcmp(path,tag_get_name(2)) ? 2 : NONE; }
static struct weapon_definition *weapon_definition_get(long tag) { return &weapon_tag; }
static struct game_globals *scenario_get_game_globals(void) { return &globals; }
static void object_placement_data_new(struct object_placement_data *p,long tag,long owner) { memset(p,0,sizeof(*p)); p->definition_index=tag; }
static long object_new(struct object_placement_data const *p)
{
 short i;
 if(fail_allocate && !--fail_allocate) return NONE;
 i=weapon_count++; CHECK(i<96,"world full"); weapons[i].handle=0x10020+i;
 weapons[i].datum.definition_index=p->definition_index; weapons[i].datum.object.parent_object_index=NONE;
 weapons[i].datum.object.position=p->position; return weapons[i].handle;
}
static void object_delete(long h)
{ short i; for(i=0;i<weapon_count;i++) if(weapons[i].handle==h) { weapons[i].handle=NONE; return; } }
static boolean unit_drop_weapon(long u,long w)
{
 short i; struct unit_datum *unit=unit_get(u); struct weapon_datum *weapon=weapon_try_and_get(w);
 if(!weapon || weapon->object.parent_object_index!=u) return FALSE;
 for(i=0;i<4;i++) if(unit->unit.weapon_object_indices[i]==w) { unit->unit.weapon_object_indices[i]=NONE; weapon->object.parent_object_index=NONE; return TRUE; }
 return FALSE;
}
static boolean unit_add_weapon_to_inventory(long u,long w,long mode)
{
 short i; struct unit_datum *unit=unit_get(u); struct weapon_datum *weapon=weapon_get(w);
 if(fail_attach && !--fail_attach) return FALSE;
 if(weapon->object.parent_object_index!=NONE) return FALSE;
 for(i=0;i<4;i++) if(unit->unit.weapon_object_indices[i]==NONE) { unit->unit.weapon_object_indices[i]=w; weapon->object.parent_object_index=u; return TRUE; }
 return FALSE;
}
static short local_player_get_next(short index) { return index==NONE ? 0 : NONE; }
static void hud_print_message(short local,wchar_t const *text) { CHECK(local==0,"wrong local notice"); local_notices++; }
short distributed_client_machines(long *machines,short max) { machines[0]=1; return 1; }
long distributed_player_machine(short slot) { CHECK(slot>=0 && slot<2,"unsalted absolute player lookup"); return slot==1 ? 1 : 0; }
void distributed_send_notice_to_machine(long machine,char const *text) { CHECK(machine==1,"wrong remote notice"); remote_notices++; }
static long player_ui_get_active_player_profile_index(short local) { return profile ? 0 : NONE; }
static boolean player_ui_get_path_to_local_player_profile_directory(short local,char *path) { strcpy(path,"owned/profile"); return TRUE; }
static boolean delta_peer_game_player_identity(int machine,int controller,struct delta_peer_identity *id)
{ CHECK(machine==1 && controller==2,"wrong authenticated controller"); memcpy(id->account,remote_account,16); id->bound=verified; return TRUE; }
static long object_index_from_name_index(short index) { return index>=0 && index<3 ? control_handles[index] : NONE; }
static struct control_datum *control_try_and_get(long h) { return object_try_and_get_and_verify_type(h,_object_mask_control); }
static struct control_definition *control_definition_get(long h) { return &control_tag; }
static boolean scenario_trigger_volume_test_point(short zone,real_point3d const *p) { return zone==0 ? p->x>10.f : p->x<5.f; }
static boolean scenario_trigger_volume_test_object(short zone,long h) { return scenario_trigger_volume_test_point(zone,&unit_get(h)->object.position); }
static boolean players_are_all_dead(void) { return (units[0].object.damage_flags&1) && (units[1].object.damage_flags&1); }
static void players_respawn_at_start(void) { respawns++; }
static void data_iterator_new(struct data_iterator *i,void *data) { i->next=0; }
static void *data_iterator_next(struct data_iterator *i) { if(i->next>=2) return NULL; i->datum_index=player_handles[i->next]; return &players[i->next++]; }

#include "equipment.inc"
#include "extraction.inc"

static void advance(short count) { while(count--) { tick++; extraction_update(); } }
static void touch(short control,short player) { CHECK(extraction_control_touched(control_handles[control],unit_handles[player]),"control not consumed"); }
static struct extraction_run *run(short player) { return &extraction_runs[player]; }
static long pickup(short player,long tag)
{
 struct object_placement_data p; struct weapon_datum *weapon; long h;
 object_placement_data_new(&p,tag,NONE); h=object_new(&p); CHECK(h!=NONE,"pickup allocation failed");
 weapon=weapon_get(h); weapon->weapon.age=.25f; weapon->weapon.magazines[0].rounds_loaded=7;
 weapon->weapon.magazines[0].rounds_total=63;
 CHECK(unit_add_weapon_to_inventory(unit_handles[player],h,0),"pickup failed"); return h;
}
static void escape(short player)
{
 units[player].object.position.x=20.f; touch(1,player); advance(300);
 CHECK(run(player)->phase==_extraction_ready,"signal not ready");
 advance(900); CHECK(run(player)->phase==_extraction_ready,"mandatory raid timer expired");
 units[player].object.position.x=0.f; advance(1);
 CHECK(run(player)->phase==_extraction_returned,"escape did not settle");
}
static void initialize(void)
{
 short i,j; connection=_game_connection_network_server;
 for(i=0;i<2;i++) {
  players[i].unit_index=unit_handles[i]; players[i].local_player_index=i ? NONE : 0;
  players[i].network_player_data.controller_index=i ? 2 : 0; units[i].unit.player_index=player_handles[i];
  units[i].object.body_vitality=units[i].object.shield_vitality=1.f;
  units[i].object.parent_object_index=NONE;
  units[i].unit.current_weapon_index=0; units[i].unit.grenade_counts[0]=1;
  for(j=0;j<4;j++) units[i].unit.weapon_object_indices[j]=NONE;
 }
 for(i=0;i<3;i++) controls[i].object.name_index=i;
 extraction_reset(); advance(1); CHECK(extraction_owns_map(),"valid authored hub refused");
}
static void treat(short count,boolean hit_last)
{
 short i; struct player_action action;
 for(i=0;i<count;i++) {
  tick++; memset(&action,0,sizeof(action)); action.control_flags=FLAG(_unit_control_swap_weapons_bit);
  extraction_medical_control(player_handles[0],&action);
  if(hit_last && i==count-1) extraction_medical_damaged(unit_handles[0]);
  extraction_update();
 }
}
int main(int argc,char **argv)
{
 char const *case_name=argv[1]; long a; short before;
 initialize();
 if(!strncmp(case_name,"medical-",8)) {
  enabled=FALSE; medical_setting=TRUE; extraction_reset(); advance(1);
  if(!strcmp(case_name,"medical-authority")) {
   units[1].object.body_vitality=-.1f; medical_support=FALSE;
   CHECK(!extraction_medical_lethal(unit_handles[1]),"unsupported peer admitted medical gameplay");
   medical_support=TRUE; medical_object_sync=FALSE;
   CHECK(!extraction_medical_lethal(unit_handles[1]),"peer without object sync admitted medical gameplay");
   medical_object_sync=TRUE; client=TRUE;
   CHECK(!extraction_medical_lethal(unit_handles[1]),"client granted itself incapacitation");
  } else if(!strcmp(case_name,"medical-replica")) {
   struct player_action action;
   client=TRUE; units[1].object.body_vitality=0.f;
   extraction_medical_network_state(unit_handles[1],TRUE,epoch,(unsigned short)tick);
   CHECK(extraction_medical_incapacitated(unit_handles[1]),"host medical state not applied");
   memset(&action,0,sizeof(action)); action.primary_trigger=1.f; action.throttle.i=1.f;
   extraction_medical_control(player_handles[1],&action);
   CHECK(!action.primary_trigger && !action.throttle.i && action.control_flags==FLAG(_unit_control_crouch_modifier_bit),"downed client moved/fired");
   extraction_medical_network_state(unit_handles[1],FALSE,epoch,(unsigned short)(tick-1));
   extraction_medical_network_state(unit_handles[1],FALSE,epoch+1,(unsigned short)(tick+1));
   CHECK(extraction_medical_incapacitated(unit_handles[1]),"stale/wrong-epoch state overrode host");
   tick++; extraction_medical_network_state(unit_handles[1],FALSE,epoch,(unsigned short)tick);
   CHECK(!extraction_medical_incapacitated(unit_handles[1]),"new stable state retained input lock");
   extraction_medical_network_state(unit_handles[1]+0x10000,TRUE,epoch,(unsigned short)(tick+1));
   CHECK(!extraction_medical_incapacitated(unit_handles[1]),"recycled object salt controlled player");
  } else {
   units[1].object.body_vitality=-.1f;
   CHECK(extraction_medical_lethal(unit_handles[1]) && !units[1].object.damage_flags && !units[1].object.body_vitality,
    "critical live Spartan died or did not stabilize");
   CHECK(!native_kills,"incapacitation charged a native death");
   advance(60);
   if(!strcmp(case_name,"medical-revive")) {
    struct player_action press;
    tick++; memset(&press,0,sizeof(press)); press.control_flags=FLAG(_unit_control_action_bit);
    extraction_medical_control(player_handles[0],&press); extraction_update();
    CHECK(!medical_players[1].progress && !press.control_flags,"a one-tick Action press began held treatment");
    treat(150,FALSE);
    CHECK(!extraction_medical_incapacitated(unit_handles[1]) && units[1].object.body_vitality==.25f && !native_kills,"held biofoam did not revive living body");
    units[1].object.body_vitality=-.1f;
    CHECK(!extraction_medical_lethal(unit_handles[1]),"same body received unlimited free downs");
   } else if(!strcmp(case_name,"medical-interrupt")) {
    treat(150,TRUE);
    CHECK(extraction_medical_incapacitated(unit_handles[1]) && !medical_players[1].progress,"last-tick combat failed to interrupt");
    advance(60); treat(60,FALSE); wall=TRUE; treat(1,FALSE);
    CHECK(!medical_players[1].progress,"biofoam passed through a wall");
    wall=FALSE; units[1].object.bounding_sphere_center.x=3.f; treat(1,FALSE);
    CHECK(!medical_players[1].progress,"out-of-range ally revived");
    units[1].object.bounding_sphere_center.x=0.f; units[1].object.owner_team_index=1; treat(1,FALSE);
    CHECK(!medical_players[1].progress,"hostile body received ally treatment");
    units[1].object.owner_team_index=0; treat(150,FALSE);
    CHECK(units[1].object.body_vitality==.25f,"valid treatment did not recover after interruption");
   } else if(!strcmp(case_name,"medical-bleed")) {
    advance(841); CHECK(native_kills && (units[1].object.damage_flags&FLAG(_object_die_act_of_god_bit)),"bleedout did not request actual native death");
    treat(150,FALSE); CHECK(!units[1].object.body_vitality,"pending native death was medically resurrected");
    units[1].object.damage_flags |= FLAG(_object_dead_bit); treat(150,FALSE);
    CHECK(!units[1].object.body_vitality,"dead Spartan was medically revived");
   } else if(!strcmp(case_name,"medical-restore")) {
    extraction_restored(); advance(1);
    CHECK(native_kills && !extraction_medical_host(),"saved critical body bypassed operation failure/quarantine");
   } else if(!strcmp(case_name,"medical-gear")) {
    /* Downed remains alive and owns the same native items until actual death. */
    units[1].object.body_vitality=1.f; medical_setting=FALSE; extraction_reset(); enabled=TRUE; advance(1); touch(0,1); a=pickup(1,2);
    medical_enabled=TRUE; units[1].object.body_vitality=-.1f; CHECK(extraction_medical_lethal(unit_handles[1]),"geared player did not incapacitate");
    advance(1); CHECK(run(1)->phase==_extraction_deployed && weapon_get(a)->object.parent_object_index==unit_handles[1],"downing lost owned gear");
    units[1].object.damage_flags |= FLAG(_object_dead_bit); advance(1);
    CHECK(run(1)->phase==_extraction_lost && !run(1)->gear.record.reserved_count,"actual death retained reservation");
   } else return 2;
  }
 } else if(!strncmp(case_name,"survival-",9)) {
  extraction_reset(); enabled=FALSE; director=TRUE;
  firefight.enabled=firefight.gear=TRUE; firefight.phase=1; advance(1);
  CHECK(extraction.survival && !extraction_owns_map(),"gear policy stole Firefight map/lives ownership");
  CHECK(run(0)->phase==_extraction_deployed && run(1)->phase==_extraction_deployed,"Survival gear not reserved at live spawn");
  a=pickup(0,1); pickup(1,2);
  if(!strcmp(case_name,"survival-win")) {
   /* Only an actual finite success admits a reward. No activation timer,
    * Extraction marker or world rewind is required for this milestone. */
   firefight.phase=4; firefight.victory=TRUE;
   CHECK(extraction_survival_settlement_pending(),"lobby passed unsettled winners"); advance(1);
   CHECK(!extraction_survival_settlement_pending() && run(0)->phase==_extraction_returned &&
    run(0)->gear.record.count==1 && run(1)->gear.record.count==1 && !weapon_try_and_get(a),"finite victory did not bank owned loot");
   advance(60); CHECK(run(0)->gear.record.count==1,"victory retry duplicated reward");
   extraction_reset(); firefight.victory=FALSE; firefight.phase=1; advance(1);
   CHECK(run(0)->gear.record.reserved_count==1 && weapon_get(units[0].unit.weapon_object_indices[0])->weapon.magazines[0].rounds_loaded==7,
    "next Survival session did not restore recovered gear");
   firefight.phase=4; firefight.victory=TRUE; advance(1);
   extraction_reset(); firefight.enabled=FALSE; enabled=TRUE; director=FALSE; advance(1); touch(0,0);
   CHECK(run(0)->gear.record.reserved_count==1 && !run(0)->gear.record.count &&
    weapon_get(units[0].unit.weapon_object_indices[0])->weapon.magazines[0].rounds_loaded==7,
    "same-cache Extraction did not reuse Survival's owned stash");
  } else if(!strcmp(case_name,"survival-loss")) {
   firefight.phase=4; CHECK(coop_enemies_firefight_outcome()==-1,"abort phase treated as victory"); advance(1);
   CHECK(!extraction_survival_settlement_pending() && run(0)->phase==_extraction_lost &&
    !run(0)->gear.record.count && !run(0)->gear.record.reserved_count && weapon_try_and_get(a),"failed run awarded or erased native loot");
  } else if(!strcmp(case_name,"survival-retry")) {
   firefight.phase=4; firefight.victory=TRUE; fail_write=TRUE; advance(1);
   CHECK(extraction_survival_settlement_pending() && run(0)->phase==_extraction_ready && weapon_try_and_get(a),"failed escrow discarded winning loot");
   fail_write=FALSE; publish_failure=TRUE; advance(30);
   CHECK(extraction_survival_settlement_pending() && run(0)->phase==_extraction_settling && !weapon_try_and_get(a),"uncertain saved success was not consumed once");
   publish_failure=FALSE; advance(30);
   CHECK(!extraction_survival_settlement_pending() && run(0)->gear.record.count==1,"winner retry never released lobby");
  } else if(!strcmp(case_name,"survival-restore")) {
   extraction_restored(); advance(1); firefight.phase=4; firefight.victory=TRUE; advance(1);
   CHECK(extraction.suspended && !run(0)->gear.opened && !extraction_survival_settlement_pending(),"restored world minted Survival loot");
  } else if(!strcmp(case_name,"survival-respawn")) {
   units[0].object.damage_flags=1; advance(1); CHECK(run(0)->phase==_extraction_lost,"dead survivor retained reservation");
   units[0].object.damage_flags=0; unit_handles[0]+=0x10000; players[0].unit_index=unit_handles[0];
   /* A native respawn is a new biped, not the dead body's stale inventory. */
   { short slot; for(slot=0;slot<4;slot++) units[0].unit.weapon_object_indices[slot]=NONE; }
   advance(1);
   CHECK(run(0)->phase==_extraction_deployed && run(0)->unit==unit_handles[0],"paid native respawn did not enter a new at-risk run");
   units[0].object.damage_flags=1; firefight.phase=4; firefight.victory=TRUE; advance(1);
   CHECK(run(0)->phase==_extraction_lost && !run(0)->gear.record.count,"dead player received finite victory reward");
  } else return 2;
 } else if(!strcmp(case_name,"binding")) {
  extraction_reset(); starts[1].position.x=20; advance(1); CHECK(!extraction_owns_map(),"field spawn admitted");
  starts[1].position.x=0; extraction_reset(); director=TRUE; advance(1); CHECK(!extraction_owns_map(),"duplicate director admitted");
  director=FALSE; extraction_reset(); names[2].name[0]=0; advance(1);
  CHECK(extraction_owns_map(),"optional terminal required"); controls[2].object.name_index=NONE;
  CHECK(!extraction_control_touched(control_handles[2],unit_handles[0]),"unnamed ordinary control intercepted");
 } else if(!strcmp(case_name,"authority")) {
  client=TRUE; connection=_game_connection_network_client; advance(1); CHECK(!extraction_owns_map() && !file_count,"client storage access");
  client=FALSE; connection=_game_connection_network_server; profile=FALSE; verified=FALSE;
  touch(0,0); touch(0,1); CHECK(!file_count,"unidentified player opened storage");
  verified=TRUE; touch(0,1); CHECK(run(1)->phase==_extraction_deployed,"verified owner refused");
  remote_account[0]++; advance(1); CHECK(run(1)->phase==_extraction_lost,"changed identity retained run");
 } else if(!strcmp(case_name,"loop")) {
  touch(0,0); touch(0,1); CHECK(run(0)->phase==_extraction_deployed && run(1)->phase==_extraction_deployed,"players not deployed");
  a=pickup(0,1); pickup(1,2); units[0].unit.grenade_counts[1]=2; escape(0); escape(1);
  CHECK(!weapon_try_and_get(a) && run(0)->gear.record.count==1 && run(1)->gear.record.count==1,"escape not consumed/owner isolated");
  touch(2,0); touch(0,0); CHECK(run(0)->gear.record.count==0 && run(0)->gear.record.reserved_count==1,"redeploy did not reserve");
  a=units[0].unit.weapon_object_indices[0]; CHECK(weapon_get(a)->weapon.magazines[0].rounds_loaded==7 &&
   weapon_get(a)->weapon.magazines[0].rounds_total==63 && weapon_get(a)->weapon.age==.25f && units[0].unit.grenade_counts[1]==2,"redeploy changed quantities");
  units[0].object.damage_flags=1; advance(1);
  CHECK(run(0)->phase==_extraction_lost && !run(0)->gear.record.reserved_count && !run(0)->gear.record.count && run(1)->gear.record.count==1,"death loss affected another stash");
  units[1].object.damage_flags=1;
  CHECK(extraction_respawn_after_loss() && respawns==1,"team wipe did not use hub respawn");
  units[1].object.damage_flags=0;
  extraction_reset(); advance(1); touch(2,1); CHECK(run(1)->gear.record.count==1,"successful remote stash not persistent");
  CHECK(local_notices && remote_notices,"missing player reports");
 } else if(!strcmp(case_name,"transfer")) {
  byte item_id[GEAR_ID_BYTES];
  touch(0,0); touch(0,1); pickup(0,1); escape(0);
  memcpy(item_id,run(0)->gear.record.items[0].id,sizeof(item_id));
  touch(0,0); a=units[0].unit.weapon_object_indices[0];
  /* A stale former owner's slot is never a salvage claim. */
  CHECK(unit_drop_weapon(unit_handles[0],a) && unit_add_weapon_to_inventory(unit_handles[1],a,0),"transfer failed");
  units[0].unit.weapon_object_indices[0]=a;
  { struct game_modes_inventory captured; CHECK(!game_modes_inventory_capture(player_handles[0],&captured),"stale owner recovered shared loot"); }
  units[0].unit.weapon_object_indices[0]=NONE; units[0].object.damage_flags=1; advance(1); escape(1);
  CHECK(run(1)->gear.record.count==1 && !run(0)->gear.record.count && !weapon_try_and_get(a) &&
   !memcmp(item_id,run(1)->gear.record.items[0].id,sizeof(item_id)),"shared loot banked twice or lost transfer identity");
 } else if(!strcmp(case_name,"disconnect")) {
  touch(0,1); pickup(1,2); escape(1); touch(0,1);
  CHECK(run(1)->gear.record.reserved_count==1,"remote gear not at risk");
  players[1].quit_out_of_game=TRUE; advance(1);
  CHECK(run(1)->player==NONE && !run(1)->gear.opened,"departed player retained stash lock");
  player_handles[1]=0x20001; units[1].unit.player_index=player_handles[1];
  players[1].quit_out_of_game=FALSE; advance(1); touch(2,1);
  CHECK(run(1)->player==0x20001 && run(1)->phase==_extraction_hub &&
   !run(1)->gear.record.count && !run(1)->gear.record.reserved_count,"recycled player inherited abandoned rewards");
 } else if(!strcmp(case_name,"grant-failure")) {
  touch(0,0); pickup(0,1); pickup(0,2); escape(0); a=pickup(0,1); before=weapon_count;
  fail_allocate=2; touch(0,0);
  CHECK(run(0)->gear.record.count==2 && !run(0)->gear.record.reserved_count && units[0].unit.weapon_object_indices[0]==a,"allocation failure lost stash or carried gear");
  CHECK(!weapon_try_and_get(0x10020+before),"partial allocation survived refund");
  before=weapon_count; fail_attach=2; touch(0,0);
  CHECK(run(0)->gear.record.count==2 && !run(0)->gear.record.reserved_count && weapon_get(a)->object.parent_object_index==NONE,"failed grant destroyed carried loot");
  CHECK(!weapon_try_and_get(0x10020+before) && !weapon_try_and_get(0x10021+before),"partial attachment survived refund");
  touch(0,0); CHECK(run(0)->phase==_extraction_deployed && run(0)->gear.record.reserved_count==2,"grant retry failed");
 } else if(!strcmp(case_name,"restore")) {
  touch(0,0); a=pickup(0,1); escape(0); extraction_restored(); advance(1); touch(0,0);
  CHECK(extraction.suspended && !run(0)->gear.opened,"saved world reopened bank");
  extraction_reset(); advance(1); touch(2,0); CHECK(run(0)->gear.record.count==1,"fresh map lost escaped gear");
 } else if(!strcmp(case_name,"pending")) {
  touch(0,0); a=pickup(0,1); units[0].object.position.x=20; touch(1,0); advance(300);
  fail_write=TRUE; units[0].object.position.x=0; advance(1);
  CHECK(run(0)->phase==_extraction_ready && weapon_try_and_get(a),"failed escrow consumed gear");
  fail_write=FALSE; publish_failure=TRUE; advance(30);
  CHECK(run(0)->phase==_extraction_settling && !weapon_try_and_get(a),"published uncertain escrow not consumed");
  publish_failure=FALSE; advance(30); CHECK(run(0)->phase==_extraction_returned && run(0)->gear.record.count==1,"durable retry failed");
  advance(60); CHECK(run(0)->gear.record.count==1,"settlement replay duplicated gear");
 } else return 2;
 extraction_reset(); return 0;
}
