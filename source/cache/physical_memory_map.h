/*
PHYSICAL_MEMORY_MAP.H

header included in hcex build.
*/

#ifndef __PHYSICAL_MEMORY_MAP_H
#define __PHYSICAL_MEMORY_MAP_H
#pragma once

/* ---------- constants */

/* port: the tag cache's size, which the loader checks a map's tag data and
structure bsps against (cache_files.c) */
#define TAG_CACHE_SIZE 0x1600000

/* port: the sound cache's size (the Xbox's 4 MB); a platform may define its
own, and the cache's pages follow it (xbox_sound_cache.c). A console that
answers for its memory class (halo_console_memory.h) has it at run time:
the Xbox's 4 MB, more on a console above its baseline */
/* Native 64-bit desktops can reserve more space for long custom audio.
 * Keep the inherited cache available if the larger allocation fails. */
#if defined(HALO_64BIT) && !defined(HALO_ANDROID) && !defined(HALO_CONSOLE_MEMORY_CLASS) && \
    defined(HALO_PORT_SOUND_CACHE_SIZE) && HALO_PORT_SOUND_CACHE_SIZE == 0x1000000
#define HALO_DESKTOP_LARGE_SOUND_CACHE_SIZE 0x2000000
#endif
#if defined(HALO_CONSOLE_MEMORY_CLASS) || defined(HALO_DESKTOP_LARGE_SOUND_CACHE_SIZE)
unsigned long physical_memory_get_sound_cache_size(void);

#define SOUND_CACHE_SIZE physical_memory_get_sound_cache_size()
#endif
#ifndef SOUND_CACHE_SIZE
#ifdef HALO_PORT_SOUND_CACHE_SIZE
/* (this fork: the native and browser builds' larger cache,
halo_port_capacity.h) */
#define SOUND_CACHE_SIZE HALO_PORT_SOUND_CACHE_SIZE
#else
#define SOUND_CACHE_SIZE 0x400000
#endif
#endif

/* ---------- macros */

/* ---------- structures */

/* ---------- prototypes/PHYSICAL_MEMORY_MAP.C */

void physical_memory_allocate(void);
void physical_memory_verify(void);
void physical_memory_free(void);

void *physical_memory_get_game_state_base_address(void);
void *physical_memory_get_tag_cache_base_address(void);
void *physical_memory_get_texture_cache_base_address(void);
void *physical_memory_get_sound_cache_base_address(void);

/* ---------- globals */

/* ---------- public code */

#endif // __PHYSICAL_MEMORY_MAP_H
