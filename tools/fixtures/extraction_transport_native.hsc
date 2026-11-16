; Original Forerunner native aircraft fixture; no game assets included.
; SPDX-License-Identifier: AGPL-3.0-or-later
; Uses native AI driving, seats, controls and cameras. Players board normally;
; vehicle_load_magic below loads only the Marine pilot, never a player.
; Not Spark's Black Echo, faction selection or final transport acceptance.
; Native scenario requirements: named Pelican/Spirit, native passenger seats,
; Marine native_squad, hub/LZ volumes, deploy/activate controls, two cameras
; and dedicated native device groups extraction_pelican_boarded and
; extraction_pelican_called and extraction_pelican_landed (initially zero,
; not bound to control power/position). Landed is native ground/attitude feedback.
; tf_field/home require aircraft-scale cruise radii and precise final landing
; points in aircraft world-origin coordinates; validate each physical touchdown
; against its unchanged pad volume. Both cameras use native object-relative offsets.
; tf_field/home are authored native flight commands; tf_wait uses a short
; pause followed by a native loop, not a pause that overflows a 16-bit timer.

(global boolean tf_banked false)
(global boolean tf_initialized false)

(script static boolean tf_player_seated
  (or
    (vehicle_test_seat_list extraction_pelican "P-riderLF" (players))
    (vehicle_test_seat_list extraction_pelican "P-riderRF" (players))
    (vehicle_test_seat_list extraction_pelican "P-riderLM" (players))
    (vehicle_test_seat_list extraction_pelican "P-riderRM" (players))
    (vehicle_test_seat_list extraction_pelican "P-riderLB" (players))
    (vehicle_test_seat_list extraction_pelican "P-riderRB" (players))
    (vehicle_test_seat_list extraction_pelican "P-riderLB01" (players))
    (vehicle_test_seat_list extraction_pelican "P-riderRB01" (players))
    (vehicle_test_seat_list extraction_pelican "P-riderLB02" (players))
    (vehicle_test_seat_list extraction_pelican "P-riderRB02" (players))
  )
)

(script continuous forerunner_pelican_complete
  ; Park first, including on map start. Only durable native settlement wakes us.
  (sleep -1)
  (set tf_banked true)
  (print "transport: Pelican crew native equipment banked")
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
  (camera_set_relative tf_insertion 0 extraction_pelican)
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
  (camera_set_relative tf_departure 0 extraction_pelican)
  (sleep 120)
  (camera_control false)
  (player_enable_input true)
  (cinematic_stop)
  (print "transport: departure camera complete")
)

(script startup tf_initialize
  (ai_allegiance player human)
  (device_set_power extraction_activate 0)
  (unit_set_enterable_by_player extraction_pelican false)
  (vehicle_hover extraction_spirit true)
  (unit_open extraction_pelican)
  (unit_open extraction_spirit)
  (unit_set_enterable_by_player extraction_spirit true)
  (ai_place be_marines/native_squad)
  (ai_set_team be_marines human)
  (vehicle_load_magic extraction_pelican "P-driver" (ai_actors be_marines/native_squad))
  (sleep_until (vehicle_test_seat_list extraction_pelican "P-driver" (ai_actors be_marines/native_squad)) 1)
  (print "transport: native Marine pilot physically seated")
  (ai_command_list be_marines/native_squad tf_wait)
  (sleep_until (>= (device_group_get extraction_pelican_landed) 0.5) 1)
  (vehicle_hover extraction_pelican true)
  (unit_set_enterable_by_player extraction_pelican true)
  (set tf_initialized true)
)

(script continuous tf_operation
  (sleep_until tf_initialized 1)
  (set tf_banked false)
  (device_set_power extraction_activate 0)
  (device_set_position extraction_activate 0)
  (unit_open extraction_pelican)
  (unit_set_enterable_by_player extraction_pelican true)
  (print "transport: prepare loadout at deployment control")
  ; Real confirmed gear and seats drive readiness, including a crew that
  ; prepared its next loadout before this coordinator reached the next cycle.
  (print "transport: board Pelican passenger seat for insertion")
  (sleep_until (and (tf_player_seated)
    (>= (device_group_get extraction_pelican_boarded) 0.5)) 1)
  (print "transport: insertion passenger physically seated")
  (unit_set_enterable_by_player extraction_pelican false)
  (unit_close extraction_pelican)
  (vehicle_hover extraction_pelican false)
  (ai_command_list be_marines/native_squad tf_field)
  ; Watch the landing volume while the camera plays. A blocking camera call
  ; can miss the landing altogether on short native flight paths.
  (wake tf_insertion_camera)
  (sleep_until (and (volume_test_objects extraction_zone extraction_pelican)
    (>= (device_group_get extraction_pelican_landed) 0.5)) 1)
  (ai_command_list be_marines/native_squad tf_wait)
  (vehicle_hover extraction_pelican true)
  (unit_open extraction_pelican)
  (print "transport: insertion landed; disembark normally")
  (unit_set_enterable_by_player extraction_pelican true)
  (sleep_until (not (tf_player_seated)) 1)
  (print "transport: passenger disembarked; native pilot departing")
  ; Keep calls available while the empty craft returns to staging. A real
  ; per-player request stays asserted until pickup; no extra queue is needed.
  (device_set_power extraction_activate 1)
  (unit_set_enterable_by_player extraction_pelican false)
  (unit_close extraction_pelican)
  (vehicle_hover extraction_pelican false)
  (ai_command_list be_marines/native_squad tf_home)
  (sleep_until (and (volume_test_objects extraction_return extraction_pelican)
    (>= (device_group_get extraction_pelican_landed) 0.5)) 1)
  (ai_command_list be_marines/native_squad tf_wait)
  (vehicle_hover extraction_pelican true)
  (print "transport: empty craft staged; waiting for extraction request")
  ; A shared call control can be used by either faction. Only a real current
  ; UNSC request may recall the Pelican; Spirit scripts consume their own
  ; extraction_spirit_called group through the same native runtime path.
  (sleep_until (>= (device_group_get extraction_pelican_called) 0.5) 1)
  (print "transport: extraction called; native pilot approaching")
  ; Native activation continues while the pilot approaches. Boarding readiness
  ; still requires every requesting passenger's countdown and actual seat;
  ; this call signal alone never authorizes departure or recovery.
  (vehicle_hover extraction_pelican false)
  (ai_command_list be_marines/native_squad tf_field)
  (sleep_until (and (volume_test_objects extraction_zone extraction_pelican)
    (>= (device_group_get extraction_pelican_landed) 0.5)) 1)
  (ai_command_list be_marines/native_squad tf_wait)
  (vehicle_hover extraction_pelican true)
  (print "transport: extraction landed; board passenger seat")
  (unit_set_enterable_by_player extraction_pelican true)
  (unit_open extraction_pelican)
  (sleep_until (and (tf_player_seated)
    (>= (device_group_get extraction_pelican_boarded) 0.5)) 1)
  (print "transport: extraction passenger physically seated")
  (unit_set_enterable_by_player extraction_pelican false)
  (unit_close extraction_pelican)
  (vehicle_hover extraction_pelican false)
  (ai_command_list be_marines/native_squad tf_home)
  (wake tf_departure_camera)
  (sleep_until (and (volume_test_objects extraction_return extraction_pelican)
    (>= (device_group_get extraction_pelican_landed) 0.5)) 1)
  (ai_command_list be_marines/native_squad tf_wait)
  (vehicle_hover extraction_pelican true)
  (unit_open extraction_pelican)
  (sleep_until tf_banked 1)
  ; Keep boarding closed until the old crew has left. Earlier passengers may
  ; prepare their next loadout without reboarding before debrief is complete.
  (print "transport: arrived and banked; disembark and prepare the next operation")
  (sleep_until (not (tf_player_seated)) 1)
  (print "transport: crew disembarked; next deployment ready")
)
