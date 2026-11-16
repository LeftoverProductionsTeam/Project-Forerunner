/* One native, host-owned gear record shared by Extraction and Survival.
 * This is not a client claim, tag importer or native game-state checkpoint.
 * SPDX-License-Identifier: AGPL-3.0-or-later */
#ifndef HALO_GEAR_H
#define HALO_GEAR_H

#include "game_modes.h"

#define GEAR_CAPACITY 32
#define GEAR_LOADOUT_WEAPONS 2
#define GEAR_ID_BYTES 16

/* Append phases: existing idle/deployed/escrow records retain their values and
 * layout. Older readers safely reject an in-flight prepared reservation. */
enum gear_phase { _gear_idle, _gear_deployed, _gear_escrow, _gear_reserved };
enum gear_outcome { _gear_no_outcome, _gear_escaped, _gear_lost };

struct gear_item
{
	byte id[GEAR_ID_BYTES];
	char map_name[256];
	unsigned long map_checksum;
	struct game_modes_inventory_weapon equipment;
};

struct gear_record
{
	unsigned long generation;
	byte owner[GEAR_ID_BYTES], run[GEAR_ID_BYTES], settled_run[GEAR_ID_BYTES];
	short phase, outcome, count, reserved_count, escrow_count;
	short grenades[2], reserved_grenades[2], escrow_grenades[2];
	struct gear_item items[GEAR_CAPACITY];
	struct gear_item reserved[GAME_MODES_INVENTORY_WEAPONS];
	struct gear_item escrow[GAME_MODES_INVENTORY_WEAPONS];
};

struct gear_session
{
	int descriptor;
	boolean opened, pending;
	short grenade_room[2]; /* native capacity minus the currently carried count */
	long loadout_unit; /* transient native body filter; NONE preserves legacy selection */
	char path[1024];
	byte run[GEAR_ID_BYTES];
	struct gear_record record;
};

/* Local profiles and verified remote accounts use separate hash domains.
 * Neither a profile path nor an account supplied by a client is a filename. */
void gear_profile_owner(char const *profile_directory, byte owner[GEAR_ID_BYTES]);
void gear_account_owner(byte const account[GEAR_ID_BYTES], byte owner[GEAR_ID_BYTES]);
boolean gear_open(struct gear_session *session, byte const owner[GEAR_ID_BYTES]);
void gear_close(struct gear_session *session);
/* Read selected gear before reserving it. Default is exact-cache admission;
 * optional game.shared_gear resolves compatible paths/quantities against the
 * current native cache. An optional transient native body filter also applies
 * to selection and reservation. Incompatible items stay safely in the stash. */
boolean gear_loadout(struct gear_session *session, struct game_modes_inventory *inventory);
/* Rotate the next compatible lead weapon to the front through the existing
 * atomic record. Stash order is the durable choice, shared by both modes. */
boolean gear_cycle_loadout(struct gear_session *session);
/* Host UI preview, valid only until the next bank operation. */
struct gear_item const *gear_loadout_choice(struct gear_session *session);
/* Discard only the confirmed idle-stash item at the previewed generation.
 * Retrying a published deletion confirms it without removing another item. */
boolean gear_discard(struct gear_session *session, byte const id[GEAR_ID_BYTES], unsigned long generation);
/* Prepare the chosen pair before any grant. This unissued phase refunds on
 * reopen; only confirmed native deployments acquire the normal loss policy. */
boolean gear_reserve(struct gear_session *session);
/* Confirm after the native grant, before any input/simulation can use it.
 * A published intent is authoritative even if its flush still needs retry. */
boolean gear_confirm_deployment(struct gear_session *session);
boolean gear_cancel_reservation(struct gear_session *session);
boolean gear_lose(struct gear_session *session);
/* Persist the live return in escrow before consuming its world objects;
 * finish then banks it exactly once. A published escrow may need a flush
 * retry even when prepare returns FALSE: inspect record.phase. */
boolean gear_prepare_escape(struct gear_session *session, struct game_modes_inventory const *inventory,
	long const object_indices[GAME_MODES_INVENTORY_WEAPONS]);
boolean gear_finish_escape(struct gear_session *session);
void gear_track_reserved(struct gear_session const *session,
	long const object_indices[GAME_MODES_INVENTORY_WEAPONS]);
void gear_world_reset(void);

#endif
