/* A co-op host's script looping sounds (network_coop.c's
host_send_looping_sounds and network_coop_note_sound, with game_sound.c's
scripted looping sounds). A host that draws nothing (a dedicated server)
never renders its sounds, so one that plays once (fake impulse: a
cutscene's) never ends there by itself: it is ended once it has had the time
to play, and is not resent near that end, so that no client starts it again. */
#include "harness.h"

typedef unsigned short word;
typedef word message_header;
#define NUMBEROF(a) (sizeof(a)/sizeof((a)[0]))
#define MAX(a,b) ((a)>(b)?(a):(b))
#define PIN(x,lo,hi) ((x)<(lo)?(lo):(x)>(hi)?(hi):(x))
#define SET_FLAG(f,b,v) ((f)=(v)?(f)|(1U<<(b)):(f)&~(1U<<(b)))
#define TICKS_PER_SECOND 30
#define SOUND_DEFINITION_TAG 0x736e6421UL
#define TAG_BLOCK_GET_ELEMENT(block, index, type) (&((type *)(block)->address)[(index)])

struct tag_block_definition;
#define HALO_PORT_MAXIMUM_NETWORK_MACHINES 2
#define _distributed_message_coop_events 7
#include "types.inc"
void network_coop_note_sound(short kind,long definition_index,long object_index,real scale);
void scripted_looping_sound_stop(long sound_index);

/* the map's sounds: how long each plays in milliseconds, its linked permutations together
(coop_sound_sync.c's coop_sound_sync_sound_milliseconds, test_coop_sound_sync.py) */
enum {SHORT_SOUND=0,LONG_SOUND=1,NO_LENGTH=2,HUGE_SOUND=3,SOUNDS=4};
struct sound_definition {long milliseconds,scripting_time,scripting_sound_index,longest_permutation_length;};
static struct sound_definition sounds[SOUNDS];
static long coop_sound_sync_sound_milliseconds(long index)
{
 CHECK(index>=0&&index<SOUNDS,"no such sound %ld",index);
 return sounds[index].milliseconds;
}

/* the map's looping sounds: 0 plays once (a cutscene's: a 10 s start and a
60 s loop), 1 is music that loops, 2 plays once with sounds of no length,
3 plays once with a sound longer than any could be */
enum {ONCE=0,MUSIC=1,ONCE_NO_LENGTH=2,ONCE_HUGE=3,DEFINITIONS=4};
static struct looping_sound_definition definitions[DEFINITIONS];
static struct looping_sound_track tracks[DEFINITIONS];
static struct looping_sound_definition *looping_sound_definition_get(long index)
{
 CHECK(index>=0&&index<DEFINITIONS,"no such looping sound %ld",index);
 return &definitions[index];
}

/* the playing looping sounds (game_sound.c's data) */
struct game_looping_sound_datum {long definition_index;unsigned long flags;real scale;boolean used;};
enum {_game_looping_sound_unattached_bit,_game_looping_sound_scripted_bit,_game_looping_sound_alternate_bit,
 _game_looping_sound_unattached_stop_bit,_game_looping_sound_unattached_stop_fixed_fadeout_bit};
static struct game_looping_sound_datum playing[8];
static long game_looping_sound_new(long object_index,long definition_index,char const *marker,long node)
{
 long index;
 (void)object_index;(void)marker;(void)node;
 for(index=0;index<8;index++)
 {
  if(!playing[index].used)
  {
   memset(&playing[index],0,sizeof(playing[index]));
   playing[index].used=TRUE;playing[index].definition_index=definition_index;
   return index;
  }
 }
 return NONE;
}
static struct game_looping_sound_datum *game_looping_sound_get(long index)
{
 CHECK(index>=0&&index<8&&playing[index].used,"no such playing sound %ld",index);
 return &playing[index];
}
static void scripted_music_stop_all(void){}

/* the host's world: its clock, whether it draws, its resends and the events it sends */
static long now;
static long game_time_get(void){return now;}
boolean debug_no_drawing;
static struct {boolean refresh;} host_resend;
static struct distributed_coop_event sent[1024];
static int sent_count,queue_full;
static struct distributed_coop_event *recorded_event_new(byte kind)
{
 struct distributed_coop_event *event;
 if(queue_full||sent_count==1024)return NULL;
 event=&sent[sent_count++];
 memset(event,0,sizeof(*event));
 event->kind=kind;event->object_index=NONE;event->tag_index=NONE;event->target=NONE;
 return event;
}

/* Existing looping-sound tests isolate their event history. The additional
 * queue checks below exercise the real sender and recipient metadata. */
#define event_new recorded_event_new
#include "under_test.inc"
#undef event_new


#define csmemset memset
#define csmemcpy memcpy
#define csmemmove memmove
#define _error_silent 0
static boolean host_enabled=TRUE;
static int event_errors,wire_count[2],wire_broadcasts;
static long test_machines[2]={0x10020,0x10021};
static struct distributed_coop_event wire_events[2][MAXIMUM_QUEUED_EVENTS];
static boolean coop_host(void){return host_enabled;}
static boolean distributed_tag_of_group(long tag,unsigned long group){return group==SOUND_DEFINITION_TAG&&tag>=0&&tag<SOUNDS;}
static void error(int level,char const *text,...){event_errors++;}
static short distributed_client_machines(long *out,short capacity){CHECK(capacity>=2,"machine list truncated");memcpy(out,test_machines,sizeof(test_machines));return 2;}
static void distributed_send_to_machine(long machine,void *packet,byte type,short count,word size){
 int i=machine==test_machines[0]?0:1;struct distributed_coop_events_message *m=packet;
 CHECK(type==_distributed_message_coop_events&&count<=MAXIMUM_QUEUED_EVENTS,"wrong existing event envelope");
 CHECK(size==sizeof(m->header)+count*sizeof(m->events[0]),"event wire size changed");
 memcpy(wire_events[i],m->events,count*sizeof(m->events[0]));wire_count[i]=count;
}
static void send_to_clients(void *m,byte type,short count,word entry_size){
 CHECK(entry_size==sizeof(struct distributed_coop_event),"classic event layout changed");wire_broadcasts++;
 for(int i=0;i<2;i++)distributed_send_to_machine(test_machines[i],m,type,count,(word)(sizeof(struct distributed_message_header)+count*entry_size));
}
#include "event_queue.inc"

struct object_marker{short node_index;struct{real_point3d position;real_vector3d forward;}node_matrix;};
static real_point3d origin;
static real_vector3d forward;
static real_point3d *global_origin3d=&origin;
static real_vector3d *global_forward3d=&forward;
static int impulse_plays,impulse_stops;
static struct sound_definition *sound_definition_get(long index){CHECK(index>=0&&index<SOUNDS,"invalid native impulse definition");return &sounds[index];}
static void sound_stop_impulse(long index){if(index!=NONE)impulse_stops++;}
static int object_get_marker_by_name(long object,char const *name,struct object_marker *out,int count){return 0;}
static long object_impulse_sound_new(long object,long tag,short node,real_point3d const *p,real_vector3d const *v,real scale){impulse_plays++;return 77;}
static void object_type_notify_impulse_sound(long object,long tag,long sound){}
static long unspatialized_impulse_sound_new(long tag,real scale){impulse_plays++;return 78;}
static char const *config_string(char const *key){return "";}
static char const *tag_get_name(long tag){return "owned/native/voice";}
static void platform_log(char const *format,...){}
#include "impulse.inc"

static void setup(void)
{
 int index;
 memset(sounds,0,sizeof(sounds));memset(definitions,0,sizeof(definitions));memset(tracks,0,sizeof(tracks));
 memset(playing,0,sizeof(playing));
 sounds[SHORT_SOUND].milliseconds=10000;
 sounds[LONG_SOUND].milliseconds=60000;
 sounds[HUGE_SOUND].milliseconds=0x7fffffffL;
 for(index=0;index<DEFINITIONS;index++)
 {
  definitions[index].runtime_scripting_sound_index=NONE;
  definitions[index].tracks.count=1;definitions[index].tracks.address=&tracks[index];
  tracks[index].start_sound.index=NONE;tracks[index].loop_sound.index=NONE;tracks[index].stop_sound.index=NONE;
  tracks[index].alternate_loop_sound.index=NONE;tracks[index].alternate_stop_sound.index=NONE;
 }
 tracks[ONCE].start_sound.index=SHORT_SOUND;tracks[ONCE].loop_sound.index=LONG_SOUND;
 tracks[MUSIC].loop_sound.index=LONG_SOUND;
 tracks[ONCE_NO_LENGTH].loop_sound.index=NO_LENGTH;
 tracks[ONCE_HUGE].loop_sound.index=HUGE_SOUND;
 SET_FLAG(definitions[ONCE].flags,_looping_sound_fake_impulse_sound_bit,TRUE);
 SET_FLAG(definitions[ONCE_NO_LENGTH].flags,_looping_sound_fake_impulse_sound_bit,TRUE);
 SET_FLAG(definitions[ONCE_HUGE].flags,_looping_sound_fake_impulse_sound_bit,TRUE);
 now=1000;debug_no_drawing=TRUE;host_resend.refresh=FALSE;sent_count=0;queue_full=FALSE;
 host_looping_sound_count=0;
}

static boolean is_playing(long definition_index){return definitions[definition_index].runtime_scripting_sound_index!=NONE;}

/* the events of a type for a sound sent since index from */
static int sent_of(int from,int type,long definition_index)
{
 int index,count=0;
 for(index=from;index<sent_count;index++)
  if(sent[index].type==type&&sent[index].tag_index==definition_index)count++;
 return count;
}

/* the last event of a type for a sound, or NULL */
static struct distributed_coop_event const *last_sent(int type,long definition_index)
{
 int index;
 for(index=sent_count-1;index>=0;index--)
  if(sent[index].type==type&&sent[index].tag_index==definition_index)return &sent[index];
 return NULL;
}

/* the host's ticks from now to the tick at (now is then the tick after),
a resend every two seconds as host_resend_update makes them */
static void run_to(long at)
{
 for(;now<=at;now++)
 {
  host_resend.refresh=now%(2*TICKS_PER_SECOND)==0;
  host_send_looping_sounds();
 }
 host_resend.refresh=FALSE;
}

int main(int argc,char **argv)
{
 char const *name=argc>1?argv[1]:"";
 setup();
 if(!strcmp(name,"private-events")||!strcmp(name,"private-slot")) {
  struct distributed_coop_event *e;
  memset(&coop_events,0,sizeof(coop_events));coop_events.next_number=65534;
  e=event_new(_coop_event_sound);e->type=_coop_sound_impulse;e->tag_index=SHORT_SOUND;
  CHECK(network_coop_private_sound(test_machines[1],LONG_SOUND),"private event not queued");
  e=event_new(_coop_event_sound);e->type=_coop_sound_impulse;e->tag_index=NO_LENGTH;
  host_send_events();
  CHECK(wire_count[0]==3&&wire_count[1]==3&&!wire_broadcasts,"private queue lost sequence entries or broadcast sound");
  CHECK(wire_events[0][0].tag_index==SHORT_SOUND&&wire_events[0][2].tag_index==NO_LENGTH,"ordinary sound lost");
  CHECK(wire_events[0][1].number==0&&wire_events[1][1].number==0&&wire_events[1][1].tag_index==LONG_SOUND,"private sequence wrap or audience failed");
  CHECK(wire_events[0][1].kind==_coop_event_sound&&wire_events[0][1].tag_index==NONE&&wire_events[0][1].object_index==NONE&&wire_events[0][1].reals[0]==0,"private payload leaked to another machine");
  if(!strcmp(name,"private-slot")){
   test_machines[1]+=0x10000;host_send_events();
   CHECK(wire_events[1][1].tag_index==NONE,"reconnected machine inherited private sound");host_send_events();
   CHECK(!coop_events.count,"private queue not retired");
   e=event_new(_coop_event_sound);e->tag_index=SHORT_SOUND;host_send_events();
   CHECK(wire_broadcasts==1&&wire_events[0][0].tag_index==SHORT_SOUND,"retired audience polluted reused event slot");
  } else {
   coop_events.sends[0]=EVENT_SENDS-1;host_send_events();
   CHECK(coop_events.count==2&&coop_events.audience[0]==test_machines[1]&&coop_events.audience[1]==NONE,"event compaction lost private audience");
   host_send_events();CHECK(!coop_events.count,"private event did not age out");
   coop_events.count=MAXIMUM_QUEUED_EVENTS;CHECK(!network_coop_private_sound(test_machines[1],SHORT_SOUND)&&event_errors==1,"queue overflow accepted");
   coop_events.count=0;host_enabled=FALSE;CHECK(!network_coop_private_sound(test_machines[1],SHORT_SOUND),"client authored private event");
   CHECK(!network_coop_private_sound(NONE,SHORT_SOUND)&&!network_coop_private_sound(test_machines[1],NONE),"invalid private destination/tag accepted");
  }
 }
 else if(!strcmp(name,"private-audio")) {
  sounds[LONG_SOUND].longest_permutation_length=4000;sounds[LONG_SOUND].scripting_sound_index=5;
  scripted_sound_new_private(LONG_SOUND,FALSE);
  CHECK(!sent_count&&!impulse_plays&&!impulse_stops&&scripted_sound_time(LONG_SOUND)==120,"remote-only radio broadcast, played/stopped locally or lost native timing");
  scripted_sound_new_private(LONG_SOUND,TRUE);
  CHECK(!sent_count&&impulse_plays==1&&impulse_stops==1&&scripted_sound_time(LONG_SOUND)==120,"local private radio duplicated/broadcast or lost native playback");
 }
 else if(!strcmp(name,"ordinary-impulse")) {
  sounds[SHORT_SOUND].longest_permutation_length=2000;
  scripted_sound_new(SHORT_SOUND,44,.5f);
  CHECK(sent_count==1&&sent[0].object_index==44&&sent[0].reals[0]==.5f&&impulse_plays==1&&scripted_sound_time(SHORT_SOUND)==60,"ordinary native sound path changed");
 }
 else if(!strcmp(name,"length"))
 {
  /* the longest a sound that plays once can take: start and loop, 70 s */
  CHECK(looping_sound_play_once_ticks(ONCE)==70*TICKS_PER_SECOND,"%ld ticks",looping_sound_play_once_ticks(ONCE));
  CHECK(looping_sound_play_once_ticks(MUSIC)==0,"music that loops was given a length");
  CHECK(looping_sound_play_once_ticks(ONCE_NO_LENGTH)==0,"sounds of no length were given one");
  tracks[ONCE].alternate_loop_sound.index=LONG_SOUND;tracks[ONCE].loop_sound.index=SHORT_SOUND;
  CHECK(looping_sound_play_once_ticks(ONCE)==70*TICKS_PER_SECOND,"the longer alternate loop was not taken");
  CHECK(looping_sound_play_once_ticks(ONCE_HUGE)==60*60*TICKS_PER_SECOND,"a map's huge length was %ld ticks",
   looping_sound_play_once_ticks(ONCE_HUGE));
 }
 else if(!strcmp(name,"ended"))
 {
  /* a dedicated server: the cutscene's sound ends once it has had the time to play, its stop sent to the clients */
  long start=now;
  scripted_looping_sound_start(ONCE,NONE,1.0f);
  CHECK(is_playing(ONCE)&&host_looping_sound_count==1,"fixture: the scripts' start did not play");
  run_to(start+70*TICKS_PER_SECOND);
  CHECK(is_playing(ONCE),"ended before it could have played out");
  run_to(start+72*TICKS_PER_SECOND+1);
  CHECK(!is_playing(ONCE)&&host_looping_sound_count==0,"never ended: the clients would hear it again and again");
  CHECK(sent_of(0,_coop_sound_looping_stop,ONCE)>=1,"its end was not sent to the clients");
 }
 else if(!strcmp(name,"not-resent-near-end"))
 {
  /* resent while it plays (for a joiner), but not in the margin before its end, when a client's copy may have ended */
  long start=now;
  int from;
  scripted_looping_sound_start(ONCE,NONE,1.0f);
  run_to(start+68*TICKS_PER_SECOND-1);
  CHECK(sent_of(0,_coop_sound_looping_start,ONCE)>=30,"resent %d times while it played",
   sent_of(0,_coop_sound_looping_start,ONCE));
  /* (none in the 2 s before its 70 s, nor after) */
  from=sent_count;
  run_to(start+72*TICKS_PER_SECOND+1);
  CHECK(sent_of(from,_coop_sound_looping_start,ONCE)==0,"resent %d times in the 2 s before its end",
   sent_of(from,_coop_sound_looping_start,ONCE));
 }
 else if(!strcmp(name,"drawing-host"))
 {
  /* a host that draws renders its sounds, which end there as they always have: never ended early here */
  long start=now;
  debug_no_drawing=FALSE;
  scripted_looping_sound_start(ONCE,NONE,1.0f);
  run_to(start+600*TICKS_PER_SECOND);
  CHECK(is_playing(ONCE),"a drawing host ended a sound its sound system plays");
 }
 else if(!strcmp(name,"music"))
 {
  /* music that loops plays until the scripts stop it, resent all along */
  long start=now;
  int from;
  scripted_looping_sound_start(MUSIC,NONE,1.0f);
  run_to(start+600*TICKS_PER_SECOND-1);
  CHECK(is_playing(MUSIC),"looping music was ended");
  from=sent_count;
  run_to(start+602*TICKS_PER_SECOND);
  CHECK(sent_of(from,_coop_sound_looping_start,MUSIC)>=1,"looping music stopped being resent");
 }
 else if(!strcmp(name,"no-length"))
 {
  /* a sound that plays once whose sounds give no length is left as before */
  long start=now;
  scripted_looping_sound_start(ONCE_NO_LENGTH,NONE,1.0f);
  run_to(start+600*TICKS_PER_SECOND);
  CHECK(is_playing(ONCE_NO_LENGTH),"a sound of no length was ended at once");
 }
 else if(!strcmp(name,"started-again"))
 {
  /* the scripts start it again while it plays: its time counts from the new start */
  long start=now;
  scripted_looping_sound_start(ONCE,NONE,1.0f);
  run_to(start+50*TICKS_PER_SECOND);
  scripted_looping_sound_start(ONCE,NONE,1.0f);
  start=now;
  run_to(start+60*TICKS_PER_SECOND);
  CHECK(is_playing(ONCE),"the new start was ended on the old one's time");
  run_to(start+73*TICKS_PER_SECOND);
  CHECK(!is_playing(ONCE),"the new start never ended");
 }
 else if(!strcmp(name,"queue-full"))
 {
  /* its end comes when no event can be sent: ended and taken out all the same, with no endless loop */
  long start=now;
  scripted_looping_sound_start(ONCE,NONE,1.0f);
  scripted_looping_sound_start(MUSIC,NONE,1.0f);
  run_to(start+72*TICKS_PER_SECOND-1);
  CHECK(is_playing(ONCE),"fixture: ended before the queue filled");
  queue_full=TRUE;
  run_to(start+73*TICKS_PER_SECOND);
  CHECK(!is_playing(ONCE)&&host_looping_sound_count==1&&is_playing(MUSIC),
   "with the queue full: %d listed, once playing %d",host_looping_sound_count,is_playing(ONCE));
 }
 else if(!strcmp(name,"resent-time"))
 {
  /* the resends of one that plays once say how long ago it started, for a machine that joins during it */
  long start=now;
  struct distributed_coop_event const *event;
  scripted_looping_sound_start(ONCE,NONE,1.0f);
  event=last_sent(_coop_sound_looping_start,ONCE);
  CHECK(event&&event->target==NONE,"the scripts' start said it began %ld ms ago",event?event->target:0);
  run_to(start+30*TICKS_PER_SECOND);
  event=last_sent(_coop_sound_looping_start,ONCE);
  CHECK(event&&event->target>=28000&&event->target<=30000,"a resend 30 s in said %ld ms",event?event->target:0);
  /* (resends fall on even seconds of the game's time: the last before now) */
  CHECK(event->target==((now-1)/(2*TICKS_PER_SECOND)*(2*TICKS_PER_SECOND)-start)*1000/TICKS_PER_SECOND,
   "a resend said %ld ms, not its tick's",event->target);
  /* music that loops starts from its beginning everywhere, as it always has */
  scripted_looping_sound_start(MUSIC,NONE,1.0f);
  start=now;
  run_to(start+10*TICKS_PER_SECOND);
  event=last_sent(_coop_sound_looping_start,MUSIC);
  CHECK(event&&event->target==NONE,"music that loops was resent with %ld ms",event?event->target:0);
 }
 else
 {
  fprintf(stderr,"unknown case %s\n",name);
  return 2;
 }
 return 0;
}
