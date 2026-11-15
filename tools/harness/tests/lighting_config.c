#include "harness.h"
#include <string.h>
static const char *mode = "off";
static unsigned long generation;
static unsigned int selected;
static int part_reads, mode_reads;
static int config_boolean(const char *name);
static const char *config_string(const char *name)
{
 CHECK(!strcmp(name,"display.light_effects"),"unexpected mode lookup");mode_reads++;return mode;
}
static unsigned long config_changes(void){return generation;}
#include "under_test.inc"
static int config_boolean(const char *name)
{
 for(unsigned int i=0;i<sizeof(parts)/sizeof(parts[0]);i++)if(!strcmp(name,parts[i].name)){
  part_reads++;return (selected&(1u<<i))!=0;
 }
 CHECK(!strcmp(name,"other.setting"),"unknown part lookup");return 1;
}
static void choose(const char *value,unsigned int mask){mode=value;selected=mask;generation++;}
static void check_parts(unsigned int expected)
{
 for(unsigned int i=0;i<sizeof(parts)/sizeof(parts[0]);i++)
  CHECK(lighting_config_boolean(parts[i].name)==((expected&(1u<<i))!=0),"wrong effect selection: %s",parts[i].name);
}
int main(int argc,char **argv)
{
 const char *case_name=argc>1?argv[1]:"";
 unsigned int count=(unsigned int)(sizeof(parts)/sizeof(parts[0])),all=(1u<<count)-1;
 CASE("stock-modes"){
  choose("off",all);check_parts(0);choose("low",0);check_parts(7);choose("high",0);check_parts(all);
  choose("auto",0);xgpu_software_renderer=1;check_parts(0);
  xgpu_software_renderer=0;xgpu_weak_renderer=1;check_parts(7);xgpu_weak_renderer=0;check_parts(all);
  CHECK(!part_reads,"presets unexpectedly read/changed custom settings");
 } else CASE("custom"){
  for(unsigned int i=0;i<count;i++){choose("custom",1u<<i);check_parts(1u<<i);}
  choose("custom",0);check_parts(0);choose("custom",all);check_parts(all);
  CHECK(lighting_config_level()==2,"custom broke public quality range");
 } else CASE("reload"){
  choose("custom",1);check_parts(1);choose("custom",2);check_parts(2);
  choose("off",2);check_parts(0);choose("high",2);check_parts(all);choose("custom",2);check_parts(2);
 } else CASE("cache"){
  choose("custom",5);check_parts(5);int reads=part_reads,lookups=mode_reads;
  for(int frame=0;frame<180;frame++)check_parts(5);
  CHECK(reads==(int)count&&part_reads==reads&&mode_reads==lookups,"settings reread per draw/frame");
  choose("custom",6);check_parts(6);CHECK(part_reads==reads+(int)count,"saved changes did not refresh parts");
 } else CASE("fallback"){
  choose("unknown",0);check_parts(all);CHECK(lighting_config_boolean("other.setting"),"unrelated boolean no longer passed through");
 } else CASE("unsupported"){
  choose("custom",all);check_parts(0);CHECK(lighting_config_level()==0&&!part_reads,"unsupported target enabled/read custom lights");
 } else return 2;
 return 0;
}
