/* The real gear codec/transactions over a bounded fake filesystem. Native
 * locking, replacement and process death are tested by test_gear_files.py. */
#include "harness.h"
#define MIN(a,b) ((a)<(b)?(a):(b))
#define MAX(a,b) ((a)>(b)?(a):(b))
#define PIN(v,a,b) MIN(MAX(v,a),b)
#define NUMBEROF(a) ((long)(sizeof(a)/sizeof((a)[0])))
#define VALID_INDEX(i,n) ((i)>=0&&(i)<(n))
#define SHORT_MAX 32767
#ifdef GEAR_NATIVE_FILES
#include "posix.h"
#else
typedef unsigned long posix_ulong;
#endif
#include "types.inc"

static struct { char path[1024]; byte data[65536]; posix_ulong size; boolean present, locked; } files[4];
static boolean client, playing = TRUE, fail_write, publish_failure, shared_cache;
static int connection = _game_connection_local, folder_calls, file_count;
static char const *map = "candidate@ce";
static unsigned long checksum = 42;
static char const *native_folder;
static boolean game_in_progress(void) { return playing; }
static boolean network_game_distributed_client(void) { return client; }
static int game_connection(void) { return connection; }
static char const *cache_file_loaded_map_name(void) { return map; }
static unsigned long cache_files_get_checksum(void) { return checksum; }
char const *platform_folder(enum port_folder folder) { folder_calls++; return native_folder ? native_folder : "owned/private"; }
#ifndef GEAR_NATIVE_FILES
static int posix_is_link(char const *path) { return 0; }
static int posix_make_directory(char const *path) { return 0; }
static int posix_stat(char const *path, struct posix_file_information *information)
{ information->flags = _posix_file_is_directory; return 0; }
static int file_index(char const *path)
{
 int index;
 for (index = 0; index < file_count; index++) if (!strcmp(files[index].path, path)) return index;
 return -1;
}
static int posix_gear_lock(char const *path)
{
 char record[1024]; int index;
 snprintf(record, sizeof(record), "%s", path); record[strlen(record)-5] = 0;
 index = file_index(record);
 if (index < 0) { CHECK(file_count < 4, "fixture files exhausted"); index = file_count++; strcpy(files[index].path, record); }
 if (files[index].locked) return -1;
 files[index].locked = TRUE; return index;
}
static void posix_gear_unlock(int descriptor) { files[descriptor].locked = FALSE; }
static int posix_gear_read(char const *path, void *buffer, posix_ulong capacity, posix_ulong *size)
{
 int index = file_index(path); *size = 0;
 if (index < 0 || !files[index].present) return 0;
 if (files[index].size > capacity) return -1;
 memcpy(buffer, files[index].data, files[index].size); *size = files[index].size; return 1;
}
static int posix_gear_replace(char const *path, void const *buffer, posix_ulong size)
{
 int index = file_index(path);
 CHECK(index >= 0 && files[index].locked, "record write without owner lock");
 if (fail_write) return 0;
 CHECK(size <= sizeof(files[index].data), "record overflow");
 memcpy(files[index].data, buffer, size); files[index].size = size; files[index].present = TRUE;
 return !publish_failure;
}
#endif
#include "sha.inc"
#define config_boolean(name) shared_cache
#ifndef EXTRACTION_FIXTURE
boolean game_modes_inventory_weapon_for_unit(long unit_index, struct game_modes_inventory_weapon const *item)
{ (void)unit_index; return game_modes_inventory_weapon_compatible(item); }
#endif
#include "under_test.inc"
#undef config_boolean

#ifndef GEAR_FIXTURE_ONLY
/* Native tag admission is a boundary adapter here; the connected Extraction
 * fixture compiles and exercises its actual implementation. */
static boolean native_compatible = TRUE;
boolean game_modes_inventory_weapon_compatible(struct game_modes_inventory_weapon const *item)
{ return native_compatible; }
static struct gear_session session, reopened;
static byte owner[GEAR_ID_BYTES];
static struct game_modes_inventory inventory;
static long objects[GAME_MODES_INVENTORY_WEAPONS] = { 0x10001, 0x10002, 0x10003, NONE };

static void capture(short count)
{
 short index;
 memset(&inventory, 0, sizeof(inventory));
 strcpy(inventory.map_name, map); inventory.map_checksum = checksum;
 inventory.weapon_count = count; inventory.selected_slot = count ? 0 : NONE;
 inventory.grenades[0] = 2; inventory.grenades[1] = 1;
 for (index = 0; index < count; index++) {
  strcpy(inventory.weapons[index].tag_path, index ? "weapons\\pistol\\pistol" : "weapons\\rifle\\rifle");
  inventory.weapons[index].slot = index;
  inventory.weapons[index].rounds_loaded[0] = 5 + index;
  inventory.weapons[index].rounds_reserve[0] = 50 + index;
  inventory.weapons[index].age = .25f;
 }
}

static void earn(short count)
{
 CHECK(gear_reserve(&session,TRUE) && gear_confirm_deployment(&session), "initial deployment failed");
 capture(count);
 CHECK(gear_prepare_escape(&session, &inventory, objects), "escape escrow failed");
 CHECK(gear_finish_escape(&session), "escape settlement failed");
}

int main(int argc, char **argv)
{
 char const *case_name = argv[1];
 struct game_modes_inventory loadout;
 byte data[GEAR_RECORD_BYTES], other[GEAR_ID_BYTES];
 struct gear_record decoded;
 posix_ulong size;
#ifdef GEAR_NATIVE_FILES
 if(argc!=3||strncmp(case_name,"native-",7))return 2;
 native_folder=argv[2];
#endif
 gear_profile_owner("u:\\saved\\profile-one", owner);
 CHECK(gear_nonzero(owner), "profile owner missing");
 if (!strcmp(case_name, "authority")) {
  client = TRUE; CHECK(!gear_open(&session, owner) && !file_count && !folder_calls, "client opened gear storage");
  client = FALSE; connection = 999;
  CHECK(!gear_open(&session, owner) && !file_count && !folder_calls, "unsupported connection opened gear storage");
  return 0;
 }
 CHECK(gear_open(&session, owner), "owner record did not open");
 CHECK(!gear_open(&reopened, owner), "same owner admitted twice");
 if (!strcmp(case_name,"native-prepared")||!strcmp(case_name,"native-issued")||
     !strcmp(case_name,"native-starter-prepared")||!strcmp(case_name,"native-starter-issued")) {
  earn(2);session.grenade_room[0]=session.grenade_room[1]=4;
  CHECK(gear_reserve(&session,strncmp(case_name,"native-starter-",15)!=0),"native preparation failed");
  if(!strcmp(case_name,"native-issued")||!strcmp(case_name,"native-starter-issued"))CHECK(gear_confirm_deployment(&session),"native issue confirmation failed");
  puts("ready");fflush(stdout);getchar(); /* Parent kills this owned process without gear_close. */
 } else if(!strcmp(case_name,"native-refund")||!strcmp(case_name,"native-starter-recovery")) {
  CHECK(session.record.count==2&&!session.record.reserved_count&&session.record.phase==_gear_idle&&
   session.record.grenades[0]==2&&session.record.grenades[1]==1,"native process death lost unissued gear");
  CHECK(gear_loadout(&session,&loadout,TRUE)&&loadout.weapon_count==2&&loadout.weapons[0].rounds_loaded[0]==5&&
   loadout.weapons[1].rounds_reserve[0]==51,"native preparation refund changed equipment");
 } else if(!strcmp(case_name,"native-loss")) {
  CHECK(session.record.phase==_gear_idle&&!session.record.count&&!session.record.reserved_count&&
   !session.record.grenades[0]&&!session.record.grenades[1],"native process death refunded issued gear");
 } else if (!strncmp(case_name,"leave-stash-",12)) {
  struct gear_record original;unsigned long generation;
  earn(2);original=session.record;
  CHECK(gear_loadout(&session,&loadout,FALSE)&&!loadout.weapon_count&&!loadout.grenades[0]&&!loadout.grenades[1],"starter preview included stored gear");
  if(!strcmp(case_name,"leave-stash-existing")) {
   CHECK(gear_reserve(&session,TRUE)&&gear_reserve(&session,FALSE)&&gear_loadout(&session,&loadout,FALSE)&&
    loadout.weapon_count==2&&session.record.reserved_count==2&&!session.record.count,"changed gesture replaced an existing reservation");
  } else {
   if(!strcmp(case_name,"leave-stash-pending"))publish_failure=TRUE;
   if(publish_failure) {
    CHECK(!gear_reserve(&session,FALSE)&&session.pending&&session.record.phase==_gear_reserved,"uncertain starter reservation lost intent");
    generation=session.record.generation;publish_failure=FALSE;
    CHECK(gear_reserve(&session,TRUE)&&session.record.generation==generation,"retry reserved saved gear after changing gesture");
   } else CHECK(gear_reserve(&session,FALSE),"starter reservation failed");
   CHECK(session.record.count==2&&!session.record.reserved_count&&!session.record.reserved_grenades[0]&&!session.record.reserved_grenades[1]&&
    !memcmp(original.items,session.record.items,2*sizeof(original.items[0]))&&!memcmp(original.grenades,session.record.grenades,sizeof(original.grenades)),"starter deployment moved stored gear into risk");
   CHECK(gear_loadout(&session,&loadout,TRUE)&&!loadout.weapon_count&&!loadout.grenades[0]&&gear_confirm_deployment(&session),"prepared starter reservation changed on grant");
   if(!strcmp(case_name,"leave-stash-loss")||!strcmp(case_name,"leave-stash-pending")) {
    CHECK(gear_lose(&session),"starter death did not settle");
    gear_close(&session);CHECK(gear_open(&session,owner)&&session.record.count==2&&
     !memcmp(original.items,session.record.items,2*sizeof(original.items[0]))&&!memcmp(original.grenades,session.record.grenades,sizeof(original.grenades)),"failed starter run lost stored gear");
   } else {
    objects[0]=0x20001;objects[1]=0x20002;capture(2);
    if(!strcmp(case_name,"leave-stash-escrow"))publish_failure=TRUE;
    if(publish_failure)CHECK(!gear_prepare_escape(&session,&inventory,objects)&&session.record.phase==_gear_escrow,"published starter escape lost escrow");
    else CHECK(gear_prepare_escape(&session,&inventory,objects)&&gear_finish_escape(&session),"starter loot could not bank");
    gear_close(&session);publish_failure=FALSE;CHECK(gear_open(&session,owner)&&session.record.count==4,"starter loot did not grow stash on reopen");
    CHECK(!memcmp(original.items,session.record.items+2,2*sizeof(original.items[0]))&&session.record.grenades[0]==4&&session.record.grenades[1]==2,"starter rewards changed the protected stash");
    CHECK(memcmp(session.record.items[0].id,session.record.items[2].id,GEAR_ID_BYTES)&&memcmp(session.record.items[1].id,session.record.items[3].id,GEAR_ID_BYTES),"starter rewards duplicated stored identities");
    generation=session.record.generation;gear_close(&session);CHECK(gear_open(&session,owner)&&session.record.count==4&&session.record.generation==generation,"reopen rewarded starter loot twice");
   }
  }
 } else if (!strcmp(case_name, "escape-reopen")) {
  CHECK(gear_reserve(&session,TRUE) && gear_confirm_deployment(&session), "deployment failed"); capture(2);
  CHECK(gear_prepare_escape(&session, &inventory, objects), "escrow failed");
  gear_close(&session);
  CHECK(gear_open(&reopened, owner) && reopened.record.count == 2, "saved escape not recovered");
  CHECK(gear_finish_escape(&reopened) && reopened.record.count == 2, "replayed settlement duplicated gear");
  CHECK(gear_reserve(&reopened,TRUE) && !reopened.record.count, "loadout was not reserved");
  CHECK(gear_loadout(&reopened, &loadout,TRUE) && loadout.weapon_count == 2 && loadout.weapons[0].rounds_loaded[0] == 5 &&
   loadout.weapons[1].rounds_reserve[0] == 51 && loadout.weapons[0].age == .25f, "native equipment changed on reopen");
  CHECK(gear_cancel_reservation(&reopened) && reopened.record.count == 2, "failed grant did not return reserved gear");
 } else if (!strcmp(case_name,"secondary-pairs")) {
  boolean seen[4][4]={{FALSE}};short first,second,left,right,index;
  struct gear_record original;
  objects[3]=0x10004;earn(4);original=session.record;
  for(first=0;first<4;first++) {
   for(second=0;second<3;second++) {
    CHECK(gear_loadout(&session,&loadout,TRUE)&&loadout.weapon_count==2,"selected pair unavailable");
    left=loadout.weapons[0].rounds_loaded[0]-5;right=loadout.weapons[1].rounds_loaded[0]-5;
    CHECK(left>=0&&left<4&&right>=0&&right<4&&left!=right&&!seen[left][right],"loadout chooser repeats or duplicates an ordered pair");
    seen[left][right]=TRUE;
    CHECK(gear_cycle_loadout(&session,1),"secondary choice refused");
    CHECK(gear_loadout(&session,&loadout,TRUE)&&loadout.weapons[0].rounds_loaded[0]==left+5,"secondary choice changed primary");
   }
   CHECK(gear_cycle_loadout(&session,0),"primary choice refused");
  }
  for(first=0;first<4;first++)for(second=0;second<4;second++)
   CHECK(first==second||seen[first][second],"a valid owned pair cannot be selected");
  CHECK(session.record.count==4&&!memcmp(session.record.grenades,original.grenades,sizeof(original.grenades)),"choice changed ownership/capacity or grenades");
  for(first=0;first<4;first++) {
   for(index=0;index<4;index++)if(!memcmp(original.items[first].id,session.record.items[index].id,GEAR_ID_BYTES))break;
   CHECK(index<4&&!memcmp(&original.items[first],&session.record.items[index],sizeof(original.items[first])),"choice modified an owned item");
  }
 } else if (!strcmp(case_name,"secondary-failure")) {
  unsigned long generation;
  earn(3);generation=session.record.generation;fail_write=TRUE;
  CHECK(!gear_cycle_loadout(&session,1)&&session.record.generation==generation,"failed secondary choice changed saved order");
  fail_write=FALSE;publish_failure=TRUE;
  CHECK(!gear_cycle_loadout(&session,1)&&session.pending,"published secondary choice lost pending intent");
  generation=session.record.generation;publish_failure=FALSE;
  CHECK(gear_cycle_loadout(&session,0)&&!session.pending&&session.record.generation==generation,"changed modifier advanced an uncertain choice twice");
  CHECK(gear_loadout(&session,&loadout,TRUE)&&loadout.weapons[0].rounds_loaded[0]==5&&loadout.weapons[1].rounds_loaded[0]==7,"retry lost selected secondary");
 } else if (!strcmp(case_name,"secondary-cache")) {
  struct gear_record mixed;
  objects[3]=0x10004;earn(4);mixed=session.record;strcpy(mixed.items[1].map_name,"other@ce");mixed.items[1].map_checksum++;
  CHECK(gear_commit(&session,&mixed)&&gear_cycle_loadout(&session,1),"mixed-cache secondary selection refused");
  CHECK(gear_loadout(&session,&loadout,TRUE)&&loadout.weapons[0].rounds_loaded[0]==5&&loadout.weapons[1].rounds_loaded[0]==8,"secondary selection included incompatible cache or changed primary");
  CHECK(!memcmp(&mixed.items[1],&session.record.items[1],sizeof(mixed.items[1])),"secondary selection moved incompatible stash");
 } else if (!strcmp(case_name,"secondary-state")) {
  unsigned long generation;
  earn(2);generation=session.record.generation;
  CHECK(!gear_cycle_loadout(&session,-1)&&!gear_cycle_loadout(&session,2),"invalid loadout slot admitted");
  CHECK(gear_cycle_loadout(&session,1)&&session.record.generation==generation,"single secondary choice wrote a redundant save");
  client=TRUE;CHECK(!gear_cycle_loadout(&session,1),"client changed authoritative secondary");client=FALSE;
  CHECK(gear_reserve(&session,TRUE)&&!gear_cycle_loadout(&session,1),"reserved gear could be reordered");
 } else if (!strcmp(case_name,"secondary-recovery")) {
  byte first[GEAR_ID_BYTES],second[GEAR_ID_BYTES];
  earn(3);memcpy(first,session.record.items[0].id,sizeof(first));memcpy(second,session.record.items[2].id,sizeof(second));
  CHECK(gear_cycle_loadout(&session,1),"secondary selection refused");
  gear_close(&session);CHECK(gear_open(&session,owner)&&gear_reserve(&session,TRUE),"saved secondary did not survive reopen");
  CHECK(!memcmp(first,session.record.reserved[0].id,sizeof(first))&&!memcmp(second,session.record.reserved[1].id,sizeof(second)),"reservation chose different owned items");
  CHECK(gear_cancel_reservation(&session)&&gear_reserve(&session,TRUE)&&gear_loadout(&session,&inventory,TRUE)&&gear_confirm_deployment(&session),"refund lost secondary selection");
  objects[0]=0x20001;objects[1]=0x20002;gear_track_reserved(&session,objects);inventory.weapons[1].rounds_loaded[0]=3;
  CHECK(gear_prepare_escape(&session,&inventory,objects)&&gear_finish_escape(&session),"selected pair could not bank");
  gear_close(&session);CHECK(gear_open(&session,owner)&&gear_loadout(&session,&loadout,TRUE)&&loadout.weapons[0].rounds_loaded[0]==5&&loadout.weapons[1].rounds_loaded[0]==3,"recovery/reopen lost primary or secondary ammunition");
  CHECK(session.record.count==3&&!memcmp(first,session.record.items[0].id,sizeof(first))&&!memcmp(second,session.record.items[1].id,sizeof(second)),"recovery duplicated or lost selected identities");
 } else if (!strcmp(case_name, "choice-reopen")) {
  earn(3); CHECK(gear_cycle_loadout(&session,0), "loadout cycle failed");
  CHECK(gear_loadout(&session,&loadout,TRUE) && loadout.weapon_count==2 && loadout.weapons[0].rounds_loaded[0]==6 &&
   loadout.weapons[1].rounds_loaded[0]==7, "chosen pair did not preserve the existing native selection order");
  gear_close(&session); CHECK(gear_open(&session,owner),"chosen loadout did not reopen");
  CHECK(gear_loadout(&session,&loadout,TRUE) && loadout.weapons[0].rounds_loaded[0]==6,"reconnect lost chosen primary");
  CHECK(gear_cycle_loadout(&session,0) && gear_loadout(&session,&loadout,TRUE) && loadout.weapons[0].rounds_loaded[0]==7,
   "owned same-tag weapon identities could not be selected independently");
 } else if (!strcmp(case_name, "choice-failure")) {
  unsigned long generation;
  earn(3); generation=session.record.generation; fail_write=TRUE;
  CHECK(!gear_cycle_loadout(&session,0) && session.record.generation==generation,"failed choice advanced the record");
  fail_write=FALSE; publish_failure=TRUE;
  CHECK(!gear_cycle_loadout(&session,0) && session.pending,"uncertain published choice forgotten");
  generation=session.record.generation; publish_failure=FALSE;
  CHECK(gear_cycle_loadout(&session,0) && !session.pending && session.record.generation==generation,
   "choice retry cycled again instead of confirming its published intent");
  CHECK(gear_loadout(&session,&loadout,TRUE) && loadout.weapons[0].rounds_loaded[0]==6,"choice retry selected another item");
 } else if (!strcmp(case_name, "choice-cancel")) {
  earn(3); CHECK(gear_cycle_loadout(&session,0) && gear_reserve(&session,TRUE),"chosen deployment failed");
  CHECK(!gear_cycle_loadout(&session,0),"deployed player changed stash choice");
  CHECK(gear_cancel_reservation(&session),"failed grant refund failed");
  gear_close(&session);CHECK(gear_open(&session,owner) && gear_loadout(&session,&loadout,TRUE) &&
   loadout.weapons[0].rounds_loaded[0]==6 && loadout.weapons[1].rounds_loaded[0]==7,
   "failed grant silently switched the saved pair");
 } else if (!strcmp(case_name, "choice-recovery")) {
  earn(3); CHECK(gear_cycle_loadout(&session,0) && gear_reserve(&session,TRUE) && gear_loadout(&session,&inventory,TRUE),"chosen deployment failed");
  CHECK(gear_confirm_deployment(&session),"chosen native grant was not confirmed");
  objects[0]=0x20001;objects[1]=0x20002;gear_track_reserved(&session,objects);
  inventory.weapons[0].rounds_loaded[0]=2;
  CHECK(gear_prepare_escape(&session,&inventory,objects) && gear_finish_escape(&session),"chosen return failed");
  gear_close(&session);CHECK(gear_open(&session,owner) && gear_loadout(&session,&loadout,TRUE) && loadout.weapon_count==2 &&
   loadout.weapons[0].rounds_loaded[0]==2 && loadout.weapons[1].rounds_loaded[0]==7,
   "recovered primary/ammo were not retained for redeployment");
 } else if (!strcmp(case_name, "choice-cache")) {
  unsigned long generation;
  struct gear_record mixed;
  earn(3);mixed=session.record;
  strcpy(mixed.items[1].map_name,"other@ce");mixed.items[1].map_checksum=checksum+1;
  mixed.items[3]=mixed.items[0];mixed.items[3].id[0]^=0x80;
  strcpy(mixed.items[3].map_name,"other@ce");mixed.items[3].map_checksum=checksum+1;mixed.count=4;
  CHECK(gear_commit(&session,&mixed) && gear_cycle_loadout(&session,0),"mixed-cache choice failed");
  map="other@ce";checksum++;
  CHECK(gear_cycle_loadout(&session,0) && gear_loadout(&session,&loadout,TRUE) && loadout.weapons[0].rounds_loaded[0]==5,
   "second cache could not choose its own primary");
  map="candidate@ce";checksum--;
  CHECK(gear_loadout(&session,&loadout,TRUE) && loadout.weapons[0].rounds_loaded[0]==7,"another cache changed the saved primary");
  generation=session.record.generation;checksum+=2;
  CHECK(gear_cycle_loadout(&session,0) && session.record.generation==generation,"unrelated cache moved the saved choice");
  checksum-=2;client=TRUE;
  CHECK(!gear_cycle_loadout(&session,0) && session.record.generation==generation,"client changed durable gear choice");client=FALSE;
 } else if (!strcmp(case_name, "discard")) {
  struct gear_record full;
  byte id[GEAR_ID_BYTES];unsigned long generation;short i;
  earn(3);full=session.record;
  for(i=0;i<GEAR_CAPACITY;i++){full.items[i]=session.record.items[0];full.items[i].id[0]=(byte)(i+1);}
  full.count=GEAR_CAPACITY;CHECK(gear_commit(&session,&full),"full stash setup failed");
  CHECK(gear_loadout_choice(&session)!=NULL,"stored choice missing");
  memcpy(id,gear_loadout_choice(&session)->id,sizeof(id));generation=session.record.generation;
  client=TRUE;CHECK(!gear_discard(&session,id,generation),"client discarded owned gear");client=FALSE;
  CHECK(!gear_discard(&session,id,generation-1) && session.record.count==GEAR_CAPACITY,"stale preview discarded gear");
  checksum++;CHECK(!gear_loadout_choice(&session),"unavailable stored gear became deployable");checksum--;
  CHECK(gear_discard(&session,id,generation) && session.record.count==GEAR_CAPACITY-1,"confirmed discard did not free exactly one slot");
  CHECK(gear_discard(&session,id,generation) && session.record.count==GEAR_CAPACITY-1 && session.record.generation==generation+1,
   "discard retry removed another stored weapon");
  gear_close(&session);CHECK(gear_open(&session,owner) && session.record.count==GEAR_CAPACITY-1 &&
   session.record.grenades[0]==2 && session.record.grenades[1]==1,"discard changed grenades or failed to persist");
 } else if (!strcmp(case_name, "discard-unavailable")) {
  byte id[GEAR_ID_BYTES],kept[GEAR_ID_BYTES];unsigned long generation;
  earn(3);memcpy(id,session.record.items[0].id,sizeof(id));memcpy(kept,session.record.items[1].id,sizeof(kept));generation=session.record.generation;
  checksum++;
  CHECK(!gear_loadout_choice(&session)&&gear_loadout(&session,&inventory,TRUE)&&!inventory.weapon_count,"unavailable items were admitted as a loadout");
  CHECK(gear_discard(&session,id,generation)&&session.record.count==2,"confirmed owner could not discard an unavailable stored item");
  CHECK(gear_discard(&session,id,generation)&&session.record.count==2,"unavailable discard retry removed another item");
  gear_close(&session);CHECK(gear_open(&session,owner)&&session.record.count==2&&!memcmp(kept,session.record.items[0].id,sizeof(kept)),"unavailable discard failed to preserve remaining stored gear");
 } else if (!strcmp(case_name, "discard-uncertain")) {
  byte id[GEAR_ID_BYTES];unsigned long generation;
  earn(3);memcpy(id,gear_loadout_choice(&session)->id,sizeof(id));generation=session.record.generation;
  fail_write=TRUE;CHECK(!gear_discard(&session,id,generation) && session.record.count==3,"failed write lost gear");
  fail_write=FALSE;publish_failure=TRUE;
  CHECK(!gear_discard(&session,id,generation) && session.pending && session.record.count==2,"published deletion was forgotten");
  publish_failure=FALSE;CHECK(gear_discard(&session,id,generation) && !session.pending && session.record.count==2 &&
   session.record.generation==generation+1,"uncertain deletion retried as another discard");
 } else if (!strcmp(case_name, "discard-state")) {
  byte id[GEAR_ID_BYTES];unsigned long generation;
  earn(3);memcpy(id,gear_loadout_choice(&session)->id,sizeof(id));generation=session.record.generation;
  CHECK(gear_cycle_loadout(&session,0) && !gear_discard(&session,id,generation) && session.record.count==3,
   "choice change reused old confirmation");
  CHECK(gear_reserve(&session,TRUE) && !gear_loadout_choice(&session) && !gear_discard(&session,id,session.record.generation),
   "active run admitted stash deletion");
 } else if (!strcmp(case_name, "loss")) {
  earn(3);
  CHECK(gear_reserve(&session,TRUE) && session.record.count == 1 && session.record.reserved_count == 2, "wrong risk reservation");
  CHECK(gear_confirm_deployment(&session),"issued loadout was not recorded");
  CHECK(gear_lose(&session) && session.record.phase == _gear_idle && session.record.count == 1 && !session.record.reserved_count,
   "failed run awarded or restored at-risk gear");
  gear_close(&session); CHECK(gear_open(&session, owner) && session.record.count == 1, "loss damaged the rest of the stash");
  CHECK(gear_reserve(&session,TRUE) && gear_confirm_deployment(&session), "next run failed"); gear_close(&session);
  CHECK(gear_open(&session, owner) && session.record.count == 0 && session.record.phase == _gear_idle, "abandoned run refunded gear");
 } else if (!strcmp(case_name, "uncertain")) {
  CHECK(gear_reserve(&session,TRUE) && gear_confirm_deployment(&session), "deployment failed"); capture(2);
  fail_write = TRUE;
  CHECK(!gear_prepare_escape(&session, &inventory, objects) && session.record.phase == _gear_deployed, "failed write changed ownership");
  fail_write = FALSE; publish_failure = TRUE;
  CHECK(!gear_prepare_escape(&session, &inventory, objects) && session.pending && session.record.phase == _gear_escrow,
   "published intent was forgotten after uncertain commit");
  CHECK(!gear_finish_escape(&session) && !session.record.count, "pending durability awarded gear");
  publish_failure = FALSE;
  CHECK(gear_finish_escape(&session) && session.record.count == 2, "pending settlement could not recover");
  CHECK(gear_finish_escape(&session) && session.record.count == 2, "retry awarded twice");
 } else if (!strcmp(case_name,"reserve-reopen")) {
  byte ids[2][GEAR_ID_BYTES];unsigned long generation;short i;
  earn(2);for(i=0;i<2;i++)memcpy(ids[i],session.record.items[i].id,GEAR_ID_BYTES);
  session.grenade_room[0]=session.grenade_room[1]=4;
  CHECK(gear_reserve(&session,TRUE)&&session.record.phase==_gear_reserved,"preparation was marked issued");
  generation=session.record.generation;
  CHECK(gear_reserve(&session,TRUE)&&session.record.generation==generation,"preparation retry reserved twice");
  capture(2);CHECK(!gear_prepare_escape(&session,&inventory,objects),"unissued preparation earned escape rewards");
  fail_write=TRUE;CHECK(!gear_cancel_reservation(&session),"fixture outage did not block refund");gear_close(&session);
  CHECK(!gear_open(&session,owner),"unwritable refund opened an unsafe stash");
  fail_write=FALSE;CHECK(gear_open(&session,owner)&&session.record.count==2&&!session.record.reserved_count&&
   session.record.grenades[0]==2&&session.record.grenades[1]==1,"unissued crash reservation lost owned gear");
  for(i=0;i<2;i++)CHECK(!memcmp(ids[i],session.record.items[i].id,GEAR_ID_BYTES),"refund changed item identity");
 } else if (!strcmp(case_name,"confirm-failure")) {
  earn(2);CHECK(gear_reserve(&session,TRUE),"preparation failed");fail_write=TRUE;
  CHECK(!gear_confirm_deployment(&session)&&session.record.phase==_gear_reserved,"failed confirmation became issued");
  fail_write=FALSE;CHECK(gear_confirm_deployment(&session)&&session.record.phase==_gear_deployed,"native grant could not confirm");
  CHECK(!gear_cancel_reservation(&session)&&!gear_reserve(&session,TRUE),"issued gear could refund or reserve twice");
  CHECK(gear_lose(&session)&&!session.record.count,"confirmed failed run refunded gear");
 } else if (!strcmp(case_name,"confirm-uncertain")) {
  unsigned long generation;
  earn(2);CHECK(gear_reserve(&session,TRUE),"preparation failed");publish_failure=TRUE;
  CHECK(gear_confirm_deployment(&session)&&session.pending&&session.record.phase==_gear_deployed,
   "published native grant was revoked after a flush error");
  generation=session.record.generation;
  CHECK(gear_confirm_deployment(&session)&&session.record.generation==generation,
   "published confirmation retry revoked gear or committed twice");
  client=TRUE;CHECK(!gear_confirm_deployment(&session),"client confirmed a host-owned deployment");client=FALSE;
  gear_close(&session);publish_failure=FALSE;
  CHECK(gear_open(&session,owner)&&!session.record.count&&!session.record.reserved_count,"issued crash reservation was refunded");
 } else if (!strcmp(case_name, "corrupt")) {
  earn(2); size = gear_encode(&session.record, data);
  CHECK(size == GEAR_HEADER_BYTES + 2 * GEAR_ITEM_BYTES + 32 && gear_decode(data, size, owner, &decoded), "codec layout mismatch");
  gear_account_owner(owner, other); CHECK(memcmp(owner, other, GEAR_ID_BYTES), "local and remote owner domains overlap");
  CHECK(!gear_decode(data, size, other, &decoded), "wrong owner accepted");
  gear_close(&session); files[0].data[100] ^= 1;
  CHECK(!gear_open(&session, owner), "corrupted stash overwritten or accepted");
  CHECK(files[0].data[100] == (byte)(data[100] ^ 1), "corrupt source was overwritten");
 } else if (!strcmp(case_name, "shared-cache")) {
  earn(2);checksum++;shared_cache=TRUE;native_compatible=FALSE;
  CHECK(gear_loadout(&session,&loadout,TRUE)&&!loadout.weapon_count&&!gear_loadout_choice(&session),
   "shared cache bypassed native admission");
  native_compatible=TRUE;
  CHECK(gear_loadout(&session,&loadout,TRUE)&&loadout.weapon_count==2&&loadout.map_checksum==checksum&&
   loadout.weapons[0].rounds_loaded[0]==5&&session.record.items[0].map_checksum==checksum-1,
   "shared loadout lost native quantities or overwrote origin provenance");
  CHECK(gear_cycle_loadout(&session,0)&&gear_reserve(&session,TRUE)&&gear_loadout(&session,&loadout,TRUE)&&
   loadout.weapons[0].rounds_loaded[0]==6&&!session.record.count&&session.record.reserved_count==2,
   "shared selection/reservation did not use the selected items");
  CHECK(gear_cancel_reservation(&session)&&session.record.count==2&&session.record.items[0].map_checksum==checksum-1,
   "shared grant cancellation changed stored origin or lost gear");
 } else if (!strcmp(case_name, "cache")) {
  earn(2); checksum++;
  CHECK(gear_loadout(&session, &loadout,TRUE) && !loadout.weapon_count, "different cache admitted recovered tags");
  CHECK(gear_reserve(&session,TRUE) && session.record.count == 2 && !session.record.reserved_count, "incompatible stash was consumed");
  CHECK(gear_lose(&session) && session.record.count == 2, "different-cache run lost banked gear");
  checksum--; CHECK(gear_loadout(&session, &loadout,TRUE) && loadout.weapon_count == 2, "same-cache cross-mode gear unavailable");
 } else return 2;
 gear_close(&session); gear_close(&reopened);
 return 0;
}
#endif
