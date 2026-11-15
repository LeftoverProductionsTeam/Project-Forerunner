/* Actual native phase/context and Delta routing, over bounded world adapters.
 * No microphone, audio hardware, packet transport or live game is simulated. */
#include "harness.h"
#include <math.h>
#include "../../../port/linux/src/delta_peer.h"
#include "types.inc"
#define HALO_PORT_MAXIMUM_NETWORK_PLAYERS 4
#define csmemset memset
enum { MACHINES=DELTA_PEER_MAXIMUM_MACHINES,_scenario_type_solo=0 };
struct network_game_client { short state; };
struct network_player { short machine_index,team_index;boolean valid; };
struct network_game {
 struct { struct { boolean teams; } universal_variant; } variant;
 struct network_player players[HALO_PORT_MAXIMUM_NETWORK_PLAYERS];
};
struct player_datum {
 long unit_index;boolean quit_out_of_game;
 struct { long player_list_index; } network_player_data;
};
struct unit_datum {
 struct { real_point3d position;unsigned long damage_flags,mask; } object;
 struct { long player_index; } unit;
};
struct data_iterator { short next;long datum_index; };
static struct network_game_client client;
static struct network_game_client *session=&client;
static struct network_game game;
static struct player_datum players[2];
static struct unit_datum units[2];
static struct { short type; } scenario={_scenario_type_solo},*global_scenario=&scenario;
static struct delta_peer_voice_world published;
static struct delta_wire_voice_config rules;
static long const handles[2]={0x20010,0x20011},owners[2]={0x10000,0x10001};
static int storage,origin_calls;
static void *player_data=&storage,*game_engine;
static boolean started=TRUE,postgame;

static struct network_game_client *global_network_game_client_get(void) {return session;}
static boolean game_in_progress(void) {return started;}
static boolean game_engine_showing_postgame(void) {return postgame;}
static short game_connection(void) {return _game_connection_network_server;}
static struct network_game *network_game_get_game(void) {return &game;}
static boolean network_player_is_valid(struct network_player const *player) {return player->valid;}
static void data_iterator_new(struct data_iterator *it,void *data) {CHECK(data,"iterated absent player data");it->next=0;}
static void *data_iterator_next(struct data_iterator *it) {
 short i=it->next++;if(i>=2)return NULL;it->datum_index=owners[i];return &players[i];
}
static void *object_try_and_get_and_verify_type(long handle,unsigned long mask) {
 short i;for(i=0;i<2;i++)if(handle==handles[i]&&(units[i].object.mask&mask))return &units[i];return NULL;
}
static void object_get_origin(long handle,real_point3d *point) {
 short i;for(i=0;i<2;i++)if(handle==handles[i]){origin_calls++;*point=units[i].object.position;return;}
 CHECK(FALSE,"stale object origin dereference");
}
static boolean valid_real_point3d(real_point3d const *point) {
 return isfinite(point->x)&&isfinite(point->y)&&isfinite(point->z);
}
static void delta_peer_game_set_voice_world(struct delta_peer_voice_world const *world) {published=*world;}
#include "under_test.inc"
#include "route.inc"

static int route(int channel) {return delta_peer_voice_route(&rules,&published,0,1,channel);}
int main(int argc,char **argv) {
 char const *name=argv[1];short i;boolean unavailable=FALSE,lobby=FALSE;
 client.state=_network_game_client_state_ingame;
 rules.channels=DELTA_VOICE_TEAM|DELTA_VOICE_PROXIMITY|DELTA_VOICE_LOBBY;
 rules.proximity=150;
 for(i=0;i<2;i++) {
  game.players[i].valid=TRUE;game.players[i].machine_index=i;game.players[i].team_index=i;
  players[i].unit_index=handles[i];players[i].network_player_data.player_list_index=i;
  units[i].unit.player_index=owners[i];units[i].object.mask=_object_mask_biped;
 }
 units[1].object.position.x=5.f;
 if(!strcmp(name,"coop-far"))units[1].object.position.x=50.f;
 else if(!strcmp(name,"pregame")){client.state=_network_game_client_state_pregame;lobby=TRUE;}
 else if(!strcmp(name,"postgame")){client.state=_network_game_client_state_postgame;lobby=TRUE;}
 else if(!strcmp(name,"mp-postgame")){game_engine=&storage;postgame=TRUE;lobby=TRUE;}
 else if(!strcmp(name,"no-session")){session=NULL;lobby=TRUE;}
 else if(!strcmp(name,"no-world")){started=FALSE;lobby=TRUE;}
 else if(!strcmp(name,"no-players")){player_data=NULL;unavailable=TRUE;}
 else if(!strcmp(name,"dead")){units[0].object.damage_flags=FLAG(_object_dead_bit);unavailable=TRUE;}
 else if(!strcmp(name,"stale")){units[0].unit.player_index+=0x10000;unavailable=TRUE;}
 else if(!strcmp(name,"missing")){players[0].unit_index=NONE;unavailable=TRUE;}
 else if(!strcmp(name,"wrong-type")){units[0].object.mask=_object_mask_scenery;unavailable=TRUE;}
 else if(!strcmp(name,"quit")){players[0].quit_out_of_game=TRUE;unavailable=TRUE;}
 else if(!strcmp(name,"invalid-position")){units[0].object.position.x=NAN;unavailable=TRUE;}
 else if(!strcmp(name,"vehicle"))units[0].object.mask=_object_mask_vehicle;
 else if(!strcmp(name,"enemy-proximity")){game_engine=&storage;game.variant.universal_variant.teams=TRUE;}
 else if(!strcmp(name,"disabled"))rules.channels&=~DELTA_VOICE_PROXIMITY;
 else if(strcmp(name,"coop-near"))return 2;
 host_world();
 CHECK(published.in_game==!lobby&&voice_in_lobby()==lobby,"native co-op/menu phase classified incorrectly");
 if(lobby) {
  CHECK(route(_delta_voice_channel_proximity)==_delta_voice_route_global,"allowed lobby voice disappeared");
  rules.channels&=~DELTA_VOICE_LOBBY;
  CHECK(route(_delta_voice_channel_proximity)==_delta_voice_route_none,"disabled lobby voice relayed");
 } else {
  CHECK(published.members[0].alive==!unavailable,"dead/stale/unlocated speaker marked alive");
  CHECK(route(_delta_voice_channel_proximity)==((unavailable||!strcmp(name,"coop-far")||!strcmp(name,"disabled"))?
   _delta_voice_route_none:_delta_voice_route_proximity),"native proximity range/ownership/permission bypassed");
  CHECK(route(_delta_voice_channel_team)==(!strcmp(name,"enemy-proximity")?_delta_voice_route_none:_delta_voice_route_global),
   "radio changed with proximity or leaked to the enemy team");
 }
 return 0;
}
