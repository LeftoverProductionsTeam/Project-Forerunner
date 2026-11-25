/* Shared native gear reservations and escape settlement. The one versioned
 * record is replaced atomically under an OS lock; no database or save rewind.
 * An interrupted deployed run loses only its reserved, at-risk gear. A saved
 * escape escrow completes on reopen, never awards the same run twice.
 * SPDX-License-Identifier: AGPL-3.0-or-later */
#include "cseries.h"
#include "gear.h"
#include <string.h>
#if !defined(HALO_ANDROID) && !defined(HALO_WEB) && !defined(HALO_CONSOLE)
#include "cache/cache_files.h"
#include "game/game.h"
#include "networking/network_game_globals.h"
#include "objects/objects.h"
#include "../src/identity.h"
#include "../src/port_folders.h"
#include "../src/posix.h"
#include "../src/port_config.h"
#include <stdio.h>
#include <stdlib.h>

extern char const *platform_folder(enum port_folder folder);

enum { GEAR_ITEM_BYTES = 546, GEAR_HEADER_BYTES = 80,
	GEAR_RECORD_BYTES = GEAR_HEADER_BYTES + (GEAR_CAPACITY + 2 * GAME_MODES_INVENTORY_WEAPONS) * GEAR_ITEM_BYTES + 32 };

static struct { long object_index; byte id[GEAR_ID_BYTES]; } world_gear[MAXIMUM_OBJECTS_PER_MAP];

static boolean gear_nonzero(byte const id[GEAR_ID_BYTES])
{
	short index;
	for (index = 0; index < GEAR_ID_BYTES; index++)
		if (id[index]) return TRUE;
	return FALSE;
}

static void gear_id(char const *domain, void const *source, short size, byte id[GEAR_ID_BYTES])
{
	byte message[1200], digest[32];
	size_t prefix = strlen(domain);

	if (prefix + size > sizeof(message))
	{
		memset(id, 0, GEAR_ID_BYTES);
		return;
	}
	memcpy(message, domain, prefix);
	memcpy(message + prefix, source, size);
	identity_sha256(message, (int)(prefix + size), digest);
	memcpy(id, digest, GEAR_ID_BYTES);
}

void gear_profile_owner(char const *directory, byte owner[GEAR_ID_BYTES])
{
	size_t length = directory ? strlen(directory) : 0;
	if (!length || length > 1100) { memset(owner, 0, GEAR_ID_BYTES); return; }
	gear_id("forerunner profile\n", directory, (short)length, owner);
}

void gear_account_owner(byte const account[GEAR_ID_BYTES], byte owner[GEAR_ID_BYTES])
{
	if (!gear_nonzero(account)) { memset(owner, 0, GEAR_ID_BYTES); return; }
	gear_id("forerunner account\n", account, GEAR_ID_BYTES, owner);
}

void gear_session_owner(long player_index, byte owner[GEAR_ID_BYTES])
{
	gear_id("forerunner session player\n", &player_index, sizeof(player_index), owner);
}

static void gear_put16(byte **cursor, unsigned short value)
{
	*(*cursor)++ = (byte)value; *(*cursor)++ = (byte)(value >> 8);
}

static unsigned short gear_get16(byte const **cursor)
{
	unsigned short value = (unsigned short)((*cursor)[0] | (unsigned short)(*cursor)[1] << 8);
	*cursor += 2; return value;
}

static void gear_put32(byte **cursor, unsigned long value)
{
	short index;
	for (index = 0; index < 4; index++) *(*cursor)++ = (byte)(value >> (index * 8));
}

static unsigned long gear_get32(byte const **cursor)
{
	unsigned long value = 0;
	short index;
	for (index = 0; index < 4; index++) value |= (unsigned long)*(*cursor)++ << (index * 8);
	return value;
}

static void gear_item_put(byte **cursor, struct gear_item const *item)
{
	union { real value; unsigned int bits; } age;
	short index;

	memcpy(*cursor, item->id, GEAR_ID_BYTES); *cursor += GEAR_ID_BYTES;
	gear_put32(cursor, item->map_checksum);
	memcpy(*cursor, item->map_name, sizeof(item->map_name)); *cursor += sizeof(item->map_name);
	memcpy(*cursor, item->equipment.tag_path, sizeof(item->equipment.tag_path)); *cursor += sizeof(item->equipment.tag_path);
	gear_put16(cursor, (unsigned short)item->equipment.slot);
	for (index = 0; index < 2; index++) gear_put16(cursor, (unsigned short)item->equipment.rounds_loaded[index]);
	for (index = 0; index < 2; index++) gear_put16(cursor, (unsigned short)item->equipment.rounds_reserve[index]);
	age.value = item->equipment.age; gear_put32(cursor, age.bits);
}

static void gear_item_get(byte const **cursor, struct gear_item *item)
{
	union { real value; unsigned int bits; } age;
	short index;

	memcpy(item->id, *cursor, GEAR_ID_BYTES); *cursor += GEAR_ID_BYTES;
	item->map_checksum = gear_get32(cursor);
	memcpy(item->map_name, *cursor, sizeof(item->map_name)); *cursor += sizeof(item->map_name);
	memcpy(item->equipment.tag_path, *cursor, sizeof(item->equipment.tag_path)); *cursor += sizeof(item->equipment.tag_path);
	item->equipment.slot = (short)gear_get16(cursor);
	for (index = 0; index < 2; index++) item->equipment.rounds_loaded[index] = (short)gear_get16(cursor);
	for (index = 0; index < 2; index++) item->equipment.rounds_reserve[index] = (short)gear_get16(cursor);
	age.bits = (unsigned int)gear_get32(cursor); item->equipment.age = age.value;
}

static boolean gear_has(struct gear_item const *items, short count, byte const id[GEAR_ID_BYTES])
{
	short index;
	for (index = 0; index < count; index++) if (!memcmp(items[index].id, id, GEAR_ID_BYTES)) return TRUE;
	return FALSE;
}

static boolean gear_items_valid(struct gear_item const *items, short count)
{
	short index, magazine;
	for (index = 0; index < count; index++)
	{
		struct game_modes_inventory_weapon const *weapon = &items[index].equipment;
		if (!gear_nonzero(items[index].id) || gear_has(items, index, items[index].id) ||
			!items[index].map_name[0] || !memchr(items[index].map_name, 0, sizeof(items[index].map_name)) ||
			!weapon->tag_path[0] || !memchr(weapon->tag_path, 0, sizeof(weapon->tag_path)) ||
			!VALID_INDEX(weapon->slot, GAME_MODES_INVENTORY_WEAPONS) || !(weapon->age >= 0.f && weapon->age <= 1.f))
			return FALSE;
		for (magazine = 0; magazine < 2; magazine++)
			if (weapon->rounds_loaded[magazine] < 0 || weapon->rounds_reserve[magazine] < 0) return FALSE;
	}
	return TRUE;
}

static boolean gear_record_valid(struct gear_record const *record)
{
	short index;
	if (!gear_nonzero(record->owner) || record->phase < _gear_idle || record->phase > _gear_reserved ||
		record->outcome < _gear_no_outcome || record->outcome > _gear_lost ||
		record->count < 0 || record->count > GEAR_CAPACITY ||
		record->reserved_count < 0 || record->reserved_count > GAME_MODES_INVENTORY_WEAPONS ||
		record->escrow_count < 0 || record->escrow_count > GAME_MODES_INVENTORY_WEAPONS ||
		record->count + record->escrow_count > GEAR_CAPACITY ||
		(record->phase == _gear_idle && (record->reserved_count || record->escrow_count)) ||
		(record->phase != _gear_idle && !gear_nonzero(record->run)) ||
		!gear_items_valid(record->items, record->count) || !gear_items_valid(record->reserved, record->reserved_count) ||
		!gear_items_valid(record->escrow, record->escrow_count)) return FALSE;
	for (index = 0; index < 2; index++)
		if (record->grenades[index] < 0 || record->reserved_grenades[index] < 0 || record->escrow_grenades[index] < 0 ||
			record->grenades[index] > SHORT_MAX - record->escrow_grenades[index] ||
			(record->phase == _gear_idle && (record->reserved_grenades[index] || record->escrow_grenades[index])) ||
			(record->phase != _gear_escrow && record->escrow_grenades[index])) return FALSE;
	if ((record->phase == _gear_idle && gear_nonzero(record->run)) ||
		((record->phase == _gear_deployed || record->phase == _gear_reserved) && record->escrow_count)) return FALSE;
	for (index = 0; index < record->reserved_count; index++)
		if (gear_has(record->items, record->count, record->reserved[index].id)) return FALSE;
	for (index = 0; index < record->escrow_count; index++)
		if (gear_has(record->items, record->count, record->escrow[index].id)) return FALSE;
	return TRUE;
}

static posix_ulong gear_encode(struct gear_record const *record, byte buffer[GEAR_RECORD_BYTES])
{
	byte *cursor = buffer;
	short index;

	memcpy(cursor, "FRNG", 4); cursor += 4;
	gear_put16(&cursor, 1); gear_put16(&cursor, (unsigned short)record->phase);
	gear_put32(&cursor, record->generation);
	memcpy(cursor, record->owner, GEAR_ID_BYTES); cursor += GEAR_ID_BYTES;
	memcpy(cursor, record->run, GEAR_ID_BYTES); cursor += GEAR_ID_BYTES;
	memcpy(cursor, record->settled_run, GEAR_ID_BYTES); cursor += GEAR_ID_BYTES;
	gear_put16(&cursor, (unsigned short)record->count);
	gear_put16(&cursor, (unsigned short)record->reserved_count);
	gear_put16(&cursor, (unsigned short)record->escrow_count);
	for (index = 0; index < 2; index++) gear_put16(&cursor, (unsigned short)record->grenades[index]);
	for (index = 0; index < 2; index++) gear_put16(&cursor, (unsigned short)record->reserved_grenades[index]);
	for (index = 0; index < 2; index++) gear_put16(&cursor, (unsigned short)record->escrow_grenades[index]);
	gear_put16(&cursor, (unsigned short)record->outcome);
	for (index = 0; index < record->count; index++) gear_item_put(&cursor, &record->items[index]);
	for (index = 0; index < record->reserved_count; index++) gear_item_put(&cursor, &record->reserved[index]);
	for (index = 0; index < record->escrow_count; index++) gear_item_put(&cursor, &record->escrow[index]);
	identity_sha256(buffer, (int)(cursor - buffer), cursor);
	return (posix_ulong)(cursor - buffer + 32);
}

static boolean gear_decode(byte const *buffer, posix_ulong size, byte const owner[GEAR_ID_BYTES], struct gear_record *record)
{
	byte const *cursor = buffer;
	byte digest[32];
	short index;
	posix_ulong expected;

	if (size < GEAR_HEADER_BYTES + 32 || size > GEAR_RECORD_BYTES || memcmp(cursor, "FRNG", 4)) return FALSE;
	cursor += 4;
	if (gear_get16(&cursor) != 1) return FALSE;
	memset(record, 0, sizeof(*record));
	record->phase = (short)gear_get16(&cursor); record->generation = gear_get32(&cursor);
	memcpy(record->owner, cursor, GEAR_ID_BYTES); cursor += GEAR_ID_BYTES;
	memcpy(record->run, cursor, GEAR_ID_BYTES); cursor += GEAR_ID_BYTES;
	memcpy(record->settled_run, cursor, GEAR_ID_BYTES); cursor += GEAR_ID_BYTES;
	record->count = (short)gear_get16(&cursor);
	record->reserved_count = (short)gear_get16(&cursor);
	record->escrow_count = (short)gear_get16(&cursor);
	for (index = 0; index < 2; index++) record->grenades[index] = (short)gear_get16(&cursor);
	for (index = 0; index < 2; index++) record->reserved_grenades[index] = (short)gear_get16(&cursor);
	for (index = 0; index < 2; index++) record->escrow_grenades[index] = (short)gear_get16(&cursor);
	record->outcome = (short)gear_get16(&cursor);
	if (record->count < 0 || record->count > GEAR_CAPACITY || record->reserved_count < 0 ||
		record->reserved_count > GAME_MODES_INVENTORY_WEAPONS || record->escrow_count < 0 ||
		record->escrow_count > GAME_MODES_INVENTORY_WEAPONS || memcmp(record->owner, owner, GEAR_ID_BYTES)) return FALSE;
	expected = GEAR_HEADER_BYTES + (record->count + record->reserved_count + record->escrow_count) * GEAR_ITEM_BYTES + 32;
	if (size != expected) return FALSE;
	identity_sha256(buffer, (int)size - 32, digest);
	if (memcmp(digest, buffer + size - 32, 32)) return FALSE;
	for (index = 0; index < record->count; index++) gear_item_get(&cursor, &record->items[index]);
	for (index = 0; index < record->reserved_count; index++) gear_item_get(&cursor, &record->reserved[index]);
	for (index = 0; index < record->escrow_count; index++) gear_item_get(&cursor, &record->escrow[index]);
	return gear_record_valid(record);
}

static boolean gear_host(void)
{
	return game_in_progress() && !network_game_distributed_client() &&
		(game_connection() == _game_connection_local || game_connection() == _game_connection_network_server);
}

static boolean gear_flush(struct gear_session *session)
{
	byte buffer[GEAR_RECORD_BYTES];
	posix_ulong size;
	if (!session->opened || !gear_host()) return FALSE;
	if (session->session_only || !session->pending) return TRUE;
	size = gear_encode(&session->record, buffer);
	if (!posix_gear_replace(session->path, buffer, size)) return FALSE;
	session->pending = FALSE;
	return TRUE;
}

static boolean gear_commit(struct gear_session *session, struct gear_record *next)
{
	byte buffer[GEAR_RECORD_BYTES], observed[GEAR_RECORD_BYTES];
	posix_ulong size, read_size;
	int written;

	if (!gear_flush(session) || session->record.generation == 0xffffffffUL || !gear_record_valid(next)) return FALSE;
	next->generation = session->record.generation + 1;
	if (session->session_only)
	{
		session->record = *next;
		return TRUE;
	}
	size = gear_encode(next, buffer);
	written = posix_gear_replace(session->path, buffer, size);
	/* An OS can report failure after replacement (for example directory sync).
	 * Remember a published intent; retry its flush, never mint another award. */
	if (written || (posix_gear_read(session->path, observed, sizeof(observed), &read_size) == 1 &&
		read_size == size && !memcmp(buffer, observed, size)))
	{
		session->record = *next;
		session->pending = !written;
	}
	return written != 0;
}

void gear_close(struct gear_session *session)
{
	if (session->opened && !session->session_only) posix_gear_unlock(session->descriptor);
	memset(session, 0, sizeof(*session));
	session->descriptor = -1;
}

boolean gear_open_session(struct gear_session *session, byte const owner[GEAR_ID_BYTES])
{
	if (session->opened || !gear_host() || !gear_nonzero(owner)) return FALSE;
	gear_close(session);
	session->opened = session->session_only = TRUE;
	session->loadout_unit = NONE;
	memcpy(session->record.owner, owner, GEAR_ID_BYTES);
	return TRUE;
}

boolean gear_open(struct gear_session *session, byte const owner[GEAR_ID_BYTES])
{
	byte buffer[GEAR_RECORD_BYTES];
	char folder[1024], name[33], lock[1100];
	char const *root;
	struct posix_file_information information;
	posix_ulong size;
	short index;
	int read;

	if (session->opened || !gear_host()) return FALSE;
	root = platform_folder(_port_folder_private);
	if (!root || !gear_nonzero(owner) ||
		snprintf(folder, sizeof(folder), "%s/gear", root) >= (int)sizeof(folder) || posix_is_link(folder)) return FALSE;
	if (posix_make_directory(folder) != 0 &&
		(posix_stat(folder, &information) != 0 || !(information.flags & _posix_file_is_directory))) return FALSE;
	for (index = 0; index < GEAR_ID_BYTES; index++) sprintf(name + index * 2, "%02x", (unsigned int)owner[index]);
	if (snprintf(session->path, sizeof(session->path), "%s/%s.gear", folder, name) >= (int)sizeof(session->path) ||
		snprintf(lock, sizeof(lock), "%s.lock", session->path) >= (int)sizeof(lock)) return FALSE;
	session->descriptor = posix_gear_lock(lock);
	if (session->descriptor < 0) return FALSE;
	session->opened = TRUE;
	session->loadout_unit = NONE;
	read = posix_gear_read(session->path, buffer, sizeof(buffer), &size);
	if (read < 0 || (read && !gear_decode(buffer, size, owner, &session->record))) goto failed;
	if (!read)
	{
		memset(&session->record, 0, sizeof(session->record));
		memcpy(session->record.owner, owner, GEAR_ID_BYTES);
		session->pending = TRUE;
		if (!gear_flush(session)) goto failed;
	}
	/* No usable native grant was confirmed: refund the saved preparation even
	 * when the failed grant's refund could not be written before disconnect. */
	if (session->record.phase == _gear_reserved && !gear_cancel_reservation(session)) goto failed;
	/* Stash survives a failed run. Issued gear is at risk from deployment;
	 * only a captured, saved escape escrow can recover rewards after a crash. */
	if (session->record.phase == _gear_deployed && !gear_lose(session)) goto failed;
	if (session->record.phase == _gear_escrow)
	{
		memcpy(session->run, session->record.run, GEAR_ID_BYTES);
		if (!gear_finish_escape(session)) goto failed;
	}
	return TRUE;
failed:
	gear_close(session);
	return FALSE;
}

/* Exact-cache defaults remain available. Shared gear resolves only definitions
 * already in the target map; it never imports content or changes stored items. */
boolean gear_item_available(struct gear_session const *session, struct gear_item const *item)
{
	char const *map = cache_file_loaded_map_name();
	if (!map) return FALSE;
	if (session->loadout_unit != NONE && !game_modes_inventory_weapon_for_unit(session->loadout_unit, &item->equipment)) return FALSE;
	if (config_boolean("game.shared_gear"))
		return game_modes_inventory_weapon_compatible(&item->equipment);
	return item->map_checksum == cache_files_get_checksum() && !strcmp(item->map_name, map);
}

static short gear_selected(struct gear_session const *session, short indices[GEAR_LOADOUT_WEAPONS])
{
	short count = 0, index;

	for (index = 0; index < session->record.count && count < GEAR_LOADOUT_WEAPONS; index++)
	{
		if (gear_item_available(session, &session->record.items[index]))
			indices[count++] = index;
	}
	return count;
}

boolean gear_cycle_loadout(struct gear_session *session, short slot)
{
	struct gear_record next;
	short indices[GEAR_CAPACITY], count = 0, index;
	boolean pending = session->pending;

	if (!VALID_INDEX(slot, GEAR_LOADOUT_WEAPONS) || !gear_flush(session) || session->record.phase != _gear_idle) return FALSE;
	/* An uncertain published selection is retried, not advanced twice. */
	if (pending) return TRUE;
	for (index = 0; index < session->record.count; index++)
	{
		if (gear_item_available(session, &session->record.items[index]))
			indices[count++] = index;
	}
	if (count < slot + 2) return TRUE;
	/* Rotate only available gear. Strict mode retains per-cache choices;
	 * shared mode selects across the native-compatible stash without imports. */
	next = session->record;
	for (index = slot; index < count; index++)
		next.items[indices[index]] = session->record.items[indices[index + 1 < count ? index + 1 : slot]];
	return gear_commit(session, &next);
}

struct gear_item const *gear_loadout_choice(struct gear_session *session)
{
	short indices[GEAR_LOADOUT_WEAPONS];
	if (!gear_flush(session) || session->record.phase != _gear_idle || !gear_selected(session, indices)) return NULL;
	return &session->record.items[indices[0]];
}

boolean gear_discard(struct gear_session *session, byte const id[GEAR_ID_BYTES], unsigned long generation)
{
	struct gear_record next;
	short index;

	if (!gear_flush(session) || session->record.phase != _gear_idle || !gear_nonzero(id) ||
		generation == 0xffffffffUL) return FALSE;
	if (session->record.generation != generation)
		return session->record.generation == generation + 1 && !gear_has(session->record.items, session->record.count, id);
	for (index = 0; index < session->record.count; index++)
	{
		struct gear_item const *item = &session->record.items[index];
		if (memcmp(item->id, id, GEAR_ID_BYTES)) continue;
		next = session->record;
		memmove(next.items + index, next.items + index + 1, (next.count - index - 1) * sizeof(next.items[0]));
		next.count--;
		return gear_commit(session, &next);
	}
	return FALSE;
}

boolean gear_loadout(struct gear_session *session, struct game_modes_inventory *inventory, boolean bring_stash)
{
	short index, count, indices[GEAR_LOADOUT_WEAPONS];
	char const *map = cache_file_loaded_map_name();
	memset(inventory, 0, sizeof(*inventory));
	inventory->selected_slot = NONE;
	if (!gear_flush(session) || !map || strlen(map) >= sizeof(inventory->map_name)) return FALSE;
	strcpy(inventory->map_name, map);
	inventory->map_checksum = cache_files_get_checksum();
	if (session->record.phase == _gear_deployed || session->record.phase == _gear_reserved)
	{
		for (index = 0; index < session->record.reserved_count; index++)
			inventory->weapons[inventory->weapon_count++] = session->record.reserved[index].equipment;
		memcpy(inventory->grenades, session->record.reserved_grenades, sizeof(inventory->grenades));
	}
	else if (session->record.phase == _gear_idle)
	{
		count = bring_stash ? gear_selected(session, indices) : 0;
		for (index = 0; index < count; index++)
			inventory->weapons[inventory->weapon_count++] = session->record.items[indices[index]].equipment;
		if (bring_stash)
			for (index = 0; index < 2; index++)
				inventory->grenades[index] = MIN(PIN(session->grenade_room[index], 0, 127), session->record.grenades[index]);
	}
	else return FALSE;
	for (index = 0; index < inventory->weapon_count; index++) inventory->weapons[index].slot = index;
	if (inventory->weapon_count) inventory->selected_slot = 0;
	return TRUE;
}

boolean gear_reserve(struct gear_session *session, boolean bring_stash)
{
	struct gear_record next;
	byte seed[GEAR_ID_BYTES + 4], *cursor = seed + GEAR_ID_BYTES;
	short count, index, kept, indices[GEAR_LOADOUT_WEAPONS];

	if (!gear_flush(session)) return FALSE;
	if (session->record.phase == _gear_reserved)
		return !memcmp(session->record.run, session->run, GEAR_ID_BYTES);
	if (session->record.phase != _gear_idle) return FALSE;
	next = session->record;
	memcpy(seed, next.owner, GEAR_ID_BYTES); gear_put32(&cursor, next.generation + 1);
	gear_id("forerunner run\n", seed, sizeof(seed), next.run);
	memcpy(session->run, next.run, GEAR_ID_BYTES);
	count = bring_stash ? gear_selected(session, indices) : 0;
	for (index = 0; index < count; index++)
	{
		next.reserved[next.reserved_count] = next.items[indices[index]];
		next.reserved[next.reserved_count++].equipment.slot = index;
	}
	for (index = kept = 0; index < next.count; index++)
		if ((count < 1 || index != indices[0]) && (count < 2 || index != indices[1])) next.items[kept++] = next.items[index];
	next.count = kept;
	for (index = 0; index < 2; index++)
	{
		next.reserved_grenades[index] = bring_stash ? MIN(PIN(session->grenade_room[index], 0, 127), next.grenades[index]) : 0;
		next.grenades[index] -= next.reserved_grenades[index];
	}
	next.phase = _gear_reserved;
	return gear_commit(session, &next);
}

boolean gear_confirm_deployment(struct gear_session *session)
{
	struct gear_record next;
	if (!session->opened || !gear_host() || memcmp(session->record.run, session->run, GEAR_ID_BYTES)) return FALSE;
	if (session->record.phase == _gear_deployed)
	{
		gear_flush(session);
		return TRUE; /* Retrying a published confirmation cannot revoke it. */
	}
	if (!gear_flush(session) || session->record.phase != _gear_reserved) return FALSE;
	next = session->record; next.phase = _gear_deployed;
	gear_commit(session, &next);
	/* Once published, never cancel issued gear because a directory flush
	 * failed. Existing pending-write handling retries without a second grant. */
	return session->record.phase == _gear_deployed;
}

static void gear_run_clear(struct gear_record *record, short outcome)
{
	memcpy(record->settled_run, record->run, GEAR_ID_BYTES);
	memset(record->run, 0, GEAR_ID_BYTES);
	record->outcome = outcome; record->phase = _gear_idle;
	record->reserved_count = record->escrow_count = 0;
	memset(record->reserved, 0, sizeof(record->reserved));
	memset(record->escrow, 0, sizeof(record->escrow));
	memset(record->reserved_grenades, 0, sizeof(record->reserved_grenades));
	memset(record->escrow_grenades, 0, sizeof(record->escrow_grenades));
}

boolean gear_cancel_reservation(struct gear_session *session)
{
	struct gear_record next;
	short index;
	if (!gear_flush(session)) return FALSE;
	if (session->record.phase == _gear_idle) return TRUE;
	if (session->record.phase != _gear_reserved ||
		session->record.count + session->record.reserved_count > GEAR_CAPACITY) return FALSE;
	next = session->record;
	/* A failed native grant restores the chosen pair ahead of the rest. */
	memmove(next.items + next.reserved_count, next.items, next.count * sizeof(next.items[0]));
	memcpy(next.items, next.reserved, next.reserved_count * sizeof(next.items[0]));
	next.count += next.reserved_count;
	for (index = 0; index < 2; index++)
	{
		if (next.grenades[index] > SHORT_MAX - next.reserved_grenades[index]) return FALSE;
		next.grenades[index] += next.reserved_grenades[index];
	}
	gear_run_clear(&next, _gear_lost);
	return gear_commit(session, &next);
}

boolean gear_lose(struct gear_session *session)
{
	struct gear_record next;
	if (!gear_flush(session)) return FALSE;
	if (session->record.phase == _gear_reserved) return gear_cancel_reservation(session);
	if (session->record.phase == _gear_escrow) return gear_finish_escape(session);
	if (session->record.phase == _gear_idle) return TRUE;
	next = session->record; gear_run_clear(&next, _gear_lost);
	return gear_commit(session, &next);
}

boolean gear_prepare_escape(struct gear_session *session, struct game_modes_inventory const *inventory,
	long const object_indices[GAME_MODES_INVENTORY_WEAPONS])
{
	struct gear_record next;
	short index;
	byte source[GEAR_ID_BYTES + 4], *cursor;

	if (!gear_flush(session)) return FALSE;
	if (session->record.phase == _gear_escrow) return TRUE;
	if (session->record.phase != _gear_deployed || inventory->weapon_count < 0 ||
		inventory->weapon_count > GAME_MODES_INVENTORY_WEAPONS ||
		session->record.count + inventory->weapon_count > GEAR_CAPACITY) return FALSE;
	next = session->record;
	next.escrow_count = inventory->weapon_count;
	for (index = 0; index < inventory->weapon_count; index++)
	{
		struct gear_item *item = &next.escrow[index];
		long object_index, slot;
		if (!VALID_INDEX(inventory->weapons[index].slot, GAME_MODES_INVENTORY_WEAPONS)) return FALSE;
		object_index = object_indices[inventory->weapons[index].slot];
		slot = DATUM_INDEX_TO_ABSOLUTE_INDEX(object_index);
		if (object_index == NONE || !VALID_INDEX(slot, NUMBEROF(world_gear))) return FALSE;
		memset(item, 0, sizeof(*item));
		if (world_gear[slot].object_index == object_index && gear_nonzero(world_gear[slot].id))
			memcpy(item->id, world_gear[slot].id, GEAR_ID_BYTES);
		else
		{
			memcpy(source, next.run, GEAR_ID_BYTES); cursor = source + GEAR_ID_BYTES;
			gear_put32(&cursor, (unsigned long)object_index);
			gear_id("forerunner item\n", source, sizeof(source), item->id);
		}
		memcpy(item->map_name, inventory->map_name, sizeof(item->map_name));
		item->map_checksum = inventory->map_checksum; item->equipment = inventory->weapons[index];
	}
	memcpy(next.escrow_grenades, inventory->grenades, sizeof(next.escrow_grenades));
	next.phase = _gear_escrow;
	return gear_commit(session, &next);
}

boolean gear_finish_escape(struct gear_session *session)
{
	struct gear_record next;
	short index;
	if (!gear_flush(session)) return FALSE;
	if (session->record.phase == _gear_idle)
		return session->record.outcome == _gear_escaped && !memcmp(session->record.settled_run, session->run, GEAR_ID_BYTES);
	if (session->record.phase != _gear_escrow) return FALSE;
	next = session->record;
	/* Successful recovery leaves the actual carried pair ready to redeploy. */
	memmove(next.items + next.escrow_count, next.items, next.count * sizeof(next.items[0]));
	memcpy(next.items, next.escrow, next.escrow_count * sizeof(next.items[0]));
	next.count += next.escrow_count;
	for (index = 0; index < 2; index++) next.grenades[index] += next.escrow_grenades[index];
	gear_run_clear(&next, _gear_escaped);
	return gear_commit(session, &next);
}

void gear_track_reserved(struct gear_session const *session, long const object_indices[GAME_MODES_INVENTORY_WEAPONS])
{
	short index;
	if (session->record.phase != _gear_deployed) return;
	for (index = 0; index < session->record.reserved_count; index++)
	{
		long object_index = object_indices[index];
		long slot = DATUM_INDEX_TO_ABSOLUTE_INDEX(object_index);
		if (object_index != NONE && VALID_INDEX(slot, NUMBEROF(world_gear)))
		{
			world_gear[slot].object_index = object_index;
			memcpy(world_gear[slot].id, session->record.reserved[index].id, GEAR_ID_BYTES);
		}
	}
}

void gear_world_reset(void)
{
	memset(world_gear, 0, sizeof(world_gear));
}
#else
/* Desktop/server persistence is not advertised on unbridged console/guest
 * filesystems. Keep their existing game modes/builds without new host imports. */
void gear_profile_owner(char const *directory, byte owner[GEAR_ID_BYTES]) { memset(owner, 0, GEAR_ID_BYTES); }
void gear_account_owner(byte const account[GEAR_ID_BYTES], byte owner[GEAR_ID_BYTES]) { memset(owner, 0, GEAR_ID_BYTES); }
void gear_session_owner(long player_index, byte owner[GEAR_ID_BYTES]) { memset(owner, 0, GEAR_ID_BYTES); }
boolean gear_open_session(struct gear_session *session, byte const owner[GEAR_ID_BYTES]) { return FALSE; }
boolean gear_open(struct gear_session *session, byte const owner[GEAR_ID_BYTES]) { return FALSE; }
void gear_close(struct gear_session *session) { memset(session, 0, sizeof(*session)); session->descriptor = -1; }
boolean gear_loadout(struct gear_session *session, struct game_modes_inventory *inventory, boolean bring_stash) { return FALSE; }
boolean gear_item_available(struct gear_session const *session, struct gear_item const *item) { return FALSE; }
boolean gear_cycle_loadout(struct gear_session *session, short slot) { return FALSE; }
struct gear_item const *gear_loadout_choice(struct gear_session *session) { return NULL; }
boolean gear_discard(struct gear_session *session, byte const id[GEAR_ID_BYTES], unsigned long generation) { return FALSE; }
boolean gear_reserve(struct gear_session *session, boolean bring_stash) { return FALSE; }
boolean gear_confirm_deployment(struct gear_session *session) { return FALSE; }
boolean gear_cancel_reservation(struct gear_session *session) { return FALSE; }
boolean gear_lose(struct gear_session *session) { return FALSE; }
boolean gear_prepare_escape(struct gear_session *session, struct game_modes_inventory const *inventory,
	long const indices[GAME_MODES_INVENTORY_WEAPONS]) { return FALSE; }
boolean gear_finish_escape(struct gear_session *session) { return FALSE; }
void gear_track_reserved(struct gear_session const *session, long const indices[GAME_MODES_INVENTORY_WEAPONS]) {}
void gear_world_reset(void) {}
#endif
