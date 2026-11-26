/* Real Extraction + shared gear + native inventory operations in one bounded
 * world. No native rendering, packets, identity proof or AI is simulated here. */
#define GEAR_FIXTURE_ONLY
#define EXTRACTION_FIXTURE
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
#define BIPED_DEFINITION_TAG 2
#define VEHICLE_DEFINITION_TAG 0x76656869
typedef unsigned short word;
typedef struct {real yaw,pitch,roll;} real_euler_angles3d;
typedef struct {real r,g,b;} real_rgb_color;
#define NETWORK_RULES_TIME_WINDOW_TICKS 450
#define csmemset memset
#define TAG_BLOCK_GET_ELEMENT(block,index,type) (&((type *)(block)->address)[index])
enum { _object_mask_biped=1, _object_mask_control=2, _object_mask_weapon=4, _object_mask_vehicle=8, _object_type_vehicle=1, _object_type_biped=0, _object_type_control=8,
 _object_mask_unit=_object_mask_biped|_object_mask_vehicle, _object_type_weapon=2,
 _object_dead_bit=0, _unit_grenade_throw_idle=0, _unit_add_weapon_starting=0,
 _object_die_act_of_god_bit=1,
 _object_die_act_of_god_silent_bit=2, _object_die_act_of_god_no_statistics_bit=3,
 _unit_control_crouch_modifier_bit=0, _unit_control_jump_bit=1, _unit_control_integrated_light_bit=4, _unit_control_action_bit=6,
 _unit_control_weapon_reload_bit=10, _unit_control_weapon_primary_trigger_bit=11,
 _unit_control_weapon_secondary_trigger_bit=12, _unit_control_throw_grenade_bit=13,
 _unit_control_use_equipment_bit=7, _unit_control_swap_weapons_bit=14, _unit_control_user_animation1_bit=2,
 _collision_test_structure_bit=0, _collision_test_objects_bit=1,
 _collision_test_front_facing_surfaces_bit=2, _collision_test_back_facing_surfaces_bit=3,
 _delta_capability_coop=6, _delta_capability_object_sync=17 };
struct tag_block { long count; void *address; };
struct tag_reference {long index;};
#include "body_types.inc"
#include "native_teams.inc"
#include "medical_hud_types.inc"
#include "equipment_ownership.inc"
struct object_fields { long first_child_object_index, next_object_index; short type; long parent_object_index, owner_player_index; unsigned long flags; short name_index; long damage_flags; real_point3d position;
 real_point3d bounding_sphere_center; real body_vitality, shield_vitality; short owner_team_index; real_vector3d forward,up,translational_velocity,angular_velocity;
 real_rgb_color base_change_colors[4], outgoing_change_colors[4]; };
struct object_datum { long definition_index; struct object_fields object; };
struct unit_animation { short state, seat_index; };
struct unit_datum { long definition_index; struct object_fields object;
 struct { long player_index, actor_index, equipment_object_index, weapon_object_indices[4], driver_object_index; short current_weapon_index, desired_weapon_index, parent_seat_index;
  signed char grenade_counts[2]; short grenade_throw_state; unsigned long control_flags,flags;
  struct unit_animation animation;
  real active_camouflage,active_camouflage_super_amount;long cause_for_camo_regrowth;
  real_vector3d throttle; } unit;
 struct { unsigned long flags; unsigned char on_ground_ticks; } vehicle; };
struct weapon_datum { long definition_index; struct object_fields object;
 struct { unsigned long flags; } item;
 struct { struct { short rounds_loaded, rounds_total; } magazines[2]; float age; } weapon; };
struct control_datum { long definition_index; struct object_fields object;
 struct {short position_group_index,power_group_index;unsigned short flags;} device; };
#define device_datum control_datum
struct device_group_datum {real actual_value;unsigned short flags;};
struct player_datum { long unit_index,team_index,dead_unit_index,action_result,action_object_index; short local_player_index; boolean quit_out_of_game;
 short powerup_durations[NUMBER_OF_PLAYER_POWERUPS];
 struct {short kills[4],deaths;} statistics;
 struct { short machine_index, controller_index; } network_player_data; };
struct weapon_definition { struct { unsigned long flags; struct tag_block magazines; } weapon; };
struct weapon_magazine_definition { short rounds_loaded_maximum, rounds_total_maximum, rounds_total_initial; };
struct game_globals_grenade { short maximum_count; };
struct game_globals_player_information { struct tag_reference player_unit; };
struct game_globals { struct tag_block grenades, player_information; };
struct control_definition { struct { short triggers_when,type;real call_value;
 struct {long index;} on_effect,off_effect,denied_effect; } control; };
struct scenario_object_name { char name[32]; short runtime_object_type,runtime_scenario_datum_index; };
struct unit_definition {struct {struct tag_block seats;} unit;struct {struct tag_reference model,animation_graph;} object;};
enum {_scenario_trigger_volume_type_oriented=1};
struct scenario_trigger_volume { char name[32]; short type; };
#include "mission_types.inc"
#include "profile_types.inc"
struct player_starting_location { real_point3d position; short structure_bsp_reference_index; };
struct scenario { short type; struct tag_block object_names, trigger_volumes, players, hs_scripts, starting_profiles, device_groups,bipeds,biped_palette; struct tag_reference hud_messages; struct tag_block controls, control_palette, structure_bsp_references, vehicles, vehicle_palette; };
struct object_placement_data { long definition_index,owner_player_index; short owner_team_index,variant_number; real_point3d position; real_vector3d forward,up,translational_velocity,angular_velocity; };
struct delta_peer_identity { byte account[16]; int bound; };
struct data_iterator { short next; long datum_index; };
struct player_action { unsigned long control_flags; real primary_trigger; struct {real i,j;} throttle;
 short desired_zoom_level, desired_weapon_index; };
struct collision_result { long object_index; };

static struct player_datum players[8];
static long player_handles[8] = { 0x10000, 0x10001, 0x10002, 0x10003, 0x10004, 0x10005, 0x10006, 0x10007 };
static long unit_handles[8] = { 0x1000a, 0x1000b, 0x1000c, 0x1000d, 0x1000e, 0x1000f, 0x10010, 0x10011 };
static struct unit_datum units[8];
static struct unit_datum model_bodies[4];
static long model_handles[4];
static boolean original_deleted[8];
static short body_creations,fail_body_create;
static long incompatible_body_weapon=NONE;
static struct unit_datum dropships[2];
static long dropship_handles[2]={0x10014,0x10015};
#include "transport_seat_flags.inc"
struct unit_seat {unsigned long flags; char label[32],marker_name[32];};
static struct unit_seat transport_seats[2];
static struct unit_definition dropship_tag={{{2,transport_seats}}},character_tag;
static boolean native_cinematic;
static long recorded_aircraft=NONE;
static long human_plane_aircraft=NONE;
static struct unit_definition *unit_definition_get(long index) { if(index==4||index==5)return &character_tag;CHECK(index==300,"native vehicle definition");return &dropship_tag; }
static short active_players=2;
static struct { long handle; struct weapon_datum datum; boolean flag; } weapons[96];
static short weapon_count, fail_allocate, fail_attach;
static boolean fail_refund, fail_confirm, publish_confirm;
static struct control_datum controls[7];
static long control_handles[7] = { 0x10003, 0x10004, 0x10005, 0x10006, 0x10007, 0x10008, 0x10009 };
static struct control_definition control_tag;
static struct device_group_datum device_groups[12];
static struct data_array device_group_table;
static struct data_array *device_groups_data=&device_group_table;
static int control_updates[7],control_effects[7];
#include "controls_types.inc"
static struct weapon_magazine_definition magazine = { 20, 200, 0 };
static struct weapon_definition weapon_tag = { { 0, { 1, &magazine } } };
static long missing_tag = NONE, unusable_tag = NONE;
static struct game_globals_grenade grenades[2] = { {4}, {4} };
static struct game_globals_player_information native_player_body = { {4} };
static struct game_globals globals = { { 2, grenades }, { 1, &native_player_body } };
static struct scenario_object_name names[11] = { {"extraction_deploy"}, {"extraction_activate"}, {"extraction_loadout"},
 {"extraction_pelican",_object_type_vehicle},{"extraction_spirit",_object_type_vehicle},
 {"extraction_unsc"},{"extraction_covenant"}, {"extraction_unsc_body",_object_type_biped,0},{"extraction_covenant_body",_object_type_biped,1} };
static struct scenario_starting_profile faction_profiles[2];
static struct scenario_object_datum body_placements[2];
static struct scenario_object_palette_entry body_palette[2];
struct scenario_device_datum { short power_group_index, position_group_index; };
struct scenario_control_datum { struct scenario_object_datum object; struct scenario_device_datum device; };
static struct scenario_control_datum placed_controls[7];
static struct scenario_device_datum const *control_scenario_device_data(struct scenario_control_datum const *control)
{ return &control->device; }
static struct scenario_object_palette_entry control_palette[1];
#define CONTROL_DEFINITION_TAG 0x6374726c
static struct scenario_trigger_volume volumes[7] = { {"extraction_zone"}, {"extraction_return"} };
static boolean overlap_hub_starts;
static struct player_starting_location starts[3];
static boolean scenario_uses_regional_gameplay(struct scenario const *scenario)
{
 return scenario->structure_bsp_references.count > 1;
}
static struct scenario scenario = { _scenario_type_solo, {3,names}, {2,volumes}, {2,starts}, {0,NULL} };
static struct hs_script completion_script;
static struct hs_script transport_scripts[3];
static short completion_wakes, transport_wakes[2], transport_aborts[2];
static struct hs_script squad_scripts[6];
static short squad_wakes[2], squad_aborts[2], squad_begins[2];
static boolean hs_wake_by_name(char const *name)
{
 if(!strcmp(name,"forerunner_pelican_complete"))transport_wakes[0]++;
 else if(!strcmp(name,"forerunner_spirit_complete"))transport_wakes[1]++;
 else if(!strcmp(name,"forerunner_pelican_aborted"))transport_aborts[0]++;
 else if(!strcmp(name,"forerunner_spirit_aborted"))transport_aborts[1]++;
 else if(!strcmp(name,"forerunner_squad_01_complete"))squad_wakes[0]++;
 else if(!strcmp(name,"forerunner_squad_02_complete"))squad_wakes[1]++;
 else if(!strcmp(name,"forerunner_squad_01_aborted"))squad_aborts[0]++;
 else if(!strcmp(name,"forerunner_squad_02_aborted"))squad_aborts[1]++;
 else if(!strcmp(name,"forerunner_squad_01_begin"))squad_begins[0]++;
 else if(!strcmp(name,"forerunner_squad_02_begin"))squad_begins[1]++;
 else {CHECK(!strcmp(name,"forerunner_extraction_complete"),"wrong mission continuation");completion_wakes++;}
 return TRUE;
}
static struct scenario *global_scenario = &scenario;
static struct scenario *global_scenario_get(void) {return global_scenario;}
static struct tag_block *scenario_get_object_type_scenario_datums(struct scenario *s,short type,long *size)
{if(type==_object_type_control){*size=sizeof(struct scenario_control_datum);return &s->controls;}
 *size=sizeof(struct scenario_object_datum);
 if(type==_object_type_vehicle)return &s->vehicles;
 CHECK(type==_object_type_biped,"unexpected template type");return &s->bipeds;}
static struct tag_block *scenario_get_object_type_scenario_palette(struct scenario *s,short type)
{if(type==_object_type_control)return &s->control_palette;
 if(type==_object_type_vehicle)return &s->vehicle_palette;
 CHECK(type==_object_type_biped,"unexpected template palette");return &s->biped_palette;}
static void *tag_block_get_element_with_size(struct tag_block *block,long index,long size)
{CHECK(VALID_INDEX(index,block->count),"template outside native block");return (byte *)block->address+index*size;}
#include "device_binding.inc"
static struct scenario_device_group authored_groups[12];
static short boarding_changes;
static real device_group_get_value(short index) {CHECK(index>=0&&index<12,"invalid native group read");return device_groups[index].actual_value;}
static void device_group_set_actual_value(short index,real value)
{CHECK(index>=0&&index<12,"invalid native group output");device_groups[index].actual_value=value;boarding_changes++;}
static boolean extra_grenades;
static boolean mcc_grenades_active(void) {return extra_grenades;}
static short mcc_grenades_type_count(void) {return extra_grenades?4:2;}
static int team_updates,actor_team_updates;
static void actor_set_team(long actor,short team) {CHECK(actor==7,"unexpected faction actor");actor_team_updates++;}
static void ai_update_team_status(void) {team_updates++;}
static void *player_data;
static long tick;
static boolean enabled=TRUE, director, profile=TRUE, verified=TRUE;
static boolean medical_setting, medical_support=TRUE, medical_object_sync=TRUE, wall;
static unsigned int epoch=1;
static int native_kills;
void platform_log(char const *format, ...) { (void)format; }
static byte remote_account[16] = {1};
static byte host_account[16] = {2};
static boolean delta_session, online=TRUE;
static unsigned long guest_address=0x7f000002UL;
static unsigned long network_game_server_machine_address(long machine)
{ CHECK(machine==1,"wrong LAN peer"); return guest_address; }
static int log_address_local(byte const *address,int size)
{ CHECK(size==4,"unexpected IP size"); return address[0]==127||address[0]==100; }
static int local_notices, remote_notices, respawns;
static char local_notice_history[4][160];
static char last_remote_notice[160],remote_notice_history[4][160];

static boolean config_boolean(char const *name)
{ CHECK(strcmp(name,"debug.network_test"),"network-test string queried as boolean"); return !strcmp(name,"game.extraction") ? enabled : !strcmp(name,"game.revive") ? medical_setting :
  !strcmp(name,"game.shared_gear") ? shared_cache : !strcmp(name,"network.online") ? online : director; }
static long config_integer(char const *name) { return !strcmp(name,"game.revive_seconds") ? 5 : 30; }
static char const *config_string(char const *name) { CHECK(!strcmp(name,"debug.network_test"),"unknown string setting"); return ""; }
static boolean network_coop_active(void) { return TRUE; }
static struct { boolean enabled, gear, victory, resupply; short phase,lives; long wave,wave_limit,deadline,wipe_deadline;char outcome[128];
 long respawn_players[HALO_PORT_MAXIMUM_NETWORK_PLAYERS]; } firefight;
#include "survival.inc"
static long game_time_get(void) { return tick; }
static struct player_datum *player_try_and_get(long handle)
{ short i; for(i=0;i<active_players;i++) if(player_handles[i]==handle) return &players[i]; return NULL; }
static struct player_datum *player_get(long h) { struct player_datum *p=player_try_and_get(h); CHECK(p,"invalid player"); return p; }
static struct update_client_queue_datum { unsigned long latched_control_flags; } input_queues[8];
static struct { boolean initialized; void *queues; } update_client_globals={TRUE,input_queues};
static void *input_queue_get(void *data,long handle)
{ short i;CHECK(data==input_queues,"wrong native queue");for(i=0;i<active_players;i++)if(player_handles[i]==handle)return &input_queues[i];return NULL; }
#define datum_try_and_get(data,index) input_queue_get(data,index)
#include "reload_queue.inc"
#undef datum_try_and_get
static void *object_try_and_get_and_verify_type(long h,long mask)
{
 short i;
 if(h==NONE) return NULL;
 for(i=0;i<2;i++) if(h==dropship_handles[i] && (mask&_object_mask_vehicle)) return &dropships[i];
 for(i=0;i<active_players;i++) if(h==unit_handles[i] && !original_deleted[i] && (mask&_object_mask_biped)) return &units[i];
 for(i=0;i<4;i++) if(model_handles[i]!=NONE && h==model_handles[i] && (mask&_object_mask_biped)) return &model_bodies[i];
 for(i=0;i<NUMBEROF(controls);i++) if(h==control_handles[i] && (mask&_object_mask_control)) return &controls[i];
 for(i=0;i<weapon_count;i++) if(h==weapons[i].handle && (mask&_object_mask_weapon)) return &weapons[i].datum;
 return NULL;
}
static struct unit_datum *unit_get(long h) { void *p=object_try_and_get_and_verify_type(h,_object_mask_biped|_object_mask_vehicle); CHECK(p,"invalid unit"); return p; }
static struct weapon_datum *weapon_try_and_get(long h) { return object_try_and_get_and_verify_type(h,_object_mask_weapon); }
static struct weapon_datum *weapon_get(long h) { struct weapon_datum *p=weapon_try_and_get(h); CHECK(p,"invalid weapon"); return p; }
static boolean cinematic_in_progress(void) { return native_cinematic; }
static boolean recorded_animation_has_thread(long vehicle_index) { return vehicle_index==recorded_aircraft; }
boolean vehicle_is_human_plane(long vehicle_index) { return vehicle_index==human_plane_aircraft; }
boolean vehicle_is_flipped(long vehicle_index) { return unit_get(vehicle_index)->object.up.k<.2f; }
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
static char const *tag_get_name(long tag) { return tag==90 ? "vehicles\\pelican" : tag==99 ? "devices\\terminal" : tag==4 ? "characters\\human" : tag==5 ? "characters\\covenant" : tag==1 ? "weapons\\rifle\\rifle" : "weapons\\pistol\\pistol"; }
static long tag_loaded(long group,char const *path)
{ long tag=group==CONTROL_DEFINITION_TAG ? (!strcmp(path,tag_get_name(99))?99:NONE) :
  group==BIPED_DEFINITION_TAG ? (!strcmp(path,tag_get_name(4))?4:!strcmp(path,tag_get_name(5))?5:NONE) :
  !strcmp(path,tag_get_name(1)) ? 1 : !strcmp(path,tag_get_name(2)) ? 2 : NONE;
 if(group==VEHICLE_DEFINITION_TAG)tag=!strcmp(path,"vehicles\\pelican")?90:NONE;
 return tag==missing_tag ? NONE : tag; }
static struct weapon_definition *weapon_definition_get(long tag) { return &weapon_tag; }
static struct game_globals *scenario_get_game_globals(void) { return &globals; }
static void object_placement_data_new(struct object_placement_data *p,long tag,long owner) { memset(p,0,sizeof(*p)); p->definition_index=tag; }
static long object_new(struct object_placement_data const *p)
{
 short i;
 if(p->definition_index==4 || p->definition_index==5) {
  short j;struct unit_datum *u;
  if(fail_body_create && !--fail_body_create)return NONE;
  for(i=0;i<4;i++)if(model_handles[i]==NONE)break;
  CHECK(i<4,"body pool full");u=&model_bodies[i];memset(u,0,sizeof(*u));
  model_handles[i]=0x50060+(++body_creations)*0x10000+i;
  u->definition_index=p->definition_index;u->object.position=p->position;u->object.bounding_sphere_center=p->position;
  u->object.forward=p->forward;u->object.up=p->up;u->object.parent_object_index=NONE;
  u->object.owner_player_index=p->owner_player_index;u->object.owner_team_index=p->owner_team_index;
  u->object.body_vitality=u->object.shield_vitality=1;
  u->unit.player_index=u->unit.actor_index=u->unit.equipment_object_index=NONE;
  u->unit.current_weapon_index=u->unit.desired_weapon_index=u->unit.parent_seat_index=NONE;
  for(j=0;j<4;j++)u->unit.weapon_object_indices[j]=NONE;
  return model_handles[i];
 }
 if(fail_allocate && !--fail_allocate) return NONE;
 i=weapon_count++; CHECK(i<96,"world full"); weapons[i].handle=0x10020+i;
 weapons[i].datum.definition_index=p->definition_index; weapons[i].datum.object.parent_object_index=NONE;weapons[i].datum.object.type=_object_type_weapon;
 weapons[i].datum.object.position=p->position; return weapons[i].handle;
}
static void object_delete(long h)
{ short i,j;for(i=0;i<4;i++)if(model_handles[i]!=NONE&&model_handles[i]==h) {
   for(j=0;j<4;j++)CHECK(model_bodies[i].unit.weapon_object_indices[j]==NONE,"body deletion erased an original weapon");model_handles[i]=NONE;return;
  }
 for(i=0;i<active_players;i++)if(unit_handles[i]==h) {for(j=0;j<4;j++)CHECK(units[i].unit.weapon_object_indices[j]==NONE,"old body erased gear");original_deleted[i]=TRUE;return;}
 for(i=0;i<weapon_count;i++) if(weapons[i].handle==h) { weapons[i].handle=NONE; return; } }
static boolean unit_can_use_weapon(long u,long w)
{ return weapon_get(w)->definition_index!=unusable_tag && !(unit_get(u)->definition_index==5 && weapon_get(w)->definition_index==incompatible_body_weapon); }
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
static void unit_delete_all_weapons(long u)
{short i;struct unit_datum *unit=unit_get(u);for(i=0;i<4;i++)if(unit->unit.weapon_object_indices[i]!=NONE) {
 long w=unit->unit.weapon_object_indices[i];CHECK(unit_drop_weapon(u,w),"default weapon drop failed");object_delete(w);}}
static void item_in_unit_inventory(long w,long u)
{struct weapon_datum *item=weapon_get(w);item->object.owner_player_index=unit_get(u)->unit.player_index;
 item->item.flags=FLAG(_item_attached_to_unit_bit)|(unit_get(u)->unit.player_index!=NONE?FLAG(_item_belongs_to_player_bit):0);}
static real_rgb_color *game_engine_player_get_change_color(real_rgb_color *color,long player)
{*color=player==player_handles[0]?(real_rgb_color){.8f,.2f,.1f}:(real_rgb_color){.1f,.6f,.9f};return color;}
static void placement_data_set_change_color(struct object_placement_data *placement,real_rgb_color const *color)
{(void)placement;(void)color;}
static boolean game_engine_has_teams(void) {return FALSE;}
static void infection_reconcile_player(long player) {(void)player;}
static void unit_set_actively_controlled(long unit,boolean enabled) {(void)enabled;unit_get(unit);}
static void player_control_new_unit(short local,long unit) {(void)local;(void)unit;}
static void observer_obsolete_position(short local) {(void)local;}
static long local_player_get_player_index(short local);
static long player_control_get_unit_index(short local) {return player_get(local_player_get_player_index(local))->unit_index;}
#define datum_try_and_get(data,index) player_try_and_get(index)
#include "body_binding.inc"
#undef datum_try_and_get
static short local_views=1;
static short local_player_get_next(short index) { return index+1<local_views ? index+1 : NONE; }
static short native_hud_notices;
static boolean notice_chat_visible=TRUE;
static boolean network_chat_notice_visible(void){return notice_chat_visible;}
static void hud_print_message(short local,wchar_t const *text) {
 native_hud_notices++;
 short i;CHECK(local>=0&&local<local_views,"wrong local notice");CHECK(wcslen(text)<63,"native HUD record truncated a notice");
 for(i=0;text[i];i++)local_notice_history[local_notices%4][i]=(char)text[i];
 local_notice_history[local_notices%4][i]=0;local_notices++;
}
short distributed_client_machines(long *machines,short max) { machines[0]=1; return 1; }
/* Native distributed_host_tick maps remote clients only, never the host. */
long distributed_player_machine(short slot) { CHECK(slot>=0 && slot<active_players,"unsalted absolute player lookup"); return players[slot].local_player_index==NONE ? 1 : NONE; }
void distributed_send_notice_to_machine(long machine,char const *text) {
 if(machine==NONE) {
  CHECK(strlen(text)<160,"host notice exceeds existing packet");
  snprintf(local_notice_history[local_notices++%4],sizeof(local_notice_history[0]),"%s",text);return;
 }
 CHECK(machine==1,"wrong remote notice");
 CHECK(strlen(text)<160,"client notice exceeds existing packet");
 snprintf(remote_notice_history[remote_notices%4],sizeof(remote_notice_history[0]),"%s",text);
 remote_notices++;snprintf(last_remote_notice,sizeof(last_remote_notice),"%s",text);
}
static long player_ui_get_active_player_profile_index(short local) { return profile ? 0 : NONE; }
static boolean player_ui_get_path_to_local_player_profile_directory(short local,char *path) { strcpy(path,"owned/profile"); return TRUE; }
static boolean delta_peer_game_player_identity(int machine,int controller,struct delta_peer_identity *id)
{ CHECK((machine==1 && controller==2)||(delta_session && machine==players[0].network_player_data.machine_index && controller==players[0].network_player_data.controller_index),"wrong authenticated controller"); memcpy(id->account,machine==1?remote_account:host_account,16); id->bound=verified; return TRUE; }
static boolean delta_peer_game_on_delta(void) { return delta_session; }
#include "vehicle_landing_types.inc"
static real magnitude_squared3d(real_vector3d const *v) { return v->i*v->i+v->j*v->j+v->k*v->k; }
#define vehicle_datum unit_datum
#include "vehicle_landing.inc"
#undef vehicle_datum
static long object_index_from_name_index(short index) { return index>=9 && index<11 ? control_handles[index-4] : index>=5 && index<7 ? control_handles[index-2] : index>=3 && index<5 ? dropship_handles[index-3] : index>=0 && index<3 ? control_handles[index] : NONE; }
static struct control_datum *control_try_and_get(long h) { return object_try_and_get_and_verify_type(h,_object_mask_control); }
static struct control_definition *control_definition_get(long h) { return &control_tag; }
static struct control_datum *control_get(long h) { struct control_datum *c=control_try_and_get(h);CHECK(c,"invalid control");return c; }
static struct device_datum *device_get(long h) { return control_get(h); }
static boolean device_group_set_desired_value(short index,real value)
{
 CHECK((index>=0&&index<3)||index==6||index==7||index==10,"invalid position group");
 if((device_groups[index].flags&FLAG(_device_group_can_change_only_once_bit))&&
    (device_groups[index].flags&FLAG(_device_group_changed_once_bit)))return FALSE;
 device_groups[index].actual_value=value;device_groups[index].flags|=FLAG(_device_group_changed_once_bit);control_updates[index==10?5:index<3?index:index-3]++;return TRUE;
}
static void device_effect_new(long control,long effect) { for(short i=0;i<NUMBEROF(controls);i++)if(control_handles[i]==control){control_effects[i]=(int)effect;return;}CHECK(FALSE,"invalid effect owner"); }
static void __attribute__((noreturn)) display_assert(void *unused,char const *path,int line,boolean fatal) { (void)unused;(void)path;(void)line;(void)fatal;abort(); }
static void __attribute__((noreturn)) system_exit(long code) { (void)code;abort(); }
boolean device_can_change_position(long control_index);
static boolean scenario_trigger_volume_test_point(short zone,real_point3d const *p)
{
 if(zone==2)return p->x>10.f&&p->x<25.f;
 if(zone==3)return p->x>25.f;
 if(zone==4)return p->x>6.f&&p->x<9.f;
 if(zone==5)return p->z>0.f;
 if(zone==6)return p->z<0.f||overlap_hub_starts;
 return zone==0 ? p->x>10.f : p->x<5.f;
}
static boolean scenario_trigger_volume_test_object(short zone,long h) { return scenario_trigger_volume_test_point(zone,&unit_get(h)->object.position); }
static boolean players_are_all_dead(void) { return (units[0].object.damage_flags&1) && (units[1].object.damage_flags&1); }
static void players_respawn_at_start(void) { respawns++; }
static void data_iterator_new(struct data_iterator *i,void *data) { i->next=0; }
static void *data_iterator_next(struct data_iterator *i) { if(i->next>=active_players) return NULL; i->datum_index=player_handles[i->next]; return &players[i->next++]; }
static void firefight_notice(char const *text);
#include "survival_lives.inc"

#include "equipment_flags.inc"
enum {_object_change_color_a=0, NUMBER_OF_OBJECT_CHANGE_COLORS=4};
struct object_change_color_definition {short scaled_by, darken_by;struct tag_block permutations;};
struct object_definition {struct {struct tag_block change_colors;} object;};
static struct object_change_color_definition armor_channels[2]={{0,0,{1,NULL}},{0,0,{1,NULL}}};
static struct object_definition armor_model={{{2,armor_channels}}};
static struct object_definition *object_definition_get(long definition){return &armor_model;}
static struct object_datum *object_get(long index);
#include "body_armor_color.inc"
struct structure_bsp;
static short object_get_regional_bsp(long object);
static struct structure_bsp const *scenario_structure_bsp_try_and_get(short bsp);
static short global_structure_bsp_index_get(void) {return 0;}
static long object_new_in_bsp(struct object_placement_data *placement,short bsp,struct structure_bsp const *structure);
static void object_activate(long object);
#include "equipment.inc"
/* Native object-list adapters; list allocation/provenance itself is exercised
 * with the real pool code in coop_gate and script_stale_objects. */
static boolean authored_player_actions=TRUE,list_used[8],list_players[8];
static short list_sizes[8],list_failure_at;
static long list_units[8][6];
static boolean list_allocation_failure;
static boolean coop_scripts_native_player_actions(void){return authored_player_actions;}
static long object_list_new(void){if(list_allocation_failure)return NONE;for(short i=0;i<8;i++)if(!list_used[i]){list_used[i]=TRUE;list_sizes[i]=0;list_players[i]=FALSE;return i;}return NONE;}
static boolean object_list_add(long l,long u){if(l==NONE||list_sizes[l]==6||list_sizes[l]+1==list_failure_at)return FALSE;list_units[l][list_sizes[l]++]=u;return TRUE;}
static void object_list_mark_players(long l){if(l!=NONE)list_players[l]=TRUE;}
static void object_list_delete(long l){if(l!=NONE)list_used[l]=FALSE;}
static long object_list_get_first(long l,long *r){*r=0;return list_sizes[l]?list_units[l][0]:NONE;}
static long object_list_get_next(long l,long *r){return ++*r<list_sizes[l]?list_units[l][*r]:NONE;}
struct tag_data {long size;void *address;};
#include "notice_hud_types.inc"
static wchar_t authored_notice[256]=L"Squad 01: recover technology.";
static struct hud_state_message_element notice_element={_hud_message_type_text,29};
static struct hud_state_message_definition notice_message={.element_count=1};
static struct hud_message_text_definition notice_text;
#define HUD_MESSAGE_TEXT_DEFINITION_GET(index) (&notice_text)
static void *tag_data_get_pointer(struct tag_data *d,long offset,long size){return (byte *)d->address+offset;}
#define SOUND_DEFINITION_TAG 0x736e6421UL
static int private_radio_calls,private_radio_played,private_radio_remote;
static boolean radio_queue_full;
static boolean distributed_tag_of_group(long tag,unsigned long group){return group==SOUND_DEFINITION_TAG&&tag==91;}
static boolean network_coop_private_sound(long machine,long sound){CHECK(machine==1&&sound==91,"wrong private radio");if(radio_queue_full)return FALSE;private_radio_remote++;return TRUE;}
static void scripted_sound_new_private(long sound,boolean play){CHECK(sound==91,"wrong native radio definition");private_radio_calls++;private_radio_played+=play;}
#include "notice_hud.inc"
#include "native_unit_state.inc"
#include "native_seat_busy.inc"
static char const *coop_enemies_authored_round(void) { return NULL; }
static short scenario_object_name_index_from_string(struct scenario *map, char const *name)
{
 short index;
 struct scenario_object_name const *entries=map->object_names.address;
 for(index=0;index<map->object_names.count;index++)
  if(!strcmp(entries[index].name,name))return index;
 return NONE;
}
enum {MAXIMUM_STRUCTURE_BSPS_PER_SCENARIO = 16};
struct structure_bsp {short bsp;};
static struct structure_bsp fixture_hub_geometry={10};
static boolean body_region_loaded;
static short body_created_bsp=NONE, weapon_created_bsp=NONE;
static long activated_body=NONE;
static struct structure_bsp const *scenario_structure_bsp_try_and_get(short bsp)
{return body_region_loaded&&bsp==fixture_hub_geometry.bsp?&fixture_hub_geometry:NULL;}
static long object_new_in_bsp(struct object_placement_data *placement,short bsp,struct structure_bsp const *structure)
{
 CHECK(structure ? bsp==structure->bsp : bsp==NONE,"object placement got the wrong region geometry");
 if(placement->definition_index==4||placement->definition_index==5)body_created_bsp=bsp;
 else weapon_created_bsp=bsp;
 return object_new(placement);
}
static void object_activate(long object){activated_body=object;}
static struct structure_bsp const *scenario_structure_bsp_preview(short bsp, boolean prefetch)
{return prefetch&&bsp==fixture_hub_geometry.bsp?&fixture_hub_geometry:NULL;}
static boolean scenario_structure_bsp_contains_placement(struct structure_bsp const *bsp,real_point3d const *position)
{return bsp->bsp==10&&position->z==-3.f;}
#include "extraction.inc"
#define vehicle_datum unit_datum
#include "vehicle_brake.inc"
#undef vehicle_datum
static struct object_datum *object_get(long index) {struct object_datum *o=object_try_and_get_and_verify_type(index,_object_mask_unit|_object_mask_weapon|_object_mask_control);CHECK(o,"invalid object");return o;}
static struct unit_datum *unit_try_and_get(long index) {return object_try_and_get_and_verify_type(index,_object_mask_unit);}
static void error(short priority,char const *text,...) {CHECK(FALSE,"native starting equipment failed");}
static void mcc_player_profile_grenades(long unit,struct scenario_starting_profile *profile,boolean reset) {}
#include "spawn_equipment_types.inc"
#include "spawn_equipment.inc"
#include "native_seat_animation_types.inc"
#define TAG_STRING_LENGTH 32
#define csstrncpy strncpy
#define csstrcat strcat
typedef struct {real_point3d position;} real_matrix4x3;
struct object_marker {real_matrix4x3 matrix;};
struct model_node {real_point3d default_translation;};
struct model {struct tag_block nodes;};
struct animation {short frame_count;};
struct animation_graph_animation_index {short animation_index;};
struct animation_graph_unit_seat {char label[32];struct tag_block animations;};
struct animation_graph {struct tag_block unit_seats,animations;};
static struct unit_seat native_seat;
static struct animation_graph_unit_seat native_animation_seat;
static struct animation_graph_animation_index native_animation_indices[12];
static struct animation native_animations[2];
static struct animation_graph native_animation_graph;
static struct model_node native_root_node={{0,0,.15f}};
static struct model native_model={{1,&native_root_node}};
static short observed_root_frame=NONE;
static boolean native_seat_marker=TRUE;
static struct model *model_definition_get(long index) {(void)index;return &native_model;}
static struct animation_graph *animation_graph_definition_get(long index) {(void)index;return &native_animation_graph;}
static struct animation_graph_animation_index *animation_graph_animation_index_get(struct tag_block *block) {return block->address;}
static long object_get_marker_by_name(long index,char const *name,struct object_marker *marker,short count)
{(void)index;(void)name;(void)count;marker->matrix.position=(real_point3d){10,20,30};return native_seat_marker?1:0;}
static void animation_get_root_matrix(struct model *model,struct animation *animation,short frame,real_matrix4x3 *root)
{(void)model;CHECK(frame>=0&&frame<animation->frame_count,"root frame outside native clip");observed_root_frame=frame;root->position=(real_point3d){(real)frame,0,0};}
static void matrix4x3_multiply(real_matrix4x3 const *a,real_matrix4x3 const *b,real_matrix4x3 *out)
{out->position=(real_point3d){a->position.x+b->position.x,a->position.y+b->position.y,a->position.z+b->position.z};}
static boolean exit_unarmed_supported=TRUE,exit_stand_supported=TRUE,exit_ground_pose;
static short exit_pose_fallbacks,exit_pose_queries,exit_ready_calls;
static short unit_weapon_next_index(long u,short current,short delta){for(short i=0;i<4;i++)if(unit_get(u)->unit.weapon_object_indices[i]!=NONE)return i;return NONE;}
static void unit_ready_desired_weapon(long u,boolean immediate){CHECK(immediate,"seat weapon not readied immediately");unit_get(u)->unit.current_weapon_index=unit_get(u)->unit.desired_weapon_index;exit_ready_calls++;}
static long unit_get_current_weapon_index(long u){short i=unit_get(u)->unit.current_weapon_index;return i==NONE?NONE:unit_get(u)->unit.weapon_object_indices[i];}
static boolean unit_set_or_test_seat_and_weapon_label(long u,char const *seat,char const *weapon,boolean change){
 CHECK(!strcmp(seat,"stand"),"exit selected a vehicle/foreign animation seat");
 if(weapon){CHECK(!strcmp(weapon,"unarmed")&&!change,"exit mutated a supported unarmed binding");exit_pose_queries++;return exit_unarmed_supported;}
 CHECK(change,"exit fallback was not applied");exit_pose_fallbacks++;exit_ground_pose=exit_stand_supported;return exit_stand_supported;
}
static void unit_exit_seat_end(long u){unit_get(u)->object.parent_object_index=NONE;}
static void unit_open(long u){}
static short animation_choose_random_permutation(long graph,short index){return index;}
static void unit_set_animation(long u,long graph,short index){CHECK(index==1,"wrong native exit animation");}
static void object_set_visibility(long u,boolean visible){}
static void ai_handle_exit_vehicle(long u,long parent){}
static long parked_region_vehicle = NONE;
static boolean game_engine_object_parked_for_unit(long index,short type,long unit)
{(void)unit;return index==parked_region_vehicle&&type==_object_type_vehicle;}
static void matrix4x3_transform_point(real_matrix4x3 const *matrix,real_point3d const *point,real_point3d *out)
{*out=(real_point3d){matrix->position.x+point->x,matrix->position.y+point->y,matrix->position.z+point->z};}
static void object_get_origin(long unit,real_point3d *origin) {*origin=unit_get(unit)->object.position;}
static void biped_get_physics_pill(long unit, real_point3d *base, real *height, real *width)
{object_get_origin(unit,base);base->z+=.25f;*height=.5f;*width=.25f;}
static boolean biped_fix_position(long unit,long ship,real_point3d const *position,real_point3d *out,real radius,
 boolean below,boolean dont_teleport,boolean scale)
{CHECK(dont_teleport && out,"hover clearance must supply a probe result without moving the passenger");
 CHECK(position->x==13.f && fabsf(position->z-30.1f)<.001f,"hover clearance did not use the native clip and biped capsule base");
 return !wall;}
enum { _biped_spherical_bit, _biped_pill_centered_at_origin_bit };
struct biped_definition { struct {unsigned long flags; real collision_radius, collision_height_standing;} biped; };
static struct biped_definition fixture_elite = {{0, .35f, 1.4f}};
static struct biped_definition *biped_definition_get(long tag)
{CHECK(tag==5,"clearance used a different faction body");return &fixture_elite;}
#ifndef MAX
#define MAX(a,b) ((a)>(b)?(a):(b))
#endif

static short fixture_unit_bsp = NONE, fixture_control_bsp = NONE;
static short object_get_regional_bsp(long object)
{
 for(short i=0;i<NUMBEROF(control_handles);i++)
  if(control_handles[i]!=NONE&&control_handles[i]==object)return fixture_control_bsp;
 return fixture_unit_bsp;
}
#include "spawn_clearance.inc"
#include "native_seats.inc"
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

#include "survival_notices.inc"
#include "resupply.inc"
#include "evacuation.inc"
static void advance(short count) { while(count--) { tick++; extraction_update();firefight_try_evacuation(tick); } }
static void touch(short control,short player) { control_touched(control_handles[control],players[player].unit_index); }
static struct extraction_run *run(short player) { return &extraction_runs[player]; }
static long pickup(short player,long tag)
{
 struct object_placement_data p; struct weapon_datum *weapon; long h;
 object_placement_data_new(&p,tag,NONE); h=object_new(&p); CHECK(h!=NONE,"pickup allocation failed");
 weapon=weapon_get(h); weapon->weapon.age=.25f; weapon->weapon.magazines[0].rounds_loaded=7;
 weapon->weapon.magazines[0].rounds_total=63;
 CHECK(unit_add_weapon_to_inventory(players[player].unit_index,h,0),"pickup failed"); return h;
}
static void escape(short player)
{
 unit_get(players[player].unit_index)->object.position.x=20.f; touch(1,player); advance(300);
 CHECK(run(player)->phase==_extraction_ready,"signal not ready");
 advance(900); CHECK(run(player)->phase==_extraction_ready,"mandatory raid timer expired");
 unit_get(players[player].unit_index)->object.position.x=0.f; advance(1);
 CHECK(run(player)->phase==_extraction_returned,"escape did not settle");
}
static void initialize(void)
{
 short i,j; connection=_game_connection_network_server;firefight.wave_limit=3;
 for(i=0;i<4;i++)model_handles[i]=NONE;
 device_group_table.data=(byte *)device_groups;device_group_table.size=sizeof(device_groups[0]);device_group_table.maximum_count=NUMBEROF(device_groups);
 for(i=0;i<2;i++) {
  players[i].unit_index=unit_handles[i]; players[i].local_player_index=i ? NONE : 0;
  units[i].definition_index=4;units[i].unit.equipment_object_index=NONE;
  units[i].object.forward.i=1;units[i].object.up.k=1;
  players[i].network_player_data.controller_index=i ? 2 : 0; units[i].unit.player_index=player_handles[i];
  units[i].object.body_vitality=units[i].object.shield_vitality=1.f;
  units[i].object.parent_object_index=NONE;
  units[i].unit.actor_index=NONE;
  units[i].unit.current_weapon_index=0; units[i].unit.grenade_counts[0]=1;
  for(j=0;j<4;j++) units[i].unit.weapon_object_indices[j]=NONE;
 }
 for(i=0;i<3;i++) {controls[i].object.name_index=i;controls[i].device.position_group_index=i;controls[i].device.power_group_index=i+3;device_groups[i+3].actual_value=1.f;}
 for(i=3;i<5;i++) {controls[i].object.name_index=i+2;controls[i].device.position_group_index=i+3;controls[i].device.power_group_index=i+5;device_groups[i+5].actual_value=1.f;}
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
static void enable_factions(boolean ships)
{
 short i;
 extraction_reset();scenario.object_names.count=7;
 if(!ships)names[3].name[0]=names[4].name[0]=0;
 scenario.starting_profiles.count=2;scenario.starting_profiles.address=faction_profiles;
 memset(faction_profiles,0,sizeof(faction_profiles));
 for(i=0;i<2;i++) {
  strcpy(faction_profiles[i].name,i?"extraction_covenant":"extraction_unsc");
  faction_profiles[i].primary_weapon.weapon.index=i+1;
  faction_profiles[i].primary_weapon.rounds_loaded=10;faction_profiles[i].primary_weapon.rounds_total=40;
  faction_profiles[i].secondary_weapon.weapon.index=NONE;
  faction_profiles[i].grenade_counts[i]=i?2:1;
  units[i].object.owner_team_index=_game_team_player;players[i].team_index=_game_team_player;
  dropships[i].definition_index=300;dropships[i].object.body_vitality=1;
 }
}
int main(int argc,char **argv)
{
 char const *case_name=argv[1]; long a; short before,i;
 boolean light=!strncmp(case_name,"light:",6);
 if(light)case_name+=6;
 initialize();
 if(!strcmp(case_name,"transport-authored-high-slot")) {
  struct scenario_object_datum vehicles[229]={0};
  struct scenario_object_palette_entry palette[1]={0};
  scenario.vehicles=(struct tag_block){NUMBEROF(vehicles),vehicles};
  scenario.vehicle_palette=(struct tag_block){NUMBEROF(palette),palette};
  names[0].runtime_object_type=_object_type_vehicle;
  names[0].runtime_scenario_datum_index=228;
  vehicles[228].name_index=0;
  vehicles[228].palette_entry_index=0;
  palette[0].reference.index=90;
  CHECK(extraction_authored_dropship(0),"valid named dropship above the classic tool limit did not resolve");
  names[0].runtime_scenario_datum_index=229;
  CHECK(!extraction_authored_dropship(0),"vehicle placement beyond the native block was admitted");
  names[0].runtime_scenario_datum_index=228;
  vehicles[228].name_index=1;
  CHECK(!extraction_authored_dropship(0),"a different placement claimed the dropship name");
  vehicles[228].name_index=0;
  palette[0].reference.index=4;
  CHECK(!extraction_authored_dropship(0),"non-vehicle tag passed the named dropship check");
  return 0;
 }
 if(!strncmp(case_name,"separate-hubs-",14)) {
  long const parked=control_handles[5];
  enable_factions(FALSE);
  strcpy(names[0].name,"extraction_unsc_deploy");
  strcpy(names[2].name,"extraction_unsc_loadout");
  strcpy(names[9].name,"extraction_covenant_deploy");
  strcpy(names[10].name,"extraction_covenant_debrief");
  names[7].name[0]=names[8].name[0]=0;
  scenario.object_names.count=11;scenario.trigger_volumes.count=7;
  strcpy(volumes[5].name,"extraction_unsc_hub");
  strcpy(volumes[6].name,"extraction_covenant_hub");
  volumes[5].type=volumes[6].type=_scenario_trigger_volume_type_oriented;
  volumes[1].name[0]=0;
  starts[0].position.z=units[0].object.position.z=3.f;
  starts[1].position.z=units[1].object.position.z=-3.f;
  players[1].team_index=units[1].object.owner_team_index=_game_team_covenant;
  scenario.controls=(struct tag_block){7,placed_controls};
  scenario.control_palette=(struct tag_block){1,control_palette};
  control_palette[0].reference.index=99;
  for(i=0;i<7;i++){
   short name=i>=5 ? i+4 : i>=3 ? i+2 : i;
   controls[i].object.name_index=placed_controls[i].object.name_index=name;
   placed_controls[i].device.power_group_index=placed_controls[i].device.position_group_index=NONE;
   names[name].runtime_object_type=_object_type_control;
   names[name].runtime_scenario_datum_index=i;
   placed_controls[i].object.position.z=(i==4||i>=5)?-3.f:3.f;
  }
  if(!strcmp(case_name,"separate-hubs-outside"))placed_controls[5].object.position.z=0.f;
  if(!strcmp(case_name,"separate-hubs-missing"))names[9].name[0]=0;
  if(!strcmp(case_name,"separate-hubs-alias"))names[9].runtime_scenario_datum_index=0;
  if(!strcmp(case_name,"separate-hubs-deferred"))placed_controls[5].object.placement_flags=FLAG(_scenario_object_placement_not_automatic_bit);
  control_handles[5]=NONE; /* auth layout before the other BSP has a live control */
  if(!strncmp(case_name,"separate-hubs-regional-",23)){
   scenario.players.count=3;
   scenario.structure_bsp_references.count=11;
   starts[0].structure_bsp_reference_index=0;
   starts[1].structure_bsp_reference_index=10;
   starts[2].position.z=0.f;
   starts[2].structure_bsp_reference_index=4;
   for(i=0;i<7;i++)
    placed_controls[i].object.on_bsp_flags=(i==4||i>=5)?FLAG(10):FLAG(0);
   if(!strcmp(case_name,"separate-hubs-regional-control"))
    placed_controls[5].object.on_bsp_flags=FLAG(0)|FLAG(10);
   else
    placed_controls[5].object.on_bsp_flags=0;
   if(!strcmp(case_name,"separate-hubs-regional-missing-geometry"))
    fixture_hub_geometry.bsp=NONE;
   if(!strcmp(case_name,"separate-hubs-regional-mixed"))starts[2].position.z=3.f;
   if(!strcmp(case_name,"separate-hubs-regional-unassigned"))starts[1].structure_bsp_reference_index=NONE;
   advance(1);
   if(strcmp(case_name,"separate-hubs-regional-starts")){
    CHECK(!extraction_owns_map(),"invalid regional start ownership admitted");
    return 0;
   }
   CHECK(extraction_owns_map(),"ordinary multiplayer start prevented hub admission");
   CHECK(extraction_player_spawn_bsp(player_handles[0])==0&&
         extraction_player_spawn_bsp(player_handles[1])==10,"faction spawn used the wrong BSP");
   {
    long biped=4;
    short profile=NONE,variant=0;
    CHECK(extraction_player_spawn_profile(player_handles[1],&biped,&profile,&variant)&&biped==4&&profile==1,
          "native default body lost its authored regional faction equipment");
   }
   CHECK(!extraction_player_start_allowed(player_handles[1],&starts[0])&&
         !extraction_player_start_allowed(player_handles[1],&starts[2])&&
         extraction_player_start_allowed(player_handles[1],&starts[1]),"faction start filter escaped the hub");
   control_handles[5]=parked;
   controls[5].device.power_group_index=3;
   controls[5].device.position_group_index=6;
   fixture_unit_bsp=10;
   fixture_control_bsp=0;
   touch(5,1);
   CHECK(!file_count,"a control in another BSP granted gear");
   fixture_unit_bsp=0;
   touch(5,1);
   CHECK(!file_count,"coordinates alone granted access from another BSP");
   fixture_unit_bsp=fixture_control_bsp=10;
   touch(5,1);
   CHECK(run(1)->phase==_extraction_deployed,"host refused an owned control inside its hub");
   return 0;
  }
  advance(1);
  if(strcmp(case_name,"separate-hubs-control")){
   CHECK(!extraction_owns_map()&&!file_count,"invalid dual hub admitted");
   return 0;
  }
  CHECK(extraction_owns_map(),"parked faction terminal rejected its authored hub");
  control_handles[5]=parked;
  controls[5].device.power_group_index=3;controls[6].device.power_group_index=4;
  controls[5].device.position_group_index=6;controls[6].device.position_group_index=7;
  units[0].object.position.z=-3.f;touch(5,0);
  CHECK(!file_count&&run(0)->phase!=_extraction_deployed,"wrong faction could deploy at the other hub");
  units[0].object.position.z=3.f;touch(2,0);
  CHECK(run(0)->loadout_seen,"UNSC loadout terminal did not route to stash");
  touch(0,0);touch(5,1);
  CHECK(run(0)->phase==_extraction_deployed&&run(1)->phase==_extraction_deployed,
   "an owned deploy control did not reserve gear at its hub");
  units[1].object.position.z=3.f;
  CHECK(!extraction_at_return(run(1),unit_handles[1]),"opposing hub accepted a Covenant return");
  units[1].object.position.z=-3.f;
  pickup(1,1);escape(1);
  CHECK(run(1)->gear.record.count==2,"Covenant hub did not bank the kit and recovered gear");
  before=remote_notices;
  touch(6,1);
  CHECK(remote_notices==before+4 && strstr(remote_notice_history[before%4],"DEBRIEF"),
   "Covenant operation report did not route to the debrief");
  return 0;
 }
 if(!strcmp(case_name,"faction-hub-clearance")) {
  real_point3d start={0,0,0};
  units[1].object.position=(real_point3d){.55f,0,0};
  CHECK(!player_extraction_start_clear(player_handles[0],5,&start,NONE),"overlapping Elite and Spartan spawn was clear");
  CHECK(!player_extraction_start_clear(player_handles[0],5,&start,0),"unassigned body did not block");
  fixture_unit_bsp=1;
  CHECK(player_extraction_start_clear(player_handles[0],5,&start,0),"different BSP blocked a regional spawn");
  CHECK(!player_extraction_start_clear(player_handles[0],5,&start,1),"same BSP did not block a regional spawn");
  units[1].object.position.x=.65f;
  CHECK(player_extraction_start_clear(player_handles[0],5,&start,1),"non-overlapping neighboring spawn was blocked");
  units[1].object.position=(real_point3d){0,0,2.f};
  CHECK(player_extraction_start_clear(player_handles[0],5,&start,1),"same horizontal coordinates blocked on a different level");
  units[1].object.position.z=0;
  units[1].object.damage_flags=FLAG(_object_dead_bit);
  CHECK(player_extraction_start_clear(player_handles[0],5,&start,1),"dead player blocked a replacement spawn");
  return 0;
 }
 if(!strncmp(case_name,"faction-hub-",12)) {
  enable_factions(FALSE);
  starts[0].position.z=3.f;
  starts[1].position.z=-3.f;
  strcpy(volumes[5].name,"extraction_unsc_hub");
  strcpy(volumes[6].name,"extraction_covenant_hub");
  volumes[5].type=volumes[6].type=_scenario_trigger_volume_type_oriented;
  scenario.trigger_volumes.count=7;
  if(!strcmp(case_name,"faction-hub-overlap"))overlap_hub_starts=TRUE;
  if(!strcmp(case_name,"faction-hub-unpaired"))scenario.trigger_volumes.count=6;
  if(!strcmp(case_name,"faction-hub-shape"))volumes[6].type=0;
  if(!strcmp(case_name,"faction-hub-empty"))starts[1].position.z=0.f;

  CHECK(extraction_player_start_allowed(player_handles[0],&starts[0]),"native first UNSC spawn refused");
  if(strcmp(case_name,"faction-hub-starts")) {
   CHECK(!extraction_owns_map(),"malformed faction hubs admitted");
   return 0;
  }
  CHECK(extraction_owns_map()&&!extraction_player_start_allowed(player_handles[0],&starts[1]),
        "UNSC start overlapped Covenant on the vertically separated placement");
  players[0].team_index=_game_team_covenant;
  CHECK(extraction_player_start_allowed(player_handles[0],&starts[1])&&
        !extraction_player_start_allowed(player_handles[0],&starts[0]),"Covenant respawn ignored faction hub");
  return 0;
 }
 if(!strncmp(case_name,"resupply-profile-",17)) {
  char const *kind=case_name+17;struct scenario_starting_profile *p=&faction_profiles[0];
  boolean reset=strcmp(kind,"additive")!=0;
  scenario.starting_profiles=(struct tag_block){2,faction_profiles};
  p->primary_weapon.weapon.index=1;p->secondary_weapon.weapon.index=NONE;
  firefight.enabled=TRUE;firefight.phase=2;
  units[0].object.body_vitality=.75f;units[0].object.shield_vitality=.5f;
  units[0].unit.grenade_counts[0]=2;units[0].unit.grenade_counts[1]=1;
  p->grenade_counts[0]=1;p->grenade_counts[1]=2;pickup(0,2);
  if(!strcmp(kind,"classic")){
   firefight.enabled=FALSE;
   enabled=FALSE;
  }
  else if(!strcmp(kind,"explicit")){p->starting_health_modifier=.4f;p->starting_shield_modifier=.8f;}
  else if(!strcmp(kind,"shield-only"))p->starting_shield_modifier=1.f;
  else if(!strcmp(kind,"unshielded"))units[0].object.shield_vitality=0;
  else if(!strcmp(kind,"incapacitated")){units[0].object.body_vitality=0;units[0].object.shield_vitality=0;}
  else CHECK(!strcmp(kind,"native")||!strcmp(kind,"additive"),"unknown spawn profile case");
  player_add_equipment(players[0].unit_index,0,reset);
  real body=!strcmp(kind,"classic")||!strcmp(kind,"incapacitated")||!strcmp(kind,"shield-only")?0:!strcmp(kind,"explicit")?.4f:.75f;
  real shield=!strcmp(kind,"classic")||!strcmp(kind,"incapacitated")||!strcmp(kind,"unshielded")?0:!strcmp(kind,"explicit")?.8f:!strcmp(kind,"shield-only")?1.f:.5f;
  CHECK(units[0].object.body_vitality==body&&units[0].object.shield_vitality==shield,"equipment profile erased native vitality, healed a downed body or changed explicit/classic profile semantics");
  CHECK(units[0].unit.grenade_counts[0]==(reset?1:3)&&units[0].unit.grenade_counts[1]==(reset?2:3),"profile reset changed native grenade grants");
  CHECK(units[0].object.damage_flags==0,"profile reset changed death flags");
  CHECK(weapon_get(units[0].unit.weapon_object_indices[0])->definition_index==(reset?1:2),"profile changed native weapon replacement");
  return 0;
 }
 if(!strncmp(case_name,"resupply-",9)) {
  char const *kind=case_name+9;long weapon=pickup(0,1),second=NONE;
  struct weapon_datum *w=weapon_get(weapon);boolean changed=TRUE;
  short count=weapon_count;int notices=local_notices+remote_notices;
  players[1].quit_out_of_game=TRUE;firefight.resupply=TRUE;
  if(!strncmp(kind,"stowed",6))w->object.parent_object_index=NONE;
  if(!strcmp(kind,"battery")||!strcmp(kind,"stowed-battery"))weapon_tag.weapon.magazines.count=0;
  else if(!strcmp(kind,"two-owned")||!strcmp(kind,"mixed-stale")) {
   second=pickup(0,2);count=weapon_count;weapon_get(second)->object.parent_object_index=NONE;
   if(!strcmp(kind,"mixed-stale")){weapon_get(second)->object.owner_player_index=player_handles[1];changed=FALSE;}
  } else if(!strcmp(kind,"transferred")){w->object.parent_object_index=unit_handles[1];changed=FALSE;}
  else if(!strcmp(kind,"dropped")){w->object.parent_object_index=NONE;w->item.flags=0;changed=FALSE;}
  else if(!strcmp(kind,"stowed-owner")){w->object.owner_player_index=player_handles[1];changed=FALSE;}
  else if(!strcmp(kind,"stowed-ground")){w->object.flags|=FLAG(_object_connected_to_map_bit);changed=FALSE;}
  else if(!strcmp(kind,"stowed-unmarked")){w->item.flags&=~FLAG(_item_attached_to_unit_bit);changed=FALSE;}
  else if(!strcmp(kind,"stowed-nonplayer")){w->item.flags&=~FLAG(_item_belongs_to_player_bit);changed=FALSE;}
  else if(!strcmp(kind,"quit")){players[0].quit_out_of_game=TRUE;changed=FALSE;}
  else if(!strcmp(kind,"dead")){units[0].object.damage_flags=FLAG(_object_dead_bit);changed=FALSE;}
  else if(!strcmp(kind,"pending")){units[0].object.damage_flags=FLAG(_object_die_act_of_god_bit);changed=FALSE;}
  else if(!strcmp(kind,"pending-silent")){units[0].object.damage_flags=FLAG(_object_die_act_of_god_silent_bit);changed=FALSE;}
  else if(!strcmp(kind,"pending-no-stats")){units[0].object.damage_flags=FLAG(_object_die_act_of_god_no_statistics_bit);changed=FALSE;}
  else if(!strcmp(kind,"wrong-body")){units[0].unit.player_index=player_handles[1];changed=FALSE;}
  else if(!strcmp(kind,"disabled")){firefight.resupply=FALSE;changed=FALSE;}
  else if(!strcmp(kind,"objective")){weapons[0].flag=TRUE;changed=FALSE;}
  else if(!strcmp(kind,"full")){w->weapon.magazines[0].rounds_total=200;changed=FALSE;}
  else CHECK(!strcmp(kind,"owned")||!strcmp(kind,"stowed"),"unknown resupply case");
  firefight_resupply_players();
  CHECK(weapon_count==count&&!extraction_runs[0].gear.opened,"resupply spawned gear or opened a stash");
  CHECK(w->weapon.magazines[0].rounds_loaded==7,"resupply changed loaded rounds");
  CHECK(weapon_tag.weapon.magazines.count ? w->weapon.age==.25f : w->weapon.age==0.f,"resupply changed ballistic age or failed battery recharge");
  CHECK(w->weapon.magazines[0].rounds_total==(!strcmp(kind,"full")||(changed&&weapon_tag.weapon.magazines.count)?200:63),"resupply changed refused gear or failed valid native reserve refill");
  if(second!=NONE)CHECK(weapon_get(second)->weapon.magazines[0].rounds_total==(changed?200:63),"stowed secondary did not follow authoritative admission");
  CHECK((local_notices+remote_notices>notices)==changed,"resupply notice did not match actual equipment change");
  return 0;
 }
 if(!strncmp(case_name,"ff-notice-",10)) {
  int host_before=local_notices,remote_before=remote_notices;char const *message="Firefight: wave cleared - recover and prepare.";
  firefight.wave=2;firefight.phase=2;
  if(!strcmp(case_name,"ff-notice-fallback"))notice_chat_visible=FALSE;
  firefight_notice(message);firefight_equipment_reports();
  CHECK(local_notices==host_before+2&&remote_notices==remote_before+2,"Survival notices lost or duplicated recipients");
  CHECK(strstr(local_notice_history[host_before%4],message)&&strstr(local_notice_history[(host_before+1)%4],"P1 | Firefight wave 2 equipment"),"host Survival report lost shared layout/controller scope");
  CHECK(strstr(remote_notice_history[(remote_before+1)%4],"P3 | Firefight wave 2 equipment"),"remote Survival equipment report lost controller scope");
  CHECK(native_hud_notices==(!strcmp(case_name,"ff-notice-fallback")?2:0),"Survival ignored native fallback or overlapped visible chat");
  if(!strcmp(case_name,"ff-notice-outcome")) {
   firefight.phase=4;firefight_notice("Firefight: victory");
   CHECK(!strcmp(firefight.outcome,"Firefight: victory"),"shared rendering lost the reconnect outcome");
  }
  return 0;
 }
 if(!strncmp(case_name,"ff-wipe-",8)) {
  firefight.enabled=firefight.gear=TRUE;firefight.phase=3;firefight.lives=0;
  memset(firefight.respawn_players,0xff,sizeof(firefight.respawn_players));
  if(!strcmp(case_name,"ff-wipe-living")) {
   units[1].object.damage_flags=FLAG(_object_dead_bit);
   CHECK(!firefight_check_defeat(tick)&&!firefight.wipe_deadline,"living survivor blocked ordinary wave completion");return 0;
  }
  if(!strcmp(case_name,"ff-wipe-owner")) {
   units[0].unit.player_index=player_handles[1];units[1].object.damage_flags=FLAG(_object_dead_bit);
   CHECK(!firefight_check_defeat(tick)&&firefight.wipe_deadline,"orphaned unit authorized completion");return 0;
  }
  if(!strcmp(case_name,"ff-wipe-paid")||!strcmp(case_name,"ff-wipe-recycled")) {
   firefight.lives=1;
   units[0].object.damage_flags=units[1].object.damage_flags=FLAG(_object_dead_bit);
   coop_enemies_firefight_player_died(unit_handles[0]);coop_enemies_firefight_player_died(unit_handles[1]);
   CHECK(!firefight.lives&&firefight.respawn_players[0]==player_handles[0],"last life did not reserve its actual player");
   if(!strcmp(case_name,"ff-wipe-recycled")) {
    player_handles[0]+=0x10000;units[0].unit.player_index=player_handles[0];
    CHECK(!coop_enemies_firefight_can_spawn(player_handles[0])&&coop_enemies_firefight_out_of_lives(),"recycled player inherited a paid respawn");return 0;
   }
   CHECK(!firefight_check_defeat(tick)&&firefight.wipe_deadline,"cleared wave could finish before the paid respawn");
   CHECK(coop_enemies_firefight_can_spawn(player_handles[0])&&!coop_enemies_firefight_can_spawn(player_handles[1]),"waiting wave denied its paid spawn or granted a free one");
   tick+=120;CHECK(!firefight_check_defeat(tick)&&firefight.wipe_deadline&&coop_enemies_firefight_can_spawn(player_handles[0]),"paid respawn wait became a timed defeat");
   unit_handles[0]+=0x10000;players[0].unit_index=unit_handles[0];units[0].object.damage_flags=0;units[0].object.body_vitality=1.f;
   coop_enemies_firefight_player_spawned(player_handles[0]);
   CHECK(firefight.respawn_players[0]==NONE&&!firefight_check_defeat(++tick)&&!firefight.wipe_deadline,"new living paid body did not release progression");return 0;
  }
  medical_setting=TRUE;extraction_reset();
  for(i=0;i<2;i++) {
   if(!strcmp(case_name,"ff-wipe-pending"))units[i].object.damage_flags=FLAG(_object_die_act_of_god_bit);
   else {units[i].object.body_vitality=-.1f;CHECK(extraction_medical_lethal(unit_handles[i]),"native living casualty setup failed");}
   coop_enemies_firefight_player_died(unit_handles[i]);
  }
  CHECK(!firefight_check_defeat(tick)&&firefight.wipe_deadline,"team unable to act advanced the cleared wave");
  tick+=120;CHECK(!firefight_check_defeat(tick),"living casualties became a premature defeat");
  if(!strcmp(case_name,"ff-wipe-eliminated")) {
   for(i=0;i<2;i++) {units[i].object.damage_flags=FLAG(_object_dead_bit);coop_enemies_firefight_player_died(unit_handles[i]);}
   CHECK(!firefight_check_defeat(tick)&&firefight.wipe_deadline==tick+90,"downed wait shortened the true elimination grace");
   CHECK(!firefight_check_defeat(tick+89)&&firefight_check_defeat(tick+90),"native elimination delay changed");
  } else {
   units[0].object.damage_flags=0;units[0].object.body_vitality=.25f;
   CHECK(!firefight_check_defeat(++tick)&&!firefight.wipe_deadline,"standing native survivor did not resume progression");
  }
  CHECK(!firefight.victory&&coop_enemies_firefight_outcome()==0,"waiting state invented successful rewards");return 0;
 }
 if(!strncmp(case_name,"squad-",6)) {
  extraction_reset();
  names[3].name[0]=names[4].name[0]=names[5].name[0]=names[6].name[0]=names[7].name[0]=names[8].name[0]=0;
  scenario.object_names.count=11;
  for(i=0;i<2;i++) {
   snprintf(names[9+i].name,32,"extraction_squad_%02d",i+1);
   controls[5+i].object.name_index=9+i;controls[5+i].device.position_group_index=10;controls[5+i].device.power_group_index=11;
   snprintf(squad_scripts[i].name,32,"forerunner_squad_%02d_complete",i+1);
   squad_scripts[i].return_type=_hs_type_void;squad_scripts[i].script_type=_hs_script_continuous;
   snprintf(squad_scripts[2+i].name,32,"forerunner_squad_%02d_aborted",i+1);
   squad_scripts[2+i].return_type=_hs_type_void;squad_scripts[2+i].script_type=_hs_script_continuous;
  }
  device_groups[11].actual_value=1;scenario.hs_scripts.count=4;scenario.hs_scripts.address=squad_scripts;
  if(!strcmp(case_name,"squad-return-roam")) {
   strcpy(squad_scripts[4].name,"forerunner_extraction_complete");
   squad_scripts[4].script_type=_hs_script_dormant;squad_scripts[4].return_type=_hs_type_void;
   scenario.hs_scripts.count++;
  }
  if(!strcmp(case_name,"squad-operation-begin")) {
   for(i=0;i<2;i++) {
    snprintf(squad_scripts[4+i].name,32,"forerunner_squad_%02d_begin",i+1);
    squad_scripts[4+i].return_type=_hs_type_void;squad_scripts[4+i].script_type=_hs_script_continuous;
   }
   scenario.hs_scripts.count=6;
  }
  if(!strcmp(case_name,"squad-duplicate"))strcpy(names[10].name,names[9].name);
  if(!strcmp(case_name,"squad-alias"))control_handles[6]=control_handles[5];
  if(!strcmp(case_name,"squad-missing"))control_handles[6]=NONE;
  if(!strncmp(case_name,"squad-objective-",16)) {
   scenario.device_groups.count=10;scenario.device_groups.address=authored_groups;
   strcpy(authored_groups[8].name,"extraction_squad_01_ready");strcpy(authored_groups[9].name,"extraction_squad_02_ready");
   device_groups[8].actual_value=device_groups[9].actual_value=1;
   if(!strcmp(case_name,"squad-objective-alias"))controls[5].device.power_group_index=8;
   if(!strcmp(case_name,"squad-objective-duplicate"))strcpy(authored_groups[9].name,authored_groups[8].name);
  }
  advance(1);
  if(!strcmp(case_name,"squad-duplicate")||!strcmp(case_name,"squad-alias")||!strcmp(case_name,"squad-missing")||
   !strcmp(case_name,"squad-objective-alias")||!strcmp(case_name,"squad-objective-duplicate")) {
   CHECK(!extraction_owns_map(),"ambiguous or missing squad terminal admitted");return 0;
  }
  CHECK(extraction_owns_map(),"valid squad hub refused");
  for(i=0;i<2;i++)players[i].team_index=units[i].object.owner_team_index=_game_team_player;
  if(!strncmp(case_name,"squad-radio-",12)) {
   short private0=extraction_voice_group(NONE,0),private1=extraction_voice_group(NONE,1);
   CHECK(private0!=private1&&private0>=2+EXTRACTION_SQUADS&&private1<=255,"unknown radio groups collide");
   CHECK(extraction_voice_group(player_handles[0],0)==0&&extraction_voice_group(player_handles[1],1)==0,"unsquadded UNSC radio missing");
   if(!strcmp(case_name,"squad-radio-state")) {
    extraction.bound=FALSE;CHECK(extraction_voice_group(player_handles[0],0)==private0,"unbound operation broadcast TEAM voice");extraction.bound=TRUE;
    extraction.admitted=FALSE;CHECK(extraction_voice_group(player_handles[0],0)==NONE,"rejected classic map lost legacy radio");extraction.admitted=TRUE;
    connection=_game_connection_network_client;CHECK(extraction_voice_group(player_handles[0],0)==NONE,"client authored voice authority");connection=_game_connection_network_server;
    CHECK(extraction_voice_group(NONE,-1)==NONE&&extraction_voice_group(NONE,HALO_PORT_MAXIMUM_NETWORK_MACHINES)==NONE,"invalid machine got a radio group");return 0;
   }
   if(!strcmp(case_name,"squad-radio-faction")) {
    players[1].team_index=_game_team_covenant;
    CHECK(extraction_voice_group(player_handles[1],1)==1,"Covenant fallback shared UNSC radio");return 0;
   }
   touch(5,0);touch(!strcmp(case_name,"squad-radio-separate")?6:5,1);
   CHECK(extraction_voice_group(player_handles[0],0)==2,"joined squad lost its host-owned radio label");
   CHECK(extraction_voice_group(player_handles[1],1)==(!strcmp(case_name,"squad-radio-separate")?3:2),"radio labels crossed authoritative memberships");
   if(!strcmp(case_name,"squad-radio-restored")) {
    extraction_restored();advance(1);
    CHECK(extraction_voice_group(player_handles[0],0)==private0&&extraction_voice_group(player_handles[1],1)==private1,"restored operation broadcast former squad radio");
   } else if(!strcmp(case_name,"squad-radio-recycled")) {
    player_handles[0]+=0x10000;
    CHECK(extraction_voice_group(player_handles[0],0)==0,"recycled player inherited previous squad radio");
   } else if(!strcmp(case_name,"squad-radio-dead")) {
    units[0].object.damage_flags=FLAG(_object_dead_bit);
    CHECK(extraction_voice_group(player_handles[0],0)==2,"death erased trusted squad radio membership");
   } else if(!strcmp(case_name,"squad-radio-leave")) {
    touch(5,0);CHECK(extraction_voice_group(player_handles[0],0)==0,"departed member retained private squad radio");
   }
   return 0;
  }
  if(!strcmp(case_name,"squad-reconnect-settlement-order")||
     !strncmp(case_name,"squad-reconnect-deferred-escrow",31)) {
   boolean deferred=strcmp(case_name,"squad-reconnect-settlement-order")!=0;
   boolean stale=!strcmp(case_name,"squad-reconnect-deferred-escrow-stale");
   boolean reported=!strcmp(case_name,"squad-reconnect-deferred-escrow-reported");
   byte recovered[GEAR_ID_BYTES];

   delta_session=TRUE;
   if(reported) touch(5,0);
   touch(5,1);touch(0,1);a=pickup(1,2);
   if(reported) { touch(0,0);pickup(0,1);escape(0); }
   units[1].object.position.x=20;touch(1,1);advance(300);
   publish_failure=TRUE;units[1].object.position.x=0;advance(1);
   CHECK(run(1)->phase==_extraction_settling&&!weapon_try_and_get(a)&&!squad_wakes[0],
         "reconnect fixture did not reach durable escape escrow");
   if(deferred) {
    players[1].quit_out_of_game=TRUE;advance(1);
    CHECK(!run(1)->gear.opened&&squad_wakes[0]==reported&&!squad_aborts[0],
          "failed escrow close changed the verified squad outcome");
    players[1].quit_out_of_game=FALSE;
   }

   publish_failure=FALSE;
   player_handles[1]+=0x10000;units[1].unit.player_index=player_handles[1];
   unit_handles[1]+=0x10000;players[1].unit_index=unit_handles[1];
   if(stale) {
    touch(5,0);touch(0,0);
    CHECK(run(0)->squad_operation==2,"fixture did not start a newer operation");
    touch(2,1);
    CHECK(run(1)->gear.record.count==1&&!extraction.squads[0].banked,
          "older escrow receipt completed the current operation");
    units[0].object.damage_flags=FLAG(_object_dead_bit);advance(1);
    CHECK(!squad_wakes[0]&&squad_aborts[0]==1,"old receipt hid the new operation's loss");
    extraction_reset();return 0;
   }
   touch(0,1);
   if(reported) {
    CHECK(run(1)->phase==_extraction_deployed&&run(1)->squad_operation==2&&
          run(1)->gear.record.reserved_count==1,"reported squad blocked escrow recovery and redeployment");
    advance(1);CHECK(squad_wakes[0]==1&&!squad_aborts[0],"reopened escrow repeated a reported completion");
    extraction_reset();return 0;
   }
   CHECK(run(1)->phase==_extraction_hub&&run(1)->gear.record.phase==_gear_idle&&
         run(1)->gear.record.count==1&&extraction.squads[0].operation==1&&!squad_wakes[0],
         "reconnect started a new operation before reporting the prior escape");
   memcpy(recovered,run(1)->gear.record.items[0].id,sizeof(recovered));

   advance(1);
   CHECK(squad_wakes[0]==1&&!squad_aborts[0],"reconnect lost its settled operation callback");
   touch(0,1);
   CHECK(run(1)->phase==_extraction_deployed&&run(1)->squad_operation==2&&
         run(1)->gear.record.reserved_count==1&&
         !memcmp(recovered,run(1)->gear.record.reserved[0].id,sizeof(recovered)),
         "reconnect could not redeploy the exact banked item after the callback");
   extraction_reset();return 0;
  }
  if(!strncmp(case_name,"squad-reconnect",15)) {
   delta_session=TRUE;
   touch(5,1);
   CHECK(run(1)->squad==0,"initial verified squad choice refused");
   if(!strcmp(case_name,"squad-reconnect-leave")) {
    touch(5,1);
    CHECK(run(1)->squad==NONE,"explicit squad leave refused");
   } else {
    touch(0,1); pickup(1,2); escape(1); touch(0,1);
    CHECK(run(1)->gear.record.reserved_count==1,"prior run did not put a real item at risk");
   }
   players[1].quit_out_of_game=TRUE; advance(1);
   CHECK(!run(1)->gear.opened,"departed member kept their gear lock");
   player_handles[1]+=0x10000; units[1].unit.player_index=player_handles[1];
   players[1].quit_out_of_game=FALSE; advance(1);
   CHECK(run(1)->squad==NONE,"new slot inherited live squad state");
   if(!strcmp(case_name,"squad-reconnect-reset")) { extraction_reset(); advance(1); }
   if(!strcmp(case_name,"squad-reconnect-reset")||!strcmp(case_name,"squad-reconnect-leave")) {
    touch(0,1); CHECK(run(1)->phase==_extraction_hub&&run(1)->squad==NONE,"cleared choice returned");return 0;
   }
   if(!strcmp(case_name,"squad-reconnect-owner")) {
    verified=FALSE;touch(0,1);
    CHECK(run(1)->squad==NONE&&run(1)->phase==_extraction_hub,"unbound reconnect recovered squad");
    verified=TRUE;remote_account[0]++;
    touch(0,1);
    CHECK(run(1)->squad==NONE&&run(1)->phase==_extraction_hub,"new owner inherited remembered squad");
    remote_account[0]--;
   } else if(!strcmp(case_name,"squad-reconnect-faction")) {
    players[1].team_index=units[1].object.owner_team_index=_game_team_covenant;
    touch(0,1);
    CHECK(run(1)->squad==NONE&&run(1)->phase==_extraction_hub,"opposing faction recovered squad");
    players[1].team_index=units[1].object.owner_team_index=_game_team_player;
   } else if(!strcmp(case_name,"squad-reconnect-full")) {
    active_players=8;
    for(i=2;i<8;i++) {players[i].team_index=_game_team_player;run(i)->player=player_handles[i];run(i)->squad=0;}
    touch(0,1);
    CHECK(run(1)->squad==NONE&&run(1)->phase==_extraction_hub,"remembered choice overfilled the squad");
    active_players=2;
   } else if(!strcmp(case_name,"squad-reconnect-active")) {
    touch(5,0);touch(0,0);
    CHECK(run(0)->phase==_extraction_deployed,"remaining squad member did not deploy");
    touch(0,1);
    CHECK(run(1)->squad==NONE&&run(1)->phase==_extraction_hub,"reconnected player entered active operation");
    escape(0);
   }
   touch(0,1);
   CHECK(run(1)->squad==0&&run(1)->phase==_extraction_deployed&&run(1)->squad_operation,
    "verified owner could not redeploy with their remembered squad");
   CHECK(!run(1)->gear.record.reserved_count&&!run(1)->gear.record.count,"reconnect returned gear lost on disconnect");
   return 0;
  }
  touch(0,0);CHECK(run(0)->phase==_extraction_hub&&!weapon_count,"ungrouped player deployed without squad selection");
  if(!strcmp(case_name,"squad-capacity")) {
   active_players=7;
   /* Membership capacity only: six bounded existing players, not six real accounts or banks. */
   for(i=1;i<7;i++) {players[i].team_index=_game_team_player;players[i].local_player_index=NONE;run(i)->player=player_handles[i];run(i)->squad=0;}
   touch(5,0);CHECK(run(0)->squad==NONE,"seventh member admitted");
   touch(6,0);CHECK(run(0)->squad==1,"independent squad blocked by full squad");
   players[6].quit_out_of_game=TRUE;touch(5,0);CHECK(run(0)->squad==0,"departed player retained a live membership slot");return 0;
  }
  if(!strcmp(case_name,"squad-state")) {
   units[0].object.position.x=20;touch(5,0);CHECK(run(0)->squad==NONE,"field squad join");units[0].object.position.x=0;
   units[0].object.parent_object_index=dropship_handles[0];touch(5,0);CHECK(run(0)->squad==NONE,"seated squad join");units[0].object.parent_object_index=NONE;
   device_groups[11].actual_value=0;touch(5,0);CHECK(run(0)->squad==NONE,"unpowered squad join");device_groups[11].actual_value=1;
   connection=_game_connection_network_client;touch(5,0);CHECK(run(0)->squad==NONE,"client chose authoritative membership");connection=_game_connection_network_server;return 0;
  }
  touch(5,0);CHECK(run(0)->squad==0&&!control_updates[5],"squad choice missing or shared native switch toggled");
  if(!strcmp(case_name,"squad-identity")) {
   verified=FALSE;touch(5,1);CHECK(run(1)->squad==NONE,"unverified remote identity joined");return 0;
  }
  if(!strcmp(case_name,"squad-faction")) {
   players[1].team_index=units[1].object.owner_team_index=_game_team_covenant;
   touch(5,1);CHECK(run(1)->squad==NONE,"opposing factions joined one squad");touch(6,1);CHECK(run(1)->squad==1,"opposing independent squad refused");return 0;
  }
  if(!strcmp(case_name,"squad-operation-begin")) {
   touch(5,1);CHECK(run(1)->squad==0,"existing squad selection failed");
   fail_write=TRUE;touch(0,0);
   CHECK(run(0)->phase==_extraction_hub&&!squad_begins[0],"failed reservation started mission setup");
   fail_write=FALSE;advance(120);
   touch(0,0);
   CHECK(run(0)->phase==_extraction_deployed&&squad_begins[0]==1&&!squad_begins[1],
    "first confirmed deployment did not wake its own setup");
   touch(0,1);advance(1);
   CHECK(run(1)->phase==_extraction_deployed&&squad_begins[0]==1,"another member restarted the active squad");
   escape(0);escape(1);
   CHECK(squad_wakes[0]==1&&!squad_wakes[1],"setup interfered with independent settlement");
   touch(6,1);touch(0,1);
   CHECK(run(1)->phase==_extraction_deployed&&squad_begins[1]==1,"independent squad could not begin");
   touch(0,0);
   CHECK(run(0)->squad_operation==2&&squad_begins[0]==2&&squad_begins[1]==1,
    "redeployment did not rearm only the returning squad");
   return 0;
  }
  if (!strcmp(case_name, "squad-join-active")) {
   touch(0, 0);
   CHECK(run(0)->phase == _extraction_deployed, "first squad did not deploy");
   touch(5, 1);
   CHECK(run(1)->squad == NONE, "hub player joined an active squad");
   touch(0, 1);
   CHECK(run(1)->phase == _extraction_hub, "late join inherited a deployment");
   touch(6, 1);
   CHECK(run(1)->squad == 1, "active operation locked another squad");
   escape(0);
   CHECK(run(0)->phase == _extraction_returned, "first squad could not settle");
   touch(5, 1);
   CHECK(run(1)->squad == 0, "finished squad did not accept a member");
   touch(0, 0);
   touch(0, 1);
   CHECK(run(0)->phase == _extraction_deployed && run(1)->phase == _extraction_deployed &&
    run(0)->squad_operation == 2 && run(1)->squad_operation == 2,
    "members selected before departure could not deploy in sequence");
   return 0;
  }
  if(!strcmp(case_name,"squad-leave")) {
   touch(5,0);CHECK(run(0)->squad==NONE&&run(0)->gear.record.phase==_gear_idle,"leave changed gear ownership or retained membership");return 0;
  }
  if(!strncmp(case_name,"squad-radio-cue",15)) {
   long sound=91;
   CHECK(!extraction_script_radio("ordinary_radio",sound),"ordinary sound script intercepted");
   CHECK(extraction_script_radio("forerunner_squad_01_radio",sound)&&!private_radio_calls,"undeployed radio");
   touch(strstr(case_name,"shared")?5:6,1);touch(0,0);touch(0,1);
   if(strstr(case_name,"local"))players[1].local_player_index=1;
   if(strstr(case_name,"remote-mixed")){active_players=3;players[2].local_player_index=NONE;}
   if(strstr(case_name,"owner"))profile=FALSE;
   if(strstr(case_name,"client"))connection=_game_connection_network_client;
   if(strstr(case_name,"invalid"))sound=NONE;
   if(strstr(case_name,"full"))radio_queue_full=TRUE;
   if(strstr(case_name,"remote"))extraction_script_radio("forerunner_squad_02_radio",sound);
   else extraction_script_radio("forerunner_squad_01_radio",sound);
   if(!strcmp(case_name,"squad-radio-cue-loop")) {
    CHECK(private_radio_calls==1&&private_radio_played==1&&!private_radio_remote,"host radio broadcast");
    extraction_script_radio("forerunner_squad_02_radio",sound);
    CHECK(private_radio_calls==2&&private_radio_played==1&&private_radio_remote==1,"remote radio heard on host or missing");
   } else if(strstr(case_name,"shared")) {
    CHECK(private_radio_calls==1&&private_radio_played==1&&private_radio_remote==(!strstr(case_name,"local")),"same squad local/remote duplicate audio");
   } else CHECK(!private_radio_calls&&!private_radio_played&&!private_radio_remote,"private radio escaped eligible machine scope");
   for(i=0;i<8;i++)CHECK(!list_used[i],"radio list leaked");
   return 0;
  }
  if(!strncmp(case_name,"squad-notice",12)) {
   int before_local,before_remote;long list;
   notice_text.text_data=(struct tag_data){sizeof(authored_notice),authored_notice};
   notice_text.elements=(struct tag_block){1,&notice_element};
   notice_text.messages=(struct tag_block){1,&notice_message};
   scenario.hud_messages.index=3;notice_element.data=(byte)(wcslen(authored_notice)+1);
   CHECK(!extraction_script_notice("ordinary_notice",0),"ordinary message script intercepted");
   before_local=local_notices;before_remote=remote_notices;
   CHECK(extraction_script_notice("forerunner_squad_01_notice",0)&&local_notices==before_local&&remote_notices==before_remote,"undeployed squad received notice");
   touch(!strcmp(case_name,"squad-notice-shared")?5:6,1);touch(0,0);touch(0,1);
   before_local=local_notices;before_remote=remote_notices;
   if(!strcmp(case_name,"squad-notice-policy"))authored_player_actions=FALSE;
   if(!strcmp(case_name,"squad-notice-owner"))profile=FALSE;
   if(!strcmp(case_name,"squad-notice-death"))units[0].object.damage_flags=FLAG(_object_die_act_of_god_bit);
   if(!strcmp(case_name,"squad-notice-epoch"))run(0)->squad_operation++;
   if(!strcmp(case_name,"squad-notice-client"))connection=_game_connection_network_client;
   if(!strcmp(case_name,"squad-notice-restored"))extraction_restored();
   if(!strcmp(case_name,"squad-notice-storage"))list_allocation_failure=TRUE;
   if(!strcmp(case_name,"squad-notice-text"))authored_notice[2]=0x100;
   if(!strcmp(case_name,"squad-notice-icon"))notice_element.type=_hud_message_type_icon;
   if(!strcmp(case_name,"squad-notice-bounds"))notice_text.text_data.size=2;
   if(!strcmp(case_name,"squad-notice-termination"))authored_notice[notice_element.data-1]='x';
   if(!strcmp(case_name,"squad-notice-long")){wmemset(authored_notice,L'x',128);authored_notice[128]=0;notice_element.data=129;}
   if(!strcmp(case_name,"squad-notice-index"))notice_message.element_start_index=1;
   CHECK(extraction_script_notice("forerunner_squad_01_notice",0),"reserved notice not handled");
   if(!strcmp(case_name,"squad-notice-loop")||!strcmp(case_name,"squad-notice-shared")) {
    CHECK(local_notices==before_local+1&&strstr(local_notice_history[before_local%4],"P1 | Squad 01: recover technology."),"local squad cue missing");
    CHECK(remote_notices==before_remote+(!strcmp(case_name,"squad-notice-shared")),"squad cue crossed recipients");
    if(!strcmp(case_name,"squad-notice-loop")) {
     extraction_script_notice("forerunner_squad_02_notice",0);
     CHECK(local_notices==before_local+1&&remote_notices==before_remote+1,"remote squad cue not isolated");
    }
   } else CHECK(local_notices==before_local&&remote_notices==before_remote,"invalid notice or unauthorized participant received text");
   for(i=0;i<8;i++)CHECK(!list_used[i],"squad notice leaked native object list");
   return 0;
  }
  if(!strncmp(case_name,"squad-query",11)) {
   long list=42;short j;
   CHECK(!extraction_script_players("ordinary_players",&list)&&list==42,"ordinary static script intercepted");
   CHECK(extraction_script_players("forerunner_squad_01_players",&list)&&list==NONE,"undeployed members exposed to objective");
   touch(!strcmp(case_name,"squad-query-storage")?5:6,1);touch(0,0);touch(0,1);
   CHECK(run(0)->phase==_extraction_deployed&&run(1)->phase==_extraction_deployed,"query participants not deployed");
   if(!strcmp(case_name,"squad-query-storage")) {
    list_failure_at=2;extraction_script_players("forerunner_squad_01_players",&list);CHECK(list==NONE,"partial player list escaped reference exhaustion");
    for(j=0;j<8;j++)CHECK(!list_used[j],"partial query list leaked");
    list_failure_at=0;list_allocation_failure=TRUE;extraction_script_players("forerunner_squad_01_players",&list);CHECK(list==NONE,"allocation failure exposed participants");return 0;
   }
   if(!strcmp(case_name,"squad-query-policy")) {
    authored_player_actions=FALSE;extraction_script_players("forerunner_squad_01_players",&list);CHECK(list==NONE&&extraction.squad_script_warning,"legacy follower policy accepted for scoped mission");authored_player_actions=TRUE;
   }
   CHECK(extraction_script_players("forerunner_squad_01_players",&list)&&list!=NONE&&list_players[list]&&list_sizes[list]==1&&list_units[list][0]==unit_handles[0],"squad query crossed ownership or lost provenance");object_list_delete(list);
   if(!strcmp(case_name,"squad-query-state")) {
    units[0].object.damage_flags=FLAG(_object_die_act_of_god_silent_bit);extraction_script_players("forerunner_squad_01_players",&list);CHECK(list!=NONE&&!list_sizes[list],"pending death advanced objective");object_list_delete(list);units[0].object.damage_flags=0;
    player_handles[0]+=0x10000;extraction_script_players("forerunner_squad_01_players",&list);CHECK(list!=NONE&&!list_sizes[list],"recycled player inherited objective membership");object_list_delete(list);player_handles[0]-=0x10000;
    profile=FALSE;extraction_script_players("forerunner_squad_01_players",&list);CHECK(list!=NONE&&!list_sizes[list],"unverified owner advanced objective");object_list_delete(list);profile=TRUE;
   }
   escape(0);extraction_script_players("forerunner_squad_01_players",&list);CHECK(list!=NONE&&list_sizes[list]==1,"durably returned participant absent from callback query");object_list_delete(list);
   touch(0,0);extraction_script_players("forerunner_squad_01_players",&list);CHECK(list!=NONE&&list_sizes[list]==1&&run(0)->squad_operation==2,"fresh operation query lost member");object_list_delete(list);
   extraction_restored();extraction_script_players("forerunner_squad_01_players",&list);CHECK(list==NONE,"saved-world query continued operation");return 0;
  }
  if(!strncmp(case_name,"squad-objective-",16)) {
   CHECK(extraction.squads[0].ready_group==8&&device_groups[8].actual_value==0,"stale authored objective admitted");
   touch(6,1);touch(0,0);touch(0,1);a=pickup(0,1);
   units[0].object.position.x=units[1].object.position.x=20;
   touch(1,0);CHECK(run(0)->phase==_extraction_deployed&&!control_updates[1],"incomplete objective activated extraction");
   device_groups[8].actual_value=.5f;touch(1,0);CHECK(run(0)->phase==_extraction_deployed,"partial objective signal admitted");
   device_groups[8].actual_value=1;touch(1,0);touch(1,1);CHECK(run(0)->phase==_extraction_activating&&run(1)->phase==_extraction_deployed,"squad objective unlocked other squad");
   device_groups[8].actual_value=0;advance(1);CHECK(run(0)->phase==_extraction_deployed&&!run(0)->activation_ticks,"objective revocation did not cancel countdown");
   device_groups[8].actual_value=1;touch(1,0);advance(300);
   if(!strcmp(case_name,"squad-objective-escrow")) {
    units[0].object.position.x=0;publish_failure=TRUE;advance(1);CHECK(run(0)->phase==_extraction_settling,"escrow not pending");
    device_groups[8].actual_value=0;publish_failure=FALSE;advance(30);CHECK(run(0)->phase==_extraction_returned&&run(0)->gear.record.count==1,"revocation rewound committed native consumption");return 0;
   }
   device_groups[8].actual_value=0;units[0].object.position.x=0;advance(1);CHECK(run(0)->phase==_extraction_deployed&&!run(0)->gear.record.count&&weapon_try_and_get(a),"revoked objective banked gear");
   device_groups[8].actual_value=1;escape(0);CHECK(run(0)->gear.record.count==1&&run(1)->phase==_extraction_deployed,"scoped objective escape changed other squad");
   touch(0,0);CHECK(run(0)->phase==_extraction_deployed&&device_groups[8].actual_value==0,"new operation inherited old mission success");return 0;
  }
  if(!strcmp(case_name,"squad-wipe-independent")) {
   touch(6,1);touch(0,0);touch(0,1);
   CHECK(run(0)->phase==_extraction_deployed&&run(1)->phase==_extraction_deployed,"independent squads did not deploy");
   units[0].object.damage_flags=FLAG(_object_dead_bit);advance(1);
   CHECK(squad_aborts[0]==1&&!squad_aborts[1]&&!completion_wakes,"wipe crossed squads or advanced global success");
   CHECK(run(1)->phase==_extraction_deployed&&run(1)->gear.record.phase==_gear_deployed,"wipe changed the other squad's run");
   units[0].object.damage_flags=0;touch(0,0);
   CHECK(run(0)->phase==_extraction_deployed&&run(0)->squad_operation==2,"wiped squad could not redeploy");
   pickup(0,1);escape(0);
   CHECK(squad_aborts[0]==1&&squad_wakes[0]==1&&!squad_wakes[1],"redeployed squad could not bank independently");
   return 0;
  }
  touch(!strcmp(case_name,"squad-independent")?6:5,1);
  CHECK(run(1)->squad==(!strcmp(case_name,"squad-independent")?1:0),"second member not assigned");
  touch(0,0);touch(0,1);
  CHECK(run(0)->phase==_extraction_deployed&&run(1)->phase==_extraction_deployed,"squad did not deploy");
  if(!strcmp(case_name,"squad-independent")) {
   a=pickup(0,1);escape(0);CHECK(squad_wakes[0]==1&&!squad_wakes[1],"another deployed squad blocked or received completion");
   CHECK(run(1)->phase==_extraction_deployed&&run(1)->gear.record.phase==_gear_deployed,"another squad's settlement changed gear");
   touch(0,0);CHECK(run(0)->phase==_extraction_deployed&&run(0)->squad_operation==2,"independent redeploy blocked");
   escape(0);CHECK(squad_wakes[0]==2&&!squad_wakes[1],"fresh operation callback absent or crossed squads");
   a=pickup(1,2);escape(1);CHECK(squad_wakes[1]==1&&run(1)->gear.record.count==1,"other squad did not bank separately");return 0;
  }
  CHECK(run(0)->squad_operation==run(1)->squad_operation,"same squad did not share operation epoch");
  if(!strncmp(case_name,"squad-bank-",11)) {
   boolean unconfirmed=!strcmp(case_name,"squad-bank-escrow-unconfirmed");
   boolean escrow=!strcmp(case_name,"squad-bank-escrow-disconnect")||unconfirmed;
   boolean captured=strcmp(case_name,"squad-bank-unsettled-disconnect")!=0;
   if(captured) {
    a=pickup(1,2);
    if(escrow) {
     units[1].object.position.x=20.f;touch(1,1);advance(300);
     publish_failure=TRUE;units[1].object.position.x=0.f;advance(1);
     CHECK(run(1)->phase==_extraction_settling,"guest did not reach pending escrow");
    } else escape(1);
    CHECK(!weapon_try_and_get(a),"escaped item was not consumed");
   }
   CHECK(!squad_wakes[0],"unfinished squad accepted an early callback");
   publish_failure=unconfirmed;
   players[1].quit_out_of_game=TRUE;advance(1);
   CHECK(!squad_wakes[0]&&!squad_aborts[0],"disconnect ended an operation with a member still in the field");
   units[0].object.damage_flags=FLAG(_object_dead_bit);
   publish_failure=TRUE;advance(1);
   CHECK(run(0)->phase==_extraction_losing&&!squad_wakes[0]&&!squad_aborts[0],"pending loss ended the squad operation");
   publish_failure=FALSE;advance(30);
   CHECK(run(0)->phase==_extraction_lost&&!run(0)->gear.record.count,"surviving player earned gear from a loss");
   CHECK(squad_wakes[0]==(captured&&!unconfirmed)&&!squad_wakes[1],"same-squad durable bank vanished after disconnect or an unbanked run completed");
   CHECK(squad_aborts[0]==!captured&&!squad_aborts[1],"loss callback missing, crossed squads, or presumed an unknown escrow lost");
   advance(30);CHECK(squad_wakes[0]==(captured&&!unconfirmed)&&squad_aborts[0]==!captured,"same operation sent duplicate callbacks");
   units[0].object.damage_flags=0;touch(0,0);
   CHECK(run(0)->phase==_extraction_deployed&&run(0)->squad_operation==2,"old completion blocked a new deployment");
   units[0].object.damage_flags=FLAG(_object_dead_bit);advance(1);
   CHECK(squad_wakes[0]==(captured&&!unconfirmed)&&squad_aborts[0]==1+!captured,"new operation inherited the previous outcome");
   return 0;
  }
  touch(6,0);CHECK(run(0)->squad==0,"deployed member changed squad");
  a=pickup(0,1);escape(0);CHECK(!squad_wakes[0],"partial squad return notified completion");
  touch(0,0);CHECK(run(0)->phase==_extraction_returned,"returned member redeployed into an unfinished operation");
  touch(6,0);CHECK(run(0)->squad==0,"returned member abandoned unfinished squad");
  if(!strcmp(case_name,"squad-return-roam")) {
   units[0].object.position.x=20;advance(1);
   CHECK(!squad_wakes[0],"banked member completed squad with a teammate still deployed");
  }
  units[1].object.position.x=20;touch(1,1);advance(300);units[1].object.position.x=0;
  if(!strcmp(case_name,"squad-pending")||!strcmp(case_name,"squad-return-roam")) {
   publish_failure=TRUE;advance(1);CHECK(run(1)->phase==_extraction_settling&&!squad_wakes[0],"pending banking completed squad");publish_failure=FALSE;advance(30);
  } else if(!strcmp(case_name,"squad-loss")) {
   units[1].object.damage_flags=FLAG(_object_dead_bit);advance(1);
  } else {advance(1);}
  CHECK(squad_wakes[0]==1&&!squad_wakes[1]&&run(0)->gear.record.count==1,"durable squad completion or individual stash wrong");
  if(!strcmp(case_name,"squad-return-roam")) {
   CHECK(run(1)->phase==_extraction_returned&&!run(1)->gear.pending,"last member did not durably settle");
   CHECK(!completion_wakes,"named squad completion bypassed the global player return check");
   units[0].object.position.x=0;advance(1);
   CHECK(completion_wakes==1,"global mission stopped waiting for both players at the return point");
  }
  advance(30);CHECK(squad_wakes[0]==1,"same operation repeated completion");
  if(!strcmp(case_name,"squad-restore")) {
   extraction_restored();advance(1);CHECK(squad_wakes[0]==1,"saved-world replay completed squad again");return 0;
  }
  touch(0,0);CHECK(run(0)->phase==_extraction_deployed&&run(0)->squad_operation==2,"finished squad could not redeploy");
  escape(0);CHECK(squad_wakes[0]==2,"old return blocked fresh operation or callback did not rearm");
  extraction_reset();return 0;
 }
 if(!strncmp(case_name,"stash-browser-",14)) {
  struct gear_record saved;byte first[GEAR_ID_BYTES],second[GEAR_ID_BYTES];int messages;
  strcpy(names[2].name,"extraction_debrief");extraction_reset();advance(1);
  touch(0,0);pickup(0,1);pickup(0,2);escape(0);extraction_reset();advance(1);
  touch(2,0);saved=run(0)->gear.record;saved.items[0].map_checksum++;
  CHECK(gear_commit(&run(0)->gear,&saved),"older-cache stash setup failed");saved=run(0)->gear.record;
  memcpy(first,saved.items[0].id,sizeof(first));memcpy(second,saved.items[1].id,sizeof(second));
  if(!strcmp(case_name,"stash-browser-fallback"))notice_chat_visible=FALSE;
  messages=local_notices;touch(2,0);
  CHECK(local_notices==messages+4,"stash page did not reuse the four-row native notice layout");
  CHECK(strstr(local_notice_history[(notice_chat_visible?messages:messages+3)%4],"STASH 1/2"),"debrief cannot browse every stored item");
  CHECK(strstr(local_notice_history[(messages+2)%4],notice_chat_visible?"Unavailable":"Weapon:"),"unavailable item was hidden or fallback rows were reversed");
  CHECK(!memcmp(&saved,&run(0)->gear.record,sizeof(saved)),"reading stash changed loadout or item ownership");
  if(!strcmp(case_name,"stash-browser-view")||!strcmp(case_name,"stash-browser-fallback")) {
   touch(2,0);CHECK(strstr(local_notice_history[(notice_chat_visible?local_notices-4:local_notices-1)%4],"STASH 2/2"),"stash browser skipped an owned item");return 0;
  }
  if(!strcmp(case_name,"stash-browser-stale")) {
   CHECK(gear_commit(&run(0)->gear,&saved),"stash generation setup failed");
   units[0].unit.control_flags=1;touch(2,0);
   CHECK(!run(0)->discard_deadline&&run(0)->gear.record.count==2,"changed stash accepted an old page as a deletion preview");return 0;
  }
  units[0].unit.control_flags=1;touch(2,0);
  CHECK(run(0)->discard_deadline&&run(0)->gear.record.count==2&&!memcmp(run(0)->discard_id,first,sizeof(first)),"unavailable stored item cannot be deliberately reviewed for discard");
  touch(2,0);CHECK(run(0)->gear.record.count==2,"duplicate action discarded stored gear before confirmation delay");
  advance(30);
  if(!strcmp(case_name,"stash-browser-cancel")) {
   units[0].unit.control_flags=0;touch(2,0);CHECK(!run(0)->discard_deadline&&run(0)->gear.record.count==2,"next-page action did not cancel deletion preview");return 0;
  }
  if(!strcmp(case_name,"stash-browser-pending")||!strcmp(case_name,"stash-browser-expired")) {
   publish_failure=TRUE;touch(2,0);CHECK(run(0)->gear.pending&&run(0)->gear.record.count==1,"published stash deletion was forgotten");
   units[0].unit.control_flags=0;touch(2,0);CHECK(run(0)->gear.pending&&run(0)->discard_deadline,"paging cancelled a published deletion");
   if(!strcmp(case_name,"stash-browser-expired"))advance(301);
   publish_failure=FALSE;
  }
  touch(2,0);
  CHECK(run(0)->gear.record.count==1&&!memcmp(run(0)->gear.record.items[0].id,second,sizeof(second))&&!run(0)->discard_deadline&&!run(0)->gear.pending,"stash discard removed another item or failed to finish");
  extraction_reset();advance(1);touch(2,0);
  CHECK(run(0)->gear.record.count==1&&!memcmp(run(0)->gear.record.items[0].id,second,sizeof(second)),"discard failed to persist or changed surviving gear");return 0;
 }
 if(!strncmp(case_name,"debrief-",8)) {
  struct gear_record before;struct game_modes_inventory captured;long carried[GAME_MODES_INVENTORY_WEAPONS];
  short j,k;int messages;char page[256];struct extraction_run *r;
  strcpy(names[2].name,"extraction_debrief");extraction_reset();advance(1);
  CHECK(extraction_owns_map()&&extraction.debrief_name==2,"native debrief binding unavailable");
  if(!strcmp(case_name,"debrief-empty")) {
   touch(2,0);CHECK(run(0)->gear.record.count==0&&!run(0)->report.ready,"empty debrief created a reward");
   for(j=0;j<4;j++)CHECK(!strstr(local_notice_history[j],"RECOVERED"),"empty report claimed recovery");
   return 0;
  }
  if(!strcmp(case_name,"debrief-remote")) {
   touch(0,1);a=pickup(1,2);escape(1);before=run(1)->gear.record;messages=remote_notices;
   touch(2,1);CHECK(remote_notices==messages+4&&strstr(remote_notice_history[messages%4],"P3 | DEBRIEF")&&
    strstr(remote_notice_history[messages%4],"RECOVERED"),"remote personal report missing");
   CHECK(!memcmp(&before,&run(1)->gear.record,sizeof(before)),"remote report modified banking");
   verified=FALSE;touch(2,1);CHECK(strstr(last_remote_notice,"BLOCKED")&&!strstr(last_remote_notice,"RECOVERED"),"unverified remote read personal report");
   return 0;
  }
  players[0].statistics.kills[0]=2;players[0].statistics.deaths=1;
  touch(0,0);a=pickup(0,1);players[0].statistics.kills[0]=5;
  if(!strcmp(case_name,"debrief-battery")) {
   /* Native plasma pistol tags retain a magazine despite using age as charge. */
   weapon_tag.weapon.flags|=FLAG(_weapon_cannot_fire_at_maximum_age_bit);
   weapon_get(a)->weapon.age=.375f;
  }
  r=run(0);
  if(!strcmp(case_name,"debrief-pending")) {
   CHECK(game_modes_inventory_capture(player_handles[0],&captured),"report capture unavailable");
   memcpy(carried,units[0].unit.weapon_object_indices,sizeof(carried));
   CHECK(gear_prepare_escape(&r->gear,&captured,carried)&&game_modes_inventory_consume(player_handles[0],&captured,carried),"native escrow preparation failed");
   r->phase=_extraction_settling;publish_failure=TRUE;extraction_finish_return(r);
   CHECK(r->gear.record.phase==_gear_idle&&r->gear.pending&&r->report.staged&&!r->report.ready,"uncertain bank exposed a report or kept escrow unexpectedly");
   before=r->gear.record;touch(2,0);
   CHECK(!r->report.ready&&!memcmp(&before,&r->gear.record,sizeof(before)),"reading pending report changed settlement");
   messages=0;for(j=0;j<4;j++) {
    if(strstr(local_notice_history[j],"DEBRIEF PENDING"))messages++;
    CHECK(!strstr(local_notice_history[j],"LAST RUN RECOVERED"),"uncertain write reported confirmed recovery");
   }
   CHECK(messages,"pending debrief did not explain settlement");
   publish_failure=FALSE;advance(31);
   CHECK(r->report.ready&&r->report.weapon_count==1&&r->gear.record.count==1,"flush retry lost recovered report details or duplicated gear");
  } else {
   if(!strcmp(case_name,"debrief-stats-reset"))players[0].statistics.kills[0]=0;
   escape(0);
  }
  CHECK(r->report.ready&&r->report.outcome==_gear_escaped&&r->report.weapon_count==1,"confirmed recovery report missing");
  if(!strcmp(case_name,"debrief-loss")) {
   touch(0,0);players[0].statistics.deaths++;units[0].object.damage_flags=FLAG(_object_dead_bit);advance(1);
   CHECK(r->phase==_extraction_lost&&r->report.ready&&r->report.outcome==_gear_lost&&r->report.weapon_count==1,"confirmed ownership loss missing from report");
   CHECK(r->report.deaths==1&&!r->gear.record.count&&!r->gear.record.reserved_count,"death report awarded gear or miscounted deaths");
   units[0].object.damage_flags=0;
  } else if(!strcmp(case_name,"debrief-restart")) {
   extraction_reset();advance(1);r=run(0);touch(2,0);
   CHECK(!r->report.ready&&r->gear.record.outcome==_gear_escaped&&r->gear.record.count==1,"restart invented details or lost confirmed outcome");
   return 0;
  } else if(!strcmp(case_name,"debrief-field")) {
   units[0].object.position.x=20;messages=local_notices;touch(2,0);
   CHECK(local_notices>messages&&!r->report_page,"field report advanced pages");
   units[0].object.position.x=0;units[0].object.parent_object_index=dropship_handles[0];touch(2,0);
   CHECK(!r->report_page,"seated report advanced pages");return 0;
  } else if(!strcmp(case_name,"debrief-redeploy"))touch(0,0);
  else if(!strcmp(case_name,"debrief-owner")) {
   delta_session=TRUE;host_account[0]++;touch(2,0);
   CHECK(!r->report.ready&&r->gear.record.count==0,"new owner saw old owner's report/stash");return 0;
  }
  if(!strcmp(case_name,"debrief-auto")) {
   messages=local_notices;advance(1);
   CHECK(r->report.announced&&r->report_page==1&&local_notices==messages+4,"confirmed on-foot hub report not announced once");
   messages=local_notices;advance(2);CHECK(local_notices==messages,"auto debrief repeated");
  }
  if(!strcmp(case_name,"debrief-fallback")) {
   notice_chat_visible=FALSE;r->report_page=0;messages=local_notices;short hud_before=native_hud_notices;
   touch(2,0);CHECK(local_notices==messages+4&&native_hud_notices==hud_before+4,"offline/hidden-chat report lost native HUD fallback");
   CHECK(strstr(local_notice_history[(local_notices-1)%4],"DEBRIEF")&&strstr(local_notice_history[(local_notices-2)%4],"Secured:"),"HUD report read in reverse order");return 0;
  }
  r->report_page=0;before=r->gear.record;
  for(k=0;k<r->report.weapon_count+2;k++) {
   touch(2,0);page[0]=0;
   for(j=0;j<4;j++){CHECK(strlen(local_notice_history[j])<=62,"report row exceeds native remote-prefix budget");strcat(page,local_notice_history[j]);}
   CHECK(strstr(page,"DEBRIEF")&&strstr(page,"Loadout terminal"),"report page lost navigation/headline");
   if(k==1) {
    CHECK(strstr(page,"rifle"),"weapon name missing from report");
    if(!strcmp(case_name,"debrief-battery"))CHECK(strstr(page,"battery: 62%")&&!strstr(page,"ammo:"),"native battery weapon with placeholder magazine reported ammunition");
    else CHECK(strstr(page,"ammo"),"weapon ammunition missing from report");
   }
   if(k==r->report.weapon_count+1)CHECK(strstr(page,!strcmp(case_name,"debrief-stats-reset")?"unavailable":"eliminations")!=NULL,"personal combat report missing");
   CHECK(!memcmp(&before,&r->gear.record,sizeof(before)),"report viewing mutated gear transaction");
  }
  for(k=0;k<r->gear.record.count;k++) {
   messages=local_notices;touch(2,0);
   CHECK(strstr(local_notice_history[messages%4],"STASH"),"owned stash pages did not follow the operation report");
   CHECK(!memcmp(&before,&r->gear.record,sizeof(before)),"browsing stash pages changed equipment");
  }
  CHECK(!r->report_page,"combined report/stash pages did not wrap");
  if(!strcmp(case_name,"debrief-stats-reset"))CHECK(!r->report.statistics,"native stat reset produced invented totals");
  extraction_reset();return 0;
 }
 if(!strncmp(case_name,"seat-exit-",10)) {
  short expected=1;long old=players[0].unit_index;
  exit_unarmed_supported=FALSE;
  if(!strcmp(case_name,"seat-exit-supported")){exit_unarmed_supported=TRUE;expected=0;}
  if(!strcmp(case_name,"seat-exit-armed")){pickup(0,1);expected=0;}
  if(!strcmp(case_name,"seat-exit-npc")){units[0].unit.player_index=NONE;expected=0;}
  if(!strcmp(case_name,"seat-exit-attached")){units[0].object.parent_object_index=dropship_handles[0];expected=0;}
  if(!strcmp(case_name,"seat-exit-dead")){units[0].object.damage_flags=FLAG(_object_dead_bit);expected=0;}
  if(!strcmp(case_name,"seat-exit-pending")){units[0].object.damage_flags=FLAG(_object_die_act_of_god_bit);expected=0;}
  if(!strcmp(case_name,"seat-exit-pending-silent")){units[0].object.damage_flags=FLAG(_object_die_act_of_god_silent_bit);expected=0;}
  if(!strcmp(case_name,"seat-exit-pending-no-stats")){units[0].object.damage_flags=FLAG(_object_die_act_of_god_no_statistics_bit);expected=0;}
  if(!strcmp(case_name,"seat-exit-no-stand"))exit_stand_supported=FALSE;
  short weapons_before=weapon_count;long inventory[4];memcpy(inventory,units[0].unit.weapon_object_indices,sizeof(inventory));
  unit_adjust_for_seat_change(old);
  CHECK(exit_ready_calls==1&&exit_pose_fallbacks==expected,"native exit fallback changed an armed/NPC/dead/attached/supported unit or failed its unarmed player");
  CHECK(exit_pose_queries==(expected||!strcmp(case_name,"seat-exit-supported")),"ineligible exit queried new pose");
  CHECK(exit_ground_pose==(expected&&exit_stand_supported),"missing graph manufactured a pose or valid stand was not selected");
  CHECK(weapon_count==weapons_before&&!memcmp(inventory,units[0].unit.weapon_object_indices,sizeof(inventory))&&players[0].unit_index==old,"exit fallback created, lost or replaced equipment/body");
  return 0;
 }
 if(!strncmp(case_name,"seat-entrance-",14)) {
  real_point3d entry={1,2,3},seat={1,2,3},hint={1,2,3};boolean expected;
  dropships[0].definition_index=300;dropship_tag.unit.seats.count=1;dropship_tag.unit.seats.address=&native_seat;
  strcpy(native_seat.label,"CD-passengerL01");strcpy(native_seat.marker_name,"CD-passengerL01");
  strcpy(native_animation_seat.label,native_seat.label);
  native_animation_seat.animations.count=12;native_animation_seat.animations.address=native_animation_indices;
  for(i=0;i<12;i++)native_animation_indices[i].animation_index=NONE;
  native_animation_indices[_unit_seat_animation_seat_exit].animation_index=0;
  native_animations[0].frame_count=native_animations[1].frame_count=10;
  native_animation_graph.unit_seats=(struct tag_block){1,&native_animation_seat};
  native_animation_graph.animations=(struct tag_block){2,native_animations};
  if(!strcmp(case_name,"seat-entrance-normal")||!strcmp(case_name,"seat-entrance-normal-marker"))native_animation_indices[_unit_seat_animation_seat_enter].animation_index=1;
  if(!strcmp(case_name,"seat-entrance-npc"))units[0].unit.player_index=NONE;
  if(!strcmp(case_name,"seat-entrance-corrupt"))native_animation_indices[_unit_seat_animation_seat_enter].animation_index=2;
  if(!strcmp(case_name,"seat-entrance-missing"))native_animation_indices[_unit_seat_animation_seat_exit].animation_index=NONE;
  if(!strcmp(case_name,"seat-entrance-zero"))native_animations[0].frame_count=0;
  if(!strcmp(case_name,"seat-entrance-marker")||!strcmp(case_name,"seat-entrance-normal-marker"))native_seat_marker=FALSE;
  expected=!strcmp(case_name,"seat-entrance-exit")||!strcmp(case_name,"seat-entrance-normal")||!strcmp(case_name,"seat-entrance-normal-marker");
  CHECK(unit_get_seat_entrance_point(unit_handles[0],dropship_handles[0],0,&entry,&seat,&hint)==expected,"native exit-only admission differs");
  if(expected)CHECK(observed_root_frame==(!strncmp(case_name,"seat-entrance-normal",20)?0:9)&&
   entry.x==10+observed_root_frame&&seat.x==10&&hint.x==10,"native entrance used the wrong endpoint/marker");
  else CHECK(entry.x==1&&seat.x==1&&hint.x==1&&observed_root_frame==NONE,"refused seat changed output or read invalid frames");
  extraction_reset();return 0;
 }
 if(!strncmp(case_name,"body-",5)) {
  long old=players[0].unit_index,b,body;struct unit_datum *unit;short i,profile,variant;long definition;
  byte saved_id[GEAR_ID_BYTES];unsigned long generation;
  if(!strcmp(case_name,"body-stash")) {touch(0,0);pickup(0,1);escape(0);memcpy(saved_id,run(0)->gear.record.items[0].id,GEAR_ID_BYTES);}
  enable_factions(FALSE);scenario.object_names.count=9;
  scenario.bipeds.count=scenario.biped_palette.count=2;scenario.bipeds.address=body_placements;scenario.biped_palette.address=body_palette;
  for(i=0;i<2;i++) {body_placements[i].name_index=7+i;body_placements[i].palette_entry_index=i;
   body_placements[i].placement_flags=FLAG(_scenario_object_placement_not_automatic_bit);body_palette[i].reference.index=4+i;}
  if(!strncmp(case_name,"body-native-",12))names[7].name[0]=0;
  if(!strcmp(case_name,"body-binding"))names[8].runtime_object_type=_object_type_vehicle;
  if(!strcmp(case_name,"body-placement"))body_placements[1].placement_flags=0;
  if(!strcmp(case_name,"body-palette"))body_placements[1].palette_entry_index=2;
  if(!strcmp(case_name,"body-duplicate"))strcpy(names[7].name,names[8].name);
  if(!strcmp(case_name,"body-profile"))scenario.starting_profiles.count=1;
  advance(1);
  if(!strcmp(case_name,"body-binding")||!strcmp(case_name,"body-placement")||!strcmp(case_name,"body-palette")||
     !strcmp(case_name,"body-duplicate")||!strcmp(case_name,"body-profile")) {
   CHECK(!extraction_owns_map()&&!body_creations,"bad character binding admitted or created a body");return 0;
  }
  CHECK(extraction_owns_map(),"valid native character bindings refused");
  if(!strncmp(case_name,"body-native-",12)) {
   a=pickup(0,1);
   touch(3,0);
   CHECK(players[0].unit_index==old&&players[0].team_index==_game_team_human&&!body_creations,
         "choosing the existing native body replaced it");
   touch(4,0);
   body=players[0].unit_index;
   CHECK(body!=old&&unit_get(body)->definition_index==5&&players[0].team_index==_game_team_covenant,
         "the explicit Covenant body was not possessed");
   if(!strcmp(case_name,"body-native-missing"))globals.player_information.count=0;
   touch(3,0);
   unit=unit_get(players[0].unit_index);
   if(!strcmp(case_name,"body-native-missing")) {
    CHECK(players[0].unit_index==body&&players[0].team_index==_game_team_covenant,
          "a missing default body changed native faction anyway");
   } else {
    CHECK(players[0].unit_index!=body&&unit->definition_index==4&&players[0].team_index==_game_team_human,
          "choosing UNSC retained the Covenant body");
    CHECK(!extraction_player_spawn_profile(player_handles[0],&definition,&profile,&variant),
          "a fallback faction stopped using the native respawn body");
   }
   CHECK(unit->unit.weapon_object_indices[0]==a&&weapon_try_and_get(a)&&
         run(0)->gear.record.phase==_gear_idle&&!run(0)->gear.record.reserved_count,
         "body selection erased carried equipment or reserved gear");
   return 0;
  }
  if(!strcmp(case_name,"body-armor-color")) {
   touch(4,0);touch(4,1);
   for(i=0;i<2;i++) {
    real_rgb_color selected;
    unit=unit_get(players[i].unit_index);
    game_engine_player_get_change_color(&selected,player_handles[i]);
    CHECK(unit->definition_index==5&&unit->unit.player_index==player_handles[i],"Covenant model lost its player");
    CHECK(unit->object.base_change_colors[0].r==selected.r&&unit->object.outgoing_change_colors[0].b==selected.b,
          "faction body did not receive its player's individual color");
    CHECK(!unit->object.base_change_colors[1].r&&!unit->object.outgoing_change_colors[1].b,
          "personal armor color replaced an authored secondary color channel");
   }
   return 0;
  }
  if(!strcmp(case_name,"body-stash")) {
   incompatible_body_weapon=1;touch(4,0);
   CHECK(players[0].unit_index!=old&&run(0)->gear.record.count==1&&!run(0)->gear.record.reserved_count&&
    !memcmp(saved_id,run(0)->gear.record.items[0].id,GEAR_ID_BYTES),"character selection lost/reserved incompatible stash");
   CHECK(unit_get(players[0].unit_index)->definition_index==5,"compatible kit could not change character");
   touch(0,0);CHECK(run(0)->gear.record.count==1&&!run(0)->gear.record.reserved_count,"incompatible stash was reserved for a different body");
   escape(0);CHECK(run(0)->gear.record.count==2&&gear_has(run(0)->gear.record.items,run(0)->gear.record.count,saved_id),"character run overwrote unselected stored gear");
   touch(3,0);CHECK(unit_get(players[0].unit_index)->definition_index==4,"UNSC reselect failed");
   touch(0,0);CHECK(gear_has(run(0)->gear.record.reserved,run(0)->gear.record.reserved_count,saved_id),"compatible body could not redeploy original stash item");
   escape(0);CHECK(run(0)->gear.record.count==2&&gear_has(run(0)->gear.record.items,run(0)->gear.record.count,saved_id),"recovered original was duplicated/lost");
   extraction_reset();return 0;
  }
  a=pickup(0,1);b=pickup(0,2);units[0].unit.current_weapon_index=1;
  units[0].object.body_vitality=.4f;units[0].object.shield_vitality=.3f;
  units[0].unit.grenade_counts[0]=2;units[0].unit.grenade_counts[1]=1;
  units[0].unit.flags=FLAG(_unit_active_camouflaged_bit)|FLAG(_unit_super_camouflaged_bit);
  units[0].unit.active_camouflage=.6f;players[0].powerup_durations[0]=300;
  if(!strcmp(case_name,"body-local"))connection=_game_connection_local;
  if(!strcmp(case_name,"body-allocation"))fail_body_create=1;
  if(!strcmp(case_name,"body-preview"))unusable_tag=2;
  if(!strcmp(case_name,"body-transfer"))fail_attach=1;
  if(!strcmp(case_name,"body-flag"))weapons[0].flag=TRUE;
  if(!strcmp(case_name,"body-item"))units[0].unit.equipment_object_index=0x700;
  if(!strncmp(case_name,"body-region-",12)) {
   scenario.structure_bsp_references.count=11;
   fixture_unit_bsp=fixture_control_bsp=10;
   body_region_loaded=!strcmp(case_name,"body-region-loaded");
  }
  incompatible_body_weapon=1;touch(4,0);
  if(!strcmp(case_name,"body-region-unavailable")||!strcmp(case_name,"body-allocation")||!strcmp(case_name,"body-preview")||!strcmp(case_name,"body-transfer")||
     !strcmp(case_name,"body-flag")||!strcmp(case_name,"body-item")) {
   CHECK(players[0].unit_index==old&&players[0].team_index==_game_team_player&&!original_deleted[0],"failed model change replaced original player");
   CHECK(weapon_try_and_get(a)&&weapon_try_and_get(b)&&run(0)->gear.record.phase==_gear_idle&&!run(0)->gear.record.reserved_count,
    "failed model change erased originals or reserved gear");extraction_reset();return 0;
  }
  body=players[0].unit_index;unit=unit_get(body);
  if(!strcmp(case_name,"body-region-loaded"))
   CHECK(body_created_bsp==10&&activated_body==body,"character change did not attach and activate in the player's own region");
  CHECK(body!=old&&unit->definition_index==5&&unit->unit.player_index==player_handles[0]&&players[0].team_index==_game_team_covenant&&original_deleted[0],"native player did not adopt selected body");
  CHECK(weapon_get(a)->object.parent_object_index==NONE&&unit->unit.weapon_object_indices[0]==b&&weapon_get(b)->object.owner_player_index==player_handles[0],"character change copied, erased or misowned actual weapons");
  CHECK(weapon_get(a)->weapon.magazines[0].rounds_loaded==7&&weapon_get(b)->weapon.magazines[0].rounds_total==63&&
   unit->unit.desired_weapon_index==0,"character change refilled ammo or lost selected compatible weapon");
  CHECK(unit->object.body_vitality==.4f&&unit->object.shield_vitality==.3f&&unit->unit.grenade_counts[0]==2&&unit->unit.grenade_counts[1]==1&&
   unit->unit.active_camouflage==.6f&&TEST_FLAG(unit->unit.flags,_unit_super_camouflaged_bit)&&players[0].powerup_durations[0]==300,"character change healed, restocked or lost native powerup state");
  before=body_creations;generation=run(0)->gear.record.generation;touch(4,0);
  CHECK(players[0].unit_index==body&&body_creations==before&&run(0)->gear.record.generation==generation,"same selection recreated body or mutated stash");
  if(!strcmp(case_name,"body-spawn")) {
   definition=4;profile=NONE;variant=0;
   CHECK(extraction_player_spawn_profile(player_handles[0],&definition,&profile,&variant)&&definition==5&&profile==1,"respawn lost native faction body/profile");
   client=TRUE;connection=_game_connection_network_client;
   CHECK(!extraction_player_spawn_profile(player_handles[0],&definition,&profile,&variant),"client overrode native spawn profile");
   client=FALSE;connection=_game_connection_network_server;
  } else {
   touch(0,0);
   if(!strcmp(case_name,"body-region-loaded"))
    CHECK(weapon_created_bsp==10,"deployed weapon was not created in the player's own region");
   escape(0);CHECK(run(0)->gear.record.count==1,"selected character failed to bank native kit");
   memcpy(saved_id,run(0)->gear.record.items[0].id,GEAR_ID_BYTES);
   touch(3,0);CHECK(unit_get(players[0].unit_index)->definition_index==4,"UNSC body could not be restored");
   touch(0,0);CHECK(run(0)->gear.record.reserved_count==1&&!memcmp(saved_id,run(0)->gear.record.reserved[0].id,GEAR_ID_BYTES),"body change lost banked item identity");
   escape(0);CHECK(run(0)->gear.record.count==2&&!memcmp(saved_id,run(0)->gear.record.items[0].id,GEAR_ID_BYTES),"native role/deploy/bank loop duplicated or erased saved gear");
  }
  extraction_reset();return 0;
 }
 if(!strncmp(case_name,"faction-",8)) {
  struct gear_record saved;long old_count;
  if(!strcmp(case_name,"faction-saved")||!strcmp(case_name,"faction-confirm")||!strcmp(case_name,"faction-reserve")) {
   touch(0,0);a=pickup(0,1);weapon_get(a)->weapon.magazines[0].rounds_loaded=3;
   weapon_get(a)->weapon.magazines[0].rounds_total=37;escape(0);
  }
  enable_factions(!strcmp(case_name,"faction-transport")||!strcmp(case_name,"faction-transport-missing"));
  if(!strcmp(case_name,"faction-transport-missing"))names[4].name[0]=0;
  if(!strcmp(case_name,"faction-binding"))scenario.starting_profiles.count=1;
  if(!strcmp(case_name,"faction-duplicate"))strcpy(faction_profiles[1].name,faction_profiles[0].name);
  if(!strcmp(case_name,"faction-alias"))control_handles[3]=control_handles[0];
  advance(1);
  if(!strcmp(case_name,"faction-binding")||!strcmp(case_name,"faction-duplicate")||!strcmp(case_name,"faction-alias")) {
   CHECK(!extraction_owns_map(),"ambiguous/missing faction interface admitted");return 0;
  }
  CHECK(extraction_owns_map(),"valid native faction hub refused");
  if(!strcmp(case_name,"faction-default-deploy")) {
   CHECK(players[0].team_index==_game_team_player&&players[1].team_index==_game_team_player,
    "hub startup rewrote unselected player teams");
   touch(2,0);fail_write=TRUE;touch(0,0);
   CHECK(players[0].team_index==_game_team_player&&run(0)->phase==_extraction_hub,
    "failed deployment changed team");
   fail_write=FALSE;touch(0,0);
   CHECK(run(0)->phase==_extraction_deployed&&players[0].team_index==_game_team_human&&
    units[0].object.owner_team_index==_game_team_human&&players[1].team_index==_game_team_player&&team_updates==1,
    "confirmed default UNSC deployment did not match explicit UNSC allegiance");
   touch(4,1);touch(0,1);
   CHECK(run(1)->phase==_extraction_deployed&&players[1].team_index==_game_team_covenant&&
    units[1].object.owner_team_index==_game_team_covenant,"explicit Covenant deployment was overridden");
   return 0;
  }
  if(!strcmp(case_name,"faction-transport-missing")) {
   touch(4,0);
   CHECK(players[0].team_index==_game_team_player&&!file_count&&!weapon_count,"missing Spirit changed faction or reserved gear");
   touch(3,0);CHECK(players[0].team_index==_game_team_human,"available Pelican faction was refused");return 0;
  }
  if(!strcmp(case_name,"faction-invalid")) {
   faction_profiles[1].starting_health_modifier=1;touch(4,0);
   CHECK(players[0].team_index==_game_team_player&&!weapon_count,"unsupported vitality profile changed role or granted gear");
   faction_profiles[1].starting_health_modifier=0;faction_profiles[1].primary_weapon.rounds_loaded=21;touch(4,0);
   CHECK(players[0].team_index==_game_team_player&&!weapon_count,"over-capacity kit changed role");
   faction_profiles[1].primary_weapon.rounds_loaded=10;extra_grenades=TRUE;touch(4,0);
   CHECK(players[0].team_index==_game_team_player&&!weapon_count,"expanded grenades silently narrowed into a faction kit");
   extra_grenades=FALSE;faction_profiles[1].primary_weapon.weapon.index=NONE;touch(4,0);
   CHECK(players[0].team_index==_game_team_player&&!weapon_count,"missing kit tag changed role");return 0;
  }
  if(!strcmp(case_name,"faction-state")) {
   units[0].object.position.x=20;touch(4,0);CHECK(!team_updates,"field player changed faction");
   units[0].object.position.x=0;units[0].object.parent_object_index=55;touch(4,0);
   CHECK(!team_updates,"seated player changed faction");units[0].object.parent_object_index=NONE;
   units[0].unit.grenade_throw_state=1;touch(4,0);CHECK(!team_updates,"throwing player changed faction");
   units[0].unit.grenade_throw_state=_unit_grenade_throw_idle;
   device_groups[9].actual_value=0;touch(4,0);CHECK(!team_updates,"unpowered choice changed faction");
   device_groups[9].actual_value=1;
  }
  if(!strcmp(case_name,"faction-choice"))units[0].unit.actor_index=7;
  before=weapon_count;
  touch(4,0);touch(3,1);
  CHECK(players[0].team_index==_game_team_covenant&&units[0].object.owner_team_index==_game_team_covenant&&
   players[1].team_index==_game_team_human&&units[1].object.owner_team_index==_game_team_human,
   "per-player faction choice changed another player or missed native allegiance");
  CHECK(!control_updates[3]&&!control_updates[4]&&weapon_count==before,
   "selection toggled shared state or granted a starter kit");
  if(!strcmp(case_name,"faction-choice")) {
   CHECK(team_updates==2&&actor_team_updates==1,"native actor allegiance refresh not reused");return 0;
  }
  if(!strcmp(case_name,"faction-transport")) {
   units[0].object.parent_object_index=dropship_handles[1];units[0].unit.parent_seat_index=0;
   CHECK(extraction_player_dropship(&units[0])==dropship_handles[1],"Covenant choice did not select Spirit");
   touch(3,0);CHECK(players[0].team_index==_game_team_covenant,"seated passenger switched factions");
   units[0].object.parent_object_index=dropship_handles[0];
   CHECK(extraction_player_dropship(&units[0])==NONE,"Covenant choice boarded UNSC transport");return 0;
  }
  old_count=weapon_count;
  if(!strcmp(case_name,"faction-reserve")) {
   fail_write=TRUE;touch(0,0);CHECK(weapon_count==old_count&&run(0)->phase==_extraction_hub,
    "failed reservation issued starter equipment");return 0;
  }
  if(!strcmp(case_name,"faction-confirm")) {
   fail_confirm=TRUE;touch(0,0);
   CHECK(run(0)->phase==_extraction_cancelling&&run(0)->gear.record.phase==_gear_reserved,
    "failed confirmation issued a faction run");
   for(a=0;a<4;a++)CHECK(units[0].unit.weapon_object_indices[a]==NONE,"failed confirmation left kit or saved weapon usable");
   CHECK(!units[0].unit.grenade_counts[1],"failed confirmation left starter grenades");
   fail_confirm=fail_write=FALSE;advance(120);
   CHECK(run(0)->gear.record.phase==_gear_idle&&run(0)->gear.record.count==1,"saved weapon refund lost after kit rollback");return 0;
  }
  touch(0,0);CHECK(run(0)->phase==_extraction_deployed,"valid faction deployment refused");
  CHECK(weapon_get(units[0].unit.weapon_object_indices[!strcmp(case_name,"faction-saved")?1:0])->definition_index==2,
   "Covenant kit not granted through native inventory");
  CHECK(units[0].unit.grenade_counts[1]==2,"Covenant grenade kit missing");
  touch(3,0);CHECK(players[0].team_index==_game_team_covenant,"deployed player changed faction");
  if(!strcmp(case_name,"faction-kit")) {
   touch(0,1);CHECK(run(1)->phase==_extraction_deployed&&weapon_get(units[1].unit.weapon_object_indices[0])->definition_index==1,
    "UNSC player did not receive its separate native kit");
   CHECK(units[1].unit.grenade_counts[0]==1,"starter grenades stacked with currently carried grenades");return 0;
  }
  if(!strcmp(case_name,"faction-state"))return 0;
  CHECK(!strcmp(case_name,"faction-saved"),"unknown faction case");
  CHECK(run(0)->gear.record.reserved_count==1&&weapon_get(units[0].unit.weapon_object_indices[0])->definition_index==1,
   "starter kit displaced or reserved itself as owned gear");
  CHECK(weapon_get(units[0].unit.weapon_object_indices[0])->weapon.magazines[0].rounds_loaded==3&&
   weapon_get(units[0].unit.weapon_object_indices[0])->weapon.magazines[0].rounds_total==37,
   "starter kit overwrote saved weapon ammunition");
  escape(0);CHECK(run(0)->gear.record.count==2,"faction escape failed to bank owned and recovered equipment");
  saved=run(0)->gear.record;touch(0,0);CHECK(run(0)->phase==_extraction_deployed&&run(0)->gear.record.reserved_count==2,
   "faction equipment did not redeploy");
  CHECK(run(0)->gear.record.reserved_grenades[1]==2&&units[0].unit.grenade_counts[1]==2,
   "starter kit stacked with recovered grenades on redeployment");
  escape(0);CHECK(run(0)->gear.record.count==2&&!memcmp(saved.items[0].id,run(0)->gear.record.items[0].id,GEAR_ID_BYTES)&&
   !memcmp(saved.items[1].id,run(0)->gear.record.items[1].id,GEAR_ID_BYTES),"redeployment duplicated faction equipment identities");
  return 0;
 }
 if(!strncmp(case_name,"transport-",10)) {
  short faction=!strcmp(case_name,"transport-covenant")?1:0;
  long craft=dropship_handles[faction],loot;
  boolean split_routes=!strncmp(case_name,"transport-called-split",22);
  boolean managed_call=!strcmp(case_name,"transport-called-managed-detour");
  boolean stash_space=!strcmp(case_name,"transport-stash-space");
  if(stash_space) {
   struct gear_record filled;
   touch(0,0);pickup(0,1);escape(0);
   filled=run(0)->gear.record;
   CHECK(filled.count==1,"native stash seed missing");
   for(a=1;a<GEAR_CAPACITY;a++) {
    filled.items[a]=filled.items[0];
    filled.items[a].id[0]^=(byte)a;
   }
   filled.count=GEAR_CAPACITY;
   CHECK(gear_commit(&run(0)->gear,&filled),"could not seed near-capacity stash");
  }
  extraction_reset();scenario.object_names.count=5;
  if(!strncmp(case_name,"transport-called",16)) {
   scenario.device_groups.count=12;scenario.device_groups.address=authored_groups;
   strcpy(authored_groups[10].name,"extraction_pelican_called");
   strcpy(authored_groups[11].name,managed_call?"extraction_pelican_boarded":"extraction_spirit_called");
   device_groups[10].actual_value=device_groups[11].actual_value=1.f;
   if(split_routes) {
    scenario.object_names.count=11;scenario.trigger_volumes.count=4;
    names[1].name[0]=volumes[0].name[0]=0;
    for(a=5;a<9;a++)names[a].name[0]=0;
    strcpy(names[9].name,"extraction_pelican_activate");
    strcpy(names[10].name,"extraction_spirit_activate");
    strcpy(volumes[2].name,"extraction_pelican_zone");
    strcpy(volumes[3].name,"extraction_spirit_zone");
    controls[5].object.name_index=9;controls[6].object.name_index=10;
    controls[5].object.position.x=20;controls[6].object.position.x=30;
    controls[5].device.position_group_index=6;controls[6].device.position_group_index=7;
    controls[5].device.power_group_index=3;controls[6].device.power_group_index=4;
    if(!strcmp(case_name,"transport-called-split-missing"))volumes[3].name[0]=0;
    if(!strcmp(case_name,"transport-called-split-alias"))controls[6].device.power_group_index=10;
   }
   if(!strcmp(case_name,"transport-called-duplicate"))strcpy(authored_groups[11].name,authored_groups[10].name);
   if(!strcmp(case_name,"transport-called-capacity"))device_group_table.maximum_count=10;
   if(!strcmp(case_name,"transport-called-missing"))scenario.object_names.count=3;
   if(!strcmp(case_name,"transport-called-alias")) {
    strcpy(authored_groups[1].name,authored_groups[10].name);authored_groups[10].name[0]=0;
   }
  }
  if(!strcmp(case_name,"transport-admission-deployment")||!strcmp(case_name,"transport-admission-evacuation")||
     !strcmp(case_name,"transport-admission-pilot")||!strcmp(case_name,"transport-pilot-receipt")||
     !strcmp(case_name,"transport-callback-settlement-fault")||stash_space) {
   scenario.device_groups.count=12;scenario.device_groups.address=authored_groups;
   strcpy(authored_groups[10].name,"extraction_pelican_boarded");
   strcpy(authored_groups[11].name,"extraction_pelican_landed");
  }
  if(!strcmp(case_name,"transport-callback-recording")||!strcmp(case_name,"transport-callback-aborted-recording")||
     !strncmp(case_name,"transport-admission-recorded-",29)) {
   scenario.device_groups.count=11;scenario.device_groups.address=authored_groups;
   strcpy(authored_groups[10].name,"extraction_pelican_boarded");
  }
  if(!strncmp(case_name,"transport-boarded",17)) {
   scenario.device_groups.count=12;scenario.device_groups.address=authored_groups;
   strcpy(authored_groups[10].name,"extraction_pelican_boarded");
   strcpy(authored_groups[11].name,"extraction_spirit_boarded");
   device_groups[10].actual_value=device_groups[11].actual_value=1.f;
   if(!strncmp(case_name,"transport-boarded-squad-call",28)) {
    scenario.object_names.count=11;
    for(a=5;a<9;a++)names[a].name[0]=0;
    for(a=0;a<2;a++) {
     snprintf(names[9+a].name,32,"extraction_squad_%02d",(int)a+1);
     controls[5+a].object.name_index=9+a;
     controls[5+a].device.position_group_index=6+a;
     controls[5+a].device.power_group_index=3;
    }
   }
   if(!strcmp(case_name,"transport-boarded-duplicate"))strcpy(authored_groups[11].name,authored_groups[10].name);
   if(!strcmp(case_name,"transport-boarded-capacity"))device_group_table.maximum_count=10;
   if(!strcmp(case_name,"transport-boarded-missing"))scenario.object_names.count=3;
   if(!strcmp(case_name,"transport-boarded-alias")) {
    strcpy(authored_groups[0].name,authored_groups[10].name);authored_groups[10].name[0]=0;
   }
  }
  if(!strncmp(case_name,"transport-landed",16)||!strncmp(case_name,"transport-hover-",16)) {
   scenario.device_groups.count=12;scenario.device_groups.address=authored_groups;
   strcpy(authored_groups[10].name,"extraction_pelican_landed");
   strcpy(authored_groups[11].name,"extraction_spirit_landed");
   if(!strcmp(case_name,"transport-landed-alias")) {
    strcpy(authored_groups[0].name,authored_groups[10].name);authored_groups[10].name[0]=0;
   }
  }
  if(!strcmp(case_name,"transport-spirit-return")||!strcmp(case_name,"transport-pelican-return")) {
   boolean spirit=!strcmp(case_name,"transport-spirit-return");
   scenario.trigger_volumes.count=5;strcpy(volumes[4].name,spirit?"spirit_return":"pelican_return");
   scenario.device_groups.count=12;scenario.device_groups.address=authored_groups;
   strcpy(authored_groups[10].name,spirit?"extraction_spirit_boarded":"extraction_pelican_boarded");
   strcpy(authored_groups[11].name,spirit?"extraction_spirit_landed":"extraction_pelican_landed");
  }
  if(!strcmp(case_name,"transport-binding"))names[3].runtime_object_type=0;
  if(!strcmp(case_name,"transport-duplicate"))strcpy(names[4].name,"extraction_pelican");
  if(!strcmp(case_name,"transport-missing-faction"))scenario.object_names.count=4;
  for(a=0;a<2;a++) {
   dropships[a].definition_index=300;dropships[a].object.body_vitality=1;
   dropships[a].object.position.x=20;dropships[a].object.up.k=1;
   dropships[a].object.flags|=FLAG(_object_on_ground_bit);dropships[a].vehicle.on_ground_ticks=5;
  }
  if(!strncmp(case_name,"transport-hover-",16)) {
   short faction=!strcmp(case_name,"transport-hover-spirit");
   long craft=dropship_handles[faction];
   struct unit_datum *ship=&dropships[faction];
   short group=10+faction;
   strcpy(transport_scripts[0].name,faction?"gs_spirit_hover_field":"gs_pelican_hover_field");
   transport_scripts[0].script_type=_hs_script_static;
   transport_scripts[0].return_type=_hs_type_boolean;
   scenario.hs_scripts.count=1;scenario.hs_scripts.address=transport_scripts;
   players[faction].team_index=units[faction].object.owner_team_index=faction?_game_team_covenant:_game_team_human;
   human_plane_aircraft=craft;
   ship->object.parent_object_index=ship->object.first_child_object_index=NONE;
   ship->object.flags &= ~FLAG(_object_on_ground_bit);
   ship->vehicle.on_ground_ticks=0;
   ship->vehicle.flags=FLAG(_vehicle_hovering_bit);
   ship->object.position.x=0;
   advance(1); touch(0,faction);
   CHECK(run(faction)->phase==_extraction_deployed,"fixture could not deploy from the hub");
   CHECK(!device_groups[group].actual_value &&
         !unit_can_enter_seat(unit_handles[faction],craft,0,NULL),"field opt-in bypassed hub landing");
   ship->object.position.x=20;
   units[faction].object.parent_object_index=craft;units[faction].unit.parent_seat_index=0;
   recorded_aircraft=craft;advance(1);
   CHECK(!device_groups[group].actual_value&&!extraction_transport_can_disembark(&units[faction]),
         "active playback opened a hovering transfer");
   recorded_aircraft=NONE;advance(1);
   CHECK(extraction.dropship_hover_transfer[faction], "compiled hover marker was not bound");
   CHECK(device_groups[group].actual_value==1.f, "hover readiness was not published");
   CHECK(extraction_transport_can_disembark(&units[faction]) && extraction_transport_uses_hover_exit(&units[faction]),
         "authored field hover did not permit a native passenger transfer");
   CHECK(extraction_transport_passenger_prompt(&units[faction]) &&
     !wcscmp(extraction_transport_passenger_prompt(&units[faction]),L"Press %action to deploy"),
     "hover transfer did not display a bound deployment action");
   CHECK(run(faction)->field_reached,"hovering insertion did not mark field arrival");
   native_animation_graph.unit_seats=(struct tag_block){1,&native_animation_seat};
   native_animation_graph.animations=(struct tag_block){2,native_animations};
   native_animation_seat.animations=(struct tag_block){9,native_animation_indices};
   native_animation_indices[8].animation_index=1;
   native_animations[1].frame_count=4;
   wall=TRUE;
   CHECK(!unit_try_and_exit_seat(unit_handles[faction]) &&
         units[faction].unit.animation.state!=_unit_state_exiting_seat,
         "blocked exit started a hover disembarkation");
   wall=FALSE;
   CHECK(unit_try_and_exit_seat(unit_handles[faction]) &&
         units[faction].unit.animation.state==_unit_state_exiting_seat && observed_root_frame==3,
         "the native exit clip did not start after clearance");
   ship->vehicle.flags=0;advance(1);
   CHECK(!device_groups[group].actual_value && !extraction_transport_can_disembark(&units[faction]),
         "released hover left the passenger gate open");
   ship->vehicle.flags=FLAG(_vehicle_hovering_bit);
   human_plane_aircraft=NONE;advance(1);
   CHECK(!device_groups[group].actual_value && !extraction_transport_can_disembark(&units[faction]),
         "a vehicle without native hover opened the field gate");

   human_plane_aircraft=craft;
   unit_exit_seat_end(unit_handles[faction]);
   units[faction].unit.animation.state=_unit_state_idle;
   units[faction].object.position.x=20;
   touch(1,faction);advance(300);
   CHECK(run(faction)->phase==_extraction_ready &&
         unit_can_enter_seat(unit_handles[faction],craft,0,NULL),"a ready crew could not board at a field hover");
   units[faction].object.parent_object_index=craft;
   units[faction].unit.parent_seat_index=0;advance(1);
   CHECK(run(faction)->dropship==craft,"hover pickup failed to register physical boarding");
   recorded_aircraft=craft;ship->object.position.x=0;advance(1);
   CHECK(run(faction)->phase==_extraction_ready,"active return recording settled the run");
   recorded_aircraft=NONE;advance(1);
   CHECK(run(faction)->phase==_extraction_ready,"a hub hover settled a field-only route");
   ship->vehicle.flags=0;ship->vehicle.on_ground_ticks=5;
   ship->object.flags|=FLAG(_object_on_ground_bit);advance(1);
   CHECK(run(faction)->phase==_extraction_returned,"grounded return failed to settle the hover operation");
   return 0;
  }
  advance(1);
  if(!strcmp(case_name,"transport-landed-egress")) {
   native_animation_graph.unit_seats=(struct tag_block){1,&native_animation_seat};
   native_animation_seat.animations=(struct tag_block){9,native_animation_indices};
   native_animation_indices[8].animation_index=1;
   units[0].definition_index=4;units[0].unit.parent_seat_index=0;
   for(a=0;a<2;a++) {
    units[0].object.parent_object_index=dropship_handles[a];
    dropships[a].object.flags&=~FLAG(_object_on_ground_bit);
    CHECK(!unit_try_and_exit_seat(unit_handles[0]),"live passenger started exit while aircraft was airborne");
    dropships[a].object.flags|=FLAG(_object_on_ground_bit);
    if(a==0) {
     human_plane_aircraft=dropship_handles[a];
     short landed_group=extraction.dropship_landing_groups[a];
     advance(1);
     CHECK(!device_groups[landed_group].actual_value&&!unit_try_and_exit_seat(unit_handles[0]),
           "a field Pelican still rolling announced arrival or allowed exit");
     dropships[a].object.flags|=FLAG(_object_at_rest_bit);
     dropships[a].object.up.k=.929f;advance(1);
     CHECK(!device_groups[landed_group].actual_value&&!unit_try_and_exit_seat(unit_handles[0]),
           "an under-threshold resting Pelican announced arrival or allowed exit");
     dropships[a].object.up.k=.98f;advance(1);
     CHECK(device_groups[landed_group].actual_value==1.f,"a safely settled Pelican did not publish arrival");
    }
    CHECK(unit_try_and_exit_seat(unit_handles[0]),"grounded passenger exit was refused");
    units[0].unit.animation.state=_unit_state_idle;
   }
   dropships[1].object.flags&=~FLAG(_object_on_ground_bit);
   dropships[1].object.damage_flags=FLAG(_object_die_act_of_god_bit);
   CHECK(unit_try_and_exit_seat(unit_handles[0]),"failing aircraft trapped its passenger");
   dropships[1].object.damage_flags=0;units[0].unit.animation.state=_unit_state_idle;
   units[0].unit.player_index=NONE;
   CHECK(unit_try_and_exit_seat(unit_handles[0]),"authored NPC exit was refused");
   units[0].unit.player_index=player_handles[0];units[0].unit.animation.state=_unit_state_idle;
   extraction.dropship_landing_groups[1]=NONE;
   CHECK(unit_try_and_exit_seat(unit_handles[0]),"legacy authored exit was refused");
   units[0].object.type=_object_type_vehicle;
   units[0].unit.player_index=NONE;
   CHECK(unit_try_and_exit_seat(unit_handles[0]) && units[0].object.parent_object_index==NONE,
         "native exit detached the carried vehicle but reported failure");
   CHECK(!unit_try_and_exit_seat(unit_handles[0]),"native exit reported an already detached vehicle");
   return 0;
  }
  if(!strcmp(case_name,"transport-spirit-return")||!strcmp(case_name,"transport-pelican-return")) {
   short faction=!strcmp(case_name,"transport-spirit-return")?1:0;
   long transport=dropship_handles[faction];
   struct unit_datum *ship=&dropships[faction];
   strcpy(transport_scripts[0].name,faction?"forerunner_spirit_complete":"forerunner_pelican_complete");
   transport_scripts[0].script_type=_hs_script_dormant;transport_scripts[0].return_type=_hs_type_void;
   scenario.hs_scripts.count=1;scenario.hs_scripts.address=transport_scripts;
   players[faction].team_index=units[faction].object.owner_team_index=faction?_game_team_covenant:_game_team_human;
   CHECK(extraction_owns_map(),"authored aircraft return rejected the shared hub");
   CHECK(!extraction_transport_brake_at_pad(transport),"empty aircraft acquired evacuation braking");
   ship->object.position.x=7.f;ship->object.first_child_object_index=NONE;
   CHECK(!unit_can_enter_seat(unit_handles[faction],transport,0,NULL),"undeployed player boarded at the aircraft pad");
   touch(0,faction);
   recorded_aircraft=transport;advance(1);
   CHECK(!device_groups[10].actual_value&&!device_groups[11].actual_value &&
    !unit_can_enter_seat(unit_handles[faction],transport,0,NULL),"active recording allowed boarding at touchdown");
   units[faction].object.parent_object_index=transport;units[faction].unit.parent_seat_index=0;advance(1);
   CHECK(!device_groups[10].actual_value&&!extraction_transport_can_disembark(&units[faction]),
    "active recording released an occupied crew");
   units[faction].object.parent_object_index=NONE;
   recorded_aircraft=NONE;
   CHECK(unit_can_enter_seat(unit_handles[faction],transport,0,NULL),"deployed player could not board at its dedicated pad");
   units[faction].object.parent_object_index=transport;units[faction].unit.parent_seat_index=0;
   units[faction].unit.animation.state=_unit_state_entering_seat;advance(1);
   CHECK(!device_groups[10].actual_value&&device_groups[11].actual_value==1.f,
    "aircraft released insertion before its passenger finished entering the seat");
   units[faction].unit.animation.state=_unit_state_idle;advance(1);
   CHECK(device_groups[10].actual_value==1.f,"physical passenger did not release aircraft insertion at its dedicated pad");
   CHECK(device_groups[11].actual_value==1.f,"ready aircraft did not publish landing");
   extraction_transport_recording_started(dropship_handles[!faction]);
   CHECK(device_groups[10].actual_value==1.f&&device_groups[11].actual_value==1.f,"other aircraft invalidated this crew");
   client=TRUE;extraction_transport_recording_started(transport);client=FALSE;
   CHECK(device_groups[10].actual_value==1.f&&device_groups[11].actual_value==1.f,"client invalidated host readiness");
   recorded_aircraft=transport;extraction_transport_recording_started(transport);
   CHECK(!device_groups[10].actual_value&&!device_groups[11].actual_value,"script could read stale readiness immediately after starting playback");
   advance(1);
   CHECK(!device_groups[10].actual_value&&!device_groups[11].actual_value,"grounded aircraft rearmed during playback");
   recorded_aircraft=NONE;advance(1);
   CHECK(device_groups[10].actual_value==1.f&&device_groups[11].actual_value==1.f,"aircraft did not rearm after native cleanup");
   ship->object.flags&=~FLAG(_object_on_ground_bit);advance(1);
   CHECK(!device_groups[10].actual_value&&!device_groups[11].actual_value,"aircraft output pair stayed ready after liftoff");
   ship->object.flags|=FLAG(_object_on_ground_bit);advance(1);
   CHECK(device_groups[10].actual_value==1.f&&device_groups[11].actual_value==1.f,"aircraft output pair did not rearm on touchdown");
   units[faction].object.parent_object_index=NONE;
   units[faction].object.position.x=ship->object.position.x=20.f;
   pickup(faction,2);touch(1,faction);advance(300);
   CHECK(unit_can_enter_seat(unit_handles[faction],transport,0,NULL),"aircraft field pickup stopped working");
   units[faction].object.parent_object_index=transport;
   units[faction].unit.animation.state=_unit_state_entering_seat;advance(1);
   CHECK(run(faction)->dropship==NONE&&!device_groups[10].actual_value,
    "aircraft acquired an evacuation receipt before its passenger finished boarding");
   units[faction].unit.animation.state=_unit_state_idle;advance(1);
   CHECK(run(faction)->dropship==transport&&device_groups[10].actual_value==1.f,
    "aircraft field boarding did not attach to the operation");
   ship->object.position.x=0.f;advance(1);
   CHECK(run(faction)->phase==_extraction_ready&&!transport_wakes[faction],"aircraft banked at the shared hub outside its dedicated pad");
   CHECK(!extraction_transport_brake_at_pad(transport),"aircraft braked outside its dedicated pad");
   ship->object.position.x=7.f;
   CHECK(extraction_transport_brake_at_pad(transport),"occupied aircraft could not brake at its dedicated pad");
   recorded_aircraft=transport;advance(1);
   CHECK(!extraction_transport_brake_at_pad(transport) && !device_groups[11].actual_value &&
    run(faction)->phase==_extraction_ready && !transport_wakes[faction],"landing bypassed the recorded return handoff");
   recorded_aircraft=NONE;
   units[faction].object.parent_object_index=NONE;
   CHECK(!extraction_transport_brake_at_pad(transport),"disembarked passenger kept the brake active");
   units[faction].object.parent_object_index=transport;
   ship->object.up.k=.94f;advance(1);
   CHECK(run(faction)->phase==_extraction_ready&&!transport_wakes[faction],"aircraft banked before native landing");
   ship->object.up.k=1.f;native_cinematic=TRUE;advance(1);
   CHECK(run(faction)->phase==_extraction_ready&&!transport_wakes[faction],"aircraft banked during the cinematic");
   native_cinematic=FALSE;
   ship->object.translational_velocity.i=.007f;advance(1);
   CHECK(run(faction)->phase==_extraction_ready&&!transport_wakes[faction],"rolling in the pad bypassed native landing");
   ship->unit.control_flags=FLAG(_unit_control_crouch_modifier_bit);
   for(a=0;a<3;a++) {
    real_vector3d force={0.f,0.f,1.f};
    vehicle_apply_return_pad_brake(transport,ship,1000.f,&force);
    CHECK(force.i<0.f && force.k==1.f,"brake failed to oppose the roll without altering lift");
    ship->object.translational_velocity.i+=force.i/1000.f;
    advance(1);
    if(a<2)CHECK(run(faction)->phase==_extraction_ready,"braking bypassed native landing speed");
   }
   CHECK(run(faction)->phase==_extraction_returned&&transport_wakes[faction]==1&&run(faction)->gear.record.count==1,
    "aircraft arrival did not bank recovered gear and release its passengers");
   CHECK(!extraction_transport_brake_at_pad(transport),"settled evacuation left the brake active");
   ship->object.up.k=.947f;
   ship->object.flags|=FLAG(_object_at_rest_bit);
   ship->object.translational_velocity.i=.0001f;advance(1);
   CHECK(device_groups[11].actual_value==1.f&&extraction_transport_can_disembark(&units[faction]),
    "a banked crew lost egress when the grounded aircraft settled on its return pad");
   units[faction].object.parent_object_index=NONE;units[faction].object.position.x=7.f;
   touch(0,faction);CHECK(run(faction)->phase==_extraction_returned,"aircraft pad became a foot deployment hub");
   units[faction].object.position.x=0.f;touch(0,faction);
   CHECK(run(faction)->phase==_extraction_deployed,"shared hub stopped admitting redeployment");
   CHECK(unit_can_enter_seat(unit_handles[faction],transport,0,NULL),
    "second deployment cannot board the same completed return stop");
   units[faction].object.parent_object_index=transport;
   advance(1);
   CHECK(device_groups[10].actual_value==1.f,"a physically seated new crew cannot depart the completed stop");

   ship->object.flags&=~FLAG(_object_on_ground_bit);advance(1);
   CHECK(!device_groups[11].actual_value&&!extraction_transport_can_disembark(&units[faction]),
    "completed return allowed airborne exit");
   ship->object.flags|=FLAG(_object_on_ground_bit);
   ship->object.position.x=0.f;advance(1);
   CHECK(!device_groups[11].actual_value&&!extraction_transport_can_disembark(&units[faction]),
    "completed return allowed slope tolerance away from its pad");
   ship->object.position.x=7.f;
   recorded_aircraft=transport;advance(1);
   CHECK(!device_groups[11].actual_value&&!extraction_transport_can_disembark(&units[faction]),
    "completed return allowed exit before native playback cleanup");
   recorded_aircraft=NONE;
   ship->object.up.k=.93f;advance(1);
   CHECK(!device_groups[11].actual_value&&!extraction_transport_can_disembark(&units[faction]),
    "completed return allowed exit from an excessively tilted hull");
   ship->object.up.k=.947f;advance(1);
   CHECK(!device_groups[11].actual_value&&!extraction_transport_can_disembark(&units[faction]),
    "new flight reused a prior stop's tolerance");
   extraction_reset();return 0;
  }
  if(!strcmp(case_name,"transport-landed-alias")) {
   CHECK(!extraction_owns_map()&&!boarding_changes,"landing output aliased a player input");return 0;
  }
  if(!strncmp(case_name,"transport-landed",16)) {
   CHECK(device_groups[10].actual_value==1.f,"physical landing not published");
   dropships[0].object.damage_flags=FLAG(_object_die_act_of_god_bit);advance(1);
   CHECK(!device_groups[10].actual_value,"pending destruction reported landing");
   dropships[0].object.damage_flags=0;
   dropships[0].vehicle.on_ground_ticks=4;advance(1);
   CHECK(!device_groups[10].actual_value,"transient contact reported landing");
   dropships[0].vehicle.on_ground_ticks=5;dropships[0].object.up.k=.94f;advance(1);
   CHECK(!device_groups[10].actual_value,"tilted craft reported landing");
   dropships[0].object.up.k=1;dropships[0].object.translational_velocity.i=.006f;advance(1);
   CHECK(!device_groups[10].actual_value,"sliding craft reported landing");
   dropships[0].object.translational_velocity.i=0;dropships[0].object.angular_velocity.k=.02f;advance(1);
   CHECK(!device_groups[10].actual_value,"rotating craft reported landing");
   dropships[0].object.angular_velocity.k=0;
   dropships[0].vehicle.flags=FLAG(_vehicle_hovering_bit);dropships[0].vehicle.on_ground_ticks=0;
   advance(1);CHECK(device_groups[10].actual_value==1.f,"native grounded parking lost landing feedback");
   dropships[0].object.flags&=~FLAG(_object_on_ground_bit);advance(1);
   CHECK(!device_groups[10].actual_value,"airborne contact retained landing output");
   dropships[0].vehicle.flags=0;dropships[0].vehicle.on_ground_ticks=5;
   dropships[0].object.flags|=FLAG(_object_on_ground_bit);
  }
  if(!strncmp(case_name,"transport-called",16)) {
   if(!strcmp(case_name,"transport-called-duplicate")||!strcmp(case_name,"transport-called-capacity")||
      !strcmp(case_name,"transport-called-missing")||!strcmp(case_name,"transport-called-alias")||
      !strcmp(case_name,"transport-called-split-missing")||!strcmp(case_name,"transport-called-split-alias")) {
    CHECK(!extraction_owns_map()&&!boarding_changes,"invalid call output admitted map or changed native groups");return 0;
   }
   CHECK(extraction_owns_map()&&!device_groups[10].actual_value&&!device_groups[11].actual_value,"stale call outputs survived binding");
   units[0].object.owner_team_index=_game_team_player;players[0].team_index=_game_team_player;
   units[1].object.owner_team_index=_game_team_covenant;players[1].team_index=_game_team_covenant;
   touch(0,0);touch(0,1);units[0].object.position.x=units[1].object.position.x=20;
   advance(1);CHECK(!device_groups[10].actual_value&&!device_groups[11].actual_value,"mere deployment called a dropship");
   if(split_routes) {
    touch(5,1);touch(6,1);advance(1);
    CHECK(!device_groups[10].actual_value&&!device_groups[11].actual_value,"Covenant called from the UNSC landing zone");
    touch(5,0);advance(1);
    CHECK(device_groups[10].actual_value&&!device_groups[11].actual_value,"Pelican terminal did not call only its craft");
    units[1].object.position.x=30;touch(6,1);advance(1);
    CHECK(device_groups[10].actual_value&&device_groups[11].actual_value,"Spirit terminal did not call its craft independently");
    units[0].object.position.x=30;advance(1);
    CHECK(!device_groups[10].actual_value&&device_groups[11].actual_value,"crossing landing zones did not cancel only the UNSC countdown");
    advance(300);
    CHECK(run(1)->phase==_extraction_ready,"Covenant activation did not finish");
    dropships[1].object.position.x=20;units[1].object.parent_object_index=dropship_handles[1];
    units[1].unit.parent_seat_index=0;advance(1);
    CHECK(run(1)->dropship==NONE&&!extraction_transport_crew_ready(1,dropship_handles[1]),"wrong landing zone produced a Spirit boarding receipt");
    dropships[1].object.position.x=30;advance(1);
    CHECK(run(1)->dropship==dropship_handles[1]&&extraction_transport_crew_ready(1,dropship_handles[1]),"correct Spirit landing zone refused boarding");
    dropships[1].object.position.x=0;advance(1);
    CHECK(run(1)->phase==_extraction_returned&&run(0)->phase==_extraction_deployed,"Spirit arrival did not settle only its passengers");
    extraction_reset();return 0;
   }
   touch(1,0);advance(1);
   CHECK(device_groups[10].actual_value==1.f&&!device_groups[11].actual_value,"UNSC request called Spirit or missed Pelican");
   if(managed_call) {
    units[0].object.position.x=7;advance(1);
    CHECK(run(0)->phase==_extraction_deployed&&!device_groups[10].actual_value,
     "unfinished managed countdown survived leaving the landing zone");
    units[0].object.position.x=20;touch(1,0);advance(300);
    CHECK(run(0)->phase==_extraction_ready&&device_groups[10].actual_value,
     "completed managed countdown failed to call the craft");
    units[0].object.position.x=7;advance(1);
    CHECK(device_groups[10].actual_value&&!device_groups[11].actual_value,
     "completed pickup was cancelled by an on-foot approach detour");
    units[0].object.parent_object_index=dropship_handles[1];advance(1);
    CHECK(device_groups[10].actual_value&&!device_groups[11].actual_value,
     "travel in a different vehicle cancelled the pickup or authorized boarding");
    units[0].object.parent_object_index=NONE;
    medical_enabled=TRUE;units[0].object.body_vitality=-.1f;
    CHECK(extraction_medical_lethal(unit_handles[0]),"pickup caller could not enter native incapacitation");
    advance(1);
    CHECK(!device_groups[10].actual_value&&run(0)->phase==_extraction_ready&&run(0)->gear.record.phase==_gear_deployed,
     "downed player kept the aircraft called or lost their completed activation");
    units[0].object.body_vitality=.25f;advance(1);
    CHECK(device_groups[10].actual_value&&!device_groups[11].actual_value,
     "revived player could not resume the completed pickup");
    players[0].quit_out_of_game=TRUE;advance(1);
    CHECK(!device_groups[10].actual_value&&!device_groups[11].actual_value&&
     run(1)->phase==_extraction_deployed,"departed requester held the aircraft or changed another run");
    extraction_reset();return 0;
   }
   /* Both players touch the same native switch. Its position must not choose
    * a faction or cancel the other player's active request. */
   touch(1,1);advance(1);
   CHECK(device_groups[10].actual_value==1.f&&device_groups[11].actual_value==1.f,"shared switch lost independent faction calls");
   if(!strcmp(case_name,"transport-called-ownership")) {
    players[0].team_index=_game_team_covenant;extraction_transport_readiness();
    CHECK(!device_groups[10].actual_value&&device_groups[11].actual_value==1.f,"stale player faction called a transport");
    players[0].team_index=_game_team_player;units[0].unit.player_index=NONE;extraction_transport_readiness();
    CHECK(!device_groups[10].actual_value,"unowned native body called a transport");
    units[0].unit.player_index=player_handles[0];run(0)->gear.pending=TRUE;extraction_transport_readiness();
    CHECK(!device_groups[10].actual_value,"unconfirmed gear called a transport");
    run(0)->gear.pending=FALSE;units[0].object.damage_flags=FLAG(_object_die_act_of_god_bit);extraction_transport_readiness();
    CHECK(!device_groups[10].actual_value,"pending death called a transport");
    units[0].object.damage_flags=0;players[0].quit_out_of_game=TRUE;extraction_transport_readiness();
    CHECK(!device_groups[10].actual_value,"departed player called a transport");
    players[0].quit_out_of_game=FALSE;
   } else {
    units[0].object.position.x=7;advance(1);
    CHECK(!device_groups[10].actual_value&&device_groups[11].actual_value==1.f,"cancelled request recalled wrong craft");
    advance(300);CHECK(run(1)->phase==_extraction_ready&&device_groups[11].actual_value==1.f,"finished activation lost the pending Spirit request");
    units[1].object.parent_object_index=dropship_handles[1];units[1].unit.parent_seat_index=0;advance(1);
    CHECK(device_groups[11].actual_value==1.f,"real LZ boarding lost the call before the flight script could consume it");
    dropships[1].object.position.x=7;advance(1);
    CHECK(!device_groups[11].actual_value&&run(1)->gear.record.phase==_gear_deployed,"flight retained call or banked before arrival");
    dropships[1].object.position.x=0;advance(1);
    CHECK(run(1)->phase==_extraction_returned&&!device_groups[10].actual_value&&!device_groups[11].actual_value,"banked crew retained a recall signal");
    units[1].object.parent_object_index=NONE;units[1].object.position.x=0;touch(0,1);
    CHECK(run(1)->phase==_extraction_deployed,"settled crew could not redeploy");
    units[1].object.position.x=20;touch(1,1);advance(1);
    CHECK(device_groups[11].actual_value==1.f&&!device_groups[10].actual_value,"next operation failed to rearm its faction call");
   }
   extraction_reset();return 0;
  }
  if(!strcmp(case_name,"transport-boarded-duplicate")||!strcmp(case_name,"transport-boarded-capacity")||
     !strcmp(case_name,"transport-boarded-missing")||!strcmp(case_name,"transport-boarded-alias")) {
   CHECK(!extraction_owns_map()&&!boarding_changes,"invalid output binding changed native controls or admitted the hub");return 0;
  }
  if(stash_space) {
   struct extraction_run *r=run(0);
   unsigned long generation;
   long live_count=weapon_count;
   players[0].team_index=units[0].object.owner_team_index=_game_team_player;
   dropships[0].object.first_child_object_index=units[0].object.first_child_object_index=NONE;
   touch(2,0);generation=r->gear.record.generation;
   touch(0,0);
   CHECK(r->phase==_extraction_hub&&r->gear.record.phase==_gear_idle&&
    r->gear.record.count==GEAR_CAPACITY&&r->gear.record.generation==generation&&
    live_count==weapon_count,"full stash began a managed flight or changed ownership");
   for(a=0;a<2;a++) {
    units[0].unit.control_flags=FLAG(_unit_control_crouch_modifier_bit);
    touch(2,0);advance(30);touch(2,0);
   }
   units[0].unit.control_flags=0;
   CHECK(r->gear.record.count==GEAR_CAPACITY-2,"hub discard did not make room");
   CHECK(!extraction_deploy(r,&players[0],FALSE)&&r->gear.record.count==GEAR_CAPACITY-2,
    "safe-stash deployment counted saved weapons that were not chosen");
   touch(0,0);
   CHECK(r->phase==_extraction_deployed&&r->gear.record.reserved_count==2&&
    r->gear.record.count+GAME_MODES_INVENTORY_WEAPONS==GEAR_CAPACITY,"legal reservation did not create exactly enough room");
   pickup(0,1);pickup(0,2);
   units[0].object.position.x=20;touch(1,0);advance(300);
   CHECK(unit_can_enter_seat(unit_handles[0],craft,0,NULL),"capacity-safe passenger was refused");
   units[0].object.parent_object_index=craft;units[0].unit.parent_seat_index=0;advance(1);
   CHECK(device_groups[10].actual_value==1.f,"capacity-safe boarded crew could not depart");
   dropships[0].object.position.x=0;advance(1);
   CHECK(r->phase==_extraction_returned&&r->gear.record.count==GEAR_CAPACITY&&
    !r->gear.record.escrow_count,"largest native inventory did not safely fill the stash on return");
   return 0;
  }
  if(!strncmp(case_name,"transport-admission-",20)) {
   long occupant=77;
   for(a=0;a<2;a++){dropships[a].object.first_child_object_index=NONE;units[a].object.first_child_object_index=NONE;}
   players[0].team_index=_game_team_player;units[0].object.owner_team_index=_game_team_player;
   if(!strcmp(case_name,"transport-admission-npc")) {
    units[0].unit.player_index=NONE;units[0].object.owner_team_index=_game_team_covenant;
    CHECK(unit_can_enter_seat(unit_handles[0],dropship_handles[0],0,NULL)&&unit_can_enter_seat(unit_handles[0],dropship_handles[1],0,NULL),"authored NPC pilot admission changed");
   } else if(!strcmp(case_name,"transport-admission-authority")) {
    connection=_game_connection_network_client;
    CHECK(unit_can_enter_seat(unit_handles[0],dropship_handles[1],0,NULL),"client rejected authoritative seat replay");
    connection=_game_connection_network_server;enabled=FALSE;
    CHECK(unit_can_enter_seat(unit_handles[0],dropship_handles[1],0,NULL),"disabled Extraction changed native admission");
    dropships[1].object.type=_object_type_vehicle;
    parked_region_vehicle=dropship_handles[1];
    CHECK(!unit_can_enter_seat(unit_handles[0],dropship_handles[1],0,&occupant)&&occupant==NONE,
     "player could board a parked off-region vehicle");
    units[0].unit.player_index=NONE;
    occupant=77;
    CHECK(!unit_can_enter_seat(unit_handles[0],dropship_handles[1],0,&occupant)&&occupant==NONE,
     "host could attach an NPC to a parked off-region vehicle");
    connection=_game_connection_network_client;
    CHECK(unit_can_enter_seat(unit_handles[0],dropship_handles[1],0,NULL),
     "a receiving peer refused an authoritative NPC attachment");
   } else if(!strcmp(case_name,"transport-admission-owner")) {
    players[0].unit_index=unit_handles[1];CHECK(!unit_can_enter_seat(unit_handles[0],craft,0,NULL),"stale player body boarded");
    players[0].unit_index=unit_handles[0];players[0].quit_out_of_game=TRUE;CHECK(!unit_can_enter_seat(unit_handles[0],craft,0,NULL),"departed owner boarded");
    players[0].quit_out_of_game=FALSE;units[0].unit.player_index=player_handles[0]+0x10000;
    CHECK(!unit_can_enter_seat(unit_handles[0],craft,0,NULL),"reused player salt boarded");
   } else if(!strcmp(case_name,"transport-admission-deployment")) {
    dropships[0].object.position.x=0;
    CHECK(!unit_can_enter_seat(unit_handles[0],craft,0,&occupant)&&occupant==NONE,
     "undeployed player occupied a managed insertion seat");
    touch(0,0);
    CHECK(unit_can_enter_seat(unit_handles[0],craft,0,NULL),"confirmed deployment could not board at the hub");
    CHECK(!unit_can_enter_seat(unit_handles[1],craft,1,NULL),"undeployed player took another crew member's seat");
    dropships[0].object.up.k=.94f;
    CHECK(!unit_can_enter_seat(unit_handles[0],craft,0,NULL),"invalid landing attitude admitted insertion boarding");
    dropships[0].object.up.k=1;
    native_cinematic=TRUE;
    CHECK(!unit_can_enter_seat(unit_handles[0],craft,0,NULL),"cinematic admitted new insertion boarding");
   } else if(!strcmp(case_name,"transport-admission-evacuation")) {
    touch(0,0);units[0].object.position.x=20;
    CHECK(!unit_can_enter_seat(unit_handles[0],craft,0,NULL),"field crew boarded without requesting extraction");
    touch(1,0);
    CHECK(!unit_can_enter_seat(unit_handles[0],craft,0,NULL),"unfinished countdown admitted evacuation boarding");
    advance(300);
    CHECK(unit_can_enter_seat(unit_handles[0],craft,0,NULL),"ready crew could not board at its landing zone");
    dropships[0].object.position.x=0;
    CHECK(!unit_can_enter_seat(unit_handles[0],craft,0,NULL),"ready field crew could acquire a seat at the destination");
    dropships[0].object.position.x=20;run(0)->gear.pending=TRUE;
    CHECK(!unit_can_enter_seat(unit_handles[0],craft,0,NULL),"pending gear admitted evacuation boarding");
   } else if(!strcmp(case_name,"transport-admission-recorded-entry")||
             !strcmp(case_name,"transport-admission-recorded-exit")) {
    boolean entry=!strcmp(case_name,"transport-admission-recorded-entry");
    short crew_group=extraction.dropship_boarding_groups[0];
    CHECK(crew_group!=NONE&&extraction.dropship_landing_groups[0]==NONE,"fixture needs only crew readiness");
    dropships[0].object.position.x=0;touch(0,0);
    CHECK(unit_can_enter_seat(unit_handles[0],craft,1,NULL),"boarding-only map refused initial passenger");
    recorded_aircraft=craft;
    if(entry) {
     CHECK(!unit_can_enter_seat(unit_handles[0],craft,1,NULL),"active recording offered a new passenger seat");
     extraction.dropship_boarding_groups[0]=NONE;
     CHECK(unit_can_enter_seat(unit_handles[0],craft,1,NULL),"unmanaged script path lost its native seat");
     extraction.dropship_boarding_groups[0]=crew_group;
     recorded_aircraft=NONE;
     CHECK(unit_can_enter_seat(unit_handles[0],craft,1,NULL),"finished playback did not reopen boarding");
    } else {
     int notices=local_notices;
     CHECK(notices>0&&strstr(local_notice_history[(notices-1)%4],"Board a Pelican passenger seat"),"deployment did not tell the player how to board");
     units[0].object.parent_object_index=craft;units[0].unit.parent_seat_index=1;
     CHECK(!extraction_transport_can_disembark(&units[0]),"active recording permitted voluntary exit");
     CHECK(extraction_transport_passenger_prompt(&units[0]) &&
       !wcscmp(extraction_transport_passenger_prompt(&units[0]),L"Stay seated until the exit opens"),
       "moving aircraft showed a seat exit action");
     dropships[0].object.position.x=20;advance(1);
     CHECK(!run(0)->field_reached&&local_notices==notices,"recorded fly-through announced a completed insertion");

     dropships[0].object.damage_flags=FLAG(_object_die_act_of_god_bit);
     CHECK(extraction_transport_can_disembark(&units[0]),"damaged craft could not release its rider");
     dropships[0].object.damage_flags=0;recorded_aircraft=NONE;advance(1);
     CHECK(run(0)->field_reached&&extraction_transport_can_disembark(&units[0]),
       "finished recording did not complete insertion and reopen the exit");
     CHECK(extraction_transport_passenger_prompt(&units[0]) &&
       !wcscmp(extraction_transport_passenger_prompt(&units[0]),L"Press %action to disembark"),
       "landed aircraft did not display a bound disembark action");
     units[0].unit.animation.state=_unit_state_hard_ping;
     CHECK(!unit_try_and_exit_seat(unit_handles[0]) &&
       !wcscmp(extraction_transport_passenger_prompt(&units[0]),L"Stay seated until the exit opens"),
       "passenger reaction showed a seat exit action the native animation refused");
     units[0].unit.animation.state=_unit_state_idle;
     CHECK(local_notices==notices+1&&strstr(local_notice_history[notices%4],"LANDED"),"safe arrival did not announce disembarkation");
     advance(1);
     CHECK(local_notices==notices+1,"landing notice repeated while the player remained seated");

     recorded_aircraft=craft;advance(1);
     CHECK(run(0)->field_reached&&!run(0)->field_exit_ready&&!extraction_transport_can_disembark(&units[0]),
       "resumed movement either forgot arrival or left the exit ready");
     CHECK(local_notices==notices+2&&strstr(local_notice_history[(notices+1)%4],"WAIT TO DISEMBARK"),
       "resumed movement left the landing notice active");
     advance(1);
     CHECK(local_notices==notices+2,"moving notice repeated while the player remained seated");
     recorded_aircraft=NONE;advance(1);
     CHECK(run(0)->field_exit_ready&&extraction_transport_can_disembark(&units[0])&&
       local_notices==notices+3&&strstr(local_notice_history[(notices+2)%4],"LANDED"),
       "the reopened seat exit did not restore landing guidance");

     units[0].object.parent_object_index=NONE;units[0].object.position.x=20;
     touch(1,0);advance(300);
     CHECK(run(0)->phase==_extraction_ready,"fixture did not activate an evacuation");
     units[0].object.parent_object_index=craft;units[0].unit.parent_seat_index=1;
     recorded_aircraft=craft;advance(1);
     CHECK(run(0)->dropship==NONE,"recorded fly-through accepted an evacuation boarding receipt");
     recorded_aircraft=NONE;advance(1);
     CHECK(run(0)->dropship==craft,"finished recording did not accept valid field boarding");
    }
   } else if(!strcmp(case_name,"transport-admission-pilot")) {
    transport_seats[0].flags=FLAG(_unit_seat_driver_bit);
    dropships[0].object.position.x=0;touch(0,0);
    CHECK(!unit_can_enter_seat(unit_handles[0],craft,0,&occupant)&&occupant==NONE,"player could take the managed driver seat");
    dropships[0].object.first_child_object_index=unit_handles[1];units[1].object.next_object_index=NONE;
    units[1].unit.player_index=NONE;units[1].unit.parent_seat_index=0;
    units[1].object.owner_team_index=_game_team_player;
    CHECK(!unit_can_enter_seat(unit_handles[0],craft,0,&occupant)&&occupant==NONE,"player was offered an AI pilot for eviction");
    CHECK(unit_can_enter_seat(unit_handles[0],craft,1,NULL),"managed passenger seat refused a deployed player");
    dropships[0].object.first_child_object_index=NONE;
    CHECK(unit_can_enter_seat(unit_handles[1],craft,0,NULL),"authored NPC pilot could not take its driver seat");
    extraction.dropship_boarding_groups[0]=NONE;
    CHECK(unit_can_enter_seat(unit_handles[0],craft,0,NULL),"legacy transport unexpectedly reserved its driver seat");
   } else if(!strcmp(case_name,"transport-admission-death")) {
    units[0].object.damage_flags=FLAG(_object_die_act_of_god_bit);CHECK(!unit_can_enter_seat(unit_handles[0],craft,0,NULL),"pending death boarded");
    units[0].object.damage_flags=0;dropships[0].object.damage_flags=FLAG(_object_dead_bit);
    CHECK(!unit_can_enter_seat(unit_handles[0],craft,0,NULL),"dead transport admitted passengers");
   } else if(!strcmp(case_name,"transport-admission-ordinary")) {
    CHECK(unit_can_enter_seat(unit_handles[0],unit_handles[1],0,NULL),"ordinary native target admission changed");
   } else if(!strcmp(case_name,"transport-admission-occupied")) {
    dropships[0].object.first_child_object_index=unit_handles[1];units[1].object.next_object_index=NONE;
    units[1].unit.parent_seat_index=0;units[1].unit.player_index=NONE;
    units[0].object.owner_team_index=_game_team_covenant;players[0].team_index=_game_team_covenant;
    CHECK(!unit_can_enter_seat(unit_handles[0],craft,0,&occupant)&&occupant==NONE,"wrong faction exposed an occupied seat to NPC eviction");
    units[0].object.owner_team_index=_game_team_player;players[0].team_index=_game_team_player;
    CHECK(!unit_can_enter_seat(unit_handles[0],craft,0,&occupant)&&occupant==unit_handles[1],"native occupied-seat reporting changed");
   } else {
    CHECK(unit_can_enter_seat(unit_handles[0],craft,0,NULL)&&!unit_can_enter_seat(unit_handles[0],dropship_handles[1],0,NULL),"UNSC could board wrong faction transport");
    units[0].object.owner_team_index=_game_team_covenant;players[0].team_index=_game_team_covenant;
    CHECK(!unit_can_enter_seat(unit_handles[0],craft,0,NULL)&&unit_can_enter_seat(unit_handles[0],dropship_handles[1],0,NULL),"Covenant could board wrong faction transport");
    players[0].team_index=_game_team_player;CHECK(!unit_can_enter_seat(unit_handles[0],dropship_handles[1],0,NULL),"inconsistent native allegiance boarded");
   }
   extraction_reset();return 0;
  }
  if(!strncmp(case_name,"transport-boarded",17)) {
   CHECK(!device_groups[10].actual_value&&!device_groups[11].actual_value,"authored stale boarding outputs survived binding");
   for(a=0;a<2;a++)players[a].team_index=units[a].object.owner_team_index=_game_team_player;
   if(!strncmp(case_name,"transport-boarded-squad-call",28)) {
    boolean other_squad=!strcmp(case_name,"transport-boarded-squad-call-isolated");
    boolean leaves=!strcmp(case_name,"transport-boarded-squad-call-leaves");
    boolean recall=!strcmp(case_name,"transport-boarded-squad-call-recall");
    long guest_loot;
    touch(5,0);touch(other_squad?6:5,1);
    touch(0,0);touch(0,1);
    CHECK(run(0)->phase==_extraction_deployed&&run(1)->phase==_extraction_deployed,"both squad operations must deploy");
    loot=pickup(0,1);guest_loot=pickup(1,2);
    units[0].object.position.x=dropships[0].object.position.x=20.f;
    units[1].object.position.x=recall?0.f:20.f;
    touch(1,0);
    CHECK(run(0)->phase==_extraction_activating&&control_updates[1]==1,"one native call did not start");
    CHECK(run(1)->phase==(other_squad||recall?_extraction_deployed:_extraction_activating),"call missed its own squadmate or reached another operation");
    if(leaves)units[1].object.position.x=0.f;
    advance(300);
    CHECK(run(0)->phase==_extraction_ready,"caller did not complete the countdown");
    if(recall) {
     unsigned long generation=run(0)->gear.record.generation;
     CHECK(run(1)->phase==_extraction_deployed,"squadmate outside the zone joined the first call");
     units[1].object.position.x=20.f;
     control_tag.control.type=1;device_group_set_actual_value(1,0.f);touch(1,0);
     CHECK(device_groups[1].actual_value==1.f&&run(0)->phase==_extraction_ready&&
      run(0)->gear.record.generation==generation&&!run(0)->activation_ticks,"recall restarted or changed caller's run");
     CHECK(run(1)->phase==_extraction_activating&&run(1)->activation_ticks==300,"recall missed nearby late squadmate");
     advance(300);
    }
    CHECK(run(1)->phase==(other_squad||leaves?_extraction_deployed:_extraction_ready),"teammate's zone countdown was not independent");
    if(other_squad||leaves) {
     CHECK(run(1)->gear.record.phase==_gear_deployed&&weapon_try_and_get(guest_loot),"excluded squadmate lost native gear");
     extraction_reset();return 0;
    }
    CHECK(extraction_transport_can_board(unit_handles[0],craft,0)&&extraction_transport_can_board(unit_handles[1],craft,1),
     "single request did not make both teammates eligible to board");
    units[0].object.parent_object_index=craft;units[0].unit.parent_seat_index=0;advance(1);
    CHECK(!device_groups[10].actual_value,"unboarded squadmate permitted departure");
    units[1].object.parent_object_index=craft;units[1].unit.parent_seat_index=1;advance(1);
    CHECK(device_groups[10].actual_value==1.f,"boarded crew could not depart");
    CHECK(extraction_transport_passenger_prompt(&units[0]) &&
      !wcscmp(extraction_transport_passenger_prompt(&units[0]),L"Stay aboard to bank your gear"),
      "evacuation seat prompted the rider to abandon recovery");
    before=remote_notices;
    dropships[0].object.position.x=0.f;advance(1);
    boolean guest_released=FALSE;
    int message;
    for(message=before;message<remote_notices;message++)
     if(strstr(remote_notice_history[message%4],"Use your seat exit to disembark"))guest_released=TRUE;
    CHECK(guest_released,"banked remote passenger did not receive normal exit guidance");
    CHECK(run(0)->phase==_extraction_returned&&run(1)->phase==_extraction_returned,
     "both passengers did not physically return and bank");
    CHECK(!weapon_try_and_get(loot)&&!weapon_try_and_get(guest_loot)&&run(0)->gear.record.count==1&&run(1)->gear.record.count==1,
     "shared request did not settle each player's own gear exactly once");
    extraction_reset();return 0;
   }
   if(!strcmp(case_name,"transport-boarded-returned-crew")) {
    touch(0,0);loot=pickup(0,1);
    units[0].object.position.x=dropships[0].object.position.x=20.f;
    touch(1,0);advance(300);
    CHECK(extraction_transport_can_board(unit_handles[0],craft,0),"first crew could not board at the field");
    units[0].object.parent_object_index=craft;units[0].unit.parent_seat_index=0;advance(1);
    dropships[0].object.position.x=0.f;advance(1);
    CHECK(run(0)->phase==_extraction_returned&&units[0].object.parent_object_index==craft&&
     !weapon_try_and_get(loot)&&run(0)->gear.record.count==1,"first crew did not bank while still seated");
    touch(0,1);
    CHECK(extraction_transport_can_board(unit_handles[1],craft,1),"next equipment owner could not board at the hub");
    units[1].object.parent_object_index=craft;units[1].unit.parent_seat_index=1;advance(1);
    CHECK(!device_groups[10].actual_value,"hub departure admitted a still-seated passenger from the previous trip");
    unit_exit_seat_end(unit_handles[0]);advance(1);
    CHECK(device_groups[10].actual_value==1.f&&run(0)->gear.record.count==1,
     "native exit did not release the new crew or altered the prior bank");
    extraction_reset();return 0;
   }
   if(!strcmp(case_name,"transport-boarded-hub")) {
    dropships[0].object.position.x=0;touch(0,0);touch(0,1);
    units[0].object.parent_object_index=craft;units[0].unit.parent_seat_index=0;advance(1);
    CHECK(!device_groups[10].actual_value,"first insertion passenger abandoned deployed hub teammate");
    units[1].object.position.x=6.f;advance(1);
    CHECK(!device_groups[10].actual_value,"crossing the hub boundary removed an approaching insertion passenger");
    units[1].object.parent_object_index=craft;units[1].unit.parent_seat_index=1;advance(1);
    CHECK(device_groups[10].actual_value==1.f,"fully boarded insertion crew not ready");
    before=boarding_changes;advance(5);CHECK(boarding_changes==before,"unchanged readiness repeatedly snapped native groups");
    dropships[0].object.flags&=~FLAG(_object_on_ground_bit);advance(1);
    CHECK(device_groups[10].actual_value==1.f,"map without a landing group acquired a new readiness gate");
    dropships[0].object.flags|=FLAG(_object_on_ground_bit);
    units[1].object.parent_object_index=NONE;advance(1);
    CHECK(!device_groups[10].actual_value,"disembarking insertion passenger retained readiness");
    units[1].object.parent_object_index=craft;dropships[0].object.position.x=20.f;advance(1);
    units[1].object.parent_object_index=NONE;units[1].object.position.x=8.f;
    dropships[0].object.position.x=0.f;advance(1);
    CHECK(device_groups[10].actual_value==1.f,"a passenger already inserted into the field blocked the next crew");
    extraction_reset();return 0;
   }
  }
  if(!strcmp(case_name,"transport-binding")||!strcmp(case_name,"transport-duplicate")) {
   CHECK(!extraction_owns_map(),"malformed transport binding admitted as a foot return");return 0;
  }
  CHECK(extraction_owns_map(),"native transport map refused");
  units[0].object.owner_team_index=faction?_game_team_covenant:_game_team_player;
  touch(0,0);loot=pickup(0,1);units[0].object.position.x=20;touch(1,0);advance(300);
  CHECK(run(0)->phase==_extraction_ready&&run(0)->gear.record.phase==_gear_deployed,"activation incorrectly settled transport loot");
  if(!strcmp(case_name,"transport-recall")||!strcmp(case_name,"transport-recall-state")||!strcmp(case_name,"transport-recall-missing")) {
   unsigned long generation=run(0)->gear.record.generation;
   control_tag.control.type=1;device_group_set_actual_value(1,0.f);
   if(!strcmp(case_name,"transport-recall-missing")) {
    extraction.dropship_names[1]=NONE;units[0].object.owner_team_index=_game_team_covenant;touch(1,0);
    CHECK(!device_groups[1].actual_value&&run(0)->gear.record.generation==generation,"missing faction transport accepted recall");
    extraction_reset();return 0;
   }
   if(!strcmp(case_name,"transport-recall-state")) {
    units[0].object.parent_object_index=craft;touch(1,0);
    CHECK(!device_groups[1].actual_value,"seated player recalled the aircraft");units[0].object.parent_object_index=NONE;
    native_cinematic=TRUE;touch(1,0);CHECK(!device_groups[1].actual_value,"cinematic recalled the aircraft");native_cinematic=FALSE;
    units[0].object.damage_flags=FLAG(_object_die_act_of_god_bit);touch(1,0);
    CHECK(!device_groups[1].actual_value,"pending death recalled the aircraft");units[0].object.damage_flags=0;
    units[0].object.position.x=0;touch(1,0);CHECK(!device_groups[1].actual_value,"hub walk recalled the aircraft");units[0].object.position.x=20;
    device_groups[4].actual_value=0;touch(1,0);CHECK(!device_groups[1].actual_value,"unpowered control recalled the aircraft");device_groups[4].actual_value=1;
    profile=FALSE;touch(1,0);
    CHECK(!device_groups[1].actual_value&&run(0)->gear.record.generation==generation&&weapon_try_and_get(loot),
     "unverified owner recalled the aircraft or altered gear");
    profile=TRUE;
   }
   touch(1,0);
   CHECK(device_groups[1].actual_value==1.f&&run(0)->phase==_extraction_ready&&!run(0)->activation_ticks,
    "already-cleared player could not recall without another countdown");
   CHECK(run(0)->gear.record.phase==_gear_deployed&&run(0)->gear.record.generation==generation&&weapon_try_and_get(loot),
    "transport recall changed ownership, gear or rewards");
   extraction_reset();return 0;
  }
  units[0].object.position.x=0;advance(1);
  CHECK(run(0)->phase==_extraction_ready&&weapon_try_and_get(loot),"foot return bypassed physical boarding");
  if(!strcmp(case_name,"transport-landed-loop")) {
   players[0].team_index=_game_team_player;
   units[0].object.position.x=20;
   dropships[0].object.flags&=~FLAG(_object_on_ground_bit);
   units[0].object.parent_object_index=craft;units[0].unit.parent_seat_index=0;
   advance(1);CHECK(run(0)->dropship==NONE,"airborne field attachment counted as landed boarding");
   units[0].object.parent_object_index=NONE;
   CHECK(!extraction_transport_can_board(unit_handles[0],craft,0),
    "landing-only map offered the airborne craft for native boarding");
   units[0].object.parent_object_index=craft;
   dropships[0].object.position.x=0;dropships[0].object.flags|=FLAG(_object_on_ground_bit);
   advance(1);CHECK(run(0)->gear.record.phase==_gear_deployed&&weapon_try_and_get(loot),
    "landing at the hub counted after attachment to an airborne craft");
   units[0].object.parent_object_index=NONE;dropships[0].object.position.x=20;
  }
  units[0].object.parent_object_index=craft;units[0].unit.parent_seat_index=0;
  if(!strcmp(case_name,"transport-loop")) {
   units[0].unit.animation.state=_unit_state_entering_seat;advance(1);
   CHECK(run(0)->dropship==craft,"legacy seat-triggered route lost its evacuation receipt");
   units[0].unit.animation.state=_unit_state_idle;
  }
  if(!strcmp(case_name,"transport-landed-loop"))
   CHECK(extraction_transport_can_board(unit_handles[0],craft,0),"landing-only map refused native boarding after touchdown");
  if(!strcmp(case_name,"transport-pilot-receipt"))transport_seats[0].flags=FLAG(_unit_seat_driver_bit);
  if(!strcmp(case_name,"transport-seat"))units[0].unit.parent_seat_index=2;
  if(!strcmp(case_name,"transport-stale"))units[0].object.parent_object_index=craft+0x10000;
  if(!strcmp(case_name,"transport-wrong-faction")||!strcmp(case_name,"transport-missing-faction"))units[0].object.owner_team_index=_game_team_covenant;
  if(!strcmp(case_name,"transport-dead"))dropships[0].object.damage_flags=FLAG(_object_dead_bit);
  advance(1);
  if(!strcmp(case_name,"transport-seat")||!strcmp(case_name,"transport-stale")||!strcmp(case_name,"transport-pilot-receipt")||
   !strcmp(case_name,"transport-wrong-faction")||!strcmp(case_name,"transport-missing-faction")||!strcmp(case_name,"transport-dead")) {
   CHECK(run(0)->dropship==NONE&&run(0)->phase==_extraction_ready,"invalid native seat/ship/faction received boarding credit");
   dropships[faction].object.position.x=0;advance(1);
   CHECK(run(0)->gear.record.phase==_gear_deployed&&weapon_try_and_get(loot),"invalid transport banked owned equipment");return 0;
  }
  CHECK(run(0)->dropship==craft&&run(0)->phase==_extraction_ready,"real assigned LZ seat not recorded");
  if(!strncmp(case_name,"transport-crash-",16)) {
   unsigned long generation=run(0)->gear.record.generation;
   long replacement=craft+0x10000;
   dropships[0].object.damage_flags=FLAG(!strcmp(case_name,"transport-crash-silent")?_object_die_act_of_god_silent_bit:
    !strcmp(case_name,"transport-crash-no-statistics")?_object_die_act_of_god_no_statistics_bit:_object_die_act_of_god_bit);
   if(!strcmp(case_name,"transport-crash-hub"))dropships[0].object.position.x=0;
   advance(1);
   CHECK(run(0)->dropship==NONE&&run(0)->phase==_extraction_ready,"dying transport retained boarding credit or banked during native destruction");
   dropships[0].object.position.x=0;advance(1);
   CHECK(run(0)->gear.record.phase==_gear_deployed&&run(0)->gear.record.generation==generation&&weapon_try_and_get(loot),
    "failed flight consumed, lost or rewarded surviving player equipment");
   /* Native salted replacement: neither a reused object slot nor a new seat
    * at the hub substitutes for another actual LZ boarding. */
   dropship_handles[0]=replacement;dropships[0].object.damage_flags=0;
   units[0].object.parent_object_index=replacement;advance(1);
   CHECK(run(0)->dropship==NONE&&run(0)->gear.record.phase==_gear_deployed&&weapon_try_and_get(loot),
    "replacement ship inherited the failed trip's arrival receipt");
   units[0].object.parent_object_index=NONE;units[0].object.position.x=20;
   control_tag.control.type=1;device_group_set_actual_value(1,0.f);touch(1,0);
   CHECK(device_groups[1].actual_value==1.f&&!run(0)->activation_ticks&&run(0)->gear.record.generation==generation,
    "survivor could not recall replacement without reserving or rewarding gear again");
   dropships[0].object.position.x=20;units[0].object.parent_object_index=replacement;advance(1);
   CHECK(run(0)->dropship==replacement,"fresh physical LZ boarding did not rearm recovery after a crash");
   dropships[0].object.position.x=7;advance(1);
   CHECK(run(0)->gear.record.phase==_gear_deployed&&weapon_try_and_get(loot),"replacement flight banked before arrival");
   native_cinematic=TRUE;dropships[0].object.position.x=0;advance(1);
   CHECK(run(0)->gear.record.phase==_gear_deployed&&weapon_try_and_get(loot),"replacement banked during departure cinematic");
   native_cinematic=FALSE;advance(1);
   CHECK(run(0)->phase==_extraction_returned&&run(0)->gear.record.count==1&&!weapon_try_and_get(loot),
    "successful replacement flight failed to consume and bank actual gear once");
   units[0].object.parent_object_index=NONE;units[0].object.position.x=0;touch(0,0);
   CHECK(run(0)->phase==_extraction_deployed&&run(0)->gear.record.reserved_count==1,
    "recovered survivor equipment was not available for the next deployment");
   extraction_reset();return 0;
  }
  if(!strncmp(case_name,"transport-boarded",17)) {
   boolean other=!strcmp(case_name,"transport-boarded-other")||!strcmp(case_name,"transport-boarded-identity");
   CHECK(device_groups[10].actual_value==1.f,"single requested evacuation was not ready");
   if(!strcmp(case_name,"transport-boarded-forced-exit")) {
    client=TRUE;
    extraction_transport_seat_transition_started(&units[0]);
    client=FALSE;
    CHECK(device_groups[10].actual_value==1.f,"a client invalidated the host's crew signal");
    units[0].object.owner_team_index=_game_team_covenant;
    dropships[0].object.damage_flags|=FLAG(_object_dead_bit);
    extraction_transport_seat_transition_started(&units[0]);
    CHECK(!device_groups[10].actual_value,"a forced exit left the ship's previous crew visible to HaloScript");
    extraction_reset();return 0;
   }
   if(!strcmp(case_name,"transport-boarded-recording")) {
    short leg;
    CHECK(extraction.dropship_landing_groups[0]==NONE,"fixture requires a boarding-only map");
    for(leg=0;leg<2;leg++) {
     recorded_aircraft=craft;
     extraction_transport_recording_started(dropship_handles[1]);
     CHECK(device_groups[10].actual_value==1.f,"another ship revoked this crew's readiness");
     client=TRUE;extraction_transport_recording_started(craft);client=FALSE;
     CHECK(device_groups[10].actual_value==1.f,"a client revoked host crew readiness");
     extraction_transport_recording_started(craft);
     CHECK(!device_groups[10].actual_value,"boarding-only map kept stale readiness when playback started");
     advance(1);
     CHECK(!device_groups[10].actual_value&&run(0)->dropship==craft,
      "active flight republished readiness or lost its native boarding record");
     recorded_aircraft=NONE;advance(1);
     CHECK(device_groups[10].actual_value==1.f,"cleanup did not rearm the remaining crew");
    }
    extraction_reset();return 0;
   }
   if(!strncmp(case_name,"transport-boarded-squad-",23)) {
    boolean same=!strcmp(case_name,"transport-boarded-squad-wait");
    boolean hub=!strcmp(case_name,"transport-boarded-squad-hub");
    extraction.squads[0].name=9;extraction.squads[1].name=10;
    extraction.squads[0].operation=1;run(0)->squad=0;run(0)->squad_operation=1;
    CHECK(extraction_open_gear(run(1),&players[1]),"fixture second owner could not open existing gear");
    run(1)->squad=same?0:1;touch(0,1);
    CHECK(run(1)->phase==_extraction_deployed,"second squad could not prepare its existing gear");
    if(hub) {
     dropships[0].object.position.x=0;run(0)->phase=_extraction_deployed;run(0)->dropship=NONE;
     advance(1);
     CHECK(device_groups[10].actual_value==1.f,"another squad on foot at the hub blocked a seated insertion crew");
    } else {
     units[1].object.position.x=20;touch(1,1);advance(1);
     CHECK(device_groups[10].actual_value==(same?0.f:1.f),"unboarded squad activation controlled another squad's departure");
     advance(300);
     CHECK(device_groups[10].actual_value==(same?0.f:1.f),"another ready field squad blocked the boarded crew");
     units[1].object.parent_object_index=craft;units[1].unit.parent_seat_index=1;
     extraction_transport_seat_transition_started(&units[1]);
     CHECK(!device_groups[10].actual_value,"late boarder left prior crew readiness visible to HaloScript");
     units[1].unit.animation.state=_unit_state_entering_seat;advance(1);
     CHECK(!device_groups[10].actual_value&&run(1)->dropship==NONE,"late boarder was counted before entering the seat");
     units[1].unit.animation.state=_unit_state_idle;advance(1);
     CHECK(device_groups[10].actual_value==1.f,"actual joint boarding failed to admit both participating squads");
     if(!strcmp(case_name,"transport-boarded-squad-stale")) {
      run(1)->squad_operation++;advance(1);
      CHECK(!device_groups[10].actual_value,"stale operation authorized a shared transport departure");
     }
     if(!strcmp(case_name,"transport-boarded-squad-revoked")) {
      extraction.squads[1].ready_group=8;device_groups[8].actual_value=0.f;advance(1);
      CHECK(run(1)->phase==_extraction_deployed&&units[1].object.parent_object_index==craft,
       "objective revocation did not leave an ineligible passenger physically attached");
      CHECK(!device_groups[10].actual_value,"evacuation admitted a still-seated passenger with revoked objectives");
      unit_exit_seat_end(unit_handles[1]);advance(1);
      CHECK(device_groups[10].actual_value==1.f&&run(1)->gear.record.phase==_gear_deployed,
       "unrelated squad could not proceed after the ineligible passenger disembarked");
     }
     if(!strcmp(case_name,"transport-boarded-squad-mixed")) {
      units[1].object.parent_object_index=NONE;advance(1);
      CHECK(device_groups[10].actual_value==1.f,"a squad that left the shared craft retained its departure veto");
     }
    }
    extraction_reset();return 0;
   }
   players[1].team_index=units[1].object.owner_team_index=other?_game_team_covenant:_game_team_player;
   touch(0,1);units[1].object.position.x=20;touch(1,1);advance(1);
   CHECK(device_groups[10].actual_value==(other?1.f:0.f),"activation crew/faction readiness incorrect");
   advance(300);CHECK(device_groups[10].actual_value==(other?1.f:0.f),"unboarded requesting crew counted as seated");
   units[1].object.parent_object_index=dropship_handles[other?1:0];units[1].unit.parent_seat_index=1;advance(1);
   CHECK(device_groups[10].actual_value==1.f&&device_groups[11].actual_value==(other?1.f:0.f),"native crew boarding did not publish independent readiness");
   if(!strcmp(case_name,"transport-boarded-native-exit")) {
    native_animation_graph.unit_seats=(struct tag_block){1,&native_animation_seat};
    native_animation_seat.animations=(struct tag_block){9,native_animation_indices};
    native_animation_indices[8].animation_index=1;
    units[0].definition_index=4;
    CHECK(unit_try_and_exit_seat(unit_handles[0]),"native exit animation failed to start");
    CHECK(units[0].object.parent_object_index==craft&&units[0].unit.animation.state==_unit_state_exiting_seat,
     "fixture did not leave the exiting passenger attached");
    CHECK(!device_groups[10].actual_value,"departure remained ready after the native exit started");
    advance(1);
    CHECK(!device_groups[10].actual_value,"still-attached exiting passenger republished departure readiness");
    units[0].object.parent_object_index=NONE;units[0].unit.animation.state=_unit_state_idle;advance(1);
    CHECK(!device_groups[10].actual_value,"missing passenger authorized departure after disembarking");
    units[0].object.parent_object_index=craft;advance(1);
    CHECK(device_groups[10].actual_value==1.f,"physical reboarding did not restore departure readiness");
   } else if(!strcmp(case_name,"transport-boarded-ship")) {
    dropships[0].object.damage_flags=FLAG(_object_die_act_of_god_bit);advance(1);
    CHECK(!device_groups[10].actual_value&&run(0)->dropship==NONE,"dying aircraft retained crew readiness or passenger receipt");
   } else if(!strcmp(case_name,"transport-boarded-pending")) {
    units[0].object.damage_flags=FLAG(_object_die_act_of_god_bit);advance(1);
    CHECK(!device_groups[10].actual_value,"pending native death authorized departure");
   } else if(!strcmp(case_name,"transport-boarded-identity")) {
    units[0].unit.player_index=NONE;advance(1);
    CHECK(!device_groups[10].actual_value,"unowned body authorized departure");
   } else if(!strcmp(case_name,"transport-boarded-owner-state")) {
    run(0)->gear.pending=TRUE;extraction_transport_readiness();
    CHECK(!device_groups[10].actual_value,"pending gear authorized a boarded departure");
    run(0)->gear.pending=FALSE;extraction_transport_readiness();
    CHECK(device_groups[10].actual_value==1.f,"valid gear did not recover departure readiness");
    players[0].team_index=_game_team_covenant;extraction_transport_readiness();
    CHECK(!device_groups[10].actual_value,"changed player allegiance authorized a stale seat");
    players[0].team_index=_game_team_player;extraction_transport_readiness();
    CHECK(device_groups[10].actual_value==1.f,"matching allegiance did not recover departure readiness");
   } else {
    units[0].object.parent_object_index=NONE;advance(1);
    CHECK(!device_groups[10].actual_value,"lost seat retained departure readiness");
    units[0].object.parent_object_index=craft;native_cinematic=TRUE;advance(1);
    CHECK(!device_groups[10].actual_value,"cinematic authorized a new departure");
    native_cinematic=FALSE;advance(1);CHECK(device_groups[10].actual_value==1.f,"normal boarding did not recover readiness");
    dropships[0].object.position.x=7;advance(1);CHECK(!device_groups[10].actual_value,"in-flight ship remained ready at a landing pad");
   }
   extraction_reset();return 0;
  }
  if(!strncmp(case_name,"transport-callback",18)) {
   boolean other=!strcmp(case_name,"transport-callback-other");
   boolean lost=!strcmp(case_name,"transport-callback-aborted-loss");
   boolean aborted_recording=!strcmp(case_name,"transport-callback-aborted-recording");
   boolean recording=aborted_recording||!strcmp(case_name,"transport-callback-recording");
   boolean aborted=lost||aborted_recording||!strcmp(case_name,"transport-callback-aborted");
   boolean field=aborted||!strcmp(case_name,"transport-callback-field");
   boolean disconnect=!strcmp(case_name,"transport-callback-disconnect");
   boolean escrow_disconnect=!strcmp(case_name,"transport-callback-escrow-disconnect");
   boolean pending=!strcmp(case_name,"transport-callback-pending")||disconnect;
   boolean stale_receipt=!strcmp(case_name,"transport-callback-stale-receipt");
   short i;
   for(i=0;i<3;i++) {
    strcpy(transport_scripts[i].name,i==0?"forerunner_pelican_complete":i==1?"forerunner_spirit_complete":"forerunner_extraction_complete");
    transport_scripts[i].script_type=_hs_script_dormant;transport_scripts[i].return_type=_hs_type_void;
   }
   if(!strcmp(case_name,"transport-callback-signature"))transport_scripts[0].script_type=_hs_script_startup;
   if(!strcmp(case_name,"transport-callback-continuous"))transport_scripts[0].script_type=_hs_script_continuous;
   scenario.hs_scripts.count=3;scenario.hs_scripts.address=transport_scripts;
   units[1].object.owner_team_index=other?_game_team_covenant:_game_team_player;
   touch(0,1);pickup(1,2);units[1].object.position.x=20;
   if(!field) {
    touch(1,1);advance(300);
    units[1].object.parent_object_index=dropship_handles[other?1:0];units[1].unit.parent_seat_index=1;advance(1);
    CHECK(run(1)->dropship==dropship_handles[other?1:0],"second crew member did not board natively");
   }
   CHECK(!transport_wakes[0]&&!transport_wakes[1]&&!completion_wakes,"boarding alone released a transport");
   if(aborted) {
    strcpy(transport_scripts[1].name,"forerunner_pelican_aborted");
    dropships[0].object.position.x=7;
    if(lost) {fail_write=TRUE;units[0].object.damage_flags=FLAG(_object_dead_bit);}
    else players[0].quit_out_of_game=TRUE;
    advance(1);CHECK(!transport_aborts[0],"aborted flight released before reaching the hub");
    dropships[0].object.position.x=0;
    native_cinematic=TRUE;advance(1);
    CHECK(!transport_aborts[0],"aborted flight released during the departure cinematic");
    native_cinematic=FALSE;
    if(recording) {
     recorded_aircraft=craft;advance(2);
     CHECK(!transport_aborts[0]&&!transport_wakes[0],"aborted aircraft released while playback still owned it");
     recorded_aircraft=NONE;
    }
    advance(1);
    if(lost) {
     CHECK(run(0)->phase==_extraction_losing&&!transport_aborts[0],"uncertain loss released the aircraft");
     fail_write=FALSE;advance(30);
     CHECK(run(0)->phase==_extraction_lost&&run(0)->gear.record.count==0,"failed flight banked gear");
    }
    CHECK(transport_aborts[0]==1&&!transport_wakes[0]&&!completion_wakes,
     "abandoned aircraft could not rearm without signaling successful recovery");
    CHECK(run(1)->phase==_extraction_deployed,"unboarded crew lost its independent operation");
    advance(60);CHECK(transport_aborts[0]==1&&!transport_aborts[1],"same abandoned arrival repeated or released the other faction");
    extraction_reset();return 0;
   }
   dropships[0].object.position.x=0;
   native_cinematic=TRUE;advance(1);
   CHECK(!transport_wakes[0]&&!transport_wakes[1],"transport released during cinematic");
   native_cinematic=FALSE;
   if(recording) {
    long crewmate_loot=units[1].unit.weapon_object_indices[0];
    CHECK(extraction.dropship_landing_groups[0]==NONE,"fixture requires a boarding-only map");
    recorded_aircraft=craft;advance(2);
    CHECK(run(0)->phase==_extraction_ready&&run(1)->phase==_extraction_ready&&
     weapon_try_and_get(loot)&&weapon_try_and_get(crewmate_loot),
     "return playback consumed passenger inventory at the hub");
    CHECK(!transport_wakes[0]&&!transport_aborts[0]&&!completion_wakes,
     "return playback released the transport or completed the mission");
    recorded_aircraft=NONE;
   }
   if(stale_receipt) {
    struct extraction_run old_arrival;
    extraction_update_run(run(0),&players[0],++tick);
    old_arrival=*run(0);
    old_arrival.dropship+=0x10000;
    extraction_remember_bank(&old_arrival);
    CHECK(extraction.dropship_banked[0]==craft,"older aircraft settlement displaced the current aircraft receipt");
    advance(1);
   } else if(pending) {
    extraction_update_run(run(0),&players[0],++tick);
    CHECK(run(0)->phase==_extraction_returned,"first crew member failed to bank");
    fail_write=TRUE;advance(1);
    CHECK(run(1)->phase==_extraction_ready&&!transport_wakes[0],"unbanked same-ship crew released the aircraft");
    if(disconnect) {
     players[0].quit_out_of_game=TRUE;advance(1);
     CHECK(!transport_wakes[0],"banked player leaving released a ship with a pending crewmate");
     fail_write=FALSE;players[1].quit_out_of_game=TRUE;advance(1);
     CHECK(transport_wakes[0]==1&&!completion_wakes,
      "banked player leaving erased this aircraft's receipt");
     advance(60);CHECK(transport_wakes[0]==1,"same arrival repeated after crew disconnected");
     extraction_reset();return 0;
    }
    fail_write=FALSE;advance(30);
   } else if(escrow_disconnect) {
    publish_failure=TRUE;advance(1);
    CHECK(run(0)->phase==_extraction_settling&&run(1)->phase==_extraction_settling&&
     !transport_wakes[0],"uncertain escrow published a transport callback");
    publish_failure=FALSE;players[0].quit_out_of_game=players[1].quit_out_of_game=TRUE;
    advance(1);CHECK(transport_wakes[0]==1&&!completion_wakes,
     "durable escrow banking on close lost its physical transport receipt");
    extraction_reset();return 0;
   } else if(!strcmp(case_name,"transport-callback-settlement-fault")) {
    long crewmate_loot=units[1].unit.weapon_object_indices[0];
    CHECK(weapon_try_and_get(crewmate_loot),"second crew member had no native loot before arrival");
    CHECK(device_groups[11].actual_value==1.f,"fixture did not publish landing before recovery");
    advance(1);
    CHECK(extraction.suspended&&run(0)->gear.record.phase==_gear_escrow&&weapon_try_and_get(loot),
     "fixture did not stop at an unconsumed first-player escrow");
    CHECK(run(1)->phase==_extraction_ready&&run(1)->gear.record.phase==_gear_deployed&&
     units[1].unit.weapon_object_indices[0]==crewmate_loot&&weapon_try_and_get(crewmate_loot),
     "second crew member settled after the world suspended banking");
    CHECK(!transport_wakes[0]&&!transport_wakes[1]&&!completion_wakes,
     "suspended settlement released the transport or completed the mission");
    CHECK(!device_groups[10].actual_value&&!device_groups[11].actual_value,
     "suspended settlement left aircraft signals enabled");
    advance(30);
    CHECK(run(1)->gear.record.phase==_gear_deployed&&weapon_try_and_get(crewmate_loot),
     "suspended settlement retried another passenger automatically");
    extraction_reset();return 0;
   } else if(!strcmp(case_name,"transport-callback-replaced")) {
    dropship_handles[0]+=0x10000;
    advance(1);
    CHECK(!transport_wakes[0]&&run(0)->phase==_extraction_ready,"replacement ship inherited old crew settlement");
    extraction_reset();return 0;
   } else advance(1);
   CHECK(run(0)->phase==_extraction_returned,"first transport did not bank crew");
   CHECK(transport_wakes[0]==(!strcmp(case_name,"transport-callback-signature")?0:1),"wrong Pelican completion count");
   CHECK(!transport_wakes[1],"Pelican released Spirit");
   if(other||field)CHECK(!completion_wakes&&run(1)->phase!=_extraction_returned,"independent player completed the whole mission");
   if(other) {
    dropships[1].object.position.x=0;advance(1);
    CHECK(transport_wakes[1]==1&&completion_wakes==1,"Spirit crew did not complete independently");
   }
   advance(60);CHECK(transport_wakes[0]<=1&&transport_wakes[1]<=1,"same arrival repeated a transport callback");
   extraction_reset();return 0;
  }
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
  native_cinematic=FALSE;
  if(!strncmp(case_name,"transport-landed",16)) {
   dropships[0].object.flags&=~FLAG(_object_on_ground_bit);advance(1);
   CHECK(run(0)->phase==_extraction_ready&&weapon_try_and_get(loot),"airborne hub arrival banked equipment");
   dropships[0].object.flags|=FLAG(_object_on_ground_bit);
  }
  advance(1);
  CHECK(run(0)->phase==_extraction_returned&&run(0)->gear.record.count==1&&!weapon_try_and_get(loot),"boarded arrival did not bank exactly once");
  advance(30);CHECK(run(0)->gear.record.count==1,"arrived passenger duplicated loot");
  extraction_reset();return 0;
 }
 if(!strcmp(case_name,"notice-routing")||!strcmp(case_name,"notice-controller")||!strcmp(case_name,"notice-broadcast")||!strcmp(case_name,"notice-fallback")) {
  int host_before=local_notices,remote_before=remote_notices;short j;
  char full[128],host_text[160]={0},remote_text[160]={0};
  if(!strcmp(case_name,"notice-broadcast")) {
   local_views=2;extraction_notice("Operation ready.");
   CHECK(local_notices==host_before+1&&remote_notices==remote_before+1,"shared host chat received duplicate global notices");
   CHECK(!strcmp(local_notice_history[host_before%4],remote_notice_history[remote_before%4]),"broadcast recipients saw different text");
   local_views=0;extraction_notice("Operation ready.");CHECK(local_notices==host_before+1&&remote_notices==remote_before+2,"dedicated notice invented a local recipient");
   CHECK(!native_hud_notices,"global notice used conflicting native HUD layout");return 0;
  }
  if(!strcmp(case_name,"notice-controller")) {
   players[0].local_player_index=1;players[0].network_player_data.controller_index=3;
   extraction_player_notice(player_handles[0],"Prepared gear is ready.");
   CHECK(local_notices==host_before+1&&!strncmp(local_notice_history[host_before%4],"P2 | ",5)&&remote_notices==remote_before&&!native_hud_notices,"local notice used wrong view, recipient or renderer");return 0;
  }
  memset(full,'x',127);full[127]=0;
  if(!strcmp(case_name,"notice-fallback")) {
   notice_chat_visible=FALSE;extraction_player_notice(player_handles[0],full);extraction_player_notice(player_handles[1],full);
   CHECK(local_notices==host_before+4&&remote_notices==remote_before+1&&native_hud_notices==4,"unavailable chat hid local feedback or changed remote routing");
   for(j=3;j>=0;j--)strcat(host_text,local_notice_history[(host_before+j)%4]);
   /* A native word boundary puts the prefix on its own line; the newline
    * replaces its separating space, while every data character survives. */
   CHECK(!strncmp(host_text,"P1 |",4)&&!strcmp(host_text+4,full),"native HUD fallback lost the controller or notice tail");
   CHECK(!strncmp(remote_notice_history[remote_before%4],"P3 | ",5)&&!strcmp(remote_notice_history[remote_before%4]+5,full),"host visibility setting changed a remote notice");return 0;
  }
  extraction_player_notice(player_handles[0],full);extraction_player_notice(player_handles[1],full);
  CHECK(local_notices==host_before+1&&remote_notices==remote_before+1&&!native_hud_notices,"long notice truncated or used conflicting layouts");
  for(j=0;j<1;j++) {
   char const *host=local_notice_history[(host_before+j)%4],*remote=remote_notice_history[(remote_before+j)%4];
   CHECK(!strncmp(host,"P1 | ",5)&&!strncmp(remote,"P3 | ",5)&&!strcmp(host+5,remote+5),"host/client formatting or controller ownership differs");
   strcat(host_text,host+5);strcat(remote_text,remote+5);
  }
  CHECK(!strcmp(host_text,full)&&!strcmp(remote_text,full),"packet splitting lost notice characters");return 0;
 }
 if(!strcmp(case_name,"notice-complete")) {
  touch(0,0);pickup(0,1);escape(0);
  CHECK(strstr(local_notice_history[(local_notices-1)%4],"GEAR BANKED | Stash"),"bank result missing from shared notice layout");
  CHECK(!native_hud_notices,"return feedback overlapped the chat/debrief layout");
  extraction_reset();return 0;
 }
 if(!strcmp(case_name,"battery-stash")) {
  struct game_modes_inventory captured,loadout;
  struct gear_record legacy;
  struct weapon_datum *weapon;
  short room[2]={4,4};
  touch(0,0);
  CHECK(run(0)->phase==_extraction_deployed,"initial deployment failed");
  a=pickup(0,1);
  weapon=weapon_get(a);
  weapon->weapon.age=.375f;
  weapon->weapon.magazines[0].rounds_loaded=0;
  weapon->weapon.magazines[0].rounds_total=100;
  weapon_tag.weapon.flags|=FLAG(_weapon_cannot_fire_at_maximum_age_bit);
  memset(&magazine,0,sizeof(magazine));
  CHECK(game_modes_inventory_capture(player_handles[0],&captured)&&captured.weapon_count==1,
   "battery weapon capture failed");
  CHECK(captured.weapons[0].age==.375f&&!captured.weapons[0].rounds_reserve[0],
   "battery capture saved the unused native round count");
  escape(0);
  CHECK(run(0)->gear.record.count==1&&!weapon_try_and_get(a),"battery weapon was not banked once");
  legacy=run(0)->gear.record;
  legacy.items[0].equipment.rounds_reserve[0]=100;
  magazine.rounds_loaded_maximum=20;
  CHECK(!game_modes_inventory_weapon_compatible(&legacy.items[0].equipment),
   "battery tag bypassed the reserve limit of a real magazine");
  magazine.rounds_loaded_maximum=0;
  CHECK(gear_commit(&run(0)->gear,&legacy),"could not seed a previously saved battery weapon");
  extraction_reset();advance(1);touch(2,0);
  CHECK(run(0)->gear.record.count==1&&run(0)->gear.record.items[0].equipment.rounds_reserve[0]==100,
   "reopen rewrote or lost the saved weapon");
  CHECK(gear_loadout(&run(0)->gear,&loadout,FALSE),"empty faction loadout could not initialize");
  scenario.starting_profiles=(struct tag_block){2,faction_profiles};
  faction_profiles[0].primary_weapon.weapon.index=1;
  faction_profiles[0].primary_weapon.rounds_total=100;
  faction_profiles[0].secondary_weapon.weapon.index=NONE;
  CHECK(game_modes_inventory_starting_profile(0,2,room,&loadout)&&loadout.weapon_count==1&&
   !loadout.weapons[0].rounds_reserve[0]&&loadout.weapons[0].age==0.f,
   "native faction profile could not grant a fresh battery weapon");
  scenario.starting_profiles.count=0;
  touch(0,0);
  CHECK(run(0)->phase==_extraction_deployed&&run(0)->gear.record.reserved_count==1,
   "old battery weapon could not be reissued from the stash");
  weapon=weapon_get(units[0].unit.weapon_object_indices[0]);
  CHECK(weapon->weapon.age==.375f&&!weapon->weapon.magazines[0].rounds_loaded&&
   !weapon->weapon.magazines[0].rounds_total,"battery reissue changed charge or restored unused rounds");
  escape(0);
  CHECK(run(0)->gear.record.count==1&&run(0)->gear.record.items[0].equipment.age==.375f&&
   !run(0)->gear.record.items[0].equipment.rounds_reserve[0],"second extraction duplicated gear or lost battery charge");
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
  CHECK(gear_loadout(&run(1)->gear,&chosen,TRUE)&&chosen.weapon_count==1&&
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
   units[1].object.position.x=20.f;touch(1,1);advance(TICKS_PER_SECOND);
   CHECK(run(1)->phase==_extraction_deployed&&run(1)->gear.pending,
    "a run with unflushed confirmation became ready to extract");
   publish_confirm=publish_failure=FALSE;
   before=weapon_count;
   advance(TICKS_PER_SECOND);
   CHECK(!run(1)->gear.pending&&weapon_count==before,
    "storage recovery left the run locked or issued gear again");
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
  CHECK(gear_loadout(&run(0)->gear,&stale,TRUE)&&stale.weapon_count==2,"source loadout missing");
  memcpy(original,run(0)->gear.record.items[1].id,sizeof(original));
  extraction_reset();map="survival@ce";checksum++;advance(1);touch(2,0);
  CHECK(gear_loadout(&run(0)->gear,&chosen,TRUE)&&!chosen.weapon_count&&run(0)->gear.record.count==2,
   "default cache isolation changed");
  CHECK(!game_modes_inventory_loadout_valid(&stale),"stale snapshot bypassed native cache admission");
  shared_cache=TRUE;
  if(!strcmp(case_name,"shared-cache-reject")) {
   generation=run(0)->gear.record.generation;missing_tag=1;touch(2,0);
   CHECK(gear_loadout(&run(0)->gear,&chosen,TRUE)&&chosen.weapon_count==1&&
    !strcmp(chosen.weapons[0].tag_path,tag_get_name(2)),"missing native tag was granted or hid compatible gear");
   missing_tag=NONE;weapon_tag.weapon.flags=FLAG(_weapon_must_be_readied_bit);
   CHECK(gear_loadout(&run(0)->gear,&chosen,TRUE)&&!chosen.weapon_count,"target objective definition became personal loot");
   weapon_tag.weapon.flags=0;weapon_tag.weapon.magazines.count=0;
   CHECK(gear_loadout(&run(0)->gear,&chosen,TRUE)&&!chosen.weapon_count,"absent magazines discarded stored ammunition");
   weapon_tag.weapon.magazines.count=1;magazine.rounds_loaded_maximum=6;
   CHECK(gear_loadout(&run(0)->gear,&chosen,TRUE)&&!chosen.weapon_count,"target magazine silently clipped recovered rounds");
   magazine.rounds_loaded_maximum=20;magazine.rounds_total_maximum=62;
   CHECK(gear_loadout(&run(0)->gear,&chosen,TRUE)&&!chosen.weapon_count,"target reserve silently clipped recovered ammunition");
   CHECK(run(0)->gear.record.count==2&&run(0)->gear.record.generation==generation&&
    run(0)->gear.record.items[0].equipment.rounds_loaded[0]==7,
    "incompatible native definitions changed the owned stash");
  } else {
   touch(2,0);
   CHECK(gear_loadout(&run(0)->gear,&chosen,TRUE)&&chosen.weapon_count==2&&chosen.map_checksum==checksum&&
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
  if(!strncmp(case_name,"medical-target-",15)) {
   struct player_action action;
   active_players=4;
   for(i=2;i<active_players;i++) {
    players[i]=players[1];players[i].unit_index=unit_handles[i];
    units[i]=units[1];units[i].unit.player_index=player_handles[i];
   }
   units[1].object.bounding_sphere_center.x=1.f;
   units[2].object.bounding_sphere_center.x=.4f;
   units[1].object.body_vitality=units[2].object.body_vitality=-.1f;
   CHECK(extraction_medical_lethal(unit_handles[1])&&extraction_medical_lethal(unit_handles[2]),"crowded medical setup failed");
   advance(60);
   if(!strcmp(case_name,"medical-target-continue")||!strcmp(case_name,"medical-target-interrupt")) {
    units[1].object.bounding_sphere_center.x=5.f;treat(30,FALSE);
    CHECK(medical_players[2].progress==30,"initial treatment failed");
    units[1].object.bounding_sphere_center.x=.2f;
    if(!strcmp(case_name,"medical-target-interrupt"))extraction_medical_damaged(unit_handles[2]);
    treat(1,FALSE);
    if(!strcmp(case_name,"medical-target-continue"))
     CHECK(medical_players[2].progress==31&&!medical_players[1].progress,"lower-slot casualty diverted active treatment");
    else CHECK(medical_players[1].progress==1&&!medical_players[2].progress,"damaged casualty retained treatment instead of allowing another ally's rescue");
   } else {
    if(!strcmp(case_name,"medical-target-busy")) {
     units[1].object.bounding_sphere_center.x=.1f;
     medical_players[1].healer=player_handles[3];medical_players[1].healer_unit=unit_handles[3];
     medical_players[1].last_control=tick;medical_players[1].progress=30;
    }
    treat(1,FALSE);
    CHECK(medical_players[2].progress==1&&!medical_players[1].progress,"player-slot order overrode the nearest available casualty");
    memset(&action,0,sizeof(action));action.control_flags=FLAG(_unit_control_swap_weapons_bit);
    extraction_medical_control(player_handles[0],&action);
    CHECK(medical_players[2].progress==1,"duplicate native control advanced treatment twice in one tick");
   }
  } else if(!strcmp(case_name,"medical-pending-lethal")) {
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
   CHECK(strstr(local_notice_history[(local_notices-1)%4],"Share My Profile"),"Delta host did not get the account requirement");
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
 } else if(!strncmp(case_name,"lan-gear-",9)) {
  byte item[GEAR_ID_BYTES],local_owner[GEAR_ID_BYTES];
  online=FALSE; verified=FALSE;
  if(!strcmp(case_name,"lan-gear-delta"))delta_session=TRUE;
  if(!strcmp(case_name,"lan-gear-virtual"))guest_address=0x64400001UL;
  extraction_reset();advance(1);
  touch(0,1);
  if(strcmp(case_name,"lan-gear-loop")) {
   CHECK(run(1)->phase==_extraction_hub&&!run(1)->gear.opened&&!file_count,"non-LAN or Delta guest received session gear");
  } else {
   CHECK(run(1)->phase==_extraction_deployed&&run(1)->gear.session_only&&!file_count,"private LAN guest cannot deploy without disk storage");
   CHECK(strstr(remote_notice_history[(remote_notices-2)%4],"LAN SESSION STASH"),"guest did not see stash lifetime");
   pickup(1,1);escape(1);
   CHECK(run(1)->gear.record.count==1&&!file_count&&strstr(last_remote_notice,"SESSION GEAR BANKED"),"private LAN escape did not bank once in memory");
   memcpy(item,run(1)->gear.record.items[0].id,sizeof(item));
   touch(0,1);
   CHECK(run(1)->phase==_extraction_deployed&&run(1)->gear.record.reserved_count==1&&
    !memcmp(run(1)->gear.record.reserved[0].id,item,sizeof(item))&&!file_count,"session reissue did not reserve the recovered item");
   gear_profile_owner("owned/profile",local_owner);
   touch(0,0);
   CHECK(run(0)->phase==_extraction_deployed&&!run(0)->gear.session_only&&file_count==1&&
    !memcmp(run(0)->owner,local_owner,sizeof(local_owner)),"LAN host did not retain its separate profile");
   players[1].quit_out_of_game=TRUE;advance(1);
   CHECK(!run(1)->gear.opened&&!run(1)->gear.record.count&&file_count==1,"guest exit retained session gear or wrote a file");
   players[1].quit_out_of_game=FALSE;touch(0,1);
   CHECK(run(1)->phase==_extraction_deployed&&run(1)->gear.session_only&&
    !run(1)->gear.record.reserved_count&&file_count==1,"new connection inherited the earlier stash");
  }
 } else if(!strcmp(case_name,"authority")) {
  client=TRUE; connection=_game_connection_network_client; advance(1); CHECK(!extraction_owns_map() && !file_count,"client storage access");
  client=FALSE; connection=_game_connection_network_server; profile=FALSE; verified=FALSE;
  touch(0,0); touch(0,1); CHECK(!file_count,"unidentified player opened storage");
  CHECK(strstr(local_notice_history[(local_notices-1)%4],"saved player profile"),"local player did not get the profile requirement");
  CHECK(strstr(last_remote_notice,"Share My Profile"),"guest did not get the account requirement");
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
 } else if(!strncmp(case_name,"starter-kit-",12)) {
  struct gear_record original;boolean covenant=!strcmp(case_name,"starter-kit-covenant");
  touch(0,0);pickup(0,1);pickup(0,2);escape(0);original=run(0)->gear.record;
  enable_factions(FALSE);advance(1);touch(covenant?4:3,0);
  input_queues[0].latched_control_flags=FLAG(light?_unit_control_integrated_light_bit:_unit_control_weapon_reload_bit);
  if(!strcmp(case_name,"starter-kit-refund"))fail_confirm=TRUE;
  touch(0,0);
  if(fail_confirm) {
   fail_confirm=fail_write=FALSE;advance(TICKS_PER_SECOND);
   CHECK(run(0)->phase!=_extraction_deployed&&run(0)->gear.record.phase==_gear_idle&&
    !run(0)->gear.record.reserved_count&&run(0)->gear.record.count==2&&
    !memcmp(original.items,run(0)->gear.record.items,2*sizeof(original.items[0])),"failed authored starter grant changed stored gear");
   CHECK(units[0].unit.weapon_object_indices[0]==NONE,"failed starter confirmation left usable issued equipment");
  } else {
   CHECK(run(0)->phase==_extraction_deployed&&run(0)->gear.record.count==2&&!run(0)->gear.record.reserved_count&&
    !memcmp(original.items,run(0)->gear.record.items,2*sizeof(original.items[0])),"faction starter deployment reserved stored gear");
   a=units[0].unit.weapon_object_indices[0];
   CHECK(a!=NONE&&weapon_get(a)->definition_index==(covenant?2:1)&&weapon_get(a)->weapon.magazines[0].rounds_loaded==10,
    "starter-only deployment omitted the authored native faction kit");
  }
 } else if(!strncmp(case_name,"starter-run-",12)) {
  struct gear_record original,other;long carried[2];short j;
  touch(0,0);pickup(0,1);pickup(0,2);escape(0);original=run(0)->gear.record;
  touch(0,1);pickup(1,2);escape(1);other=run(1)->gear.record;
  carried[0]=pickup(0,1);carried[1]=pickup(0,2);units[0].unit.grenade_counts[0]=1;
  input_queues[0].latched_control_flags=FLAG(light?_unit_control_integrated_light_bit:_unit_control_weapon_reload_bit);
  if(!strcmp(case_name,"starter-run-pending"))publish_failure=TRUE;
  if(!strcmp(case_name,"starter-run-storage"))fail_write=TRUE;
  touch(0,0);
  if(publish_failure) {
   CHECK(run(0)->blocked&&run(0)->gear.record.phase==_gear_reserved&&!run(0)->gear.record.reserved_count,"pending starter deployment lost its empty reservation");
   publish_failure=FALSE;input_queues[0].latched_control_flags=0;touch(0,0);
  }
  if(fail_write) {
   fail_write=FALSE;advance(1);
   CHECK(run(0)->phase!=_extraction_deployed&&run(0)->gear.record.count==2&&!run(0)->gear.record.reserved_count,"failed starter issue lost stored gear");
   CHECK(!memcmp(original.items,run(0)->gear.record.items,2*sizeof(original.items[0])),"starter refund changed stored identities");return 0;
  }
  CHECK(run(0)->phase==_extraction_deployed&&run(0)->gear.record.count==2&&!run(0)->gear.record.reserved_count&&
   !run(0)->gear.record.reserved_grenades[0]&&!run(0)->gear.record.reserved_grenades[1]&&
   !memcmp(original.items,run(0)->gear.record.items,2*sizeof(original.items[0]))&&!memcmp(original.grenades,run(0)->gear.record.grenades,sizeof(original.grenades)),"starter deployment reserved protected gear");
  for(j=0;j<2;j++)CHECK(units[0].unit.weapon_object_indices[j]==carried[j]&&weapon_try_and_get(carried[j]),"starter deployment dropped/replaced carried native gear");
  CHECK(units[0].unit.grenade_counts[0]==1&&!memcmp(&other,&run(1)->gear.record,sizeof(other)),"starter choice changed carried grenades or another owner");
  if(!strcmp(case_name,"starter-run-loss")||!strcmp(case_name,"starter-run-disconnect")) {
   if(!strcmp(case_name,"starter-run-loss"))units[0].object.damage_flags=FLAG(_object_die_act_of_god_bit);
   else players[0].quit_out_of_game=TRUE;
   advance(1);extraction_reset();units[0].object.damage_flags=0;players[0].quit_out_of_game=FALSE;advance(1);touch(2,0);
   CHECK(run(0)->gear.record.count==2&&!memcmp(original.items,run(0)->gear.record.items,2*sizeof(original.items[0]))&&
    !memcmp(original.grenades,run(0)->gear.record.grenades,sizeof(original.grenades)),"failed starter run changed banked gear");
  } else {
   weapon_get(carried[0])->weapon.magazines[0].rounds_loaded=3;escape(0);
   CHECK(run(0)->gear.record.count==4&&!memcmp(original.items,run(0)->gear.record.items+2,2*sizeof(original.items[0])),"successful starter loot did not accumulate alongside stored gear");
   input_queues[0].latched_control_flags=0;touch(2,0);input_queues[0].latched_control_flags=FLAG(light?_unit_control_integrated_light_bit:_unit_control_weapon_reload_bit);touch(2,0);
   input_queues[0].latched_control_flags=0;touch(0,0);
   CHECK(run(0)->gear.record.reserved_count==2&&run(0)->gear.record.count==2&&
    !memcmp(original.items[0].id,run(0)->gear.record.reserved[1].id,GEAR_ID_BYTES)&&
    weapon_get(units[0].unit.weapon_object_indices[0])->weapon.magazines[0].rounds_loaded==3,"accumulated gear could not be selected and redeployed");
  }
 } else if(!strncmp(case_name,"loadout-secondary",17)) {
  byte primary[GEAR_ID_BYTES],secondary[GEAR_ID_BYTES];struct gear_record other;
  unsigned long generation;
  touch(0,0);pickup(0,1);pickup(0,2);a=pickup(0,1);weapon_get(a)->weapon.magazines[0].rounds_loaded=3;escape(0);
  touch(0,1);pickup(1,2);escape(1);other=run(1)->gear.record;
  memcpy(primary,run(0)->gear.record.items[0].id,sizeof(primary));memcpy(secondary,run(0)->gear.record.items[2].id,sizeof(secondary));
  input_queues[0].latched_control_flags=FLAG(light?_unit_control_integrated_light_bit:_unit_control_weapon_reload_bit);touch(2,0);
  CHECK(!memcmp(primary,run(0)->gear.record.items[0].id,sizeof(primary)),"first preview reordered primary");
  if(!strcmp(case_name,"loadout-secondary-unavailable")||!strcmp(case_name,"loadout-secondary-release")) {
   if(!strcmp(case_name,"loadout-secondary-unavailable")) {
    CHECK(update_client_loadout_modifier_held(player_handles[0])&&!update_client_loadout_modifier_held(player_handles[1])&&
     !update_client_loadout_modifier_held(NONE)&&!update_client_loadout_modifier_held(player_handles[0]+0x10000),"queue modifier crossed full player handles");
    update_client_globals.queues=NULL;CHECK(!update_client_loadout_modifier_held(player_handles[0]),"absent queue published a modifier");
    update_client_globals.queues=input_queues;update_client_globals.initialized=FALSE;
   }
   else input_queues[0].latched_control_flags=0;
   touch(2,0);
   CHECK(memcmp(primary,run(0)->gear.record.items[0].id,sizeof(primary)),"unavailable/released reload modifier changed the secondary");
   return 0;
  }
  generation=run(0)->gear.record.generation;
  if(!strcmp(case_name,"loadout-secondary-pending"))publish_failure=TRUE;
  touch(2,0);
  if(publish_failure) {
   CHECK(run(0)->blocked&&run(0)->gear.pending,"uncertain secondary did not block deployment");
   publish_failure=FALSE;input_queues[0].latched_control_flags=0;touch(2,0);
  }
  CHECK(run(0)->gear.record.generation==generation+1&&!run(0)->gear.pending&&!run(0)->blocked&&
   !memcmp(primary,run(0)->gear.record.items[0].id,sizeof(primary))&&!memcmp(secondary,run(0)->gear.record.items[1].id,sizeof(secondary)),"terminal changed primary, lost secondary or advanced retry twice");
  CHECK(!memcmp(&other,&run(1)->gear.record,sizeof(other)),"another player's selected equipment changed");
  CHECK(strstr(local_notice_history[(local_notices-2)%4],"second"),"secondary interaction is undiscoverable");
  input_queues[0].latched_control_flags=0;units[0].unit.control_flags=0;extraction_reset();advance(1);touch(0,0);
  CHECK(weapon_get(units[0].unit.weapon_object_indices[0])->definition_index==1&&
   weapon_get(units[0].unit.weapon_object_indices[1])->definition_index==1&&
   weapon_get(units[0].unit.weapon_object_indices[1])->weapon.magazines[0].rounds_loaded==3,"native grant/reopen ignored secondary selection");
  escape(0);extraction_reset();advance(1);touch(0,0);
  CHECK(run(0)->gear.record.reserved_count==2&&!memcmp(secondary,run(0)->gear.record.reserved[1].id,sizeof(secondary)),"recovery/redeployment changed secondary identity");
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
  CHECK(gear_loadout(&run(0)->gear,&chosen,TRUE) && chosen.weapon_count==2 &&
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
