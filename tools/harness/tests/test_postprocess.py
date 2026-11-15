"""Real bgfx FXAA pass/rectangle/lifetime with a bounded GPU API fixture.

These checks do not compile GPU shaders or prove rendered/stereo appearance.
"""
import sys
from pathlib import Path

import pytest

sys.path.insert(0, str(Path(__file__).resolve().parents[2]))
from harness import CHECK_FAILED, build, constant, enum_with, function, mutated, read, run, structure


def generated(fault=None):
    backend = read('port/linux/src/d3d8_bgfx.inl')
    device = read('port/linux/src/d3d8_gl.c')
    post = read('port/linux/src/xgpu_post.c')
    code = '\n'.join(function(backend, name) for name in (
        'bg_post_release', 'bg_fxaa_rectangle', 'bg_view_set', 'bg_composite_copy', 'bg_screen_anti_alias'))
    if fault:
        code = mutated(code, *fault)
    shader = ''
    for source, name in ((post, 'fxaa_source'), (backend, 'bg_blit_vertex_source')):
        start = source.index('static const char ' + name + '[] =')
        shader += source[start:source.index(';\n', start) + 2] + '\n'
    shader += function(post, 'xgpu_post_fxaa_glsl') + '\n' + function(device, 'scaled_pixel')
    types = (structure(backend, 'bg_target') + '\n' + enum_with(device, '_anti_aliasing_off,') + '\n' +
             '#define BG_MAXIMUM_VIEWS %d\n' % constant(backend, 'BG_MAXIMUM_VIEWS'))
    return (('types.inc', types), ('shader.inc', shader), ('under_test.inc', code))


@pytest.mark.parametrize('case', ['rectangle', 'split', 'reuse-resize', 'off', 'shader-pending',
                                 'failed-link', 'allocation-failure', 'framebuffer-failure', 'limits'])
def test_fxaa_pass(case):
    status, output = run(build('postprocess', generated()), case)
    assert status == 0, output


@pytest.mark.parametrize('fault,case', [
    (('BGFX_STATE_WRITE_RGB | BGFX_STATE_PT_TRISTRIP, 0);',
      'BGFX_STATE_WRITE_RGB | BGFX_STATE_WRITE_A | BGFX_STATE_PT_TRISTRIP, 0);'), 'split'),
    (('bg_post_release();\n\t\tbg.post_width = target->pixel_width;',
      'bg.post_width = target->pixel_width;'), 'reuse-resize'),
    (('bounds[axis] = ((float)corners[axis] + 0.5f) / (float)size;', 'bounds[axis] = 0.0f;'), 'rectangle'),
])
def test_negative_control(fault, case):
    status, output = run(build('postprocess', generated(fault)), case)
    assert status == CHECK_FAILED, output


def test_native_hook_and_shared_shader():
    device = read('port/linux/src/d3d8_gl.c')
    hook = function(device, 'halo_screen_anti_alias')
    assert hook.index('bg_screen_anti_alias(') < hook.index('if (!device.gl_ready')
    post = function(read('port/linux/src/xgpu_post.c'), 'program_build')
    assert 'xgpu_post_fxaa_glsl()' in post
