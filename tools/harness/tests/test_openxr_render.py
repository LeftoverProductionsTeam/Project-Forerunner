"""Native bgfx eye-target/resolve lifecycle; GPU/runtime calls are fixtures."""
import sys
from pathlib import Path

import pytest

sys.path.insert(0, str(Path(__file__).resolve().parents[2]))
from harness import CHECK_FAILED, build, constant, enum_with, function, mutated, read, run, structure


def generated(fault=None):
    source = read('port/linux/src/d3d8_bgfx.inl')
    device = read('port/linux/src/d3d8_gl.c')
    header = read('port/linux/src/vr_openxr.h')
    types = '\n'.join(structure(source, name) for name in ['bg_target', 'bg_framebuffer', 'bg_openxr_image'])
    types += '\n' + '\n'.join(structure(header, name) for name in
                              ['vr_openxr_pose', 'vr_openxr_hand', 'vr_openxr_frame', 'vr_openxr_image'])
    types += '\n' + enum_with(header, '_vr_layer_none') + '\n' + enum_with(device, '_anti_aliasing_off,')
    for name in ['BG_MAXIMUM_VIEWS', 'BG_TARGET_BUCKETS']:
        types += '\n#define %s %d\n' % (name, constant(source, name))
    start = source.index('static struct\n{\n\tstruct bg_openxr_image *images')
    state = source[start:source.index('} bg_openxr;', start) + len('} bg_openxr;')]
    code = '\n'.join(function(source, name) for name in [
        'bg_view_set', 'bg_screen_samples', 'bg_samples_flags', 'bg_target_bucket', 'bg_target_get',
        'bg_framebuffer_get', 'bg_openxr_msaa_release', 'bg_openxr_msaa_target', 'bg_composite_copy',
        'xgpu_openxr_frame_begin', 'xgpu_openxr_eye_begin', 'xgpu_openxr_eye_end', 'bg_openxr_frame_submitted'])
    code += '\n' + function(read('port/linux/src/d3d8_shader_prep.inl'), 'prep_screen_samples')
    if fault:
        code = mutated(code, *fault)
    return (('types.inc', types), ('state.inc', state), ('under_test.inc', code))


@pytest.mark.parametrize('case', ['pipeline', 'formats', 'reuse-resize', 'off-menu', 'allocation-failure',
                                 'framebuffer-failure', 'resolve-failure', 'invalid-image', 'logical-view'])
def test_eye_lifecycle(case):
    status, output = run(build('openxr_render', generated()), case)
    assert status == 0, output


@pytest.mark.parametrize('fault,case', [
    (('samples = bg_openxr.draw_target ? bg_openxr.draw_target->samples : 0;', 'samples = 0;'), 'pipeline'),
    (('if (bg_openxr.draw_target != &bg_openxr.current->target)', 'if (FALSE)'), 'pipeline'),
    (('target->samples != samples || target->pixel_width != image->target.pixel_width ||',
      'TRUE || target->samples != samples || target->pixel_width != image->target.pixel_width ||'), 'reuse-resize'),
])
def test_negative_control(fault, case):
    status, output = run(build('openxr_render', generated(fault)), case)
    assert status == CHECK_FAILED, output
