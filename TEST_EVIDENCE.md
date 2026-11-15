# Forerunner test evidence

15 November 2026. Source inspection, compilation, automated native simulation,
rendered gameplay, human play and cross-platform acceptance are distinct here.
Bounded native multiplayer, rendering and read-only Spark discovery are verified;
full gameplay, Firefight, Extraction, VR, editing and MegaWorld acceptance remain open.

The owner's later Development & Verification Directive now authorizes focused
compilation/automated tests and targeted isolated native smoke/synchronization
checks. Earlier source-only entries remain unverified until actual results are
recorded. Full gameplay, stress, VR hardware, MegaWorld and release acceptance
remain separate final-phase gates. Current work prioritizes inventory authority,
shared-loot synchronization and safe Extraction persistence.

15 November, native-body loadout admission (base `016dbc727a`): the existing
connected Extraction/gear C harness passes 23 MSVC x86 `/O2 /W3 /WX` workflows.
Three new flows reject an unusable first weapon, unusable second weapon and a
Survival loadout before any carried object is dropped. They check both saved
identities are refunded, provisional objects do not remain, current/desired
weapon selection stays unchanged, and a compatible retry deploys and recovers
the same pair through existing settlement. Removing the new native admission
call reproduces carried-equipment displacement in all three flows. Existing
transfer, grant failure, disconnect, native controls, pending escrow, refund,
initial ammo, finite Survival and evacuation checks remain passing.

The production call reuses `source/units/units.c::unit_can_use_weapon`, including
native seat/weapon-label admission and `game_engine_allow_pick_up`; no duplicate
biped compatibility table. OS, identity, biped animation and pickup calls are
bounded adapters in the harness. This proves the real transaction's ordering,
not native alternate-biped or authored-hub gameplay. A subsequent unexpected
attachment refusal can still drop the previous equipment as existing world loot;
the change prevents known native incompatibility from causing that displacement.
Inspection also established that ordinary `object_delete` is deferred; no
speculative dangling-pointer/deletion patch was made.

Incremental Windows64 build passes: `build/native-biped-loadout-build.log`,
SHA-256 `76c471d688c4fe739a72cb98bb1878c5a8caaa17babd96ff02a3df6352afdf8c`.
Actual alternate-biped deployment, authored hub and other-platform acceptance
remain open. Unrelated runtime components were not retested, no maps or profiles
were opened for writes, and repository-local compile scratch is removed.

15 November, rendered co-op host-hitch recovery (base `633b5ea92f`): the existing
`tick_fraction_held` harness compiles actual clock/pacing functions with MSVC
x86 `/O2 /W3 /WX`. Ten cases pass: local interpolation, held/released frames,
ahead hold/recovery, latency bounds/invalid values, stale evidence, receipt-time
wrap, local/host/menu isolation and existing behind-client catch-up. Four broken
controls detect missing pacing, stale-host freezing and both original frame
interpolation faults. Native source is used; socket/time adapters are fixtures.

Root cause: the existing native frame cap discards excess elapsed time after a
host stall; joining clients had behind-clock recovery but no ahead-clock pacing.
The opt-in native logger now includes the already validated host tick, without
changing position sampling, comparison pairing or limits. Fresh host evidence
paces client ticks, never rewinds them. Existing two-second freshness and
15-tick latency bounds are reused. No new packet, version, save or setting.

`build/native-render-clock-diagnostic` passes an unforced hidden OpenGL A30
session before the fix; this timing-sensitive pass is not a repair claim.
Two attempted pause monitors (`native-render-clock-hitch-before` and
`native-render-clock-hitch-before-applied`) failed to apply the stimulus because
of the log expression and then an incorrect staged executable path. Both are
monitor ERRORs, not controlled-hitch passes. Inspection of the existing runner
established its actual `A/bin/halo.exe` path before the valid comparison.

Existing `net_compare.py --window`: A30/co-op, seconds 35, skip 5, start 8,
timeout 100, OpenGL both peers, XR false, Extraction false. A bounded watcher
suspends only the exact owned staged host process after logged tick 180, resumes
it in `finally`, and records `hitch.json`; no engine hitch hook or test framework
was added. Same comparator, settings and camera/input script before and after:

- `build/native-render-clock-hitch-control/results.json`: **FAIL**, applied
  2.188-second pause, 36 compared seconds, mean/max separation 1.230/2.823 world
  units. The client stays 39 ticks ahead for the remaining session; original
  position/ammunition mismatches and result remain intact.
- `build/native-render-clock-hitch-fixed/results.json`: **PASS**, applied
  2.203-second pause, 36 compared seconds, mean/max separation 0.044519/0.115970,
  maximum aim difference 1 degree. The client returns to two ticks ahead instead
  of retaining its stall lead. Drift 52/827 samples, mean 0.000063486, maximum
  0.008118 world units. One transient round and grenade mismatch remains within
  unchanged limits. Both recorded resume statuses are zero; owned jobs exited.

Windows64 compiles in the existing non-LTO/bgfx/OpenXR cache:
`build/native-render-clock-fix-build.log`, executable SHA-256
`e53db69f4da8f62f2de8c35f6855b0a1c121b5df0b3298a4387b7a9d345125a9`.
Known Expat import warnings remain unrelated. Native logs sample actual object
positions; no tolerance, camera interpolation or judge changes were made. This
is bounded rendered desktop co-op/hitch proof, not WAN/loss, long-session,
large-squad, other-platform or VR hardware acceptance. Original map hashes match
`build/native-gameplay-inputs.json`; copied maps/A/B folders and compile scratch
are removed. Compact evidence and one pre-change binary remain for recovery.

15 November, OpenH1 dominant-hand source integration (base `4bde81ed08`):
existing `openxr_input` harness passes 14 MSVC x86 `/O2 /W3 /WX` cases, including
the actual controller roles, native facing/convergence, selected-hand model
camera, lateral calibration, two-eye node reflection and draw-cull scope. Default
input, menu roles, physical PTT, pause, invalid axes and neutral rebinding pass.
Wrong-hand input, wrong-hand native firing and leaked mirror winding are each
detected by a broken-integration control. World/math/XR adapters are fixtures;
they are not native hardware, GPU appearance or haptic acceptance.

Native Windows64 builds pass in the existing non-LTO/bgfx/OpenXR cache, including
`build/native-xr-handedness-final-build.log`. Final help-string-only compile is
`build/native-xr-handedness-checkpoint-build.log`; executable SHA-256
`b68dbb8071c439455f8f768a262c230b318680c01cdcd25ab12c3e945bec6ceb`.
Hidden rendered native A30/co-op
uses the existing `net_compare.py --window`, OpenGL both peers, 35 seconds,
skip 5/start 8/timeout 100, XR explicitly disabled, host left/client right hand
preferences. `build/native-xr-handedness-flat-smoke/results.json`: **FAIL**,
37 compared seconds, mean/max position separation 0.671228/1.553354, aim max 4
degrees, drift max 0.005742. Native NVIDIA/OpenGL startup succeeds; player
position/ammunition agreement does not. This run preceded the small Guide-only
XR ownership correction, which is compiled afterward and not exercised with
XR disabled. No rendered multiplayer acceptance is inferred from compilation.

Regression control: owned edits were preserved in a named stash/recovery patch,
then the actual committed `4bde81ed08` source was built in the existing cache.
`build/windows64/halo.xr-baseline.exe` SHA-256:
`4f889fb2dbadb690e23407a2605e1539201f404f129853cdfec6a77ba593e78e`.
Same hidden rendered scenario: `build/native-xr-handedness-prechange-gl` **FAIL**,
37 seconds, mean/max position separation 1.580735/3.389448, aim max 113 degrees,
drift max 0.010665. All limits/results are unchanged; these observations establish
a preexisting rendered agreement failure, not its cause or a VR hardware result.
The logger reads raw native object positions. Preserve both sessions' logs and
the one baseline executable for the next networking investigation; do not repeat
the old protocol/revival or missing authored-map investigations without cause.
Restored VR patch SHA-256 matches its pre-control copy exactly. Original map
hashes are unchanged, copied maps/A/B folders and fixture scratch are removed.
No XR loader/headset/microphone access was requested. Headset/driver/stereo
mirroring, rumble, Linux/other targets and mixed VR/desktop acceptance remain open.

15 November, shared Survival evacuation: existing connected Extraction/gear C
harness passes 20 MSVC x86 `/O2 /W3 /WX` cases. Seven new workflows cover escape,
redeployment without changing item IDs, another player's continuing run, delayed
bank writes, physical return/full native ownership, missing/invalid hub bindings,
finite victory/defeat during activation and zero survivors. Existing finite gear,
restore, death/respawn, transfer, failure, native controls, refund and ammo cases
remain passing. Early-result and missing-hub admission faults are detected.
The fixture compiles the actual director evacuation decision and shared recovery
code; OS/packets/native authored map behavior are not simulated by those checks.

Windows64: `build/native-survival-evacuation-build.log`, existing non-LTO/bgfx/XR
cache; SHA-256 `8f2e4c32d8f22df2a4ca5e35bf461ca2e2909ad16d103f0828a0150f752946c3`.
Existing native `net_compare.py`: B40/co-op, 75 seconds, skip 3, start 8, timeout
100; `HALO_NET_FIREFIGHT_ENCOUNTER=true`, wave limit 0, initial enemies 1, gear
true, normal aim 1 and actor snapshots 1 on both, `HALO_EXTRACTION=false`.
`build/native-survival-endless-guard/results.json`: PASS, 77 compared seconds,
mean/max player separation 0.041/1.282994, max aim 11 degrees, drift max 0.046678.
Single-second position/grenade/ammo differences stay within the unchanged
persistence judge. Host logs show two wave clears, 200 team points and wave 3
spawning; no victory/evacuation or early lobby. Existing AI checker with
`--require-damage`: PASS, 50 snapshots/376 unit comparisons, position max 0.626841,
10 common movers, 4 damaged units, 2 deaths. Existing corpse normalization and
all limits are unchanged; raw corpse differences remain in `actor-agreement.json`.
Original B40/ui/carousel input hashes are preserved in the existing provenance.
Staged inputs, A/B runtime copies and compiler scratch are removed. Stock B40
lacks the required evacuation hub: this proves bounded native endless combat,
not actual authored escape/stash/lobby acceptance, a Flood outbreak or other
platforms. Those acceptance gates remain open with the exact existing markers.

15 November, authored co-op script policy: existing `coop_gate` and
`coop_script_players` fixtures pass 15 + 12 cases with MSVC `/O2 /W3 /WX`.
Six-player fixtures exercise exact ALL/subset gates without gathering, scoped
object/flag visibility, individual teleport/suspend/exit/board/equipment actions,
native save safety and map-latched policy changes. Default campaign gate/action
coverage also passes. Ignoring the option makes `native-lists` and `native-actions`
fail. Existing explicit teleport placement retains its host-only authority;
empty-player wait protection is exercised with adaptations disabled.

Final Windows64 build: `build/native-script-policy-latched-build.log`; SHA-256
`545415731e599d1d6bf5785fe398bade4b59caedbcee42695d573bc8f3c42dad`.
Existing non-LTO bgfx/OpenXR environment/dependencies reused. Actual engine smoke:
existing `tools/net_compare.py`, A30/co-op, 35 seconds, skip 5, start 8, timeout
100, existing actor snapshots; `HALO_EXTRACTION=false` on both peers and
`HALO_NET_COOP_SCRIPT_ADAPTATIONS=false` only on A/host. B retains the default.
`build/native-script-policy-latched-smoke/results.json`: PASS, 37 seconds compared,
position mean/max 0.015/0.068513, aim max 1 degree, drift max 0.026215; five
nonpersistent grenade mismatches stay within the existing unchanged judge.
The prior pre-latch run is retained in `build/native-authored-script-policy-smoke`
(PASS/37 seconds); lifecycle review justified the final run. No simulation,
transport or comparison limits changed. Original three input hashes match;
staging and A/B copies plus fixture scratch are removed, compact logs remain.
This proves bounded native co-op with the option, not authored independent-squad
quests, a complete campaign, cutscene/restore acceptance, Linux or other targets.

15 November, Extraction departure/ammo handoff: the existing connected C harness
passes MSVC `/O2 /W3 /WX`: `initial-ammo`, `refund-disconnect`, `refund-recycled`,
`refund-map-exit`, `refund-retry`, `loop`, `transfer`, `disconnect`, `grant-failure`,
`shared-cache`, `shared-cache-reject`, `shared-cache-failure`, `authority`,
`native-controls`, `survival-win`, `survival-loss`, `survival-retry`, `pending`.
The initial-ammo case rejects reserve above both native limits, then preserves
the admitted ammunition and item ID through redeployment and another escape.
Refund cases inject partial-attachment failure followed by a failed refund write,
restore storage before departure/cleanup, reopen, redeploy and recover exactly
the two reserved items. Deployed disconnect still loses gear. Restoring the old
ammo bound or unconditional departure-loss call fails its corresponding case.
Windows64 incremental build: `build/native-inventory-handoff-build.log`; retained
non-LTO configuration/dependencies, no new launcher or test framework. Scratch
is removed; no maps or peer runtimes were staged. No native authored Extraction
launch was attempted: stock co-op smoke would not exercise these transactions.
Actual authored-map/remote synchronization remains unverified. Continued storage
failure at close and abrupt crashes retain the existing conservative loss policy;
this fix does not promise refunds during an unwritable-storage outage.

Historical local evidence below originally used the external owned directory
`C:\Users\patri\AppData\Local\Temp\forerunner-20261114`.

Artifact availability after cleanup, 15 November: that external staging tree and
`frn-verify-002` were already absent when the cleanup guard checked their paths;
this agent's deletion command stopped before removing anything. The historical
raw logs/binaries named below can no longer be reopened there. Their live
observations remain recorded here and in the chat; source checkpoints remain
committed in the root repository. Do not mistake the recorded results for
currently retained build artifacts.

The existing `tools/net_compare.py` now removes only newly created peer folders
after completion, interruption or partial setup failure, retaining run logs.
Preexisting folders remain owned by their caller. `--keep` retains logs;
`--keep-runtime` explicitly retains full disposable copies. Staging inside the
input runtime or game-data folder is refused before creation. Direct
standard-library checks use tiny fixtures inside this repository and remove
their scratch directory afterward: **7 passed**, including handled interruption,
explicit retention, preexisting-file preservation and both nested input paths.
Python syntax and CLI help also pass; no builds or game launches were needed.
The full pytest suite is not rerun because the prior temporary pytest/toolchain
installation is gone. Earlier native acceptance limits remain unchanged.

## Toolchain and inputs

- Current inventory verification uses the retained, digest-checked compiler with
  installed MSVC **14.52.37025** and SDK **10.0.26100.0**. Earlier entries below
  describe their original toolchains, not the currently installed MSVC revision.
- Python 3.12.14; Ninja 1.13.0; LLVM-MinGW 20261104, clang 23.1.5.
- Compiler archive SHA-256:
  `552191b5754c49a5a20723276a6fc56541ea1922c4c50b7b669022333fc1b40f`.
  Origin and download digest are recorded in `toolchain/llvm-download.json`.
- Installed MSVC 14.51.36231 and Windows SDK 10.0.26100.0, x86 libraries.
  Explicit INCLUDE/LIB paths are necessary with this portable compiler.
  Its COFF linker is the same `ld.lld.exe` copied under `lld-link.exe` in the
  disposable toolchain; no system tool installation was changed.
- Pinned Python packages: `tools/forerunner-build-requirements.txt`.
- Xbox maps copied under the user's authorization; input hashes are in
  `data/inputs.json` and `multiplayer-data/inputs.json`. No game data is tracked.

## Candidate compilation and native smoke checks

Native co-op voice context, base `193196a0ac`, 15 November:

- The native co-op predicate requires **no** multiplayer engine. The former
  Chupathingy voice gate required an engine and therefore always classified
  co-op as lobby voice. `network_game_client_in_game` now owns the native session
  phase; both voice providers reuse it. Current owned-unit, live/type and valid-
  coordinate checks protect host routing and local placement. No packet, codec,
  capture policy, host setting or microphone permission change.
- **17 connected MSVC x86 cases pass**, compiling the actual phase/co-op helpers,
  both voice classifiers, host world/position handoff and actual Delta router with
  its native headers. Cases include near/far co-op, radio, postgame/lobby, missing
  session/world/player data, dead/stale/invalid units, vehicles, enemy proximity
  and disabled channels. Restoring either the engine-pointer gate or unowned-unit
  positioning is detected. World/platform adapters remain fixtures.
- Windows64/bgfx/OpenXR builds incrementally in the retained non-LTO environment
  (`build/native-voice-context-build.log`). Binary SHA-256:
  `409c06edcd91557bbf257ff827d77b540236e271ef1af061ec69bc626eb36bea`.
  Existing private A30 runner, `HALO_VOICE_TEST=tone`,
  `HALO_VOICE_CHAT=push_to_talk`, `HALO_VOICE_CHANNEL=proximity`,
  `HALO_NET_VOICE_LOBBY=false`, dummy SDL audio: **PASS**, 37 matched gameplay
  seconds, mean/max player distance 0.039/0.078; no persistent divergence.
- Both peers log synthetic input in place of the microphone, Opus encoding,
  private relay and decoded proximity playback (route/channel **2/2**). Host/client
  have 27/28 voice reports, zero reported lost/late frames, mono 48 kHz 16-bit
  decoded WAVs with 1,333,440 / 1,336,320 samples, RMS **0.175643 / 0.175644**.
  The inherited tone bypasses physical capture and held-PTT input; no microphone
  opened. This proves synthetic native transport/placement/decoding, not physical
  microphones, controller PTT, audible 3D/stereo/obstruction or headset behavior.
- Compact evidence: `build/native-voice-context-smoke/` logs/results; existing
  A30 input provenance `build/native-gameplay-inputs.json` is reused. Original
  hashes remain unchanged. Peer runtimes/maps and measured synthetic WAVs are
  removed. Linux/other targets, party bridging, cross-region audio and actual
  OpenCE/Delta voice interoperability remain unverified.

Firefight pursuit ownership/allegiance, base `b80ee6b437`, 15 November:

- Actual `firefight_pursue` now filters by native actor hostility, live owned
  player handle and quit state before applying Faber's enemy/sight/path effects.
  Native allegiance/prop handlers still own team changes. **10 MSVC x86 focused
  cases pass**, including ally-nearest, all-allied, native team change, stale
  full-handle ownership, departure, missing/dead body, swarm and inactive actor.
  Removing hostility or ownership protection fails the corresponding regression.
  These adapters do not prove native alliance-change behavior.
- Windows64/bgfx/OpenXR **compiles**, existing non-LTO cache, only dependent
  objects/link (`build/native-firefight-pursuit-build.log`). Binary SHA-256:
  `b2abdb2b892503617c2cda5e2e046b786ed60bd047c563f5b2056bc7ca2260cf`.
- Existing private `net_compare.py` B40/co-op runner, one finite wave/one initial
  enemy, normal aiming/fire input, native actor readback once a second. The first
  35-second run passes synchronization but stops at preparation and is not pursuit
  acceptance. The longer run reaches a genuine **wave clear, 100 points, victory,
  postgame and pregame on both peers**. Tracked handle `e4c900e7` has three live
  samples per peer, horizontal movement **1.301527 host / 1.344849 client** world
  units, observed normal damage and death on both. No scripted damage/kill, new
  comparison tolerance, rendered capture or hardware claim. Gear/medical policies
  remain off; this does not reopen their existing acceptance investigations.
- Existing `forerunner_actor_check.py --require-damage`: **PASS**, 22 snapshots /
  153 unit comparisons, maximum position difference 0.095915, nine common movers,
  three damaged units and one common death. Existing corpse-body normalization
  stays unchanged, with raw differences retained in its JSON. Player comparison
  records 49 matched seconds with no persistent divergence, mean/max position
  0.053/1.272, drift max 0.045668. **The original timed-run verdict is ERROR**:
  the completed encounter supplies 50 seconds against the requested 90; it does
  not meet that run's duration/sample requirement. No threshold was relaxed and
  no follow-up run was manufactured to replace that result.
- Evidence: `build/native-firefight-pursuit-wave/` (raw logs, original result and
  `actor-agreement.json`), preparation-only `build/native-firefight-pursuit-smoke/`,
  input provenance `build/native-firefight-pursuit-inputs.json`. Authorized source
  hashes are unchanged; both attempts' copied maps, saves and peer runtimes are
  removed. Native ally/faction-change scenarios, defeated/lives paths, authored
  Zombie candidate, VR, rendering and non-Windows play remain unaccepted.

Shared native-cache equipment, base `8f315791c1`, 15 November:

- Optional default-off `game.shared_gear` reuses native loaded-weapon admission
  for selected/discarded cross-cache gear. The existing loadout validator shares
  that same implementation; target map/checksum validation of snapshots remains.
  Version-1 codec, file backend, ownership, escrow and wire formats are unchanged.
- **15 gear-source and 12 connected Extraction/Survival workflows pass**, MSVC
  14.52.37025 x86 `/O2 /W3 /WX`. Coverage includes rebuilt-cache opt-in, strict
  default/per-cache choice preservation, target loaded-tag/flag/magazine rejection
  without stash mutation, exact ammo/age and persistent item identity, cross-map
  native grants/refunds, confirmed foreign-cache discard, escape and finite-
  Survival recovery. Bypassing native admission is detected; the existing
  uncertain-choice negative control is updated and still detects its regression.
  These use bounded world/filesystem adapters, not played maps or real save files.
- `ninja windows64 -j6`: **PASS** with existing non-LTO/bgfx/OpenXR configuration
  and cached dependencies. Logs: `build/native-shared-gear-build.log` and
  `build/native-shared-gear-final-build.log`. Only dependent native units/link
  rebuild; no new dependency preparation or renderer changes. Source scratch is
  removed. Actual shared-cache gameplay, Linux/other-target execution and hardware
  acceptance remain open. The preceding A30 native result belongs to its earlier
  executable; it is not new-feature acceptance. Existing missing authored-hub
  acceptance and protocol/revival evidence are not investigated again here.

Native Extraction control handoff, base `6c702d327d`, 15 November:

- `extraction_control_touched` now preserves native control state/effects only
  after successful deployment or accepted activation, and rechecks native power,
  usability and one-use locks on the host. The existing connected Extraction
  harness includes the actual `device_can_change_position`, `control_touched`
  and `control_toggle`. Six MSVC x86 `/O2 /W3 /WX` workflows pass: native controls,
  loop, grant-failure, authority, binding and pending settlement. Restoring the
  original swallowed deployment signal fails the native-controls case. OS/world
  adapters remain fixtures; this is not authored-map or native persistence proof.
- Restored one root-local toolchain/cache. The existing downloader verifies the
  same pinned bx/bimg/bgfx archive hashes, retains licenses and applies all nine
  existing patches without initializing nested repositories. Installed MSVC
  14.52.37025 and SDK 10.0.26100.0, LLVM-MinGW 20261104 (digest above), Ninja 1.13.0.
  `configure.py --portable --without-ffmpeg --with-openxr --lto=off`, then
  `ninja windows64 -j6`: **PASS**. bgfx/OpenXR are included; rendering/stereo are
  not exercised. Expat emits LNK4217 local-import warnings; native menu/lobby
  startup below works. Full LTO was stopped after excessive optimization time;
  a link-only LTO=0/codegen=0 attempt failed on an existing credits-string symbol.
  No credits/source workaround was applied. Optimized full-LTO linking remains
  unresolved; use the established non-LTO verification mode until revisiting it
  is necessary. Logs: `build/native-gameplay-nonlto-{config,build}.log`.
- Existing `tools/net_compare.py`, same freshly compiled executable on both
  peers, `--maps a30 --variant coop --seconds 35 --skip 5 --start 8 --timeout 100`,
  with `HALO_EXTRACTION=true` and `HALO_NETWORK_TEST_ACTORS=5` on both:
  **PASS**, 37 seconds compared, unchanged tolerances, position mean 0.042 / max
  0.118347 world units, aim max 1 degree; drift max 0.013139 across 802 samples.
  Three grenade and one ammunition mismatched seconds stay within the existing
  tolerances; 14 client corrections. Native logs show co-op snapshot traffic,
  both players moving, weapon/grenade changes and no observed assertion.
  Actor telemetry stays zero in this bounded opening section: do not claim AI
  encounter progression, authored Extraction acceptance, rendering or human play.
- First staging attempt omitted the stock lobby prerequisite `carousel.map` and
  timed out in the menu with an explicit missing-map log. Correcting the inputs
  to only `ui.map`, `a30.map`, `carousel.map` passes without another engine build.
  Authorized originals were read-only, copied/verified by SHA-256, and rechecked
  unchanged afterward. Both peers use isolated saves and local gameplay, with
  public lobby/sharing/downloads disabled; ordinary startup still refreshes
  public metadata. Runtime/map copies are removed after both attempts.
  Compact logs/results: `build/native-operations-smoke-complete-inputs/`; the
  failed run's logs remain in `build/native-operations-smoke/`. Input provenance:
  `build/native-gameplay-inputs.json`. Compiled executable SHA-256:
  `c9403a652b789e1e392df27664542ea9cf32265a319918390f0ea4b644cc7852`.

Connected AA regression repairs, base `861a6497ce`:

- The previous FXAA fixture injected a mode and missed the native reader's old
  FXAA-to-MSAA remap. The fixture now compiles the actual native choice table,
  reader, selector and bgfx pass together. Saved FXAA fails before the production
  repair and passes after it. **10 MSVC x86 workflows pass**, including switching
  FXAA/MSAA/off, unchanged SMAA/SSAA fallback, preserved stored preferences,
  OpenGL SMAA/SSAA and returning to cached FXAA shaders.
- The OpenXR pipeline fixture now also compiles `bg_target_find`. It reproduces
  material reads bypassing the owned MSAA eye target before the repair. After
  routing reads to the active draw target, **9 MSVC x64 workflows pass**, retaining
  existing eye, sample, resource failure and queue/release checks. The two defects
  are preserved as negative controls. Installed MSVC 14.52.37025 / SDK 10.0.26100.0,
  `/O2 /W3 /WX`; all disposable repository-local output removed.
- These strengthen compiled source evidence; GPU/runtime calls remain fixtures.
  No actual driver compilation, rendered comparison, headset, timing, full-engine
  build, gameplay acceptance, user assets/save mutation, external staging or push.
  The earlier isolated pass tests did not prove these settings/material paths.

Inherited model Light effects on bgfx, base `1a1dfcac9b`:

- Installed MSVC 14.52.37025 x64 / SDK 10.0.26100.0, `/O2 /W3 /WX`
  compiles real shader selection, optional startup/retirement, native texture
  binding, shader preparation/collection and shared GLSL emit/rewrite functions.
  **15 workflows pass**: vertex/per-pixel variants; pending vertex/pixel jobs,
  failed links and full native fallback; capability limits and partial startup
  cleanup; missing/failed/invalid-volume fields; selected-slot/view changes;
  Vulkan/Metal/D3D11 source rewrites; recorded/pending shader preparation.
  The first compile caught implicit boolean-to-float initialization warnings;
  explicit float values fixed production source before these passing results.
- **10 existing model-lighting source regressions** recompile/pass on MSVC x86,
  including Classic, async/link fallback and shared FXAA rewrites. Three broken
  controls are detected: a conflicting texture binding, admitting a non-volume
  field and retaining an effect mask after shader fallback. Native draw ordering
  and preparation connections are inspected; both existing Video menu styles
  regenerate/parse and changed Python syntax passes.
- GPU/compiler-job/OpenXR APIs are fixtures. Backend rewrites are not GPU shader
  compilation. No full engine build, rendered model/field, native map transition,
  hardware stereo, screenshots or measured GPU/CPU costs are claimed. Existing
  native volume creation/cache/dirty-page ownership is reused; no second field
  generator, texture cache, simulation or packet change. World/ray-query AO and
  OpenH1 enhanced-lighting parity remain OPEN. Disposable check outputs are
  removed; no original map/save, Spark/upstream file, push or release.

Individual inherited Light effects, base `485fa43a12`:

- MSVC 14.52.37025 x64 / SDK 10.0.26100.0, `/O2 /W3 /WX` compiles the
  actual `lighting_config.c` through the existing harness. **5 workflows pass**:
  unchanged Off/Low/High/Auto and software/weak-device policy, each custom part
  independently, saved-choice reload across mode switches, no per-frame setting
  rereads, and unknown-mode/unrelated-setting fallback. Three additional
  compilations with Android, GL ES and server guards keep Custom disabled.
- MSVC x86 compiles/passes **6 native-menu workflows**, including custom Light
  effects preview, Cancel, existing writer save and reopen. Both generated Video
  styles parse; all twelve desktop-only rows match registered boolean settings
  and existing effect consumers. Changed Python syntax passes. No new writer,
  storage format, gameplay packet or visual implementation is introduced.
- Pinned Faber/Forge/GulchCE settings files were inspected read-only through
  GitHub, without creating a checkout. Exact pins/blob identities and remaining
  options are in `UPSTREAM_FEATURE_MATRIX.md`. These are declaration/consumer
  comparisons and compiled fixtures, not proof of rendered visual parity.
  Real menu navigation/effects, stereo and dense-encounter cost remain OPEN.
  No live GPU, game, microphone/headset, original saves/assets or other repository
  was touched. All repository-local compile scratch was removed.

OpenXR MSAA through the existing renderer, base `6465f3b2a4`:

- Installed MSVC 14.52.37025 x64 / SDK 10.0.26100.0, `/O2 /W3 /WX`
  compile the actual eye acquisition, target/sample selection, framebuffer cache,
  resolve, frame submission and alpha-test shader-preparation functions using
  the existing source-extracting harness. **9 workflows pass**: stereo handoff,
  BGRA/RGBA/depth/intermediate capabilities including disjoint masks, reuse/
  resize retirement, off/menu, allocation and framebuffer failure, failed resolve,
  invalid/reused image contracts and logical viewport changes. Allocation failure
  remains cached rather than retrying every frame. Runtime images are not freed.
- Three broken controls are detected: mismatched depth/intermediate samples,
  skipped resolve and allocation per frame. The **9 existing FXAA source cases**
  recompile/pass on MSVC x86; **5 existing format checks** pass. Existing Video XML
  is regenerated/parsed; changed Python syntax passes. Test scratch is removed.
- GPU/OpenXR calls are fixtures, not a live runtime or driver. No headset, game
  launch, full-engine build, GPU shader compile, actual screenshots, performance
  measurements or target/hardware acceptance is claimed. World/ray-query AO,
  bgfx HDR and enhanced Light effects remain unfinished. No assets, saved data,
  packets, upstream/Spark files, branches or remotes are changed; no push.

Shared pre-HUD FXAA, base `f84773a792`:

- Installed MSVC 14.52.37025 x86 / SDK 10.0.26100.0, `/O2 /W3 /WX` compile
  the actual bgfx pass, view/copy helpers, target rectangle and release code
  through the existing source-extracting harness. **9 cases pass**: clipping,
  split/eye view ordering, same-size reuse/resize retirement, off, pending
  shaders, failed link, texture/framebuffer allocation failures and view/uniform/
  target limits. The source copy and filter share the window scissor; GPU API
  calls are a fixture, not a driver or headset. An initial fixture compile lacked
  DWORD/C boolean declarations; correcting that fixture resolved it.
- Three source mutations are detected: writing destination alpha, dropping
  resize retirement and breaking outer texel-centre clamping. Shader pointers
  are cached after their first request; repeated passes/resizes do not rewrite
  or hash their source again. Allocation failures do not loop every frame.
- The existing shader-rewrite fixture compiles/passes for Vulkan, Metal and
  D3D11 with the actual shared FXAA source, metrics/bounds and sampler binding.
  OpenGL's program builder calls the same source getter. Native hook ordering
  remains before HUD and after tone mapping, inside the primary view.
- These are direct compiled fixtures, not a pytest suite, GPU shader compile,
  full engine build or rendered acceptance. Current root checkout still lacks
  a usable full native toolchain/dependency cache and runtime data. Visual
  parity, real stereo/linear encoding, map/resize transitions and GPU costs
  remain OPEN. No external staging, nested repository, game launch, asset/save
  mutation or push. All `.pytest_cache/gear-check-*` scratch was removed.

Shared graphics lighting and native presets, base `32e88a25f7`:

- Installed MSVC 14.52.37025 x86, SDK 10.0.26100.0, `/O2 /W3 /WX` compile
  real-source functions extracted by the existing fixtures. **10 graphics
  cases pass**: Classic, point/distant lights, unrecognized programs, compiler
  readiness in either stage, failed link, immediate/streamed cache separation
  and GLSL rewrites for Vulkan/Metal/D3D11. The real native light-register
  uploader supplies all twelve registers and skips extra uploads for Classic.
- **5 native-menu workflows pass**: prior account-action cases plus graphics
  preview/Cancel, manual Custom and OK/reopen. The existing source combiner's
  MCC/legacy test passes. Disabling lighting, removing shader fallback, writing
  during preview and suppressing Custom reporting each fail their controls.
- Existing Video generators produced CE XML and the Xbox-style page's shared
  preset row. XML parsing, generator equality and Python syntax pass. A fixture
  lookup initially assumed an unsized array, then omitted a required constant;
  both fixture issues were corrected without changing engine behavior. The
  Xbox-style page references CE help/widgets rather than repeating their text;
  verification follows those references. No substitute test framework was added.
- This is direct fixture compilation/execution, not a pytest run, full engine
  build, GPU shader compilation or rendered acceptance. Metal/D3D11 here describe
  rewritten shader source, not execution on those APIs. No owned usable engine
  build, toolchain/dependency cache or map data remains inside this checkout for
  the required native smoke/screenshots. Do not revive the removed external
  staging or create a nested project/repository copy to work around that gap.
- World/ray-query AO, compatible-GPU execution, unsupported-GPU fallback,
  map transitions, stereo effects, identical-camera Classic/Enhanced captures,
  dense encounter GPU costs and target-specific acceptance remain OPEN. The old
  shadow comparison's sample shortfall is unchanged. Scratch was local and removed;
  no maps, saves or other repository files were modified, and nothing was pushed.

Confirmed stash discard, base `e08f85816`:

- Existing actual-source gear fixture: **14 cases pass**, including a full
  32-item stash, exact-item removal, client/cache/generation refusal, reopen,
  active-reservation protection and failed/unconfirmed write retries.
- Existing connected Extraction fixture: **25 workflows pass**, including
  preview/confirmation delay, Cancel, published-save retry while standing,
  expiry, hub exit, new native body, vehicle entry, changed/revoked owner and
  incapacitation. Prior Extraction, finite Survival and medical cases still pass.
- Installed MSVC 14.52.37025 x86, SDK 10.0.26100.0, `/O2 /W3 /WX`. These are
  direct compiled fixture runs, not a pytest run, full engine build or native
  gameplay. The generation negative control was detected during development.
  OS locking/replacement is unchanged and not re-proven by the fake filesystem.
- Generated includes, objects and executables used repository-local disposable
  `.pytest_cache/gear-check-*` scratch, removed after each run. No game assets,
  personal inventories, other repositories or headset devices were accessed.

Durable native loadout choice, base `2ecf1877d`:

- Installed MSVC **14.52.37025**, Windows SDK **10.0.26100.0**, x86 C11 `/O2 /W3
  /WX` compile the existing actual-source gear and connected Extraction fixtures.
  Generated includes came from the existing harness generators; no engine copy,
  replacement harness or new toolchain was created. Pytest itself remains absent;
  these are direct compiled fixture executions, not a claimed pytest run.
- **11 gear cases pass**: existing authority, escape/reopen, loss, uncertain
  write, corruption and cache protections; durable choices across reopening,
  failed/unconfirmed writes, failed-grant refunds, recovery/ammo and independent
  cache preferences. Disabling the pending-choice guard fails `choice-failure`
  as expected. The disk codec/version/size, item identities and writer are kept.
- **21 connected source workflows pass**, including the new terminal choice,
  prior Extraction/shared-loot/death/restore/pending cases, finite Survival and
  medical cases. The saved-choice case exercises an uncertain terminal write,
  retry without a second cycle, record reopen, native equipment grant, recovery,
  redeployment and finite-Survival reuse of the same chosen primary with three
  recovered rounds. Its final cross-mode extension was recompiled and passed.
- The first connected MSVC compile caught an inherited long-to-short local-player
  iterator conversion; its local variable now matches the native short API.
  Warnings remain errors. No comparison tolerance or acceptance gate changed.
- Generated headers, objects and test executables stayed in one tiny disposable
  `.pytest_cache/gear-check-*` area inside Forerunner and were removed afterward.
  No original saves, game data, Spark files, headset devices or external staging
  were accessed. Existing native OS-lock/atomic-replacement code is unchanged and
  not re-proven by its fake filesystem. A full engine build, actual terminal
  gameplay, native multiplayer persistence and Linux/VR execution remain open.

Faber shadow-budget graphics reuse, base `d13303555`:

- `verification-87521711b-001/graphics-shadow-menu-final-configured.log`:
  **10 passed**. Existing harness compiles the real native shadow helpers/map
  lifecycle: stock behavior, ranked fading, shared same-frame stereo decisions,
  independent windows, bounded history, cinematic sentinel and map reset. Removing
  the frame-sharing guard fails the negative control. Generated menu content,
  spinner values and runtime setting are connected; existing text-fit checks pass.
  Initial fixture macro/compiler-environment failures are retained in the earlier
  `graphics-shadow-menu*.log` files. They were corrected in the fixture/environment.
- Disposable Windows64/OpenXR/bgfx compilation passed, snapshot
  `a7335cc6769343d5230afb89b8d130685ce9058adf821a28ea593aeb6885092a`;
  executable SHA-256
  `f4508e336f1ebcea93fa3c1a8660040056a0a519c3ea56ae1cda419c3c532046`.
  Log: `frn-verify-002/compile-openxr-graphics-shadow.log`.
- `native-graphics-shadow/`: hidden rendered B40 co-op with native shadow budget
  **8 versus 0**, VR/microphones off, isolated copied data. Both peers started
  bgfx Vulkan on the NVIDIA GeForce RTX 4070 Ti. Captured `frames/frame05200.bmp`
  was visually inspected: native first-person weapon/HUD, structure, effects and
  world rendering are present. No user desktop input or headset was used.
- Native synchronization result is **ERROR / insufficient evidence**, not PASS:
  **47 seconds compared**, offsets 0/1, position mean/max **0.044/0.204** world
  units, aim maximum **1 degree**, three grenade-boundary mismatches, prediction
  drift **77/288 ticks**, mean/max **0.000134/0.006119**, **187** corrections.
  Only **288** eligible on-foot samples were available, below the existing **300**
  minimum. No tolerance or coverage requirement was relaxed. Longer isolated
  coverage remains open; this run establishes rendered startup and bounded state
  observations, not accepted multiplayer graphics parity or measured GPU savings.
- All owned jobs above completed before the owner's renewed directory restriction.
  Further file operations remain inside `D:\Project Forerunner`; external staging
  and upstream checkouts are left untouched. Hardware VR, Linux execution, large
  crowds and OpenH1 AO/lighting integration remain acceptance requirements.

Native living incapacitation and held-Action biofoam, base `8d2cbb07f`:

- `verification-87521711b-001/medical-held-action-workflows.log`: **45 passed**
  in the existing connected Extraction/gear/resupply/co-op harnesses. Real source
  covers host authority, unsupported peers (including missing object sync),
  actual-death refusal, held treatment, final-tick combat/range/wall/faction
  interruption, bleedout, restore quarantine and gear loss only at actual death.
  `medical-final-controls.log`: **23 passed**, adding the native distinction
  between a one-tick Action press and its continuous long-hold gesture. These
  fake-world checks do not reproduce rendering, real packets or verified identity.
- `medical-checkpoint-configured.log`: **9 passed**, including the actual public
  epoch getter with both native-header and learned-layout epochs. Initial
  `medical-epoch-actions*.log` failures were missing Windows SDK headers/linker,
  followed by a run with the installed SDK and native linker; no production
  workaround or tolerance change was used.
- `medical-delta.log`: **2 passed**, three unrelated cases deselected. Existing
  C89 codec/session checks cover negotiated medical bytes, legacy zero reserved
  bytes, malformed schema/epoch/vitality and 5,000 random inputs. The three
  focused source/layout join checks also pass; packet sizes remain unchanged.
- Disposable Windows64/OpenXR/bgfx compilation succeeded at source snapshot
  `e68eb15a12578cb936e588283120c4870896aa5c5abe43fbf04b635d23eccd81`;
  executable SHA-256
  `cebdb17bac1364ce79fb384d112e1aaf7fe36bb5d1733f45f074e682476b7991`.
  Log: `frn-verify-002/compile-openxr-medical-held-action.log`.
  The later config description, regression assertion and checkpoint prose do
  not change gameplay behavior in this measured binary.
- `native-medical-held-action/`: **PASS**, two private copies of that binary,
  Xbox B40 co-op, synthetic held Action on the healer and held W+Action on the
  casualty. Host medical on, client local medical setting off. Native AI is
  paused; the existing single-shot simulator injures the critical player.
  Actual engine logs show living incapacitation, five-second continuous biofoam,
  the same body returning to 0.25 health on both peers, matching down-state entry
  and exit metadata at native epoch 1, and suppressed downed movement. **66
  seconds compared**, offset 0, sampled position mean/max **0.026/1.350** world
  units, one position and one alive boundary mismatch; prediction drift
  **63/1066 ticks**, mean/max **0.000120/0.006441**, **193** client corrections.
  Existing comparison tolerances were unchanged. Native scripted death later
  kills the other player; it does not prove corpse revival refusal for the
  treated casualty, which remains a focused-source check.
- Failed probes are retained: `native-medical-interaction` scheduled cues past
  its end; `native-medical-interaction-fixed` supplied default Slayer on a solo
  map and invalid cue separators; `native-medical-treatment` used parenthesized
  script expressions instead of console lines; `native-medical-treatment-console`
  exposed `damage_object` accepting a missing scenario tag (NONE), now guarded in
  both native script damage helpers. `native-medical-native-shot` reset health
  too early and the shield recharged. The subsequent timed probe and diagnosis
  found real client divergence: the public epoch getter returned the unbound
  learned-layout epoch instead of the active native snapshot epoch. The epoch
  repair restores synchronization. Treatment still stopped after one tick
  because native queues latch Action. Reusing the existing unlatched long-hold
  gesture fixes that cause; temporary diagnostics have been removed.
- `native-medical-legacy-fixed/` passed **2/2**, retained x86 version 44 and x64
  version 45 in swapped hosting directions, medical off. The earlier
  `native-medical-legacy/` failed admission before the explicit bounded 44/45
  bridge; that failure remains recorded. Final compatibility after the epoch
  getter repair, `native-medical-final-legacy/`, also passes **2/2** swapped-host
  games: **34 seconds** each, offset 0, sampled position mean/max
  **0.042/1.287** and **0.045/1.349**, one position boundary mismatch each;
  prediction maximum **0.032693/0.007611**, **141/140** client corrections.

This proves a bounded native medical interaction, not human acceptance, Black
Echo, independent squads, actual PvPvE death looting, recovered rewards, hardware
VR, Linux execution or connected-region gameplay. Original maps, Spark, personal
profiles and save records were not modified.

Finite Survival/shared gear, base `0f8bf79f3` plus the connected mode changes:

- `verification-87521711b-001/survival-gear-workflow.log`: **37 passed** using the
  existing asset-free harness. The actual Extraction/session/gear/equipment code
  and native director outcome getters cover finite success, failure/abort, live
  owner admission, death/paid native respawn, pending escrow and lobby wait,
  checkpoint quarantine, unchanged recovered ammo and same-cache Survival-to-
  Extraction reuse. Forcing failed phase-4 outcomes to success fails the negative
  control. Existing resupply, gear and co-op wipe checks also pass. Native actor
  spawning, physical player behavior and real verified identity are not simulated.
- The first paid-respawn fixture reused the dead body's stale parented weapons;
  capture correctly refused them. The fixture now models a new native biped's
  fresh inventory. No production ownership guard was weakened to make it pass.
- Existing Windows64/OpenXR/bgfx export builds incrementally:
  `frn-verify-002/compile-openxr-survival-gear.log`. Snapshot
  `8b28d8749a9be951d0499a3e771f8540cc3a580216d7c52a9bce6d4711e62c3f`,
  binary `dad2038465913a13c08a2b23fc8808dd7d3053d38080684f2a4895e1fee14931`.
  Later changes only add checks/docs and clarify the source header comment.
- One private x64 host/x86 peer B40 co-op smoke with finite Firefight/shared gear
  requested passed: 34 matched seconds, tick offset 0; mean/max sampled separation
  0.040/1.350 units, one isolated position mismatch; predicted error 63/360 ticks,
  mean/max 0.000431/0.006691 units. Tolerances unchanged. Evidence is in
  `verification-87521711b-001/native-survival-gear`; the host reached native
  director preparation/shared-life notice. No `.gear` records or successful
  finite-victory banking were observed with these synthetic test players.
  This is bounded native compatibility/preparation, not successful Survival,
  persistent-owner admission, native reward recovery or gameplay acceptance.
- Shared lives, native respawns and authored HaloScript/director ownership remain
  with existing systems. Default-off finite gear policy shares the same sessions,
  lock/codec/escrow and native object operations; zero-limit endless runs do not
  reserve gear without a recovery path. No new packets, editor, storage writer,
  framework, global pool limit or original asset/profile/save change.
- The later full Persistent Spartan Operations directive is preserved under
  `docs/forerunner/`. Spark `e472e79` was inspected read-only: native logic/encounter
  authoring exists; no reviewed Black Echo/per-squad quest export was found in
  the inspected source. Its required map/mission/event/region/audio handoff is
  recorded in the existing contract. No quest, six-member squad, revive or
  multi-region gameplay support is claimed from these gear results.

CI publication safeguards after the Extraction milestone:

- Existing `tools/test_build_workflow.py` reused: **5 passed**, 32 deselected,
  `verification-87521711b-001/ci-publication-guards.log`. Draft conditions are
  evaluated with approved/default inputs, push/PR/schedule events, own/upstream
  repositories, master/main/unreviewed/tag refs and failed/skipped dependencies.
  Automatic Forerunner updater identity stays off even with inherited release/
  nightly input values. No destructive release mutation remains in those jobs.
- Three previously ungated native-workflow jobs now wait for the existing source
  gate; its master/main/release-* admission was extended without admitting other
  `ma*` branches. The missing selected source ref in its web package job was fixed.
  Additional existing shell-gate fixture cases cover master/maintenance, but cannot
  run here without a POSIX shell and remain unverified. No shell/install framework
  was added. Windows collection now skips the existing sh/jq probe when sh is absent.
- Broader static checks exposed inherited self-saving/unguarded Android cache
  policy differences. Both assertions also fail against unmodified `87f988317`,
  confirmed read-only in `ci-inherited-baseline.log`. They were not hidden or
  rewritten; this is not a claim that the whole inherited workflow suite passes.
- Existing native/browser artifacts and supported platform package paths remain;
  only explicit manual requests in `LeftoverProductionsTeam/Project-Forerunner`
  may prepare new draft tags/assets. Old releases/tags are neither deleted nor
  overwritten; public publication, signing and update activation remain separate.
  Map-index publishing is disabled; the inherited container publishing job already
  excludes this repository. Spark CI was inspected read-only. No GitHub workflow,
  draft, release, push, tag, registry upload or updater activation was executed.
  Actual CI execution and Windows/Linux/macOS package acceptance remain open.

Connected Extraction/shared gear, base `143c02c40` plus the owner-approved record
implementation and connected gameplay source:

- One per-owner version-1 record uses native stable-file exclusion and flushed
  atomic replacement. Existing pytest/asset-free harnesses are reused. No database,
  extra launcher, inventory wire messages, new framework or Spark dependency.
- `extraction-workflow-checks.log`: **29 passed**. Nine connected Extraction checks
  execute the actual mode, gear codec/transactions and native equipment helpers
  against bounded world/identity/file adapters. They cover separate local/remote
  stashes, preserved ammo/battery/grenades, transferred recovered-item identity,
  stale-owner refusal, death/disconnect/recycled-handle loss, failed native grant
  cleanup/refund, timerless ready state, optional markers, restore quarantine and
  published-but-uncertain escrow retry. Removing native parent ownership makes
  the transfer negative control fail. Seven gear checks include failed-run and
  codec/corruption recovery; nine existing co-op wipe checks remain passing.
- Four of those 29 checks compile/run the real Windows native file backend:
  bounded binary reads/replacement, competing process exclusion, lock release on
  killed owned process, abandoned staging recovery and hard-link refusal before
  truncation. They do not establish sudden power-loss durability or game-network
  identity correctness. POSIX source is implemented but not compiled/run here.
- Incremental existing Windows64 OpenXR/bgfx build succeeds with all connected
  runtime code. Final export snapshot
  `c01ee9a8054f663c0f9a7833ef587645a2c4c45e8fc23ba76d34cfcf9d5494a6`,
  binary `62695b1188cf70900c6865e2a064231e8ae740af69fe6e49ec827bf583ef1f64`;
  log `frn-verify-002/compile-openxr-extraction-workflow.log` (sibling of the
  evidence root). No unrelated engine rebuilds. The earlier functional build
  used for native smoke has snapshot
  `1fe6d85185dfb0cd8be99603e9088a36d9d57ac7c585184de8a3c124b71ee102`,
  binary `ef09e200c08a35b456145db18d1791586577e0e65a71a02eda2ba5b4b23d2505`.
  Subsequent runtime changes only update setting help/header comments; fixture
  additions and milestone documentation do not change gameplay behavior.
- `verification-87521711b-001/native-extraction-compatibility`: private loopback
  B40 co-op, new x64 host versus retained x86 peer, disposable maps/config/saves,
  microphones/public networking/downloads/updates off. **PASS**, 34 matched seconds,
  tick offset 0; mean/max sampled position 0.040/1.350 units; one isolated position
  mismatch at tick 960, identical weapons/ammo/health there. Predicted local-player
  error 63/360 ticks, mean 0.000431/max 0.006691 units. Existing scripted hits run
  every four seconds after the host sample, so that boundary is a plausible cause
  of the isolated mismatch, not a proven simulation defect. No tolerance changed.
  The prior `native-extraction-gate` run was **insufficient**, despite zero sampled
  differences: campaign transport/death supplied only 231 required on-foot ticks.
  Both outcomes/logs remain retained. No `.gear` records were created on this
  non-Extraction stock map with Extraction requested on the host.
- This is compiled/automated implementation plus bounded native compatibility,
  NOT a native multiplayer Extraction, Survival, headset or gameplay acceptance
  claim. Real authored hub/terminal use, late-join/death placement, native gear
  transfer/escape/reopen/redeployment and host-bound identity still require an
  isolated compiled-map session. Spark read-only review at `e472e79` confirms all
  hub starts are checked, but its current runtime pin/cache deployment needs the
  handoff described in `SPARK_INTEGRATION_CONTRACT.md`. No original assets,
  personal profile/identity/save data, Spark files or remote state were changed.

Windows64 OpenXR/Vulkan integration, base `3d5f28dcc` plus tracked repairs:

- Existing configure/Ninja path built with `--with-openxr`, bgfx enabled, optional
  FFmpeg omitted. Pinned bx/bimg/bgfx and digest-checked OpenXR 1.1.63 headers were
  obtained only in the owned export; unchanged third-party licenses remain there.
  Initial x64 compilation exposed a native script address passed as a host pointer
  and missing declarations in Extraction, MCC skulls and native actor readback.
  Repairs reuse `xbox_pointer` and existing declarations; no ABI or protocol changed.
  All enabled VR input, rendering, menus, Vulkan interop and patched bgfx units link.
- Final compiled snapshot:
  `786d6058e89500c80a8749f1049f839c4f6fa04b87585c20c3635e29aadfe061`;
  binary SHA-256:
  `d87150b7a5db734f72d522f875b4710b7bf4ce0a72453f580c7948608e2a3a45`.
  Existing export: `C:\Users\patri\AppData\Local\Temp\frn-verify-002\source`.
  Target manifest beside it: `forerunner-openxr-build.json`; configure/compile logs
  `*-openxr*.log` preserve failures and incremental repairs. Earlier x86 manifest
  and failed-pass snapshots remain intact; no duplicate build system was added.
- Focused real-source checks: **24 campaign/script tests**, **8 shared XR input
  tests**, **12 comparator tests** pass. Encoded script arguments replace the old
  fixture's host-pointer shortcut. Controller checks cover native face/grip/stick/
  trigger mappings, focus/disconnect, pause priority, PTT release/rearm, tracking,
  menus/reference changes, privacy and axis conversion; they simulate located
  actions, not a headset/runtime. Logs under `verification-87521711b-001`:
  `openxr-script-addresses.log`, `openxr-input.log`, `openxr-comparison-coverage.log`.
- Initial Vulkan/x86 combat run matched only one second because native log cadence
  retained a slow first-render tick offset. Its original PASS is **insufficient**:
  rejudging now returns ERROR for inadequate matched coverage. The first invocation
  also used an unknown `HALO_VR_ENABLED` variable, so it covered VR-off rendering.
  Logs/rejudged report: `native-openxr-desktop-smoke*`. Native sampling now follows
  absolute simulation seconds and judging requires at least 75% of requested
  seconds to match; existing distance/time/drift tolerances are unchanged.
- Corrected `HALO_VR=true` run used the same owned map copies, hidden real Vulkan
  rendering, existing x86 peer and scripted combat/death inputs. `XR_RUNTIME_JSON`
  named an absent isolated file to prevent headset activation; the actual branch
  reported **installed loader unavailable**, then continued flat-screen Vulkan.
  **PASS: 32 matched seconds**, offsets 0/1 tick, sampled position mean/max
  **0.432/22.766**, aim max 1 degree, five shield and one position mismatches,
  one client correction. Predicted drift: **141/848 samples apart**, mean
  **0.000188**, max **0.021676** world units. The 22.766-unit sample is host tick
  330/client 331: the host logs before `network_test_gather`, the client after its
  scripted relocation; positions agree next second. Shield samples similarly
  straddle the post-log scripted hits at 120/360/480/720/960. This supports bounded
  compatibility, not exact lockstep or a new defect claim. Report/logs:
  `native-openxr-fallback-smoke.json`, `native-openxr-fallback-smoke/runs/`.
  Thirteen native framebuffer captures; `openxr-fallback-frames/frame02700.bmp`
  visually confirms world, native weapon and HUD. Both owned peers exited.
- Existing Spark reference client accepts the x64 binary's contract-1 discovery
  (`openxr-spark-discovery.log`); advertised/native protocol remain 44. No editing
  operation, new launcher, microphone capture, original asset/save write, publish
  or hardware session occurred. Stereo, physical interactions, physical devices,
  full matches and Linux/macOS acceptance remain open. Deferred inventory settlement
  is not supplied by this build milestone.

Firefight shared-equipment follow-up on `c6f9255c2`:

- The new focused check first reproduces `firefight_resupply_players` modifying
  reserve ammunition through a stale slot whose weapon belongs to a different
  unit. The repair compares the native parent to the player's full unit handle.
  **6 passed in 0.57 seconds**: owned ballistic reserve, transferred/detached gear,
  owned/foreign batteries, ineligible players and an ownership negative control.
  Existing harness compiles the actual production function; this is not physics,
  network transfer or persistence simulation. Logs under
  `verification-87521711b-001`: `resupply-ownership-{repro,fix}.log`.
- The same disposable Windows x86 build resumed with six build steps (changed
  gameplay object, existing build identity dependents and link). Source snapshot:
  `ff3c55854e939cf26257d3292dda56579eb7b557ff6ad268155d7bf7add13b1a`;
  binary SHA-256:
  `8d909af34044a1f95e98c8e86710e6b3c6d60d5c0c3f8d7bfe84f9431c4f5196`.
  Manifest: `C:\Users\patri\AppData\Local\Temp\frn-verify-002\forerunner-build.json`;
  link log beside it: `compile-resupply-ownership.log`. Prior manifest preserved
  as `forerunner-build.before-resupply-ownership.json`.
- Existing `net_compare.py`, same binary both peers, B40 `coop`, stationary native
  aiming/fire, host encounter/resupply enabled, no scripted hits/deaths:
  **PASS**, 62 seconds compared, tick offset 0. Wave one records two final enemy
  deaths and score 200; both peers receive its clear, ammunition resupply and
  reinforcements notices, then wave two starts. Null renderer, private loopback,
  copied data/saves/config, microphone/public networking/downloads off.
  Player position mean/max difference **0.015/1.350**, transient mismatches in
  one position second and one kills second, **153 client corrections**.
  Per-tick drift **57/1130** apart, mean **0.000106**, max **0.006441** world units.
  Existing tolerances unchanged. Report/logs under `verification-87521711b-001`:
  `native-resupply-smoke.json`, `native-resupply-smoke/runs/b40.AB.1/`.
  Both owned processes exited; original B40/UI hashes match
  (`resupply-source-map-hashes.{before,after}.json`).
- This verifies ordinary native resupply/synchronization through one wave and
  into the next, not a full three-wave encounter, forced stale-ownership transfer,
  recovered loot, durable settlement or Linux/VR/human acceptance. No save writer,
  protocol, renderer or second director was added.

Inventory checkpoint, base `87521711b` plus tracked repairs:

- Actual deferred-transfer reproducer failed before the fix: an older partial
  inventory reclaimed a weapon assigned to a different unit. Existing real-source
  harness now covers immediate/late recipients and recovery of the remaining
  equipment through a fresh host update. Removing the invalidation call remains
  a failing negative control. Initial focused integration also found eight co-op
  fixture compile failures from missing campaign/Extraction dependencies; the
  fixture now extracts the real pending-map clear and checks Extraction loss
  dispatch/retry without claiming to simulate native spawning or settlement.
- Final focused group: **58 passed in 6.55 seconds** across
  `test_retried_inventories`, `test_coop_wipe`, `test_coop_watch_host`,
  `test_hs_map_name_context` and `test_script_level_resolve`.
  Logs: `verification-87521711b-001/{inventory-ownership-repro,
  focused-foundations,focused-wipe-repair,focused-foundations-final}.log`.
- Windows x86 compilation succeeds from disposable export
  `C:\Users\patri\AppData\Local\Temp\frn-verify-002\source`.
  Two pre-existing source-only integration errors were repaired: the listener
  header lacked `real_math.h`, and menu network getters were declared after
  their first use. Only changed files were refreshed and the same build resumed.
  Final source snapshot SHA-256:
  `cf19f5ebfb17a34c3c25156e7e9c770719a7c799f71aafdfa3d9b1741b4919b5`.
  Binary SHA-256:
  `cdc78c86fc77b759dfda5c05739e440e2ea647b5a17057115625f4af41d17a56`.
  Identity/result: `frn-verify-002/forerunner-build.json`; final link log:
  `frn-verify-002/compile-menu-declarations.log` (both outside the evidence root).
  Optional FFmpeg/bgfx omitted; this does not compile the optional OpenXR path.
- Existing `net_compare.py` ran the same binary as both peers on private loopback,
  copied data/config/saves, hidden null renderer, microphone off, public networking
  and data downloads disabled. Blood Gulch with scripted shoot/death events:
  **PASS**, 26 compared seconds, tick offset 0, maximum sampled player-position
  difference **0.177**, maximum aim difference **1 degree**, one transient grenade
  mismatch, two client corrections. Per-tick drift: **151/516 samples apart**,
  mean **0.171423**, maximum **88.195320** world units. Existing tolerances were
  unchanged. Report/logs: `verification-87521711b-001/native-inventory-smoke.json`
  and `native-inventory-smoke/runs/bloodgulch.AB.1/{host,client}.log` under that
  verification directory. Both owned game processes exited. Before/after hashes
  for original Spark `bloodgulch.map`/`ui.map` match (`smoke-source-map-hashes.*.json`).
- This proves bounded native baseline synchronization, not a forced delayed-item
  transfer, durable rewards, rendered/human gameplay, native Linux, VR compilation
  or headset acceptance. Persistent inventory and remote-owner settlement remain
  open requirements; no stash or original save was written.

| Candidate / revision | Result | Evidence |
| --- | --- | --- |
| OpenCE `1519c65facbb068e44b956da2769198a83a17847` | Windows x86 compiled, exit 0 | `opence-build.log`; `opence/build/windows/halo.exe` |
| Fractum `eae91445333bc8ebfb1604e08fcedffbd5c3a558` | Windows x86 compiled, exit 0; optional FFmpeg omitted | `fractum-build.log`; `review-fractum/build/windows/halo.exe` |
| Both candidates, `--version` | Exit 0, advertised network version 44 | `smoke-opence/version.log`, `smoke-fractum/version.log` |
| Both candidates, dedicated server without maps | Correct exit 2 and missing-data diagnostic | `smoke-*/server-without-data.log` |
| Both candidates, local Blood Gulch | Native headless map load, real player movement and simulation ticks, clean timed exit | `smoke-*/local-bloodgulch.log` |

The first Fractum 12-second smoke ended in its default 15-second lobby. The
corrected test used an explicit 4-second countdown and 20-second lifetime, logging
player ticks through 390. Neither smoke establishes visible rendering or human play.
Candidate links emit inherited Expat LNK4217 warnings; linking succeeded.

## Automated source/tool checks

- Unchanged Fractum: `test_network_rules`, `test_network_test_startup`, `test_delta`,
  `test_delta_compat`, `test_map_families`: **119 passed, 73 skipped**.
  `fractum-baseline-tests.log`. Skipped cases are not acceptance evidence.
- Windows comparison tool: **30 passed, 4 skipped**. Native tests exercise private
  copies, wrapper descendants, failed startup and forced tool death; POSIX-only
  checks are skipped on Windows.
- Isolated source-build helper: **15 passed, 1 skipped**; the skipped case requires
  Windows file-symlink permission. Real disposable Git fixtures exercise preservation,
  path boundaries, source identity, concurrent edits and excluded data.
- Workshop suite was blanket-skipped upstream. Restoring its actual production
  dependencies exposed Windows test assumptions and a real disabled-folder scan
  regression. After repairing them: **135 passed, 8 skipped**. Actual parser,
  filesystem and safe-path code is compiled by the suite. With Steam folders
  disabled, the game retains its own downloaded maps without querying the registry;
  explicit base-resource discovery remains available. `forerunner-workshop-tests.log`.

## Real peer sessions

These launch compiled engines and play actual maps with scripted player input.
They are not mocked gameplay, physical multi-machine tests or human acceptance.
No tolerance was lowered to turn a failure into a pass.

| Run | Actual result | Evidence |
| --- | --- | --- |
| Initial mixed peers, minimal map fixture | ERROR: lobby setup needs stock `carousel.map` before selecting Blood Gulch | `native-crossplay.json`, run debug logs |
| Mixed peers, complete stock multiplayer fixture, 4-second countdown | FAIL both hosting directions: clients have no live players/items; zero drift samples | `native-crossplay-complete-maps.json` |
| Mixed peers, complete fixture, normal 15-second countdown | FAIL: same dead-player/object divergence; excludes short countdown as sole cause | `native-crossplay-lobby15.json` |
| OpenCE-to-OpenCE control, 18 seconds | State comparisons agree, but ERROR: only 276 of required 300 drift samples | `native-opence-control.json` |
| OpenCE-to-OpenCE control, 30 seconds, 15-second lobby | **PASS**: 32 seconds compared; mean position difference 0.043, maximum 0.098; 0 of 960 drift samples apart; no mismatched seconds | `native-opence-control30.json` |

Mixed-peer logs showed protocol-44 metadata applying an extra checkpoint-epoch
header to a native header that already contains it, followed by dropped distributed
messages. The repair separates the native-layout floor (44) from Delta's historical
wire ledger (43), preventing known native fields from being applied twice. It
preserves announced protocol 44. Real C tests reproduce published table facts, and
an old-guard negative control fails. The signed-loader-specific Python case is
platform-skipped here; future version-45+ adaptation is unit-tested, not accepted
through native peers.

## Integrated snapshot and repair evidence

- Isolated source export: `integrated-build-001/forerunner-build.json`.
  Snapshot SHA-256: `2142b3e098b1f0ff07bcbc2ed6e79d17b8b7ad21e0267a61408627bcd1e1ecf0`.
  Based on `b2bd604329c71f06c1109c632ff19ea4c29e4027` plus identified working edits.
- Portable Windows x86 debug build: **compiled, exit 0**.
  Executable SHA-256: `9861e1a45f496b16eb80fab0fd13d1b82a4e5028174411e175299f1120c09e18`.
  Logs: `integrated-build-001/configure.log`, `integrated-build-001/compile.log`.
- Combined root regression suite: **301 passed, 87 skipped**, exit 0, 56.77 seconds.
  `forerunner-regression.log`. It includes native-layout, Delta, startup, map-family,
  process ownership, source-export and restored Workshop checks.
- Repaired native mixed peer test, same maps/countdown/tolerances:
  OpenCE host / Forerunner client **PASS**, 32 seconds compared, no mismatched
  seconds, mean position difference 0.062 and maximum 0.143; 112 of 393 drift
  samples apart (28.5%, below unchanged 60% threshold). Reverse hosting remains
  **ERROR**: live host state arrives but the OpenCE client has no local player;
  zero drift samples. `native-crossplay-fixed.json`. This is partial acceptance.
- Hidden-window OpenGL run: **FAIL**, access violation `0xC0000005` after fixed
  vertex-shader compilation errors. Actual driver: OpenGL 4.5, NVIDIA RTX 4070 Ti.
  Only a startup frame was captured; it is not rendering acceptance.
  `native-render/render.log`, `native-render/result.json`. Source diagnosis is open.

An equal advertised protocol number is not proof of interoperability.

## Subsequent repairs and latest verified snapshot

The earlier failures above are historical, not the current outcome. Local commits:

| Commit | Verified change |
| --- | --- |
| `4abc536a` | Native protocol-44 layout floor; historical Delta wire ledger unchanged |
| `cbae5069` | Queue valid player additions crossing the loading transition |
| `f4959670` | Native test readiness waits for authoritative membership and requested lobby delay |
| `5a6daa2c` | Desktop fixed-function GLSL version and immediate draw declaration safety |
| `95a9a8ad` | Workshop tests and disabled Steam-folder discovery |
| `45ce45e0` | Isolated native build, process ownership, repeatable render checker |
| `9cd38e48` | Read-only versioned Spark discovery and reference client |

`integrated-build-004/forerunner-build.json` identifies the latest compiled
source snapshot, including per-file/tool hashes and commands. Source SHA-256:
`6ffae2f1afa6945c90fe19e8f1beb6387d97136c5cbbca5e6c34b705e0366b05`.
Executable SHA-256:
`0834644353aeb7040aec4da457a57b9fd8642cc1e513260eee19308185d490e9`.
Compilation exits 0. Portable debug, PGO/LTO off, FFmpeg/bgfx omitted. The export
was taken before the local implementation commits; its manifest identifies actual
working source bytes, rather than pretending those bytes were the base commit.

Latest combined regression: **390 passed, 87 skipped**, exit 0, 93.23 seconds,
`forerunner-regression-ready.log`. This covers native layout/Delta, membership and
loading regressions, map families, owned process cleanup, source isolation,
Workshop paths, real shader generation and compiled-engine Spark discovery.
New C harness cases execute production source and include old-behavior negative
controls. The skipped platform cases remain unverified.

### Native movement: latest binary versus unchanged OpenCE

`native-crossplay-ready.json`: **4/4 PASS**, each with 32 seconds compared,
30-second requested play, 15-second lobby, unchanged tolerances. AB means OpenCE
hosts; BA means Forerunner hosts. Position values are world units.

| Map / direction | Mean / maximum position difference | Apart / drift samples | Largest drift | Transient mismatched seconds |
| --- | --- | --- | --- | --- |
| Blood Gulch AB | 0.044 / 0.098 | 91 / 893 | 0.000720 | None |
| Blood Gulch BA | 0.047 / 0.102 | 0 / 960 | 0 | None |
| Beaver Creek AB | 0.034 / 0.085 | 72 / 616 | 0.032841 | Grenades 3 |
| Beaver Creek BA | 0.013 / 0.074 | 120 / 859 | 0.008004 | Grenades 4; alive/score/deaths/friendly-fire 1 each |

No sustained divergence exceeded the existing criteria. This does not mean zero
transient differences or bit-identical simulation. Each direction has at least
300 drift samples. Raw engine/debug logs are under `native-crossplay-ready/runs/`.

### Native scripted combat

`native-crossplay-queued-events.json`: **2/2 PASS**, 47 seconds compared in each
direction with damage, death, score and respawn scripting. Tested snapshot 002
predates the later readiness/second renderer fix. AB: mean/max position
0.045/0.105, 299/1410 drift samples apart, largest drift 0.02142. BA:
0.047/0.099, 338/1203 apart, largest drift **100.795601** during scripted
teleports, with two transient grenade mismatches. These debug teleports are
logged through fair-play diagnostics; they are not ordinary on-foot physics
measurements. A full finite match and repeat on the latest binary remain open.

### Native OpenGL rendering

The first shader fix removed GLSL errors but exposed an access violation in
`vertex_shader_get` during immediate loading-progress draws. The declaration is
absent for immediate draws; the second fix avoids dereferencing it and retains
the active declaration's packed mask for streamed draws. A distinct program-mask
negative control protects that ownership rule. Shader tests: **15 pass**.

`native-render-reproducible/result.json`: latest binary exits **0**, real native
player ticks recorded, **13 framebuffer captures** through frame 2880 using
`tools/forerunner_render_check.py`. OpenGL 4.5, NVIDIA RTX 4070 Ti, driver 616.92.
Earlier snapshot 003's captured Blood Gulch frame was visually inspected: map,
weapon and plasma projectile are present. Latest captures are native artifacts,
not generated artwork. Hidden/occluded presentation stalls occurred in an earlier
cold run; there is no performance or audible-audio acceptance claim.

### Spark discovery

Contract v1 advertises only `runtime.discover`. **10 compiled-engine tests pass**:
version/schema, unsupported-version refusal, no map/display requirement, no
config/filesystem mutation, no server launch, reference-client negotiation and
safe rejection of unsupported operations. The client also refuses unchanged
OpenCE before invoking the unknown flag (`spark-legacy-refusal.log`). Live
editing, transactions, standalone server/mobile/browser adapters are unverified.

## Required but not yet demonstrated

Human gameplay; full matches and campaign/co-op completion; many-player load;
non-Windows builds/peers; Firefight/Extraction/Zombies; physical VR; live Spark
editing; connected-world traversal; clean-machine release acceptance.

## Campaign-host repair and bounded native AI readback

Snapshot 005 source SHA-256:
`51bf2e7a13df5e9dfeeaa9b585c3f7e5c417b24424f53abcd1705d3e8218890e`;
executable SHA-256:
`12b60dfe75fc3d0fbf55f1e0f578b8d1106f08135bec434995702a2a7056ffdc`.
It compiles successfully in `integrated-build-005`. The campaign host assertion
was stale Slayer state: `network_game_create_game_objects` skipped co-op's
zero-valued authoritative variant. The fix always applies that variant/options.
Nine real-source harness cases pass, including two old-condition controls.

`native-coop-b40-fixed.json`: **2/2 PASS**, 92 seconds compared each direction
against unchanged OpenCE. AB mean/max position 0.046/1.287, drift 218/1553,
largest 0.046305; BA 0.035/0.102, drift 81/1112, largest 0.164551. Transient
differences: AB position 1 second/grenades 6; BA grenades 7. No tolerances lowered.
`native-mp-after-coop-fix.json`: Blood Gulch **2/2 PASS**, 32 seconds each,
zero drift in 960 samples each and no mismatched seconds.

`native-coop-ai-b40.json`: native Forerunner/Forerunner player comparison passes.
Actual world actor logs contain 65 common non-cinematic snapshots/672 unit
comparisons, nine common living/moving actors, seven damaged actors and six
death transitions. `native-coop-ai-b40-actors-semantic.json`: **PASS**. Largest
position difference 1.373198 was transient, below persistence criteria.
The first raw comparison failed only on negative corpse overkill versus zero.
Source confirms dead units leave actor control and kill replay uses zero;
the narrow rule equates nonpositive bodies only when both are confirmed dead.
147 normalized samples/raw maximum 5.166667 remain reported. Live/positive body,
death, shield, identity, seat and position checks remain active. The raw failure
is retained as `native-coop-ai-b40-actors.json`; no gameplay health was altered.

`native-coop-a10-fixed.json`: no startup assertion, but **ERROR** after 112
seconds of opening state with zero on-foot samples; not campaign acceptance.
Original failed A10/B40 host assertions and symbolization remain preserved.
Campaign fixture copies match original authorized hashes; B40:
`0A7B26D6D828E7B773D36D9D5C798B9BF4B143332301A0C0E7A3D685782EAB48`.

Focused regression: **431 passed, 87 skipped** (`forerunner-campaign-regression.log`),
plus **10 compiled-engine Spark tests passed** on snapshot 005. Final actor
semantic tool checks: **30 passed** (`actor-semantic-tests.log`, overlapping
21 earlier checks). Actor logger real-source cases/controls: 20 passed within
the regression. No full campaign, Firefight match or non-Windows build claimed.

## First finite Firefight encounter

Gameplay commit `21a809b004a057548b641d023df24a613cf14f47`, local `master`.
Existing `coop_enemies.c` adapts Faber's actor construction, pursuit and
clear-all-wave progression; existing co-op packets carry actors, damage and notices.
Host setting `network.firefight_encounter` is off by default. This bounded
encounter awards 100 shared team points per confirmed wave-enemy death.

`firefight-build-002` compiled successfully. Source snapshot SHA-256:
`d990a0d29b1913bb58dd2ed3977c73b91b5ab3fcfbb7b490447b94f8a4d4a275`;
executable SHA-256:
`fb8d204f21e57a4db6ed44f078338a02023c49a3998b6a437e1f4d1d8f8fa4b7`.
`native-firefight-001` spawned enemies but stalled in wave 1: missing pursuit.
The second attempt reused Faber's pursuit/perception/path code instead of
teleporting units or injecting damage. Both attempts remain preserved.

`native-firefight-002/result.json`: **three waves completed**, two hidden
OpenGL Windows clients on B40, automated aiming/fire input, normal projectiles
and finite ammunition, no scripted kill/damage injection. All six spawned
handles were observed alive, moving and dead on both peers
(`wave-unit-evidence.json`). Host movement per wave unit: 1.63 to 2.60 world units;
client movement: 1.52 to 2.46. Both receive wave scores **200/400/600** and victory;
both private debug logs record postgame then pregame. Processes were stopped by
their owning test after that transition; this is not a clean-exit benchmark.

Existing player comparator: **PASS**, 88 seconds compared, mean/max position
0.040/0.420, drift 349/901 (38.7%), largest 0.011993, two transient grenade
seconds and one alive mismatch. Actor comparator with damage required: **PASS**,
61 snapshots/443 unit comparisons, maximum position difference 0.992687,
12 common movers including ambient units, seven damaged units, six deaths.
Raw corpse overkill remains visible under the previously documented semantic rule.
No tolerances lowered. `player-agreement.json`, `actor-agreement.json`.

56 framebuffer captures per peer. Visual inspection confirms first-person weapon,
HUD, wave notices, real enemy combat and return to menus. Representative capture:
`native-firefight-002/observed-client-5400.png`; the original BMP is retained.
Focused existing co-op checks: **83 passed**, `firefight-focused-regression.log`.
An initial invocation used an absent test path, ran no tests, and was corrected.

This accepts one bounded automated encounter, not the full Faber feature set or
human gameplay. Full HUD/lives/settings, individual kill credit/medals, defeat,
late join/reconnect, creator-map and non-Windows acceptance remain open.

## Subsequent source-only work: not runtime evidence

The owner deferred tests, builds, runtime queries and game launches. Shared-life
source `c8196f81` and the subsequent loaded-actor/native-spawn increment have not
been compiled or executed. Historical B40 results above do not cover these edits.
The new source retains native faction/swarm attachment, requires valid placement
and excludes feigned Flood deaths from wave credit; it does not establish Zombie
gameplay, carrier-child accounting, an authored-rules importer or MP-map support.

The requested companion check-in read Spark at `1f16760` and its unfinished
Firefight source edits. Its registration/discovery consumer and prepared-round
export are implemented source, not a tested Spark-to-Forerunner loop. The existing
integration contract records the concrete OpenCE build and shared-folder launch
guards. No Spark file, original map, runtime or installation was changed; no test,
build, capability query, game launch, fetch or push was performed in this increment.

Source checkpoint `1e791b589` replaces wave-death polling with a final `unit_died`
notification, retaining exact handles and one award before destruction can remove
the body. It also fixes `actors_spawn_from_unit` using the ordinary actor binding
after selecting a swarm binding. Static source review only: no tests, build or game
was run. Carrier child spawning/accounting and the recorded physics crash remain
open; these changes are not Zombie gameplay acceptance.

The subsequent source-only carrier increment adapts Faber's tracked-unit table
(32 slots) and Outbreak encounterless-child gate. Only tracked Firefight parents
get that gate; their real native children join pursuit, final-death credit and wave
completion. A pending carrier spawn holds the wave open; partial/unsupported or
over-capacity creation aborts. No tests/builds/native runs were performed. Normal
object deletion was confirmed deferred by source inspection, so the apparent
destruction-order issue was not patched or claimed as the Zombie crash's cause.

The next source-only increment adds a one-time host Firefight summary to the
existing reliable new-client-ready branch, plus terminal outcome replay. Repeated
object-resync requests do not trigger that branch. It shares the carrier-pending
calculation with wave completion and changes no message layout or discovery
operation. No tests/builds/native runs were performed; text replay does not accept
late-join actors, reconnects or a full Firefight HUD.

The next source-only lifecycle repair clears the whole finite encounter on a
full world revert, restoring the original map-start lives and clearing score,
wave/timers and old handles. The existing respawn-only wipe path does not invoke
this reset. Preparation retires restored pending hostile carrier bursts before
new waves. No tests/builds/native runs were performed; saved mid-wave continuation
and complete restart/reconnect gameplay acceptance remain open.

The next source-only increment adds optional one-shot desktop startup of an
installed CE solo map through the existing menu/campaign/profile/loader path.
It uses `game.start_map` / `HALO_START_MAP`, existing resource checks, and the
existing persistent-checkpoint opt-out for fresh authored playback. It does not
create a profile, host a session or import rules; refusal leaves normal menus.
No tests, builds, capability queries or game launches were run. Accepted binaries
predate this setting; authored revision loading and complete combat remain open.

The next source-only increment adds individual kill credit for the finite
director's exact tracked AI bodies, adapting Faber's final-kill notification to
the native replicated player counter. It retains the actual last damage owner,
clears attribution for AI/environmental/no-statistics damage and awards only once
on final death, with departed/reused handles refused and short-counter saturation.
Source review only; no tests/builds/queries/games ran. This is not accepted kill
agreement, full individual scoring, assists, medals or a Firefight HUD.

The next source-only increment permits one network co-op host to start the finite
director. It retains the existing zero-player, cinematic and all-present-players
alive/on-foot checks. No tests/builds/queries/games ran; neither solo acceptance nor
new two-peer acceptance is claimed. Historical Zombie crash source review did not
prove a cause, and no speculative physics/lifetime patch was applied.

The next source-only increment sends changed combat counts through the existing
host HUD/reliable client notice path, bounded to once per second. It shares the
late-join formatter and resets its text/timer cache at map start/full revert.
No tests/builds/queries/games ran. This is transient feedback source, not accepted
visual/peer agreement or a persistent Firefight HUD.

The next source-only repair reserves the respawn paid for by each available
shared life, including the final one at zero. Grants use full player handles,
survive failed native spawns, clear after a live owned spawn and are included in
team-wipe/defeat decisions. All-dead spawn failures reuse the existing death timer.
No tests/builds/queries/games ran. Final-life, simultaneous deaths, failure retry,
quit/slot reuse and exhausted defeat remain unaccepted gameplay gates.

The next source-only repair keeps a dead tracked carrier off the garbage list
until the existing native spawn attempt finishes, then restores the normal
garbage policy. Direct destruction and zero-child outcomes remain native.
No tests/builds/queries/games ran. Collection under memory pressure, actual child
combat/peer agreement and the historical Zombie crash remain unaccepted.

The next source-only repair checks a child variant's required actor/unit
references before native getters/placement, adapting Faber's existing spawn check.
An incomplete variant warns and fails; a tracked Firefight request also aborts.
No tests/builds/queries/games ran. The retired raw C47 QA files were not recovered;
its retained nearest-symbol receipt does not prove the old physics-crash cause.

The next source-only increment adds optional carried-weapon resupply between
finite waves, adapting Faber's reserve refill and using existing reserve/battery
inventory snapshots. It is off by default and preserves magazine/reload state.
No tests/builds/queries/games ran. Ballistic reserve and battery replenishment,
reload timing, excluded dead/dropped inventory and real owner-client agreement
remain unaccepted.

The next source-only transition repair places the existing Firefight arena return
before MCC package and Xbox/CE campaign completion. This avoids the source path
through MCC progress/next-mission handling for finite victory, defeat or abort.
No tests/builds/queries/games or profile writes ran. Result/lobby behavior and
campaign progress protection still require native validation.

The companion milestone read Spark `3e47ad0` and its unfinished append count/grid
changes. Its source-backed encounter append still enters through an MCC-only
working-tag route; no CE recovered Firefight/controller or Forerunner launch pass
was established. The existing contract records the source boundary and next work.
No tests/builds/queries/games or Spark modifications ran in this review.

Authored-solo source audit confirmed the existing CE campaign/profile guards and
fresh-start checkpoint opt-out. Restart Level resets loaded state; a revised cache
needs a fresh process. Spark's current terminal/score globals do not establish
visible HUD/outcome/replay behavior. No tests/builds/queries/games or profile writes
ran; source inspection is not authored-round gameplay acceptance.

Source-only menu increment adds finite Firefight OFF/ON to existing Co-op Options
through the current generator and setting binding. Only server-settings XML was
regenerated; init/save/cancel and row fitting reuse existing code. No tests/builds/
queries/games ran. Native layout/input, persistence/cancel and hosting behavior
remain unverified; historical B40 gameplay does not cover this menu control.

Source-only transition repair prevents the existing clear/preparation countdowns
from advancing during a pending unfunded team wipe. In particular, final-wave
victory cannot overtake the later three-second elimination deadline. Existing
alive/paid-respawn checks release the hold; existing defeat/return handles expiry.
No tests/builds/queries/games ran. Fatal final-wave timing, funded respawns and
normal progression require native acceptance; prior B40 results do not cover this.

Resumed source-only handoff increment retains a validated CE solo startup request
while the user creates a native engine profile in an empty private save folder.
It starts once in the same process without old-checkpoint loading; existing
session rejection remains. Discovery adds optional desktop `startup_map` metadata,
not a new operation or map-loaded acknowledgement. Spark read-only source review
reached `3329a4a`; owned receipt staging now exists but uses the older menu-only
binary. No tests/builds/queries/games ran. Cold/existing profile behavior, malformed
input, session conflicts, revised cache loading and authored gameplay remain open.

Source-only shared-life repair skips eliminated players in network respawn
iteration and retry scheduling using the existing host eligibility predicate.
Paid grants/funded failures retain retries; ordinary co-op is not restricted.
No tests/builds/queries/games ran. Mixed paid/unpaid deaths, zero-life late join,
failed placement, quit/slot reuse and victory/defeat return remain native gates.

Independent source increment adds opt-in `game.solo_skulls` for native local solo
maps through the existing skull parser/effects, with native start/restart/disposal
hooks and Bandanna's existing ammo helper. Network sessions, multiplayer scenarios,
editors and MCC-owned state are excluded; unsupported names warn and are skipped.
No tests/builds/queries/games ran. Effect fidelity, map transitions, restart and
checkpoint behavior, empty defaults and unchanged MCC/network behavior remain
unverified. No multiplayer, scoring, menu or Spark-rules acceptance is implied.

Independent Forge source increment adds console-only `forge_clone` using existing
native creation and host/quota gates, adapting Faber's placement recipe. Old table
indices stay fixed; map admission forbids the new function. Unsupported classes,
attached/dead objects, objective weapons and scaled network copies are refused.
No tests/builds/queries/games ran. Native handle composition, placement/appearance,
allocation/quota failure, client/public-host refusal, late join, platform/script
table compatibility and unchanged native gameplay remain unverified. No persistent
layout, AI cloning, full Forge UI or Spark write operation is claimed.

Independent co-op source repair credits all non-quit host-local participants for
the actual network campaign mission through the existing bounded profile writer.
Solo callers keep their wrapper; finite Firefight/MCC paths remain separate and
unknown CE missions cannot set stock flags. No tests/builds/queries/games or
profile writes ran. Stale solo selection, split-screen/dead/quit/absent profiles,
difficulty flags and normal solo behavior remain unverified. Remote-client
progression still needs an explicit authoritative completion interface.

Native creation excludes dynamic scenery; `forge_clone` now refuses host scenery
copies before allocation/quota changes. Local scenery and existing shared classes
retain their paths. No tests/builds/queries/games ran; refusal, quota preservation
and peer geometry agreement remain unverified. No new transport was added.

Approved Windows64 VR source checkpoint: optional OpenXR/Vulkan bootstrap,
runtime device/GPU selection through the existing bgfx renderer, shared
Touch/Index actions, session/frame lifecycle and eye-image ownership APIs are
present in source. Reuses OpenH1 `baed77571d53abe32b27434b4ba869f11db00fe9`
with attribution. No configure/header download, build, tests, runtime query,
game launch or hardware validation ran. The frame APIs are not yet called by
the game loop; stereo renderer import/synchronization, camera/FOV and gameplay
input wiring and physical interactions remain open. Neither headset is claimed
supported in practice, and no performance comparison has been measured.

The subsequent source-only renderer adapter uses bgfx's existing borrowed-image
support for acquired OpenXR textures, existing logical target scaling and the
same Vulkan submission queue. A scoped queue patch avoids transitioning cached
images outside application ownership and leaves released eyes in the required
color-attachment layout. Source also supplies a right-eye desktop mirror and
headset-session pacing exclusions. The earlier device patch now uses exact raw
pinned source context. No patch application, compilation, tests or runtime/
hardware validation ran. Stereo game cameras/FOV, menus, input, color-space
appearance, failure recovery, image lifetime and actual GPU scheduling remain
unverified; the game-side caller is still unfinished. No performance result or
playable VR support is claimed.

The next source checkpoint connects the OpenH1-derived eye pose/scale and
asymmetric FOV adapter to one existing native frame traversal, retaining native
local-player indices. Native camera layouts, simulated player count, tick rate
and wire protocol are unchanged. Local-reference/session re-anchoring and XR
cadence bypasses are source only. A skipped-submit Vulkan hook flushes pending
resource commands before releasing eyes and submits no projection for skipped
views. No patch application, builds, tests, launches, runtime queries or headset
runs occurred. Both headset targets, pose/projection accuracy, resize/loss,
render caches/occlusion, color, wall collision, menus, native game controls and
physical interactions remain unverified or unfinished. No full-VR or measured
performance acceptance is claimed.

Basic Touch/Index input and rumble are now connected in source through native
Xbox input. Focus/stale/tracking neutralization, a reserved-button-free Index
pause combination and combined controller hints are present; input reuses
located poses and freshly synchronizes actions. XR focus does not bypass SDL's
remapper gate. No tests, builds, native queries, launches or hardware sessions
ran. Button/axis signs and layout behavior, menu/chat/guide ownership, fallback,
focus loss/recovery, haptics and both controller families remain unverified.
Tracked weapon aim, physical interactions and headset menu display are unfinished.

Headset menu display now has UNVERIFIED source: existing native menus/overlay
compose once into the left-eye image and submit as a VIEW-space quad, with the
same completed canvas mirrored to the desktop. Menu ownership suppresses stereo
preparation for that frame; gameplay keeps the existing stereo path. Only source
and diffs were read. No builds, tests, patch application, runtime queries, game
launches or hardware validation ran. Image orientation/color, clipping, focus,
mouse/controller transitions, hidden-window presentation, resize/loss and comfort
remain unverified. Tracked pointers, weapon aim and physical interactions remain
unfinished; no full-VR acceptance is claimed.

UNVERIFIED replay source now follows non-player bipeds, corpse/death state and
world-space attached objects in the existing HREC object stream. The browser
viewer can display these tracks with existing local-map models or a labeled
placeholder. Source/diffs only; no parser tests, builds, recordings, browser
launches or native runs were executed. AI animation/action fidelity is open.

UNVERIFIED Extraction interaction/state source uses native controls and a named
zone, with no raid timer, cancellable ten-second activation, death/identity checks
and checkpoint-rewind invalidation. Only source/diffs read; no tests, builds,
launches or Spark changes. This is not a complete deploy/loot/escape/persistence
loop. The second flagship single-map acceptance and 19-region MegaWorld stages
remain open, alongside VR/Firefight. No region streaming or traversal is accepted.

UNVERIFIED native voice privacy repair fails closed for unavailable named
microphones and applies mode/device changes by closing capture and discarding
queued/codec history. Source/diffs and existing declarations were read only; no
audio device enumeration, microphone access, tests, builds or game/headset runs.
The source audit is in UPSTREAM_FEATURE_MATRIX.md. Cross-session party transport,
OpenCE voice bridging, VR listener tracking and region/portal occlusion remain
open; inherited source wiring is not a functional audio acceptance claim.

UNVERIFIED shared native equipment capture is connected to Extraction return
reports/redeployment and Firefight wave-clear summaries. Source/diffs only; no
tests, builds, launches or gameplay. Physical hub/activation routing, vehicle
traversal, exact ammo/battery/ownership handling and both native Linux/Windows
builds remain unverified. No durable stash, crash-safe award, cross-cache item
admission, large-session performance or complete flagship acceptance is claimed.

UNVERIFIED positional audio now uses Halo's listener-space transform for native
proximity voice. Unlocated proximity is silent; team/party radio stays 2D.
Optional XR builds supply the centered eye-camera pose to the existing listener
and run sound once after camera preparation. Native acoustic location is queried
for the moved head; invalid/stale poses use native fallback. Source/diffs only:
no builds, tests, audio-device access, launches or hardware sessions. Direction,
gain, latency and head/underwater/boundary transitions remain unverified; no voice
portal occlusion, MegaWorld acoustics or full-VR acceptance is claimed.

UNVERIFIED Firefight source restores general host HaloScript updates and disables
the finite director for recognized Spark compiled round scripts. A narrow result
guard prevents unrelated campaign wins from ending an active finite round early.
Only source, caller/dependency paths and diffs were read. No builds, tests or game
launches; prior B40 results predate these semantics. Doors/triggers/objectives,
single director selection, native authored rounds and finite outcome timing all
need authorized gameplay validation. The AGENTS full-roadmap update preserves the
source-only/safety rules and is not an acceptance claim.

UNVERIFIED Extraction safeguards bind the local saved profile, explicitly reset
outside-image run state in native before-load callbacks, and require successful
native equipment capture before completing a return. The capture-failure message
is bounded to one per attempt/run; no gear or reward is granted. Source/dependency
review only, including pinned Faber's stats writer; no tests, builds, launches or
personal-save access. Same-clock restores, profile switching, capture recovery
and eventual durable/duplicate-resistant settlement remain unverified.

UNVERIFIED optional controller-facing source adapts pinned OpenH1 body heading,
ray convergence and collision queries into Forerunner's existing OpenXR adapter.
Native input carries facing/throttle; cached tracking is read without another
pose/action sync or XR frame owner. Default stick aiming is preserved. Map and
restore hooks clear tracked unit state. Source/caller and diff review only:
no builds, tests, game launches, microphone/device queries or headset runs.
Menu/focus/tracking loss, recentering, locomotion, firing convergence, networking
and restore behavior all require later authorized validation. Tracked gun models,
correct VR reticles, physical interactions and vehicle aiming remain unfinished.

UNVERIFIED Extraction outcome gate: source review traces HaloScript game_won
through HS_EVALUATE_NO_ARGUMENTS into main_won_map and its deferred transition.
Both gates consult the existing admitted Extraction state; first-tick script
ordering requires the deferred recheck. No scripts are suppressed and no stash
reward is granted. No builds, tests or launches. Ordinary campaign completion,
map binding failure, early scripted wins, escape and redeployment need later
native validation; death recovery and durable settlement remain open.

UNVERIFIED optional voice obstruction reuses compute_sound_obstruction,
native dsound obstruction/occlusion conversions and the existing I3DL2 mixer.
All private mixer callers were reviewed. Speaker/environment state is local;
radio routes clear/ignore it, unlocated/unreachable proximity stays silent, and
the setting defaults off. Native query-cache BSP keys and clear hooks prevent
geometry/restore reuse. Source/diff review only; no tests, builds, launches,
audio enumeration or microphone/headset runs. Audible attenuation, clear paths,
wall/door/tunnel transitions, map/restore cache behavior, radio and Linux/Windows
parity all await authorization. XR-origin and multi-region audio remain open.

UNVERIFIED local Extraction recovery source shares native player_spawn/equipment
and all-dead handling with the existing checkpoint respawn API. All callers were
read: ordinary/co-op/finite Firefight calls retain checkpoint relocation; the
Extraction loss path chooses authored starts without checkpoint/teammate moves.
The run result is cleared; allocation failure retains native timed retry, and
scripted failures with living players retain campaign handling. No tests,
builds, launches or save/profile access. Real death, repeated wipes, allocation
failure, authored hub starts, equipment, world/AI/loot continuity and ordinary
respawns all need later native validation. Persistence and multiplayer remain
unfinished; source presence is not proof of safe respawn or loss acceptance.

UNVERIFIED controller weapon posing adapts pinned OpenH1's weapon camera and
grip-offset defaults into existing native first-person node construction. Source
review follows render_window through first_person_weapon_render_update/node
matrices and interpolation, then world rendering with the restored head camera.
Both eyes reuse one pose; viewmodel projection begin/end keeps balanced nesting
while skipping flat-screen FOV changes during stereo eye passes. Invalid offset
text/non-finite/out-of-range values use upstream defaults. No tests, builds,
launches or hardware access. Pose/grip alignment, animated weapon effects,
both-eye consistency, near-depth/projection, tracking loss, recentering, model
visibility and native flat/Linux parity all require later authorized validation.
Correct VR reticles and physical interaction acceptance remain open.

UNVERIFIED VR reticle source follows native pre-spread ray/autoaim preview,
finite projectile collision and per-eye native world-to-screen projection.
The same cached target serves both eye HUD passes; only is_crosshair_bitmap
pieces inside crosshairs_draw are moved/hidden. Scope images and CE warnings
retain existing flags/placement. Shared HUD function retains the original FOV
expression outside the XR scope. Native body-camera gamepad weapon posing now
follows pinned OpenH1. Source/callers/diffs only; no tests, builds, launches,
headset access or native validation. Hit/spread references, both-eye consistency,
FOV/viewport changes, HUD warnings, tracking loss, aiming transitions, recenter,
world/fog rendering, Linux/flat parity and comfort need authorized validation.
This is not ballistic prediction, a physical scope or hardware acceptance.

UNVERIFIED settings flush repair: pinned SDL header and source flow reviewed;
failed flush now prevents replacement. No tests, builds, launches or settings/
save access. Fault behavior and platform parity await authorized validation.

UNVERIFIED profile-directory guard: native enumeration/path-copy bounds and
Extraction deployment/return/reset paths were read. Same-slot/different-directory
returns are refused; unavailable lookups remain pending with bounded cadence.
No tests, builds, launches or personal settings/save access. Re-enumeration,
missing/read-only profiles, transient lookup failure, clock resets and normal
returns need later native validation. This is local ownership evidence only,
not durable identity, authenticated networking or a stash persistence claim.

UNVERIFIED host wave-limit/count source: inspected all wave/score readers,
initialization/revert paths, native spread indices and shared child capacity.
Defaults retain three waves/two initial enemies and the existing victory text.
Zero-limit progression reuses Faber's set_limit policy; native scripts still
disable this producer. Placement excludes dead bipeds and refuses full occupancy
queries. No tests, builds, launches or native validation. Default/custom/endless
outcomes, resource pressure, corpse clearance, carrier children, late joins,
lives/reverts/resupply and platform parity require later validation. Wave/score
saturation is source-reviewed, not a duration or population performance claim.

UNVERIFIED headset obstruction source follows pinned OpenH1's applied-head
query and eye/head clearance into existing native listener/mixer and BSP queries.
Published origin/cluster is taken after matrix application and rejected across
BSP mismatch. Both cache levels now key geometry/mode and clear on map/restore;
source geometry fallback retains head orientation without observer mutation.
Reviewed native/server source inclusion and non-OpenXR guards only. No tests,
builds, launches, audio enumeration or hardware access. Leaning/blocked heads,
doors/tunnels/underwater, tracking expiry, mode/BSP/restore changes, cache cadence,
radio/proximity and Linux/Windows parity require later authorized validation.

UNVERIFIED snap-turn source: reviewed pinned OpenH1's vr.c latch/thresholds,
native player-control call ownership, cached XR tracking, body heading and
world/menu reset paths. Added optional controller-only snap mode with native
smooth default, neutral rearming and bounded angle fallback. No new XR action
sync, renderer, packets or simulation rate. No tests, builds, launches or
hardware access. Threshold/held-stick behavior, turning direction, focus/menu
resume, recenter/restore, native mixed controls, vehicles, Touch/Index bindings
and Linux/non-OpenXR parity need later authorized validation.

UNVERIFIED Extraction marker admission: inspected Spark f8213a5 native logic
source, native scenario name/index structures, control_touched trigger mode and
runtime outcome/reset callers. Binding now requires unique names and distinct
touched-by-player native terminals, with representable short-index counts.
No tests, builds, launches or assets/saves accessed. Missing/duplicate/aliased
markers, wrong control modes, ordinary campaign fallback, valid solo deployment,
all hub starts, death recovery and redeployment need later authorized validation.

UNVERIFIED bounded survival growth: reviewed existing Faber capped roster/count
policy, host wave progression, native spawn/child tracking, map-start settings
and full-revert paths. The optional growth rule uses the same producer and
bounds the wave-derived multiplier before arithmetic. Zero default preserves
fixed size, and authored scripts still disable this director. No tests, builds,
launches or hardware/save access. Default/invalid/custom/endless sizes, saturation,
reverts, carrier capacity, placement pressure and peer replication remain native
validation gates; this is not measured large-horde or Zombie-mode acceptance.

UNVERIFIED upstream map-transfer reuse: compared OpenCE-Forge 2467e708..91099ce3
and cherry-picked ebfa05b7/796df195 with authorship/provenance. Inspected request
array bounds, host/peer selection, allowance/rate widths, owned serving streams
versus borrowed download streams, frame cleanup and upload setting consumers.
Forerunner-specific private-join status now shares download_rate with log/overlay.
No tests, builds, generators, launches, assets/saves or publication were run.
The generator and its committed menu output came together unchanged. Transfer
rates, concurrent peers, partial seeding, disconnect/restore cleanup, payload
hashes and Linux/Windows/web/guest parity require later authorized validation.
Upstream's commit messages describe upstream evidence, not Forerunner acceptance.

UNVERIFIED upstream campaign/co-op source adoption: reviewed original commits
and merge differences in native director/observer, co-op presentation, owning
HS thread checks, CE/MCC destination lookup, outcomes and restore callbacks.
Preserved existing downloaded-map filtering, trusted menus, Stubbs, MCC order
and host local-profile credit; CE uses actual map-family APIs and format checks.
Pending destinations clear through existing lifecycle/restore paths, and reduced
MCC/CE configurations retain guards. Original upstream fixture sources came with
their commits; the CE fixture was adapted, but no tests were run or generated.
No builds, launches, hardware or save access. Dedicated/P2P cutscenes, old-host
fallback, package/CE bounds, first-tick mode ownership, menu/Stubbs selection,
death/restart/restore, progress credit and platform parity require later native
validation. Original commit test reports remain upstream-only evidence.

UNVERIFIED XR PTT source: reviewed existing Touch/Index actions, cached tracking
and native input/Guide/chat/capture ownership plus every halo_keyboard_actions
consumer (gameplay, both chats, votes and voice). The optional binding emits only
the existing PTT bit, reserves its gameplay button through tracking gaps, and
requires release after ownership/focus/reference/binding changes. Native pause,
voice modes/device/host restrictions remain owners; no voice protocol changed.
No tests, builds, launches, audio/device enumeration or hardware/save access.
Selected/unknown/off bindings, held-button resume, pause/Guide/chat/capture,
hand tracking, recenter, microphone permission/device/mode changes, voting and
Quest 2/Index plus Linux/non-XR parity require later authorized validation.
