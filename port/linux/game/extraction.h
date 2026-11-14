/* Native timerless Extraction interactions. Source only, UNVERIFIED.
   SPDX-License-Identifier: AGPL-3.0-or-later */
#ifndef HALO_EXTRACTION_H
#define HALO_EXTRACTION_H

void extraction_reset(void);
void extraction_update(void);
/* A valid opted-in hub/run stays on its map for return and redeployment.
   Does not admit missing markers, multiplayer or ordinary campaign maps. */
boolean extraction_owns_map(void);
/* TRUE handles a real local wipe without rewinding the world. If allocation
   fails, the caller retains its native loss timer and retries. */
boolean extraction_respawn_after_loss(void);
/* Called only by the native control-use path. TRUE consumes this named
   Extraction control; ordinary controls keep their existing behavior. */
boolean extraction_control_touched(long control_index, long unit_index);

#endif
