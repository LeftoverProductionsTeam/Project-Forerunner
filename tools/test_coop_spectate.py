"""Network co-op spectating (port/linux/game/coop_spectate.c): a dead player
sees their own body for the first 3 seconds of each death, as the retail
dead camera shows it, before watching a teammate. The timing helper is
compiled from the source with the build machine's C compiler."""
import os
import re
import shutil
import subprocess
import tempfile
from pathlib import Path

import pytest
from test_hud_button_style import run_c

ROOT = Path(__file__).resolve().parent.parent
SPECTATE_C = ROOT / "port/linux/game/coop_spectate.c"


def _block(text, start_marker):
    start = text.index(start_marker)
    end = text.index("\n}", start) + 3
    return text[start:end]


PRELUDE = """
#include <assert.h>
#include <stdio.h>
typedef int boolean;
#define TRUE 1
#define FALSE 0
#define NONE (-1)
#define TICKS_PER_SECOND 30
"""

MAIN = r"""
int main(void)
{
	struct coop_spectate_death death = { NONE, NONE };
	long tick;

	/* no body yet (still joining): spectate at once */
	assert(!showing_own_body(&death, NONE, 0));
	/* a death at tick 100: the body for 90 ticks, then a teammate */
	for (tick = 100; tick < 190; tick++)
		assert(showing_own_body(&death, 5, tick));
	assert(!showing_own_body(&death, 5, 190));
	assert(!showing_own_body(&death, 5, 5000));
	/* the next death has a new body: the body again */
	assert(showing_own_body(&death, 9, 6000));
	assert(showing_own_body(&death, 9, 6089));
	assert(!showing_own_body(&death, 9, 6090));
	/* a new game's clock (behind the start) is a new death */
	assert(showing_own_body(&death, 9, 10));
	assert(!showing_own_body(&death, 9, 100));
	puts("PASS");
	return 0;
}
"""


def _unit():
    text = SPECTATE_C.read_text()
    ticks = re.search(r"#define SPECTATE_DEATH_TICKS .*", text).group(0)
    return "\n".join([
        PRELUDE,
        ticks,
        _block(text, "struct coop_spectate_death\n{"),
        _block(text, "static boolean showing_own_body("),
        MAIN,
    ])


def test_own_body_first():
    cc = shutil.which(os.environ.get("CC", "cc")) or shutil.which("clang") or shutil.which("gcc")
    if not cc:
        pytest.skip("no C compiler")
    with tempfile.TemporaryDirectory(prefix="halo-coop-spectate-") as temp:
        source = Path(temp) / "spectate.c"
        exe = Path(temp) / "spectate"
        source.write_text(_unit())
        subprocess.run([cc, "-std=c11", "-Wall", "-Werror", str(source), "-o", str(exe)], check=True)
        result = subprocess.run([str(exe)], check=True, capture_output=True, text=True)
        assert "PASS" in result.stdout


def _spectator_unit(fault=None):
    text = SPECTATE_C.read_text()
    names = ('static long spectate_player_unit(', 'static long next_living_player(',
             'static boolean next_pressed(', 'static boolean showing_own_body(',
             'long coop_spectate_unit(', 'boolean coop_spectate_nothing_to_watch(',
             'void coop_spectate_draw(')
    code = _block((ROOT / 'source/game/game_allegiance.c').read_text(),
                  'boolean game_solo_teams_are_friendly(') + '\n'
    code += '\n'.join(_block(text, name) for name in names)
    if fault == 'wrap':
        code = code.replace('return first;', 'return first == after ? NONE : first;')
    elif fault == 'departure':
        code = code.replace('player->quit_out_of_game || ', '')
    elif fault == 'ownership':
        code = code.replace('unit->unit.player_index == player_index &&', 'TRUE &&')
    elif fault == 'faction':
        code = code.replace('game_solo_teams_are_friendly(viewer->team_index, unit->object.owner_team_index) &&', 'TRUE &&')
    elif fault == 'host-fallback':
        code = code.replace('if (!game_solo_teams_are_friendly(player->team_index, team))', 'if (FALSE)')
    elif fault == 'mutual':
        code = code.replace(' && !game_team_is_enemy((short)second, (short)first)', '')
    elif fault == 'team-wrap':
        code = code.replace('game_solo_teams_are_friendly(long first, long second)', 'game_solo_teams_are_friendly(short first, short second)')
    return PRELUDE + r'''
#include <string.h>
#include <wchar.h>
#include <stdarg.h>
#define MAXIMUM_LOCAL_PLAYERS 4
#define MAXIMUM_GAMEPADS 4
#define FIRST_GAMEPAD_ANALOG_BUTTON 0
#define _gamepad_analog_button_a 0
#define _object_mask_biped 1
#define _object_dead_bit 2
#define TEST_FLAG(value,bit) ((value)&(1UL<<(bit)))
#define NUMBEROF(value) (sizeof(value)/sizeof((value)[0]))
#define SPECTATE_DEATH_TICKS (3*TICKS_PER_SECOND)
#define NUMBER_OF_SOLO_CAMPAIGN_TEAMS 10
struct player_datum {long unit_index,dead_unit_index,team_index;boolean quit_out_of_game;
 struct {short controller_index;} network_player_data;wchar_t name[12];};
struct unit_datum {struct {long damage_flags;short owner_team_index;} object;struct {long player_index;} unit;};
struct data_iterator {short next;long datum_index;};
struct gamepad_state {unsigned char buttons[16];};
struct coop_spectate_death {long dead_unit_index,dead_since;};
static struct player_datum players[3];static struct unit_datum units[3];
static long handles[3]={0x10000,0x10001,0x10002},bodies[3]={0x10010,0x10011,0x10012};
static long locals[4]={0x10000,NONE,NONE,NONE},tick=100;
static long coop_spectate_watched[4]={NONE,NONE,NONE,NONE};
static struct coop_spectate_death coop_spectate_deaths[4];
static void *player_data;static struct gamepad_state pad[4];static unsigned char jump[4];
static int draws,joining,waiting,labels;static wchar_t named[12];
static boolean betrayed;
static boolean game_team_is_enemy(short viewer,short target) {
 assert(viewer>=0&&viewer<10&&target>=0&&target<10);
 return (viewer==3)!=(target==3) || (betrayed&&viewer==2&&target==1);
}
static struct player_datum *player_try_and_get(long h) {int i;for(i=0;i<3;i++)if(handles[i]==h)return &players[i];return NULL;}
static struct player_datum *player_get(long h) {struct player_datum *p=player_try_and_get(h);assert(p);return p;}
static long local_player_get_player_index(short local) {assert(local>=0&&local<4);return locals[local];}
static void *object_try_and_get_and_verify_type(long h,long mask) {int i;assert(mask==_object_mask_biped);for(i=0;i<3;i++)if(bodies[i]==h)return &units[i];return NULL;}
static void data_iterator_new(struct data_iterator *it,void *data) {(void)data;it->next=0;}
static void *data_iterator_next(struct data_iterator *it) {if(it->next>=3)return NULL;it->datum_index=handles[it->next];return &players[it->next++];}
static struct gamepad_state const *input_get_gamepad_state(short controller) {return &pad[controller];}
static unsigned char input_abstraction_port_accept(short controller) {return jump[controller];}
static long game_time_get(void) {return tick;}
static boolean players_coop_waiting_to_start(long player) {(void)player;return FALSE;}
static void draw_bottom_text(wchar_t const *text) {
 draws++;if(!wcscmp(text,L"JOINING THE GAME"))joining++;
 if(!wcscmp(text,L"WAITING FOR A TEAMMATE"))waiting++;
}
static int usnprintf(wchar_t *out,size_t count,wchar_t const *format,...) {
 va_list args;wchar_t const *name;va_start(args,format);name=va_arg(args,wchar_t const *);
 wcsncpy(named,name,11);named[11]=0;va_end(args);labels++;wcsncpy(out,L"SPECTATING",count);return 0;
}
''' + code + r'''
int main(void) {
 int i;
 for(i=0;i<3;i++) {
  players[i].unit_index=bodies[i];players[i].dead_unit_index=NONE;units[i].unit.player_index=handles[i];
  players[i].team_index=1;units[i].object.owner_team_index=1;
  players[i].network_player_data.controller_index=(short)i;players[i].name[0]=(wchar_t)('A'+i);
 }
 players[0].unit_index=NONE;players[2].unit_index=NONE;
 /* The sole survivor remains selected, including each new next press. */
 assert(coop_spectate_unit(0)==bodies[1]);
 pad[0].buttons[0]=1;assert(coop_spectate_unit(0)==bodies[1]);
 assert(coop_spectate_unit(0)==bodies[1]);pad[0].buttons[0]=0;coop_spectate_unit(0);
 /* The next key is edge-triggered and wraps through two survivors. */
 players[2].unit_index=bodies[2];jump[0]=1;assert(coop_spectate_unit(0)==bodies[2]);
 assert(coop_spectate_unit(0)==bodies[2]);jump[0]=0;coop_spectate_unit(0);
 jump[0]=1;assert(coop_spectate_unit(0)==bodies[1]);jump[0]=0;coop_spectate_unit(0);
 /* Departure, a stale body, death and changed ownership retarget immediately. */
 players[1].quit_out_of_game=TRUE;assert(coop_spectate_unit(0)==bodies[2]);
 players[2].unit_index+=0x10000;assert(coop_spectate_unit(0)==NONE);
 assert(coop_spectate_nothing_to_watch(0));coop_spectate_draw(0);assert(joining==1&&!labels);
 players[2].unit_index=bodies[2];units[2].object.damage_flags=1UL<<_object_dead_bit;
 assert(coop_spectate_unit(0)==NONE);units[2].object.damage_flags=0;
 units[2].unit.player_index=handles[1];assert(coop_spectate_unit(0)==NONE);
 units[2].unit.player_index=handles[2];assert(coop_spectate_unit(0)==bodies[2]);
 coop_spectate_draw(0);assert(labels==1&&!wcscmp(named,L"C"));
 /* The draw may run after departure but before the next camera selection. */
 players[2].quit_out_of_game=TRUE;coop_spectate_draw(0);assert(labels==1&&joining==2);
 players[2].quit_out_of_game=FALSE;handles[2]+=0x10000;units[2].unit.player_index=handles[2];
 assert(coop_spectate_unit(0)==bodies[2]);
 /* The native own-body delay is retained for each death; respawn restores play. */
 players[0].dead_unit_index=bodies[0];assert(coop_spectate_unit(0)==NONE);tick+=89;
 assert(coop_spectate_unit(0)==NONE);tick++;assert(coop_spectate_unit(0)==bodies[2]);
 players[0].unit_index=bodies[0];assert(coop_spectate_unit(0)==NONE);
 i=draws;coop_spectate_draw(0);assert(draws==i);
 /* Opposing factions never enter the view or HUD, including live changes. */
 players[0].unit_index=NONE;players[0].dead_unit_index=NONE;
 players[1].quit_out_of_game=FALSE;units[1].object.owner_team_index=3;players[1].team_index=3;
 coop_spectate_watched[0]=handles[1];assert(coop_spectate_unit(0)==bodies[2]);
 jump[0]=1;assert(coop_spectate_unit(0)==bodies[2]);jump[0]=0;coop_spectate_unit(0);
 units[2].object.owner_team_index=3;players[2].team_index=3;
 i=labels;coop_spectate_draw(0);assert(labels==i&&waiting==1);
 assert(coop_spectate_unit(0)==NONE&&!coop_spectate_nothing_to_watch(0));
 /* A dead enemy still must not expose the host's fallback camera. */
 players[1].unit_index=NONE;players[2].unit_index=NONE;
 assert(!coop_spectate_nothing_to_watch(0));
 /* The body's native team wins while player metadata catches up. */
 players[1].team_index=1;players[2].quit_out_of_game=TRUE;players[1].unit_index=bodies[1];
 assert(!coop_spectate_nothing_to_watch(0));
 /* The viewer changing allegiance retargets; asymmetric hostility refuses. */
 players[0].team_index=3;assert(coop_spectate_unit(0)==bodies[1]);
 players[0].team_index=1;players[1].team_index=2;units[1].object.owner_team_index=2;
 betrayed=TRUE;assert(coop_spectate_unit(0)==NONE&&!coop_spectate_nothing_to_watch(0));betrayed=FALSE;
 assert(coop_spectate_unit(0)==bodies[1]);
 /* Unknown/native teams are refused before narrowing long player metadata. */
 players[0].team_index=0x10001;assert(coop_spectate_unit(0)==NONE&&!coop_spectate_nothing_to_watch(0));
 players[0].team_index=1;units[1].object.owner_team_index=-1;
 assert(coop_spectate_unit(0)==NONE&&!coop_spectate_nothing_to_watch(0));
 units[1].object.owner_team_index=10;
 assert(coop_spectate_unit(0)==NONE&&!coop_spectate_nothing_to_watch(0));
 units[1].object.owner_team_index=1;players[1].team_index=1;
 assert(coop_spectate_unit(0)==bodies[1]);
 players[0].unit_index=bodies[0];i=draws;
 /* Missing/recycled local handles and invalid split-screen slots are safe. */
 locals[0]+=0x10000;assert(coop_spectate_unit(0)==NONE&&!coop_spectate_nothing_to_watch(0));
 coop_spectate_draw(0);assert(draws==i);
 assert(coop_spectate_unit(-1)==NONE&&coop_spectate_unit(4)==NONE);
 assert(!coop_spectate_nothing_to_watch(-1)&&!coop_spectate_nothing_to_watch(4));
 coop_spectate_draw(-1);coop_spectate_draw(4);assert(draws==i);
 puts("PASS: connected native spectator selection, ownership, input, body delay and HUD");return 0;
}
'''


def test_spectator_selection_and_hud(tmp_path):
    run_c(tmp_path, _spectator_unit())


@pytest.mark.parametrize('fault', ['wrap', 'departure', 'ownership', 'faction', 'host-fallback', 'mutual', 'team-wrap'])
def test_spectator_negative_control(tmp_path, fault):
    with pytest.raises(subprocess.CalledProcessError):
        run_c(tmp_path, _spectator_unit(fault))


if __name__ == "__main__":
    test_own_body_first()
    print("PASS")
