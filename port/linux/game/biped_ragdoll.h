/* Presentation-only ragdolls, adapted from Watanoge's Halo Content Evolved.
 * SPDX-License-Identifier: AGPL-3.0-or-later */
#ifndef BIPED_RAGDOLL_H
#define BIPED_RAGDOLL_H

#include "cseries.h"
#include "math/real_math.h"

#if !defined(HALO_SERVER) && !defined(HALO_CONSOLE) && !defined(HALO_XBOX_CONSOLE) && \
    !defined(HALO_ANDROID) && !defined(HALO_IOS) && !defined(HALO_WEB) && !defined(HALO_BROWSER) && !defined(HALO_SWITCH)
#define HALO_PRESENTATION_RAGDOLLS

void biped_ragdoll_update(long biped_index);
void biped_ragdoll_reset(void);
void biped_ragdoll_note_killing_blow(long biped_index, real_point3d const *epicenter,
	real_vector3d const *direction, real instantaneous_acceleration);
void biped_ragdoll_note_impact(long biped_index, real_point3d const *epicenter,
	real_vector3d const *acceleration);
real_matrix4x3 const *biped_ragdoll_drawn_nodes(long biped_index);
#endif

#endif
