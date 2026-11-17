; Original bounded faction/gear fixture. No mission or aircraft acceptance.
; SPDX-License-Identifier: AGPL-3.0-or-later
; Native controls/profiles are authored in the owned disposable CE scenario.
; The runtime owns per-player selection, deployment, activation and banking.

; Reserved native queries. Stock runtimes return no object from this fallback.
; Mission logic uses a nonempty list before testing objectives for a squad.
(script static object_list forerunner_squad_01_players
  (list_get (players) 32767)
)

(script static object_list forerunner_squad_02_players
  (list_get (players) 32767)
)

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

; Plain HMT notices are sent only to current living verified squad participants.
; Author sq01_recover/sq01_return/sq02_recover/sq02_return in scenario HUD text.
(script static hud_message forerunner_squad_01_notice
  (if (= (device_group_get extraction_squad_01_ready) 1) sq01_return sq01_recover)
)
(script static hud_message forerunner_squad_02_notice
  (if (= (device_group_get extraction_squad_02_ready) 1) sq02_return sq02_recover)
)
