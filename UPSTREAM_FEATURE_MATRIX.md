# Forerunner upstream feature matrix

Reviewed 2026-11-14. These are pinned source snapshots, not a claim that every
advertised feature has passed Forerunner acceptance. Source presence, compilation,
automated checks, and gameplay evidence are separate gates. See
[TEST_EVIDENCE.md](TEST_EVIDENCE.md) for run results.

## Foundation and provenance

The selected Windows x86 foundation is FractumSeraph's combined OpenCE/Chupathingy tree at
`eae91445333bc8ebfb1604e08fcedffbd5c3a558`. It contains the inspected OpenCE
HEAD as an ancestor; their left/right divergence is 0/3,734 commits. Its common
ancestor with inspected Chupathingy HEAD is
`a42702df52416de4008e91fe18c49f94d9efabb0` (1/1,760 commits).
Ancestry establishes traceability, not behavioral equivalence.

The sole missing Chupathingy commit, `78d28a70839998775322af8aa571fa494966c0c8`,
was cherry-picked with `-x` as `b2bd604329c71f06c1109c632ff19ea4c29e4027`,
retaining Milenko's authorship. It validates Workshop filenames and preview paths
in `port/linux/src/steam_workshop.c`, with parser/fuzzer regressions. The overlap
with Fractum's narrower dot-name check was resolved using the stronger upstream
component validator. Validation of the resulting tree is recorded separately.

| Repository | Inspected commit | Common ancestor with inspected OpenCE | Decision |
| --- | --- | --- | --- |
| [OpenCE](https://github.com/OpenCommunityEdition/OpenCE) | `1519c65facbb068e44b956da2769198a83a17847` | Same commit | Retain current fixes through the combined foundation; use as comparison build. |
| [Chupathingy](https://github.com/ChupathingyCE/chupathingyce) | `78d28a70839998775322af8aa571fa494966c0c8` | Shared ancestry is preserved through Fractum; see comparison above | Retain combined implementations; import missing changes individually with author credit. |
| [FractumSeraph/OpenCE](https://github.com/FractumSeraph/OpenCE) | `eae91445333bc8ebfb1604e08fcedffbd5c3a558` | `1519c65facbb068e44b956da2769198a83a17847` | Selected after repairs, Windows x86 compilation, swapped-host native movement/combat and hidden OpenGL gameplay. Other acceptance gates remain open. |
| [OpenCE-Faber](https://github.com/SutlabSol/OpenCE-Faber) | `e4ffe0fe9874870b4fca1fca76beb83514c793f4` | `da8d455623cf50002e6a798b1980068fd3ca0292` | Adapt Firefight and object editing incrementally. Whole-branch adoption would replace newer core fixes. |
| [Watanoge/halo-content-evolved](https://github.com/Watanoge/halo-content-evolved) | `aff7a5d1b8ef592eea1106b64713e2a5d0a74569` | Not computed; source inspected through read-only GitHub access | Mota-recommended Forge/Firefight compatibility reference. Inspect its existing extensions before implementing equivalents; no branch merge, network compatibility or gameplay acceptance is claimed. |
| [Mota's OpenCE-Forge, main](https://github.com/SutlabSol/OpenCE-Forge) | `91099ce375632167e425edd7fd40dadf83ab93d2` | Source refresh, 13 commits after the prior `2467e708` pin | Version 2.10.11 source uses Forge protocol `0x4630`, separate from stock sessions. Reused compatible OpenCE transfer commits `ebfa05b7` and `796df195` with original authorship; Forge streaming/placement and live compatibility remain unimplemented/unverified here. |
| [OpenH1/GOAT H1, dev](https://github.com/OpenH1/goath1) | `baed77571d53abe32b27434b4ba869f11db00fe9` | `6ec42856d73e258e5a8c7e9cd19161f46d89d0e4` | Separate optional VR/renderer adaptation; do not merge its protocol and simulation changes wholesale. |
| [GulchCE](https://github.com/chestahh/GulchCE) | `66b4df0bc9afeb09189a7f9c0fff0c5b1f5d51b2` | `1519c65facbb068e44b956da2769198a83a17847` | Compare individual MCC fixes against already inherited MCC modules; preserve map-family boundaries. |
| [DamnationCE](https://github.com/xshxdex98/DamnationCE) | `d31736d3748f1156995e6bba13126f87aeff40ee` | `0b2c6759bb4cb3ee5dcc3d650de2dcda74d79620` | Older CE/co-op reference; avoid copying AI/network systems already present in newer ancestry. |
| [OpenCE-PS4](https://github.com/iHaiDeeZ/OpenCE-PS4) | `dcf1b836085829999a004afef86bb3c45d361642` | No common Git ancestor: patch/toolchain repository | Optional console adapter. `patches/BASE.txt` separately pins engine base `9662f0f6cb60746444bee9549e4271e387fdc659`. |

## Implementation and acceptance boundaries

| Capability | Actual source examined | Current integration decision and evidence |
| --- | --- | --- |
| Classic simulation and independent rendering | `source/game/game_time.c`; `port/linux/src/port_config.c` (`display.interpolation`); `tools/windows_build.py` (`-ffp-contract=off`); vendored musl math | Retain 30 Hz behavior and stock options. OpenCE and Fractum Windows x86 candidates compiled and ran local Blood Gulch player ticks. Full campaign/physics/rendering acceptance remains open. |
| Multiplayer, co-op, dedicated servers | `network_distributed.c`, `network_actors.c`, `network_coop.c`, `network_rules.c`, `server_*.c`; `halo_port_limits.h` | Retain host authority and existing per-peer serializers. Capacity is 128 players/machines in source; a 128-player session has not been demonstrated. Cross-play requires its own native gate. |
| Infection and conventional extra modes | `port/linux/game/infection.c`, `game_modes.c`, `gun_game.c` | Existing source is inherited; do not reimplement Infection. No completed Infection match is accepted here yet. |
| Xbox, PC, CE, HaloMD, MCC assets | `map_families.c`, `ce_*.c`, `mcc_*.c`, `steam_workshop.c`; `docs/mcc_maps.md` | Preserve distinct loaders/resource ownership. Xbox smoke tests do not establish CE or MCC fidelity. Workshop parser fix imported; broader format/gameplay testing remains open. |
| Forge and live object editing | Faber `port/forge/src/forge_3d_mode.c`, `forge_network.c`, `forge_hooks.c`; build-time symbol replacement | Real upstream code, not integrated. Windows input/hooks and its separate Forge agreement require adapters. Save/export claims need native readback and target-runtime tests. |
| Firefight | Faber `firefight.c`, `firefight_spawner.c`, `firefight_generators.c`, `firefight_stats.c`, `firefight_network.c`; game-engine callbacks | Actor creation/pursuit and clear-all-wave progression adapted into existing co-op code in `21a809b0`. Three-wave B40 encounter completes with two native clients. Full HUD/lives/settings and remaining systems are open. No native-struct packet imported; future structured state needs negotiated Delta support. |
| Extraction and AI zombie survival | No corresponding native mode found in the reviewed foundation/Faber trees | Not implemented in Forerunner. Build on shared native objectives/spawning after baseline acceptance; asset-extraction utilities are unrelated. |
| Vulkan/Metal | Inherited `tools/bgfx_build.py`, `port/linux/src/d3d8_bgfx.inl`; OpenH1 host renderers | Keep existing optional bgfx path before replacing stable rendering. Windows64/OpenXR compiles and bounded native desktop fallback/interoperability are recorded in TEST_EVIDENCE.md; headset and full backend acceptance remain open. |
| OpenXR, tracked input, hand interaction | OpenH1 `port/linux/host/host_vr.c`, `port/linux/src/vr.c`, `port/linux/game/vr_*.c` | Adapt as optional capability. Linux builders enable VR; Windows builder explicitly uses `vr=False`. Host/local physical reload and remote-client timed reload differ by design. No headset or mixed-session test performed. |
| Audio, HUD, input | Inherited SDL3 platform/audio/input and HUD modules | Retain existing implementations and artwork notices. Headless dummy-audio runs prove no audible/spatial audio result. |
| Recording and replay | Inherited `network_recording.c`, `guide_record.c`; Fractum viewer/service code | Retain source; replay fidelity and production playback acceptance remain open. Do not treat every recording format as deterministic Theater. |
| Spark adapter and MegaWorld | `server_mode.c` capability contract v1; `tools/spark_runtime.py` reference client | Read-only discovery implemented and tested against real engine. Live editing and connected-world traversal are not implemented. |

## Licensing and repository rules

Mota's 14 November community lead identifies Watanoge's Forge/Firefight additions
and describes Mota's newer streaming-tag/placement-sidecar work. The inspected
Watanoge snapshot credits Mota and contains `port/forge/src/forge_runtime_tags.c`,
`forge_network.c`, `forge_cache_builder.c` and `port/linux/game/firefight*.c`.
Its documented pack path imports tags on demand, preloads network actors in
matching order, and its saved-map reader also reads placement text from a CE
cache trailer. Mota subsequently supplied his separate OpenCE-Forge main branch;
its sidecar reader/writer and format are now identified in the existing Spark
contract. Watanoge's Forge session selects a distinct network version and uses
fork-specific messages; CE-file loading alone is not live Forge/Firefight
protocol compatibility. `port/linux/TAG_INJECTION.md` requires matching import
roots/order and locally available packs across peers. Our existing `mods.c`
is visual-only replacement, not gameplay-tag admission. No upstream code was
imported or executed in this audit. Follow the existing Spark contract before
adding a placement-file reader; preserve original maps and locally owned data.

Mota's later update refreshed the pin to `91099ce3` (2.10.11). Its Forge-specific
changes cover client-to-host compressed bundle retention/re-serving, held-object
slot mapping, missing-tag cache handling, reliable backlog pacing and MCC preview
decode bounds/swizzle output. Those modules are absent here; do not import a
second editor/preview layer or call them accepted Forerunner capabilities.
The compatible OpenCE map-transfer commits already present in local object
history were cherry-picked with `-x`: `ebfa05b7` and its required slow-peer
follow-up `796df195`. No fetch/remotes/branch changes were needed. Requests stay
within existing wire limits (eight host, two peer, 32 in flight); transfer
arithmetic uses 64-bit intermediates and catalog streams close each serving
frame. Forerunner's additional status caller now shares the same rate helper.
The upstream author reports its own tests in commit messages; **no Forerunner
tests, builds or launches ran**, and all local behavior/performance is UNVERIFIED.

Campaign/co-op source from that same pin now also reuses `ac152e44`, `8c70e633`,
`511da10e`, `8903981f` and `958cd833` via `cherry-pick -x`, retaining bnunu's and
chestahh's authorship. Native unattended scripted cameras, old-server fallback
and owning-thread/campaign destination handling share existing engine systems.
Local adaptation uses Forerunner's CE map families, preserves MCC order/menu and
mode safeguards, clears pending destinations through native restore/lifecycle
hooks and retains reduced-feature stubs. Source only, UNVERIFIED; upstream
validation is not local acceptance and no network version/capability changed.

Voice audit (owner-requested, source only): native desktop Chupathingy voice is
already connected through `voice_game.c`, Delta Peer, `voice.c`/Opus and
`dsound_voice_*` in the existing Halo mixer. Host proximity routing admits nearby
enemies as well as teammates; team/party radio is global within the session.
Range, PTT, open-mic opt-in, mute and channel/master volumes already exist.
Capture defaults off and uses SDL recording-device selection on Windows; no
Vive/Meta microphone SDK or second VOIP implementation is needed for that path.
None of this source inspection proves current audio or headset gameplay.

Outstanding connections: the cross-session `voice_game_set_party_path` interface
has no external caller, and `docs/delta.md` still marks the OpenCE/Delta voice
bridge planned. Proximity streams use Halo's 3D/HRTF mixer, but do not acquire
portal/collision occlusion from the native sound-source path. Host routing uses
world positions and one primary player per machine, not a region audio context.
New UNVERIFIED source converts proximity voice through the native listener
matrix before Halo/DirectSound playback, instead of passing world coordinates
to the canonical-origin mixer. Optional OpenXR builds derive a centered listener
from the actual eye cameras before their single sound pass, with native acoustic
location lookup and stale/invalid fallback. Multi-region cave/tunnel acoustics,
VR PTT ergonomics and actual Windows headset capture remain unverified/open.

Focused Forerunner source repair keeps an unavailable explicitly named microphone
closed instead of substituting the system mic. Mode/device changes close old
capture and discard buffered/encoded voice and encoder history. Existing settings
and codec/network framing are retained; hardware/privacy transitions are untested.

The combined foundation uses [AGPL-3.0-or-later](LICENSE), retaining
Chupathingy's [GPL-3.0-or-later notices](LICENSE.GPL-3.0). This is inherited work;
its authorship/history must remain intact. OpenCE, Faber, OpenH1, GulchCE, and the
PS4 repository carry AGPL notices at the inspected snapshots. Damnation's root
`LICENSE.md` is CC0; imported and third-party files still need individual review.

Read each candidate's agent/contribution guidance before adapting it. Keep
third-party versions, checksums, notices, fonts, and art credits. OpenH1's
`port/assets/hud/NOTICE.md` explicitly places Aerocatia/Jessie art outside its
engine license. Console SDK permissions do not grant rights to SDK/game assets.
No proprietary game data belongs in this repository or release packages.

Future integration uses source comparisons and small author-preserving commits.
No public release, push, or upstream submission is authorized by this ledger.


Graphics parity review, 15 November 2026: retain compatible options from every
reviewed fork using the existing setting/menu/render paths. Names alone do not
establish parity: Chupathingy's FOV defaults/units differ, while OpenCE/Fractum
display declarations already overlap the foundation. OpenH1 `baed7757` contains
actual Vulkan ray-query AO (`xgpu_vulkan_rt.c`), world SDF/object-proxy occlusion,
enhanced dynamic lights, per-pixel lighting, HDR controls and projection/view
options. Its ray-traced mode covers ambient occlusion; full path-traced global
illumination is not established. These paths are NOT imported merely by recording
them. Native GPU/device/shader ownership in the existing renderer needs an
approved extension; no second OpenH1 renderer is authorized by this review.

Faber shadow-budget reuse adapts SutlabSol/OpenCE-Faber commit
`d67d7922a96e9a24e16cbbe4015c3fe8abc1473f`,
`source/render/render_objects.c`, under AGPL-3.0-or-later. The original
screen-size ranking/fade behavior is retained in Forerunner's native shadow
path, with stock-off default, bounded history, map reset and one shared
per-window threshold per frame for stereo passes. Existing generated graphics
menus expose it. Focused/native evidence is in TEST_EVIDENCE.md; this is not
measured large-horde performance or physical VR acceptance.
