/* Real HUD activation/clearing and co-op cache/replay against bounded viewers. */
#include "harness.h"
#define MAXIMUM_LOCAL_PLAYERS 4
#define MAXIMUM_NUMBER_OF_LOCAL_PLAYERS 4
#define VALID_INDEX(i,n) ((i)>=0&&(i)<(n))
#define _object_mask_all 1
#define _object_mask_unit 2
#define csmemset memset
#define NUMBEROF(a) (sizeof(a)/sizeof((a)[0]))
#define HALO_PORT_MAXIMUM_NETWORK_MACHINES 2
#define MAXIMUM_TRACKED_PLAYERS HALO_PORT_MAXIMUM_NETWORK_PLAYERS
#define MAXIMUM_LOCAL_PLAYERS 4
#define _error_silent 0
#include "types.inc"
struct tag_block {long count;void *address;};
struct player_datum {short local_player_index;long team_index,unit_index;boolean quit_out_of_game;};
static struct player_datum players[3]={{0,1,0x10010,FALSE},{1,3,0x10011,FALSE},{NONE,2,0x10012,FALSE}};
static long player_handles[3]={0x10000,0x10001,0x10002};
static struct hud_nav_point_player_datum views[MAXIMUM_LOCAL_PLAYERS];
static struct {struct {struct tag_block arrows;} waypoint;} hud_definition;
static struct object_datum {long definition_index;struct {short name_index;} object;} object={0x4001,{NONE}};
static boolean stale,deleted;
static int role=1,sent;
static struct scenario {struct tag_block cutscene_flags;} scenario_state;
static struct {short objective,nav_point_count,nav_refresh_next,nav_refresh_remaining;struct {short kind,nav_index;long target,marker;real vertical_offset;} nav_points[MAXIMUM_NAV_POINTS];} host_hud_state,checkpoint_hud_state;
static struct coop_checkpoint_players checkpoint_players;
static struct {boolean refresh,joined;} host_resend;
enum {_coop_event_hud,_coop_event_nav_point,_coop_event_reverted};
enum {_coop_hud_objective};
struct distributed_coop_event {byte type;short value;long target,object_index;real reals[1];short name_index;long definition_index;};
static struct distributed_coop_event events[MAXIMUM_QUEUED_EVENTS];
static int queue_used,queue_limit=MAXIMUM_QUEUED_EVENTS;
static struct {struct {long player,team;boolean seen;} nav_players[MAXIMUM_LOCAL_PLAYERS];} coop_presentation;
struct data_iterator {int slot;long datum_index;};
static void *player_data;
static void data_iterator_new(struct data_iterator *iterator,void *data){(void)data;iterator->slot=NONE;}
static void *data_iterator_next(struct data_iterator *iterator){int i=++iterator->slot;if(i==3)return NULL;iterator->datum_index=player_handles[i];return &players[i];}
static struct player_datum *player_try_and_get(long index)
{int i;for(i=0;i<3;i++)if(player_handles[i]==index)return &players[i];return NULL;}
static struct player_datum *player_get(long index)
{struct player_datum *p=player_try_and_get(index);CHECK(p,"invalid native player");return p;}
static short local_player_get_next(short previous){return previous<1?(short)(previous+1):NONE;}
static long local_player_get_player_index(short local){return player_handles[local]+(stale&&local==0?0x10000:0);}
static struct hud_nav_point_player_datum *get_nav_point_datum(short local)
{CHECK(VALID_INDEX(local,MAXIMUM_LOCAL_PLAYERS),"out of bounds local HUD");return &views[local];}
static boolean coop_host(void){return role==1;}
static boolean coop_client(void){return role==2;}
static struct distributed_coop_event *event_new(byte kind)
{struct distributed_coop_event *e;if(role!=1||queue_used==queue_limit)return NULL;if(kind==_coop_event_nav_point)sent++;
 e=&events[queue_used++];memset(e,0,sizeof(*e));e->name_index=NONE;e->definition_index=NONE;return e;}
#define global_scenario_get() (&scenario_state)
#define hud_globals (&hud_definition)
static void *object_try_and_get_and_verify_type(long index,int mask)
{int i;for(i=0;i<3;i++)if(index==players[i].unit_index)return &object;
 return mask!=_object_mask_unit&&!deleted&&index==0x9001?&object:NULL;}
struct object_iterator {long index;};
static long object_index_from_name_index(short name){return name==7&&!deleted?0x9001:NONE;}
static void object_iterator_new(struct object_iterator *it,unsigned long mask,int flags){it->index=NONE;}
static struct object_datum *object_iterator_next(struct object_iterator *it){return NULL;}
static long player_index_from_unit_index(long unit)
{int i;for(i=0;i<3;i++)if(unit==players[i].unit_index)return player_handles[i];return NONE;}
static void error(int level,char const *text,...){(void)level;CHECK(!strncmp(text,"co-op: reverted",15),"unexpected native HUD error: %s",text);}
static long machine_players[HALO_PORT_MAXIMUM_NETWORK_MACHINES][MAXIMUM_LOCAL_PLAYERS];
static long *machine_get_player_list(short machine){return machine_players[machine];}
static int coop_events,client_devices,client_names_differing[1],host_player_structure_bsps[1];
static struct {int offered,voters;} skip_vote;
static void skip_vote_clear(void){}
static void client_stop_script_sounds(void){}
static void client_cinematic_end(void){}
static void client_host_camera_set(boolean enabled){CHECK(!enabled,"recovery enabled the discarded camera");}
enum {_game_connection_network_client=1,_game_connection_network_server};
static short game_connection(void){return role==2?_game_connection_network_client:_game_connection_network_server;}
static long clock_time=1000;
static long game_time_get(void){return clock_time;}
static void game_time_set_distributed(long now){clock_time=now;}
static void update_queues_reset_and_fill_with_lies(void){}
static void game_state_port_restamp_revert_time(void){}
static void hs_runtime_port_shift_sleep_times(long ticks){}
static void network_distributed_checkpoint_boundary(void){}
static void coop_enemies_reset(void){}
static void network_distributed_reverted(void){}
static void network_coop_note_nav_point(short kind,short nav,long target,long marker,real offset);
#include "under_test.inc"
typedef void (*object_deleted_proc)(long);
static int object_deletes,ai_deletes,player_deletes;
static void objects_fix_for_deleted_object(long index) {object_deletes++;}
static void ai_handle_deleted_object(long index) {ai_deletes++;}
static void players_handle_deleted_object(long index) {player_deletes++;}
#include "deleted.inc"
static int contains(short local,long marker)
{int i;for(i=0;i<MAXIMUM_NUMBER_OF_NAV_POINTS;i++)if(views[local].nav_points[i].type!=NONE&&views[local].nav_points[i].reference_index==marker)return 1;return 0;}
int main(int argc,char **argv)
{
 char const *name=argc>1?argv[1]:"host";struct hud_nav_point_player_datum other;int i;
 memset(views,NONE,sizeof(views));scenario_state.cutscene_flags.count=3;hud_definition.waypoint.arrows.count=2;host_hud_state.objective=NONE;
 if(!strncmp(name,"checkpoint-",11)){
  if(!strcmp(name+11,"client")||!strcmp(name+11,"modern-client")){
   role=2;
   hud_unit_activate_nav_point_with_flag(0,players[0].unit_index,1,1.f);
   hud_unit_activate_nav_point_with_object(0,players[1].unit_index,0x9001,1.f);
   if(!strcmp(name+11,"modern-client"))network_coop_reset_recovery();else network_coop_checkpoint_reset();
   CHECK(!host_hud_state.nav_point_count&&!contains(0,1)&&!contains(1,0x9001),"checkpoint epoch retained discarded client guidance");
   hud_unit_activate_nav_point_with_flag(0,players[1].unit_index,2,1.f);
   CHECK(host_hud_state.nav_point_count==1&&contains(1,2)&&!contains(1,0x9001),"restored host guidance did not replace discarded markers");
  }else if(!strcmp(name+11,"roster")){
   network_coop_note_nav_point(_coop_nav_point_unit_flag,0,players[1].unit_index,1,1.f);
   network_coop_note_nav_point(_coop_nav_point_team_flag,0,3,2,1.f);
   network_coop_checkpoint_saved();
   players[1].unit_index+=0x10000;network_coop_reverted(clock_time);
   CHECK(host_hud_state.nav_point_count==1&&host_hud_state.nav_points[0].kind==_coop_nav_point_team_flag,
         "restoration resurrected a departed viewer's binding or discarded a valid team flag");
  }else{
   host_hud_state.objective=1;
   network_coop_note_nav_point(_coop_nav_point_unit_flag,0,players[1].unit_index,1,1.f);
   network_coop_checkpoint_saved();
   host_hud_state.objective=2;
   network_coop_note_nav_point(_coop_nav_point_unit_flag,NONE,players[1].unit_index,1,0);
   network_coop_note_nav_point(_coop_nav_point_unit_object,0,players[1].unit_index,0x9001,1.f);
   host_hud_state.nav_refresh_next=host_hud_state.nav_refresh_remaining=1;
   network_coop_reverted(clock_time);
   CHECK(host_hud_state.objective==1&&host_hud_state.nav_point_count==1&&host_hud_state.nav_points[0].marker==1,
         "revert replayed the discarded timeline instead of checkpoint guidance");
   CHECK(!host_hud_state.nav_refresh_next&&!host_hud_state.nav_refresh_remaining,"revert retained a discarded replay cursor");
   role=2;host_hud_state.objective=3;network_coop_checkpoint_saved();role=1;network_coop_reverted(clock_time);
   CHECK(host_hud_state.objective==1,"a client overwrote the host checkpoint guidance");
  }
  return 0;
 }
 if(!strncmp(name,"ordering-",9)){
  struct distributed_coop_event event={_coop_nav_point_team_object,0,3,0x9001,{1.f},NONE,NONE};
  role=2;
  if(!strcmp(name+9,"named")){
   object.object.name_index=7;event.object_index=0xa001;event.name_index=7;event.definition_index=object.definition_index;
   client_apply_nav_point(&event);
   CHECK(host_hud_state.nav_point_count==1&&contains(1,0x9001),"named scenario guidance used the host's different local handle");
   event.value=NONE;client_apply_nav_point(&event);
   CHECK(!host_hud_state.nav_point_count&&!contains(1,0x9001),"named guidance deactivation did not use the local binding");
   event.value=0;deleted=TRUE;client_apply_nav_point(&event);
   CHECK(!host_hud_state.nav_point_count,"uncreated named marker was retained");
   deleted=FALSE;event.definition_index++;client_apply_nav_point(&event);
   CHECK(!host_hud_state.nav_point_count,"named guidance accepted a different definition");
   event.definition_index--;client_apply_nav_point(&event);
   CHECK(host_hud_state.nav_point_count==1&&contains(1,0x9001),"host replay did not restore named guidance after creation");
   role=1;queue_used=0;CHECK(send_nav_point(_coop_nav_point_unit_object,0,players[1].unit_index,0x9001,1.f),"named send failed");
   CHECK(events[0].name_index==7&&events[0].definition_index==object.definition_index,"named guidance omitted native identity metadata");
  }else if(!strcmp(name+9,"pending")){
   deleted=TRUE;client_apply_nav_point(&event);
   CHECK(!host_hud_state.nav_point_count&&!contains(1,0x9001),"an event before object creation retained an unreachable marker");
   deleted=FALSE;client_apply_nav_point(&event);
   CHECK(host_hud_state.nav_point_count==1&&contains(1,0x9001),"normal host replay did not restore guidance after replica creation");
  }else{
   client_apply_nav_point(&event);deleted=TRUE;event.value=NONE;client_apply_nav_point(&event);
   CHECK(!host_hud_state.nav_point_count&&!contains(1,0x9001),"deactivation required a still-existing marker object");
  }
  return 0;
 }
 if(!strncmp(name,"lifetime-",9)){
  char const *mode=name+9;long old=0x10010,fresh=0x20010;
  if(!strcmp(mode,"progress")){
   boolean seen[24]={0};int unique=0,tick,j;
   role=2;for(i=0;i<24;i++)network_coop_note_nav_point(_coop_nav_point_team_object,0,2,0x9100+i,0);
   role=1;host_resend.refresh=TRUE;queue_used=0;host_send_hud_state();
   for(i=0;i<queue_used;i++)seen[events[i].object_index-0x9100]=TRUE;
   /* Deleting an already-sent entry swaps an unseen tail behind the cursor. */
   object_deleted_procs_call(0x9105);seen[5]=FALSE;host_resend.refresh=FALSE;
   for(tick=0;tick<8&&host_hud_state.nav_refresh_remaining;tick++){
    queue_used=0;host_send_hud_state();CHECK(queue_used<=8,"cleanup exceeded the existing replay budget");
    for(j=0;j<queue_used;j++){
     i=events[j].object_index-0x9100;CHECK(i!=5&&i>=0&&i<24,"deleted/invalid marker replayed");seen[i]=TRUE;
    }
   }
   for(i=0;i<24;i++)unique+=seen[i];
   CHECK(unique==23&&!host_hud_state.nav_refresh_remaining,"native deletion lost a moved tail's late-join guidance");return 0;
  }
  if(!strcmp(mode,"client"))role=2;
  if(!strcmp(mode,"ordinary"))role=0;
  if(!strcmp(mode,"host")||!strcmp(mode,"client")){
   /* Repeated native respawns exceed the replay cache without increasing it. */
   for(i=0;i<MAXIMUM_NAV_POINTS*2;i++){
    long body=0x30000+i;
    network_coop_note_nav_point(_coop_nav_point_unit_flag,0,body,0,0);
    CHECK(host_hud_state.nav_point_count==1,"native body replacement exhausted the retained guidance cache");
    object_deleted_procs_call(body);
   }
   network_coop_note_nav_point(_coop_nav_point_unit_flag,1,fresh,1,0);
   CHECK(host_hud_state.nav_point_count==1&&host_hud_state.nav_points[0].target==fresh,
         "fresh participant guidance was not retained after deletion");
   CHECK(object_deletes==MAXIMUM_NAV_POINTS*2&&ai_deletes==object_deletes&&player_deletes==object_deletes,
         "existing native deletion callbacks changed");
   return 0;
  }
  network_coop_note_nav_point(_coop_nav_point_team_flag,0,2,1,.5f);
  network_coop_note_nav_point(_coop_nav_point_unit_flag,0,old,0,.5f);
  network_coop_note_nav_point(_coop_nav_point_unit_flag,1,fresh,1,1.f);
  network_coop_note_nav_point(_coop_nav_point_team_object,0,2,old,0);
  network_coop_note_nav_point(_coop_nav_point_unit_object,1,fresh,old,0);
  if(!strcmp(mode,"ordinary")){
   object_deleted_procs_call(old);CHECK(!host_hud_state.nav_point_count&&object_deletes==1&&ai_deletes==1&&player_deletes==1,
                                      "ordinary native deletion acquired co-op state or skipped its original callbacks");return 0;
  }
  if(!strcmp(mode,"handles")){
   object_deleted_procs_call(NONE);object_deleted_procs_call(1);
   CHECK(host_hud_state.nav_point_count==5,"a flag index/NONE was treated as a full object handle");
  }
  host_resend.refresh=TRUE;queue_used=0;host_send_hud_state();
  object_deleted_procs_call(old);
  CHECK(host_hud_state.nav_point_count==2,"deleted viewer/target left stale replay entries");
  for(i=0;i<2;i++)CHECK((host_hud_state.nav_points[i].kind==_coop_nav_point_team_flag&&host_hud_state.nav_points[i].marker==1)||
                      (host_hud_state.nav_points[i].kind==_coop_nav_point_unit_flag&&host_hud_state.nav_points[i].target==fresh),
                      "native flags or the same slot's new full handle were discarded");
  if(!strcmp(mode,"replay")){
   queue_used=0;host_resend.refresh=TRUE;host_send_hud_state();
   CHECK(queue_used==2,"late join replay retained dead marker bindings");
   for(i=0;i<queue_used;i++)CHECK(events[i].target!=old&&events[i].object_index!=old,"a deleted object was replayed");
  }
  return 0;
 }
 if(!strcmp(name,"replay")||!strcmp(name,"congested")||!strcmp(name,"shrinking")||!strcmp(name,"removed")){
  int count=!strcmp(name,"replay")||!strcmp(name,"congested")?MAXIMUM_NAV_POINTS:24;
  int seen[MAXIMUM_NAV_POINTS]={0},tick,unique=0,j,expected=count;
  role=2;for(i=0;i<count;i++)network_coop_note_nav_point(_coop_nav_point_team_object,0,2,0x9100+i,0);
  role=1;if(!strcmp(name,"congested"))queue_limit=3;
  for(tick=0;tick<MAXIMUM_NAV_POINTS;tick++){
   if(tick==1&&(!strcmp(name,"shrinking")||!strcmp(name,"removed"))){
    expected=!strcmp(name,"removed")?0:2;
    for(i=expected;i<count;i++)network_coop_note_nav_point(_coop_nav_point_team_object,NONE,2,0x9100+i,0);
    memset(seen,0,sizeof(seen));unique=0;
   }
   queue_used=0;host_resend.refresh=tick==0||(!strcmp(name,"congested")&&tick%60==0);
   host_send_hud_state();
   CHECK(queue_used<=8,"marker replay monopolized the shared mission event queue");
   for(j=0;j<queue_used;j++){
    i=(int)(events[j].object_index-0x9100);
    CHECK(i>=0&&i<expected,"removed or invalid marker was replayed");
    if(!seen[i]){seen[i]=1;unique++;}
   }
   if(tick>0&&unique==expected)break;
  }
  CHECK(unique==expected,"busy event queue lost late squad guidance (%d/%d)",unique,expected);
  if(!strcmp(name,"removed"))CHECK(!host_hud_state.nav_refresh_remaining,"empty cache retained pending replay");
  return 0;
 }
 if(!strncmp(name,"squads-",7)){
  if(!strcmp(name,"squads-client"))role=2;
  for(i=0;i<12;i++){
   network_coop_note_nav_point(_coop_nav_point_unit_flag,0,0x10010+i,0,1.f);
   network_coop_note_nav_point(_coop_nav_point_unit_flag,1,0x10010+i,1,2.f);
  }
  CHECK(host_hud_state.nav_point_count==24,"two six-member squads lost native objective bindings (%d/24)",host_hud_state.nav_point_count);
  players[0].unit_index=0x1001b;coop_refresh_nav_points();players[0].team_index=3;coop_refresh_nav_points();
  CHECK(contains(0,0)&&contains(0,1),"the last squad member lost its own objective guidance");
  CHECK(!contains(1,0)&&!contains(1,1),"unit guidance leaked into an unrelated local viewer");
  CHECK(sent==(role==1?24:0),"client cache sent authoritative events");
  return 0;
 }
 if(!strcmp(name,"client"))role=2;
 if(!strcmp(name,"legacy"))role=0;
 hud_activate_team_nav_point_with_flag(0,2,0,.5f);
 hud_activate_team_nav_point_with_flag(1,3,1,1.f);
 if(!strcmp(name,"legacy")){CHECK(!host_hud_state.nav_point_count&&!sent,"ordinary native play acquired a co-op cache");return 0;}
 CHECK(host_hud_state.nav_point_count==2,"received host markers were not retained on the client");
 CHECK(sent==(role==1?2:0),"client cache sent authoritative events");
 coop_refresh_nav_points();CHECK(!contains(0,0)&&!contains(0,1)&&contains(1,1),"initial team guidance mismatch");
 players[0].team_index=2;coop_refresh_nav_points();
 CHECK(contains(0,0)&&!contains(0,1),"faction selection failed to show only its native marker");
 CHECK(!contains(1,0)&&contains(1,1),"one local viewer changed the other's faction guidance");
 if(!strcmp(name,"host")||!strcmp(name,"client"))return 0;
 other=views[1];
 if(!strcmp(name,"switch")){
  players[0].team_index=3;coop_refresh_nav_points();CHECK(!contains(0,0)&&contains(0,1),"old faction marker survived selection");
 }else if(!strcmp(name,"respawn")||!strcmp(name,"unit")){
  network_coop_note_nav_point(_coop_nav_point_unit_flag,0,players[0].unit_index,2,0);
  hud_activate_nav_point_with_flag(0,player_handles[0],2,0);
  players[0].unit_index=0x20010;coop_refresh_nav_points();
  CHECK(contains(0,0)&&contains(0,2),"same-team respawn changed native mission guidance");
  if(!strcmp(name,"unit")){
   network_coop_note_nav_point(_coop_nav_point_unit_flag,0,players[0].unit_index,2,0);
   players[0].unit_index=0x30010;players[0].unit_index=0x20010;players[0].team_index=3;coop_refresh_nav_points();
   CHECK(contains(0,2)&&contains(0,1)&&!contains(0,0),"current explicit unit marker was lost during team refresh");
  }
 }else if(!strcmp(name,"deactivate")){
  hud_deactivate_team_nav_point_with_flag(2,0);
  players[0].team_index=3;coop_refresh_nav_points();players[0].team_index=2;coop_refresh_nav_points();
  CHECK(!contains(0,0),"deactivated objective resurrected after faction changes");
 }else if(!strcmp(name,"stale")){
  struct hud_nav_point_player_datum before=views[0];stale=TRUE;players[0].team_index=3;coop_refresh_nav_points();
  CHECK(!memcmp(&before,&views[0],sizeof(before)),"stale local player handle changed a valid HUD");
 }else if(!strcmp(name,"deleted")){
  network_coop_note_nav_point(_coop_nav_point_team_object,0,2,0x9001,0);
  deleted=TRUE;players[0].team_index=3;coop_refresh_nav_points();players[0].team_index=2;coop_refresh_nav_points();CHECK(!contains(0,0x9001)&&contains(0,0),"deleted object replayed or suppressed valid guidance");
 }else if(!strcmp(name,"restore")){
  hud_activate_nav_point_with_flag(0,player_handles[0],2,0);
  memset(&coop_presentation,0,sizeof(coop_presentation));coop_refresh_nav_points();
  CHECK(contains(0,2),"recovery erased native checkpoint-restored guidance");
 }else if(!strcmp(name,"unchanged")){
  views[0].nav_points[0].reference_index=12345;views[0].nav_points[0].type=0;coop_refresh_nav_points();
  CHECK(contains(0,12345),"unchanged identity unnecessarily cleared the native HUD");
 }else if(!strcmp(name,"capacity")){
  for(i=0;i<MAXIMUM_NAV_POINTS+5;i++)network_coop_note_nav_point(_coop_nav_point_team_object,0,2,0x9100+i,0);
  CHECK(host_hud_state.nav_point_count==MAXIMUM_NAV_POINTS,"bounded marker cache overflowed");
 }else CHECK(FALSE,"unknown case %s",name);
 CHECK(!memcmp(&other,&views[1],sizeof(other)),"refresh rewrote an unrelated split-screen viewer");
 return 0;
}
