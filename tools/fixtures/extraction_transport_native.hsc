; Original Forerunner native aircraft fixture; no game assets included.
; SPDX-License-Identifier: AGPL-3.0-or-later
; Uses native AI driving, seats, controls and cameras. Players board normally;
; vehicle_load_magic below loads only the Marine pilot, never a player.
; Not Spark's Black Echo, faction selection or final transport acceptance.
; Native scenario requirements: named Pelican/Spirit, front passenger seats,
; Marine native_squad, hub/LZ volumes, deploy/activate controls and two cameras.
; tf_field/home/hold are authored native flight commands; tf_wait uses a short
; pause followed by a native loop, not a pause that overflows a 16-bit timer.

(global boolean tf_banked false)

(script static boolean tf_player_seated
  (or
    (vehicle_test_seat_list extraction_pelican "P-riderLF" (players))
    (vehicle_test_seat_list extraction_pelican "P-riderRF" (players))
  )
)

(script dormant forerunner_extraction_complete
  (set tf_banked true)
  (print "transport: native equipment banked")
)

(script dormant tf_insertion_camera
  (cinematic_start)
  (player_enable_input false)
  (camera_control true)
  (camera_set tf_insertion 0)
  (sleep 120)
  (camera_control false)
  (player_enable_input true)
  (cinematic_stop)
)

(script dormant tf_departure_camera
  (cinematic_start)
  (player_enable_input false)
  (camera_control true)
  (camera_set tf_departure 0)
  (sleep 120)
  (camera_control false)
  (player_enable_input true)
  (cinematic_stop)
)

(script startup tf_operation
  (ai_allegiance player human)
  (device_set_power extraction_activate 0)
  (vehicle_hover extraction_pelican true)
  (vehicle_hover extraction_spirit true)
  (unit_open extraction_pelican)
  (unit_open extraction_spirit)
  (unit_set_enterable_by_player extraction_pelican true)
  (unit_set_enterable_by_player extraction_spirit true)
  (ai_place be_marines/native_squad)
  (ai_set_team be_marines human)
  (vehicle_load_magic extraction_pelican "P-driver" (ai_actors be_marines/native_squad))
  (sleep_until (vehicle_test_seat_list extraction_pelican "P-driver" (ai_actors be_marines/native_squad)) 1)
  (print "transport: native Marine pilot physically seated")
  (ai_command_list be_marines/native_squad tf_wait)
  (print "transport: prepare loadout at deployment control")
  (sleep_until (>= (device_get_position extraction_deploy) 0.5) 1)
  (print "transport: board Pelican passenger seat for insertion")
  (sleep_until (tf_player_seated) 1)
  (print "transport: insertion passenger physically seated")
  (unit_set_enterable_by_player extraction_pelican false)
  (unit_close extraction_pelican)
  (vehicle_hover extraction_pelican false)
  (ai_command_list be_marines/native_squad tf_field)
  ; Watch the landing volume while the camera plays. A blocking camera call
  ; can miss the landing altogether on short native flight paths.
  (wake tf_insertion_camera)
  (sleep_until (volume_test_objects extraction_zone extraction_pelican) 1)
  (ai_command_list be_marines/native_squad tf_wait)
  (vehicle_hover extraction_pelican true)
  (unit_open extraction_pelican)
  (print "transport: insertion landed; disembark normally")
  (unit_set_enterable_by_player extraction_pelican true)
  (sleep_until (not (tf_player_seated)) 1)
  (print "transport: passenger disembarked; native pilot departing")
  ; Calling extraction must not depend on the pilot reaching its holding
  ; waypoint. Keep the flight/boarding gate separate from the native control.
  (device_set_power extraction_activate 1)
  (unit_set_enterable_by_player extraction_pelican false)
  (unit_close extraction_pelican)
  (vehicle_hover extraction_pelican false)
  (ai_command_list be_marines/native_squad tf_hold)
  (sleep_until (>= (device_get_position extraction_activate) 0.5) 1)
  (print "transport: extraction called; native pilot approaching")
  ; Presentation delay for the existing ten-second activation. This script
  ; grants no reward: native per-player activation/boarding/settlement own it.
  (sleep 300)
  (sleep_until (not (volume_test_objects extraction_zone extraction_pelican)) 1)
  (ai_command_list be_marines/native_squad tf_field)
  (sleep_until (volume_test_objects extraction_zone extraction_pelican) 1)
  (ai_command_list be_marines/native_squad tf_wait)
  (vehicle_hover extraction_pelican true)
  (print "transport: extraction landed; board passenger seat")
  (unit_set_enterable_by_player extraction_pelican true)
  (unit_open extraction_pelican)
  (sleep_until (tf_player_seated) 1)
  (print "transport: extraction passenger physically seated")
  (unit_set_enterable_by_player extraction_pelican false)
  (unit_close extraction_pelican)
  (vehicle_hover extraction_pelican false)
  (ai_command_list be_marines/native_squad tf_home)
  (wake tf_departure_camera)
  (sleep_until (volume_test_objects extraction_return extraction_pelican) 1)
  (ai_command_list be_marines/native_squad tf_wait)
  (vehicle_hover extraction_pelican true)
  (unit_open extraction_pelican)
  (sleep_until tf_banked 1)
  (unit_set_enterable_by_player extraction_pelican true)
  (print "transport: arrived and banked; native loadout terminal remains available")
)
