"""Compile the real loading-screen shader builder and its shared text writer.

This checks source generation, not GPU compilation or rendered gameplay.
"""
import os
from pathlib import Path
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
