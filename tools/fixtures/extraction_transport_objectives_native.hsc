; Original bounded native two-squad combat/salvage operation.
; SPDX-License-Identifier: AGPL-3.0-or-later
; Compose with the existing transport controller and squad player queries.
; Native encounters sqop_flood_01/02 carry the loaded salvage weapon variant.
; Both encounters require native Flood allegiance/navigation and initially-not-
; placed squads. Preserve native difficulty/co-op actor scaling.
; field_salvage is a distinct loaded weapon tag dropped by those actors, not
; the starting weapon's tag path. It must be compatible with the player biped.
; Distinct extraction_squad_01_ready/02_ready groups start at zero and are not
; control power/position groups. Compile the accompanying native HUD messages.
; Each ready group requires its native extraction_squad_01/02 hub terminal.
; One authored operation: subsequent mission playthroughs are not implemented.
; The built-in Firefight director must remain OFF. No script grants/banks gear.

(global short sqop_stage 0)
(global short sqop_scan_01 0)
(global short sqop_scan_02 0)
(global object_list sqop_players_01 (list_get (players) 32767))
(global object_list sqop_players_02 (list_get (players) 32767))

(script static hud_message forerunner_squad_01_notice
  (if (= sqop_stage 1) sqop_fight
    (if (>= (device_group_get extraction_squad_01_ready) 1)
      sqop_ready sqop_recover))
)

(script static hud_message forerunner_squad_02_notice
  (if (= sqop_stage 1) sqop_fight
    (if (>= (device_group_get extraction_squad_02_ready) 1)
      sqop_ready sqop_recover))
)

(script startup sqop_contact
  ; Both real squads must reach the field and leave the aircraft first.
  (sleep_until (and
    (> (list_count (forerunner_squad_01_players)) 0)
    (> (list_count (forerunner_squad_02_players)) 0)
    (volume_test_objects_all extraction_zone (forerunner_squad_01_players))
    (volume_test_objects_all extraction_zone (forerunner_squad_02_players))
    (not (tf_player_seated))) 15)
  (set sqop_stage 1)
  (forerunner_squad_01_notice)
  (forerunner_squad_02_notice)
  (print "operation: field contact incoming")
  ; Warn before contact and let the empty aircraft clear its native route.
  (sleep 90)
  (ai_force_active sqop_flood_01 true)
  (ai_force_active sqop_flood_02 true)
  (ai_allow_dormant sqop_flood_01 false)
  (ai_allow_dormant sqop_flood_02 false)
  (ai_place sqop_flood_01)
  (ai_place sqop_flood_02)
  (sleep_until (and (> (ai_living_count sqop_flood_01) 0)
    (> (ai_living_count sqop_flood_02) 0)) 1)
  (print "operation: native field Flood active")
  (sleep_until (and (= (ai_living_count sqop_flood_01) 0)
    (= (ai_living_count sqop_flood_02) 0)) 15)
  (set sqop_stage 2)
  (ai_allow_dormant sqop_flood_01 true)
  (ai_allow_dormant sqop_flood_02 true)
  (ai_force_active sqop_flood_01 false)
  (ai_force_active sqop_flood_02 false)
  (forerunner_squad_01_notice)
  (forerunner_squad_02_notice)
  (print "operation: native field Flood cleared; recover their salvage")
)

(script startup sqop_salvage_01
  (sleep_until (= sqop_stage 2) 15)
  (sleep_until (begin
    (set sqop_players_01 (forerunner_squad_01_players))
    (if (>= sqop_scan_01 (list_count sqop_players_01)) (set sqop_scan_01 0))
    (if (and (< sqop_scan_01 (list_count sqop_players_01))
      (unit_has_weapon (unit (list_get sqop_players_01 sqop_scan_01))
        "forerunner\operations\field_salvage"))
      (device_group_set_immediate extraction_squad_01_ready 1))
    (set sqop_scan_01 (+ sqop_scan_01 1))
    (>= (device_group_get extraction_squad_01_ready) 1)) 15)
  (forerunner_squad_01_notice)
  (print "operation: squad 01 owns native field salvage")
)

(script startup sqop_salvage_02
  (sleep_until (= sqop_stage 2) 15)
  (sleep_until (begin
    (set sqop_players_02 (forerunner_squad_02_players))
    (if (>= sqop_scan_02 (list_count sqop_players_02)) (set sqop_scan_02 0))
    (if (and (< sqop_scan_02 (list_count sqop_players_02))
      (unit_has_weapon (unit (list_get sqop_players_02 sqop_scan_02))
        "forerunner\operations\field_salvage"))
      (device_group_set_immediate extraction_squad_02_ready 1))
    (set sqop_scan_02 (+ sqop_scan_02 1))
    (>= (device_group_get extraction_squad_02_ready) 1)) 15)
  (forerunner_squad_02_notice)
  (print "operation: squad 02 owns native field salvage")
)
