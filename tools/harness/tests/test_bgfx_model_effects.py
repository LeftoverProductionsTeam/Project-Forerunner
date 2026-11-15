"""Inherited model lighting, native visibility bindings and backend fallbacks."""
import sys
from pathlib import Path

import pytest

sys.path.insert(0, str(Path(__file__).resolve().parents[2]))
from harness import CHECK_FAILED, build, enum_with, function, mutated, read, run, structure


def generated(fault=None):
    source = read('port/linux/src/d3d8_bgfx.inl')
    code = '\n'.join(function(source, name) for name in [
        'bg_vertex_shader', 'bg_model_program', 'bg_model_effects_supported',
        'bg_model_effects_start', 'bg_model_effects_bind'])
    for name in ['bg_varyings', 'bg_varying_types', 'bg_attribute_names']:
        start = source.index('static const char *const ' + name + '[')
        code += '\n' + source[start:source.index('};', start) + 2]
    header = read('port/linux/src/xgpu.h')
    types = structure(header, 'xgpu_text')
    prep = read('port/linux/src/d3d8_shader_prep.inl')
    types += '\n' + structure(prep, 'prep_pending')
    types += '\n' + enum_with(read('port/linux/src/xgpu_shader_keys.h'), '_xgpu_key_lit_bit')
    code += '\n' + function(read('port/linux/src/xgpu_text.c'), 'xgpu_text_append')
    for name in ['bg_replace_all', 'bg_constants_used', 'bg_vulkan_glsl']:
        code += '\n' + function(source, name)
    effects = read('port/linux/src/plasma_model_shader.c')
    for name in ['plasma_model_emit_uniform', 'plasma_model_emit_pixel']:
        code += '\n' + function(effects, name)
    code += '\n' + function(read('port/linux/src/nv2a_psh.c'), 'model_lighting')
    for name in ['prep_start', 'prep_collect']:
        code += '\n' + function(prep, name)
    if fault:
        code = mutated(code, *fault)
    return (('types.inc', types), ('under_test.inc', code))


@pytest.mark.parametrize('case', ['pixel', 'vertex', 'pending-vertex', 'pending-pixel', 'failed-link',
                                 'native-fallback', 'capabilities', 'startup-failure', 'missing-fields',
                                 'one-slot', 'invalid-volume', 'shader-vulkan', 'shader-metal', 'shader-d3d11',
                                 'preparation'])
def test_model_effects(case):
    status, output = run(build('bgfx_model_effects', generated()), case)
    assert status == 0, output


@pytest.mark.parametrize('fault,case', [
    (('if (!strcmp(name, "plasma_occ_texture0")) binding = 6;',
      'if (!strcmp(name, "plasma_occ_texture0")) binding = 4;'), 'shader-vulkan'),
    (('description.depth > 1 && !description.cube_map && BGFX_HANDLE_IS_VALID(resolved)',
      'BGFX_HANDLE_IS_VALID(resolved)'), 'invalid-volume'),
    (('*plasma_mask = 0;', '*plasma_mask = *plasma_mask;'), 'pending-pixel'),
])
def test_broken_binding_or_fallback_is_detected(fault, case):
    status, output = run(build('bgfx_model_effects', generated(fault)), case)
    assert status == CHECK_FAILED, output


def test_draw_and_preparation_connections():
    source = read('port/linux/src/d3d8_bgfx.inl')
    draw = function(source, 'bg_draw')
    assert 'bg_model_effects_supported()' in draw and 'plasma_model_lighting_mask()' in draw
    assert 'key.per_pixel_lighting != 0, plasma_mask != 0,' in draw
    assert draw.index('bg_apply_textures();') < draw.index('bg_model_effects_bind(plasma_mask)')
    assert draw.index('bg_model_effects_bind(plasma_mask)') < draw.index('bgfx_submit(')
    prep = read('port/linux/src/d3d8_shader_prep.inl')
    assert 'pending.plasma = plasma;' in function(prep, 'prep_start')
    assert 'pending->key.per_pixel_lighting != 0, pending->plasma' in function(prep, 'prep_collect')
