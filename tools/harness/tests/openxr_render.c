#include "harness.h"
#include <stdint.h>
#include <stdbool.h>
#define HALO_OPENXR
#define HALO_BGFX
#define HALO_GL_ES
typedef int BOOL, bgfx_texture_format_t;
typedef unsigned long DWORD;
typedef struct {unsigned short idx;} bgfx_texture_handle_t;
typedef bgfx_texture_handle_t bgfx_frame_buffer_handle_t;
typedef bgfx_texture_handle_t bgfx_program_handle_t;
#define BGFX_INVALID_HANDLE {UINT16_MAX}
#define BGFX_HANDLE_IS_VALID(h) ((h).idx!=UINT16_MAX)
enum {BGFX_TEXTURE_FORMAT_BGRA8=0,BGFX_TEXTURE_FORMAT_RGBA8=1,
 BGFX_CAPS_FORMAT_TEXTURE_FRAMEBUFFER_MSAA_X2=2,BGFX_CAPS_FORMAT_TEXTURE_FRAMEBUFFER_MSAA_X4=4,
 BGFX_CAPS_FORMAT_TEXTURE_FRAMEBUFFER_MSAA_X8=8,BGFX_CAPS_FORMAT_TEXTURE_FRAMEBUFFER_MSAA_X16=16,
 BGFX_TEXTURE_RT=32,BGFX_TEXTURE_SRGB=64,BGFX_TEXTURE_RT_MSAA_X2=128,BGFX_TEXTURE_RT_MSAA_X4=256,
 BGFX_TEXTURE_RT_MSAA_X8=512,BGFX_TEXTURE_RT_WRITE_ONLY=1024,BGFX_SAMPLER_U_CLAMP=2048,BGFX_SAMPLER_V_CLAMP=4096,
 BGFX_VIEW_MODE_SEQUENTIAL=0,BGFX_CLEAR_NONE=0,BGFX_CLEAR_COLOR=1,BGFX_STATE_WRITE_RGB=1,
 BGFX_STATE_WRITE_A=2,BGFX_STATE_PT_TRISTRIP=4,BGFX_DISCARD_ALL=0};
#define SCREEN_HEIGHT 480
#define SECONDARY_TARGET_WIDTH 320
#define SECONDARY_TARGET_HEIGHT 240
enum {_render_target_secondary};
typedef struct {DWORD Data;unsigned long width,height;BOOL depth;} D3DSurface;
typedef struct {unsigned int formats[3];} bgfx_caps_t;
struct platform_input_state {int menus;};
#include "types.inc"
#include "state.inc"
static struct {
 BOOL ready,view_used;int depth_format;unsigned short view,view_rect[4];
 bgfx_texture_handle_t full_quad,samplers[4];bgfx_program_handle_t copy;
 bgfx_frame_buffer_handle_t framebuffer,view_framebuffer;
 struct bg_framebuffer *framebuffers;struct bg_target *targets[BG_TARGET_BUCKETS];
} bg;
static struct {D3DSurface back_buffer;} device;
static float screen_scale[2]={2,2};
static int bg_vulkan=TRUE,mode=_anti_aliasing_msaa,anti_aliasing_value;
static struct {int samples;} anti_aliasing_values[]={{8}};
static bgfx_caps_t caps;
static struct vr_openxr_image images[2]={{1001,1280,1440,1},{1002,1280,1440,1}};
static int acquired[2],release_count,owned_created,owned_freed,framebuffers_freed,imported_count;
static int menu,fail_allocation,fail_framebuffer,fail_submission;
static int copies,layer=-1,queue_finished,events,copy_event,queue_event,end_event;
static unsigned short next_texture=10,next_framebuffer=200,source_texture,destination_framebuffer;
static struct {int external,samples;unsigned long width,height;} textures[128];
static struct {unsigned short color,depth;} framebuffers[256];
static int anti_aliasing(void){return mode;}
static int bg_active(void){return bg.ready;}
static const bgfx_caps_t *bgfx_get_caps(void){return &caps;}
static void platform_log(const char *text,...){(void)text;}
static void bg_log_once(const char *text){(void)text;}
static int halo_screen_width(void){return 640;}
static void surface_dimensions(const D3DSurface *s,unsigned long *w,unsigned long *h,BOOL *depth){*w=s->width;*h=s->height;*depth=s->depth;}
static BOOL surface_is_shadow_map(const D3DSurface *s){(void)s;return FALSE;}
static void render_target_scale(int target,float scale[2]){(void)target;scale[0]=scale[1]=2;}
static int halo_shadow_map_scale(void){return 1;}
static bgfx_texture_handle_t bgfx_create_texture_2d(uint16_t w,uint16_t h,bool mips,uint16_t layers,
 bgfx_texture_format_t format,uint64_t flags,const void *memory,uint64_t external)
{
 unsigned short index=next_texture++;(void)format;(void)memory;CHECK(!mips&&layers==1,"wrong eye texture layout");
 CHECK(index<128,"fixture texture table full");
 if(external)imported_count++;else if(!((flags&BGFX_TEXTURE_RT_WRITE_ONLY)!=0))owned_created++;
 if(!external && fail_allocation)return (bgfx_texture_handle_t)BGFX_INVALID_HANDLE;
 textures[index].external=external!=0;textures[index].width=w;textures[index].height=h;
 textures[index].samples=flags&BGFX_TEXTURE_RT_MSAA_X8?8:flags&BGFX_TEXTURE_RT_MSAA_X4?4:flags&BGFX_TEXTURE_RT_MSAA_X2?2:0;
 return (bgfx_texture_handle_t){index};
}
static void bgfx_destroy_texture(bgfx_texture_handle_t h){CHECK(!textures[h.idx].external,"freed runtime image");owned_freed++;}
static bgfx_frame_buffer_handle_t bgfx_create_frame_buffer_from_handles(uint8_t count,const bgfx_texture_handle_t *h,bool destroy)
{
 unsigned short index=next_framebuffer++;CHECK(count>0&&count<=2&&!destroy,"wrong framebuffer ownership");
 CHECK(index<256,"fixture framebuffer table full");
 if(fail_framebuffer&&!textures[h[0].idx].external&&textures[h[0].idx].samples)return (bgfx_frame_buffer_handle_t)BGFX_INVALID_HANDLE;
 framebuffers[index].color=h[0].idx;framebuffers[index].depth=count==2?h[1].idx:UINT16_MAX;
 return (bgfx_frame_buffer_handle_t){index};
}
static void bgfx_destroy_frame_buffer(bgfx_frame_buffer_handle_t h){CHECK(BGFX_HANDLE_IS_VALID(h),"invalid framebuffer destroy");framebuffers_freed++;}
static void bgfx_set_view_mode(unsigned short view,int seq){(void)view;CHECK(seq==BGFX_VIEW_MODE_SEQUENTIAL,"unordered eyes");}
static void bgfx_set_view_clear(unsigned short view,int clear,int color,float depth,int stencil){(void)view;(void)clear;(void)color;(void)depth;(void)stencil;}
static void bgfx_set_view_frame_buffer(unsigned short view,bgfx_frame_buffer_handle_t h){(void)view;destination_framebuffer=h.idx;}
static void bgfx_set_view_rect(unsigned short view,unsigned short x,unsigned short y,uint16_t w,uint16_t h,float z0,float z1){(void)view;(void)x;(void)y;(void)w;(void)h;(void)z0;(void)z1;}
static void bgfx_set_scissor(unsigned short x,unsigned short y,unsigned short w,unsigned short h){(void)x;(void)y;(void)w;(void)h;}
static void bgfx_set_vertex_buffer(int stream,bgfx_texture_handle_t h,int first,int count){(void)stream;(void)first;CHECK(h.idx==bg.full_quad.idx&&count==4,"resolve used new geometry");}
static void bgfx_set_texture(int stage,bgfx_texture_handle_t sampler,bgfx_texture_handle_t h,uint64_t flags){(void)stage;(void)sampler;(void)flags;source_texture=h.idx;}
static void bgfx_set_state(uint64_t state,int rgba){(void)state;(void)rgba;}
static void bgfx_touch(unsigned short view){(void)view;}
static void bgfx_submit(unsigned short view,bgfx_program_handle_t program,int depth,int discard)
{
 unsigned short to=framebuffers[destination_framebuffer].color;(void)view;(void)depth;(void)discard;
 CHECK(program.idx==bg.copy.idx,"wrong resolve program");CHECK(textures[to].external,"resolve did not reach runtime image");
 CHECK(!textures[source_texture].external&&textures[source_texture].samples>=2,"runtime image was rendered multisampled/read back");
 CHECK(to!=source_texture,"resolve feedback");copies++;copy_event=++events;
}
static int vr_openxr_frame_begin(struct vr_openxr_frame *frame){frame->should_render=1;return 1;}
static void platform_input_read(struct platform_input_state *input,BOOL consume){(void)consume;input->menus=menu;}
static int vr_openxr_eye_acquire(int eye,struct vr_openxr_image *image){CHECK(!acquired[eye],"eye acquired twice");acquired[eye]=1;*image=images[eye];return 1;}
static int bgfx_vk_openxr_finish_frame(void){queue_finished=1;queue_event=++events;return !fail_submission;}
static void vr_openxr_eye_release(int eye){CHECK(queue_finished,"image released before queue submission");if(acquired[eye]){acquired[eye]=0;release_count++;}++events;}
static void vr_openxr_frame_end(enum vr_openxr_layer value,float aspect){(void)aspect;CHECK(!acquired[0]&&!acquired[1],"frame ended holding image");layer=value;end_event=++events;}
#include "under_test.inc"
static void start(void){struct vr_openxr_frame frame;queue_finished=0;CHECK(xgpu_openxr_frame_begin(&frame),"frame did not start");}
static void eyes(void){for(int eye=0;eye<2;eye++){CHECK(xgpu_openxr_eye_begin(eye),"eye begin failed");xgpu_openxr_eye_end();}}
static int cached_texture(unsigned short index){for(struct bg_framebuffer *f=bg.framebuffers;f;f=f->next)if(f->color==index||f->depth==index)return 1;return 0;}
int main(int argc,char **argv)
{
 const char *case_name=argc>1?argv[1]:"";
 bg.ready=TRUE;bg.depth_format=2;bg.copy.idx=3;bg.full_quad.idx=1;bg.framebuffer=(bgfx_frame_buffer_handle_t)BGFX_INVALID_HANDLE;
 bg.view_framebuffer=(bgfx_frame_buffer_handle_t)BGFX_INVALID_HANDLE;
 device.back_buffer=(D3DSurface){1,640,480,FALSE};caps.formats[0]=caps.formats[1]=caps.formats[2]=2|4;
 CASE("pipeline") {
  D3DSurface depth={2,640,480,TRUE},secondary={3,320,240,FALSE};start();
  for(int eye=0;eye<2;eye++){
   CHECK(xgpu_openxr_eye_begin(eye),"eye failed");CHECK(bg_openxr.draw_target==&bg_openxr.msaa[eye]&&bg_openxr.draw_target->samples==4,"requested samples ignored");
   CHECK(bg_target_get(&device.back_buffer)==bg_openxr.draw_target,"draws bypassed MSAA target");
   struct bg_target *z=bg_target_get(&depth),*color=bg_target_get(&secondary);
   CHECK(z&&color&&z->samples==4&&color->samples==4&&z->pixel_height==images[eye].height,"depth/intermediate samples or pixels disagree");
   CHECK(BGFX_HANDLE_IS_VALID(bg_framebuffer_get(bg_openxr.draw_target,z)),"mixed attachment failed");
   xgpu_openxr_eye_end();CHECK(bg_openxr.rendered[eye]&&!bg_openxr.draw_target,"eye not resolved/cleared");
  }
  bg_openxr_frame_submitted();CHECK(copies==2&&layer==_vr_layer_stereo&&release_count==2,"stereo handoff incomplete");
  CHECK(copy_event<queue_event&&queue_event<end_event,"resolve/submission/release ordering wrong");
 } else CASE("formats") {
  caps.formats[0]=2|4;caps.formats[1]=2|4|8;caps.formats[2]=2|4|8;images[0].bgra=0;
  start();CHECK(xgpu_openxr_eye_begin(0)&&bg_openxr.draw_target->samples==4,"RGBA8 ignored intermediate caps");xgpu_openxr_eye_end();
  caps.formats[0]=0;CHECK(xgpu_openxr_eye_begin(1)&&!bg_openxr.draw_target->samples,"unsupported MSAA did not fall back");xgpu_openxr_eye_end();bg_openxr_frame_submitted();
  caps.formats[0]=2|4|8;caps.formats[1]=2;images[0].handle+=10;
  start();CHECK(xgpu_openxr_eye_begin(0)&&bg_openxr.draw_target->samples==2,"RGBA8 eye caps ignored");
  CHECK(prep_screen_samples()==bg_openxr.draw_target->samples,"prepared alpha-test samples disagree with headset");
  xgpu_openxr_eye_end();bg_openxr_frame_submitted();
  caps.formats[0]=4;caps.formats[1]=2|8;images[0].handle+=10;
  start();CHECK(xgpu_openxr_eye_begin(0)&&!bg_openxr.draw_target->samples,"disjoint sample masks silently chose unsupported count");
  CHECK(!prep_screen_samples(),"prepared alpha samples ignored disjoint formats");xgpu_openxr_eye_end();bg_openxr_frame_submitted();
 } else CASE("reuse-resize") {
  start();eyes();bg_openxr_frame_submitted();CHECK(owned_created==2,"eye resource count wrong");
  unsigned short old=bg_openxr.msaa[0].texture.idx;images[0].handle+=10;images[1].handle+=10;
  start();eyes();bg_openxr_frame_submitted();CHECK(owned_created==2&&imported_count==4,"allocated per swapchain image/frame");
  images[0].width=1600;images[0].handle+=10;start();eyes();bg_openxr_frame_submitted();
  CHECK(owned_created==3&&owned_freed==1&&framebuffers_freed&&!cached_texture(old),"resize retained owned texture/framebuffers");
 } else CASE("off-menu") {
  start();eyes();bg_openxr_frame_submitted();mode=_anti_aliasing_off;start();
  CHECK(owned_freed==2&&!bg_openxr.msaa[0].samples&&!bg_openxr.msaa[1].samples,"off retained eye targets");eyes();bg_openxr_frame_submitted();CHECK(copies==2,"off introduced resolve");
  mode=_anti_aliasing_msaa;menu=1;start();CHECK(xgpu_openxr_eye_begin(0)&&!bg_openxr.draw_target->samples,"menu used MSAA");xgpu_openxr_eye_end();
  bg_openxr.menu_rendered=TRUE;bg_openxr_frame_submitted();CHECK(layer==_vr_layer_menu&&owned_created==2,"menu did not keep native single canvas");
 } else CASE("allocation-failure") {
  fail_allocation=1;start();eyes();bg_openxr_frame_submitted();int attempts=owned_created;
  start();eyes();bg_openxr_frame_submitted();CHECK(!copies&&owned_created==attempts&&layer==_vr_layer_stereo,"allocation fallback retried or lost native frame");
 } else CASE("framebuffer-failure") {
  fail_framebuffer=1;start();eyes();bg_openxr_frame_submitted();int attempts=owned_created;
  start();eyes();bg_openxr_frame_submitted();CHECK(!copies&&owned_created==attempts&&owned_freed==2&&layer==_vr_layer_stereo,"framebuffer fallback looped/leaked");
 } else CASE("resolve-failure") {
  start();CHECK(xgpu_openxr_eye_begin(0),"eye failed");bg.view=BG_MAXIMUM_VIEWS-2;xgpu_openxr_eye_end();
  CHECK(bg_openxr.failed&&!bg_openxr.rendered[0],"failed resolve claimed rendered eye");bg_openxr_frame_submitted();CHECK(layer==_vr_layer_none&&release_count==1,"failed pair submitted/held image");
 } else CASE("invalid-image") {
  start();images[0].width=0;CHECK(!xgpu_openxr_eye_begin(0),"invalid image admitted");bg_openxr_frame_submitted();CHECK(layer==_vr_layer_none&&release_count==1&&!copies,"invalid image leaked");
  images[0].width=1280;start();CHECK(xgpu_openxr_eye_begin(0),"valid image did not recover");xgpu_openxr_eye_end();bg_openxr_frame_submitted();
  images[0].width=1600;start();CHECK(!xgpu_openxr_eye_begin(0),"reused image handle changed dimensions silently");bg_openxr_frame_submitted();
  CHECK(layer==_vr_layer_none&&release_count==3,"changed image contract leaked or submitted");
 } else CASE("logical-view") {
  start();eyes();bg_openxr_frame_submitted();device.back_buffer.width=800;start();CHECK(xgpu_openxr_eye_begin(0),"eye failed");
  CHECK(owned_created==2&&bg_openxr.draw_target->width==800&&bg_openxr.draw_target->scale[0]==1.6f,"logical view change reallocated or kept old scale");xgpu_openxr_eye_end();bg_openxr_frame_submitted();
 } else return 2;
 return 0;
}
