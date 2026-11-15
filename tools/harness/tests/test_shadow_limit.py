"""Faber shadow budget in the real native helper and map lifecycle."""
import pytest
from harness import CHECK_FAILED, build, function, mutated, read, run

def generated(fault=None):
    source = read("source/render/render_objects.c")
    start = source.index("#define PORT_SHADOW_CANDIDATE_LIMIT")
    end = source.index("#define OBJECT_LIGHTING_MAXIMUM_COLOR_DELTA", start)
    code = "#define OBJECT_SHADOW_MINIMUM_PIXELS 30.f\n" + source[start:end]
    code += function(source, "render_objects_initialize_for_new_map")
    if fault:
        code = mutated(code, *fault)
    return (("under_test.inc", code),)

@pytest.mark.parametrize("case", ["stock", "rank", "stereo", "windows", "bounds", "reset"])
def test_shadow_budget(case):
    status, output = run(build("shadow_limit", generated()), case)
    assert status == 0, output

def test_stereo_negative_control():
    status, output = run(build("shadow_limit", generated((
        "if (port_shadow_windows[window].frame == render.frame_index)", "if (FALSE)"))), "stereo")
    assert status == CHECK_FAILED, output


def test_generated_shadow_control_matches_runtime():
    from tools import port_settings
    from harness import ROOT
    name = "main_menu.settings_select.player_setup.player_profile_edit.video_settings.xml"
    generated = "\n".join(port_settings.settings_files()[name])
    actual = (ROOT / "port/assets/menus/ce" / name).read_text()
    assert generated == actual
    assert 'setting="display.shadow_limit" values="0|8|16|32|64|128|256"' in actual
    pages = (ROOT / "port/assets/menus/chupathingy/settings_pages.xml").read_text()
    assert "video_settings/op_shadow_limit" in pages
    config = read("port/linux/src/port_config.c")
    assert '{ "display.shadow_limit", _config_integer, "0", "HALO_SHADOW_LIMIT"' in config
