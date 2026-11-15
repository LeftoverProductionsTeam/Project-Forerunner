#include "harness.h"
#include <float.h>
#define REAL_MAX FLT_MAX
#define MIN(a,b) ((a)<(b)?(a):(b))
#define MAX(a,b) ((a)>(b)?(a):(b))
#define PIN(v,a,b) MIN(MAX(v,a),b)
#define csmemset memset
struct { short window_index; long frame_index; } render;
static long limit;
static long validated;
static int states;
static int *cached_object_render_states=&states;
static void data_make_valid(void *data) { (void)data;validated++; }
long config_integer(char const *name) { (void)name;return limit; }
#include "under_test.inc"
static void candidates(void) {
 port_shadow_minimum_pixels(100.f);port_shadow_minimum_pixels(80.f);
 port_shadow_minimum_pixels(60.f);port_shadow_minimum_pixels(40.f);
}
int main(int argc,char **argv) {
 char const *name=argv[1];short i;real threshold;
 (void)argc;
 render_objects_initialize_for_new_map();render.frame_index=1;
 if(!strcmp(name,"stock")) {
  limit=0;port_shadow_limit_begin();
  for(i=0;i<1000;i++)CHECK(port_shadow_minimum_pixels(200.f)==30.f,"stock shadow fade changed");
  CHECK(!port_shadow_windows[0].count,"unlimited setting collected unnecessary history");
  limit=2;render.frame_index++;port_shadow_limit_begin();candidates();
  render.frame_index++;port_shadow_limit_begin();CHECK(port_shadow_windows[0].threshold>30.f,"budget did not raise threshold");
  limit=0;render.frame_index++;port_shadow_limit_begin();CHECK(port_shadow_minimum_pixels(200.f)==30.f,"turning budget off retained old limit");
 } else if(!strcmp(name,"rank")) {
  limit=2;port_shadow_limit_begin();candidates();render.frame_index++;port_shadow_limit_begin();
  CHECK(port_shadow_windows[0].threshold>37.499f && port_shadow_windows[0].threshold<37.501f,"largest shadows were not favored with eased threshold");
 } else if(!strcmp(name,"stereo")) {
  limit=2;port_shadow_limit_begin();candidates();render.frame_index++;port_shadow_limit_begin();
  threshold=port_shadow_windows[0].threshold;port_shadow_minimum_pixels(90.f);port_shadow_minimum_pixels(70.f);
  port_shadow_limit_begin();
  CHECK(port_shadow_windows[0].threshold==threshold,"same-frame eye pass changed shadow threshold");
  for(i=0;i<100;i++)port_shadow_minimum_pixels(100.f);
  CHECK(port_shadow_windows[0].count==2,"other eye counted the same crowd twice");
 } else if(!strcmp(name,"windows")) {
  limit=2;port_shadow_limit_begin();candidates();render.window_index=1;port_shadow_limit_begin();
  for(i=0;i<4;i++)port_shadow_minimum_pixels(300.f);
  render.frame_index++;render.window_index=0;port_shadow_limit_begin();threshold=port_shadow_windows[0].threshold;
  render.window_index=1;port_shadow_limit_begin();
  CHECK(port_shadow_windows[1].threshold>threshold && port_shadow_windows[0].threshold==threshold,"split-screen shadow decisions shared state");
 } else if(!strcmp(name,"bounds")) {
  limit=1;port_shadow_limit_begin();for(i=0;i<1000;i++)port_shadow_minimum_pixels(100.f);
  CHECK(port_shadow_windows[0].count==PORT_SHADOW_CANDIDATE_LIMIT,"crowd overflowed bounded render history");
  CHECK(port_shadow_minimum_pixels(REAL_MAX)==30.f && port_shadow_windows[0].count==PORT_SHADOW_CANDIDATE_LIMIT,"cinematic sentinel entered ranking");
  limit=2147483647L;render.frame_index++;port_shadow_limit_begin();
  CHECK(port_shadow_windows[0].threshold==30.f,"oversized budget indexed outside candidates");
  limit=-1;render.frame_index++;port_shadow_limit_begin();
  CHECK(!port_shadow_windows[0].collect,"negative setting enabled budget");
 } else if(!strcmp(name,"reset")) {
  limit=2;port_shadow_limit_begin();candidates();render.frame_index++;port_shadow_limit_begin();
  render_objects_initialize_for_new_map();
  for(i=0;i<PORT_SHADOW_WINDOWS;i++)CHECK(port_shadow_windows[i].frame==NONE && !port_shadow_windows[i].count && !port_shadow_windows[i].threshold,"map retained shadow crowd history");
  CHECK(validated==2,"native render-state lifecycle was bypassed");
 } else CHECK(0,"unknown case");
 return 0;
}
