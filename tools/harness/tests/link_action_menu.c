#include "harness.h"
#ifndef _WIN32
#include <strings.h>
#define _stricmp strcasecmp
#endif
#define MAXIMUM_STRINGS 32
#define NUMBEROF(a) ((short)(sizeof(a)/sizeof((a)[0])))
struct widget_instance {
 char const *name;
 long definition_tag_index; short type;
 struct widget_instance *parent,*child,*next;
 union {struct {short selected_index,number_of_items;} list;} parameters;
};
struct player_profile {byte fields[8];};
#include "under_test_types.inc"
/* the profile's settings: none here */
static struct {char const *name;short field;long default_value;} profile_settings[1]={{"profile.none",0,0}};
static struct player_profile *player_ui_get_edit_player_profile(void){return NULL;}
static byte *profile_setting_field(struct player_profile *profile,short field){return &profile->fields[field];}
static boolean profile_setting_boolean(short field){(void)field;return FALSE;}
static boolean config_default(char const *name,char *text,unsigned int size){(void)name;snprintf(text,size,"x");return TRUE;}
static boolean config_text(char const *name,char *text,unsigned int size);
static boolean config_boolean(char const *name){(void)name;return FALSE;}
static int config_writes;
static boolean config_write(char const *name,char const *value);
static void platform_log(char const *format,...){(void)format;}
/* player_link.c: the master a refusing server needs, and what OK does */
static int needed_master=-1,begun=-1,begin_count,unlink_count;
static int player_link_needed_master(void){return needed_master;}
static int player_link_begin(int master){begun=master;begin_count++;return 1;}
static void player_link_unlink(void){unlink_count++;}
static struct pc_menu_setting link_setting={7,"link.action",6,{"none","link_1","link_2","link_3","link_4","unlink"},NONE};
static struct pc_menu_setting *pc_menu_setting_get(long definition_index);
static struct widget_instance *named(struct widget_instance *widget,char const *name,long unused)
{
 struct widget_instance *child,*found;(void)unused;
 if(widget->name && !strcmp(widget->name,name))return widget;
 for(child=widget->child;child;child=child->next)if((found=named(child,name,0))!=NULL)return found;
 return NULL;
}
#include "under_test.inc"
static char graphics_values[NUMBEROF(graphics_presets)][32];
static struct pc_menu_setting graphics_setting={100,"graphics.preset",3,{"classic","enhanced","custom"},NONE};
static struct pc_menu_setting graphics_rows[NUMBEROF(graphics_presets)];
static struct widget_instance graphics_screen,graphics_spinner,graphics_widgets[NUMBEROF(graphics_presets)];
static boolean config_text(char const *name,char *text,unsigned int size)
{
 for(short i=0;i<NUMBEROF(graphics_presets);i++)if(!strcmp(name,graphics_presets[i].name)){
  snprintf(text,size,"%s",graphics_values[i]);return TRUE;
 }
 snprintf(text,size,"x");return TRUE;
}
static boolean config_write(char const *name,char const *value)
{
 for(short i=0;i<NUMBEROF(graphics_presets);i++)if(!strcmp(name,graphics_presets[i].name)){
  snprintf(graphics_values[i],sizeof(graphics_values[i]),"%s",value);config_writes++;return TRUE;
 }
 CHECK(FALSE,"synthetic preset or unexpected setting written: %s",name);return FALSE;
}
static struct pc_menu_setting *pc_menu_setting_get(long index)
{
 if(index==7)return &link_setting;
 if(index==100)return &graphics_setting;
 if(index>=200 && index<200+NUMBEROF(graphics_rows))return &graphics_rows[index-200];
 return NULL;
}
static void graphics_open(void)
{
 memset(&graphics_screen,0,sizeof(graphics_screen));memset(&graphics_spinner,0,sizeof(graphics_spinner));
 graphics_screen.child=&graphics_spinner;
 graphics_spinner.name="graphics_preset_spinner";graphics_spinner.definition_tag_index=100;graphics_spinner.type=2;
 graphics_spinner.parent=&graphics_screen;graphics_spinner.next=graphics_widgets;graphics_spinner.parameters.list.number_of_items=3;
 for(short i=0;i<NUMBEROF(graphics_presets);i++){
  struct widget_instance *w=&graphics_widgets[i];struct pc_menu_setting *s=&graphics_rows[i];
  memset(w,0,sizeof(*w));memset(s,0,sizeof(*s));w->type=2;w->definition_tag_index=200+i;w->parent=&graphics_screen;
  w->parameters.list.number_of_items=2;w->next=i+1<NUMBEROF(graphics_presets)?&graphics_widgets[i+1]:NULL;
  s->setting=graphics_presets[i].name;s->value_count=2;s->values[0]=graphics_presets[i].classic;s->values[1]=graphics_presets[i].enhanced;
  CHECK(setting_load(w),"graphics option did not load");
 }
 CHECK(setting_load(&graphics_spinner),"preset did not load");
}
static void graphics_case(char const *case_name)
{
 for(short i=0;i<NUMBEROF(graphics_presets);i++)snprintf(graphics_values[i],sizeof(graphics_values[i]),"%s",graphics_presets[i].classic);
 config_writes=0;graphics_open();CHECK(graphics_spinner.parameters.list.selected_index==0,"classic not recognized");
 graphics_spinner.parameters.list.selected_index=1;graphics_preset_show(&graphics_screen);
 for(short i=0;i<NUMBEROF(graphics_presets);i++)CHECK(!strcmp(graphics_rows[i].values[graphics_widgets[i].parameters.list.selected_index],
  graphics_presets[i].enhanced),"enhanced did not preview a row");
 CHECK(!config_writes && !strcmp(graphics_preset_current(),"classic"),"preview changed saved settings");
 if(!strcmp(case_name,"graphics-preview")){
  /* Closing without OK drops only widgets, as native Cancel does. */
  graphics_open();CHECK(graphics_spinner.parameters.list.selected_index==0 && !config_writes,"cancel persisted the preview");
  return;
 }
 if(!strcmp(case_name,"graphics-custom")){
  graphics_widgets[1].parameters.list.selected_index=0;graphics_preset_show(&graphics_screen);
  CHECK(graphics_spinner.parameters.list.selected_index==2,"manual edit did not show custom");
 }
 graphics_preset_show(&graphics_screen);settings_each(&graphics_screen,setting_changed_save);
 CHECK(config_writes>0,"OK did not save real graphics options");
 graphics_open();CHECK(graphics_spinner.parameters.list.selected_index==(!strcmp(case_name,"graphics-custom")?2:1),"reopen lost the saved combination");
}
static struct widget_instance spinner;
/* the screen opened (the spinner's "port setting load"), the spinner moved
by `moves`, then OK ("port settings save") */
static void open_choose_ok(int needed,int moves)
{
 needed_master=needed;begun=-1;begin_count=unlink_count=config_writes=0;
 memset(&spinner,0,sizeof(spinner));spinner.definition_tag_index=7;spinner.type=2;
 spinner.parameters.list.number_of_items=6;
 CHECK(setting_load(&spinner),"load failed");
 spinner.parameters.list.selected_index=(short)((spinner.parameters.list.selected_index+moves+6)%6);
 CHECK(setting_changed_save(&spinner,&link_setting),"save failed");
 CHECK(config_writes==0,"link.action written to config.toml");
}
int main(int argc,char **argv)
{
 const char *case_name=argc>1?argv[1]:"";
 if(!strncmp(case_name,"graphics-",9)){graphics_case(case_name);return 0;}
 CASE("chosen_already") {
  /* (a server refused this machine for its VPN: master 1 is shown, and OK
  on it opens its page) */
  open_choose_ok(0,0);
  CHECK(spinner.parameters.list.selected_index==1,"the needed master not shown");
  CHECK(begin_count==1&&begun==0,"OK on the master chosen already did nothing");
 } else CASE("chosen") {
  open_choose_ok(-1,0);
  CHECK(spinner.parameters.list.selected_index==0,"NO CHANGE not shown");
  CHECK(begin_count==0&&unlink_count==0,"NO CHANGE did something");
  open_choose_ok(-1,2);
  CHECK(begin_count==1&&begun==1,"WITH MASTER 2 not begun");
  open_choose_ok(0,-1);
  CHECK(begin_count==0&&unlink_count==0,"NO CHANGE did something");
  open_choose_ok(2,2);
  CHECK(unlink_count==1&&begin_count==0,"UNLINK ALL not done");
 } else return 2;
 return 0;
}
