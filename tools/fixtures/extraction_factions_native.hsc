; Original bounded faction/gear fixture. No mission or aircraft acceptance.
; SPDX-License-Identifier: AGPL-3.0-or-later
; Native controls/profiles are authored in the owned disposable CE scenario.
; The runtime owns per-player selection, deployment, activation and banking.

(script startup faction_hub_ready
  (ai_allegiance player human)
  (print "faction fixture: native hub ready")
)

(script dormant forerunner_extraction_complete
  (print "faction fixture: native recovery complete")
)
