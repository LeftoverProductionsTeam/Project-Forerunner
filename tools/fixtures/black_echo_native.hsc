; Disposable native gameplay fixture, not Spark's accepted Black Echo map.
; Uses the same ordinary encounters, manual objective setters and control names
; reviewed in Spark d50b6fa35c747498036a31ba830670665585aee7. No JSON importer.
; Original Forerunner fixture source: AGPL-3.0-or-later.

(global boolean gs_manual_4_done false)
(global boolean gs_manual_7_done false)
(global boolean be_rescue_failed false)
(global short be_collector_index 0)

(script static void gs_complete_4 (set gs_manual_4_done true))
(script static void gs_complete_7 (set gs_manual_7_done true))

; The runtime may wake this only after actual authoritative gear settlement.
; It is a one-shot map completion notification, not a repeatable reward writer.
(script dormant forerunner_extraction_complete
  (gs_complete_7)
  (object_destroy be_technology)
  (print "be stage: equipment banked and survivors returned")
)

(script startup be_operation
  ; Solo scenarios initialize only same-team allegiances; Marines need this.
  (ai_allegiance player human)
  (device_set_power extraction_activate 0)
  (print "be stage: awaiting deployment terminal")
  (sleep_until (>= (device_get_position extraction_deploy) 0.5) 1)
  (print "be stage: deployed")
  (ai_place be_marines)
  (sleep_until (> (ai_living_count be_marines) 0) 15 300)
  (ai_set_team be_marines human)
  (ai_place be_covenant)
  (sleep_until (> (ai_living_count be_covenant) 0) 1)
  (print "be stage: native hostile encounter active")
  (sleep_until (= (ai_living_count be_covenant) 0) 15)
  (print "be stage: facility defenders eliminated")
  ; Native squad migration walks the existing actors to authored hub positions.
  (ai_migrate be_marines be_marines/be_return)
  (sleep_until (or (< (ai_living_count be_marines) 1)
    (volume_test_objects_all be_survivors (ai_actors be_marines))) 15)
  (if (< (ai_living_count be_marines) 1) (begin
    (set be_rescue_failed true)
    (print "be stage: rescue failed - alternate evacuation")
  ))
  (if (not be_rescue_failed) (print "be stage: surviving marines rescued"))
  (object_create be_technology)
  (print "be stage: collect classified technology")
  ; Scan one native player per tick, including late joiners and larger crews.
  ; Ownership still completes this shared authored objective, not volume entry.
  (sleep_until (begin
    (if (>= be_collector_index (list_count (players))) (set be_collector_index 0))
    (if (and (< be_collector_index (list_count (players)))
      (unit_has_weapon (unit (list_get (players) be_collector_index)) "forerunner\be_native\technology"))
      (gs_complete_4))
    (set be_collector_index (+ be_collector_index 1))
    gs_manual_4_done
  ) 1)
  (print "be stage: technology collected by native weapon ownership")
  (if (volume_test_objects be_containment (players)) (begin
    ; Script-owned waves use native activation, not the runtime wave director.
    ; Keep the outbreak awake even outside a player's current activation PVS.
    (ai_force_active be_flood true)
    (ai_allow_dormant be_flood false)
    (ai_place be_flood)
    (sleep_until (> (ai_living_count be_flood) 0) 1)
    (print "be stage: optional flood outbreak active")
    (sleep_until (= (ai_living_count be_flood) 0) 15)
    (ai_allow_dormant be_flood true)
    (ai_force_active be_flood false)
    (print "be stage: outbreak contained")
  ))
  (ai_place be_defense)
  ; Observe the spawn before a short-lived defender can die between polls.
  (sleep_until (> (ai_living_count be_defense) 0) 1)
  (print "be stage: defend extraction")
  (sleep_until (= (ai_living_count be_defense) 0) 15)
  (device_set_power extraction_activate 1)
  (print "be stage: activate extraction and return to hub")
  (sleep_until gs_manual_7_done 1)
  (print "be stage: operation complete - native redeployment remains available")
)
