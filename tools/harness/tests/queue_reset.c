#include "harness.h"
#undef match_assert
#define match_assert(file,line,condition) CHECK(condition,"%s:%d",file,line)

#define MAX(a,b) ((a)>(b)?(a):(b))
#define csmemset memset
#define MAXIMUM_CLIENT_UPDATES 8
struct player_action {
 unsigned long control_flags;
 struct {real yaw,pitch;} desired_facing;
 struct {real i,j;} throttle;
 real primary_trigger;
 short desired_weapon_index,desired_grenade_index,desired_zoom_level;
};
struct update { long update_number; struct { short action_count; struct player_action actions[4]; } update; };
struct update_server_queue_datum { short identifier, pad; long next_update_number; struct player_action current_action; };
struct data_iterator { struct data_array *data; long slot, datum_index; };
static struct data_array *player_data;
static struct { boolean initialized; long next_update_number_to_build; struct data_array *queues; struct update updates[16]; } update_server_globals;
static struct { boolean initialized; long next_update_number_to_dequeue, latest_update_number_received;
 struct player_action saved_action_collection[4]; struct data_array *queues; struct update updates[8]; } update_client_globals;
static long now;
static int distributed_resets;

static void data_make_valid(struct data_array *array) { (void)array; }
static void data_delete_all(struct data_array *array) { memset(array->data,0,array->size*array->maximum_count); array->count=0; }
static long datum_new_at_index(struct data_array *array,long index)
{
 struct datum_header *header=datum_get(array,index);
 CHECK(!header->identifier,"queue occupied"); memset(header,0,array->size); header->identifier=(short)(index>>16);
 array->count=MAX(array->count,(long)DATUM_INDEX_TO_ABSOLUTE_INDEX(index)+1); return index;
}
static void data_iterator_new(struct data_iterator *iterator,struct data_array *array)
{ iterator->data=array; iterator->slot=0; iterator->datum_index=NONE; }
static void *data_iterator_next(struct data_iterator *iterator)
{
 while(iterator->slot<iterator->data->maximum_count)
 {
  long slot=iterator->slot++;
  struct datum_header *header=(struct datum_header *)(iterator->data->data+slot*iterator->data->size);
  if(header->identifier) { iterator->datum_index=((long)(unsigned short)header->identifier<<16)|slot; return header; }
 }
 return NULL;
}
static void *checked_datum_get(struct data_array *array,long index)
{
 struct datum_header *header=datum_get(array,index);
 CHECK(header->identifier,"lookup of empty queue slot %ld",index); return header;
}
#define datum_get checked_datum_get
static void update_queues_distributed_reset(void) { distributed_resets++; }
static long game_time_get(void) { return now; }
#include "under_test.inc"

#define csmemcpy memcpy
#define NUMBEROF(a) (sizeof(a)/sizeof((a)[0]))
#define VALID_INDEX(i,n) ((i)>=0&&(unsigned long)(i)<(n))
#define DATUM_INDEX_NEW(i,salt) ((long)(((unsigned long)(unsigned short)(salt)<<16)|(i)))
enum {_game_connection_network_server=2,MAXIMUM_TRACKED_PLAYERS=128,MAXIMUM_NUMBER_OF_PLAYERS=128,
      DISTRIBUTED_INPUT_SILENCE_TICKS=10,MAXIMUM_LOCAL_PLAYERS=4};
static short host_player_structure_bsps[MAXIMUM_TRACKED_PLAYERS];
static struct {boolean valid;long tick,received_time,structure_bsp_tick;} update_server_distributed_inputs[MAXIMUM_NUMBER_OF_PLAYERS];
static unsigned long update_server_pending_control_flags[MAXIMUM_NUMBER_OF_PLAYERS];
static struct {boolean valid; long update_number; struct player_action action;
 unsigned long pending_control_flags;} update_client_relayed_actions[MAXIMUM_NUMBER_OF_PLAYERS];
static struct {boolean valid;long tick;short structure_bsp_index;struct player_action action;
 unsigned short control_flags[4];} update_client_local_inputs[MAXIMUM_LOCAL_PLAYERS];
static short bsp;
static struct player_datum {short local_player_index;} remote={NONE};
static struct player_datum *player_try_and_get(long player_index) {(void)player_index;return &remote;}
static boolean network_synchronizes_structure_bsp(void) {return TRUE;}
static struct {boolean scene_refresh;} host_resend;
static short global_structure_bsp_index_get(void) {return bsp;}
static short game_connection(void) {return _game_connection_network_server;}
static boolean game_time_initialized(void) {return TRUE;}
static void update_server_take_local_actions(void) {}
static struct update *update_server_get_update(long number) {return &update_server_globals.updates[number];}
static void update_client_handle_server_update(void *update,long number) {(void)update;(void)number;}
static boolean distributed_action_valid(struct player_action const *action) {(void)action;return TRUE;}
static void *datum_try_and_get(struct data_array *data,long index) {return datum_get(data,index);}
#include "inputs.inc"

static void check_bsp_inputs(void)
{
 const long player=DATUM_INDEX_NEW(0,1);
 const struct player_action held={.control_flags=4,.throttle={1,0},.primary_trigger=1,.desired_weapon_index=1};
 const struct player_action released={.desired_weapon_index=NONE,.desired_grenade_index=NONE,.desired_zoom_level=NONE};
 const unsigned short held_buttons[]={4,4,4,4}, latest_buttons[]={0,4,4,4};
 struct data_array *queues=game_state_data_new("server",1,sizeof(struct update_server_queue_datum));
 struct player_action *action, sent;
 unsigned short sent_buttons[4];
 long sent_tick;

 update_client_local_inputs[0].valid=TRUE;
 update_client_local_inputs[0].structure_bsp_index=0;
 bsp=0;
 CHECK(update_client_distributed_input(0,&sent_tick,&sent,sent_buttons),"captured input was never sent");
 bsp=1;
 CHECK(!update_client_distributed_input(0,&sent_tick,&sent,sent_buttons),"client relabeled the old input with a new region");
 update_server_globals.queues=queues;
 update_server_globals.initialized=TRUE;
 datum_new_at_index(queues,player);

 bsp=0;
 update_server_handle_distributed_input(player,10,0,&held,held_buttons,4);
 update_server_next_update();
 CHECK(update_server_globals.updates[0].update.actions[0].primary_trigger==1,"same-region input lost");

 bsp=1;
 update_server_next_update();
 action=&update_server_globals.updates[1].update.actions[0];
 CHECK(!action->control_flags&&!action->throttle.i&&!action->primary_trigger&&action->desired_weapon_index==NONE,
       "previous region's held input reached destination");

 update_server_handle_distributed_input(player,11,0,&held,held_buttons,4);
 CHECK(!network_coop_player_has_structure_bsp(player)&&!update_server_pending_control_flags[0],
       "still-loading player's input affected destination");
 update_server_handle_distributed_input(player,13,1,&released,latest_buttons,4);
 CHECK(!update_server_prediction_matches_structure_bsp(player,12),
       "delayed movement from the source BSP passed after the destination acknowledgment");
 CHECK(update_server_prediction_matches_structure_bsp(player,13),
       "current destination movement was refused");
 update_server_handle_distributed_input(player,12,0,&held,held_buttons,4);
 update_server_next_update();
 action=&update_server_globals.updates[2].update.actions[0];
 CHECK(network_coop_player_has_structure_bsp(player)&&!action->control_flags&&!action->primary_trigger,
       "old region input history or out-of-order report overrode new region");
 update_server_handle_distributed_input(player,14,1,&held,held_buttons,4);
 update_server_next_update();
 action=&update_server_globals.updates[3].update.actions[0];
 CHECK(action->control_flags==4&&action->throttle.i==1&&action->primary_trigger==1,
       "new input failed to resume in destination");
 CHECK(update_server_prediction_matches_structure_bsp(player,13),
       "ordinary destination input advanced the transition tick");
 bsp=0;
 network_coop_clear_structure_bsp_acknowledgments();
 bsp=1;
 network_coop_clear_structure_bsp_acknowledgments();
 CHECK(!network_coop_player_has_structure_bsp(player) &&
       !update_server_prediction_matches_structure_bsp(player,14),
       "return to a previously acknowledged BSP restored held input without a new acknowledgment");
 bsp=0;
 network_coop_clear_structure_bsp_acknowledgments();
 update_server_handle_distributed_input(player,17,0,&released,latest_buttons,4);
 CHECK(!update_server_prediction_matches_structure_bsp(player,16)&&
       update_server_prediction_matches_structure_bsp(player,17),
       "return crossing did not replace the transition tick");
 free(queues->data);
 free(queues);
}

static void check_reused_queue(boolean server)
{
 const long previous=DATUM_INDEX_NEW(0,1), fresh=DATUM_INDEX_NEW(0,2);
 const struct player_action action={.primary_trigger=1};
 const unsigned short buttons[]={0,4};
 struct data_array *queues=game_state_data_new("old queue",1,sizeof(struct update_server_queue_datum));

 bsp=1;
 if(server)
 {
  update_server_globals.queues=queues;
  update_server_globals.initialized=TRUE;
  update_server_add_player(previous);
  update_server_handle_distributed_input(previous,100,bsp,&action,buttons,1);
  update_server_pending_control_flags[0]=4;
  update_server_add_player(previous);
  CHECK(network_coop_player_has_structure_bsp(previous)&&update_server_distributed_inputs[0].valid&&
   update_server_pending_control_flags[0]==4,"duplicate add cleared the active player's input");
  update_server_add_player(fresh);
  CHECK(!network_coop_player_has_structure_bsp(fresh),"new player inherited a departed player's BSP readiness");
  CHECK(!update_server_distributed_inputs[0].valid&&!update_server_pending_control_flags[0],
   "new player inherited old timestamps or pending buttons");
  update_server_handle_distributed_input(fresh,2,bsp,&action,buttons,2);
  CHECK(network_coop_player_has_structure_bsp(fresh)&&update_server_distributed_inputs[0].tick==2&&
   !update_server_pending_control_flags[0],"first region acknowledgement was rejected or consumed stale button history");
 }
 else
 {
  update_client_globals.queues=queues;
  update_client_add_player(previous);
  update_client_relayed_actions[0].valid=TRUE;
  update_client_relayed_actions[0].pending_control_flags=4;
  update_client_add_player(previous);
  CHECK(update_client_relayed_actions[0].valid&&update_client_relayed_actions[0].pending_control_flags==4,
   "duplicate add cleared the active relay");
  update_client_add_player(fresh);
  CHECK(!update_client_relayed_actions[0].valid&&!update_client_relayed_actions[0].pending_control_flags,
   "client predicted a new player from the previous owner's action");
 }
 free(queues->data);
 free(queues);
}

static void check_reset(int first,int last,boolean server,boolean client,long time)
{
 int i; struct data_array *queues;
 memset(&update_server_globals,0,sizeof(update_server_globals)); memset(&update_client_globals,0,sizeof(update_client_globals));
 player_data=game_state_data_new("players",128,sizeof(struct datum_header));
 update_server_globals.queues=game_state_data_new("server",128,sizeof(struct update_server_queue_datum));
 update_client_globals.queues=game_state_data_new("client",128,sizeof(struct datum_header));
 for(i=first;i<=last;i++)datum_new_at_index(player_data,(long)((0x8001UL+i)<<16)|i);
 update_server_globals.initialized=server; update_client_globals.initialized=client; now=time; distributed_resets=0;
 update_server_globals.next_update_number_to_build=999; update_client_globals.next_update_number_to_dequeue=999;
 update_queues_reset_and_fill_with_lies();
 CHECK(distributed_resets==1,"distributed inputs were not reset");
 CHECK(update_server_globals.next_update_number_to_build==(client?time:(server?0:999)),"wrong server clock");
 if(client)CHECK(update_client_globals.next_update_number_to_dequeue==time && update_client_globals.latest_update_number_received==time-1,"wrong client clock");
 queues=update_server_globals.queues;
 for(i=0;i<128;i++)
 {
  struct update_server_queue_datum *queue=(void *)(queues->data+i*queues->size);
  CHECK(!!queue->identifier==(server&&i>=first&&i<=last),"wrong queue membership at %d",i);
  if(queue->identifier)CHECK(queue->next_update_number==(client?time:0),"queue %d clock %ld",i,queue->next_update_number);
 }
 if(client)
 {
  long start=MAX(0,time-MAXIMUM_CLIENT_UPDATES);
  for(i=0;start<time;i++,start++)CHECK(update_client_globals.updates[i].update_number==start && update_client_globals.updates[i].update.action_count==1,"bad synthetic client update");
 }
 free(player_data->data);free(player_data);free(queues->data);free(queues);
 free(update_client_globals.queues->data);free(update_client_globals.queues);
}
int main(int argc,char **argv)
{
 int i; char const *case_name=argc==2?argv[1]:"";
 CASE("empty")check_reset(1,0,TRUE,TRUE,1200);
 else CASE("sparse") { for(i=1;i<128;i++)check_reset(i,i,TRUE,TRUE,1200); }
 else CASE("slot-zero")check_reset(0,0,TRUE,TRUE,1200);
 else CASE("eight")check_reset(0,7,TRUE,TRUE,1200);
 else CASE("full")check_reset(0,127,TRUE,TRUE,1200);
 else CASE("dedicated")check_reset(1,7,TRUE,TRUE,1200);
 else CASE("client-only")check_reset(1,7,FALSE,TRUE,1200);
 else CASE("uninitialized")check_reset(1,7,FALSE,FALSE,1200);
 else CASE("early") {check_reset(1,7,TRUE,TRUE,0);check_reset(1,7,TRUE,TRUE,3);}
 else CASE("bsp-input")check_bsp_inputs();
 else CASE("bsp-slot-reuse")check_reused_queue(TRUE);
 else CASE("relay-slot-reuse")check_reused_queue(FALSE);
 else return 2;
 return 0;
}
