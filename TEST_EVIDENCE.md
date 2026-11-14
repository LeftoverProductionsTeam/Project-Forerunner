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
