# Forerunner compatibility matrix

Reviewed 2026-11-14. A matching version number, a readable cache, and a working
multiplayer session are different claims. Current evidence is limited to the
builds/tests in [TEST_EVIDENCE.md](TEST_EVIDENCE.md). No universal cross-play or
legacy-export guarantee is made.

## Network agreements

| Runtime snapshot | Source agreement | Forerunner acceptance |
| --- | --- | --- |
| OpenCE `1519c65f` | `HALO_PORT_NETWORK_VERSION = 44` | Native movement passes on Blood Gulch and Beaver Creek in both hosting directions against repaired Forerunner. Bounded scripted combat also passes; wider gameplay remains open. |
| Fractum foundation `eae91445` | Announced version 44; join range 11–45; host minimum 42; explicit per-peer layouts and Delta handling | Candidate compiled and ran local player ticks. This source machinery does not establish every permitted version as gameplay-compatible. |
| Chupathingy `78d28a70` | Base version 32; accepted legacy range 11–45; separate Delta agreement/layout table | Inherited code and one additional parser fix; exact session combinations require testing. |
| Faber `e4ffe0fe` | Base version 41; Forge has a separate agreement | No accepted cross-play test with current Forerunner. Firefight/Forge not integrated. |
| OpenH1 dev `baed7757` | Legacy 41; Standard version 30 with separate advertised bit/base checks; optional 30/60/120 Hz substeps | Not integrated. Preserve classic 30 Hz profile; explicitly negotiate any future advanced profile. |
| GulchCE `66b4df0b` | Base version 44, plus distinct Standard/Third Person agreements and saved-state changes | Same numeric base as OpenCE is insufficient. Its current README requires matching GulchCE builds; no compatibility accepted here. |
| DamnationCE `d31736d3` | Actual source version 27, accepted 26–27 | Older protocol reference only. README's version-11 banner is stale. |
| Xbox retail, Halo PC/CE, MCC official runtimes | Their own original agreements | No direct network compatibility demonstrated or promised. Loading their assets does not reproduce their network protocols. |

The declarations are in each snapshot's
`port/linux/include/halo_port_limits.h`; Fractum's serializers are in
`port/linux/game/network_rules.c` and `network_distributed.c`. OpenH1's separate
profile is implemented in `network_standard.c`. Wire/state changes need explicit
agreement/version handling and regression tests. Unsupported combinations must
be rejected instead of silently losing gameplay state.

## Platform and execution modes

| Target | Source/build path retained or investigated | Local validation |
| --- | --- | --- |
| Windows x86 desktop | Inherited shared platform plus `port/windows`; clang/MSVC ABI, SDL3, OpenGL | Compiled, swapped-host native sessions and hidden OpenGL gameplay capture pass. Human play, audio and performance acceptance remain open. |
| Windows x64 desktop | Inherited `HALO_64BIT`; optional bgfx/Vulkan/D3D path | Not compiled or gameplay-tested here. Do not infer MCC/VR support from x86 success. |
| Linux desktop/dedicated server | Inherited native/guest/server builders | Not compiled or gameplay-tested on this Windows host; WSL was unavailable during baseline setup. |
| macOS | Inherited build/64-bit platform and optional bgfx/Metal | Source retained; no local build, hardware, or gameplay evidence. |
| Android and browser | Inherited ILP32 guest/host and both browser build paths | Source retained; no local platform or native/browser peer acceptance. |
| Linux OpenXR/Steam Frame VR | OpenH1 Linux VR builder and OpenXR host | Source reviewed only. Requires supported hardware/runtime and real tracked combat with desktop peers. |
| Windows VR | OpenH1 Windows builder explicitly sets `vr=False` | Unsupported by the inspected build path; Forerunner has no Windows VR implementation. |
| PS4 and other consoles | Optional external console adapters; PS4 uses x32 guest, OpenOrbis, Mesa Zink/RADV | Not built or validated. Never package user maps through default console packaging scripts. |

## Authoring/export capability by runtime target

Spark owns authoring and legacy conversion; Forerunner owns execution. This table
describes the current Forerunner integration, not any independent Spark exporter.
No new authoring/export workflow has passed an end-to-end acceptance test here.

| Target runtime | Existing format boundary to preserve | Current export/extended-feature status |
| --- | --- | --- |
| Xbox retail | Original Xbox tags/scenario/cache constraints | No Forerunner exporter or target gameplay validation. Do not insert extended native fields. |
| Halo PC retail | PC tags/caches and resource ownership | No Forerunner exporter or target validation. Native Firefight/Extraction/VR sidecars are not supported by this target. |
| Halo Custom Edition | CE tags/scenarios/cache and resource maps | Loader source retained. No new export accepted; OpenSauce-required maps are refused by the inherited policy. |
| MCC | MCC cache/package/resource formats, independent of Xbox/CE | Loader/adapter source retained, with documented limits. No new exporter or official-runtime gameplay acceptance. |
| Chupathingy | Its supported map families plus optional protocol features | No Forerunner export acceptance. Engine-specific sidecars need an explicitly implemented consumer. |
| OpenCE | Xbox/CE and supported MCC paths, version-specific capabilities | No Forerunner export acceptance. Stock supported data must remain independently usable. |
| Forerunner | Native formats, versioned Spark mode receipts, existing Forge transactions and capability discovery | Latest Win64 runs the released MegaWorld ground preview. Private Win64 LAN gameplay verified the full-world UNSC bank on fd7a49fa. Selected Forge and Firefight operations have separate native evidence. Full editor coverage, Covenant banking and the compact vehicle route remain unfinished. |

An extended configuration may only be offered for a runtime that advertises and
implements it. Unsupported exports must report the missing capability; they may
not silently discard objectives, invent proprietary Halo tag fields, or claim
that an ordinary map collection is a seamless world.
