"""Keep fast multiplayer hosting playable before the user changes rules."""

from pathlib import Path

from harness import function
from test_hud_button_style import run_c


ROOT = Path(__file__).resolve().parents[1]
PLAYER_UI = ROOT / "source/interface/player_ui.c"


def function_body(source, signature):
    start = source.index(signature)
    opening = source.index("{", start)
    depth = 0
    for index in range(opening, len(source)):
        if source[index] == "{":
            depth += 1
        elif source[index] == "}":
            depth -= 1
            if depth == 0:
                return source[opening + 1:index]
    raise AssertionError(f"unterminated function: {signature}")


def test_fast_setup_seeds_slayer_before_playlist_and_server_creation():
    source = PLAYER_UI.read_text(encoding="ascii")
    body = function_body(source, "void player_ui_fast_setup_network_server(")

    clear = body.index("player_ui_globals.multiplayer_variant_specified = FALSE;")
    build = body.index(
        'game_engine_get_variant_by_name(&default_variant, "slayer")')
    select = body.index("player_ui_set_game_variant(")
    playlist = body.index("game_engine_playlist_initialize();")
    create = body.index("create_global_network_game_server()")

    # playlist_initialize snapshots the selected variant into global_stage;
    # server creation then copies that stage into its authoritative game.
    assert clear < build < select < playlist < create


def test_playlist_uses_installed_default_without_replacing_explicit_choices(tmp_path):
    source = (ROOT / 'source/game/game_engine.c').read_text()
    code = r'''
#include <assert.h>
#include <stddef.h>
#include <string.h>
#define csstrcpy strcpy
#define csstrncpy strncpy
#define csmemcpy memcpy
struct game_variant { int value; };
static struct { char map_name[128]; struct game_variant variant; } global_stage;
static int dedicated, carousel_present, beaver_present, reads;
static char *selected;
static char const *rotation;
static char const *const maps[] = { "missing@ce", "levels\\test\\beavercreek\\beavercreek" };
static int halo_server_mode(void) { return dedicated; }
static char const *server_director_first_level(void) { return rotation; }
static char *main_get_multiplayer_map_name(void) { return selected; }
static int player_ui_game_variant_specified(struct game_variant *v) { v->value=7; return 1; }
static int cache_files_map_installed(char const *name)
{
    if (!strcmp(name, "levels\\test\\carousel\\carousel")) return carousel_present;
    return beaver_present && !strcmp(name, maps[1]);
}
static short ui_widget_port_multiplayer_maps(char const *const **names,short *last)
{ reads++; *names=maps; return 2; }
'''
    code += function(source, 'game_engine_playlist_next')
    code += r'''
int main(void)
{
    /* A normal full install keeps the classic default and avoids scanning. */
    carousel_present=1;
    game_engine_playlist_next(0,0,2);
    assert(!strcmp(global_stage.map_name,"levels\\test\\carousel\\carousel") && !reads);
    /* Missing/unready rows are skipped, using the same native menu list. */
    carousel_present=0; beaver_present=1;
    game_engine_playlist_next(0,0,2);
    assert(!strcmp(global_stage.map_name,maps[1]) && reads==1);
    assert(global_stage.variant.value==7);
    /* Explicit choices, even missing ones, retain ordinary refusal behavior. */
    selected="missing@ce";
    game_engine_playlist_next(0,0,2);
    assert(!strcmp(global_stage.map_name,selected) && reads==1);
    selected=NULL; dedicated=1; rotation="rotation@ce";
    game_engine_playlist_next(0,0,2);
    assert(!strcmp(global_stage.map_name,rotation) && reads==1);
    /* No installed map is not manufactured into a successful selection. */
    dedicated=0; rotation=NULL; beaver_present=0;
    game_engine_playlist_next(0,0,2);
    assert(!strcmp(global_stage.map_name,"levels\\test\\carousel\\carousel") && reads==2);
    return 0;
}
'''
    run_c(tmp_path, code)
