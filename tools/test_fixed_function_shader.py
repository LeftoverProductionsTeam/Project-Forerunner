"""Compile the real loading-screen shader builder and its shared text writer.

This checks source generation, not GPU compilation or rendered gameplay.
"""
import os
from pathlib import Path
import re
import shutil
import subprocess

import pytest
from harness import constant, function, macro, structure

ROOT = Path(__file__).resolve().parents[1]
SRC = ROOT / "port/linux/src"


def build(work, gles=False, old_desktop=False):
    compiler = os.environ.get("CC") or shutil.which("clang") or shutil.which("cc")
    if not compiler:
        pytest.skip("a C compiler is required")
    vertex = (SRC / "nv2a_vsh.c").read_text()
    header = (SRC / "xgpu.h").read_text()
    start = vertex.index("static const char shader_prologue[] =")
    prologue = vertex[start:vertex.index(";\n", start) + 2]
    builder = function(vertex, "nv2a_fixed_function_glsl")
    if old_desktop:
        missing = '\n#else\n\txgpu_text_append(&text, "%s", XGPU_GL_VERSION_LINE);'
        assert builder.count(missing) == 1
        builder = builder.replace(missing, "", 1)
    code = r'''
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
typedef int BOOL;
static BOOL xgpu_reversed_depth;
#ifdef HALO_GLES
static struct {const char *shading_language;} xgpu_capabilities;
#else
const char *desktop_version;
#define XGPU_GL_VERSION_LINE desktop_version
#endif
'''
    code += "#define XGPU_VERTEX_ATTRIBUTE_COUNT %d\n" % constant(header, "XGPU_VERTEX_ATTRIBUTE_COUNT")
    code += macro(header, "NV2A_FINITE")
    code += structure(header, "xgpu_text") + "\n"
    code += function((SRC / "xgpu_text.c").read_text(), "xgpu_text_append") + "\n"
    code += prologue + "\n" + builder
    code += r'''
int main(int argc, char **argv)
{
    char expected[64], *source;
    if (argc != 3) return 2;
    snprintf(expected, sizeof(expected), "#version %s\n", argv[1]);
#ifdef HALO_GLES
    xgpu_capabilities.shading_language = argv[1];
#else
    desktop_version = expected;
#endif
    xgpu_reversed_depth = atoi(argv[2]);
    source = nv2a_fixed_function_glsl();
    if (!source || strncmp(source, expected, strlen(expected)))
    {
        fprintf(stderr, "loading-screen shader has no context version at its start\n");
        free(source);
        return 1;
    }
    puts(source);
    free(source);
    return 0;
}
'''
    work.mkdir()
    source = work / "fixed_shader.c"
    source.write_text(code)
    executable = work / ("fixed_shader.exe" if os.name == "nt" else "fixed_shader")
    flags = ["--target=i686-pc-windows-msvc", "-fuse-ld=lld", "-D_CRT_SECURE_NO_WARNINGS"] if os.name == "nt" else []
    if gles:
        flags += ["-DHALO_GLES", "-DHALO_GL_NO_CLIP_CONTROL"]
    result = subprocess.run([compiler, *flags, "-std=gnu89", "-Wall", "-Wextra", "-Werror",
                             str(source), "-o", str(executable)], capture_output=True, text=True)
    assert result.returncode == 0, result.stderr
    return executable


@pytest.fixture(scope="module")
def builders(tmp_path_factory):
    work = tmp_path_factory.mktemp("fixed-function-shader")
    return {"desktop": build(work / "desktop"), "gles": build(work / "gles", gles=True),
            "old": build(work / "old-desktop", old_desktop=True)}


@pytest.mark.parametrize("version", ["450 core", "410 core", "400 core", "330 core"])
@pytest.mark.parametrize("reversed_depth", [False, True])
def test_desktop_uses_context_version_first(builders, version, reversed_depth):
    result = subprocess.run([str(builders["desktop"]), version, str(int(reversed_depth))],
                            capture_output=True, text=True)
    assert result.returncode == 0, result.stderr
    assert result.stdout.count("#version") == 1
    assert "layout(location = 15) in vec4 v15_in;" in result.stdout
    assert "gl_Position = v0_in;" in result.stdout
    assert ("gl_Position.z = gl_Position.w - gl_Position.z;" in result.stdout) == reversed_depth


@pytest.mark.parametrize("version", ["300 es", "310 es"])
def test_gles_retains_its_context_version_and_precision(builders, version):
    result = subprocess.run([str(builders["gles"]), version, "0"], capture_output=True, text=True)
    assert result.returncode == 0, result.stderr
    assert result.stdout.count("#version") == 1
    assert result.stdout.startswith("#version " + version + "\nprecision highp float;\n")
    assert "gl_Position.y = -gl_Position.y;" in result.stdout


def test_missing_desktop_version_is_detected(builders):
    result = subprocess.run([str(builders["old"]), "450 core", "0"], capture_output=True, text=True)
    assert result.returncode == 1
    assert "no context version" in result.stderr


def build_selection(work, old_eager_mask=False):
    compiler = os.environ.get("CC") or shutil.which("clang") or shutil.which("cc")
    if not compiler:
        pytest.skip("a C compiler is required")
    device_source = (SRC / "d3d8_gl.c").read_text()
    header = (SRC / "xgpu.h").read_text()
    selection = function(device_source, "vertex_shader_get")
    if old_eager_mask:
        guarded = "immediate ? 0 : device.vertex_shader->packed_mask"
        assert selection.count(guarded) == 1
        selection = selection.replace(guarded, "device.vertex_shader->packed_mask", 1)
    code = r'''
#include <stdio.h>
#include <string.h>
typedef int BOOL;
typedef unsigned int DWORD;
typedef unsigned int GLuint;
#define FALSE 0
'''
    code += "#define XGPU_VERTEX_ATTRIBUTE_COUNT %d\n" % constant(header, "XGPU_VERTEX_ATTRIBUTE_COUNT")
    for source, name in ((device_source, "vertex_element"), (header, "nv2a_vertex_lighting"),
                         (device_source, "vertex_shader_object")):
        code += structure(source, name) + "\n"
    code += r'''
static struct {struct vertex_shader_object *vertex_shader;} device;
static struct vertex_shader_object *seen_program;
static BOOL seen_immediate, seen_lit, seen_plasma, seen_prepare;
static unsigned long seen_mask;
static GLuint vertex_shader_make(struct vertex_shader_object *program, BOOL immediate, BOOL lit, BOOL plasma,
    unsigned long mask, BOOL prepare)
{
    seen_program = program;
    seen_immediate = immediate;
    seen_lit = lit;
    seen_plasma = plasma;
    seen_mask = mask;
    seen_prepare = prepare;
    return 93;
}
'''
    code += selection + r'''
int main(int argc, char **argv)
{
    struct vertex_shader_object program = {0}, declaration = {0};
    BOOL immediate;
    GLuint shader;
    if (argc != 2) return 2;
    immediate = strcmp(argv[1], "streamed") != 0;
    /* A loaded program can use a different vertex declaration. */
    program.packed_mask = 0x21;
    declaration.packed_mask = 0x9249;
    device.vertex_shader = !strcmp(argv[1], "fixed") ? NULL : &declaration;
    shader = vertex_shader_get(&program, immediate, 1, 1);
    if (shader != 93 || seen_program != &program || seen_immediate != immediate ||
        !seen_lit || !seen_plasma || seen_prepare) return 1;
    if (seen_mask != (immediate ? 0 : declaration.packed_mask))
    {
        fprintf(stderr, "incorrect declaration mask for draw kind\n");
        return 1;
    }
    return 0;
}
'''
    work.mkdir()
    source = work / "selection.c"
    source.write_text(code)
    executable = work / ("selection.exe" if os.name == "nt" else "selection")
    flags = ["--target=i686-pc-windows-msvc", "-fuse-ld=lld", "-D_CRT_SECURE_NO_WARNINGS"] if os.name == "nt" else []
    result = subprocess.run([compiler, *flags, "-std=gnu89", "-Wall", "-Wextra", "-Werror",
                             str(source), "-o", str(executable)], capture_output=True, text=True)
    assert result.returncode == 0, result.stderr
    return executable


@pytest.fixture(scope="module")
def selection_tools(tmp_path_factory):
    work = tmp_path_factory.mktemp("fixed-function-selection")
    return {"fixed": build_selection(work / "fixed"), "old": build_selection(work / "old", old_eager_mask=True)}


@pytest.mark.parametrize("kind", ["fixed", "immediate", "streamed"])
def test_shader_selection_respects_draw_declaration(selection_tools, kind):
    result = subprocess.run([str(selection_tools["fixed"]), kind], capture_output=True, text=True)
    assert result.returncode == 0, result.stderr


def test_eager_immediate_declaration_mask_is_detected(selection_tools):
    # A valid declaration makes the old-code negative control fail its
    # semantic check without deliberately crashing a test executable.
    result = subprocess.run([str(selection_tools["old"]), "immediate"], capture_output=True, text=True)
    assert result.returncode == 1
    assert "incorrect declaration mask" in result.stderr


def bgfx_lighting_source(fault=None):
    """Real shader selection, model-light emitter and backend GLSL rewriting.

    Only asynchronous compiler readiness and the GPU link are stood in for.
    This cannot establish that a real driver compiles or renders the result.
    """
    device = (SRC / "d3d8_gl.c").read_text()
    backend = (SRC / "d3d8_bgfx.inl").read_text()
    if fault:
        before, after = {
            "lighting-disabled": ("&& per_pixel_lighting() ?", "&& FALSE ?"),
            "no-fallback": ("if ((!entry || !BGFX_HANDLE_IS_VALID(entry->program)) && key->per_pixel_lighting)",
                            "if (FALSE)"),
        }[fault]
        assert backend.count(before) == 1
        backend = backend.replace(before, after)
    header = (SRC / "xgpu.h").read_text()
    code = r'''
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
typedef int BOOL;
typedef unsigned int DWORD, GLuint;
#define TRUE 1
#define FALSE 0
#define HALO_BGFX
#define BGFX_HANDLE_IS_VALID(h) ((h) != -1)
#define CHECK(x) do {if (!(x)) {fprintf(stderr,"%d: %s\n",__LINE__,#x);exit(1);}} while (0)
'''
    for name in ("XGPU_MODEL_LIGHT_COUNT", "XGPU_VERTEX_ATTRIBUTE_COUNT", "XGPU_VERTEX_CONSTANT_COUNT",
                 "XGPU_VERTEX_CONSTANT_BIAS"):
        code += "#define %s %d\n" % (name, constant(header, name))
    code += structure(header, "xgpu_text") + "\n"
    code += function((SRC / "xgpu_text.c").read_text(), "xgpu_text_append") + "\n"
    for source, name in ((device, "vertex_element"), (header, "nv2a_vertex_lighting"),
                         (device, "vertex_shader_object")):
        code += structure(source, name) + "\n"
    code += r'''
static void plasma_model_emit_pixel(struct xgpu_text *text) {(void)text; abort();}
'''
    code += function((SRC / "nv2a_psh.c").read_text(), "model_lighting") + "\n"
    # Preserve the renderer's actual binding locations and backend rewrite.
    for name in ("bg_varyings", "bg_varying_types", "bg_attribute_names"):
        start = backend.index("static const char *const " + name + "[")
        code += backend[start:backend.index("};", start) + 2] + "\n"
    code += r'''
static BOOL bg_vulkan, bg_d3d11;
enum {_bg_shader_depth_bias=1};
'''
    for name in ("bg_replace_all", "bg_constants_used", "bg_vulkan_glsl"):
        code += function(backend, name) + "\n"
    post = (SRC / "xgpu_post.c").read_text()
    start = post.index("static const char fxaa_source[] =")
    code += post[start:post.index(";\n", start) + 2] + "\n" + function(post, "xgpu_post_fxaa_glsl") + "\n"
    code += r'''
struct bg_shader {int lit, ready;};
struct bg_program {int program;};
struct nv2a_pixel_shader_key {unsigned char per_pixel_lighting;};
static struct bg_shader vertices[4], pixels[3]={{0,1},{1,1},{1,1}};
static int made, enabled=1, pending_vertex, pending_pixel, failed_link, flags_seen;
static BOOL per_pixel_lighting(void) {return enabled;}
static char *nv2a_vertex_shader_to_glsl(const DWORD *instructions, unsigned long count,
    unsigned long packed, struct nv2a_vertex_lighting const *lighting, BOOL plasma)
{
    CHECK(!packed && !plasma && count==1 && *instructions==42);
    CHECK(!lighting || lighting->lights==1 || lighting->lights==2);
    return _strdup(lighting ? "lit" : "stock");
}
static struct bg_shader *bg_shader_get(const char *source, BOOL fragment, int flags,
    const char *what, BOOL wait)
{
    struct bg_shader *shader=&vertices[made++];
    CHECK(made<=4 && !fragment && !flags && !wait && !strcmp(what,"vertex"));
    shader->lit=!strcmp(source,"lit");shader->ready=1;
    return shader;
}
static BOOL bg_shader_ready(struct bg_shader *shader)
{return shader->ready && !(pending_vertex && shader->lit);}
'''
    code += function(backend, "bg_vertex_shader") + "\n"
    code += r'''
static struct bg_shader *bg_fragment_shader(struct nv2a_pixel_shader_key const *key, int flags)
{
    flags_seen=flags;
    return pending_pixel && key->per_pixel_lighting ? NULL : &pixels[key->per_pixel_lighting];
}
static struct bg_program *bg_program_entry(struct bg_shader *vertex,struct bg_shader *pixel)
{
    static struct bg_program entries[2]={{0},{1}}, broken={-1};
    CHECK(vertex->lit==pixel->lit);
    return failed_link && vertex->lit ? &broken : &entries[vertex->lit];
}
'''
    code += function(backend, "bg_model_program") + "\n"
    code += structure(device, "draw_uniforms") + r'''
static struct {
    float constants[XGPU_VERTEX_CONSTANT_COUNT][4], viewport_scale[4], viewport_offset[4];
    struct {float MinZ,MaxZ;} viewport;
} device;
static struct draw_uniforms bg_uniforms;
static float bg_depth_range[4], bg_depth_bias[4], bg_model_lights[XGPU_MODEL_LIGHT_COUNT][4];
'''
    start = backend.index("static const struct\n{\n\tconst char *name;\n\tconst float *source;")
    code += backend[start:backend.index("};", start) + 2] + "\n" + macro(backend, "BG_UNIFORM_COUNT") + "\n"
    upload = function(backend, "bg_uniforms_update")
    states = sorted(set(re.findall(r'\bD3D(?:RS|TSS)_\w+', upload)) - {"D3DTSS_MAXSTAGES"})
    code += "enum {" + ",".join(states) + "};\n"
    code += r'''
#define D3DTSS_MAXSTAGES 4
#define UI_OFFSET 0
static DWORD D3D__RenderState[64],D3D__TextureState[4][64];
static struct {float target_scale[2];int uniforms[BG_UNIFORM_COUNT];} bg;
static int light_uploads;
static float uploaded_lights[XGPU_MODEL_LIGHT_COUNT][4];
static float dword_to_float(DWORD value) {float result;memcpy(&result,&value,sizeof(result));return result;}
static void color_to_vec4(DWORD value,float result[4]) {(void)value;memset(result,0,4*sizeof(float));}
static void bgfx_set_uniform(int handle,const void *data,int count) {
    if (handle==99) {CHECK(count==XGPU_MODEL_LIGHT_COUNT);memcpy(uploaded_lights,data,sizeof(uploaded_lights));light_uploads++;}
}
'''
    code += function(device, "model_light_register") + "\n" + upload + r'''
static void verify_light_upload(void) {
    float scale[4][4]={{0}};
    unsigned long i;int light,component;
    for (i=0;i<BG_UNIFORM_COUNT;i++) if (!strcmp(bg_uniform_sources[i].name,"model_lights")) bg.uniforms[i]=99;
    for (i=0;i<XGPU_VERTEX_CONSTANT_COUNT;i++) for(component=0;component<4;component++) device.constants[i][component]=(float)(i*4+component+1);
    bg_uniforms_update(scale,TRUE);CHECK(light_uploads==1);
    for(light=0;light<XGPU_MODEL_LIGHT_COUNT;light++) for(component=0;component<4;component++)
        CHECK(uploaded_lights[light][component]==device.constants[XGPU_VERTEX_CONSTANT_BIAS+(light ? -80+light : -82)][component]);
    bg_uniforms_update(scale,FALSE);CHECK(light_uploads==1);
}
'''
    code += r'''
int main(int argc,char **argv)
{
    DWORD instruction=42;
    struct vertex_shader_object object={0};
    struct nv2a_pixel_shader_key key={0};
    struct bg_shader *vertex,*pixel;
    struct bg_program *entry;
    struct xgpu_text text={0};
    char *rewritten;
    unsigned long size;
    int expected=1, immediate=0;
    CHECK(argc==2);
    object.instructions=&instruction;object.instruction_count=1;object.lighting.lights=2;
    if (!strcmp(argv[1],"classic")) {enabled=0;expected=0;}
    else if (!strcmp(argv[1],"distant")) object.lighting.lights=1;
    else if (!strcmp(argv[1],"unrecognized")) {object.lighting.lights=0;expected=0;}
    else if (!strcmp(argv[1],"pending-vertex")) {pending_vertex=1;expected=0;}
    else if (!strcmp(argv[1],"pending-pixel")) {pending_pixel=1;expected=0;}
    else if (!strcmp(argv[1],"failed-link")) {failed_link=1;expected=0;}
    else if (!strcmp(argv[1],"immediate")) immediate=1;
    entry=bg_model_program(&object,immediate,&key,1,&vertex,&pixel);
    CHECK(entry && entry->program==expected && vertex->lit==expected && flags_seen==1);
    CHECK(key.per_pixel_lighting==(expected ? object.lighting.lights : 0));
    pending_vertex=pending_pixel=failed_link=0;
    entry=bg_model_program(&object,immediate,&key,1,&vertex,&pixel);
    CHECK(entry && entry->program==(enabled && object.lighting.lights ? 1 : 0));
    /* The same streamed/immediate variant is cached; each draw kind owns
       its own slot. Failed enhanced links must not poison the stock one. */
    {int before=made;bg_model_program(&object,immediate,&key,1,&vertex,&pixel);CHECK(made==before);}
    if (!strcmp(argv[1],"immediate")) {
        struct bg_shader *other=bg_vertex_shader(&object,FALSE,TRUE);
        CHECK(other && other->lit && object.bg_shader[2] && object.bg_shader[3]);
    }
    model_lighting(&text,object.lighting.lights==2,FALSE);
    bg_vulkan=strcmp(argv[1],"metal") && strcmp(argv[1],"d3d11");
    bg_d3d11=!strcmp(argv[1],"d3d11");
    rewritten=bg_vulkan_glsl(text.buffer,TRUE,0,&size);
    CHECK(rewritten && strstr(rewritten,"vec4 model_lights[12];"));
    CHECK(strstr(rewritten,"layout(location = 9) in vec4 xWorldNormal;"));
    CHECK((strstr(rewritten,"layout(location = 10) in vec3 xWorldPosition;")!=NULL)==
        (object.lighting.lights==2 || bg_d3d11));
    CHECK(strstr(rewritten,"model_lights[11].xyz") && size>=12*16);
    CHECK(!strstr(rewritten,"plasma_model_slots"));
    free(rewritten);
    rewritten=bg_vulkan_glsl(xgpu_post_fxaa_glsl(),TRUE,0,&size);
    CHECK(rewritten && strstr(rewritten,"vec4 metrics;") && strstr(rewritten,"vec4 bounds;"));
    CHECK(strstr(rewritten,"gl_FragCoord.xy * metrics.xy") && strstr(rewritten,"clamp(position, bounds.xy, bounds.zw)"));
    CHECK(strstr(rewritten,bg_vulkan ? "binding = 2) uniform texture2D color_texture_texture" : "binding = 0) uniform sampler2D color_texture"));
    CHECK(size>=32);
    free(rewritten);free(text.buffer);verify_light_upload();
    return 0;
}
'''
    # strdup is the POSIX spelling on the Unix test compiler.
    return code.replace("_strdup", "strdup") if os.name != "nt" else code


@pytest.mark.parametrize("fault,case", [(None, None), ("lighting-disabled", "point"), ("no-fallback", "pending-pixel")])
def test_bgfx_model_lighting(tmp_path, fault, case):
    compiler = os.environ.get("CC") or shutil.which("clang") or shutil.which("cc")
    if not compiler:
        pytest.skip("a C compiler is required")
    source = tmp_path / "bgfx-lighting.c"
    source.write_text(bgfx_lighting_source(fault))
    binary = tmp_path / ("bgfx-lighting.exe" if os.name == "nt" else "bgfx-lighting")
    target = ["--target=i686-pc-windows-msvc", "-fuse-ld=lld", "-D_CRT_SECURE_NO_WARNINGS"] if os.name == "nt" else []
    result = subprocess.run([compiler, *target, "-std=gnu89", "-Wall", "-Wextra", "-Werror",
                             str(source), "-o", str(binary)], capture_output=True, text=True)
    assert result.returncode == 0, result.stderr
    for case in ([case] if fault else ("classic", "point", "distant", "unrecognized", "pending-vertex", "pending-pixel",
                                      "failed-link", "immediate", "metal", "d3d11")):
        result = subprocess.run([str(binary), case], capture_output=True, text=True)
        assert result.returncode == (1 if fault else 0), (case, result.stderr)
