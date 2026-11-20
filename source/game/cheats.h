/*
CHEATS.H

header included in hcex build.
*/

#ifndef __CHEATS_H
#define __CHEATS_H
#pragma once

/* ---------- constants */

/* ---------- macros */

/* ---------- structures */

struct cheat_globals
{
	boolean deathless_player;
	boolean jetpack;
	boolean infinite_ammo;
	boolean bump_possession;
	boolean super_jump;
	boolean reflexive_damage_effects;
	boolean medusa;
	boolean omnipotent;
	boolean controller_enabled;
	boolean bottomless_clip;
};

/* ---------- prototypes/CHEATS.C */

void cheats_initialize(
	void);
void cheats_initialize_for_new_map(
	void);
void cheat_teleport_to_camera(
	void);
void cheat_active_camouflage(
	void);
void cheat_all_weapons(
	void);
void cheat_all_powerups(
	void);
void cheat_all_vehicles(
	void);
void cheats_dispose(
	void);
void cheats_dispose_from_old_map(
	void);
void cheats_update(
	void);
void cheats_network_client_enforce(
	void);
/* port: a network game's host's objects from its console (the cheats,
hs_object_create): whether one more may be made, and the one made (NONE
for none), which is deleted again if an original Xbox could not keep it;
FALSE then */
boolean cheats_console_object_allowed(
	void);
boolean cheats_console_object_made(
	long object_index);
/* port: a fresh native Forge copy, at the source transform; console only. */
long cheat_forge_target(void);
union real_point3d;
void cheats_forge_reset(void);
void cheats_forge_update(void);
boolean cheat_forge_pick(void);
long cheat_forge_place(void);
boolean cheat_forge_undo(void);
boolean cheat_forge_remove_selected(void);
boolean cheat_forge_move_selected(void);
boolean cheat_forge_rotate_selected(void);
boolean cheat_forge_undo_move(void);
boolean cheat_forge_redo_move(void);
enum forge_physics_mode
{
	_forge_physics_refused,
	_forge_physics_normal,
	_forge_physics_fixed,
	_forge_physics_phased,
};
boolean cheats_forge_physics_available(void);
boolean cheats_forge_remote_physics_active(void);
enum forge_physics_mode cheats_forge_network_physics(long object_index);
union real_vector3d;
void cheats_forge_receive_physics(long object_index, enum forge_physics_mode mode,
	union real_point3d const *position, union real_vector3d const *forward, union real_vector3d const *up);
enum forge_physics_mode cheat_forge_cycle_physics(void);
boolean cheat_forge_sidecar(boolean saving);
enum forge_hmv_action
{
	_forge_hmv_load,
	_forge_hmv_load_saved,
	_forge_hmv_save
};
boolean cheat_forge_hmv(enum forge_hmv_action action, char const **title, char const **author);
long cheat_forge_clone_at(long object_index, union real_point3d const *position);
long cheat_forge_clone(
	long object_index);
void cheats_load(
	void);
void cheat_active_camouflage_local_player(
	short player_index);

/* ---------- globals */

extern struct cheat_globals cheat;

/* ---------- public code */

#endif // __CHEATS_H
