"""Native widget teardown must clean up without opening a successor on old tags."""
from pathlib import Path
import subprocess

import pytest
from harness import function, mutated
from test_hud_button_style import run_c

ROOT = Path(__file__).resolve().parents[1]

STUBS = r'''
#include <assert.h>
#include <stddef.h>
#include <string.h>
#include <wchar.h>
typedef int boolean;
#define TRUE 1
#define FALSE 0
#define NONE (-1)
#define MAXIMUM_NUMBER_OF_LOCAL_PLAYERS 4
#define WIDGET_DELETED_PLAYER_CONTROL_INHIBIT_FLAGS 0
#define TEST_FLAG(flags,bit) ((flags)&(1<<(bit)))
#define match_assert(file,line,test) assert(test)
#define match_vassert(file,line,test,message) assert(test)
#define error(...) ((void)0)
#define csmemset memset
enum {_widget_event_deleted, _event_handler_run_function_bit,
      _event_handler_open_widget_bit, _ui_widget_type_text_box,
      _ui_widget_type_spinner_list, _ui_widget_type_column_list};
struct event_record { short controller_index; };
struct ui_widget_event_handler_reference {
    short event_type, function;
    unsigned flags;
    struct { long index; } widget_tag;
};
struct ui_widget_definition {
    const char *name;
    struct { long count; void *address; } event_handlers;
};
struct widget_instance {
    boolean delete_recursion_lock, pause_game_time;
    short local_player_index, type;
    long definition_tag_index;
    struct widget_instance *parent,*child,*next,*previous;
    union {
        struct { void *text; } text_box;
        struct { void *list_items,*item_text; struct widget_instance *extended_description; } list;
    } parameters;
};
static struct {
    boolean initialized,sound_paused;
    short pause_game_time_count;
    struct widget_instance *active_widgets[4];
    void *widget_stack[4];
} widget_globals;
static int cleanup_count, opened, freed, stacks, keyboard = 1, settings_closed;
static int widget_memory_pool, we_are_at_the_main_menu;
static wchar_t const *ui_widget_port_error_text;
static struct widget_instance *ui_widget_port_error_text_box;
static struct widget_instance saving, difficulty;
static struct ui_widget_event_handler_reference handler = {
    _widget_event_deleted, 0, (1<<_event_handler_run_function_bit)|(1<<_event_handler_open_widget_bit), {17}
};
static struct ui_widget_definition saving_definition = {"saving profile", {1, &handler}};
#define xbox_pointer(p) (p)
#define ui_widget_definition_get(tag) (&saving_definition)
static void player_control_inhibit_buttons(int a,int b,int c) {}
static boolean ui_widget_event_handler_function_invoke(struct widget_instance *w,
    struct event_record *e,short f,boolean *deleted) {cleanup_count++;return TRUE;}
static struct widget_instance *ui_widget_launch_widget(struct widget_instance *w,long tag)
    {opened++;widget_globals.active_widgets[0]=&difficulty;return &difficulty;}
static boolean game_time_get_paused(void) {return FALSE;}
static void game_time_set_paused(boolean p) {}
static void main_menu_ensure_player_queues_exist(void) {}
static void game_time_dispose_from_old_map(void) {}
static void game_time_initialize_for_new_map(void) {}
static void game_time_start(void) {}
static void sound_pause(boolean p) {}
static void ui_widget_delete_children_recursive(struct widget_instance *w) {}
static void dispose_pointer(int pool,void *pointer) {freed++;}
static boolean virtual_keyboard_active(void) {return keyboard;}
static void virtual_keyboard_close(void) {keyboard=0;}
static void dispose_widget_stack(void **s) {stacks++;*s=NULL;}
static void mcc_ui_settings_close(int player) {settings_closed++;}
'''

MAIN = r'''
int main(void)
{
    int i;
    widget_globals.initialized=TRUE;
    saving.local_player_index=NONE;
    difficulty.local_player_index=NONE;
    widget_globals.active_widgets[0]=&saving;
    /* Ordinary profile-screen completion must still open difficulty selection. */
    ui_widget_delete(&saving);
    assert(cleanup_count==1 && opened==1 && freed==1);
    assert(widget_globals.active_widgets[0]==&difficulty);
    saving.delete_recursion_lock=FALSE;
    cleanup_count=opened=freed=0;
    widget_globals.active_widgets[0]=&saving;
    for(i=0;i<4;i++)widget_globals.widget_stack[i]=(void *)1;
    ui_widgets_close_all();
    assert(cleanup_count==1 && opened==0 && freed==1);
    assert(!keyboard && stacks==4 && settings_closed==1);
    for(i=0;i<4;i++)assert(!widget_globals.active_widgets[i] && !widget_globals.widget_stack[i]);
    assert(!ui_widgets_closing_all);
    /* The teardown scope must not disable subsequent normal menu transitions. */
    saving.delete_recursion_lock=FALSE;
    widget_globals.active_widgets[0]=&saving;
    ui_widget_delete(&saving);
    assert(cleanup_count==2 && opened==1);
    return 0;
}
'''


def generated(regression=False):
    source = (ROOT / 'source/interface/ui_widget.c').read_text()
    state = 'static boolean ui_widgets_closing_all;'
    assert state in source
    delete = function(source, 'ui_widget_delete')
    if regression:
        delete = mutated(delete, ' && !ui_widgets_closing_all', '')
    return STUBS + state + '\n' + delete + '\n' + function(source, 'ui_widgets_close_all') + MAIN


def test_profile_completion_and_map_teardown(tmp_path):
    run_c(tmp_path, generated())


def test_old_profile_successor_leak_is_detected(tmp_path):
    with pytest.raises(subprocess.CalledProcessError):
        run_c(tmp_path, generated(regression=True))

