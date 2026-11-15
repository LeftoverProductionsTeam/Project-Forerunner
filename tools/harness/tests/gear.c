/* The real gear codec/transactions over a bounded fake filesystem. Native
 * locking, replacement and process death are tested by test_gear_files.py. */
#include "harness.h"
#define MIN(a,b) ((a)<(b)?(a):(b))
#define MAX(a,b) ((a)>(b)?(a):(b))
#define PIN(v,a,b) MIN(MAX(v,a),b)
#define NUMBEROF(a) ((long)(sizeof(a)/sizeof((a)[0])))
#define VALID_INDEX(i,n) ((i)>=0&&(i)<(n))
#define SHORT_MAX 32767
typedef unsigned long posix_ulong;
#include "types.inc"

static struct { char path[1024]; byte data[65536]; posix_ulong size; boolean present, locked; } files[4];
static boolean client, playing = TRUE, fail_write, publish_failure;
static int connection = _game_connection_local, folder_calls, file_count;
static char const *map = "candidate@ce";
static unsigned long checksum = 42;
static boolean game_in_progress(void) { return playing; }
static boolean network_game_distributed_client(void) { return client; }
static int game_connection(void) { return connection; }
static char const *cache_file_loaded_map_name(void) { return map; }
static unsigned long cache_files_get_checksum(void) { return checksum; }
char const *platform_folder(enum port_folder folder) { folder_calls++; return "owned/private"; }
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
#include "sha.inc"
#include "under_test.inc"

#ifndef GEAR_FIXTURE_ONLY
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
 CHECK(gear_reserve(&session), "initial deployment failed");
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
 if (!strcmp(case_name, "escape-reopen")) {
  CHECK(gear_reserve(&session), "deployment failed"); capture(2);
  CHECK(gear_prepare_escape(&session, &inventory, objects), "escrow failed");
  gear_close(&session);
  CHECK(gear_open(&reopened, owner) && reopened.record.count == 2, "saved escape not recovered");
  CHECK(gear_finish_escape(&reopened) && reopened.record.count == 2, "replayed settlement duplicated gear");
  CHECK(gear_reserve(&reopened) && !reopened.record.count, "loadout was not reserved");
  CHECK(gear_loadout(&reopened, &loadout) && loadout.weapon_count == 2 && loadout.weapons[0].rounds_loaded[0] == 5 &&
   loadout.weapons[1].rounds_reserve[0] == 51 && loadout.weapons[0].age == .25f, "native equipment changed on reopen");
  CHECK(gear_cancel_reservation(&reopened) && reopened.record.count == 2, "failed grant did not return reserved gear");
 } else if (!strcmp(case_name, "choice-reopen")) {
  earn(3); CHECK(gear_cycle_loadout(&session), "loadout cycle failed");
  CHECK(gear_loadout(&session,&loadout) && loadout.weapon_count==2 && loadout.weapons[0].rounds_loaded[0]==6 &&
   loadout.weapons[1].rounds_loaded[0]==7, "chosen pair did not preserve the existing native selection order");
  gear_close(&session); CHECK(gear_open(&session,owner),"chosen loadout did not reopen");
  CHECK(gear_loadout(&session,&loadout) && loadout.weapons[0].rounds_loaded[0]==6,"reconnect lost chosen primary");
  CHECK(gear_cycle_loadout(&session) && gear_loadout(&session,&loadout) && loadout.weapons[0].rounds_loaded[0]==7,
   "owned same-tag weapon identities could not be selected independently");
 } else if (!strcmp(case_name, "choice-failure")) {
  unsigned long generation;
  earn(3); generation=session.record.generation; fail_write=TRUE;
  CHECK(!gear_cycle_loadout(&session) && session.record.generation==generation,"failed choice advanced the record");
  fail_write=FALSE; publish_failure=TRUE;
  CHECK(!gear_cycle_loadout(&session) && session.pending,"uncertain published choice forgotten");
  generation=session.record.generation; publish_failure=FALSE;
  CHECK(gear_cycle_loadout(&session) && !session.pending && session.record.generation==generation,
   "choice retry cycled again instead of confirming its published intent");
  CHECK(gear_loadout(&session,&loadout) && loadout.weapons[0].rounds_loaded[0]==6,"choice retry selected another item");
 } else if (!strcmp(case_name, "choice-cancel")) {
  earn(3); CHECK(gear_cycle_loadout(&session) && gear_reserve(&session),"chosen deployment failed");
  CHECK(!gear_cycle_loadout(&session),"deployed player changed stash choice");
  CHECK(gear_cancel_reservation(&session),"failed grant refund failed");
  gear_close(&session);CHECK(gear_open(&session,owner) && gear_loadout(&session,&loadout) &&
   loadout.weapons[0].rounds_loaded[0]==6 && loadout.weapons[1].rounds_loaded[0]==7,
   "failed grant silently switched the saved pair");
 } else if (!strcmp(case_name, "choice-recovery")) {
  earn(3); CHECK(gear_cycle_loadout(&session) && gear_reserve(&session) && gear_loadout(&session,&inventory),"chosen deployment failed");
  objects[0]=0x20001;objects[1]=0x20002;gear_track_reserved(&session,objects);
  inventory.weapons[0].rounds_loaded[0]=2;
  CHECK(gear_prepare_escape(&session,&inventory,objects) && gear_finish_escape(&session),"chosen return failed");
  gear_close(&session);CHECK(gear_open(&session,owner) && gear_loadout(&session,&loadout) && loadout.weapon_count==2 &&
   loadout.weapons[0].rounds_loaded[0]==2 && loadout.weapons[1].rounds_loaded[0]==7,
   "recovered primary/ammo were not retained for redeployment");
 } else if (!strcmp(case_name, "choice-cache")) {
  unsigned long generation;
  struct gear_record mixed;
  earn(3);mixed=session.record;
  strcpy(mixed.items[1].map_name,"other@ce");mixed.items[1].map_checksum=checksum+1;
  mixed.items[3]=mixed.items[0];mixed.items[3].id[0]^=0x80;
  strcpy(mixed.items[3].map_name,"other@ce");mixed.items[3].map_checksum=checksum+1;mixed.count=4;
  CHECK(gear_commit(&session,&mixed) && gear_cycle_loadout(&session),"mixed-cache choice failed");
  map="other@ce";checksum++;
  CHECK(gear_cycle_loadout(&session) && gear_loadout(&session,&loadout) && loadout.weapons[0].rounds_loaded[0]==5,
   "second cache could not choose its own primary");
  map="candidate@ce";checksum--;
  CHECK(gear_loadout(&session,&loadout) && loadout.weapons[0].rounds_loaded[0]==7,"another cache changed the saved primary");
  generation=session.record.generation;checksum+=2;
  CHECK(gear_cycle_loadout(&session) && session.record.generation==generation,"unrelated cache moved the saved choice");
  checksum-=2;client=TRUE;
  CHECK(!gear_cycle_loadout(&session) && session.record.generation==generation,"client changed durable gear choice");client=FALSE;
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
  checksum++;CHECK(!gear_discard(&session,id,generation),"wrong cache discarded owned gear");checksum--;
  CHECK(gear_discard(&session,id,generation) && session.record.count==GEAR_CAPACITY-1,"confirmed discard did not free exactly one slot");
  CHECK(gear_discard(&session,id,generation) && session.record.count==GEAR_CAPACITY-1 && session.record.generation==generation+1,
   "discard retry removed another stored weapon");
  gear_close(&session);CHECK(gear_open(&session,owner) && session.record.count==GEAR_CAPACITY-1 &&
   session.record.grenades[0]==2 && session.record.grenades[1]==1,"discard changed grenades or failed to persist");
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
  CHECK(gear_cycle_loadout(&session) && !gear_discard(&session,id,generation) && session.record.count==3,
   "choice change reused old confirmation");
  CHECK(gear_reserve(&session) && !gear_loadout_choice(&session) && !gear_discard(&session,id,session.record.generation),
   "active run admitted stash deletion");
 } else if (!strcmp(case_name, "loss")) {
  earn(3);
  CHECK(gear_reserve(&session) && session.record.count == 1 && session.record.reserved_count == 2, "wrong risk reservation");
  CHECK(gear_lose(&session) && session.record.phase == _gear_idle && session.record.count == 1 && !session.record.reserved_count,
   "failed run awarded or restored at-risk gear");
  gear_close(&session); CHECK(gear_open(&session, owner) && session.record.count == 1, "loss damaged the rest of the stash");
  CHECK(gear_reserve(&session), "next run failed"); gear_close(&session);
  CHECK(gear_open(&session, owner) && session.record.count == 0 && session.record.phase == _gear_idle, "abandoned run refunded gear");
 } else if (!strcmp(case_name, "uncertain")) {
  CHECK(gear_reserve(&session), "deployment failed"); capture(2);
  fail_write = TRUE;
  CHECK(!gear_prepare_escape(&session, &inventory, objects) && session.record.phase == _gear_deployed, "failed write changed ownership");
  fail_write = FALSE; publish_failure = TRUE;
  CHECK(!gear_prepare_escape(&session, &inventory, objects) && session.pending && session.record.phase == _gear_escrow,
   "published intent was forgotten after uncertain commit");
  CHECK(!gear_finish_escape(&session) && !session.record.count, "pending durability awarded gear");
  publish_failure = FALSE;
  CHECK(gear_finish_escape(&session) && session.record.count == 2, "pending settlement could not recover");
  CHECK(gear_finish_escape(&session) && session.record.count == 2, "retry awarded twice");
 } else if (!strcmp(case_name, "corrupt")) {
  earn(2); size = gear_encode(&session.record, data);
  CHECK(size == GEAR_HEADER_BYTES + 2 * GEAR_ITEM_BYTES + 32 && gear_decode(data, size, owner, &decoded), "codec layout mismatch");
  gear_account_owner(owner, other); CHECK(memcmp(owner, other, GEAR_ID_BYTES), "local and remote owner domains overlap");
  CHECK(!gear_decode(data, size, other, &decoded), "wrong owner accepted");
  gear_close(&session); files[0].data[100] ^= 1;
  CHECK(!gear_open(&session, owner), "corrupted stash overwritten or accepted");
  CHECK(files[0].data[100] == (byte)(data[100] ^ 1), "corrupt source was overwritten");
 } else if (!strcmp(case_name, "cache")) {
  earn(2); checksum++;
  CHECK(gear_loadout(&session, &loadout) && !loadout.weapon_count, "different cache admitted recovered tags");
  CHECK(gear_reserve(&session) && session.record.count == 2 && !session.record.reserved_count, "incompatible stash was consumed");
  CHECK(gear_lose(&session) && session.record.count == 2, "different-cache run lost banked gear");
  checksum--; CHECK(gear_loadout(&session, &loadout) && loadout.weapon_count == 2, "same-cache cross-mode gear unavailable");
 } else return 2;
 gear_close(&session); gear_close(&reopened);
 return 0;
}
#endif
