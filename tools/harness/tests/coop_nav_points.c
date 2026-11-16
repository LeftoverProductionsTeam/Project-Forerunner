/* Real HUD activation/clearing and co-op cache/replay against bounded viewers. */
#include "harness.h"
#define MAXIMUM_LOCAL_PLAYERS 4
#define MAXIMUM_NUMBER_OF_LOCAL_PLAYERS 4
#define VALID_INDEX(i,n) ((i)>=0&&(i)<(n))
#define _object_mask_all 1
#define csmemset memset
#include "types.inc"
struct tag_block {long count;void *address;};
struct player_datum {short local_player_index;long team_index,unit_index;boolean quit_out_of_game;};
static struct player_datum players[3]={{0,1,0x10010,FALSE},{1,3,0x10011,FALSE},{NONE,2,0x10012,FALSE}};
static long player_handles[3]={0x10000,0x10001,0x10002};
static struct hud_nav_point_player_datum views[MAXIMUM_LOCAL_PLAYERS];
static struct {struct {struct tag_block arrows;} waypoint;} hud_definition;
static void *object;
static boolean stale,deleted;
static int role=1,sent;
static struct {struct tag_block cutscene_flags;} scenario;
static struct {short objective,nav_point_count,nav_refresh_next,nav_refresh_remaining;struct {short kind,nav_index;long target,marker;real vertical_offset;} nav_points[MAXIMUM_NAV_POINTS];} host_hud_state;
static struct {boolean refresh,joined;} host_resend;
enum {_coop_event_hud,_coop_event_nav_point};
enum {_coop_hud_objective};
struct distributed_coop_event {byte type;short value;long target,object_index;real reals[1];};
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
{if(role!=1||queue_used==queue_limit)return NULL;if(kind==_coop_event_nav_point)sent++;return &events[queue_used++];}
#define global_scenario_get() (&scenario)
#define hud_globals (&hud_definition)
static void *object_try_and_get_and_verify_type(long index,int mask)
{(void)mask;return !deleted&&index==0x9001?&object:NULL;}
static void error(int level,char const *text){(void)level;CHECK(FALSE,"unexpected native HUD error: %s",text);}
static void network_coop_note_nav_point(short kind,short nav,long target,long marker,real offset);
#include "under_test.inc"
static int contains(short local,long marker)
{int i;for(i=0;i<MAXIMUM_NUMBER_OF_NAV_POINTS;i++)if(views[local].nav_points[i].type!=NONE&&views[local].nav_points[i].reference_index==marker)return 1;return 0;}
int main(int argc,char **argv)
{
 char const *name=argc>1?argv[1]:"host";struct hud_nav_point_player_datum other;int i;
 memset(views,NONE,sizeof(views));scenario.cutscene_flags.count=3;hud_definition.waypoint.arrows.count=2;host_hud_state.objective=NONE;
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
