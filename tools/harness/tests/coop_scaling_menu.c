#include "harness.h"
#define MIN(a,b) ((a)<(b)?(a):(b))
enum { _cooperative_enemies_per_player=0, _cooperative_enemies_multiplier=1, _loadout_custom=1 };
struct widget_instance {
 char const *name; short type,vertical_offset; boolean visible;
 struct widget_instance *child,*next,*focused_child;
 union {
  struct {short selected_index,number_of_items;struct widget_instance *extended_description;} list;
  struct {short string_list_index;} text_box;
 } parameters;
};
static struct widget_instance *named(struct widget_instance *w,char const *name,int unused)
{
 (void)unused;if(!strcmp(w->name,name))return w;
 for(struct widget_instance *c=w->child;c;c=c->next){struct widget_instance *r=named(c,name,0);if(r)return r;}
 return NULL;
}
static void visible_set(struct widget_instance *w,boolean visible){if(w)w->visible=visible;}
/* menu_functions.c's TEXT CHAT row, which the function holds at OFF while a
game is listed: none here */
#define TEXT_CHAT_SPINNER "text_chat_spinner"
#define TEXT_CHAT_OFF 2
static int p2p_hosting_listed(void){return 0;}
/* The existing settings writer is the boundary; exercise the real mode
 * read/apply helpers without another configuration format or menu framework. */
#define csmemset memset
struct gametype_option { char const *spinner; short count; char const *setting; char const *setting_values[3]; };
struct pc_menu_setting { char const *setting; short value_count; char const *values[3]; };
static boolean extraction_mode,firefight_mode,other_mode;
static int setting_writes,fail_write;
static long custom_number;
static boolean config_boolean(char const *name){return !strcmp(name,"game.extraction")?extraction_mode:firefight_mode;}
static boolean setting_text(char const *name,char *text,unsigned int size,boolean defaults)
{
 if(!strcmp(name,"test.number")){snprintf(text,size,"%ld",custom_number);return TRUE;}
 boolean value=!strcmp(name,"test.bool")?other_mode:config_boolean(name);
 (void)defaults;snprintf(text,size,"%s",value?"true":"false");return TRUE;
}
static short setting_value_index(struct pc_menu_setting const *setting,char const *text)
{
 for(short i=0;i<setting->value_count;i++)if(!strcmp(text,setting->values[i]))return i;
 return 0;
}
static boolean setting_write(char const *name,char const *value)
{
 setting_writes++;if(setting_writes==fail_write)return FALSE;
 if(!strcmp(name,"test.number")){custom_number=atol(value);return TRUE;}
 boolean enabled=!strcmp(value,"true");
 if(!strcmp(name,"game.extraction"))extraction_mode=enabled;
 else if(!strcmp(name,"network.firefight_encounter"))firefight_mode=enabled;
 else if(!strcmp(name,"test.bool"))other_mode=enabled;
 else return FALSE;
 return TRUE;
}
#include "under_test.inc"

#define PIN(value,min,max) ((value)<(min)?(min):MIN((value),(max)))
#define SOUND_FORWARD 0
struct player_profile { int unused; };
static const short player_spawn_count=1;
static const char *solo_map;
static boolean solo_extraction_start_pending, snapshot_allowed, solo_started;
static const char *main_get_map_name(void) { return solo_map; }
static short main_get_solo_level_from_name(const char *name) { return strcmp(name,"a10") ? NONE : 0; }
static const char *main_get_solo_level_name(short level) { (void)level; return "a10"; }
static boolean custom_campaign_map(const char *name) { return !strcmp(name,"patrol@ce"); }
static boolean mcc_maps_level_campaign(const char *name) { return name && !strcmp(name,"mcc_maps\\patrol"); }
static boolean campaign_profile(short controller, struct player_profile *profile) { (void)controller; (void)profile; return TRUE; }
static boolean campaign_fail(void) { return FALSE; }
static void main_set_map_name(const char *name) { solo_map=name; snapshot_allowed=TRUE; }
static void main_disallow_persistent_storage(void) { snapshot_allowed=FALSE; }
static void main_set_difficulty(short difficulty) { (void)difficulty; }
static void game_connection_set(short connection) { (void)connection; }
static void player_ui_set_single_player_local_player_controller(short local,short controller) { (void)local; (void)controller; }
static void main_menu_switch_to_single_player(void) { solo_started=TRUE; }
static void player_ui_remember_player1_profile(boolean remember) { (void)remember; }
static void ui_play_audio_feedback_sound(short sound) { (void)sound; }
struct event_record { short controller_index; };
static short event_controller(struct widget_instance *widget, struct event_record *event)
{ (void)widget; return event->controller_index; }
static short main_get_difficulty(void) { return 1; }
#include "solo_start.inc"
#define ROWS 10
static char const *rows[]={"op_coop_friendly_fire","op_coop_extra_enemies","op_coop_extra_pickups","op_coop_extra_vehicles",
 "op_coop_enemies_multiplier","op_coop_enemies_per_player","op_coop_player_collisions","op_coop_ally_friendly_fire",
 "op_coop_team_wipe","op_coop_vehicle_riding"};
static char const *spinners[]={"coop_friendly_fire_spinner","coop_extra_enemies_spinner","coop_extra_pickups_spinner","coop_extra_vehicles_spinner",
 "coop_enemies_multiplier_spinner","coop_enemies_per_player_spinner","coop_player_collisions_spinner",
 "coop_ally_friendly_fire_spinner","coop_team_wipe_spinner","coop_vehicle_riding_spinner"};
static short counts[]={4,3,3,3,5,5,2,2,3,2};
static struct widget_instance list,help,row[ROWS],label[ROWS],spinner[ROWS],clone[ROWS];
static void setup(void)
{
 memset(&list,0,sizeof(list));list.name="coop_options_menu";list.vertical_offset=9;list.child=row;
 list.parameters.list.extended_description=&help;
 for(int i=0;i<ROWS;i++){
  short y=73+30*i;
  row[i]=(struct widget_instance){.name=rows[i],.visible=TRUE,.vertical_offset=y,.child=&label[i],.next=i<ROWS-1?&row[i+1]:NULL};
  label[i]=(struct widget_instance){.name="label",.vertical_offset=4,.next=&spinner[i]};
  spinner[i]=(struct widget_instance){.name=spinners[i],.type=2,.vertical_offset=1,.child=&clone[i]};
  spinner[i].parameters.list.number_of_items=counts[i];
  clone[i]=(struct widget_instance){.name="clone",.vertical_offset=2};
 }
}
int main(int argc,char **argv)
{
 if(argc==2&&(!strcmp(argv[1],"solo-extraction-ok")||!strcmp(argv[1],"solo-extraction-missing-ok")||
             !strcmp(argv[1],"solo-campaign-ok"))) {
  struct event_record event={0};
  boolean deleted=FALSE;
  boolean ordinary=!strcmp(argv[1],"solo-campaign-ok");
  boolean missing=!strcmp(argv[1],"solo-extraction-missing-ok");
  solo_map=ordinary?"a10":missing?"deleted@ce":"patrol@ce";
  solo_extraction_start_pending=!ordinary;snapshot_allowed=TRUE;
  CHECK(switch_from_main_menu_to_single_player(&list,&event,&deleted)==!missing,"wrong OK button launch result");
  CHECK(solo_started==!missing&&snapshot_allowed==(ordinary||missing),"OK button bypassed operation launch policy");
  return 0;
 }
 if(argc==2&&!strcmp(argv[1],"solo-extraction-fresh")) {
  solo_map="patrol@ce";solo_extraction_start_pending=TRUE;snapshot_allowed=TRUE;
  CHECK(difficulty_start(1,0)&&solo_started&&!snapshot_allowed&&!solo_extraction_start_pending,
   "Extraction restored the old world or left its start request pending");
  CHECK(!strcmp(solo_map,"patrol@ce"),"Extraction launched a different scenario");
  return 0;
 }
 if(argc==2&&!strcmp(argv[1],"solo-extraction-missing")) {
  solo_map="deleted@ce";solo_extraction_start_pending=TRUE;
  CHECK(!difficulty_start(1,0)&&!solo_started,"missing operation fell through to the stock campaign");
  return 0;
 }
 if(argc==2&&!strcmp(argv[1],"solo-campaign-continue")) {
  solo_map="a10";
  CHECK(difficulty_start(1,0)&&solo_started&&snapshot_allowed,"ordinary campaign lost snapshot loading");
  return 0;
 }
 if(argc==2&&!strcmp(argv[1],"entry-modes")) {
  CHECK(forerunner_operation_mode("port forerunner host extraction")==2,"Extraction selected ordinary co-op");
  CHECK(forerunner_operation_mode("port forerunner host firefight")==1,"Firefight selected ordinary co-op");
  CHECK(forerunner_operation_mode("port forerunner host native")==0,"Authored encounter selected a second director");
  CHECK(forerunner_operation_mode("port forerunner host ")==NONE&&forerunner_operation_mode("port forerunner host other")==NONE,"Unknown action changed game rules");
  return 0;
 }

 const char *case_name=argc>1?argv[1]:"";setup();
 CASE("layout") {
  for(int repeat=0;repeat<100;repeat++)for(int mode=0;mode<3;mode++)for(int pickups=0;pickups<3;pickups++)for(int vehicles=0;vehicles<3;vehicles++){
   spinner[1].parameters.list.selected_index=mode;spinner[2].parameters.list.selected_index=pickups;
   spinner[3].parameters.list.selected_index=vehicles;
   list.focused_child=&row[1];gametype_option_help(&list);short y=73;
   CHECK(row[5].visible==(mode==0||pickups==0||vehicles==0),"per-player visibility");
   CHECK(row[4].visible==(mode==1||pickups==1||vehicles==1),"multiplier visibility");
    CHECK(list.focused_child==&row[1],"focus changed");
    int visible=0;for(int i=0;i<ROWS;i++)visible+=row[i].visible;
    short spacing=visible==10?26:visible==9?29:30;
    CHECK(row[9].visible,"riding row hidden");
    for(int i=0;i<ROWS;i++){
     if(row[i].visible){CHECK(row[i].vertical_offset==y,"gap or overlap at %s",rows[i]);
      CHECK(row[i].vertical_offset+28<=340,"rows overlap help area");y+=spacing;}
    CHECK(label[i].vertical_offset==4&&spinner[i].vertical_offset==1&&clone[i].vertical_offset==2,
     "relative descendant positions %s",rows[i]);
    CHECK(row[i].next==(i<ROWS-1?&row[i+1]:NULL),"row order changed");
   }
    CHECK(row[9].vertical_offset==y-spacing,"riding row not last");
  }
 } else CASE("help") {
  for(int mode=0;mode<3;mode++)for(int pickups=0;pickups<3;pickups++)for(int vehicles=0;vehicles<3;vehicles++){
   spinner[1].parameters.list.selected_index=mode;spinner[2].parameters.list.selected_index=pickups;
   spinner[3].parameters.list.selected_index=vehicles;
   gametype_option_help(&list);short offset=0;
   for(int i=0;i<ROWS;i++){
    if(row[i].visible)for(int choice=0;choice<counts[i];choice++){
     list.focused_child=&row[i];spinner[i].parameters.list.selected_index=choice;gametype_option_help(&list);
     CHECK(help.parameters.text_box.string_list_index==offset+choice,"wrong help row%d choice%d",i,choice);
     spinner[1].parameters.list.selected_index=mode;spinner[2].parameters.list.selected_index=pickups;
     spinner[3].parameters.list.selected_index=vehicles;
    }
    offset+=counts[i];
   }
  }
 } else CASE("unrelated") {
  spinner[1].name="unrelated";list.name="other_options";gametype_option_help(&list);
  for(int i=0;i<ROWS;i++)CHECK(row[i].visible&&row[i].vertical_offset==73+30*i,"unrelated menu altered");
 } else CASE("mode-loop") {
  struct gametype_option option={"coop_firefight_spinner",3,"network.firefight_encounter",{"false","true","false"}};
  for(int initial=0;initial<4;initial++)for(int choice=0;choice<3;choice++){
   extraction_mode=(initial&2)!=0;firefight_mode=(initial&1)!=0;
   CHECK(gametype_option_setting_index(&option)==(extraction_mode?2:firefight_mode?1:0),"mode readback");
   setting_writes=fail_write=0;
   CHECK(gametype_option_setting_write(&option,choice),"mode write failed");
   CHECK(extraction_mode==(choice==2)&&firefight_mode==(choice==1),"competing mode left enabled");
   CHECK(gametype_option_setting_index(&option)==choice,"saved mode did not reopen");
  }
 } else CASE("mode-write-failure") {
  struct gametype_option option={"coop_firefight_spinner",3,"network.firefight_encounter",{"false","true","false"}};
  for(int choice=0;choice<3;choice++)for(int failure=1;failure<=2;failure++){
   extraction_mode=choice!=2;firefight_mode=choice==2;setting_writes=0;fail_write=failure;
   CHECK(!gametype_option_setting_write(&option,choice),"write failure hidden");
   CHECK(setting_writes==failure,"enable attempted after disabling failed");
   CHECK(!(extraction_mode&&firefight_mode),"write failure enabled competing director");
  }
 } else CASE("mode-unrelated") {
  struct gametype_option option={"other_spinner",2,"test.bool",{"false","true"}};
  extraction_mode=TRUE;firefight_mode=FALSE;other_mode=FALSE;setting_writes=fail_write=0;
  CHECK(gametype_option_setting_write(&option,1)&&other_mode,"ordinary setting not written");
  CHECK(gametype_option_setting_index(&option)==1,"ordinary readback");
  CHECK(extraction_mode&&!firefight_mode&&setting_writes==1,"ordinary setting changed operations mode");
 } else CASE("custom-setting") {
  struct gametype_option option={"ff_waves_spinner",3,"test.number",{"0","3",NULL}};
  custom_number=999;setting_writes=fail_write=0;
  CHECK(gametype_option_setting_index(&option)==2,"custom value shown as a preset");
  CHECK(gametype_option_setting_write(&option,2),"keep-current failed");
  CHECK(custom_number==999&&!setting_writes,"custom value overwritten");
  CHECK(gametype_option_setting_write(&option,1),"preset failed");
  CHECK(custom_number==3&&setting_writes==1&&gametype_option_setting_index(&option)==1,"preset did not reopen");
  CHECK(gametype_option_setting_write(&option,2)&&custom_number==3&&setting_writes==1,"keep-current rewrote a preset");
 } else return 2;
 return 0;
}
