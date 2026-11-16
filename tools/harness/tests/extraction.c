/* Real Extraction + shared gear + native inventory operations in one bounded
 * world. No native rendering, packets, identity proof or AI is simulated here. */
#define GEAR_FIXTURE_ONLY
#include "gear.c"
#include <wchar.h>
#include <math.h>
#ifndef _WIN32
#include <strings.h>
#define _stricmp strcasecmp
#endif
#define valid_real(x) isfinite(x)
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
enum { _object_mask_biped=1, _object_mask_control=2, _object_mask_weapon=4, _object_mask_vehicle=8, _object_type_vehicle=1,
 _object_dead_bit=0, _unit_grenade_throw_idle=0, _unit_add_weapon_starting=0,
 _scenario_type_solo=0, _object_die_act_of_god_bit=1,
 _object_die_act_of_god_silent_bit=2, _object_die_act_of_god_no_statistics_bit=3,
 _unit_control_crouch_modifier_bit=0, _unit_control_action_bit=6,
 _unit_control_weapon_reload_bit=10, _unit_control_weapon_primary_trigger_bit=11,
 _unit_control_weapon_secondary_trigger_bit=12, _unit_control_throw_grenade_bit=13,
 _unit_control_use_equipment_bit=7, _unit_control_swap_weapons_bit=14, _unit_control_user_animation1_bit=2,
 _collision_test_structure_bit=0, _collision_test_objects_bit=1,
 _collision_test_front_facing_surfaces_bit=2, _collision_test_back_facing_surfaces_bit=3,
 _delta_capability_coop=6, _delta_capability_object_sync=17 };
struct tag_block { long count; void *address; };
#include "native_teams.inc"
#include "medical_hud_types.inc"
#include "equipment_ownership.inc"
struct object_fields { long parent_object_index, owner_player_index; unsigned long flags; short name_index; long damage_flags; real_point3d position;
 real_point3d bounding_sphere_center; real body_vitality, shield_vitality; short owner_team_index; };
struct object_datum { long definition_index; struct object_fields object; };
struct unit_datum { long definition_index; struct object_fields object;
 struct { long player_index, weapon_object_indices[4]; short current_weapon_index, desired_weapon_index, parent_seat_index;
  signed char grenade_counts[2]; short grenade_throw_state; unsigned long control_flags; } unit; };
struct weapon_datum { long definition_index; struct object_fields object;
 struct { unsigned long flags; } item;
 struct { struct { short rounds_loaded, rounds_total; } magazines[2]; float age; } weapon; };
struct control_datum { long definition_index; struct object_fields object;
 struct {short position_group_index,power_group_index;unsigned short flags;} device; };
#define device_datum control_datum
struct device_group_datum {real actual_value;unsigned short flags;};
struct player_datum { long unit_index; short local_player_index; boolean quit_out_of_game;
 struct { short machine_index, controller_index; } network_player_data; };
struct weapon_definition { struct { unsigned long flags; struct tag_block magazines; } weapon; };
struct weapon_magazine_definition { short rounds_loaded_maximum, rounds_total_maximum, rounds_total_initial; };
struct game_globals_grenade { short maximum_count; };
struct game_globals { struct tag_block grenades; };
struct control_definition { struct { short triggers_when,type;real call_value;
 struct {long index;} on_effect,off_effect,denied_effect; } control; };
struct scenario_object_name { char name[32]; short runtime_object_type; };
struct unit_definition { struct { struct tag_block seats; } unit; };
struct scenario_trigger_volume { char name[32]; };
#include "mission_types.inc"
struct player_starting_location { real_point3d position; };
struct scenario { short type; struct tag_block object_names, trigger_volumes, players, hs_scripts; };
struct object_placement_data { long definition_index; real_point3d position; };
struct delta_peer_identity { byte account[16]; int bound; };
struct data_iterator { short next; long datum_index; };
struct player_action { unsigned long control_flags; real primary_trigger; struct {real i,j;} throttle;
 short desired_zoom_level, desired_weapon_index; };
struct collision_result { long object_index; };

static struct player_datum players[8];
static long player_handles[8] = { 0x10000, 0x10001, 0x10002, 0x10003, 0x10004, 0x10005, 0x10006, 0x10007 };
static long unit_handles[8] = { 0x1000a, 0x1000b, 0x1000c, 0x1000d, 0x1000e, 0x1000f, 0x10010, 0x10011 };
static struct unit_datum units[8];
static struct unit_datum dropships[2];
static long dropship_handles[2]={0x10014,0x10015};
static struct unit_definition dropship_tag={{{2,NULL}}};
static boolean native_cinematic;
static struct unit_definition *unit_definition_get(long index) { CHECK(index==300,"native vehicle definition");return &dropship_tag; }
static short active_players=2;
static struct { long handle; struct weapon_datum datum; boolean flag; } weapons[96];
static short weapon_count, fail_allocate, fail_attach;
static boolean fail_refund, fail_confirm, publish_confirm;
static struct control_datum controls[3];
static long control_handles[3] = { 0x10003, 0x10004, 0x10005 };
static struct control_definition control_tag;
static struct device_group_datum device_groups[6];
static struct data_array device_group_table;
static struct data_array *device_groups_data=&device_group_table;
static int control_updates[3],control_effects[3];
#include "controls_types.inc"
static struct weapon_magazine_definition magazine = { 20, 200, 0 };
static struct weapon_definition weapon_tag = { { 0, { 1, &magazine } } };
static long missing_tag = NONE, unusable_tag = NONE;
static struct game_globals_grenade grenades[2] = { {4}, {4} };
static struct game_globals globals = { { 2, grenades } };
static struct scenario_object_name names[5] = { {"extraction_deploy"}, {"extraction_activate"}, {"extraction_loadout"},
 {"extraction_pelican",_object_type_vehicle},{"extraction_spirit",_object_type_vehicle} };
static struct scenario_trigger_volume volumes[2] = { {"extraction_zone"}, {"extraction_return"} };
static struct player_starting_location starts[2];
static struct scenario scenario = { _scenario_type_solo, {3,names}, {2,volumes}, {2,starts}, {0,NULL} };
static struct hs_script completion_script;
static short completion_wakes;
static boolean hs_wake_by_name(char const *name)
{ CHECK(!strcmp(name,"forerunner_extraction_complete"),"wrong mission continuation");completion_wakes++;return TRUE; }
static struct scenario *global_scenario = &scenario;
static void *player_data;
static long tick;
static boolean enabled=TRUE, director, profile=TRUE, verified=TRUE;
static boolean medical_setting, medical_support=TRUE, medical_object_sync=TRUE, wall;
static unsigned int epoch=1;
static int native_kills;
void platform_log(char const *format, ...) { (void)format; }
static byte remote_account[16] = {1};
static byte host_account[16] = {2};
static boolean delta_session;
static int local_notices, remote_notices, respawns;
static char local_notice_history[4][63];
static char last_remote_notice[128];

static boolean config_boolean(char const *name)
{ return !strcmp(name,"game.extraction") ? enabled : !strcmp(name,"game.revive") ? medical_setting :
  !strcmp(name,"game.shared_gear") ? shared_cache : director; }
static long config_integer(char const *name) { return !strcmp(name,"game.revive_seconds") ? 5 : 30; }
static boolean network_coop_active(void) { return TRUE; }
static struct { boolean enabled, gear, victory; short phase; long wave_limit,deadline,wipe_deadline; } firefight;
#include "survival.inc"
static long game_time_get(void) { return tick; }
static struct player_datum *player_try_and_get(long handle)
{ short i; for(i=0;i<active_players;i++) if(player_handles[i]==handle) return &players[i]; return NULL; }
static struct player_datum *player_get(long h) { struct player_datum *p=player_try_and_get(h); CHECK(p,"invalid player"); return p; }
static void *object_try_and_get_and_verify_type(long h,long mask)
{
 short i;
 if(h==NONE) return NULL;
 for(i=0;i<2;i++) if(h==dropship_handles[i] && (mask&_object_mask_vehicle)) return &dropships[i];
 for(i=0;i<active_players;i++) if(h==unit_handles[i] && (mask&_object_mask_biped)) return &units[i];
 for(i=0;i<3;i++) if(h==control_handles[i] && (mask&_object_mask_control)) return &controls[i];
 for(i=0;i<weapon_count;i++) if(h==weapons[i].handle && (mask&_object_mask_weapon)) return &weapons[i].datum;
 return NULL;
}
static struct unit_datum *unit_get(long h) { void *p=object_try_and_get_and_verify_type(h,_object_mask_biped|_object_mask_vehicle); CHECK(p,"invalid unit"); return p; }
static struct weapon_datum *weapon_try_and_get(long h) { return object_try_and_get_and_verify_type(h,_object_mask_weapon); }
static struct weapon_datum *weapon_get(long h) { struct weapon_datum *p=weapon_try_and_get(h); CHECK(p,"invalid weapon"); return p; }
static boolean cinematic_in_progress(void) { return native_cinematic; }
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
{ long tag=!strcmp(path,tag_get_name(1)) ? 1 : !strcmp(path,tag_get_name(2)) ? 2 : NONE;
 return tag==missing_tag ? NONE : tag; }
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
static boolean unit_can_use_weapon(long u,long w)
{ unit_get(u); return weapon_get(w)->definition_index!=unusable_tag; }
static boolean unit_drop_weapon(long u,long w)
{
 short i; struct unit_datum *unit=unit_get(u); struct weapon_datum *weapon=weapon_try_and_get(w);
 if(!weapon || (weapon->object.parent_object_index!=NONE && weapon->object.parent_object_index!=u)) return FALSE;
 for(i=0;i<4;i++) if(unit->unit.weapon_object_indices[i]==w) { unit->unit.weapon_object_indices[i]=NONE; weapon->object.parent_object_index=NONE;
  weapon->item.flags=0;weapon->object.flags|=FLAG(_object_connected_to_map_bit);return TRUE; }
 return FALSE;
}
static boolean unit_add_weapon_to_inventory(long u,long w,long mode)
{
 short i; struct unit_datum *unit=unit_get(u); struct weapon_datum *weapon=weapon_get(w);
 if(fail_attach && !--fail_attach) { if(fail_refund) fail_write=TRUE; return FALSE; }
 if(!unit_can_use_weapon(u,w)) return FALSE;
 if(weapon->object.parent_object_index!=NONE) return FALSE;
 for(i=0;i<4;i++) if(unit->unit.weapon_object_indices[i]==NONE) { unit->unit.weapon_object_indices[i]=w; weapon->object.parent_object_index=u;
  weapon->item.flags=FLAG(_item_attached_to_unit_bit)|FLAG(_item_belongs_to_player_bit);
  weapon->object.owner_player_index=unit->unit.player_index;weapon->object.flags&=~FLAG(_object_connected_to_map_bit);
  if(fail_confirm)fail_write=TRUE;if(publish_confirm)publish_failure=TRUE;return TRUE; }
 return FALSE;
}
static short local_player_get_next(short index) { return index==NONE ? 0 : NONE; }
static void hud_print_message(short local,wchar_t const *text) {
 short i;CHECK(local==0,"wrong local notice");CHECK(wcslen(text)<63,"native HUD record truncated a notice");
 for(i=0;text[i];i++)local_notice_history[local_notices%4][i]=(char)text[i];
 local_notice_history[local_notices%4][i]=0;local_notices++;
}
short distributed_client_machines(long *machines,short max) { machines[0]=1; return 1; }
/* Native distributed_host_tick maps remote clients only, never the host. */
long distributed_player_machine(short slot) { CHECK(slot>=0 && slot<active_players,"unsalted absolute player lookup"); return players[slot].local_player_index==NONE ? 1 : NONE; }
void distributed_send_notice_to_machine(long machine,char const *text) { CHECK(machine==1,"wrong remote notice"); remote_notices++;snprintf(last_remote_notice,sizeof(last_remote_notice),"%s",text); }
static long player_ui_get_active_player_profile_index(short local) { return profile ? 0 : NONE; }
static boolean player_ui_get_path_to_local_player_profile_directory(short local,char *path) { strcpy(path,"owned/profile"); return TRUE; }
static boolean delta_peer_game_player_identity(int machine,int controller,struct delta_peer_identity *id)
{ CHECK((machine==1 && controller==2)||(delta_session && machine==players[0].network_player_data.machine_index && controller==players[0].network_player_data.controller_index),"wrong authenticated controller"); memcpy(id->account,machine==1?remote_account:host_account,16); id->bound=verified; return TRUE; }
static boolean delta_peer_game_on_delta(void) { return delta_session; }
static long object_index_from_name_index(short index) { return index>=3 && index<5 ? dropship_handles[index-3] : index>=0 && index<3 ? control_handles[index] : NONE; }
static struct control_datum *control_try_and_get(long h) { return object_try_and_get_and_verify_type(h,_object_mask_control); }
static struct control_definition *control_definition_get(long h) { return &control_tag; }
static struct control_datum *control_get(long h) { struct control_datum *c=control_try_and_get(h);CHECK(c,"invalid control");return c; }
static struct device_datum *device_get(long h) { return control_get(h); }
static boolean device_group_set_desired_value(short index,real value)
{
 CHECK(index>=0&&index<3,"invalid position group");
 if((device_groups[index].flags&FLAG(_device_group_can_change_only_once_bit))&&
    (device_groups[index].flags&FLAG(_device_group_changed_once_bit)))return FALSE;
 device_groups[index].actual_value=value;device_groups[index].flags|=FLAG(_device_group_changed_once_bit);control_updates[index]++;return TRUE;
}
static void device_effect_new(long control,long effect) { for(short i=0;i<3;i++)if(control_handles[i]==control){control_effects[i]=(int)effect;return;}CHECK(FALSE,"invalid effect owner"); }
static void display_assert(void *unused,char const *path,int line,boolean fatal) { (void)unused;(void)path;(void)line;(void)fatal;abort(); }
static void system_exit(long code) { (void)code;abort(); }
boolean device_can_change_position(long control_index);
static boolean scenario_trigger_volume_test_point(short zone,real_point3d const *p) { return zone==0 ? p->x>10.f : p->x<5.f; }
static boolean scenario_trigger_volume_test_object(short zone,long h) { return scenario_trigger_volume_test_point(zone,&unit_get(h)->object.position); }
static boolean players_are_all_dead(void) { return (units[0].object.damage_flags&1) && (units[1].object.damage_flags&1); }
static void players_respawn_at_start(void) { respawns++; }
static void data_iterator_new(struct data_iterator *i,void *data) { i->next=0; }
static void *data_iterator_next(struct data_iterator *i) { if(i->next>=active_players) return NULL; i->datum_index=player_handles[i->next]; return &players[i->next++]; }

#include "equipment_flags.inc"
#include "equipment.inc"
#include "extraction.inc"
#include "controls.inc"

static struct hud_waypoint_arrow medical_arrows[2]={{"default"},{"target_blue"}};
static struct medical_hud_fixture {struct {struct tag_block arrows;struct {long index;} arrow_bitmap;} waypoint;} medical_hud={{{2,medical_arrows},{1}}};
static struct medical_hud_fixture *hud_globals=&medical_hud;
static long drawn_units[4];static real_point3d drawn_positions[4];static short drawn_styles[4],drawn_screens[4];
static short medical_draws,medical_traces;
static long local_player_get_player_index(short local) {return local>=0&&local<2 ? player_handles[local] : NONE;}
static void object_get_bounding_sphere(long u,real_point3d *p,real *r) {*p=unit_get(u)->object.bounding_sphere_center;*r=.5f;}
static void render_interpolation_object_point(long u,real_point3d *p) {(void)u;p->x+=.1f;}
static void unit_get_head_position(long u,real_point3d *p) {*p=unit_get(u)->object.bounding_sphere_center;p->z+=.6f;}
static short hud_get_nav_point_render_type(short local,real_point3d const *head,real_point3d const *p,long u)
{(void)local;(void)head;(void)p;CHECK(medical_traces<4,"unbounded medical line-of-sight tests");drawn_units[medical_traces++]=u;return wall ? 2 : 0;}
static void custom_render_nav_point(short local,real_point3d const *p,short style,short screen)
{(void)local;CHECK(medical_draws<4,"medical draw budget exceeded");drawn_positions[medical_draws]=*p;drawn_styles[medical_draws]=style;drawn_screens[medical_draws++]=screen;}
#include "medical_hud.inc"
static short medical_markers(short local) {medical_draws=medical_traces=0;hud_render_medical_nav_points(local);CHECK(medical_draws==medical_traces,"marker did not use native line-of-sight/draw path");return medical_draws;}

static void firefight_notice(char const *text) { extraction_notice(text); }
#include "evacuation.inc"
static void advance(short count) { while(count--) { tick++; extraction_update();firefight_try_evacuation(tick); } }
static void touch(short control,short player) { control_touched(control_handles[control],unit_handles[player]); }
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
 short i,j; connection=_game_connection_network_server;firefight.wave_limit=3;
 device_group_table.data=(byte *)device_groups;device_group_table.size=sizeof(device_groups[0]);device_group_table.maximum_count=6;
 for(i=0;i<2;i++) {
  players[i].unit_index=unit_handles[i]; players[i].local_player_index=i ? NONE : 0;
  players[i].network_player_data.controller_index=i ? 2 : 0; units[i].unit.player_index=player_handles[i];
  units[i].object.body_vitality=units[i].object.shield_vitality=1.f;
  units[i].object.parent_object_index=NONE;
  units[i].unit.current_weapon_index=0; units[i].unit.grenade_counts[0]=1;
  for(j=0;j<4;j++) units[i].unit.weapon_object_indices[j]=NONE;
 }
 for(i=0;i<3;i++) {controls[i].object.name_index=i;controls[i].device.position_group_index=i;controls[i].device.power_group_index=i+3;device_groups[i+3].actual_value=1.f;}
 control_tag.control.on_effect.index=1;control_tag.control.off_effect.index=2;control_tag.control.denied_effect.index=3;
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
 if(!strncmp(case_name,"transport-",10)) {
  short faction=!strcmp(case_name,"transport-covenant")?1:0;
  long craft=dropship_handles[faction],loot;
  extraction_reset();scenario.object_names.count=5;
  if(!strcmp(case_name,"transport-binding"))names[3].runtime_object_type=0;
  if(!strcmp(case_name,"transport-duplicate"))strcpy(names[4].name,"extraction_pelican");
  if(!strcmp(case_name,"transport-missing-faction"))scenario.object_names.count=4;
  for(a=0;a<2;a++) {
   dropships[a].definition_index=300;dropships[a].object.body_vitality=1;
   dropships[a].object.position.x=20;
  }
  advance(1);
  if(!strcmp(case_name,"transport-binding")||!strcmp(case_name,"transport-duplicate")) {
   CHECK(!extraction_owns_map(),"malformed transport binding admitted as a foot return");return 0;
  }
  CHECK(extraction_owns_map(),"native transport map refused");
  units[0].object.owner_team_index=faction?_game_team_covenant:_game_team_player;
  touch(0,0);loot=pickup(0,1);units[0].object.position.x=20;touch(1,0);advance(300);
  CHECK(run(0)->phase==_extraction_ready&&run(0)->gear.record.phase==_gear_deployed,"activation incorrectly settled transport loot");
  units[0].object.position.x=0;advance(1);
  CHECK(run(0)->phase==_extraction_ready&&weapon_try_and_get(loot),"foot return bypassed physical boarding");
  units[0].object.parent_object_index=craft;units[0].unit.parent_seat_index=0;
  if(!strcmp(case_name,"transport-seat"))units[0].unit.parent_seat_index=2;
  if(!strcmp(case_name,"transport-stale"))units[0].object.parent_object_index=craft+0x10000;
  if(!strcmp(case_name,"transport-wrong-faction")||!strcmp(case_name,"transport-missing-faction"))units[0].object.owner_team_index=_game_team_covenant;
  if(!strcmp(case_name,"transport-dead"))dropships[0].object.damage_flags=FLAG(_object_dead_bit);
  advance(1);
  if(!strcmp(case_name,"transport-seat")||!strcmp(case_name,"transport-stale")||
   !strcmp(case_name,"transport-wrong-faction")||!strcmp(case_name,"transport-missing-faction")||!strcmp(case_name,"transport-dead")) {
   CHECK(run(0)->dropship==NONE&&run(0)->phase==_extraction_ready,"invalid native seat/ship/faction received boarding credit");
   dropships[faction].object.position.x=0;advance(1);
   CHECK(run(0)->gear.record.phase==_gear_deployed&&weapon_try_and_get(loot),"invalid transport banked owned equipment");return 0;
  }
  CHECK(run(0)->dropship==craft&&run(0)->phase==_extraction_ready,"real assigned LZ seat not recorded");
  advance(900);CHECK(run(0)->phase==_extraction_ready,"time alone extracted seated passengers");
  if(!strcmp(case_name,"transport-disembark"))units[0].object.parent_object_index=NONE;
  if(!strcmp(case_name,"transport-team-change"))units[0].object.owner_team_index=_game_team_flood;
  if(!strcmp(case_name,"transport-replaced")) {
   dropship_handles[faction]+=0x10000;units[0].object.parent_object_index=dropship_handles[faction];
  }
  if(!strcmp(case_name,"transport-disembark")||!strcmp(case_name,"transport-team-change")||!strcmp(case_name,"transport-replaced")) {
   advance(1);CHECK(run(0)->dropship==NONE,"lost seat/body/faction kept stale boarding receipt");
   dropships[faction].object.position.x=0;units[0].object.parent_object_index=dropship_handles[faction];
   units[0].object.owner_team_index=faction?_game_team_covenant:_game_team_player;advance(1);
   CHECK(run(0)->phase==_extraction_ready&&weapon_try_and_get(loot),"reboarding at destination bypassed the landing zone");return 0;
  }
  dropships[faction].object.position.x=7;advance(1);
  CHECK(run(0)->phase==_extraction_ready&&weapon_try_and_get(loot),"in-flight/local body position caused early recovery");
  dropships[faction].object.position.x=0;native_cinematic=TRUE;advance(1);
  CHECK(run(0)->phase==_extraction_ready&&weapon_try_and_get(loot),"cinematic banked before departure sequence ended");
  native_cinematic=FALSE;advance(1);
  CHECK(run(0)->phase==_extraction_returned&&run(0)->gear.record.count==1&&!weapon_try_and_get(loot),"boarded arrival did not bank exactly once");
  advance(30);CHECK(run(0)->gear.record.count==1,"arrived passenger duplicated loot");
  extraction_reset();return 0;
 }
 if(!strcmp(case_name,"notice-complete")) {
  touch(0,0);pickup(0,1);escape(0);
  CHECK(strstr(local_notice_history[(local_notices-1)%4],"RUN COMPLETE | Stash"),"bank report headline not queued first for display");
  CHECK(strstr(local_notice_history[(local_notices-2)%4],"next run."),"bank report lost redeployment instructions");
  extraction_reset();return 0;
 }
 if(!strncmp(case_name,"stowed-",7)) {
  struct game_modes_inventory captured;short room[2];long stowed;
  pickup(0,1);stowed=pickup(0,2);weapon_get(stowed)->object.parent_object_index=NONE;
  if(!strcmp(case_name,"stowed-loop")) {
   CHECK(game_modes_inventory_capture(player_handles[0],&captured)&&captured.weapon_count==2,
    "native stowed weapon blocked capture");
   touch(0,0);CHECK(run(0)->phase==_extraction_deployed,"stowed starting gear blocked deployment");
   escape(0);CHECK(run(0)->gear.record.count==2&&!weapon_try_and_get(stowed),"stowed loot not consumed and banked once");
   touch(0,0);CHECK(run(0)->phase==_extraction_deployed&&run(0)->gear.record.reserved_count==2,"stowed gear not redeployable");
   weapon_get(units[0].unit.weapon_object_indices[1])->object.parent_object_index=NONE;
   escape(0);CHECK(run(0)->gear.record.count==2,"redeployed stowed gear duplicated or lost");
   extraction_reset();advance(1);touch(2,0);CHECK(run(0)->gear.record.count==2,"stowed bank did not survive reopen");
  } else {
   if(!strcmp(case_name,"stowed-owner"))weapon_get(stowed)->object.owner_player_index=player_handles[1];
   else if(!strcmp(case_name,"stowed-ground"))weapon_get(stowed)->object.flags|=FLAG(_object_connected_to_map_bit);
   else if(!strcmp(case_name,"stowed-unmarked"))weapon_get(stowed)->item.flags&=~FLAG(_item_attached_to_unit_bit);
   else if(!strcmp(case_name,"stowed-nonplayer"))weapon_get(stowed)->item.flags&=~FLAG(_item_belongs_to_player_bit);
   else if(!strcmp(case_name,"stowed-parent"))weapon_get(stowed)->object.parent_object_index=unit_handles[1];
   else CHECK(FALSE,"unknown stowed case");
   CHECK(!game_modes_inventory_capture(player_handles[0],&captured)&&!captured.weapon_count,
    "unsafe detached ownership recovered loot");
   CHECK(!game_modes_inventory_grenade_room(player_handles[0],room),"unsafe inventory admitted loadout");
   touch(0,0);CHECK(run(0)->phase==_extraction_hub,"unsafe inventory deployed");
  }
  extraction_reset();return 0;
 }
 if(!strncmp(case_name,"mission-",8)) {
  strcpy(completion_script.name,"forerunner_extraction_complete");
  completion_script.script_type=_hs_script_dormant;completion_script.return_type=_hs_type_void;
  scenario.hs_scripts.count=1;scenario.hs_scripts.address=&completion_script;
  advance(1);CHECK(!completion_wakes,"idle hub completed an operation");
  touch(0,0);touch(0,1);pickup(0,1);pickup(1,2);
  escape(0);CHECK(!completion_wakes,"first survivor completed the whole operation");
  if(!strcmp(case_name,"mission-signature")||!strcmp(case_name,"mission-return-type")) {
   if(!strcmp(case_name,"mission-signature"))completion_script.script_type=_hs_script_continuous;
   else completion_script.return_type=_hs_type_boolean;
   escape(1);CHECK(!completion_wakes,"incompatible mission continuation was woken");
  } else if(!strcmp(case_name,"mission-restore")) {
   extraction_restored();advance(1);CHECK(!completion_wakes,"restored world completed an operation");
  } else {
   units[1].object.position.x=20.f;touch(1,1);advance(300);
   units[1].object.position.x=0.f;
   if(!strcmp(case_name,"mission-pending")) {
    publish_failure=TRUE;advance(1);
    CHECK(run(1)->phase==_extraction_settling&&!completion_wakes,"uncertain banking completed an operation");
    publish_failure=FALSE;advance(30);
   } else {
    units[0].object.damage_flags=FLAG(_object_die_act_of_god_silent_bit);advance(1);
    CHECK(run(1)->phase==_extraction_returned&&!completion_wakes,"pending native death completed an operation");
    units[0].object.damage_flags=0;advance(1);
   }
   CHECK(completion_wakes==1,"durably returned survivors did not wake authored completion");
   advance(30);touch(0,0);escape(0);
   CHECK(completion_wakes==1,"same-map redeployment repeated a one-shot completion");
  }
  extraction_reset();return 0;
 }
 if(!strcmp(case_name,"pending-deploy")||!strcmp(case_name,"pending-escape")) {
  short flags[]={_object_die_act_of_god_bit,_object_die_act_of_god_silent_bit,_object_die_act_of_god_no_statistics_bit};
  short i;struct game_modes_inventory captured;byte item_id[GEAR_ID_BYTES];
  touch(0,0);a=pickup(0,1);escape(0);
  memcpy(item_id,run(0)->gear.record.items[0].id,GEAR_ID_BYTES);
  if(!strcmp(case_name,"pending-escape")) {
   touch(0,0);a=units[0].unit.weapon_object_indices[0];
   units[0].object.position.x=20.f;touch(1,0);advance(300);units[0].object.position.x=0.f;
  }
  for(i=0;i<3;i++) {
   unsigned long generation=run(0)->gear.record.generation;
   units[0].object.damage_flags=FLAG(flags[i]);
   CHECK(!game_modes_inventory_capture(player_handles[0],&captured),"pending native death captured recoverable equipment");
   if(!strcmp(case_name,"pending-deploy")) {
    touch(0,0);CHECK(run(0)->phase==_extraction_returned&&run(0)->gear.record.count==1,
     "pending native death deployed persistent equipment");
   } else {
    advance(1);CHECK(run(0)->phase==_extraction_ready&&weapon_try_and_get(a)&&!run(0)->gear.record.count,
     "pending native death banked a successful escape or consumed native loot");
   }
   CHECK(run(0)->gear.record.generation==generation,"pending death changed the persistent stash");
  }
  units[0].object.damage_flags=0;
  if(!strcmp(case_name,"pending-deploy")) {
   touch(0,0);CHECK(run(0)->phase==_extraction_deployed,"cleared native request prevented legitimate deployment");
  }
  else {
   units[0].object.damage_flags=FLAG(_object_dead_bit);advance(1);
   CHECK(run(0)->phase==_extraction_lost&&!run(0)->gear.record.count,"native death earned escape rewards");
   /* Halo releases the exact carried weapon; a survivor takes it through
    * native ownership, not a new corpse inventory or a former-owner claim. */
   CHECK(unit_drop_weapon(unit_handles[0],a),"native corpse drop failed");touch(0,1);
   CHECK(unit_add_weapon_to_inventory(unit_handles[1],a,0),"survivor could not pick up death loot");escape(1);
   CHECK(run(1)->gear.record.count==1&&!run(0)->gear.record.count&&!weapon_try_and_get(a)&&
    !memcmp(item_id,run(1)->gear.record.items[0].id,GEAR_ID_BYTES),"death loot did not settle once with its original identity for the survivor");
  }
  extraction_reset();return 0;
 } else if(!strncmp(case_name,"biped-",6)) {
  byte ids[2][GEAR_ID_BYTES];long carried[2];short index;
  touch(0,0);pickup(0,1);pickup(0,2);escape(0);
  for(index=0;index<2;index++)memcpy(ids[index],run(0)->gear.record.items[index].id,GEAR_ID_BYTES);
  carried[0]=pickup(0,1);carried[1]=pickup(0,2);
  units[0].unit.current_weapon_index=units[0].unit.desired_weapon_index=1;
  before=weapon_count;
  unusable_tag=!strcmp(case_name,"biped-first") ? 1 : 2;
  if(!strcmp(case_name,"biped-survival")) {
   enabled=FALSE;director=TRUE;firefight.enabled=firefight.gear=TRUE;firefight.phase=1;
   extraction_reset();advance(1);
  } else touch(0,0);
  CHECK(run(0)->phase==_extraction_returned || run(0)->phase==_extraction_hub,
   "unusable biped loadout was deployed");
  CHECK(run(0)->gear.record.count==2&&!run(0)->gear.record.reserved_count,
   "unusable native weapon lost its reservation refund");
  for(index=0;index<2;index++) {
   CHECK(units[0].unit.weapon_object_indices[index]==carried[index]&&
    weapon_get(carried[index])->object.parent_object_index==unit_handles[0],
    "known-incompatible loadout dropped carried equipment before admission");
   CHECK(!memcmp(ids[index],run(0)->gear.record.items[index].id,GEAR_ID_BYTES),
    "refused native loadout changed saved item identity");
  }
  CHECK(units[0].unit.current_weapon_index==1&&units[0].unit.desired_weapon_index==1,
   "known-incompatible loadout changed the carried selection");
  for(index=before;index<weapon_count;index++)CHECK(weapons[index].handle==NONE,
   "refused native loadout left newly created world loot");
  unusable_tag=NONE;
  if(director)advance(30);else touch(0,0);
  CHECK(run(0)->phase==_extraction_deployed&&run(0)->gear.record.reserved_count==2,
   "compatible body could not retry the same saved loadout");
  for(index=0;index<2;index++)CHECK(!memcmp(ids[index],run(0)->gear.record.reserved[index].id,GEAR_ID_BYTES),
   "retry duplicated or substituted recovered equipment");
  if(director){firefight.phase=4;firefight.victory=TRUE;advance(1);}else escape(0);
  CHECK(run(0)->phase==_extraction_returned&&run(0)->gear.record.count==2,
   "retried loadout did not complete the existing recovery loop");
  extraction_reset();return 0;
 }
 if(!strcmp(case_name,"initial-ammo")) {
  struct game_modes_inventory chosen;
  byte id[GEAR_ID_BYTES];
  magazine.rounds_total_maximum=40;magazine.rounds_total_initial=63;
  touch(0,1);pickup(1,1);escape(1);
  memcpy(id,run(1)->gear.record.items[0].id,sizeof(id));
  touch(2,1);
  CHECK(gear_loadout(&run(1)->gear,&chosen)&&chosen.weapon_count==1&&
   game_modes_inventory_loadout_valid(&chosen),"authored starting reserve stranded recovered equipment");
  chosen.weapons[0].rounds_reserve[0]=64;
  CHECK(!game_modes_inventory_loadout_valid(&chosen),"reserve above both native limits was admitted");
  touch(0,1);a=units[1].unit.weapon_object_indices[0];
  CHECK(run(1)->phase==_extraction_deployed&&weapon_get(a)->weapon.magazines[0].rounds_total==63,
   "starting reserve was clipped or failed to deploy");
  escape(1);
  CHECK(run(1)->gear.record.count==1&&!memcmp(id,run(1)->gear.record.items[0].id,sizeof(id))&&
   run(1)->gear.record.items[0].equipment.rounds_reserve[0]==63&&!weapon_try_and_get(a),
   "redeployed starting ammunition changed identity, duplicated or lost rounds");
  extraction_reset();return 0;
 }
 if(!strncmp(case_name,"refund-",7)) {
  byte ids[2][GEAR_ID_BYTES];
  short index;
  touch(0,1);pickup(1,1);pickup(1,2);escape(1);
  for(index=0;index<2;index++) memcpy(ids[index],run(1)->gear.record.items[index].id,GEAR_ID_BYTES);
  before=weapon_count;fail_attach=2;fail_refund=TRUE;touch(0,1);
  CHECK(run(1)->phase==_extraction_cancelling&&run(1)->gear.record.reserved_count==2&&
   !run(1)->gear.record.count,"failed refund was not pending");
  CHECK(!weapon_try_and_get(0x10020+before)&&!weapon_try_and_get(0x10021+before),"ungranted equipment survived refund");
  fail_refund=FALSE;if(!strstr(case_name,"outage"))fail_write=FALSE;
  if(!strcmp(case_name,"refund-disconnect")||!strcmp(case_name,"refund-disconnect-outage")) {
   players[1].quit_out_of_game=TRUE;advance(1);
   CHECK(!run(1)->gear.opened,"disconnected refund retained bank lock");
   players[1].quit_out_of_game=FALSE;advance(1);
  } else if(!strcmp(case_name,"refund-recycled")||!strcmp(case_name,"refund-recycled-outage")) {
   player_handles[1]+=0x10000;units[1].unit.player_index=player_handles[1];advance(1);
  } else if(!strcmp(case_name,"refund-map-exit")||!strcmp(case_name,"refund-map-outage")) {
   extraction_reset();advance(1);
  } else if(!strcmp(case_name,"refund-retry")) {
   advance(30);
  } else return 2;
  fail_write=FALSE;touch(2,1);
  CHECK(run(1)->gear.record.count==2&&!run(1)->gear.record.reserved_count&&
   run(1)->gear.record.grenades[0]==1,"ungranted reservation became departure loss or lost grenades");
  for(index=0;index<2;index++) CHECK(gear_has(run(1)->gear.record.items,2,ids[index]),"refund changed owned item identity");
  touch(0,1);
  CHECK(run(1)->phase==_extraction_deployed&&run(1)->gear.record.reserved_count==2,
   "refunded owner could not redeploy");
  escape(1);
  CHECK(run(1)->gear.record.count==2&&!run(1)->gear.record.reserved_count,
   "redeployed refund did not settle exactly once");
  extraction_reset();return 0;
 }
 if(!strcmp(case_name,"reservation-published")) {
  byte ids[2][GEAR_ID_BYTES];short i;
  touch(0,1);pickup(1,1);pickup(1,2);escape(1);
  for(i=0;i<2;i++)memcpy(ids[i],run(1)->gear.record.items[i].id,GEAR_ID_BYTES);
  before=weapon_count;publish_failure=TRUE;touch(0,1);
  CHECK(run(1)->phase==_extraction_returned&&run(1)->gear.pending&&run(1)->gear.record.phase==_gear_reserved&&weapon_count==before,
   "unflushed preparation granted native equipment");
  players[1].quit_out_of_game=TRUE;advance(1);CHECK(!run(1)->gear.opened,"unissued preparation retained owner lock");
  publish_failure=FALSE;players[1].quit_out_of_game=FALSE;advance(1);touch(2,1);
  CHECK(run(1)->gear.record.count==2&&!run(1)->gear.record.reserved_count,"unissued published preparation lost owned equipment");
  for(i=0;i<2;i++)CHECK(!memcmp(ids[i],run(1)->gear.record.items[i].id,GEAR_ID_BYTES),"unissued preparation changed saved identity");
  extraction_reset();return 0;
 }
 if(!strcmp(case_name,"confirmation-outage")||!strcmp(case_name,"confirmation-published")) {
  byte ids[2][GEAR_ID_BYTES];short i;
  touch(0,1);pickup(1,1);pickup(1,2);escape(1);
  for(i=0;i<2;i++)memcpy(ids[i],run(1)->gear.record.items[i].id,GEAR_ID_BYTES);
  units[1].unit.grenade_counts[0]=1;before=weapon_count;
  if(!strcmp(case_name,"confirmation-outage"))fail_confirm=TRUE;else publish_confirm=TRUE;
  touch(0,1);
  if(fail_confirm) {
   CHECK(run(1)->phase==_extraction_cancelling&&run(1)->gear.record.phase==_gear_reserved,
    "unpublished native confirmation became an issued deployment");
   CHECK(units[1].unit.grenade_counts[0]==1,"confirmation rollback lost or duplicated preexisting grenades");
   for(i=before;i<weapon_count;i++)CHECK(weapons[i].handle==NONE,"unissued confirmation left usable recovered equipment");
   players[1].quit_out_of_game=TRUE;advance(1);CHECK(!run(1)->gear.opened,"outage disconnect retained owner lock");
   fail_confirm=fail_write=FALSE;players[1].quit_out_of_game=FALSE;advance(1);touch(2,1);
   CHECK(run(1)->gear.record.count==2&&!run(1)->gear.record.reserved_count,
    "unissued confirmation lost saved equipment after reconnect");
   touch(0,1);
  } else {
   CHECK(run(1)->phase==_extraction_deployed&&run(1)->gear.pending&&run(1)->gear.record.phase==_gear_deployed,
    "published confirmation revoked the issued loadout");
   for(i=before;i<weapon_count;i++)CHECK(weapons[i].handle!=NONE&&weapons[i].datum.object.parent_object_index==unit_handles[1],
    "published native deployment lost its owned equipment");
   publish_confirm=publish_failure=FALSE;
  }
  CHECK(run(1)->phase==_extraction_deployed,"confirmed retry failed to deploy");escape(1);
  CHECK(run(1)->gear.record.count==2&&run(1)->gear.record.grenades[0]==2,
   "confirmed recovery duplicated or lost weapons/grenades");
  for(i=0;i<2;i++)CHECK(!memcmp(ids[i],run(1)->gear.record.items[i].id,GEAR_ID_BYTES),"native confirmation changed recovered item identity");
  extraction_reset();return 0;
 }
 if(!strcmp(case_name,"shared-cache")||!strcmp(case_name,"shared-cache-reject")||!strcmp(case_name,"shared-cache-discard")) {
  struct game_modes_inventory chosen,stale;
  byte original[GEAR_ID_BYTES];unsigned long generation;
  touch(0,0);pickup(0,1);pickup(0,2);escape(0);
  CHECK(gear_loadout(&run(0)->gear,&stale)&&stale.weapon_count==2,"source loadout missing");
  memcpy(original,run(0)->gear.record.items[1].id,sizeof(original));
  extraction_reset();map="survival@ce";checksum++;advance(1);touch(2,0);
  CHECK(gear_loadout(&run(0)->gear,&chosen)&&!chosen.weapon_count&&run(0)->gear.record.count==2,
   "default cache isolation changed");
  CHECK(!game_modes_inventory_loadout_valid(&stale),"stale snapshot bypassed native cache admission");
  shared_cache=TRUE;
  if(!strcmp(case_name,"shared-cache-reject")) {
   generation=run(0)->gear.record.generation;missing_tag=1;touch(2,0);
   CHECK(gear_loadout(&run(0)->gear,&chosen)&&chosen.weapon_count==1&&
    !strcmp(chosen.weapons[0].tag_path,tag_get_name(2)),"missing native tag was granted or hid compatible gear");
   missing_tag=NONE;weapon_tag.weapon.flags=FLAG(_weapon_must_be_readied_bit);
   CHECK(gear_loadout(&run(0)->gear,&chosen)&&!chosen.weapon_count,"target objective definition became personal loot");
   weapon_tag.weapon.flags=0;weapon_tag.weapon.magazines.count=0;
   CHECK(gear_loadout(&run(0)->gear,&chosen)&&!chosen.weapon_count,"absent magazines discarded stored ammunition");
   weapon_tag.weapon.magazines.count=1;magazine.rounds_loaded_maximum=6;
   CHECK(gear_loadout(&run(0)->gear,&chosen)&&!chosen.weapon_count,"target magazine silently clipped recovered rounds");
   magazine.rounds_loaded_maximum=20;magazine.rounds_total_maximum=62;
   CHECK(gear_loadout(&run(0)->gear,&chosen)&&!chosen.weapon_count,"target reserve silently clipped recovered ammunition");
   CHECK(run(0)->gear.record.count==2&&run(0)->gear.record.generation==generation&&
    run(0)->gear.record.items[0].equipment.rounds_loaded[0]==7,
    "incompatible native definitions changed the owned stash");
  } else {
   touch(2,0);
   CHECK(gear_loadout(&run(0)->gear,&chosen)&&chosen.weapon_count==2&&chosen.map_checksum==checksum&&
    !strcmp(chosen.map_name,map)&&!strcmp(chosen.weapons[0].tag_path,tag_get_name(2))&&
    !strcmp(run(0)->gear.record.items[0].map_name,"candidate@ce"),"shared terminal lost target admission or origin provenance");
   if(!strcmp(case_name,"shared-cache-discard")) {
    units[0].unit.control_flags=FLAG(_unit_control_crouch_modifier_bit);touch(2,0);advance(30);touch(2,0);
    CHECK(run(0)->gear.record.count==1&&!gear_has(run(0)->gear.record.items,run(0)->gear.record.count,original),
     "confirmed foreign-cache discard failed or removed another weapon");
   } else {
    touch(0,0);a=units[0].unit.weapon_object_indices[0];
    CHECK(run(0)->phase==_extraction_deployed&&run(0)->gear.record.reserved_count==2&&
     !run(0)->gear.record.count&&weapon_get(a)->definition_index==2&&weapon_get(a)->weapon.magazines[0].rounds_loaded==7,
     "shared deployment changed selection, quantities or reservation");
    weapon_get(a)->weapon.magazines[0].rounds_loaded=3;weapon_get(a)->weapon.age=.5f;escape(0);
    CHECK(run(0)->gear.record.count==2&&!memcmp(original,run(0)->gear.record.items[0].id,sizeof(original))&&
     !strcmp(run(0)->gear.record.items[0].map_name,map)&&run(0)->gear.record.items[0].map_checksum==checksum,
     "target recovery duplicated identity or retained the old cache provenance");
    extraction_reset();map="zombies@ce";checksum++;enabled=FALSE;director=TRUE;
    firefight.enabled=firefight.gear=TRUE;firefight.phase=1;advance(1);
    a=units[0].unit.weapon_object_indices[0];
    CHECK(run(0)->phase==_extraction_deployed&&weapon_get(a)->definition_index==2&&
     weapon_get(a)->weapon.magazines[0].rounds_loaded==3&&weapon_get(a)->weapon.age==.5f&&
     !memcmp(original,run(0)->gear.record.reserved[0].id,sizeof(original)),"different-cache Survival lost recovered gear or identity");
    firefight.phase=4;firefight.victory=TRUE;advance(1);
    CHECK(run(0)->phase==_extraction_returned&&run(0)->gear.record.count==2&&!weapon_try_and_get(a),
     "shared Survival victory did not consume and bank gear exactly once");
   }
  }
  extraction_reset();return 0;
 }
 if(!strcmp(case_name,"native-controls")) {
  device_groups[3].actual_value=0.f;touch(0,0);
  CHECK(run(0)->phase==_extraction_hub&&!file_count&&!control_updates[0],"unpowered deployment changed gear or signaled scripts");
  device_groups[3].actual_value=1.f;fail_write=TRUE;touch(0,0);
  CHECK(run(0)->phase==_extraction_hub&&!control_updates[0],"failed reservation signaled authored deployment");
  fail_write=FALSE;touch(0,0);
  CHECK(run(0)->phase==_extraction_deployed&&control_updates[0]==1&&device_groups[0].actual_value==1.f&&control_effects[0]==1,"accepted deployment did not reach native state/effects");
  touch(0,0);CHECK(control_updates[0]==1,"duplicate deployment retriggered authored mission");
  device_groups[0].flags|=FLAG(_device_group_can_change_only_once_bit);touch(0,1);
  CHECK(run(1)->phase==_extraction_hub&&!run(1)->gear.opened&&control_updates[0]==1,"one-use mission lock reserved another player's gear");
  units[0].object.position.x=20.f;device_groups[4].actual_value=0.f;touch(1,0);
  CHECK(run(0)->phase==_extraction_deployed&&!control_updates[1],"unpowered extraction activated");
  device_groups[4].actual_value=1.f;controls[1].device.flags=FLAG(_device_not_usable_bit);touch(1,0);
  CHECK(run(0)->phase==_extraction_deployed&&!control_updates[1],"locked mission control activated");
  controls[1].device.flags=0;touch(1,0);touch(1,0);
  CHECK(run(0)->phase==_extraction_activating&&control_updates[1]==1&&control_effects[1]==1,"accepted activation did not signal exactly once");
  advance(300);units[0].object.position.x=0.f;advance(1);
  CHECK(run(0)->phase==_extraction_returned,"native signal handoff broke durable return");
  controls[2].object.name_index=NONE;touch(2,0);CHECK(control_updates[2]==1,"ordinary native control was intercepted");
  return 0;
 }
 if(!strncmp(case_name,"medical-",8)) {
  enabled=FALSE; medical_setting=TRUE; extraction_reset(); advance(1);
  if(!strcmp(case_name,"medical-pending-lethal")) {
   short flags[]={_object_die_act_of_god_bit,_object_die_act_of_god_silent_bit,_object_die_act_of_god_no_statistics_bit};
   short i;units[1].object.body_vitality=-.1f;
   for(i=0;i<3;i++) {
    units[1].object.damage_flags=FLAG(flags[i]);
    CHECK(!extraction_medical_lethal(unit_handles[1])&&!medical_players[1].used&&units[1].object.body_vitality==-.1f,
     "native forced death became a living medical casualty");
   }
   units[1].object.damage_flags=0;
   CHECK(extraction_medical_lethal(unit_handles[1]),"refused forced death consumed a legitimate incapacitation");
  } else if(!strcmp(case_name,"medical-guidance-client")) {
   client=TRUE;medical_enabled=FALSE;units[0].object.body_vitality=units[1].object.body_vitality=0.f;
   CHECK(!medical_markers(0),"client invented an unreceived casualty marker");
   extraction_medical_network_state(unit_handles[1],TRUE,epoch,(unsigned short)tick);
   CHECK(medical_markers(0)==1,"negotiated host casualty did not reach a living zero-quantized-health client");
   units[0].object.body_vitality=-.1f;CHECK(!medical_markers(0),"negative-health viewer received guidance");
   units[0].object.body_vitality=NAN;CHECK(!medical_markers(0),"invalid-health viewer received guidance");units[0].object.body_vitality=0.f;
   epoch++;CHECK(!medical_markers(0),"old native epoch leaked a casualty marker");
   tick++;extraction_medical_network_state(unit_handles[1],TRUE,epoch,(unsigned short)tick);
   CHECK(medical_markers(0)==1,"current epoch failed to restore received guidance");
   tick++;extraction_medical_network_state(unit_handles[1],FALSE,epoch,(unsigned short)tick);
   CHECK(!medical_markers(0),"received recovery retained a medical marker");
  } else if(!strcmp(case_name,"medical-guidance-budget")) {
   short i;active_players=8;
   for(i=1;i<active_players;i++) {
    players[i].unit_index=unit_handles[i];players[i].local_player_index=NONE;
    units[i].unit.player_index=player_handles[i];units[i].object.parent_object_index=NONE;
    units[i].object.bounding_sphere_center.x=8.f-i;units[i].object.body_vitality=-.1f;
    CHECK(extraction_medical_lethal(unit_handles[i]),"crowded casualty setup failed");
   }
   CHECK(medical_markers(0)==4&&medical_traces==4,"nearest medical draw/trace bound lost");
   for(i=0;i<4;i++)CHECK(drawn_units[i]==unit_handles[7-i],"medical markers did not prioritize nearest casualties");
   players[7].quit_out_of_game=TRUE;CHECK(medical_markers(0)==4&&drawn_units[0]==unit_handles[6],"departed casualty retained guidance");
  } else if(!strcmp(case_name,"medical-authority")) {
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
   if(!strcmp(case_name,"medical-guidance")) {
    CHECK(medical_markers(0)==1&&drawn_units[0]==unit_handles[1]&&drawn_styles[0]==1&&drawn_screens[0]==0,
     "living ally did not use the native blue target marker");
    CHECK(fabsf(drawn_positions[0].x-.1f)<.0001f&&fabsf(drawn_positions[0].z-.35f)<.0001f,
     "medical marker ignored render interpolation or casualty offset");
    wall=TRUE;CHECK(medical_markers(0)==1&&drawn_screens[0]==2,"occluded casualty bypassed native waypoint selection");wall=FALSE;
    CHECK(medical_markers(0)==1&&!file_count,"repeated local/eye draw modified the bank");
    units[1].object.owner_team_index=1;CHECK(!medical_markers(0),"enemy casualty position was revealed");units[1].object.owner_team_index=0;
    players[1].quit_out_of_game=TRUE;CHECK(!medical_markers(0),"quit casualty position was revealed");players[1].quit_out_of_game=FALSE;
    units[1].object.damage_flags=FLAG(_object_dead_bit);CHECK(!medical_markers(0),"dead corpse retained a revive marker");
    units[1].object.damage_flags=FLAG(_object_die_act_of_god_bit);CHECK(!medical_markers(0),"pending death retained a revive marker");units[1].object.damage_flags=0;
    units[0].unit.player_index=player_handles[1];CHECK(!medical_markers(0),"stale viewer body ownership revealed medical guidance");units[0].unit.player_index=player_handles[0];
    players[0].local_player_index=NONE;CHECK(!medical_markers(0),"unowned local view revealed guidance");players[0].local_player_index=0;
    CHECK(!medical_markers(NONE)&&!medical_markers(MAXIMUM_NUMBER_OF_LOCAL_PLAYERS),"invalid local view drew medical guidance");
    players[1].local_player_index=1;CHECK(!medical_markers(1),"incapacitated local player drew aid targets");players[1].local_player_index=NONE;
    unit_handles[1]+=0x10000;players[1].unit_index=unit_handles[1];CHECK(!medical_markers(0),"recycled body inherited a medical marker");unit_handles[1]-=0x10000;players[1].unit_index=unit_handles[1];
    units[1].object.body_vitality=.25f;CHECK(!medical_markers(0),"recovered native body retained its revive marker");
   } else if(!strcmp(case_name,"medical-guidance-styles")) {
    medical_arrows[1].name[0]=0;CHECK(medical_markers(0)==1&&drawn_styles[0]==0,"custom HUD lost native default marker fallback");
    medical_arrows[0].name[0]=0;CHECK(!medical_markers(0),"missing native marker art produced an invalid draw");
    strcpy(medical_arrows[1].name,"target_blue");medical_hud.waypoint.arrow_bitmap.index=NONE;
    CHECK(!medical_markers(0),"missing native waypoint bitmap produced an invalid draw");
   } else if(!strcmp(case_name,"medical-guidance-countdown")) {
    int notices=remote_notices;
    advance((short)(medical_players[1].deadline-tick-25*TICKS_PER_SECOND-1));
    CHECK(remote_notices==notices,"bleedout guidance spammed every tick");advance(1);
    CHECK(remote_notices==notices+1&&strstr(last_remote_notice,"25 seconds"),"host bleedout deadline did not reach the casualty");
    extraction_update();CHECK(remote_notices==notices+1,"same-tick medical update repeated the countdown");
    advance(20*TICKS_PER_SECOND);
    CHECK(remote_notices==notices+5&&strstr(last_remote_notice,"5 seconds"),"bounded five-second bleedout reminders lost urgency");
    advance(TICKS_PER_SECOND);
    CHECK(remote_notices==notices+6&&strstr(last_remote_notice,"4 seconds"),"final bleedout seconds did not update");
    units[1].object.body_vitality=.25f;advance(150);
    CHECK(remote_notices==notices+6&&!medical_markers(0),"recovery retained countdown or marker");
   } else if(!strcmp(case_name,"medical-pending-final")) {
    short flags[]={_object_die_act_of_god_bit,_object_die_act_of_god_silent_bit,_object_die_act_of_god_no_statistics_bit};
    short i,pending;
    for(pending=0;pending<2;pending++)for(i=0;i<3;i++) {
     struct player_action action;long now=tick;
     medical_players[1].progress=149;medical_players[1].healer=player_handles[0];
     medical_players[1].healer_unit=unit_handles[0];medical_players[1].last_control=now;
     memset(&action,0,sizeof(action));action.control_flags=FLAG(_unit_control_swap_weapons_bit);tick++;
     extraction_medical_control(player_handles[0],&action);
     CHECK(medical_players[1].progress==150,"final treatment tick was not queued before death");
     units[pending].object.damage_flags=FLAG(flags[i]);extraction_update();
     CHECK(units[1].object.body_vitality==0.f&&!medical_players[1].progress,"native death on final tick completed revival");
     units[pending].object.damage_flags=0;
    }
    treat(150,FALSE);CHECK(units[1].object.body_vitality==.25f,"interrupted native death prevented legitimate retry");
   } else if(!strcmp(case_name,"medical-pending-target")||!strcmp(case_name,"medical-pending-healer")) {
    short flags[]={_object_die_act_of_god_bit,_object_die_act_of_god_silent_bit,_object_die_act_of_god_no_statistics_bit};
    short i;long pending=!strcmp(case_name,"medical-pending-target") ? 1 : 0;
    for(i=0;i<3;i++) {
     units[pending].object.damage_flags=FLAG(flags[i]);
     CHECK(!medical_markers(0),"pending native death retained medical guidance");
     treat(150,FALSE);
     CHECK(units[1].object.body_vitality==0.f&&!medical_players[1].progress,"pending native death participated in successful revival");
     units[pending].object.damage_flags=0;
    }
    treat(150,FALSE);CHECK(units[1].object.body_vitality==.25f,"cleared native request prevented later legitimate revival");
   } else if(!strcmp(case_name,"medical-revive")) {
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
    CHECK(native_kills && !extraction_medical_host()&&!medical_markers(0),"saved critical body bypassed operation failure/quarantine or retained guidance");
   } else if(!strcmp(case_name,"medical-gear")) {
    /* Downed remains alive and owns the same native items until actual death. */
    units[1].object.body_vitality=1.f; medical_setting=FALSE; extraction_reset(); enabled=TRUE; advance(1); touch(0,1); a=pickup(1,2);
    medical_enabled=TRUE; units[1].object.body_vitality=-.1f; CHECK(extraction_medical_lethal(unit_handles[1]),"geared player did not incapacitate");
    advance(1); CHECK(run(1)->phase==_extraction_deployed && weapon_get(a)->object.parent_object_index==unit_handles[1],"downing lost owned gear");
    units[1].object.damage_flags |= FLAG(_object_dead_bit); advance(1);
    CHECK(run(1)->phase==_extraction_lost && !run(1)->gear.record.reserved_count,"actual death retained reservation");
   } else return 2;
  }
 } else if(!strncmp(case_name,"evac-",5)) {
  extraction_reset();enabled=FALSE;director=TRUE;
  firefight.enabled=firefight.gear=TRUE;firefight.phase=1;firefight.wave_limit=0;
  if(!strcmp(case_name,"evac-binding")) {
   names[1].name[0]=0;advance(1);
   CHECK(!extraction.admitted&&!file_count&&!run(0)->gear.opened,"endless missing activation reserved personal gear");
   strcpy(names[1].name,"extraction_activate");starts[1].position.x=20.f;extraction_reset();advance(1);
   CHECK(!extraction.admitted&&!file_count,"endless field spawn admitted as a safe return hub");
   starts[1].position.x=0.f;control_tag.control.triggers_when=1;extraction_reset();advance(1);
   CHECK(!extraction.admitted&&!file_count,"endless non-player control admitted");
   control_tag.control.triggers_when=0;extraction_reset();advance(1);
   CHECK(extraction.admitted&&extraction.evacuation&&run(0)->phase==_extraction_deployed,"valid endless native recovery refused");
  } else {
   advance(1);
   CHECK(extraction.survival&&extraction.evacuation&&!extraction_owns_map()&&
    run(0)->phase==_extraction_deployed&&run(1)->phase==_extraction_deployed,"Survival recovery bindings or director ownership failed");
   a=pickup(0,1);pickup(1,2);
   if(!strcmp(case_name,"evac-finite-win")||!strcmp(case_name,"evac-finite-loss")) {
    firefight.wave_limit=3;units[0].object.position.x=20.f;touch(1,0);
    CHECK(run(0)->phase==_extraction_activating,"native evacuation activation failed");
    firefight.phase=4;firefight.victory=!strcmp(case_name,"evac-finite-win");advance(1);
    if(firefight.victory) CHECK(run(0)->phase==_extraction_returned&&!weapon_try_and_get(a)&&
     run(0)->gear.record.count==1&&!extraction_survival_settlement_pending(),"finite victory during activation lost or stranded loot");
    else CHECK(run(0)->phase==_extraction_lost&&weapon_try_and_get(a)&&
     !run(0)->gear.record.count,"defeat during activation banked loot");
   } else if(!strcmp(case_name,"evac-no-survivors")) {
    units[0].object.damage_flags=units[1].object.damage_flags=1;advance(1);
    CHECK(!extraction_survival_evacuation_ready()&&!firefight.victory&&
     !run(0)->gear.record.count&&!run(1)->gear.record.count,"zero living evacuees won or banked loot");
   } else {
    escape(0);
    CHECK(!weapon_try_and_get(a)&&run(0)->gear.record.count==1&&!coop_enemies_firefight_outcome(),
     "individual evacuation failed or ended another player's active run");
    advance(1);CHECK(run(0)->phase==_extraction_returned,"escaped player automatically redeployed owned gear");
    if(!strcmp(case_name,"evac-loop")) {
     byte id[GEAR_ID_BYTES];memcpy(id,run(0)->gear.record.items[0].id,GEAR_ID_BYTES);
     touch(0,0);a=units[0].unit.weapon_object_indices[0];
     CHECK(run(0)->phase==_extraction_deployed&&!run(0)->gear.record.count&&weapon_try_and_get(a),"escaped owner could not rejoin through hub deployment");
     escape(0);CHECK(run(0)->gear.record.count==1&&!memcmp(id,run(0)->gear.record.items[0].id,GEAR_ID_BYTES),"redeployment duplicated or changed recovered item identity");
     escape(1);
    } else if(!strcmp(case_name,"evac-pending")) {
     units[1].object.position.x=20.f;touch(1,1);advance(300);units[1].object.position.x=0.f;
     fail_write=TRUE;advance(1);
     CHECK(run(1)->phase==_extraction_ready&&!firefight.victory,"failed recovery write ended encounter");
     fail_write=FALSE;publish_failure=TRUE;advance(30);
     CHECK(run(1)->phase==_extraction_settling&&!firefight.victory,"uncertain banking ended encounter early");
     publish_failure=FALSE;advance(30);
     CHECK(run(1)->gear.record.count==1,"banking retry lost or duplicated escaped loot");
    } else if(!strcmp(case_name,"evac-presence")) {
     units[0].object.position.x=20.f;escape(1);
     CHECK(!firefight.victory&&!extraction_survival_evacuation_ready(),"evacuation ignored a living player's physical return");
     units[0].object.position.x=0.f;units[0].unit.player_index=NONE;
     CHECK(!extraction_survival_evacuation_ready(),"stale native ownership completed team evacuation");
     units[0].unit.player_index=player_handles[0];
     { short flags[]={_object_die_act_of_god_bit,_object_die_act_of_god_silent_bit,_object_die_act_of_god_no_statistics_bit};short i;
      for(i=0;i<3;i++) {
       units[0].object.damage_flags=FLAG(flags[i]);
       CHECK(!extraction_survival_evacuation_ready(),"pending native death completed the team's evacuation");
      }
      units[0].object.damage_flags=0;
     }
     advance(1);
    } else return 2;
    CHECK(firefight.phase==4&&firefight.victory&&coop_enemies_firefight_outcome()==1&&
     firefight.deadline==tick+5*TICKS_PER_SECOND,"all recovered living players did not reach the existing result/lobby path");
    touch(0,0);CHECK(run(0)->phase==_extraction_returned&&run(0)->gear.record.count==1,"result-screen deployment reserved gear before lobby return");
   }
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
 } else if(!strncmp(case_name,"account-host",12)) {
  byte expected[16],item[GEAR_ID_BYTES];
  delta_session=TRUE;
  players[0].network_player_data.machine_index=4;players[0].network_player_data.controller_index=3;
  if(!strcmp(case_name,"account-host-unbound")) {
   verified=FALSE;touch(0,0);
   CHECK(!file_count&&run(0)->phase==_extraction_hub,"unbound Delta host fell back to a local profile stash");
  } else {
   if(!strcmp(case_name,"account-host-role"))memcpy(host_account,remote_account,16);
   gear_account_owner(host_account,expected);touch(0,0);
   CHECK(run(0)->phase==_extraction_deployed&&!memcmp(run(0)->owner,expected,16),"Delta host used profile ownership instead of its verified account");
   if(!strcmp(case_name,"account-host-role")) {
    pickup(0,1);escape(0);CHECK(run(0)->gear.record.count==1,"host recovery missing");
    memcpy(item,run(0)->gear.record.items[0].id,GEAR_ID_BYTES);
    extraction_reset();players[0].local_player_index=NONE;players[0].network_player_data.controller_index=2;
    advance(1);touch(0,0);
    CHECK(run(0)->phase==_extraction_deployed&&!memcmp(run(0)->owner,expected,16),"joining changed the same verified account's owner: phase %d, owner mismatch %d, notice %s",run(0)->phase,memcmp(run(0)->owner,expected,16),last_remote_notice);
    CHECK(run(0)->gear.record.reserved_count==1&&!memcmp(run(0)->gear.record.reserved[0].id,item,GEAR_ID_BYTES),"host-to-client role change lost or duplicated banked equipment");
   }
  }
 } else if(!strcmp(case_name,"authority")) {
  client=TRUE; connection=_game_connection_network_client; advance(1); CHECK(!extraction_owns_map() && !file_count,"client storage access");
  client=FALSE; connection=_game_connection_network_server; profile=FALSE; verified=FALSE;
  touch(0,0); touch(0,1); CHECK(!file_count,"unidentified player opened storage");
  verified=TRUE; touch(0,1); CHECK(run(1)->phase==_extraction_deployed,"verified owner refused");
  remote_account[0]++; advance(1); CHECK(run(1)->phase==_extraction_lost,"changed identity retained run");
 } else if(!strcmp(case_name,"stash-discard")) {
  touch(0,0);pickup(0,1);pickup(0,2);escape(0);
  touch(0,1);pickup(1,1);escape(1);
  units[0].unit.control_flags=FLAG(_unit_control_crouch_modifier_bit);touch(2,0);
  CHECK(run(0)->discard_deadline && run(0)->gear.record.count==2,"first use discarded gear without a preview");
  touch(2,0);CHECK(run(0)->gear.record.count==2,"same-tick action confirmed discard");
  advance(30);units[0].unit.control_flags=0;touch(2,0);
  CHECK(!run(0)->discard_deadline && run(0)->gear.record.count==2,"standing did not cancel discard");
  units[0].unit.control_flags=1;touch(2,0);advance(30);touch(2,0);
  CHECK(run(0)->gear.record.count==1 && run(1)->gear.record.count==1,"discard changed another owner's stash");
  extraction_reset();advance(1);units[0].unit.control_flags=0;touch(0,0);
  CHECK(run(0)->gear.record.reserved_count==1 && weapon_get(units[0].unit.weapon_object_indices[0])->definition_index==2,
   "discarded gear returned or surviving gear could not redeploy");
 } else if(!strcmp(case_name,"discard-pending")) {
  unsigned long generation;
  touch(0,0);pickup(0,1);pickup(0,2);escape(0);units[0].unit.control_flags=1;touch(2,0);advance(30);
  fail_write=TRUE;touch(2,0);CHECK(run(0)->blocked && run(0)->gear.record.count==2,"failed discard lost gear");
  fail_write=FALSE;publish_failure=TRUE;touch(2,0);
  CHECK(run(0)->gear.pending && run(0)->gear.record.count==1,"published discard was forgotten");generation=run(0)->gear.record.generation;
  units[0].unit.control_flags=0;touch(2,0);
  CHECK(run(0)->discard_deadline && run(0)->gear.pending,"standing reported a published deletion as cancelled");
  publish_failure=FALSE;touch(2,0);
  CHECK(!run(0)->gear.pending && !run(0)->blocked && !run(0)->discard_deadline && run(0)->gear.record.count==1 &&
   run(0)->gear.record.generation==generation,"discard retry removed another item");
  units[0].unit.control_flags=1;touch(2,0);touch(2,0);
  CHECK(run(0)->gear.record.count==1,"duplicate interactions discarded another item without confirmation delay");
 } else if(!strcmp(case_name,"discard-identity")) {
  touch(0,1);pickup(1,1);pickup(1,2);escape(1);units[1].unit.control_flags=1;touch(2,1);
  CHECK(run(1)->discard_deadline,"remote preview missing");remote_account[0]++;advance(1);
  CHECK(!run(1)->discard_deadline,"changed account retained discard intent");touch(2,1);
  CHECK(!run(1)->discard_deadline && !run(1)->gear.record.count,"new account inherited old stash or confirmation");
  remote_account[0]--;touch(2,1);CHECK(run(1)->gear.record.count==2,"identity change removed original owner's gear");
  verified=FALSE;advance(1);touch(2,1);
  CHECK(!run(1)->discard_deadline && run(1)->gear.record.count==2,"unverified player discarded gear");verified=TRUE;
  touch(2,1);medical_setting=TRUE;extraction_medical_reset();units[1].object.body_vitality=-.1f;
  CHECK(extraction_medical_lethal(unit_handles[1]),"downed fixture failed");advance(1);touch(2,1);
  CHECK(!run(1)->discard_deadline && run(1)->gear.record.count==2,"incapacitated player discarded stored gear");
 } else if(!strcmp(case_name,"discard-state")) {
  touch(0,0);pickup(0,1);pickup(0,2);escape(0);units[0].unit.control_flags=1;touch(2,0);
  units[0].object.position.x=20.f;advance(1);CHECK(!run(0)->discard_deadline,"leaving hub retained confirmation");
  units[0].object.position.x=0.f;touch(2,0);advance(301);CHECK(!run(0)->discard_deadline,"confirmation did not expire");
  touch(2,0);unit_handles[0]+=0x10000;players[0].unit_index=unit_handles[0];advance(1);
  CHECK(!run(0)->discard_deadline,"new native body inherited discard intent");
  touch(2,0);CHECK(run(0)->gear.record.count==2,"body change deleted gear on preview");
  units[0].object.parent_object_index=123;advance(1);touch(2,0);
  CHECK(!run(0)->discard_deadline && run(0)->gear.record.count==2,"vehicle control authorized discard");
  units[0].object.parent_object_index=NONE;
 } else if(!strcmp(case_name,"loadout-choice")) {
  struct game_modes_inventory chosen;
  unsigned long generation;
  touch(0,0);pickup(0,1);pickup(0,2);escape(0);
  touch(2,0);generation=run(0)->gear.record.generation;publish_failure=TRUE;touch(2,0);
  CHECK(run(0)->gear.pending && run(0)->blocked && run(0)->gear.record.generation==generation+1,
   "terminal forgot an uncertain saved choice");
  publish_failure=FALSE;touch(2,0);
  CHECK(!run(0)->gear.pending && !run(0)->blocked && run(0)->gear.record.generation==generation+1,
   "terminal retry advanced the pair again or retained its error");
  extraction_reset();advance(1);touch(2,0);
  CHECK(gear_loadout(&run(0)->gear,&chosen) && chosen.weapon_count==2 &&
   !strcmp(chosen.weapons[0].tag_path,"weapons\\pistol\\pistol"),"reopened hub lost selected primary");
  touch(0,0);a=units[0].unit.weapon_object_indices[0];
  CHECK(weapon_get(a)->definition_index==2,"deployment ignored the saved primary");
  weapon_get(a)->weapon.magazines[0].rounds_loaded=3;escape(0);extraction_reset();advance(1);touch(0,0);
  CHECK(weapon_get(units[0].unit.weapon_object_indices[0])->definition_index==2 &&
   weapon_get(units[0].unit.weapon_object_indices[0])->weapon.magazines[0].rounds_loaded==3,
   "recovery/redeployment changed chosen primary or ammunition");
  escape(0);extraction_reset();enabled=FALSE;director=TRUE;
  firefight.enabled=firefight.gear=TRUE;firefight.phase=1;advance(1);
  CHECK(run(0)->phase==_extraction_deployed &&
   weapon_get(units[0].unit.weapon_object_indices[0])->definition_index==2 &&
   weapon_get(units[0].unit.weapon_object_indices[0])->weapon.magazines[0].rounds_loaded==3,
   "finite Survival did not reuse the same owner's saved Extraction choice");
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
 } else if(!strcmp(case_name,"grant-failure")||!strcmp(case_name,"shared-cache-failure")) {
  touch(0,0); pickup(0,1); pickup(0,2); escape(0);
  if(!strcmp(case_name,"shared-cache-failure")) {
   extraction_reset();map="rebuilt@ce";checksum++;shared_cache=TRUE;advance(1);
  }
  a=pickup(0,1); before=weapon_count;
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
