#include "harness.h"

/* Real resupply over native ownership/capacity contracts, without spawning
 * objects or pretending to persist gear. */
#define NUMBEROF(a) ((long)(sizeof(a) / sizeof((a)[0])))
#define MIN(a, b) ((a) < (b) ? (a) : (b))
#define TAG_BLOCK_GET_ELEMENT(b, i, t) (&((t *)(b)->address)[i])
#include "types.inc"

struct tag_block { long count; void *address; };
struct weapon_magazine_definition { short rounds_total_maximum, rounds_loaded_maximum; };
struct weapon_definition { struct { struct tag_block magazines; } weapon; };
struct weapon_datum
{
 struct { long parent_object_index; } object;
 struct { long flags; } item;
 long definition_index;
 struct { struct { short rounds_loaded, rounds_total; } magazines[2]; real age; } weapon;
};
struct unit_datum
{
 struct { long damage_flags; } object;
 struct { long player_index, weapon_object_indices[MAXIMUM_WEAPONS_PER_UNIT]; } unit;
};
struct player_datum { boolean quit_out_of_game; long unit_index; };
struct data_iterator { long datum_index; };
static struct { boolean resupply; } firefight;
static struct player_datum player;
static struct unit_datum unit;
static struct weapon_datum weapon;
static struct weapon_magazine_definition capacity = { 120, 12 };
static struct weapon_definition definition = { { { 1, &capacity } } };
static int notices;
static void *player_data;
static void data_iterator_new(struct data_iterator *iterator, void *data) { iterator->datum_index = 0; }
static void *data_iterator_next(struct data_iterator *iterator) { return ++iterator->datum_index == 1 ? &player : NULL; }
static void *object_try_and_get_and_verify_type(long index, unsigned long mask) { return index == 2 ? &unit : NULL; }
static struct weapon_datum *weapon_try_and_get(long index) { return index == 3 ? &weapon : NULL; }
static struct weapon_definition *weapon_definition_get(long index) { return &definition; }
static void firefight_notice(char const *text) { notices++; }
#include "under_test.inc"

int main(int argc, char **argv)
{
 char const *case_name = argv[1];
 short slot;

 firefight.resupply = TRUE;
 player.unit_index = 2;
 unit.unit.player_index = 1;
 for (slot = 0; slot < MAXIMUM_WEAPONS_PER_UNIT; slot++) unit.unit.weapon_object_indices[slot] = NONE;
 unit.unit.weapon_object_indices[0] = 3;
 weapon.object.parent_object_index = 2;
 weapon.item.flags = FLAG(_item_attached_to_unit_bit);
 weapon.weapon.magazines[0].rounds_loaded = 5;
 weapon.weapon.magazines[0].rounds_total = 7;
 weapon.weapon.age = .5f;
 CASE("owned") {
  firefight_resupply_players();
  CHECK(weapon.weapon.magazines[0].rounds_total == 120 && notices == 1, "owned reserve not replenished");
  CHECK(weapon.weapon.magazines[0].rounds_loaded == 5 && weapon.weapon.age == .5f, "loaded rounds or ballistic age changed");
 } else CASE("transferred") {
  weapon.object.parent_object_index = 9;
  firefight_resupply_players();
  CHECK(weapon.weapon.magazines[0].rounds_total == 7 && !notices, "stale slot resupplied another owner's weapon");
 } else CASE("dropped") {
  weapon.object.parent_object_index = NONE;
  firefight_resupply_players();
  CHECK(weapon.weapon.magazines[0].rounds_total == 7 && !notices, "detached weapon resupplied");
 } else CASE("battery") {
  definition.weapon.magazines.count = 0;
  firefight_resupply_players();
  CHECK(weapon.weapon.age == 0 && notices == 1, "owned battery not restored");
  weapon.weapon.age = .5f;
  weapon.object.parent_object_index = 9;
  firefight_resupply_players();
  CHECK(weapon.weapon.age == .5f && notices == 1, "another owner's battery changed");
 } else CASE("ineligible") {
  player.quit_out_of_game = TRUE;
  firefight_resupply_players();
  player.quit_out_of_game = FALSE;
  unit.object.damage_flags = FLAG(_object_dead_bit);
  firefight_resupply_players();
  unit.object.damage_flags = 0;
  unit.unit.player_index = 8;
  firefight_resupply_players();
  unit.unit.player_index = 1;
  firefight.resupply = FALSE;
  firefight_resupply_players();
  CHECK(weapon.weapon.magazines[0].rounds_total == 7 && !notices, "ineligible resupply mutated gear");
 } else return 2;
 return 0;
}
