/*
PROJECTILE_DEFINITIONS.C

symbols in this file:
00306A90 00a0:
	_default_projectile_material_response (0000)
*/

/* ---------- headers */

#include "cseries.h"
#include "projectile_definitions.h"
#include "cache/cache_files.h"

#include "effects/effect_definitions.h"

/* ---------- constants */

/* ---------- macros */

/* ---------- structures */

/* ---------- prototypes */

/* ---------- globals */

struct projectile_material_response_definition default_projectile_material_response =
{
	0,
	_projectile_material_response_disappear,
#ifdef HALO_64BIT
	{ EFFECT_DEFINITION_TAG, 0, 0, NONE },
#else
	{ EFFECT_DEFINITION_TAG, "", 0, NONE },
#endif
	{ 0 },
	_projectile_material_response_disappear,
	0,
	0.0f,
	0.0f,
	0.0f,
	0.0f,
	0.0f,
#ifdef HALO_64BIT
	{ EFFECT_DEFINITION_TAG, 0, 0, NONE },
#else
	{ EFFECT_DEFINITION_TAG, "", 0, NONE },
#endif
	{ 0 },
	_projectile_material_effect_scale_damage,
	0,
	0.0f,
	0.0f,
#ifdef HALO_64BIT
	{ EFFECT_DEFINITION_TAG, 0, 0, NONE },
#else
	{ EFFECT_DEFINITION_TAG, "", 0, NONE },
#endif
	{ 0 },
	0.0f,
	0.0f,
	0.0f,
	0.0f,
};

/* ---------- public code ---------- */

struct projectile_definition *projectile_definition_try_get(long index)
{
	/* A weapon trigger can also launch a native object such as a vehicle. */
	if (index == NONE || tag_get_group_tag(index) != PROJECTILE_DEFINITION_TAG)
		return NULL;
	return projectile_definition_get(index);
}

/* ---------- private code */
