#include "harness.h"
#include <stdint.h>
#include <stdbool.h>
#define HALO_BGFX

typedef int BOOL;
typedef int GLint;
typedef unsigned long DWORD;
typedef struct { unsigned short idx; } bgfx_texture_handle_t;
typedef bgfx_texture_handle_t bgfx_frame_buffer_handle_t;
typedef bgfx_texture_handle_t bgfx_program_handle_t;
#define BGFX_INVALID_HANDLE {UINT16_MAX}
#define BGFX_HANDLE_IS_VALID(h) ((h).idx!=UINT16_MAX)
enum {BGFX_STATE_WRITE_RGB=1, BGFX_STATE_WRITE_A=2, BGFX_STATE_PT_TRISTRIP=4,
 BGFX_SAMPLER_U_CLAMP=8,BGFX_SAMPLER_V_CLAMP=16,BGFX_TEXTURE_FORMAT_BGRA8=0,
 BGFX_TEXTURE_RT=32,BGFX_DISCARD_ALL=0,BGFX_CLEAR_NONE=0,BGFX_VIEW_MODE_SEQUENTIAL=0};
#include "types.inc"
#include "shader.inc"
static struct {
 BOOL ready,view_used;unsigned short view;bgfx_frame_buffer_handle_t view_framebuffer;
 unsigned short view_rect[4];unsigned long write_serial,post_width,post_height;
 bgfx_texture_handle_t post_texture,full_quad,samplers[4],fxaa_metrics,fxaa_bounds,fxaa_sampler;
 bgfx_frame_buffer_handle_t post_framebuffer;bgfx_program_handle_t copy;
 struct bg_shader *fxaa_vertex,*fxaa_fragment;
} bg;
static struct {int back_buffer;} device;
static struct bg_target target;
struct bg_shader {BOOL ready;};
static struct bg_shader vertex={TRUE},fragment={TRUE};
static int texture_attempts,texture_frees,framebuffer_frees,copy_count,fxaa_count;
static int anti_aliasing_value=-1,xgpu_bgfx=TRUE,prepare_calls;
static const char *saved_aa="fxaa";
static int fail_texture,fail_framebuffer,fail_link,phase,shader_requests;
static unsigned short next_texture=20,bound_framebuffer,bound_texture,scissor[4];
static unsigned short copy_scissor[4];
static unsigned long long draw_state;
static float metrics[4],bounds[4];
static const char *config_string(const char *name){CHECK(!strcmp(name,"display.anti_aliasing"),"unexpected setting");return saved_aa;}
static void anti_aliasing_prepare(void){prepare_calls++;}
static void platform_log(const char *text,...){(void)text;}
static struct bg_target *bg_target_get(const void *surface){(void)surface;return &target;}
static void bg_log_once(const char *text){(void)text;}
static void bgfx_destroy_frame_buffer(bgfx_frame_buffer_handle_t handle){CHECK(BGFX_HANDLE_IS_VALID(handle),"invalid framebuffer freed");framebuffer_frees++;}
static void bgfx_destroy_texture(bgfx_texture_handle_t handle){CHECK(BGFX_HANDLE_IS_VALID(handle),"invalid texture freed");texture_frees++;}
static bgfx_texture_handle_t bgfx_create_texture_2d(unsigned short w,unsigned short h,int mips,int layers,
 int format,unsigned long long flags,const void *memory,int handle)
{
 (void)mips;(void)layers;(void)format;(void)flags;(void)memory;(void)handle;texture_attempts++;
 CHECK(w==target.pixel_width && h==target.pixel_height,"scratch has wrong size");
 return (bgfx_texture_handle_t){fail_texture?UINT16_MAX:next_texture++};
}
static bgfx_frame_buffer_handle_t bgfx_create_frame_buffer_from_handles(int count,const bgfx_texture_handle_t *textures,int destroy)
{CHECK(count==1 && !destroy && BGFX_HANDLE_IS_VALID(*textures),"bad framebuffer ownership");return (bgfx_frame_buffer_handle_t){fail_framebuffer?UINT16_MAX:(unsigned short)(textures->idx+100)};}
static struct bg_shader *bg_shader_get(const char *source,int pixel,int flags,const char *what,int wait)
{CHECK(!flags&&!wait,"FXAA blocked on compiler");(void)what;CHECK(source==(pixel?fxaa_source:bg_blit_vertex_source),"shader source was copied or changed");shader_requests++;return pixel?&fragment:&vertex;}
static BOOL bg_shader_ready(struct bg_shader *shader){return shader->ready;}
static bgfx_program_handle_t bg_program_get(struct bg_shader *v,struct bg_shader *f)
{CHECK(v==&vertex&&f==&fragment,"wrong program pair");return (bgfx_program_handle_t){fail_link?UINT16_MAX:4};}
static bgfx_frame_buffer_handle_t bg_framebuffer_get(struct bg_target *color,struct bg_target *depth)
{CHECK(color==&target && !depth,"FXAA wrote depth");return (bgfx_frame_buffer_handle_t){400};}
static void bgfx_set_view_mode(unsigned short view,int sequential){(void)view;CHECK(sequential==BGFX_VIEW_MODE_SEQUENTIAL,"unordered postprocess");}
static void bgfx_set_view_clear(unsigned short view,int clear,int color,float depth,int stencil)
{(void)view;(void)color;(void)depth;(void)stencil;CHECK(clear==BGFX_CLEAR_NONE,"FXAA cleared native picture");}
static void bgfx_set_view_frame_buffer(unsigned short view,bgfx_frame_buffer_handle_t framebuffer)
{(void)view;bound_framebuffer=framebuffer.idx;}
static void bgfx_set_view_rect(unsigned short view,unsigned short x,unsigned short y,unsigned short w,unsigned short h,float z0,float z1)
{(void)view;(void)x;(void)y;(void)z0;(void)z1;CHECK(w==target.pixel_width && h==target.pixel_height,"FXAA changed viewport scale");}
static void bgfx_set_vertex_buffer(int stream,bgfx_texture_handle_t handle,int first,int count)
{(void)stream;(void)first;CHECK(handle.idx==bg.full_quad.idx&&count==4,"new postprocess geometry");}
static void bgfx_set_texture(int stage,bgfx_texture_handle_t sampler,bgfx_texture_handle_t texture,unsigned long long flags)
{(void)stage;(void)sampler;(void)flags;bound_texture=texture.idx;}
static void bgfx_set_state(unsigned long long state,int rgba){(void)rgba;draw_state=state;}
static void bgfx_set_scissor(unsigned short x,unsigned short y,unsigned short w,unsigned short h)
{scissor[0]=x;scissor[1]=y;scissor[2]=w;scissor[3]=h;}
static void bgfx_set_uniform(bgfx_texture_handle_t uniform,const float *value,int count)
{CHECK(count==1,"wrong uniform size");memcpy(uniform.idx==bg.fxaa_metrics.idx?metrics:bounds,value,4*sizeof(float));}
static void bgfx_submit(unsigned short view,bgfx_program_handle_t program,int depth,int discard)
{
 (void)view;(void)depth;(void)discard;
 if(program.idx==bg.copy.idx){CHECK(bound_framebuffer==bg.post_framebuffer.idx,"copy overwrote native target");
  CHECK(bound_texture==target.texture.idx && bound_texture!=bg.post_texture.idx,"feedback in source copy");
  memcpy(copy_scissor,scissor,sizeof(copy_scissor));copy_count++;phase=1;
 }else{CHECK(program.idx==4&&phase==1,"FXAA did not copy first");CHECK(bound_framebuffer==400,"FXAA wrong destination");
  CHECK(bound_texture==bg.post_texture.idx&&bound_texture!=target.texture.idx,"feedback in FXAA");
  CHECK(!(draw_state&BGFX_STATE_WRITE_A),"FXAA overwrote native alpha");fxaa_count++;phase=0;
 }
}
#include "under_test.inc"
static void choose(const char *name){saved_aa=name;anti_aliasing_read();}
int main(int argc,char **argv)
{
 const char *case_name=argc>1?argv[1]:"";
 bg.ready=TRUE;bg.full_quad.idx=1;bg.copy.idx=3;bg.fxaa_metrics.idx=6;bg.fxaa_bounds.idx=7;bg.fxaa_sampler.idx=8;
 bg.post_texture=(bgfx_texture_handle_t)BGFX_INVALID_HANDLE;bg.post_framebuffer=(bgfx_frame_buffer_handle_t)BGFX_INVALID_HANDLE;
 bg.view_framebuffer=(bgfx_frame_buffer_handle_t)BGFX_INVALID_HANDLE;
 target.width=640;target.height=480;target.pixel_width=1280;target.pixel_height=960;
 target.scale[0]=target.scale[1]=2.0f;target.texture.idx=10;
 CASE("rectangle") {
  unsigned short rect[4];float m[4],b[4];
  CHECK(bg_fxaa_rectangle(&target,-10,240,650,490,rect,m,b),"clipped rectangle rejected");
  CHECK(rect[0]==0&&rect[1]==480&&rect[2]==1280&&rect[3]==480,"split rectangle wrong");
  CHECK(m[0]==1.f/1280&&m[1]==1.f/960&&b[0]==.5f/1280&&b[1]==480.5f/960&&b[3]==959.5f/960,"texel bounds crossed window edge");
  CHECK(!bg_fxaa_rectangle(&target,0,0,0,480,rect,m,b),"empty view admitted");
 } else CASE("split") {
  bg_screen_anti_alias(0,240,640,480);CHECK(copy_count==1&&fxaa_count==1&&scissor[1]==480&&scissor[3]==480,"split pass missing/wrong scissor");
  CHECK(!memcmp(copy_scissor,scissor,sizeof(scissor)),"split source copy touched other views");
  target.texture.idx=11;bg_screen_anti_alias(0,0,640,240);
  CHECK(texture_attempts==1&&copy_count==2&&fxaa_count==2&&scissor[1]==0&&scissor[3]==480,"eyes/windows did not share ordered scratch");
 } else CASE("reuse-resize") {
  bg_screen_anti_alias(0,0,640,480);bg_screen_anti_alias(0,0,640,480);CHECK(texture_attempts==1,"same-size pass allocated again");
  target.pixel_width=1920;target.scale[0]=3.f;bg_screen_anti_alias(0,0,640,480);
  CHECK(texture_attempts==2&&texture_frees==1&&framebuffer_frees==1&&fxaa_count==3,"resize did not retire old resources");
  CHECK(shader_requests==2,"each pass repeated shader translation/hash lookup");
 } else CASE("off") {
  choose("off");bg_screen_anti_alias(0,0,640,480);CHECK(!texture_attempts&&!copy_count&&!fxaa_count,"off did work");
  choose("fxaa");bg_screen_anti_alias(0,0,640,480);choose("off");bg_screen_anti_alias(0,0,640,480);
  CHECK(texture_frees==1&&framebuffer_frees==1&&!bg.post_width,"off retained source copy");
 } else CASE("shader-pending") {
  vertex.ready=FALSE;bg_screen_anti_alias(0,0,640,480);CHECK(!texture_attempts&&!copy_count,"pending vertex changed picture");
  vertex.ready=TRUE;fragment.ready=FALSE;bg_screen_anti_alias(0,0,640,480);CHECK(!texture_attempts&&!copy_count,"pending fragment changed picture");
  fragment.ready=TRUE;bg_screen_anti_alias(0,0,640,480);CHECK(fxaa_count==1,"ready shader never recovered");
 } else CASE("failed-link") {
  fail_link=TRUE;bg_screen_anti_alias(0,0,640,480);CHECK(!texture_attempts&&!copy_count,"bad link changed picture");
 } else CASE("allocation-failure") {
  fail_texture=TRUE;bg_screen_anti_alias(0,0,640,480);bg_screen_anti_alias(0,0,640,480);
  CHECK(texture_attempts==1&&!copy_count&&!fxaa_count,"failed allocation looped or drew");
 } else CASE("framebuffer-failure") {
  fail_framebuffer=TRUE;bg_screen_anti_alias(0,0,640,480);bg_screen_anti_alias(0,0,640,480);
  CHECK(texture_attempts==1&&texture_frees==1&&!copy_count&&!fxaa_count,"failed framebuffer leaked/retried or drew");
 } else CASE("limits") {
  bg.view=BG_MAXIMUM_VIEWS-5;bg_screen_anti_alias(0,0,640,480);CHECK(!copy_count&&!texture_attempts,"exhausted view list admitted pass");
  bg.view=0;bg.fxaa_sampler=(bgfx_texture_handle_t)BGFX_INVALID_HANDLE;bg_screen_anti_alias(0,0,640,480);CHECK(!texture_attempts,"invalid uniform admitted pass");
  bg.fxaa_sampler.idx=8;target.pixel_width=65536;bg_screen_anti_alias(0,0,640,480);CHECK(!texture_attempts,"oversized target admitted");
 } else CASE("saved-settings") {
  bg_screen_anti_alias(0,0,640,480);CHECK(anti_aliasing()==_anti_aliasing_fxaa&&fxaa_count==1&&prepare_calls==1,"saved FXAA never reached its pass");
  choose("msaa4x");bg_screen_anti_alias(0,0,640,480);CHECK(anti_aliasing()==_anti_aliasing_msaa&&anti_aliasing_values[anti_aliasing_value].samples==4&&fxaa_count==1&&texture_frees==1,"MSAA selection or old FXAA retirement wrong");
  choose("smaa");CHECK(anti_aliasing()==_anti_aliasing_msaa&&anti_aliasing_values[anti_aliasing_value].samples==2&&!strcmp(saved_aa,"smaa"),"unsupported SMAA fallback changed saved preference");
  choose("ssaa2x");CHECK(anti_aliasing()==_anti_aliasing_msaa&&anti_aliasing_values[anti_aliasing_value].samples==2,"unsupported SSAA fallback changed");
  xgpu_bgfx=FALSE;choose("smaa");CHECK(anti_aliasing()==_anti_aliasing_smaa,"OpenGL lost native SMAA");
  choose("ssaa2x");CHECK(anti_aliasing()==_anti_aliasing_ssaa,"OpenGL lost native supersampling");
  xgpu_bgfx=TRUE;choose("unknown");bg_screen_anti_alias(0,0,640,480);CHECK(anti_aliasing()==_anti_aliasing_off&&fxaa_count==1,"unknown setting drew FXAA");
  choose("fxaa");bg_screen_anti_alias(0,0,640,480);CHECK(fxaa_count==2&&shader_requests==2,"return to FXAA did not reuse cached shaders");
 } else return 2;
 return 0;
}
