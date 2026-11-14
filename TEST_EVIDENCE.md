# Forerunner test evidence

14 November 2026. Source inspection, compilation, automated native simulation,
rendered gameplay, human play and cross-platform acceptance are distinct here.
Bounded native multiplayer, rendering and read-only Spark discovery are verified;
full gameplay, Firefight, Extraction, VR, editing and MegaWorld acceptance remain open.

All local evidence below is in the external owned directory
`C:\Users\patri\AppData\Local\Temp\forerunner-20261114`.

## Toolchain and inputs

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
