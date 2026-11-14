/* Native timerless Extraction interactions. Source only, UNVERIFIED.
   SPDX-License-Identifier: AGPL-3.0-or-later */
#ifndef HALO_EXTRACTION_H
#define HALO_EXTRACTION_H

void extraction_reset(void);
void extraction_update(void);
/* Called only by the native control-use path. TRUE consumes this named
   Extraction control; ordinary controls keep their existing behavior. */
boolean extraction_control_touched(long control_index, long unit_index);

#endif
