/* Real destination resolvers with a small catalog; no retail maps needed. */
#include "harness.h"
#ifndef _WIN32
#include <strings.h>
#define _stricmp strcasecmp
#define _strnicmp strncasecmp
#endif
#define csstrncpy strncpy
#define csmemcpy memcpy
#define HALO_CUSTOM_EDITION 1
enum { _map_family_xbox, _map_family_custom_edition };
static char main_script_next_level[64];
static int mcc_lookups, ce_lookups;
static char const *catalog_level = "mcc_maps\\campaign\\next";
static boolean catalog_campaign = TRUE;
static boolean mcc_level_name(char const *name) { return name && !strncmp(name, "mcc_maps\\", 9); }
static short map_family_parse(char const *name, char *file, long size) {
    (void)file; (void)size;
    return !strcmp(name, "start@ce") ? _map_family_custom_edition : _map_family_xbox;
}
static short map_family_from_wire_name(char const *name, char *file, long size) {
    (void)file; (void)size;
    return !strncmp(name, "custom_maps\\", 12) ? _map_family_custom_edition : _map_family_xbox;
}
static short mcc_maps_find(char const *name) { mcc_lookups++; return strcmp(name, catalog_level) ? NONE : 0; }
static boolean mcc_maps_campaign(short index) { (void)index; return catalog_campaign; }
static char const *mcc_maps_level_name(short index) { (void)index; return catalog_level; }
static boolean map_family_campaign(char const *name) {
    ce_lookups++; return !strcmp(name, "next@ce");
}
static boolean stubbs_mode_active(void) { return FALSE; }
static short stubbs_campaign_level_from_name(char const *name, short fallback) {
    (void)name; return fallback;
}
static char const *tag_name_strip_path(char const *name) {
    char const *slash = strrchr(name, '\\');
    return slash ? slash + 1 : name;
}
#include "under_test.inc"

int main(int argc, char **argv)
{
    char next[64] = "unchanged", long_name[66];
    char const *name, *current = "mcc_maps\\campaign\\start", *requested = "next";
    size_t size = sizeof(next);
    boolean expected = TRUE;
    CHECK(argc == 2, "case required");
    name = argv[1];
    if (!strcmp(name, "stock-credit")) {
        CHECK(main_get_solo_level_from_name("a10") == 0, "stock lobby name");
        CHECK(main_get_solo_level_from_name("levels\\A30\\A30") == 1, "stock scenario path");
        CHECK(main_get_solo_level_from_name("maps/d40.MAP") == 9, "stock cache path");
        CHECK(main_get_solo_level_from_name("a100") == NONE, "similar name earned stock credit");
        CHECK(main_get_solo_level_from_name("levels\\a10\\operation") == NONE, "parent folder earned stock credit");
        CHECK(main_get_solo_level_from_name("a10@ce") == NONE, "CE alias earned stock credit");
        CHECK(main_get_solo_level_from_name("custom_maps\\a10.map") == NONE, "CE wire name earned stock credit");
        CHECK(main_get_solo_level_from_name("mcc_maps\\campaign\\a10") == NONE, "MCC bypassed its own progress");
        CHECK(main_get_solo_level_from_name(NULL) == NONE, "missing name resolved");
        return 0;
    }
    if (!strcmp(name, "mcc")) {}
    else if (!strcmp(name, "mcc-path")) requested = "levels/next.MAP";
    else if (!strcmp(name, "mcc-foreign-path")) {
        requested = "mcc_maps\\other\\next.map";
        /* Paths in script strings cannot select another package. */
    } else if (!strcmp(name, "mcc-missing")) { requested = "absent"; expected = FALSE; }
    else if (!strcmp(name, "mcc-multiplayer")) { catalog_campaign = FALSE; expected = FALSE; }
    else if (!strcmp(name, "ce")) { current = "custom_maps\\start"; requested = "levels\\next.map"; }
    else if (!strcmp(name, "ce-missing")) { current = "custom_maps\\start"; requested = "absent"; expected = FALSE; }
    else if (!strcmp(name, "xbox")) { current = "levels\\a10\\a10"; expected = FALSE; }
    else if (!strcmp(name, "empty")) { requested = "levels/"; expected = FALSE; }
    else if (!strcmp(name, "long")) {
        memset(long_name, 'a', sizeof(long_name) - 1); long_name[sizeof(long_name) - 1] = 0;
        requested = long_name; expected = FALSE;
    } else if (!strcmp(name, "small-buffer")) { size = strlen(catalog_level); expected = FALSE; }
    else if (!strcmp(name, "transitions")) {
        CHECK(main_script_level_resolve(current, "next", next, sizeof(next)), "MCC resolves");
        CHECK(main_script_level_resolve("custom_maps\\start", "next", next, sizeof(next)), "CE resolves after MCC");
        CHECK(!strcmp(next, "next@ce"), "no MCC result leaks into CE");
        CHECK(!main_script_level_resolve("levels\\a10\\a10", "next", next, sizeof(next)), "Xbox unchanged");
        CHECK(main_script_level_resolve(current, "next", next, sizeof(next)), "MCC resolves after Xbox");
        CHECK(mcc_lookups == 2 && ce_lookups == 1, "each catalog owns only its family");
        return 0;
    } else CHECK(FALSE, "unknown case");
    CHECK(main_script_level_resolve(current, requested, next, size) == expected, "destination acceptance");
    if (expected) CHECK(!strcmp(next, mcc_level_name(current) ? catalog_level : "next@ce"), "canonical destination");
    else CHECK(!strcmp(next, "unchanged"), "refusal leaves output alone");
    if (!mcc_level_name(current)) CHECK(!mcc_lookups, "legacy paths never query MCC catalog");
    else CHECK(!ce_lookups, "MCC paths never query CE catalog");
    CHECK(!mcc_main_script_level_resolve(current, requested, next, 0), "zero output capacity refused");
    return 0;
}
