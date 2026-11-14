# Forerunner known limitations

Checkpoint date: 2026-11-14. This is an active engineering project, not a completed
unified-engine release. Refer to [TEST_EVIDENCE.md](TEST_EVIDENCE.md) for current
run outcomes and [UPSTREAM_FEATURE_MATRIX.md](UPSTREAM_FEATURE_MATRIX.md) for
source provenance.

## Current evidence limits

- The original OpenCE and combined Fractum Windows x86 candidates compiled with
  LLVM 23.1.5, installed MSVC 14.51 libraries, and Windows SDK 10.0.26100.0.
  Portable builds disabled PGO/LTO; the combined candidate also disabled FFmpeg
  for the baseline. These results do not certify other architectures or releases.
- The repaired baseline renders Blood Gulch through native OpenGL 4.5 on an
  NVIDIA RTX 4070 Ti and exits cleanly. Hidden-window capture and dummy audio
  do not prove audible sound, comfort, visible-window performance or human play.
- The latest focused regression suite passed 390 tests and skipped 87. Skipped tests
  are unverified; this is not the complete upstream suite. Fractum's agent guide
  also records that its CI excludes OpenCE Stagekit because its cache loader
  differs. That coverage gap remains open.
- Early mixed-peer failures exposed duplicate native layout fields, a loading
  transition admission race and unreliable test automation readiness. Those
  repairs now pass four movement sessions on two maps/both hosting directions.
  Bounded scripted damage/death/respawn sessions also pass. Some transient
  grenade/player comparisons differ within unchanged persistence tolerances;
  these results are not bit-identical simulation or full-match acceptance.
  Failed runs remain recorded in the evidence history.
- Current integrated-source compilation, restored Workshop parser tests,
  Windows comparison-runner checks, and subsequent peer runs must be read from
  their explicit evidence records. A commit or source review alone is not a pass.

## Unfinished acceptance requirements

| Requirement | Remaining gate |
| --- | --- |
| Classic campaign/multiplayer/co-op | Human rendered play, complete matches/campaign, map changes, reconnects, vehicles, checkpoints/reset, AI and cross-platform regression. Bounded automated movement/combat is covered. |
| 128-player sessions | Source capacity exists; full session load, synchronization, resource limits, and performance have not been demonstrated. |
| Firefight | Bounded B40 encounter completes three two-enemy waves with shared score/victory on two automated rendered clients. Shared lives/defeat and loaded actor/native spawn selection have later unbuilt, untested source. Full Faber HUD, individual scoring/medals, configurable multi-group waves/objectives, late join/reconnect, creator maps and human acceptance remain open. |
| Extraction | No native authoritative deploy/loot/objective/extract/persistence loop implemented. Death, failed extraction, and item ownership remain open. |
| Zombies | Existing multiplayer Infection source is inherited but not accepted through a completed match. AI Flood survival, barricades, scarcity, and optional extraction endings remain unimplemented. |
| Forge/Spark integration | Read-only versioned engine discovery is tested; Spark's new registration/discovery consumer and prepared-round export are source-only. Its OpenCE build and exact-hash shared-folder launch guards still need a reviewed Forerunner route. Manipulation, transactions, conflict detection, persistence, and Spark create/test/revise remain open. Faber's Windows hooks are not a cross-platform editing API. |
| VR | OpenH1 source reviewed; no integration or headset test. Its inspected Windows build disables VR. Linux tracking and mixed desktop/VR combat require real hardware evidence. |
| MegaWorld | No two-way player/vehicle traversal between connected environments demonstrated. Existing maps/BSP transitions do not establish a 19-map seamless world. |
| Product/release | Installers, clean-machine testing, accessibility, performance budgets, supported-platform packages, and human release review remain open. No public release is authorized. |

## Integration constraints

- Network version numbers alone are not compatibility proof. OpenCE and Fractum
  currently advertise 44; OpenH1 Legacy is 41 with a separate Standard agreement;
  GulchCE's base 44 still carries additional agreements. Preserve explicit
  profiles and reject unsupported feature combinations.
- Faber Firefight transmits native structures through `csmemcpy`; ABI, padding,
  endianness, and semantic validation need review before adapting it. Its
  fake-world harness does not prove real peer synchronization.
- Keep Xbox, PC/CE, HaloMD, and MCC loaders/resources separate. MCC loading is
  inherited source, not verified full gameplay fidelity. The inspected
  `HALO_64BIT` MCC path explicitly refuses maps whose pointer layout it cannot
  represent; wider address space alone is not a solution.
- Preserve stock 30 Hz simulation and deterministic math. Modern rendering,
  alternate substeps, and VR must not silently change a classic session's rules.
- Preserve all upstream licenses/authorship and per-asset notices. The engine
  license grants no rights in Halo data. Game data stays in authorized disposable
  test copies, outside tracked source; existing installations and Guilty Spark
  are not replacement/build-output locations.
