/* Native timerless Extraction interactions. Gameplay acceptance UNVERIFIED.
   SPDX-License-Identifier: AGPL-3.0-or-later */
#ifndef HALO_EXTRACTION_H
#define HALO_EXTRACTION_H

void extraction_reset(void);
/* A restored world may contain items already committed to a stash. Banking
   stays disabled until a fresh map replaces that world. */
void extraction_restored(void);
void extraction_update(void);
/* A valid opted-in hub/run stays on its map for return and redeployment.
   Requires native hub markers; remote gear needs a host-bound identity.
   Maps naming extraction_pelican/extraction_spirit additionally require a real
   assigned seat at the LZ, continuous boarding to the return hub and an ended
   native cinematic. Transport gameplay remains UNVERIFIED. */
boolean extraction_owns_map(void);
/* Optional reserved static object_list helpers, compiled as ordinary HSC.
 * TRUE handles the name, returning only this operation's living participants
 * or NONE when unavailable. Requires exact authored co-op script policy. */
boolean extraction_script_players(char const *name, long *object_list);
/* TRUE handles a real host-owned wipe without rewinding the world. If allocation
   fails, the caller retains its native loss timer and retries. */
boolean extraction_respawn_after_loss(void);
/* Optional authored native character/profile for this faction's hub respawn.
 * False preserves the ordinary campaign/multiplayer spawn definition. */
boolean extraction_player_spawn_profile(long player_index, long *biped, short *profile, short *variant);
/* Called by the native control-use path. TRUE consumes refused or storage
 * actions. FALSE preserves ordinary controls and the authored state/effect
 * transition after successful deployment or activation, for native HaloScript. */
boolean extraction_control_touched(long control_index, long unit_index);
/* Host-only admission for opted-in named transports. Ordinary vehicles, NPC
 * pilots and authoritative client seat replay keep the native path. */
boolean extraction_transport_can_board(long unit_index, long vehicle_index);
/* The existing Firefight director delays lobby return only while a living
 * winner's shared gear recovery is pending. No extra wave/lives director. */
boolean extraction_survival_settlement_pending(void);
/* Existing director result gate: every living player recovered and physically
 * in the authored return hub. False for missing bindings, zero survivors,
 * pending writes, failed/ended encounters or a restored world. */
boolean extraction_survival_evacuation_ready(void);

struct player_action;
/* Optional native medical control: downed is alive, never a cleared dead bit.
 * Epoch/tick metadata reuses negotiated Delta object sync; legacy stays stock. */
boolean extraction_medical_incapacitated(long unit_index);
/* Read-only local guidance and host notices share actual medical ownership and
 * native allegiance. No spectator/enemy positions or new network state. */
boolean extraction_medical_ally_incapacitated(long viewer_unit, long target_unit);
boolean extraction_medical_predictable(short absolute_player_index);
boolean extraction_medical_network_enabled(void);
void extraction_medical_network_state(long unit_index, boolean downed, unsigned int epoch, unsigned short tick);
void extraction_medical_control(long player_index, struct player_action *action);
boolean extraction_medical_lethal(long unit_index);
void extraction_medical_damaged(long unit_index);

#endif
