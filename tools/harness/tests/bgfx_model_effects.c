#include "harness.h"
#include <stdint.h>
#include <stdbool.h>
#include <stdarg.h>
#define HALO_BGFX
typedef int BOOL;
typedef unsigned int DWORD,GLuint;
typedef struct {uint16_t idx;} bgfx_texture_handle_t;
typedef bgfx_texture_handle_t bgfx_uniform_handle_t;
typedef bgfx_texture_handle_t bgfx_program_handle_t;
#define BGFX_INVALID_HANDLE {UINT16_MAX}
#define BGFX_HANDLE_IS_VALID(h) ((h).idx!=UINT16_MAX)
#define XGPU_VERTEX_ATTRIBUTE_COUNT 16
#define XGPU_VERTEX_CONSTANT_COUNT 192
#define XGPU_MODEL_LIGHT_COUNT 12
enum {BGFX_TEXTURE_FORMAT_BGRA8, BGFX_CAPS_FORMAT_TEXTURE_3D=1, BGFX_CAPS_FORMAT_TEXTURE_VERTEX=2,
 BGFX_UNIFORM_TYPE_VEC4=1,BGFX_UNIFORM_TYPE_SAMPLER=2,BGFX_TEXTURE_NONE=0,
 BGFX_SAMPLER_U_CLAMP=1,BGFX_SAMPLER_V_CLAMP=2,BGFX_SAMPLER_W_CLAMP=4,_bg_shader_depth_bias=1};
typedef struct {uint32_t formats[1];struct {uint32_t maxTextureSamplers;} limits;} bgfx_caps_t;
struct nv2a_vertex_lighting {int lights;};
struct vertex_shader_object {DWORD *instructions;unsigned long instruction_count;struct nv2a_vertex_lighting lighting;BOOL lighting_failed,plasma_model,plasma_lighting_failed;void *bg_shader[8];};
struct nv2a_pixel_shader_key {unsigned char per_pixel_lighting,plasma_model,alpha_test_samples,count_samples;};
struct bg_shader {BOOL lit,plasma;};
struct bg_program {bgfx_program_handle_t program;};
struct xgpu_texture_description {int depth,cube_map;};
#include "types.inc"
struct xgpu_program_key {unsigned long long vertex_code;unsigned long flags,packed_mask;struct nv2a_pixel_shader_key pixel;};
static struct {struct prep_pending pending[2];unsigned long pending_count,made,failed;} prep;
static struct vertex_shader_object *preparing;
static struct bg_shader prepared;
#define PREP_THREAD_SECONDS 60
static struct {bgfx_uniform_handle_t plasma_uniforms[4],plasma_samplers[2];bgfx_texture_handle_t plasma_white;} bg;
static bgfx_caps_t caps={ {3},{8} };
static BOOL bg_vulkan,bg_d3d11,enabled=1;
static int made,pending_vertex,pending_pixel,pending_all,failed_link;
static struct bg_shader shaders[8],pixels[2][2];
static int uniform_creates,texture_creates,uniform_frees,texture_frees,fail_uniform;
static int field_calls[2],texture_calls,missing,fail_upload,flat,cube,view;
static unsigned int white_copy;
static float uniforms[4][4];
static unsigned short bound[2];
static int per_pixel_lighting(void){return enabled;}
static char *nv2a_vertex_shader_to_glsl(const DWORD *instructions,unsigned long count,unsigned long packed,
 const struct nv2a_vertex_lighting *lighting,BOOL plasma)
{
 CHECK(instructions&&count==1&&!packed,"wrong translator input");
 return _strdup(plasma?(lighting?"lit plasma":"plasma"):(lighting?"lit":"stock"));
}
static struct bg_shader *bg_shader_get(const char *text,BOOL fragment,int flags,const char *name,BOOL wait)
{
 CHECK(made<8&&!fragment&&!flags&&!wait&&!strcmp(name,"vertex"),"wrong compiler request");
 shaders[made].lit=strstr(text,"lit")!=NULL;shaders[made].plasma=strstr(text,"plasma")!=NULL;return &shaders[made++];
}
static BOOL bg_shader_ready(struct bg_shader *s){return !(pending_vertex&&s->plasma)&&!(pending_all&&s->lit);}
static struct bg_shader *bg_fragment_shader(const struct nv2a_pixel_shader_key *key,int flags)
{
 (void)flags;int lit=key->per_pixel_lighting!=0,plasma=key->plasma_model!=0;
 pixels[lit][plasma]=(struct bg_shader){lit,plasma};return pending_pixel&&plasma?NULL:&pixels[lit][plasma];
}
static struct bg_program *bg_program_entry(struct bg_shader *vertex,struct bg_shader *pixel)
{
 static struct bg_program entries[2][2]={{{{10}},{{11}}},{{{12}},{{13}}}},broken={BGFX_INVALID_HANDLE};
 CHECK(vertex->lit==pixel->lit&&(!pixel->plasma||vertex->plasma),"mixed shader stages");
 return failed_link&&vertex->plasma?&broken:&entries[vertex->lit][vertex->plasma];
}
static const bgfx_caps_t *bgfx_get_caps(void){return &caps;}
static bgfx_uniform_handle_t bgfx_create_uniform(const char *name,int type,int count)
{
 CHECK(count==1&&((strstr(name,"texture")!=NULL)==(type==BGFX_UNIFORM_TYPE_SAMPLER)),"wrong uniform kind");
 uniform_creates++;return (bgfx_uniform_handle_t){uniform_creates==fail_uniform?UINT16_MAX:(uint16_t)uniform_creates};
}
static void bgfx_destroy_uniform(bgfx_uniform_handle_t handle){CHECK(BGFX_HANDLE_IS_VALID(handle),"invalid free");uniform_frees++;}
static const void *bgfx_copy(const void *data,unsigned int size){CHECK(size==4,"unexpected fallback texture size");memcpy(&white_copy,data,4);return &white_copy;}
static bgfx_texture_handle_t bgfx_create_texture_3d(uint16_t w,uint16_t h,uint16_t d,bool mips,int format,
 uint64_t flags,const void *data,uint64_t external)
{
 CHECK(w==1&&h==1&&d==1&&!mips&&format==BGFX_TEXTURE_FORMAT_BGRA8&&!flags&&!external,"wrong fallback texture");
 CHECK(*(const unsigned int *)data==0xffffffff,"occlusion fallback is not white");texture_creates++;return (bgfx_texture_handle_t){50};
}
static void bgfx_destroy_texture(bgfx_texture_handle_t handle){CHECK(handle.idx==50,"released native field texture");texture_frees++;}
static void bg_log_once(const char *text){(void)text;}
static void *plasma_occlusion_model_texture(int slot,float transform[4])
{
 static DWORD native[2][5];field_calls[slot]++;transform[0]=(float)(slot+view);transform[1]=2;transform[2]=3;transform[3]=0.25f;
 native[slot][0]=(DWORD)slot;return missing?NULL:native[slot];
}
static BOOL bg_texture_get(const DWORD *resource,const DWORD *palette,struct xgpu_texture_description *description,
 bgfx_texture_handle_t *handle,BOOL *mipmapped,long *override)
{
 CHECK(!palette,"visibility needs no palette");texture_calls++;*description=(struct xgpu_texture_description){flat?1:16,cube};
 handle->idx=(uint16_t)(100+resource[0]+view);*mipmapped=FALSE;*override=-1;return !fail_upload;
}
static void bgfx_set_texture(uint8_t stage,bgfx_uniform_handle_t sampler,bgfx_texture_handle_t texture,uint32_t flags)
{
 CHECK(stage>=6&&stage<=7&&sampler.idx==bg.plasma_samplers[stage-6].idx,"replaced material stage or wrong sampler");
 CHECK(flags==7,"visibility did not clamp all axes");bound[stage-6]=texture.idx;
}
static void bgfx_set_uniform(bgfx_uniform_handle_t uniform,const void *data,uint16_t count)
{
 CHECK(count==1&&uniform.idx>=1&&uniform.idx<=4,"wrong lighting uniform");memcpy(uniforms[uniform.idx-1],data,16);
}
static int bg_active(void){return 1;}
static struct vertex_shader_object *prep_vertex_program(unsigned long long hash){(void)hash;return preparing;}
static unsigned char prep_screen_samples(void){return 0;}
static double prep_seconds(void){return 1.0;}
static void prep_pending_add(const struct prep_pending *p){CHECK(prep.pending_count<2,"fixture queue full");prep.pending[prep.pending_count++]=*p;}
static void prep_pending_remove(unsigned long i){prep.pending[i]=prep.pending[--prep.pending_count];}
static bgfx_program_handle_t bg_program_get(struct bg_shader *vertex,struct bg_shader *pixel){prepared=*vertex;return bg_program_entry(vertex,pixel)->program;}
/* The actual preparation functions compile in full. Their OpenGL branch is
 * excluded at run time by the active backend, and must never execute here. */
static int xgpu_parallel_compile;
static GLuint vertex_shader_make(struct vertex_shader_object *o,BOOL immediate,BOOL lit,BOOL plasma,unsigned long packed,BOOL wait)
{(void)o;(void)immediate;(void)lit;(void)plasma;(void)packed;(void)wait;abort();}
static GLuint fragment_shader_make(const struct nv2a_pixel_shader_key *key,BOOL wait){(void)key;(void)wait;abort();}
static void *program_find(GLuint vertex,GLuint pixel){(void)vertex;(void)pixel;abort();}
static GLuint glCreateProgram(void){abort();}
static void glAttachShader(GLuint program,GLuint shader){(void)program;(void)shader;abort();}
static void glLinkProgram(GLuint program){(void)program;abort();}
static int xgpu_program_finish(GLuint program,BOOL wait){(void)program;(void)wait;abort();}
static void prep_gl_take(struct prep_pending *p,BOOL linked){(void)p;(void)linked;abort();}
#include "under_test.inc"
static void shader_case(const char *name)
{
 bg_vulkan=!strcmp(name,"shader-vulkan");bg_d3d11=!strcmp(name,"shader-d3d11");
 for(int fragment=0;fragment<2;fragment++){
  struct xgpu_text text={0};unsigned long bytes=0;
  if(fragment)for(int stage=0;stage<4;stage++)xgpu_text_append(&text,"uniform sampler2D tex%d;\n",stage);
  if(fragment)model_lighting(&text,TRUE,TRUE);else plasma_model_emit_uniform(&text);
  char *rewritten=bg_vulkan_glsl(text.buffer,fragment,0,&bytes);
  CHECK(rewritten&&strstr(rewritten,"vec4 plasma_model_slots_v4;")&&strstr(rewritten,"vec4 plasma_occ_enabled_v4;"),"effect uniforms lost");
  CHECK(strstr(rewritten,bg_vulkan?"binding = 8) uniform texture3D plasma_occ_texture0_texture":"binding = 6) uniform sampler3D plasma_occ_texture0"),"first field binding changed by material stages");
  CHECK(strstr(rewritten,bg_vulkan?"binding = 9) uniform texture3D plasma_occ_texture1_texture":"binding = 7) uniform sampler3D plasma_occ_texture1"),"second field binding changed by material stages");
  CHECK(bytes>=64&&strstr(rewritten,"textureLod(plasma_occ_texture0"),"native visibility lookup missing");
  if(fragment)CHECK(strstr(rewritten,"plasma_model_falloff")&&strstr(rewritten,"model_lights[11]"),"inherited point/ambient lighting lost");
  free(rewritten);free(text.buffer);
 }
}
int main(int argc,char **argv)
{
 const char *case_name=argc>1?argv[1]:"";DWORD instruction=42;
 if(!strncmp(case_name,"shader-",7)){shader_case(case_name);return 0;}
 CASE("capabilities"){
  caps.limits.maxTextureSamplers=7;bg_model_effects_start();CHECK(!bg_model_effects_supported()&&!uniform_creates&&!texture_creates,"exceeded sampler budget");
  caps.limits.maxTextureSamplers=8;caps.formats[0]=1;bg_model_effects_start();CHECK(!bg_model_effects_supported()&&!uniform_creates,"vertex volume unsupported but enabled");return 0;
 }
 if(!strcmp(case_name,"startup-failure"))fail_uniform=3;
 bg_model_effects_start();
 CASE("startup-failure"){
  CHECK(!bg_model_effects_supported()&&uniform_frees==5&&texture_frees==1,"partial startup leaked or stayed enabled");
  for(int i=0;i<180;i++)CHECK(!bg_model_effects_supported(),"failed startup enabled later");
  CHECK(uniform_creates==6&&texture_creates==1,"startup retried each draw");return 0;
 }
 CHECK(bg_model_effects_supported(),"fixtures could not start effects");
 if(!strcmp(case_name,"preparation")){
  struct vertex_shader_object o={0};struct xgpu_program_key record={0};o.instructions=&instruction;o.instruction_count=1;o.lighting.lights=2;o.plasma_model=TRUE;preparing=&o;
  record.flags=_xgpu_key_immediate_bit|_xgpu_key_lit_bit|_xgpu_key_plasma_bit;pending_vertex=1;
  CHECK(prep_start(&record)&&prep.pending_count==1&&prep.pending[0].plasma&&prep.pending[0].immediate&&prep.pending[0].key.plasma_model,"pixel preparation lost effect flag");
  pending_vertex=0;prep_collect(2.0,FALSE);CHECK(!prep.pending_count&&prep.made==1&&prepared.plasma&&prepared.lit,"pending pixel pair changed variant");
  record.flags=_xgpu_key_plasma_bit;pending_vertex=1;
  CHECK(prep_start(&record)&&prep.pending_count==1&&prep.pending[0].plasma&&!prep.pending[0].key.plasma_model,"vertex-only preparation lost effect flag");
  pending_vertex=0;prep_collect(2.0,FALSE);CHECK(!prep.pending_count&&prep.made==2&&prepared.plasma&&!prepared.lit,"pending vertex pair changed variant");
  caps.formats[0]=0;record.flags=_xgpu_key_lit_bit|_xgpu_key_plasma_bit;
  CHECK(prep_start(&record)&&prep.made==3&&!prepared.plasma&&prepared.lit,"unsupported effects prepared anyway");return 0;
 }
 if(!strcmp(case_name,"missing-fields")){fail_upload=1;bg_model_effects_bind(3);CHECK(!uniforms[1][0]&&!uniforms[1][1]&&bound[0]==50&&bound[1]==50,"upload failure used stale/black field");missing=1;bg_model_effects_bind(3);CHECK(texture_calls==2,"missing field was uploaded");return 0;}
 if(!strcmp(case_name,"invalid-volume")){flat=1;bg_model_effects_bind(3);CHECK(!uniforms[1][0]&&bound[0]==50,"2D field admitted");flat=0;cube=1;bg_model_effects_bind(3);CHECK(!uniforms[1][1]&&bound[1]==50,"cube field admitted");return 0;}
 if(!strcmp(case_name,"one-slot")){
  bg_model_effects_bind(1);CHECK(field_calls[0]==1&&!field_calls[1]&&uniforms[1][0]&&!uniforms[1][1],"queried unselected field");
  view=10;bg_model_effects_bind(2);CHECK(field_calls[0]==1&&field_calls[1]==1&&!uniforms[1][0]&&uniforms[1][1]&&bound[0]==50&&bound[1]==111&&uniforms[3][0]==11,"view/slot retained stale field");return 0;
 }
 if(strcmp(case_name,"pixel")&&strcmp(case_name,"vertex")&&strcmp(case_name,"pending-vertex")&&
    strcmp(case_name,"pending-pixel")&&strcmp(case_name,"failed-link")&&strcmp(case_name,"native-fallback"))return 2;
 struct vertex_shader_object object={0};struct nv2a_pixel_shader_key key={0};struct bg_shader *vertex,*pixel;
 unsigned int mask=3;object.instructions=&instruction;object.instruction_count=1;object.lighting.lights=2;
 enabled=strcmp(case_name,"vertex")!=0;pending_vertex=!strcmp(case_name,"pending-vertex");pending_pixel=!strcmp(case_name,"pending-pixel");failed_link=!strcmp(case_name,"failed-link");pending_all=!strcmp(case_name,"native-fallback");
 int fallback=pending_vertex||pending_pixel||failed_link||pending_all;
 struct bg_program *program=bg_model_program(&object,FALSE,&mask,&key,0,&vertex,&pixel);
 CHECK(program&&BGFX_HANDLE_IS_VALID(program->program)&&mask==(fallback?0u:3u),"effect pending/failure lost native model");
 CHECK(vertex->plasma==!fallback&&vertex->lit==(enabled&&!pending_all)&&key.plasma_model==(!fallback&&enabled),"wrong selected variants");
 if(mask){bg_model_effects_bind(mask);CHECK(uniforms[0][0]==1&&uniforms[0][1]==1&&uniforms[1][0]==1&&uniforms[1][1]==1&&bound[0]==100&&bound[1]==101,"native field handoff failed");}
 pending_vertex=pending_pixel=failed_link=pending_all=0;mask=3;
 program=bg_model_program(&object,FALSE,&mask,&key,0,&vertex,&pixel);CHECK(program&&mask==3&&vertex->plasma,"pending effect did not recover");
 int before=made;mask=3;bg_model_program(&object,FALSE,&mask,&key,0,&vertex,&pixel);CHECK(made==before,"recompiled per draw");
 mask=3;bg_model_program(&object,TRUE,&mask,&key,0,&vertex,&pixel);CHECK(object.bg_shader[enabled?6:4]&&object.bg_shader[enabled?7:5],"immediate variant alias");
 CHECK(texture_creates==1,"duplicated native volume uploads");return 0;
}
