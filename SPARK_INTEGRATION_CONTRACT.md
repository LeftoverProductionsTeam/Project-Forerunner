# Spark–Forerunner integration contract status

Version 1 discovery, 14 November 2026. Spark owns authoring; Forerunner owns
execution. Native read-only discovery is implemented and tested on the Windows
x86 desktop/headless executable. Live editing is not implemented yet.

Authoritative source: `D:\Project Forerunner`, local `master`. Spark is
`D:\Project Guilty Spark`, inspected read-only. This existing document is the
handoff; each team updates only its own source/checkpoint at meaningful milestones.
No separate coordination service or repeated cross-project polling/builds.

Review baseline: Forerunner gameplay `21a809b0`, shared-life source `c8196f81`,
repository reconciliation `3347a00c2`; Spark inspected read-only at
`3329a4a5ab12b6f02c75b82e054f75b0896f1e2a`, clean at inspection, including its
receipt-backed owned Forerunner solo staging source. Forerunner source was reviewed
at `7732e61dd` before the startup-profile continuation change. Spark's
source changes remain untouched. Forerunner changes after the accepted gameplay build
are source-only/unverified; source discovery metadata is not gameplay acceptance.

## Discovery and feature negotiation

Run the compiled engine with `--spark-capabilities 1`. It exits without loading
maps or starting a game and writes one JSON document plus a newline to stdout.
Diagnostics may appear on stderr. No settings, save, log or cache file is written.
Tests verify this with no game data, an existing configuration and `--server`
also present. Missing or unsupported contract versions exit **3** with a clear
stderr diagnostic and no JSON response; no downgrade is attempted.

The response has these fields:

| Field | Version 1 meaning |
| --- | --- |
| `schema` | `spark-forerunner.runtime-capabilities` |
| `contract_version` | Integer `1` |
| `engine` | `Forerunner` |
| `build` | Upstream version, exact build commit/snapshot identity, platform, architecture |
| `simulation_hz` | Classic simulation rate, currently 30 |
| `network` | Compiled advertised version and native-layout version; both currently 44 |
| `operations` | Currently only `runtime.discover` |
| `process_entrypoints` | Inherited CLI entrypoints listed below; not live-edit RPC operations |
| `startup_map` | Optional new unverified desktop CE startup metadata: setting/environment, `basename@ce`, cache version 609, solo scenario, native-menu profile creation, fresh start and no network hosting. Absent on the accepted old executable and non-desktop/server builds. Not a new operation or loaded-map acknowledgement. |
| `upstream_projects` | OpenCE, ChupathingyCE, FractumSeraph/OpenCE attribution |

Network numbers describe the compiled code, not universal peer compatibility or
an acceptance result. Read [test evidence](TEST_EVIDENCE.md) for actual runs.
An additive response field may be ignored by a version-1 client; changing the
meaning of an existing field or operation requires a new contract version.
Clients must refuse unknown versions and required operations absent from the list.

The reference client is [tools/spark_runtime.py](tools/spark_runtime.py):

```powershell
python tools/spark_runtime.py --engine '<compiled-engine-path>' --require runtime.discover
```

It first checks the runtime's `--help` advertisement, so an older engine that
ignores an unknown flag is refused before it could accidentally launch a game.
It validates the response schema/version and required operation names. Requiring
`world.object.create` currently fails explicitly; no object edit is attempted.
The separate Spark session can use this contract without depending on private C
structures. Standalone server/mobile/browser builds have not passed this adapter's
acceptance; do not infer their support from the desktop executable.

## Existing engine entrypoints

The inherited native runtime supports process launch and these command-line
entrypoints in source: `--version`, `--help`, `--config <file>`, `--server`, and
`--probe [invite]`. Local compilation has verified `--version` and the dedicated
server's missing-data rejection. Native headless local simulation is also verified.
These are existing engine interfaces, not a completed lifecycle or editing RPC API.
The current `--version` still reports the upstream engine identity.

Do not bind a Spark client to private C pointers, chat-command payloads or an
unversioned debug console. Do not infer feature negotiation or undo support from
the ability to launch a process.

## Actual Firefight integration boundary

| Operation / input | Current support |
| --- | --- |
| Contract `1`, `runtime.discover` | Implemented. Accepted Windows/x86 executable reports 30 Hz, network/native layout `44`, and only this operation. |
| Process launch / `--config <file>` | Existing CLI, not start/stop/connect RPC. Spark may own its launched process and private data/save folders. |
| `game.start_map` / `HALO_START_MAP` | New unbuilt/untested source: one-shot desktop selection of an installed `basename@ce` solo cache through the normal campaign path and current difficulty. With no engine player profile, retains the validated request while the user creates one in native menus, then starts in the same process. Skips loading an older saved checkpoint; ordinary human input and native HaloScript remain. Not hosting, a live RPC or JSON import. |
| `network.firefight_encounter` / `HALO_NET_FIREFIGHT_ENCOUNTER` | Opt-in host setting. New unverified menu source exposes OFF/ON under Server Setup → Co-op Options → Firefight, using existing setting save/cancel. Leave OFF for a Spark-authored wave controller. B40 proof: two players, nearby loaded Grunt, three two-enemy waves, shared score/notices and lobby return. Not a generic authored encounter adapter. |
| `network.firefight_lives` / `HALO_NET_FIREFIGHT_LIVES` | Unverified host source, 0–99, frozen at map start. Deaths spend available lives for player-bound respawns; the last paid respawn completes at zero. Failed native spawns retain grants; departed/reused player slots cannot transfer them. Respawn-only wipes preserve progress. A full world revert clears grants and restarts at wave one with zero score and the initial allowance; not saved mid-wave continuation. |
| `network.firefight_resupply` / `HALO_NET_FIREFIGHT_RESUPPLY` | New unverified host source, off by default and frozen at map start. Replenishes living players' carried weapon reserves/battery charge before waves two and three through existing inventory replication. Keeps native magazine/reload state and equipment. Finite director only; does not enable Spark's native HaloScript resupply option or import its JSON. |
| `network.firefight_actor`, `network.firefight_spawn_encounter` | New unverified host source: exact loaded actor-variant path and optional named native encounter's squad start in the current BSP. Retains faction/swarm attachment and final-death credit. Three waves of two initial bodies plus native carrier children, bounded to 32 tracked bodies per wave on a co-op scenario; not Spark's authored rules. |
| Spark `FirefightConfig` schema `1` | Authoring/preview only. **Forerunner has no importer for this JSON.** |
| `game.solo_skulls` / `HALO_SOLO_SKULLS` | New unverified, empty-by-default source reuses the existing 17 implemented skull hooks for local native solo content, including CE. Applied at map start/restart; cleared on disposal. Unsupported names warn and are skipped. No automatic mapping from Spark rules, network support, scoring multiplier or new discovery operation. MCC retains its separate existing setting. |
| Standard actor palettes, encounters/squads, player profiles, HaloScript | Existing native authoring/runtime boundary. Spark emits source; the inherited engine executes supported compiled caches. Specific generated maps still require gameplay acceptance. |
| Firefight combat/late-join status text | New unverified source sends changed host wave/phase, enemy/pending-spawn counts and score/lives during combat, at most once per second. A newly loaded client's reliable ready message gets an immediate summary and any terminal outcome. Existing transient host HUD/client chat notices only; not a persistent panel, query RPC, structured HUD state or full late-join gameplay acceptance. |
| Tracked Firefight AI kill credit | New unverified host source credits the last damaging player's existing native kill counter once on final death. AI/environmental/no-statistics damage clears individual attribution; departed/reused player handles are refused. Existing statistics transport only; no individual point totals, assists, medals or new discovery operation. |
| Live edits, runtime-state query, transactions/undo | Not implemented by this adapter. No private-memory/debug-console fallback. |
| Native console `forge_clone` | New unverified engine-only placement primitive, not a Spark operation. Duplicates supported loaded objects through existing native creation, local/host authority and console quotas. Map scripts cannot call it. Scenery and scaled copies require local play: native creation does not transmit either dynamic scenery or scale, so those host copies are refused. No source save/undo or editor transaction; Spark must not bind a write RPC to this console command. |

Accepted discovery identity: `7c65d0865c34-dirty.d990a0d29b19`, Windows/x86,
upstream version `0.9.1-dev`; exact executable/source hashes are in TEST_EVIDENCE.md.
Matching contract, cache version and peer network version are separate checks.
No version-1 discovery field grants gameplay, live editing or universal cross-play.

## Companion check-in: current source and the missing connection

Spark has implemented the first registration/discovery connection in existing
modules: `src/runtimeWorkbench.ts`, `src/components/RuntimeRegistration.tsx` and
`src-tauri/src/runtime_connections.rs`. Forerunner is an installed OpenCE-family
profile, not a guessed download or a new runtime manager. Its opt-in query uses
the saved executable SHA, checks `--help`, requests contract 1, bounds time/output,
validates the response and rechecks executable identity. Source inspection matches
Forerunner's current response fields. These Spark changes have not been built or
queried under the owner's no-tests instruction.

Spark also has source for equipped native solo rounds, per-group spawn-zone
assignments and full prepared-round source export through its existing QuickForge
pipeline. `src/components/Firefight.tsx::includeSourceAssets` now reuses the
existing source-import path for actor/weapon closures, retaining source SHA,
namespaced dependencies and stale-project/Save guards. The current native
scenario reader exposes actor palette, squad/start overrides and firing positions;
`firefight.rs::verify_native_round` checks the planned actor/count/state/positions.
`forge_build.rs` applies current readback to retained build reuse too. These are
unverified source improvements, not proof of extraction, compilation or gameplay.
Recent source-only changes are described in Spark's `docs/FIREFIGHT_ENGINE.md`;
its offline preview remains authoring feedback, never runtime AI authority.

Spark now has unverified source for preserving encounter appends using one actor
variant, in `crates/spark-core/src/native_operations.rs` and
`src-tauri/src/native_operations_native.rs`. It appends after existing encounter/
palette indices, checks dependencies and uses the existing preview/accept/source
recovery path. Its current worktree adds 1–32 authored starting positions and a
shared grid helper; this is one squad, not a multi-group wave controller.

The entrypoint still calls `runtime_workbench::compiled_tags`, whose
`source_context_for_build` requires `mcc-cea`. It also requires a solo scenario in
both compiled readback and working source. This is not an admitted CE append route
or a legitimate conversion of the original Demo multiplayer Zombie map. Existing
`runtime_workbench::collaboration_tags` shows edition-specific checks around
`owned_source_context`; reuse the relevant CE receipt/source checks in a scoped
append route instead of removing the general MCC tools guard.

Spark now has unverified source for reviewed controller-module attachment in
`crates/spark-core/src/working_tags.rs`, `src-tauri/src/runtime_workbench.rs` and
`src/components/RuntimeDock.tsx`. It retains original source-module inventory and
signatures, checks the reviewed attachment hash, refuses name collisions and
refuses existing script bytecode without recovered source modules. The UI and
working-tag admission still target MCC/H1A. This progress does not provide CE
recovered-source attachment or an accepted original-Zombie conversion. Do not
apply fresh-source index-zero commands to the recovered scenario, or treat an
actor append as a complete authored round.

The current uncommitted `src/haloScript.ts` and `src/components/Firefight.tsx`
also bind wave groups to exact source-cache encounter names, checking actor
variant, normal/insane counts, starting-location count and not-initially-created
state. Groups require distinct encounters and explicit ownership review; manual
or startup activation is selected deliberately. Bound controllers retain scenario
equipment. This reuses the existing exporter rather than creating an engine JSON
importer. Navigation, script interaction, visible score/outcome and gameplay are
not established by those source checks. All these changes remain untested.

Forerunner's useful companion role is execution of the resulting standard CE
content: native actors, physics, combat, saves/profile isolation and authoritative
co-op replication. Spark keeps conversion, encounter/script editing, source
identity and build readback. Reuse its authoring work; do not copy its preview
simulation or map writer into the engine. Native authored controllers still run
with `network.firefight_encounter=false`; the finite engine director is a separate
choice, not an automatic interpretation of Spark's broader rules.

Two concrete restrictions remain in Spark:

- `crates/spark-core/src/firefight.rs::native_round_plan` admits fresh CE,
  Chupathingy and MCC source; it refuses recovered/native documents and OpenCE
  documents. `forge_build.rs::authoring_preflight` also explicitly refuses OpenCE
  compilation. Registration/discovery does not make this a supported build route.
- `runtime.rs::shared_content_interface` now admits the recorded Forerunner
  executable through its exact-hash `forerunner` profile. New source in
  `src-tauri/src/runtime_rebuild.rs` stages validated self-contained CE609 solo
  receipts through the existing owned playtest registry: private executable/SDL,
  Xbox UI, exact cache, CE banks and isolated settings/saves/logs/cache. Launch
  clears inherited overrides and sets `HALO_NET_FIREFIGHT_ENCOUNTER=false`.
  This is untested source, and its admitted old binary still requires native menu
  selection. It deliberately does not send `HALO_START_MAP` or claim loaded-map
  confirmation, automatic revision reload, live editing or multiplayer acceptance.

The smallest connection uses a reviewed **CE content build target** and a
separately selected **Forerunner executable profile**. Reuse the existing compiler,
receipt and launch plan; do not enable every OpenCE build, remove recovery guards,
or create a JSON importer merely to connect these two choices.

Forerunner's existing source provides `paths.data` / `HALO_DATA_ROOT`,
`paths.saves` / `HALO_SAVE_ROOT`, and `paths.custom_maps` /
`HALO_CUSTOM_MAPS_ROOT`. Use private owned configuration/data/save folders with
`--config`; retain exact executable/cache/resource identities. Standard CE caches
go in `maps_ce` with their own required CE resource banks; required Xbox UI data
stays in `maps`. `map_families.c::map_family_list_campaigns` and
`menu_functions.c::custom_campaign_map` supply the existing CE solo-map chooser
and launch route. There is no advertised direct-map launch RPC. Documented path
settings are source facts, not acceptance of Spark's current launcher with this
executable. Do not bypass its identity checks through a debug console.

The new `game.start_map` setting closes the source-side menu-selection gap for
the initial solo gate. Spark can put `start_map = 'gs_firefight@ce'` in `[game]`
of its private `--config` file (or set `HALO_START_MAP`) after compiling/staging
that standard CE solo cache and its resource banks. Use private data/save roots,
an engine player profile created through its normal UI, and the existing build
receipt/hash checks; a scenario's equipment profile is not the engine player
profile. Invalid paths, truncated names, wrong-family/multiplayer/missing caches,
missing resources and competing session/test/server startup are refused without
leaving an existing session. An empty private profile folder now leaves the menus
available and retains the already validated map request. Creating a normal profile
continues that request in the same process, without repeated cache checks or log
spam. One successful selection per process; returning to menus does not replay it.
This setting has not been compiled or exercised and is absent from the previously
accepted executables. Discovery still advertises only `runtime.discover`.

The solo startup sets the existing local campaign connection. The finite director
only updates for a network co-op host, so it does not run on this path even if an
unrelated host preference was enabled. Spark should keep its explicit director-OFF
setting for clarity and any future co-op route. Native HaloScript owns encounter
placement and wave rules; no private `gs_ff_*` global reader or rules importer is
added. A native `game_won` on a custom solo cache has no Xbox campaign successor;
the existing main-menu path handles completion. Restart Level resets that same
loaded revision, not a newly written cache.

The authored solo source path already has profile guards:
`main_get_solo_level_from_name` rejects `@ce` names and
`player_profile_save_level_completed` refuses unknown/out-of-range campaign
levels. `main_reset_map_private` resets the loaded world; it is not a content
revision loader. Spark must stop only its owned process and launch a fresh one
against the newly reviewed cache for the revision gate. These are inspected
source paths, not new native acceptance.

Spark's `src/haloScript.ts::exportHaloCeFirefightRound` currently records team
wave-clear points and terminal states in `gs_ff_score` / `gs_ff_state`, then waits
for another start request. Forerunner does not interpret those names as a HUD,
session-result API or restart command. Spark still needs visible round/outcome
feedback and an explicit replay/exit policy using reviewed native content. A
one-player/one-life configuration check or a terminal global alone does not prove
that human gameplay flow. Keep these globals separate from negotiated runtime
operations; discovery remains read-only.

Forerunner's finite host director may select an original loaded Zombie variant
and native spawn location once compatible source exists. It remains separate
from Spark's `gs_firefight` HaloScript controller: select exactly one wave owner.
The director still requires a network co-op/solo scenario. New unverified source
allows one ready host to start; every present player must be alive/on foot and
the opening cinematic finished. This is separate from the ordinary solo
HaloScript route above. The original Demo multiplayer cache still does not
satisfy the director's scenario requirement.
Native final-death notification now has unverified source so a destroyed wave body
can be credited before removal. A subsequent unverified source increment adapts
Faber's Outbreak gate for tracked parents: native children join that wave's same
32-slot tracker, and a dead carrier's pending native spawn delays wave completion.
Partial, unsupported or over-capacity child creation aborts explicitly. This is
neither a converted Zombie map nor accepted carrier gameplay.
Later unverified source keeps dead tracked carriers off the native garbage list
until `unit_test_spawning` finishes, then restores normal garbage eligibility.
Native direct destruction and valid zero-child rolls retain their behavior;
this does not establish a repair for the historical Zombie physics crash.
Carrier child variants now also have an unverified prerequisite check adapted
from Faber: missing actor/unit references report the variant path and reject the
spawn before native placement; tracked Firefight requests abort. Optional empty
tag fields are not a runnable actor definition. No source tag is rewritten.
The finite terminal return also has an unverified ordering repair: its existing
same-arena co-op lobby path runs before MCC package or Xbox/CE campaign completion,
so a finite result does not enter campaign progress/next-mission handling.

## Reuse audit and ownership

Paths below are relative to Spark. No code was removed or moved.

| Useful existing code | Decision |
| --- | --- |
| `crates/spark-core/src/firefight.rs`: config/asset validation, `available_enemies`, `presets` | Keep in Spark; reuse its source-bound rules, actual Zombie actor paths and terminology instead of another editor model. |
| Same file: `native_round_plan`, `verify_native_round`; native controller generation | Keep source/profile/encounter/script emission and build readback in Spark. Forerunner must not grow a duplicate map compiler. |
| Same file: `Director`, `simulate`, `preview` | Offline authoring preview, not live gameplay. Virtual spawn/death commands must never become Forerunner's AI authority or acceptance evidence. |
| `legacy.rs`, `source_recovery.rs`, conversion/resource modules | Keep source-preserving conversion and reviewed recovery in Spark. Engine loading, actor lifetime, combat and replication repairs belong in Forerunner. |
| `runtime.rs`, `runtime_adapters.rs`, native launch/rebuild/observation modules | Keep editor process management and evidence-based capability UI in Spark. Reuse concrete native fault findings rather than rebuilding its observer. |

The inspected production modules do not implement native `actor_create_for_unit`,
`unit_control`, Firefight engine updates or OpenXR. Upstream source links are not
duplicate gameplay implementations. This audit warrants no runtime-code deletion
from Spark. Spark is MIT-licensed; preserve its notice for code actually reused,
independently of the engine's AGPL and content licenses.

## Required gameplay data and next shared gate

Use Spark's existing project/config and native build receipt, not a new manifest
service. The handoff needs:

- Original source/resource hashes and format; derived cache hash/version/scenario
  path; source revision and reviewed conversion deviations.
- Actual actor-variant paths/dependencies/factions, encounter/squad/group counts,
  spawn positions/facing/BSP/navigation and equipped player starting profiles.
- Finite wave/round rules, point values, lives/respawn and victory/defeat policy;
  selected executable identity and required capabilities. Reject unsupported rules.

These are requirements, not implemented runtime JSON fields. Spark's native solo
export currently permits one player/life, finite waves and a reviewed native
loadout, and only fresh QuickForge source. It refuses recovered/native documents.
Its simulation is broader than that native exporter; do not silently relax the guard.

The user selected **Zombie Apocalypse**, not replacement stock enemies. Available
Demo input: `D:\Project Guilty Spark\Runtimes\Zombie Apocalypse Halo Demo PC\maps\bloodgulch.map`,
SHA-256 `b38f35508188872b2535888193de80f766d92d3b8d53a8ada7be4fc9808a996d`.
It is version 6, not a Forerunner-specific cache. Use Spark's existing native CE
conversion path for a separate version-609 candidate; do not flip its header.
Earlier experimental recovery has missing original audio, reviewed placement/carrier
deviations and native spawner/physics faults. Preserve its credits and mechanics;
do not call that recovery faithful or gameplay-accepted.

Smallest intended end-to-end gate, still unimplemented/unverified:

1. Spark imports the exact Zombie source/resources and produces a separately
   identified CE candidate through its existing reviewed conversion workflow.
2. Extend its existing native-authoring path legitimately to the derived scenario:
   preserve the map, choose one reviewed zone, two finite waves of original Zombie
   actor variants and an equipped start. Do not replace the map with a stock arena.
3. Compile/read back the scenario, actor references, loadout and native controller.
   Start with the existing one-player source limit; two-peer authoring is a separate gate.
4. Forerunner executes that map. For native HaloScript-controlled waves, keep
   `network.firefight_encounter=false` so the B40 director does not replace or
   double-spawn the authored encounter.
5. Observe real combat/score/completion, revise a wave in Spark, rebuild and observe
   the change. Compiler/preview success is insufficient. Execution is deferred under
   the owner's current instruction to do code only, with tests later.

**Spark implements next:** owned receipt-backed solo staging now exists in source.
Keep its native menu route for the currently admitted executable. After an exact
new Forerunner build is reviewed and authorized for validation, admit that identity
and preserve the optional `startup_map` discovery object in the existing capability
parser (the current struct discards it). Only then send the receipt's exact
`basename@ce` through `HALO_START_MAP` or private `[game].start_map`; absence retains
the menu fallback. Keep private ownership and native director-OFF settings, and do
not treat startup metadata/logs as loaded-map or gameplay confirmation. Reuse conversion/native source generation
for the chosen Zombie candidate and support a reviewed derived-source encounter
handoff without dropping the existing recovery guards. Do not advertise live
editing, JSON import or multiplayer authoring as ready. For the chosen recovered
Zombie map, extend the existing Source Guardian/working-tag build path to append
reviewed encounters, actor references and the controller while preserving original
tables/scripts and dependencies. Reuse the new actor/weapon closure inclusion and
native layout readback. Extend the new preserving append operation through a
reviewed CE-specific source admission, multi-group authoring and the existing
controller-attachment implementation; its current MCC-only admission does not
close that gap. Keep native
HaloScript/encounter ownership separate from the finite host director: an authored
controller runs with `network.firefight_encounter=false`.
Include visible score/outcome feedback and deliberate restart/exit behavior in
that controller handoff; do not assume its private terminal globals drive engine
menus or the finite director's notices. Revised cache acceptance uses a fresh
owned process, not only Restart Level in the old one.
The first authored solo loop is a content/runtime handoff gate, not acceptance of
the synchronized Firefight milestone. Two-peer execution remains a separate gate.

**Forerunner owes:** the actual Zombie actor/spawner lifecycle, relevant Faber
HUD/lives/configuration integration and safe loading of the resulting content.
Actor/spawn selection and carrier-child tracking now have unverified source beyond
the Grunt default, but the co-op/solo restriction, carrier gameplay acceptance,
full wave composition and missing rules
importer remain open. Spark need not duplicate engine simulation to hide them.
New structured multiplayer data must
use negotiated Delta support, not raw native-struct packets or silent protocol-44 changes.

## Adapter requirements retained for implementation

Discovery is the first adapter operation. Explicit supported protocol profiles,
compiled rendering modes, current-session authority and gameplay-tested capability
must remain separate facts when those fields are added; they are not inferred
from the current numbers or process entrypoints.

Subsequent operations require session identity, host authority, request/transaction
identity, expected object or configuration revision, bounded validated payloads,
structured errors and observable completion. Undo/redo must apply complete
transactions and reject stale revisions. Object handles must survive safe
inspection without exposing raw addresses; deleted or reset objects invalidate
their prior handles.

Required groups remain: lifecycle/connect; object inspect/create/transform/delete;
AI encounters; Firefight and Extraction configuration; gametype markers/rules;
supported environment parameters; diagnostics/performance; preview/validate/save/
export; conflict detection and undo/redo. None is silently declared implemented.

## Native formats and extended data

Preserve supported native scenario/tag formats. Extended runtime data belongs in
explicit versioned sidecars, with target runtime, schema version, map/content
identity and required capabilities. A runtime must reject an unknown required
feature before modifying a session. No proprietary Halo tag extension or legacy
conversion is introduced by this checkpoint.

Community compatibility lead (14 November): Mota recommends Watanoge's existing
Forge/Firefight work and reports a newer map-preserving streaming-tag/text
placement sidecar. The inspected Watanoge snapshot is pinned in
[the upstream matrix](UPSTREAM_FEATURE_MATRIX.md); its tag-pack and CE-cache
trailer paths do not establish that newer sidecar's exact format. No such
sidecar reader, Spark write operation or cross-fork session compatibility is
currently advertised by Forerunner. Reuse the actual upstream format when its
source is available rather than inventing a competing schema. A placement file
does not supply missing client gameplay tags: every participating peer must
resolve compatible content and dependencies before spawning, including late
joiners. Existing visual-only loose-tag mods do not meet that requirement.
Keep proprietary base MCC data local; no asset-sharing capability is implied.

Legacy target conversion remains Spark's responsibility. The
[compatibility matrix](COMPATIBILITY_MATRIX.md) records the evidence limits for
Xbox, PC, CE, MCC and the community runtimes; unsupported gameplay is not a valid
export target merely because a file can be written.

Live-edit transactions remain a later gate. The immediate shared target is the
authored native Firefight loop above, without a generic RPC framework.
