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
long cheats_forge_aimed_object(long unit_index);
int cheats_forge_guest_edits_allowed(void);
enum forge_guest_action
{
	_forge_guest_rotate,
	_forge_guest_select,
	_forge_guest_place,
	_forge_guest_move,
	_forge_guest_remove,
	_forge_guest_cycle_physics,
	NUMBER_OF_FORGE_GUEST_ACTIONS
};
boolean cheats_forge_guest_edit(enum forge_guest_action action, long unit_index, long object_index);
union real_point3d;
void cheats_forge_reset(void);
void cheats_forge_update(void);
boolean cheat_forge_pick(void);
long cheat_forge_place(void);
short cheats_forge_catalog(long group, long *definitions, short capacity);
boolean cheats_forge_catalog_place(long definition_index);
long cheats_forge_selection(void);
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
enum forge_physics_mode cheats_forge_object_physics(long object_index);
union real_vector3d;
#if defined(HALO_GAME_BROWSER) && !defined(HALO_WEB)
struct real_matrix4x3;
void cheats_forge_grab_input(boolean grab, boolean rotate);
void cheats_forge_grab_cancel(void);
long cheats_forge_grab_preview(struct real_matrix4x3 *pose, float *radius);
#endif
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
