/* Native recorded-flight handoff, using production playback lifecycle code. */
#include "harness.h"
#include <string.h>

typedef unsigned short word;
#define TAG_STRING_LENGTH 31
#define VALID_INDEX(index,count) ((index)>=0&&(index)<(count))
#define NUMBEROF(array) (sizeof(array)/sizeof((array)[0]))
#define SET_FLAG(flags,bit,value) ((flags)=(value)?((flags)|FLAG(bit)):((flags)&~FLAG(bit)))
#define datum_new flight_datum_new
#define datum_get flight_datum_get
#define datum_delete flight_datum_delete

struct unit_control_data { long unused; };
struct tag_data { long size; byte *address; };
struct tag_block { long count; void *address; };
#include "recording_types.inc"
static struct recorded_animation_definition recording;
static struct { struct tag_block recorded_animations; } scenario;
static struct animation_thread slot;
static struct animation_thread_debug debug_slot;
static boolean allocated, controllable, possessed, active, deactivation;
static short destroyed, hovered;
static byte stream[8];
struct data_iterator { long datum_index; short next; };
static void *animation_threads;
#define TAG_BLOCK_GET_ELEMENT(block,index,type) (((type *)(block)->address)+(index))
#define animation_threads_debug_get(index) (&debug_slot)
#define global_scenario_get() (&scenario)
#undef xbox_pointer
#define xbox_pointer(address) (address)
#define _object_mask_unit 1
#define _error_silent 0
#undef match_assert
#define match_assert(file,line,test) CHECK(test,"native assertion")
#define match_vassert(file,line,test,message) CHECK(test,"native stream assertion")
#define error(...) ((void)0)
static void data_iterator_new(struct data_iterator *i,void *data) {(void)data;i->next=0;}
static void *data_iterator_next(struct data_iterator *i) {i->datum_index=0;return !i->next++&&allocated?&slot:NULL;}
static long datum_new(void *data) {(void)data;memset(&slot,0,sizeof(slot));allocated=TRUE;return 0;}
static void *datum_get(void *data,long index) {(void)data;(void)index;return &slot;}
static void datum_delete(void *data,long index) {(void)data;(void)index;allocated=FALSE;}
static void *object_try_and_get_and_verify_type(long index,long type) {(void)type;return index==1?&slot:NULL;}
static void object_get_and_verify_type(long index,long type) {CHECK(object_try_and_get_and_verify_type(index,type),"invalid fixture unit");}
static void player_index_from_unit_index(long index) {(void)index;}
static boolean unit_controllable(long index) {(void)index;return controllable;}
static void unit_set_controllable(long index,boolean value) {(void)index;controllable=value;}
static void unit_set_possessed(long index,boolean value) {(void)index;possessed=value;}
static void unit_set_actively_controlled(long index,boolean value) {(void)index;active=value;}
static void object_set_automatic_deactivation(long index,boolean value) {(void)index;deactivation=value;}
static void unit_control(long index,struct unit_control_data *control) {(void)index;(void)control;}
static void hs_object_destroy(long index) {(void)index;destroyed++;}
static void vehicle_hover(long index,boolean value) {(void)index;if(value)hovered++;}
static byte *tag_data_get_pointer(struct tag_data const *data,long offset,long size) {(void)size;return data->address+offset;}
static boolean recorded_animation_playable(struct recorded_animation_definition const *r) {(void)r;return TRUE;}
static struct recorded_animation_definition const *animation_thread_get_animation(struct animation_thread *t) {(void)t;return &recording;}
static void mcc_recording_control_begin(long index,struct recorded_animation_definition const *r,struct unit_control_data *c) {(void)index;(void)r;(void)c;}
static boolean initialize(void *state,void *controller,byte **event,byte version,byte const *end)
{(void)state;(void)controller;(void)event;(void)version;(void)end;return TRUE;}
static boolean apply(void *state,struct unit_control_data *controller,long *tick,byte **event,byte const *end)
{(void)state;(void)controller;(void)tick;*event=(byte *)end;return FALSE;}
static struct animation_playback codec={initialize,apply};
static struct animation_playback *playback_codec[4]={&codec,&codec,&codec,&codec};
#include "recording_lifecycle.inc"

int main(int argc,char **argv)
{
 word old_flags;
 CHECK(argc==2,"missing case");
 controllable=strcmp(argv[1],"uncontrolled")!=0;
 scenario.recorded_animations.count=1;scenario.recorded_animations.address=&recording;
 recording.version=1;recording.length_in_ticks=1;recording.event_stream.size=sizeof(stream);recording.event_stream.address=stream;
 old_flags=!strcmp(argv[1],"hover")?FLAG(_recording_thread_hover_vehicle_on_complete_bit):FLAG(_recording_thread_delete_unit_on_complete_bit);
 CHECK(recorded_animation_play_internal(1,0,old_flags),"first flight refused");
 CHECK(!controllable&&possessed&&active&&!deactivation,"native recording did not take control");
 CHECK(!recorded_animation_play_internal(1,0,0),"active flight replaced without kill/finish");
 if(!strcmp(argv[1],"finished")) recorded_animations_update();
 else recorded_animation_kill(1);
 CHECK(recorded_animation_play_internal(1,0,0),"next flight refused during cleanup window");
 CHECK(!(slot.flags&(FLAG(_recording_thread_killed_bit)|FLAG(_recording_thread_delete_unit_on_complete_bit)|FLAG(_recording_thread_hover_vehicle_on_complete_bit))),"new flight inherited old kill/delete/hover flags");
 recorded_animations_update();recorded_animations_update();
 CHECK(!allocated&&!possessed&&!active&&deactivation,"completed flight retained native control");
 CHECK(controllable==(strcmp(argv[1],"uncontrolled")!=0),"chained flight lost original controllability");
 CHECK(!destroyed&&!hovered,"old flight completion action affected new departure");
 return 0;
}
