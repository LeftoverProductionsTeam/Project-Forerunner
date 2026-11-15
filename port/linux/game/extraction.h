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
   Requires native hub markers; remote gear needs a host-bound identity. */
boolean extraction_owns_map(void);
/* TRUE handles a real host-owned wipe without rewinding the world. If allocation
   fails, the caller retains its native loss timer and retries. */
boolean extraction_respawn_after_loss(void);
/* Called only by the native control-use path. TRUE consumes this named
   Extraction control; ordinary controls keep their existing behavior. */
boolean extraction_control_touched(long control_index, long unit_index);
/* The existing Firefight director delays lobby return only while a living
 * winner's shared gear recovery is pending. No extra wave/lives director. */
boolean extraction_survival_settlement_pending(void);

struct player_action;
/* Optional native medical control: downed is alive, never a cleared dead bit.
 * Epoch/tick metadata reuses negotiated Delta object sync; legacy stays stock. */
boolean extraction_medical_incapacitated(long unit_index);
boolean extraction_medical_predictable(short absolute_player_index);
boolean extraction_medical_network_enabled(void);
void extraction_medical_network_state(long unit_index, boolean downed, unsigned int epoch, unsigned short tick);
void extraction_medical_control(long player_index, struct player_action *action);
boolean extraction_medical_lethal(long unit_index);
void extraction_medical_damaged(long unit_index);

#endif
