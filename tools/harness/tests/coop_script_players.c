#include "harness.h"
#include <ctype.h>
/* Keep the bounded adapter distinct from the Windows CRT declaration. */
#define strlwr fixture_strlwr
typedef unsigned short word;
#define NUMBEROF(a) (sizeof(a) / sizeof((a)[0]))
struct player_datum
{
	long unit_index;
};
struct data_iterator
{
	int slot;
};
static struct player_datum people[128];
static int occupied[128], host = TRUE, calls[128];
static short player_spawn_count=1;
static boolean campaign_coop;
static boolean network_coop_active(void) {return campaign_coop;}
#include "cooperative.inc"
static long ref_head;
static struct data_array *player_data, *object_list_data;
static long data_next_index(struct data_array *data, long previous)
{
	int i;
	for (i = previous + 1; i < 128; i++)
		if (occupied[i])
			return i;
	return NONE;
}
static struct player_datum *player_get(long i)
{
	return &people[i];
}
static void data_iterator_new(struct data_iterator *it, struct data_array *data)
{
	it->slot = NONE;
}
static struct player_datum *data_iterator_next(struct data_iterator *it)
{
	it->slot = data_next_index(player_data, it->slot);
	return it->slot == NONE ? NULL : &people[it->slot];
}
static boolean coop_scripts_host(void)
{
	return host;
}
static boolean adaptations = TRUE;
static boolean extraction = FALSE;
static boolean config_boolean(char const *name)
{
	if (!strcmp(name, "game.extraction"))
		return extraction;
	CHECK(!strcmp(name, "network.coop_script_adaptations"), "unknown policy");
	return adaptations;
}
static void reference_list_port_full(struct data_array *data)
{
	CHECK(FALSE, "fixture full");
}
static void data_delete_all(struct data_array *data)
{
	memset(data->data, 0, data->maximum_count * data->size);
	data->count = 0;
}
#include "reference.inc"
static long object_list_new(void)
{
	ref_head = NONE;
	return 0;
}
static void object_list_add(long list, long unit)
{
	reference_list_add(object_list_data, &ref_head, unit);
}
static void object_list_mark_players(long list) {}
static void object_list_delete(long list)
{
	data_delete_all(object_list_data);
}
#include "followers.inc"
#include "players.inc"
static long player_index_from_unit_index(long unit)
{
	int i;
	for (i = 0; i < 128; i++)
		if (occupied[i] && people[i].unit_index == unit)
			return i;
	return NONE;
}
static boolean coop_scripts_unit_near(long unit, real_point3d const *p)
{
	return TRUE;
}
static boolean player_position_switches_bsp(real_point3d const *p)
{
	return FALSE;
}
static real_point3d *object_get_origin(long unit, real_point3d *p)
{
	memset(p, 0, sizeof(*p));
	return p;
}
static boolean coop_scripts_arrive_at(long player, real_point3d const *p, long const *arrived,
									  short count)
{
	calls[player]++;
	return TRUE;
}
static void unit_scripting_suspended(long unit, boolean value)
{
	calls[player_index_from_unit_index(unit)]++;
}
struct biped_datum
{
	struct
	{
		long parent_object_index;
		unsigned long damage_flags;
		real body_vitality, shield_vitality;
	} object;
	struct { unsigned long flags; } biped;
};
static struct biped_datum objects[128];
#define object_get(unit) (&objects[player_index_from_unit_index(unit)])
static void unit_scripting_exit_vehicle(long unit)
{
	calls[player_index_from_unit_index(unit)]++;
}
static char *strlwr(char *p)
{
	char *s = p;
	while (*s)
	{
		*s = tolower((unsigned char)*s);
		s++;
	}
	return p;
}
static void vehicle_scripting_load_magic(long vehicle, char const *seat, long list)
{
	struct data_reference *r = datum_get(object_list_data, ref_head);
	CHECK(!strcmp(seat, "p-rider"), "seat prefix changed");
	calls[player_index_from_unit_index(r->datum_index)]++;
}
static void player_add_equipment(long unit, short profile, boolean reset)
{
	calls[player_index_from_unit_index(unit)]++;
}
enum { _object_dead_bit=0, _biped_airborne_bit=0 };
static boolean native_safe;
static boolean game_safe_to_save(void) { return native_safe; }
static struct biped_datum *biped_try_and_get(long unit)
{
	long player=player_index_from_unit_index(unit);
	return player==NONE ? NULL : &objects[player];
}
static boolean ai_port_enemies_can_see_unit(long unit) { return FALSE; }
#include "actions.inc"
static struct {boolean set;real maximum_body,maximum_shield;} players_vitality;
static boolean coop_host(void) {return host;}
static void *object_try_and_get(long unit)
{long slot=player_index_from_unit_index(unit);return slot==NONE?NULL:&objects[slot];}
static void object_initialize_vitality(long unit,real const *body,real const *shield)
{calls[player_index_from_unit_index(unit)]++;}
static void unit_scripting_set_current_vitality_of(long unit,real body,real shield)
{long slot=player_index_from_unit_index(unit);CHECK(slot!=NONE,"invalid vitality target");objects[slot].object.body_vitality=body;objects[slot].object.shield_vitality=shield;calls[slot]++;}
#define object_datum biped_datum
#include "vitality.inc"
#undef object_datum
static int script_units(long *units)
{
	int n = 0;
	long ref;
	hs_players();
	for (ref = ref_head; ref != NONE;)
	{
		struct data_reference *r = datum_get(object_list_data, ref);
		units[n++] = r->datum_index;
		ref = r->next_reference_index;
	}
	object_list_delete(0);
	return n;
}
static void verify(void)
{
	long named[128], followers[130];
	int n = script_units(named), i, j, count;
	for (i = 0; i < n; i++)
	{
		memset(followers, 0x55, sizeof(followers));
		count = players_following(named[i], followers, 128);
		CHECK(count == (i == 0 && n > 2 ? n - 2 : 0),
			  "script player %d, %d live units: %d followers", i, n, count);
		for (j = 0; j < count; j++)
			CHECK(followers[j] == named[j + 2], "follower %d disagrees with hs_players", j);
	}
	CHECK(!players_following(NONE, followers, 128), "NONE has followers");
	CHECK(!players_following(123456, followers, 128), "nonplayer has followers");
}
static void reset(int count)
{
	int i;
	memset(occupied, 0, sizeof(occupied));
	memset(calls, 0, sizeof(calls));
	host = TRUE;
	coop_scripts_new_game();
	for (i = 0; i < count; i++)
	{
		occupied[i] = 1;
		people[i].unit_index = 1000 + 13 * i;
		objects[i].object.parent_object_index = 555;
	}
	data_delete_all(object_list_data);
}
int main(int argc, char **argv)
{
	char const *name = argc > 1 ? argv[1] : "";
	int n, i;
	long units[128], out[128];
	real_point3d destination = {1, 2, 3};
	object_list_data = game_state_data_new("test refs", 128, sizeof(struct data_reference));
	if (!strcmp(name,"cooperative-identity"))
	{
		CHECK(!game_is_cooperative(),"ordinary solo changed");
		campaign_coop=TRUE;
		CHECK(game_is_cooperative(),"network campaign misidentified as solo");
		campaign_coop=FALSE;player_spawn_count=2;
		CHECK(game_is_cooperative(),"classic split-screen changed");
		return 0;
	}
	if (strstr(name,"vitality"))
	{
		adaptations=strcmp(name,"native-current-vitality")!=0;reset(3);
		for(i=0;i<3;i++)objects[i].object.body_vitality=objects[i].object.shield_vitality=1.f;
		if(!strcmp(name,"legacy-maximum-vitality"))
		{
			CHECK(network_coop_set_players_vitality(people[2].unit_index,TRUE,70.f,90.f),"legacy maximum was not shared");
			CHECK(players_vitality.set&&players_vitality.maximum_body==70.f&&players_vitality.maximum_shield==90.f,"legacy late-spawn maximum lost");
			for(i=0;i<3;i++)CHECK(calls[i]==1,"legacy maximum missed a player");
		}
		else
		{
			unit_scripting_set_current_vitality(people[2].unit_index,.2f,.3f);
			CHECK(!players_vitality.set,"current repair changed late-spawn maximum");
			for(i=0;i<3;i++)CHECK(calls[i]==(adaptations||i==2),"scripted repair changed an unrelated player under native policy");
			CHECK(objects[2].object.body_vitality==.2f&&objects[2].object.shield_vitality==.3f,"targeted current repair was lost");
		}
	}
	else if (!strcmp(name, "counts"))
	{
		for (n = 0; n <= 128; n++)
		{
			reset(n);
			verify();
		}
	}
	else if (!strcmp(name, "gaps"))
	{
		reset(128);
		for (i = 0; i < 128; i++)
		{
			if (i % 3 == 0)
				occupied[i] = 0;
			if (i % 5 == 0)
				people[i].unit_index = NONE;
		}
		verify();
	}
	else if (!strcmp(name, "respawn"))
	{
		reset(12);
		for (i = 11; i >= 0; i--)
		{
			people[i].unit_index = NONE;
			verify();
			people[i].unit_index = 9000 + i;
			verify();
			occupied[i] = 0;
			verify();
		}
	}
	else if (!strcmp(name, "bounded"))
	{
		reset(12);
		n = script_units(units);
		for (i = 0; i <= n; i++)
		{
			out[i] = 123456;
			CHECK(players_following(units[0], out, i) == (i < n - 2 ? i : n - 2),
				  "capacity ignored");
			CHECK(out[i] == 123456, "output overrun");
		}
	}
	else if (!strcmp(name, "nonhost"))
	{
		reset(12);
		n = script_units(units);
		host = FALSE;
		CHECK(!players_following(units[0], out, 128), "nonhost followers");
	}
	else if (!strcmp(name, "native-save"))
	{
		reset(6);
		CHECK(coop_scripts_safe_to_save(), "legacy one-safe-player cutscene policy changed");
		adaptations = FALSE;
		coop_scripts_new_game();
		CHECK(!coop_scripts_safe_to_save(), "authored native save safety was weakened");
		native_safe = TRUE;
		CHECK(coop_scripts_safe_to_save(), "native safe condition failed in authored mode");
	}
	else if (!strcmp(name, "native-actions") || !strcmp(name, "extraction-actions"))
	{
		reset(6);
		n = script_units(units);
		extraction = !strcmp(name, "extraction-actions");
		adaptations = extraction;
		coop_scripts_new_game();
		extraction = FALSE;
		CHECK(coop_scripts_native_player_actions(), "map-start native policy did not latch");
		CHECK(!players_following(units[0], out, 128), "authored player0 acquired unrelated followers");
		coop_scripts_teleport_followers(units[0], &destination);
		coop_scripts_suspend_followers(units[0], TRUE);
		coop_scripts_exit_followers(units[0], 555);
		coop_scripts_board_followers(units[0], 555, "P-RiderLF");
		coop_scripts_player_add_equipment(units[0], 2, TRUE);
		for (i = 0; i < n; i++)
			CHECK(calls[player_index_from_unit_index(units[i])] == (i == 0),
				"authored action affected an unselected player or omitted its actual recipient");
		adaptations = TRUE;
		coop_scripts_new_game();
		CHECK(players_following(units[0], out, 128) == n - 2, "legacy followers did not resume");
	}
	else
	{
		reset(12);
		n = script_units(units);
		if (!strcmp(name, "teleport"))
			coop_scripts_teleport_followers(units[0], &destination);
		else if (!strcmp(name, "suspend"))
			coop_scripts_suspend_followers(units[0], TRUE);
		else if (!strcmp(name, "exit"))
		{
			objects[player_index_from_unit_index(units[4])].object.parent_object_index = 556;
			coop_scripts_exit_followers(units[0], 555);
		}
		else if (!strcmp(name, "board"))
			coop_scripts_board_followers(units[0], 555, "P-RiderLF");
		else if (!strcmp(name, "equipment"))
			coop_scripts_player_add_equipment(units[0], 2, TRUE);
		else
			CHECK(FALSE, "unknown case");
		for (i = 0; i < n; i++)
			CHECK(calls[player_index_from_unit_index(units[i])] ==
					  ((i >= 2 && (!strcmp(name, "exit") ? i != 4 : TRUE)) ||
					   (i == 0 && !strcmp(name, "equipment"))),
				  "wrong action recipient %d", i);
	}
	return 0;
}
