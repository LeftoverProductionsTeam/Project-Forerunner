; Original Forerunner native aircraft fixture; no game assets included.
; SPDX-License-Identifier: AGPL-3.0-or-later
; Uses native AI driving, seats, controls and cameras. Players board normally;
; vehicle_load_magic below loads only the authored AI pilot, never a player.
; Not Spark's Black Echo, faction selection or final transport acceptance.
; Native scenario requirements: named Pelican/Spirit, native passenger seats,
; authored pilot squad, hub/LZ volumes, deploy/activate controls, two cameras
; and dedicated matching boarded/called/landed device groups (initially zero,
; not bound to control power/position). Landed is native ground/attitude feedback.
; tf_field/home require aircraft-scale cruise radii and precise final landing
; points in aircraft world-origin coordinates; validate each physical touchdown
; against its unchanged pad volume. Both cameras use native object-relative offsets.
; tf_field/home are authored native flight commands; tf_wait uses a short
; pause followed by a native loop, not a pause that overflows a 16-bit timer.

; Author one craft per fixture using these native bindings. For the Spirit use
; extraction_spirit, its matching *_boarded/called/landed groups,
; be_covenant/native_squad, covenant and "c_dropship driver". Both crafts use
; the same flight/boarding/banking sequence; this is not a second director.
; The pilot's animation graph must support its driver's exact seat label.
; Stock Grunts have Spirit passenger poses, not a c_dropship driver pose; use
; an authored compatible pilot variant rather than bypassing native admission.
; Globals initialize before objects_place. Resolve the native name on each call,
; so loading and native object recreation cannot leave a cached missing/stale ship.
(script static vehicle tf_ship extraction_pelican)
(global ai tf_pilot be_marines/native_squad)
(global team tf_pilot_team human)
(global string tf_driver_seat "P-driver")
(global device_group tf_boarded extraction_pelican_boarded)
(global device_group tf_called extraction_pelican_called)
(global device_group tf_landed extraction_pelican_landed)

(global boolean tf_banked false)
(global boolean tf_initialized false)

(script static boolean tf_player_seated
  (or
    (vehicle_test_seat_list (tf_ship) "P-riderLF" (players))
    (vehicle_test_seat_list (tf_ship) "P-riderRF" (players))
    (vehicle_test_seat_list (tf_ship) "P-riderLM" (players))
    (vehicle_test_seat_list (tf_ship) "P-riderRM" (players))
    (vehicle_test_seat_list (tf_ship) "P-riderLB" (players))
    (vehicle_test_seat_list (tf_ship) "P-riderRB" (players))
    (vehicle_test_seat_list (tf_ship) "P-riderLB01" (players))
    (vehicle_test_seat_list (tf_ship) "P-riderRB01" (players))
    (vehicle_test_seat_list (tf_ship) "P-riderLB02" (players))
    (vehicle_test_seat_list (tf_ship) "P-riderRB02" (players))
    (vehicle_test_seat_list (tf_ship) "CD-passengerL01" (players))
    (vehicle_test_seat_list (tf_ship) "CD-passengerL02" (players))
    (vehicle_test_seat_list (tf_ship) "CD-passengerL03" (players))
    (vehicle_test_seat_list (tf_ship) "CD-passengerL04" (players))
    (vehicle_test_seat_list (tf_ship) "CD-passengerR01" (players))
    (vehicle_test_seat_list (tf_ship) "CD-passengerR02" (players))
    (vehicle_test_seat_list (tf_ship) "CD-passengerR03" (players))
    (vehicle_test_seat_list (tf_ship) "CD-passengerR04" (players))
  )
)

(script continuous forerunner_pelican_complete
  ; Park first, including on map start. Only durable native settlement wakes us.
  (sleep -1)
  (set tf_banked true)
  (print "transport: Pelican crew native equipment banked")
)

(script continuous forerunner_spirit_complete
  (sleep -1)
  (set tf_banked true)
  (print "transport: Spirit crew native equipment banked")
)

(script dormant forerunner_extraction_complete
  (print "transport: all surviving crews native equipment banked")
)

(script continuous tf_insertion_camera
  (sleep -1)
  (print "transport: insertion camera started")
  (cinematic_start)
  (player_enable_input false)
  (camera_control true)
  (camera_set_relative tf_insertion 0 (tf_ship))
  (sleep 120)
  (camera_control false)
  (player_enable_input true)
  (cinematic_stop)
  (print "transport: insertion camera complete")
)

(script continuous tf_departure_camera
  (sleep -1)
  (print "transport: departure camera started")
  (cinematic_start)
  (player_enable_input false)
  (camera_control true)
  (camera_set_relative tf_departure 0 (tf_ship))
  (sleep 120)
  (camera_control false)
  (player_enable_input true)
  (cinematic_stop)
  (print "transport: departure camera complete")
)

(script startup tf_initialize
  (ai_allegiance player tf_pilot_team)
  (device_set_power extraction_activate 0)
  (vehicle_hover extraction_pelican true)
  (vehicle_hover extraction_spirit true)
  (unit_set_enterable_by_player extraction_pelican false)
  (unit_set_enterable_by_player extraction_spirit false)
  (vehicle_hover (tf_ship) false)
  (unit_open (tf_ship))
  (ai_place tf_pilot)
  (ai_set_team tf_pilot tf_pilot_team)
  (vehicle_load_magic (tf_ship) tf_driver_seat (ai_actors tf_pilot))
  (sleep_until (vehicle_test_seat_list (tf_ship) tf_driver_seat (ai_actors tf_pilot)) 1)
  (print "transport: native pilot physically seated")
  (ai_command_list tf_pilot tf_wait)
  (sleep_until (>= (device_group_get tf_landed) 0.5) 1)
  (vehicle_hover (tf_ship) true)
  (unit_set_enterable_by_player (tf_ship) true)
  (set tf_initialized true)
)

(script continuous tf_operation
  (sleep_until tf_initialized 1)
  (set tf_banked false)
  (device_set_power extraction_activate 0)
  (device_set_position extraction_activate 0)
  (unit_open (tf_ship))
  (unit_set_enterable_by_player (tf_ship) true)
  (print "transport: prepare loadout at deployment control")
  ; Real confirmed gear and seats drive readiness, including a crew that
  ; prepared its next loadout before this coordinator reached the next cycle.
  (print "transport: board assigned passenger seat for insertion")
  (sleep_until (and (tf_player_seated)
    (>= (device_group_get tf_boarded) 0.5)) 1)
  (print "transport: insertion passenger physically seated")
  (unit_set_enterable_by_player (tf_ship) false)
  (unit_close (tf_ship))
  (vehicle_hover (tf_ship) false)
  (ai_command_list tf_pilot tf_field)
  ; Watch the landing volume while the camera plays. A blocking camera call
  ; can miss the landing altogether on short native flight paths.
  (wake tf_insertion_camera)
  (sleep_until (and (volume_test_object extraction_zone (tf_ship))
    (>= (device_group_get tf_landed) 0.5)) 1)
  (ai_command_list tf_pilot tf_wait)
  (vehicle_hover (tf_ship) true)
  (unit_open (tf_ship))
  (print "transport: insertion landed; disembark normally")
  (unit_set_enterable_by_player (tf_ship) true)
  (sleep_until (not (tf_player_seated)) 1)
  (print "transport: passenger disembarked; native pilot departing")
  ; Keep calls available while the empty craft returns to staging. A real
  ; per-player request stays asserted until pickup; no extra queue is needed.
  (device_set_power extraction_activate 1)
  (unit_set_enterable_by_player (tf_ship) false)
  (unit_close (tf_ship))
  (vehicle_hover (tf_ship) false)
  (ai_command_list tf_pilot tf_home)
  (sleep_until (and (volume_test_object extraction_return (tf_ship))
    (>= (device_group_get tf_landed) 0.5)) 1)
  (ai_command_list tf_pilot tf_wait)
  (vehicle_hover (tf_ship) true)
  (print "transport: empty craft staged; waiting for extraction request")
  ; A shared call control can be used by either faction. Only a real current
  ; request for this assigned craft may recall it. The matching call group
  ; comes from the same native faction/ownership rules for Pelican or Spirit.
  (sleep_until (>= (device_group_get tf_called) 0.5) 1)
  (print "transport: extraction called; native pilot approaching")
  ; Native activation continues while the pilot approaches. Boarding readiness
  ; still requires every requesting passenger's countdown and actual seat;
  ; this call signal alone never authorizes departure or recovery.
  (vehicle_hover (tf_ship) false)
  (ai_command_list tf_pilot tf_field)
  (sleep_until (and (volume_test_object extraction_zone (tf_ship))
    (>= (device_group_get tf_landed) 0.5)) 1)
  (ai_command_list tf_pilot tf_wait)
  (vehicle_hover (tf_ship) true)
  (print "transport: extraction landed; board passenger seat")
  (unit_set_enterable_by_player (tf_ship) true)
  (unit_open (tf_ship))
  (sleep_until (and (tf_player_seated)
    (>= (device_group_get tf_boarded) 0.5)) 1)
  (print "transport: extraction passenger physically seated")
  (unit_set_enterable_by_player (tf_ship) false)
  (unit_close (tf_ship))
  (vehicle_hover (tf_ship) false)
  (ai_command_list tf_pilot tf_home)
  (wake tf_departure_camera)
  (sleep_until (and (volume_test_object extraction_return (tf_ship))
    (>= (device_group_get tf_landed) 0.5)) 1)
  (ai_command_list tf_pilot tf_wait)
  (vehicle_hover (tf_ship) true)
  (unit_open (tf_ship))
  (sleep_until tf_banked 1)
  ; Keep boarding closed until the old crew has left. Earlier passengers may
  ; prepare their next loadout without reboarding before debrief is complete.
  (print "transport: arrived and banked; disembark and prepare the next operation")
  (sleep_until (not (tf_player_seated)) 1)
  (print "transport: crew disembarked; next deployment ready")
)
