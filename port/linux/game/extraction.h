/* Native timerless Extraction interactions. Gameplay acceptance UNVERIFIED.
   SPDX-License-Identifier: AGPL-3.0-or-later */
#ifndef HALO_EXTRACTION_H
#define HALO_EXTRACTION_H

void extraction_reset(void);
/* A restored world may contain items already committed to a stash. Banking
   stays disabled until a fresh map replaces that world. */
void extraction_restored(void);
void extraction_update(void);
/* Shared operation/Survival notices use the existing chat and native-HUD
 * fallback, independent of whether gear recovery is enabled. */
void extraction_notice(char const *text);
void extraction_player_notice(long player_index, char const *text);
/* A valid opted-in hub/run stays on its map for return and redeployment.
   Requires native hub markers; remote gear needs a host-bound identity.
   Maps naming extraction_pelican/extraction_spirit additionally require a real
   assigned seat at the LZ, continuous boarding to the return hub and an ended
   native cinematic. Transport gameplay remains UNVERIFIED. */
boolean extraction_owns_map(void);
/* Host-owned radio labels for an opted-in operation, not native game teams.
 * NONE preserves ordinary co-op. A missing player gets a machine-private label
 * that also isolates a machine whose local players belong to different groups. */
short extraction_voice_group(long player_index, long machine_index);
/* Optional reserved static object_list helpers, compiled as ordinary HSC.
 * TRUE handles the name, returning only this operation's living participants
 * or NONE when unavailable. Requires exact authored co-op script policy. */
boolean extraction_script_players(char const *name, long *object_list);
/* Ordinary static hud_message helpers notify only this operation's eligible
 * squad participants after native HSC evaluation. Return value stays native. */
boolean extraction_script_notice(char const *name, short message_index);
/* Native sound-returning HSC helper; entire receiving machine must belong to
 * the eligible audience. No microphone use or new voice protocol. */
boolean extraction_script_radio(char const *name, long sound);
/* TRUE handles a real host-owned wipe without rewinding the world. If allocation
   fails, the caller retains its native loss timer and retries. */
boolean extraction_respawn_after_loss(void);
/* Optional authored native character/profile for this faction's hub respawn.
 * False preserves the ordinary campaign/multiplayer spawn definition. */
boolean extraction_player_spawn_profile(long player_index, long *biped, short *profile, short *variant);
struct player_starting_location;
short extraction_player_spawn_bsp(long player_index);
boolean extraction_player_start_allowed(long player_index, struct player_starting_location const *start);
/* Called by the native control-use path. TRUE consumes refused or storage
 * actions. FALSE preserves ordinary controls and the authored state/effect
 * transition after successful deployment or activation, for native HaloScript. */
boolean extraction_control_touched(long control_index, long unit_index);
wchar_t const *extraction_control_prompt(long control_index);
/* Host-only admission for opted-in named transports. Ordinary vehicles, NPC
 * pilots and authoritative client seat replay keep the native path. */
boolean extraction_transport_can_board(long unit_index, long vehicle_index, short seat_index);
/* True only for an occupied evacuation already inside a dedicated return pad. */
boolean extraction_transport_brake_at_pad(long vehicle_index);
/* Scope the native idle-pilot power cut to a named grounded field transfer. */
boolean extraction_transport_idle_at_field(long vehicle_index);
/* Native voluntary player exits from transports with authored landing signals. */
struct unit_datum;
boolean extraction_transport_can_disembark(struct unit_datum const *passenger);
wchar_t const *extraction_transport_passenger_prompt(struct unit_datum const *passenger);
boolean extraction_transport_uses_hover_exit(struct unit_datum const *passenger);
/* Revoke the host's crew-ready signal when native boarding or disembarkation starts. */
void extraction_transport_seat_transition_started(struct unit_datum const *passenger);
/* Clear the host's landed and boarded outputs when native playback takes control. */
void extraction_transport_recording_started(long unit_index);
/* The existing Firefight director delays lobby return only while a living
 * winner's shared gear recovery is pending. No extra wave/lives director. */
boolean extraction_survival_settlement_pending(void);
/* Existing director result gate: every living player recovered and physically
 * in the authored return hub. False for missing bindings, zero survivors,
 * pending writes, failed/ended encounters or a restored world. */
boolean extraction_survival_evacuation_ready(void);

struct player_action;
/* Optional native medical control: downed is alive, never a cleared dead bit.
 * Epoch/tick metadata reuses negotiated Delta object sync; legacy stays stock. */
boolean extraction_medical_incapacitated(long unit_index);
/* Read-only local guidance and host notices share actual medical ownership and
 * native allegiance. No spectator/enemy positions or new network state. */
boolean extraction_medical_ally_incapacitated(long viewer_unit, long target_unit);
boolean extraction_medical_predictable(short absolute_player_index);
boolean extraction_medical_network_enabled(void);
void extraction_medical_network_state(long unit_index, boolean downed, unsigned int epoch, unsigned short tick);
void extraction_medical_control(long player_index, struct player_action *action);
boolean extraction_medical_lethal(long unit_index);
void extraction_medical_damaged(long unit_index);

#endif
