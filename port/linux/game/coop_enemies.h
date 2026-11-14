/*
COOP_ENEMIES.H

Network co-op's extra enemies, for its players (coop_enemies.c).
*/

#ifndef __COOP_ENEMIES_H
#define __COOP_ENEMIES_H

/* network_coop.c: a game begins (its EXTRA ENEMIES read), or the host's
game state went back (a cutscene skipped): the dropships' riders kept are
forgotten. A full world revert restarts the built-in Firefight encounter;
respawn-only team wipes preserve its progress and shared lives. */
void coop_enemies_new_game(
	void);
void coop_enemies_reset(
	void);

/* encounters.c: how many enemies more a squad placed with `count` gets
(none but on a co-op host, for an encounter of the players' enemies) */
short coop_enemies_extra_count(
	long encounter_index,
	short count);

/* the places coop_enemies_spread_position tries around a starting location */
#define COOP_ENEMIES_SPREAD_SPOTS 90

/* encounters.c: finds a spot for a squad's `number`th extra enemy
(counting from 1) around the starting location `origin`, on its floor,
clear of walls and objects, with room to stand and nobody there. FALSE if
there's no room. `taken` (COOP_ENEMIES_SPREAD_SPOTS bits, or NULL) marks the
places found taken, which stay so while the squad's enemies are placed: they
aren't tried again. */
boolean coop_enemies_spread_position(
	real_point3d const *origin,
	short number,
	long *taken,
	real_point3d *position);
/* encounters.c: where the `number`th extra enemy (counting from 1) goes
when no starting location has room: a place on rings around `origin` with
the way open and ground below, whoever stands there (as extra enemies were
placed before free ground was looked for), so the squad still gets every
enemy the setting asks for. FALSE if none (the starting location itself
then) */
boolean coop_enemies_fallback_position(
	real_point3d const *origin,
	short number,
	real_point3d *position);

/* units.c's vehicle_scripting_load_magic: a rider seated, and one left
without a seat (the vehicle's seats taken: an extra enemy), which is kept,
off the map, until the vehicle's riders get out (TRUE: kept, the caller
erases it) */
void coop_enemies_rider_seated(
	long vehicle_index,
	long unit_index);
boolean coop_enemies_rider_unseated(
	long vehicle_index,
	long unit_index);

/* network_coop.c, after each of the host's ticks: the riders kept placed
at their vehicle's seats once its own riders get out */
void coop_enemies_update(
	void);

/* The built-in director owns its tracked enemies/lives/result, never the whole
script scheduler. Recognized Spark native round scripts disable that director. */
boolean coop_enemies_firefight_active(void);
/* main.c: active built-in rounds decide when their result may end the map. */
boolean coop_enemies_firefight_result_ready(void);
boolean coop_enemies_firefight_out_of_lives(void);
boolean coop_enemies_firefight_can_spawn(long player_index);
/* players.c, before detaching a real dead player's body. */
void coop_enemies_firefight_player_died(long unit_index);
/* players.c, only a successful owned live spawn clears its paid respawn grant. */
void coop_enemies_firefight_player_spawned(long player_index);
/* units.c: actual damage owner, or NONE for uncredited/no-statistics damage. */
void coop_enemies_firefight_enemy_damaged(long unit_index, long player_index);
/* units.c: final unit_died only, after detaching its actor bindings. */
void coop_enemies_firefight_enemy_died(long unit_index);
/* units.c: release a dead carrier's garbage hold after its native spawn attempt. */
void coop_enemies_firefight_spawn_finished(long unit_index);
/* actors.c: only tracked Firefight parents may spawn encounterless children.
The host tracks each actual child; NONE or exhausted tracking aborts the encounter. */
boolean coop_enemies_firefight_tracks_unit(long unit_index);
boolean coop_enemies_firefight_track_spawned_unit(long parent_index, long unit_index);
/* network_distributed.c: one host-state notice after a new client's ready message. */
void coop_enemies_firefight_send_status(long machine_index);

#endif
