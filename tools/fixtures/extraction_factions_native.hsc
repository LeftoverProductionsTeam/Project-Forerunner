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

; Optional squad hub terminals select membership in the runtime, not in globals.
; These reusable callbacks may run while another squad remains in the field.
(script continuous forerunner_squad_01_complete
  (sleep -1)
  (print "faction fixture: squad 01 durable recovery complete")
)

(script continuous forerunner_squad_02_complete
  (sleep -1)
  (print "faction fixture: squad 02 durable recovery complete")
)
