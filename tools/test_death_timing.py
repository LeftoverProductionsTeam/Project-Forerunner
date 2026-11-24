"""Compile actual death handlers, co-op respawn and camera code against a fake world."""
import argparse
import os
import re
import shutil
import subprocess
import tempfile
from pathlib import Path

ROOT = Path(__file__).resolve().parent.parent


def function(source, name):
    match = re.search(r"^(?:static )?[\w *]+\b" + name + r"\(\s*[^;{]*\)\s*\{", source, re.M)
    if not match:
        raise ValueError(f"Function not found: {name}")
    start = source.index('{', match.start())
    depth, end = 1, start + 1
    # The extracted functions have no braces inside strings/comments.
    while depth:
        depth += (source[end] == '{') - (source[end] == '}')
        end += 1
    return source[match.start():end]


PRELUDE = r'''
#include <assert.h>
#include <stdio.h>
#include <string.h>
#include <float.h>
typedef int boolean;
typedef float real;
typedef unsigned char byte;
typedef struct {real x,y,z;} real_point3d;
typedef struct {real i,j,k;} real_vector3d;
typedef struct {real yaw,pitch;} real_euler_angles2d;
#define TRUE 1
#define FALSE 0
#define NONE (-1)
#define MIN(a,b) ((a)<(b)?(a):(b))
#define MAX(a,b) ((a)>(b)?(a):(b))
#define FLAG(b) (1u<<(b))
#define TEST_FLAG(v,b) ((v)&FLAG(b))
#define match_vassert(...) ((void)0)
static struct {short loss_timer,respawn_timer;boolean lost_map,respawn,saving_map,reset_map,revert_map;long switch_to_structure_bsp_index;} main_globals;
static long main_loss_last_tick,main_respawn_last_tick;
static long tick;
static int paused,cinematic,reverts,attempts,spawns,teleports,player_count=2;
static int projectiles,enemies,airborne,vehicle_moving,spawn_failed,teleport_failed,multiplayer;
static int owned_hub,spawn_room,opening_network,opening_solid,opening_stock,opening_stubbs;
static struct {int respawn_failure,respawn_failed;} players_state,*players_globals=&players_state;
struct player_datum {long unit_index,team_index;boolean quit_out_of_game;};
static struct player_datum players[4];
struct object_datum {struct {real_point3d bounding_sphere_center;} object;};
static struct object_datum objects[4];
struct biped_datum {struct {unsigned flags;} biped;};
static struct biped_datum biped;
struct players_vehicle_datum {struct {int unknown_state;} vehicle;};
static struct players_vehicle_datum vehicle;
struct data_iterator {long datum_index;int next;};
static int player_data;
/* (the network co-op paths, off: this is a local game) */
enum {_game_connection_local,_game_connection_network_client,_game_connection_network_server};
static short game_connection(void) {return _game_connection_local;}
static boolean network_coop_active(void) {return opening_network;}
static int spectator_mode;
static boolean coop_spectating(void) {return spectator_mode;}
static boolean players_respawn_network_coop(boolean starting) {(void)starting;return FALSE;}
static void players_respawn_at_checkpoint(void) {}
static boolean players_are_all_dead(void) {return FALSE;}
static boolean main_coop_host_revert(void) {return FALSE;}
static boolean network_coop_wipe_reverts(void) {return FALSE;}
/* These independent mode/transition owners are off in this local timing world. */
static void main_script_map_name_clear(void) {}
static boolean coop_enemies_firefight_active(void) {return FALSE;}
static boolean coop_enemies_firefight_out_of_lives(void) {return FALSE;}
static boolean extraction_respawn_after_loss(void) {return FALSE;}
static boolean mcc_skulls_restart_on_death(void) {return FALSE;}
enum {_biped_airborne_bit=0,_object_mask_vehicle=1};
static void data_iterator_new(struct data_iterator *i,int d) {(void)d;i->next=0;}
static void *data_iterator_next(struct data_iterator *i) {
    if(i->next>=player_count) return NULL;
    i->datum_index=i->next++;return &players[i->datum_index];
}
static struct player_datum *player_get(long i) {assert(i>=0&&i<player_count);return &players[i];}
static struct object_datum *object_try_and_get(long i) {
    if(i==NONE) return NULL;
    assert(i>=100&&i<104);return &objects[i-100];
}
static struct object_datum *object_get(long i) {return object_try_and_get(i);}
static long object_get_ultimate_parent(long i) {return vehicle_moving ? 999 : i;}
static struct biped_datum *biped_try_and_get(long i) {(void)i;biped.biped.flags=airborne;return &biped;}
static void *object_try_and_get_and_verify_type(long i,int mask) {
    (void)i;(void)mask;vehicle.vehicle.unknown_state=vehicle_moving;return &vehicle;
}
static int dangerous_projectiles_near_player(void) {return projectiles;}
static int any_unit_is_dangerous(void) {return 0;}
static int ai_enemies_attacking_player(void) {return enemies;}
static void player_spawn(long i) {spawns++;if(!spawn_failed) players[i].unit_index=100+i;}
static int player_teleport(long i,long target,const real_point3d *p) {
    (void)i;assert(target==100);assert(p->x==100);teleports++;return !teleport_failed;
}
static long game_time_get(void) {return tick;}
static int game_time_get_paused(void) {return paused;}
static int cinematic_in_progress(void) {return cinematic;}
static boolean extraction_owns_map(void) {return owned_hub;}
static boolean players_coop_room_to_spawn(void) {return spawn_room;}
struct player_starting_location {real_point3d position;short structure_bsp_reference_index;};
static struct player_starting_location starts[2];
struct scenario {struct {long count;} players;};
static struct scenario opening_scenario={{2}};
static short global_structure_bsp_index;
#define TAG_BLOCK_GET_ELEMENT(block,index,type) (&starts[index])
enum {_collision_test_front_facing_surfaces_bit,_collision_test_structure_bit,_collision_test_objects_scenery_bit};
static struct scenario *global_scenario_get(void) {return &opening_scenario;}
static boolean stubbs_mode_active(void) {return opening_stubbs;}
static char const *main_get_map_name(void) {return "campaign";}
static short main_get_solo_level_from_name(char const *name) {(void)name;return opening_stock?0:NONE;}
static long scenario_leaf_index_from_point(real_point3d const *p) {return p->x<200.f?0:NONE;}
static boolean collision_test_point(unsigned flags,real_point3d const *p,long ignore)
{(void)flags;(void)p;(void)ignore;return opening_solid;}
static real game_engine_get_starting_location_rating(long player,struct player_starting_location const *p)
{(void)player;(void)p;return 1.f;}
static void object_get_origin(long unit,real_point3d *out) {*out=object_get(unit)->object.bounding_sphere_center;}
static real distance_squared3d(real_point3d const *a,real_point3d const *b)
{return (a->x-b->x)*(a->x-b->x);}
static void player_spawn_at_location(long player,struct player_starting_location const *p)
{assert(p==&starts[1]);player_spawn(player);}
static int game_engine_running(void) {return multiplayer;}
static void game_state_revert(void) {reverts++;}
static real_vector3d zero_vector,*global_zero_vector3d=&zero_vector;
static void vector3d_from_euler_angles2d(real_vector3d *v,const real_euler_angles2d *a) {
    (void)a;v->i=1;v->j=v->k=0;
}
static void observer_up_from_forward(const real_vector3d *f,real_vector3d *u) {
    (void)f;u->k=1;u->i=u->j=0;
}
static struct {real dead_timer,multiplayer_switch_timer,singleplayer_switch_timer;} dead_camera_constants={3,15,3};
'''
TESTS = r'''
static void reset(int count) {
    int i;
    memset(&main_globals,0,sizeof(main_globals));memset(&players_state,0,sizeof(players_state));
    tick=main_loss_last_tick=main_respawn_last_tick=0;
    paused=cinematic=reverts=attempts=spawns=teleports=0;
    spectator_mode=0;
    projectiles=enemies=airborne=vehicle_moving=spawn_failed=teleport_failed=multiplayer=0;
    owned_hub=0;spawn_room=1;
    opening_network=opening_solid=opening_stock=opening_stubbs=0;
    player_count=count;
    for(i=0;i<count;i++) {players[i].unit_index=i?NONE:100;players[i].team_index=0;objects[i].object.bounding_sphere_center.x=100+i;}
}
static void rates(void) {
    int rates[]={15,30,60,120,144,240,1000};
    int r,frame,count;
    for(r=0;r<7;r++) {
        int hz=rates[r];
        reset(2);main_lost_map();
        for(frame=1;!reverts;frame++) {
            assert(frame<hz*4);tick=(long)frame*30/hz;
            main_lost_map();main_lost_map_private();
            if(tick<92) assert(!reverts);
        }
        assert(reverts==1&&!main_globals.lost_map);
        printf("%d FPS: reload at %.3fs\n",hz,(frame-1)/(double)hz);
        for(count=2;count<=4;count++) {
            reset(count);main_respawn(FALSE);
            for(frame=1;main_globals.respawn;frame++) {
                assert(frame<hz*4);tick=(long)frame*30/hz;
                main_respawn(FALSE);main_respawn_private();
                if(tick<92) assert(!spawns&&!attempts);
            }
            assert(spawns==count-1&&teleports==count-1&&!main_globals.lost_map);
        }
    }
}
static void clock_and_lifecycle(void) {
    int i;
    reset(2);main_lost_map();main_respawn(FALSE);
    for(i=0;i<10000;i++) {main_lost_map_private();main_respawn_private();}
    assert(!reverts&&!attempts&&!main_globals.loss_timer&&!main_globals.respawn_timer);
    tick=40;main_lost_map_private();main_respawn_private();
    paused=1;tick=400;main_lost_map_private();main_respawn_private();
    assert(main_globals.loss_timer==40&&main_globals.respawn_timer==40);
    paused=0;main_lost_map_private();main_respawn_private();assert(!reverts&&!attempts);
    tick=451;main_lost_map_private();main_respawn_private();assert(!reverts&&!attempts);
    tick=452;main_lost_map_private();main_respawn_private();assert(reverts==1&&spawns==1);
    main_lost_map();players[1].unit_index=NONE;main_respawn(FALSE);
    assert(!main_globals.loss_timer&&!main_globals.respawn_timer);
    tick=0;main_lost_map_private();main_respawn_private();
    tick=91;main_lost_map_private();main_respawn_private();assert(reverts==1&&spawns==1);
    tick=92;main_lost_map_private();main_respawn_private();assert(reverts==2&&spawns==2);
    reset(2);main_respawn(FALSE);tick=40;main_respawn_private();
    cinematic=1;tick=400;main_respawn_private();assert(main_globals.respawn_timer==40);
    cinematic=0;main_respawn_private();assert(!attempts);
    tick=452;main_respawn_private();assert(spawns==1);
    reset(2);main_lost_map();cinematic=1;tick=92;main_lost_map_private();assert(reverts==1);
    reset(2);main_respawn(TRUE);main_respawn_private();assert(spawns==1);
    reset(2);main_respawn(TRUE);paused=1;main_respawn_private();assert(!attempts);
    paused=0;main_respawn_private();assert(spawns==1);
    reset(2);main_lost_map();tick=60;main_lost_map_private();
    main_reset_map();assert(!main_globals.lost_map);tick=600;main_lost_map();
    main_lost_map_private();assert(!reverts&&!main_globals.loss_timer);
    tick=692;main_lost_map_private();assert(reverts==1);
    main_lost_map();tick=720;main_lost_map_private();main_revert_map();
    assert(!main_globals.lost_map);tick=800;main_lost_map();main_lost_map_private();
    assert(reverts==1&&!main_globals.loss_timer);
    reset(2);main_lost_map();tick=2147483647L;main_lost_map_private();assert(reverts==1);
    reset(2);tick=2147483600L;main_lost_map();tick=2147483647L;main_lost_map_private();
    assert(main_globals.loss_timer==47);tick=-2147483647L-1;main_lost_map_private();
    assert(!main_globals.loss_timer);tick=-2147483556L;main_lost_map_private();assert(reverts==1);
}
static void coop_blocked(void) {
    int cause,i;
    for(cause=0;cause<5;cause++) {
        reset(2);
        if(cause==0) projectiles=1;
        if(cause==1) enemies=1;
        if(cause==2) airborne=1;
        if(cause==3) vehicle_moving=1;
        if(cause==4) spawn_failed=1;
        main_respawn(FALSE);tick=92;main_respawn_private();assert(main_globals.respawn);
        for(i=0;i<40000;i++) {tick++;main_respawn_private();}
        assert(main_globals.respawn_timer==92&&main_globals.respawn);
        projectiles=enemies=airborne=vehicle_moving=spawn_failed=0;
        main_respawn_private();assert(!main_globals.respawn&&players[1].unit_index!=NONE);
    }
    reset(2);main_respawn(FALSE);players[0].unit_index=NONE;main_lost_map();
    tick=91;main_lost_map_private();main_respawn_private();assert(!reverts&&!spawns);
    tick=92;main_lost_map_private();main_respawn_private();assert(reverts==1&&!spawns);
}
static void cameras(void) {
    struct dead_camera c;
    struct dead_camera_command command;
    struct camera_control control={0,.1f};
    reset(2);memset(&c,0,sizeof(c));memset(&command,0,sizeof(command));
    c.player_index=1;c.current_player_index=NONE;c.unit_index=101;c.switch_timer=0;c.timer=3;
    players[0].unit_index=NONE;
    dead_camera_update(&c,&control,&command);
    assert(c.unit_index==101&&command.position.x==101&&c.switch_timer==3);
    c.switch_timer=0;dead_camera_update(&c,&control,&command);assert(c.unit_index==101);
    players[0].unit_index=100;c.switch_timer=0;
    dead_camera_update(&c,&control,&command);assert(c.unit_index==100&&c.current_player_index==0);
    c.unit_index=NONE;c.position.x=77;c.switch_timer=1;
    dead_camera_update(&c,&control,&command);assert(command.position.x==77);
    c.unit_index=101;c.current_player_index=NONE;c.switch_timer=0;paused=1;
    dead_camera_update(&c,&control,&command);assert(c.unit_index==101);
    paused=0;multiplayer=1;c.switch_timer=0;
    dead_camera_update(&c,&control,&command);assert(c.unit_index==100&&c.switch_timer==15);
    spectator_mode=1;c.unit_index=101;c.switch_timer=0;
    dead_camera_update(&c,&control,&command);assert(c.unit_index==101);
}
static void authored_hub_spawns(void) {
    reset(2);tick=COOP_LEVEL_START_TICKS;spawn_room=0;
    assert(players_coop_extras_held()); /* Campaign waits for a safe teammate. */
    owned_hub=1;
    assert(!players_coop_extras_held()); /* An admitted hub has its own starts. */
    cinematic=1;assert(players_coop_extras_held());
    cinematic=0;tick=0;assert(players_coop_extras_held());
    owned_hub=0;spawn_room=1;tick=COOP_LEVEL_START_TICKS;
    assert(!players_coop_extras_held());
}
static void opening_partner_spawns(void) {
    reset(3);opening_network=opening_stock=1;
    starts[0]=(struct player_starting_location){{100,0,0},0};
    starts[1]=(struct player_starting_location){{101.5f,0,0},0};
    tick=1;assert(!players_coop_spawn_opening_partner(1));
    tick=0;owned_hub=1;assert(!players_coop_spawn_opening_partner(1));owned_hub=0;
    opening_stock=0;assert(!players_coop_spawn_opening_partner(1));opening_stock=1;
    opening_stubbs=1;assert(!players_coop_spawn_opening_partner(1));opening_stubbs=0;
    cinematic=1;assert(!players_coop_spawn_opening_partner(1));cinematic=0;
    opening_solid=1;assert(!players_coop_spawn_opening_partner(1));opening_solid=0;
    starts[1].structure_bsp_reference_index=1;assert(!players_coop_spawn_opening_partner(1));
    starts[1].structure_bsp_reference_index=0;
    starts[1].position.x=100.5f;assert(!players_coop_spawn_opening_partner(1));
    starts[1].position.x=101.5f;
    assert(!spawns&&players[1].unit_index==NONE);
    assert(players_coop_spawn_opening_partner(1)&&players[1].unit_index==101);
    assert(!players_coop_spawn_opening_partner(2)&&spawns==1);
}
int main(void) {
    rates();clock_and_lifecycle();coop_blocked();cameras();authored_hub_spawns();opening_partner_spawns();
    puts("PASS: fixed-tick delays, render-only/catch-up frames, pause/cinematics, clock reset,");
    puts("      2-4 player co-op, blocked retry/overflow, dead-camera fallback and authored hub spawn policy");
    return 0;
}
'''


def structure(source, name):
    start = source.index('struct ' + name + '\n{')
    return source[start:source.index('\n};', start) + 4] + '\n'


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('--cc', default='clang')
    parser.add_argument('--main-source', type=Path, default=ROOT / 'source/main/main.c')
    args = parser.parse_args()
    main_source = args.main_source.read_text()
    camera_source = (ROOT / 'source/camera/dead_camera.c').read_text()
    camera_header = (ROOT / 'source/camera/dead_camera.h').read_text()
    players_source = (ROOT / 'source/game/players.c').read_text()
    unit = PRELUDE
    unit += '\n#define TICKS_PER_SECOND 30\n'
    unit += re.search(r'^#define COOP_LEVEL_START_TICKS[^\n]*', players_source, re.M).group(0) + '\n'
    unit += function(players_source, 'players_coop_extras_held') + '\n'
    unit += function(players_source, 'players_coop_spawn_opening_partner') + '\n'
    for name in ('camera_control', 'dead_camera_command'):
        unit += structure(camera_source, name)
    unit += structure(camera_header, 'dead_camera')
    unit += '#define players_respawn_coop players_respawn_coop_real\n'
    unit += function(players_source, 'players_respawn_coop')
    unit += '\n#undef players_respawn_coop\nstatic boolean players_respawn_coop(void) {attempts++;return players_respawn_coop_real();}\n'
    for name in ('main_lost_map', 'main_respawn', 'main_reset_map', 'main_revert_map', 'main_death_timer_expired',
                 'main_lost_map_private', 'main_respawn_private'):
        unit += function(main_source, name)
    for name in ('player_has_allies', 'player_get_next_player_with_a_unit', 'dead_camera_update'):
        unit += function(camera_source, name)
    unit += TESTS
    cc = shutil.which(args.cc) or args.cc
    with tempfile.TemporaryDirectory(prefix='halo-death-timing-') as temp:
        c = Path(temp) / 'death.c'
        exe = Path(temp) / ('death.exe' if os.name == 'nt' else 'death')
        c.write_text(unit)
        flags = ['--target=i686-pc-windows-msvc', '-fuse-ld=lld'] if os.name == 'nt' else []
        subprocess.run([cc, *flags, '-std=c11', '-Wno-parentheses', '-O2', str(c), '-o', str(exe)], check=True)
        subprocess.run([str(exe)], check=True)


if __name__ == '__main__':
    main()
