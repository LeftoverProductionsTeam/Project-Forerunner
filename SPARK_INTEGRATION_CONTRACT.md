# Spark–Forerunner integration contract status

Version 1 discovery, 14 November 2026. Spark owns authoring; Forerunner owns
execution. Native read-only discovery is implemented and tested on the Windows
x86 desktop/headless executable. Live editing is not implemented yet.

Authoritative source: `D:\Project Forerunner`, local `master`. Spark is
`D:\Project Guilty Spark`, inspected read-only. This existing document is the
handoff; each team updates only its own source/checkpoint at meaningful milestones.
No separate coordination service or repeated cross-project polling/builds.

Windows64 integration milestone: the enabled OpenXR/bgfx Vulkan executable compiles
and passes the existing contract-1 reference discovery client. A private native
Vulkan/x86-peer comparison and missing-loader desktop fallback pass; headset
behavior remains unverified. `source/hs/hs.c::main_set_map_name_evaluate` now decodes
the native string address on x64 using the same `xbox_pointer` boundary as other
string evaluators. Existing script/console ownership remains unchanged. No schema,
editing operation, native-map authoring requirement or protocol version changed.
Spark can consume the x64 discovery response; this is not VR/editor-map acceptance.
Exact source/binary identity and sparse-sample rejection are in `TEST_EVIDENCE.md`.

Current native gameplay checkpoint, 15 November: two real Delta accounts complete
the owned CE fixture's combat/escape/bank/process-restart/redeployment path, with
matching final equipment on both clients. One host/five independent clients also
pass bounded player/AI/item encounter checks. Spark's finished map, independent
squad objectives and broad recovery acceptance remain open; see `TEST_EVIDENCE.md`.

Earlier verification baseline: the owner's Development & Verification Directive
authorizes focused compilation/tests and isolated native smoke checks. Historical
source-only entries below retain their original evidence limits. On base
`87521711b` plus tracked repairs, Windows x86 compiles; 58 focused inventory/
campaign/co-op checks and one private two-peer Blood Gulch combat/death smoke
pass. See `TEST_EVIDENCE.md` for source identity, tolerances and observed drift.
This does not validate Spark's authored candidate, persistence, VR or Linux.

`port/linux/game/network_objects.c::{network_objects_handle_inventories,
distributed_client_forget_retried_ownership}` now prevents a deferred old unit
inventory from reclaiming a weapon transferred by a newer host update. Existing
authoritative host refresh repairs its remainder. No packet, protocol version,
capability, authoring format or personal-inventory file changed. Spark must
continue providing native pickups/encounters and the existing Extraction controls/
volumes; it must not implement a competing inventory authority or award a stash.
Forerunner now implements durable owner/item/run identity, reservations and
settlement, with the bounded native evidence below; broad recovery acceptance
remains open. Spark's next handoff remains a compiled native Zombie Apocalypse/solo
encounter with its required resources and valid hub starts/Extraction markers;
no editor rewrite or new launcher/interface is requested by this repair.

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
| `network.firefight_encounter` / `HALO_NET_FIREFIGHT_ENCOUNTER` | Opt-in host setting. Unverified menu source exposes OFF/ON under Server Setup → Co-op Options → Firefight. Leave OFF for authored wave controllers. New source also yields when compiled `gs_firefight`, `gs_ff_reset` or `gs_ff_bootstrap` is present; that prevents a second finite director without pausing general HaloScript. Recognition is not validation, generic controller discovery or native co-op acceptance. Historical B40 evidence predates this repair. |
| `network.firefight_lives` / `HALO_NET_FIREFIGHT_LIVES` | Unverified host source, 0–99, frozen at map start. Deaths spend available lives for player-bound respawns; the last paid respawn completes at zero. Failed native spawns retain grants; departed/reused player slots cannot transfer them. Respawn-only wipes preserve progress. A full world revert clears grants and restarts at wave one with zero score and the initial allowance; not saved mid-wave continuation. |
| `network.firefight_resupply` / `HALO_NET_FIREFIGHT_RESUPPLY` | New unverified host source, off by default and frozen at map start. Replenishes living players' carried weapon reserves/battery charge before waves two and three through existing inventory replication. Keeps native magazine/reload state and equipment. Finite director only; does not enable Spark's native HaloScript resupply option or import its JSON. |
| `network.firefight_actor`, `network.firefight_spawn_encounter` | New unverified host source: exact loaded actor-variant path and optional named native encounter's squad start in the current BSP. Retains faction/swarm attachment and final-death credit. Three waves of two initial bodies plus native carrier children, bounded to 32 tracked bodies per wave on a co-op scenario; not Spark's authored rules. |
| Spark `FirefightConfig` schema `1` | Authoring/preview only. **Forerunner has no importer for this JSON.** |
| `game.solo_skulls` / `HALO_SOLO_SKULLS` | New unverified, empty-by-default source reuses the existing 17 implemented skull hooks for local native solo content, including CE. Applied at map start/restart; cleared on disposal. Unsupported names warn and are skipped. No automatic mapping from Spark rules, network support, scoring multiplier or new discovery operation. MCC retains its separate existing setting. |
| Standard actor palettes, encounters/squads, player profiles, HaloScript | Existing native authoring/runtime boundary. Spark emits source; the inherited engine executes supported compiled caches. Specific generated maps still require gameplay acceptance. |
| `network.coop_script_adaptations` / `HALO_NET_COOP_SCRIPT_ADAPTATIONS` | Host setting, default true for legacy campaigns, frozen at map start through `network_coop_new_game` / `coop_scripts_new_game`. Spark may set false in its private launch configuration for authored operations: native player lists/ALL gates, visibility recipients, individual transport/equipment actions and script save safety retain their authored semantics. Host script execution, existing replication, explicit native teleports and empty-player wait protection remain. Source: `port/linux/game/coop_scripts.c::coop_scripts_adaptations_active`, existing `source/hs/hs_library_external.c` callers, `port/linux/src/port_config.c`. Connected source checks and Windows64 compilation pass; specific authored mission/squad acceptance remains unverified. No new RPC, quest executor, squad membership or discovery capability. |
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

## Timerless Extraction and connected MegaWorld requirements

Owner-added flagship milestones complement Firefight and VR. Extraction must
complete one playable map with original Marathon-inspired terminal/hub UX,
inventory/loadouts, deploy, AI/optional PvPvE, loot, interaction-based extraction,
escape, loss, persistent rewards, report and redeployment. No mandatory raid
timer; an activation countdown is permitted. Spark owns the map/source/UX
authoring; Forerunner owns authoritative gameplay and durable result settlement.

`game.extraction` remains off by default and uses the existing native solo-map
path, locally or on the native co-op host. The existing Server Setup -> Co-op
Options -> GAMEPLAY selector offers CLASSIC, FIREFIGHT and EXTRACTION.
`tools/port_settings.py::COOP_SETUP_SCREENS` generates the existing menu asset;
`menu_functions.c::{gametype_option_setting_index,gametype_option_setting_write}`
read/apply `game.extraction` and `network.firefight_encounter` exclusively through
the existing settings writer. Classic keeps native authored encounters, including
authored Firefight, without the separate wave director. Native offline save,
reopen, Cancel and mode transitions pass; Windows64 and 14 focused cases pass
(`build/native-operations-mode-menu/result.json`). No capability, launch handoff,
wire format or Spark authoring interface changed. Bounded native transport and
earlier on-foot mission acceptance are recorded below; complete combined
Spark-authored aircraft/mission gameplay remains UNVERIFIED.

Optional squad hub binding: native controls named `extraction_squad_01`,
`extraction_squad_02`, etc. select transient host-owned membership in existing
Extraction runs (`extraction.c::extraction_squad_choose`). The bound is
`EXTRACTION_SQUADS`, derived from the runtime's existing player capacity, with at
most six connected members per squad. This is not 128-player acceptance. Each
choice must uniquely resolve to a live touched-by-player control with usable
native power/position groups, distinct from the other hub controls and transport
output groups. Put it on reachable ground in the return hub, clear of aircraft
and cabinets. Duplicate/missing/aliased bindings refuse hub admission.
When any such choice is authored, players must join a compatible faction squad
before deployment; maps with none retain the existing ungrouped path. Normal use
of one's selected terminal leaves the squad when its operation is finished.
Selection consumes the action without toggling a shared device group. Existing
verified identities and gear locks remain mandatory; membership does not transfer
stash ownership. Active operations freeze existing members' squad/faction changes
and repeated deployment. A returning squad can redeploy independently of another.

`forerunner_squad_01_complete`, `forerunner_squad_02_complete`, etc. are optional
native void callbacks after that operation's living survivors physically return
and finish durable banking. Other squads and hub members who have not deployed
in that operation are excluded. Pending banking/death blocks completion; zero
survivors never produces success. Reuse a continuous script beginning with
`(sleep -1)` for later operations, as in the existing
`tools/fixtures/extraction_factions_native.hsc`. These callbacks never award gear.
`extraction_update` and `extraction_players_returned_for_squad` own the gate.
The two-account native fixture proves separate selection, callbacks, banking and
same-ID redeployment (`build/native-delta-squad-settlement/result.json`). It does
not add squad objective queries, alliances, scoped presentation, multiple aircraft
per faction or reconnect-persistent squad membership. Existing discovery version
1 and legacy clients do not imply those capabilities; maps requiring this binding
must target a runtime containing these helpers. No new packet/save format or
editor subsystem is requested. Spark's next full mission handoff remains below.

A candidate needs
live scenario **control** objects named `extraction_deploy` and
`extraction_activate` (native use, `triggers_when = 0`) and trigger volumes named
`extraction_zone` and `extraction_return`. Controls must be powered, usable and
reachable. `extraction.c::extraction_control_touched` rechecks native power,
usability and one-use locks on the host before reserving gear or activating.
Accepted deployment and activation continue through
`device_controls.c::control_touched` / `control_toggle`, so HaloScript can observe
their authored device-group transition and native effects. Refused actions and
stash management do not toggle the controls. A deployment signal means its native
loadout grant succeeded; an activation signal starts the cancellable countdown,
not a completed escape or banked reward. Keep rearming/toggle behavior in the
authored control groups/scripts. Source and focused native-function checks cover
this handoff; acceptance on Spark's authored map remains unverified.
An optional native `(script dormant forerunner_extraction_complete ...)` returning
`void` now receives a one-shot wake after every living, owned survivor is back in
the return volume and has finished durable gear settlement. Pending native death,
incapacitation, field players and incomplete banking prevent notification. This
reuses `extraction.c::extraction_players_returned` and `hs_wake_by_name`; disabled
scripts remain disabled. It is not a reward writer, repeatable event, per-squad
quest API or proof that authored objectives were completed. Spark must keep the
activation control unpowered/unusable until its objective conditions permit
escape. Its dormant wrapper may then call its own `gs_complete_7`. No hardcoded
Spark objective index, arbitrary console call, new packet or save format is used.
Source and bounded native fixture callback/banking checks pass; complete
Spark-authored operation acceptance remains UNVERIFIED.
Put the deploy control inside the safe hub's return volume, outside
the activation zone, and the extraction control inside the activation zone.
Put every native player starting location selectable by this solo map inside
the safe hub's return volume and outside the activation zone. Supply the normal
native starting-equipment profiles; Forerunner does not invent a replacement
loadout or inject missing tags. `players.c::players_respawn_at_start` shares
native player creation/equipment with checkpoint respawn but skips checkpoint
and teammate relocation. `players.c::players_respawn_network_coop` uses those hub
starts for individual deaths. The first-life late-join path now also retains those
starts and skips campaign teammate relocation, leaving live field players alone.
An admitted hub does not need room beside a field teammate to admit a new player;
the level-opening/cinematic holds still apply.
`extraction.c::extraction_respawn_after_loss`, called from
`main.c::main_lost_map_private`, settles failed deployed reservations and
uses that path after a real team wipe, without world/save rewind.
Allocation failure retries through the existing native loss timer. Scripted
failure with live players retains campaign handling. Finite Firefight keeps
its existing death owner; combined finite-director outcomes remain unfinished.
The owned two-account native fixture verifies hub placement on live reconnect;
Spark's actual map, other platforms and broader Extraction acceptance remain open.
Spark must supply and
eventually validate its native starts, collision clearance and equipment.
Native use begins a run or a cancellable ten-second signal activation. Leaving
cancels it; death/replaced player handles, a different local saved profile or
any checkpoint/disk/core restore invalidate the run. The restore callback also
disables banking until a fresh map, even when native load paths preserve the
clock: restored objects must not recover already banked gear. After activation the
same live player must physically reach the return
volume outside the activation zone. Keep a real bidirectional player/vehicle
route and native safe-hub geometry; the engine adds no teleport or map switch.
Arrival must successfully capture actually owned native equipment before a
return can complete; failure leaves it pending without publishing a result.
A valid return persists escape escrow before consuming the exact native weapons
and grenades, banks it once, and shows a post-run summary. Use the deploy control
again to reserve a compatible native-cache loadout before restoring its original
ammo/battery quantities. Native
personal debrief is optional: author a unique touched-by-player control named
`extraction_debrief` in the safe hub, with usable native approach/aim geometry for
each supported biped. Keep its power/position groups separate from transport
outputs and its object distinct from the other Extraction controls. Ordinary
Use pages through the owner's completed-run summary, recovered/lost loadout and
native personal kills/deaths. The first summary appears after durable settlement,
on foot at the hub and outside cinematics. Pending writes never claim recovery;
reading a report neither grants nor consumes equipment. Detailed run pages last
for the session; restart fallback shows only the persisted outcome/current stash,
without inventing previous-run item history. Maps omitting the control retain
their existing notices. Older runtimes do not implement this optional name.
Source: `extraction.c::{extraction_bind,extraction_control_touched,
extraction_report_stage,extraction_debrief,extraction_update_run}`. No new save,
network packet, launcher, asset format or discovery operation.
Windows64 compilation, 12 connected report cases, three defect controls and
49 transport regressions pass. `build/native-delta-debrief-complete` verifies two
real bound owners reading native recovery/loss pages, unchanged gear while
reading, same-item redeployment and one casualty without altering the other
stash. Player/item comparisons pass (275 seconds/279 snapshots). The discovered
plasma battery/placeholder-magazine display correction is compiled/automated;
its final visual recheck, full aircraft loop and platform parity remain open.

Native
campaign `game_won` calls do not leave an admitted Extraction map:
`extraction.c::extraction_owns_map` gates both `main.c::main_won_map` and its
deferred transition. This includes wins scheduled by first-tick scripts before
control binding. Unrelated HaloScript updates remain enabled; missing/invalid
bindings and ordinary campaign maps retain native completion. This is
not complete gameplay acceptance: rarity/upgrades, armor/equipment, full hub UI
and native multiplayer Extraction validation remain unfinished. There is no new
discovery capability. Spark must not
advertise playable Extraction from these bindings alone. Retain native AI,
collision, encounter and equipment source; the engine does not import authoring
JSON or run a second Firefight director for native scripted encounters.

Spark source inspected read-only at clean local master
`73c1041439a92ae623a70081ba158cb366bf82f6`: `megaworld.rs` identifies regions as
`<cache_sha256>:<bsp_index>` and retains source bounds, translation and yaw;
`MEGAWORLD_SEAM_ROUTES_20261108.md` uses the existing assembly/native compiler
for tunnel, vehicle and ridge routes. Preserve those identities and transforms;
do not define another editor/layout format. The first handoff needs two owned
regions in one connected compiled scenario, source/compiled collision and portal
identities, a continuous common coordinate frame and independently usable
forward/reverse player and Warthog routes. Include their real materials, native
dependencies and explicit region-to-source mapping, without reducing source
geometry or substituting teleports. No proprietary map data is to be distributed.

Forerunner's current `scenario_switch_structure_bsp` unloads the old BSP and
reconnects global collision, objects, AI, renderer and audio state. It is not
multi-region streaming or simultaneous residency. OpenCE main inspected at
`1519c65facbb068e44b956da2769198a83a17847` is already inherited; community
multi-BSP reports do not prove a newer implementation here. Two joined regions
within current limits are the first native traversal candidate, not proof of
19-region capacity. Concurrent residency, bounded resource loading/unloading,
cross-region authority/replication, persistent world state and portal-aware
render/audio contexts remain engine requirements. Keep all limits and original
installations intact; no stock Xbox support or seamless BSP switching is claimed.

Development order remains two-region traversal, state preservation, bounded
multi-region residency, all 19 regions, then timerless Extraction across the
finished world. No gameplay acceptance exists for these new stages.

Both flagship modes also require one engine-owned persistent gear foundation:
weapon/ammo/armor/equipment loot, rarity/upgrades/capacity, stash/loadout choice,
authoritative pickup and loss, and crash-safe duplicate-resistant settlement.
Firefight may bank at defined milestones or evacuation; Extraction banks only
after successful escape and has no mandatory raid timer. Compatible recovered
equipment can cross modes. Spark authors discoverable loot and mode rules using
supported native tags; it must not write player stashes, invent awarded gear or
advertise a persistence endpoint that Forerunner has not implemented. Extraction
now consumes the shared weapon/grenade bank below. Finite built-in Survival can
opt into that same bank at its actual victory milestone; native authored Survival
can use the existing hub/evacuation controls with the built-in director OFF.
Endless built-in evacuation and full Zombie progression remain unfinished.

The shared native gear reader in `game_modes.c` now feeds both Extraction return
reports and Firefight wave-clear reports before optional resupply. It reads real
host/local ownership, tag paths, slot selection, loaded/reserve magazines, battery
age and grenades; it refuses duplicate object references and excludes objective
weapons. Map name/checksum are provenance, not cross-cache content admission.
These snapshots are not a sidecar, disk format or client claim.

Persistence reuse review: pinned Faber `e4ffe0fe` has per-profile statistics in
`port/linux/game/firefight_stats.c::{get_active_profile_index, save_profile_records}`.
Its `write_xbox_text_file` uses an in-place Win32 `CREATE_ALWAYS` write; the record
tracks stats, not owned item reservations/settlement. Reuse the existing native
saved-profile boundary, not that writer as a crash-safe cross-platform stash.
Forerunner binds an offline/local-profile run to `player_ui_get_active_player_profile_index`
and the native enclosing directory returned by
`player_ui_get_path_to_local_player_profile_directory`. The index is an
enumeration slot (`saved_game_files.c`), so `extraction.c` also compares that
directory before publishing a return. Deployment needs an available saved
profile; losing/changing that identity settles the deployed run as lost. Remote
players and the local host of a Delta session require the inherited
`delta_peer_game_player_identity(machine,controller)`
with verified host-bound proof (`identity.bound`), not player names or self-reported
account claims. It already exists in `delta_peer_game.c`; the declaration is now
shared through `delta_peer.h`. The host now uses its verified account too, so
hosting versus joining does not select different account stashes. A missing or
unbound host proof cannot fall back to profile storage. The local host uses its
native `network_player_data.machine_index`: `distributed_player_machine` deliberately
maps remote clients only. Two distinct verified accounts now complete native
regression-fixture banking and process-restart reservation of the same equipment;
exact readback and remaining acceptance limits are in `TEST_EVIDENCE.md`.
Local profile and verified account owner domains
are distinct; this does not promise portable client-managed or cross-host stashes.
`xbox_files.c::CreateFileA` currently ignores `share_mode`; Faber's sharing-zero
open therefore supplies no process exclusivity here. `port_config.c` now checks
flush failure before its existing atomic replacement, but settings replacement
does not provide item reservations or competing-runtime ownership.

The owner approved native gear records and locking. `gear.c`/`gear.h` now provide
one bounded version-1 binary record per owner under the existing private save
folder (`private/gear/<owner-hash>.gear`), a stable separate OS lock, explicit
little-endian codec and SHA-256 integrity check. `posix_files.c` and
`win32_files.c` flush staging data and atomically replace the record; linked or
nonregular staging files are refused before truncation. No database, second
inventory network protocol, profile-path export or personal save migration.
`gear_reserve` now saves unissued phase 3 before native application;
`gear_confirm_deployment` records phase 1 after the grant, before any tick/input
can use it. Failed confirmation reuses `game_modes_inventory_revoke` to remove
only that grant's objects/grenades before refund. An unissued preparation refunds
on reopen even after an outage survives disconnect; a confirmed deployment
retains death/disconnect/abandonment loss. The version-1 layout and existing phase
0/1/2 values are unchanged; older readers reject phase 3 safely, so pending
preparations require the updated runtime. Legacy deployed records retain their
old loss policy. Spark never edits these private records. Saved escape escrow
rolls forward once on reopen;
published-but-uncertain writes retry the same intent rather than minting rewards.
These are implementation/automated checks, not power-loss or gameplay acceptance.

Native `unit_kill`, `unit_kill_silent` and `unit_kill_no_statistics` requests
block new equipment grants, recovery settlement and medical treatment until
Halo resolves them. Spark can keep using these native calls; operation helpers
cannot turn their requests into a living casualty or a successful gear return.
Death and eligible drops still belong to Halo, including
survivor pickup through native ownership. Bounded desktop co-op demonstrates
forced-death treatment interruption and replicated drops; authored death-loot
banking remains unverified. Source: `extraction.c::extraction_unit_pending_death`,
`extraction_update_run`, `extraction_medical_update` and
`game_modes.c::game_modes_inventory_capture`. No new authoring interface.

`game_modes.c::{game_modes_inventory_grenade_room,game_modes_inventory_weapon_compatible,game_modes_inventory_loadout_valid,
game_modes_inventory_apply,game_modes_inventory_consume}` reuse native tag lookup,
object creation, weapon drop/attach/delete and actual owned snapshots. Old carried
weapons are dropped as loot, not erased. Capacity is 32 stored weapons and two
selected deployment weapons; grenades withdraw only into native capacity.
Exact source cache name/checksum remains the default admission policy. Optional
host `game.shared_gear=true` / `HALO_SHARED_GEAR=true` lets
`gear.c::gear_item_available` select stored weapons from other maps or rebuilt
caches through the same native `game_modes_inventory_weapon_compatible` check.
Spark must compile the matching weapon paths into the target map with magazine
limits that accept the exact saved ammunition; absent tags, native objective
flags or incompatible quantities leave items safely stored. No tag import, save
version change, editor inventory format or new discovery operation. The current
target definition owns weapon behavior: matching paths do not prove identical
mod assets, damage, effects or upgrade semantics. Compatible choice, discard,
reserve/refund and recovered identity use the existing owner bank. Connected
source checks and Windows64 compilation pass; actual authored cross-map
deployment/recovery/Survival acceptance remains unverified. Rarity, armor,
equipment upgrades and arbitrary cross-cache asset equivalence remain open.

Spark may optionally add a powered touched-by-player `extraction_loadout` control
inside the return hub, outside activation. It cycles compatible stored weapons
through native player notices. The host saves the choice by ordering only
available items through `gear.c::gear_cycle_loadout` and its existing locked,
atomic version-1 record: the exact-cache subsequence by default, or the native-
compatible subsequence when shared gear is enabled. Reopening and Survival reuse the choice;
failed grants restore it and returned carried gear stays ready for redeployment.
An uncertain published choice is flushed on retry rather than cycled again.
The same optional control now supports bounded stash management: crouch-use
previews the chosen stored weapon and ammo, then release/use again after one
second within ten seconds to discard exactly that item. Stand-use cancels a
preview. A previously confirmed, published save is finished on retry, never
presented as undone. Identity/body/life/hub changes invalidate previews. This
uses `gear_loadout_choice` / `gear_discard` and the existing atomic owner record;
it awards nothing, changes no grenades and does not touch deployed reservations.
Spark needs no new marker/schema or editor subsystem for this interaction.
`extraction.c::extraction_control_touched` / `extraction_loadout_notice` own the
terminal interaction and error/readback. Existing four-marker maps use the saved
compatible order, or the first pair for a new record, without requiring that
optional control. No new cache/schema field, inventory packet or Spark discovery
capability was added. Focused source compilation/workflows pass; native gameplay
acceptance remains open. Spark must include its native
`triggers_when` readback alongside deploy/activate, preserve all hub starts and
hand off a compiled native solo cache plus required resource caches. Do not import
personal stash records, synthesize grants, require a raid timer or run the built-in
Firefight director over authored encounters.

## Current code-reuse and ownership checkpoint

Shared-equipment follow-up on `c6f9255c2`: native optional Firefight resupply
(`coop_enemies.c::firefight_resupply_players`) now requires actual weapon-parent
ownership, matching `game_modes.c::game_modes_inventory_capture` rather than
trusting a stale slot and generic attachment flag. Six focused checks, Windows
x86 incremental compilation and a private two-peer B40 one-wave/resupply smoke
pass; the stale-transfer path itself has fixture evidence only. No Spark schema,
native script, packet, authoring operation or persistent inventory file changed.
Authored encounters still own their waves and native pickups; Spark must not
enable this built-in director alongside its own. Durable inventory ownership/
settlement and the compiled Zombie Apocalypse candidate remain open handoffs.

Spark inspected read-only at `d56354bee4813ea3a311afe0ac48a9b378505c3a`, with
unfinished `World.tsx`, `GpuLightmapDraft.tsx` and `gpuLightmap.ts` work present
and left untouched. Its MIT license remains its own; no Rust/editor source was
copied into Forerunner or linked across repositories. The responsibility review
uses the already pinned OpenCE/Chupathingy foundation, Faber `e4ffe0fe`,
OpenH1 `baed7757`, and Mota's initially inspected `2467e708` sidecar source
(refreshed to `91099ce3` below) in the upstream matrix.

| Responsibility | Existing implementation and handoff |
| --- | --- |
| Native Firefight | Spark `crates/spark-core/src/firefight.rs::{native_round_plan, verify_native_round}` authors/checks encounters, profiles and the three `gs_*` scripts above. Forerunner's existing `source/hs/hs.c::hs_update` executes native scripts; `source/game/game.c::game_tick` no longer suppresses them during finite Firefight. `coop_enemies.c::firefight_update` disables its optional director once it finds any of those compiled bindings. The finite director's terminal-result gate prevents unrelated campaign wins from skipping its waves/result notice; native-script-owned rounds retain normal mission results. Other controllers still require the finite setting OFF. |
| Loadouts and shared equipment | Spark `firefight.rs::{native_player_profile_arguments, verify_native_player_profile}` emits/checks standard native starting profiles. Forerunner uses native inventories and `game_modes.c::game_modes_inventory_capture` for both mode reports. Reuse that boundary; do not import Spark's preview director or turn a starting profile into a persistent stash. Durable owner-bound reservations/settlement, rarity/upgrades and full inventory UI remain open. |
| Extraction | Forerunner `extraction.c` owns per-player host/local control/volume state and consumes `gear.c` reservations/settlement through native equipment helpers. Spark authors the four required names through `src/nativeLogic.ts::extractionLayout` and `crates/spark-core/src/native_logic.rs::{scenario_commands,compile}`. Optional `extraction_loadout` cycles a compatible pair. Supply all hub starts, powered touched-by-player terminals, continuous physical route and compiled cache handoff; no editor or launcher is duplicated. Native pickups/AI and inherited co-op replication remain owners. Compiled/automated source behavior is not native multiplayer Extraction acceptance. |
| MegaWorld and baked content | Spark `megaworld.rs`/`megaworld_build.rs` own source identities, assembly and native output. The earlier inspected `gpuLightmap.ts::bakeGpuLightmapDraft` produces a direct-light draft, not native lightmap tags; that historical finding is not current native-lightmap acceptance. Forerunner keeps its native BSP/cache/collision/renderer/resource paths; simultaneous bounded multi-region residency is unfinished. A preview/GPU draft is not a new accepted engine format or proof of 19-region traversal. |
| VR, voice and Forge | Continue the existing OpenXR/bgfx renderer adapter using pinned OpenH1 transforms/interactions, the inherited Delta/Opus/Halo sound path, and compatible Faber/Mota placement behavior. Do not remove OpenCE browser/guest voice paths as duplicates of native Delta voice: they still have distinct callers/build gates. No second renderer, VOIP stack, sidecar format or cross-repository build dependency was added. |

The existing network co-op host director in `coop_enemies.c::firefight_update`
also supports optional map-start `network.firefight_wave_growth` (default zero,
valid 0..32). Each cleared built-in wave adds that many initial enemies up to
the existing 32-body tracker. Wave arithmetic is bounded before multiplication,
including endless runs; carrier children still share capacity and native
placement/allocation failures abort. This is UNVERIFIED source in the same
director/AI/replication path, not a new population capability. Spark's native
scripts still disable it and retain their own authored composition/progression.
No export format, network field, global pool or supported discovery changed.

The same director
now reads map-start `network.firefight_wave_limit` (default 3, zero repeats until
defeat/exit) and `network.firefight_initial_enemies` (default 2, valid 1..32).
Zero-limit progression adapts pinned Faber `firefight.c::firefight_update`'s
`set_limit` policy without adding its separate round/set engine or packets.
Native script ownership still disables this entire director, including these
options; Spark's authored scenarios retain their own HaloScript rules. Larger
counts need native room/resources and share the unchanged 32-body tracker with
carrier children; failure aborts rather than dropping untracked enemies.
Dead bipeds no longer count as standing occupants in the reused spread helper;
full queries remain conservatively occupied. No bodies/loot are deleted by
this placement check. Loaded Flood variants can use existing actor selection,
but this is not accepted Zombie outbreaks, dynamic objectives or huge-horde/
128-player proof. Optional desktop host `network.firefight_gear` now reuses
`extraction.c`'s owner sessions, native grants, loss and escape escrow. It is
frozen at map start. Finite rounds keep victory recovery without a hub. With the
existing valid Extraction hub, players may also activate, hold the ten-second
signal zone, return alive, bank carried equipment and redeploy using the native
deployment control while others continue. Zero-limit endless gear requires this
valid recovery interface before opening/reserving any stash. Use repeatable,
touched-by-player `extraction_deploy` / `extraction_activate` controls, optional
`extraction_loadout`, and `extraction_zone` / `extraction_return` volumes. All
authored player starts must lie in the return hub outside the activation zone;
keep the existing named spawn encounter/actor placement safely away from it.
No new marker, authoring format, JSON importer or RPC is needed.

Source: `extraction.c::{extraction_bind,extraction_control_touched,
extraction_update_run,extraction_survival_evacuation_ready}` and
`coop_enemies.c::firefight_try_evacuation`. Only completed native recovery and
physical return of every living player may give the existing director an early
team-evacuation result/lobby return. Individual banking does not end other active
runs or automatically redeploy that owner. Pending writes, zero survivors,
incapacitation, stale ownership and restored worlds cannot complete evacuation;
finite defeat/abort during activation never awards gear. Actual finite victory
still recovers living players, including an in-progress activation. Shared
lives, owner identities, native inventory consumption, escrow, records and packets
are reused. Source workflow checks and Windows64 compile pass. Native stock B40
endless combat passes through two clears/third wave with gear requested, but
stock B40 has no recovery hub: it does not prove the authored evacuation loop.
Spark must supply that compiled native hub/encounter for gameplay acceptance.
Specific Zombie/Survival evacuation, bank reopen, hardware/platform and full
Survival acceptance remain open; this is not an independent squad quest system.

## Persistent operations: next native handoff

Graphics requirement clarification: retain Spark's native GPU-baked BSP/material
content and its target limits. Forerunner's reused Faber `display.shadow_limit`
changes only native runtime shadow selection; it requires no new authoring field
or cache conversion. OpenH1's reviewed ray-query/world-space AO and enhanced
lighting remain pending runtime integration, not advertised Spark capabilities.
The owner has explicitly authorized that adaptation in the existing renderer.
The current native Video presets and shared per-pixel model-lighting connection
require no new Spark schema, compiled-map handoff or lighting asset. They have
focused compiled-source evidence, not native map/stereo/target acceptance.
bgfx still lacks HDR. Its inherited model Light effects now have a source
connection to the native selected-light mask, shaders and volume-texture cache,
with capability/resource/async fallback; actual GPU/stereo output is unverified.
No new Spark lighting data, fields, asset conversion or authored-map requirement.
The new shared pre-HUD FXAA pass uses native render targets and requires no
authoring/cache change. Its source/lifetime checks pass; rendered/stereo output
is unverified. Optional OpenXR MSAA now resolves two owned eye targets through
that same renderer, with capability/resource fallback; compiled lifecycle checks
use GPU fixtures, and headset output/cost remain unverified. No authoring change.
SMAA/SSAA and World/ray-query AO remain separate unfinished work.
Its ambient-only AO must preserve baked self-occlusion rather than darken native
lightmaps twice. No new editor, lighting asset format or dependency on an upstream
checkout was introduced. See `UPSTREAM_FEATURE_MATRIX.md` and `TEST_EVIDENCE.md`.

The owner's [complete operations directive](docs/forerunner/PERSISTENT_SPARTAN_OPERATIONS_2026-11-14.md)
adds **Operation Black Echo** as the first operations acceptance map: deployment,
rescue, native enemy encounter, technology pickup, escalation, physical extraction,
durable reward and redeployment. It then requires independent squads of at most
six, agreed temporary alliances/PvE/PvPvE rules, interruptible downed-but-alive
revival, true-death looting and reconnect-safe ownership. 128 players and two
concurrent connected regions before all 19 remain proof targets, not capabilities.

Read-only review of Spark `d50b6fa35c747498036a31ba830670665585aee7`
finds Black Echo in `src/creatorStudio.ts::blackEchoMission` / `addBlackEchoDraft`,
ordinary HaloScript in `src/haloScript.ts`, native logic/readback in
`crates/spark-core/src/native_logic.rs`, and encounters in `firefight.rs`.
Its 15 November native milestone in `docs/RUNTIME_INTEGRATION_20261107.md`
records an MCC13 cache with SHA-256
`1773f672708b0aecb629517874f0c143e6ca48bdfc388b653ad91eb4d25d4d2b`,
four compiled groups, normal 1 / insane 2 actors each, five scripts, seven globals
and nine HUD messages. Spark removed that disposable compiled cache and SDK;
Forerunner has no retained playable Black Echo package from that milestone.
That MCC13 compilation also does not satisfy the current CE609-only
`game.start_map` interface by renaming the cache. Supply a retained CE609 solo
cache and its native resource banks for that existing startup path, or review
the existing MCC launch path separately; do not advertise format compatibility
from the disposable CE fixture alone.
This supersedes the older statement that Black Echo authoring was absent.
Read-only refresh at Spark `ac3f1440fad9d7a854f1627202efeb481b2b996c`:
`src/haloScript.ts` implements `weaponOwned` by scanning actual native
player inventories, and `forerunnerExtractionSettled` by a guarded dormant
`forerunner_extraction_complete` callback. It powers `extraction_activate` off
until earlier objectives finish. `creative.rs::verify_native_mission` requires
CE609 for that callback, actual placed technology with the exact weapon tag ID,
and nonempty starting profiles with technology absent from every initial loadout.
These source interfaces match Forerunner's current native ownership/settlement
path; Spark need not implement its own stash writer or revive a manual volume
substitute. Its actual native editor/build candidate is now
`08389ed43814960ddfdfcb401746f277f6a038e2e413e2a26ac8339703cec514`, CE609 solo,
106,368,008B. Forerunner reproduced the exact cache and independent scenario/HSC
readbacks using the committed `scripts/test-firefight-native.mjs` recipe. The
two-account operation in `build/native-spark-black-echo` passes native combat,
rescue, ownership-based collection, optional Flood, defense, escape/bank/callback
and process-restart equipment redeployment. This supersedes the missing-CE-cache
blocker above, without certifying the older MCC package or full expanded mission.
The current candidate has no vehicles or vehicle palette. Next authoring gate:
native transport/seat/pilot/flight/camera content, then actual faction gameplay.
Spark's
scenario globals are shared world state, not independently scoped squad quests.
Do not substitute editor-only JSON, shared global scripts or a second quest VM
for independent authoritative squads.
Faction guidance uses existing `activate_team_nav_point_flag/object` and matching
deactivation calls. Forerunner now retains received markers in its existing
co-op cache and refreshes the changed local viewer after faction selection:
`network_coop.c::{network_coop_note_nav_point,coop_refresh_nav_points}` and
`hud_nav_points.c::hud_clear_player_nav_points`. No packet or authoring schema
changes. Windows64, 12 connected cases/four defect controls and two real Delta
owners' native selection/swapping pass; the final 54-second/58-item comparison
has no divergence. First attachment, same-team respawn and recovery preserve
native guidance (focused checks; native restore acceptance remains open).
Team markers are shared by faction; this does not provide independent six-member
squad membership, separate quest globals or squad-specific objective text.
Native authoring requirements demonstrated by the disposable CE mission:
transplanted multiplayer content does not automatically make human-team Marines
friendly to player-team Spartans. Supply native `(ai_allegiance player human)`
unless the inherited campaign already establishes it. Escort encounters also
need reachable navigation/firing positions inside their actual rescue volume;
`encounters.c::encounter_update_follow` also requires migration-enabled squads
with firing-group masks and a nearer destination squad. A single default squad
with zero masks does not follow merely because `ai_follow_target_players` was
called. An explicit native `ai_migrate` to a return squad is another supported
authoring route; it must still physically reach the volume. Preserve native
allegiance, combat and movement; do not globally force factions friendly or
replace rescue/technology ownership checks with volume entry.
Native faction-team handoff now preserves authored `ai_attach <unit> <encounter>`
and `ai_set_team <encounter> <team>` assignments in co-op. Exact runtime paths:
`source/ai/actors.c::{actor_attach_unit,actor_set_team}`,
`source/game/players.c::{network_player_reconcile_coop_team,network_player_attach_unit}`
and `port/linux/game/network_objects.c::{distributed_host_update_objects,distributed_client_apply_change}`.
The existing reliable description carries the native team; no new packet or
editor save format. The two-account native command smoke in
`build/native-delta-allegiance-rejoin` verifies assignment, live changes, rejoin
and restoration of ordinary co-op, not a finished faction mission. Spark must
use valid native encounters/actor dependencies and host-owned player references;
`(players)` list order is not a network-slot identifier. A reconnect creates a
new body requiring the operation's role assignment. These calls do not select
a biped, grant faction equipment, establish independent squad objectives or
choose a dropship. The full selection/role policy and those gameplay gates remain
engine work; do not expose this as completed UNSC/Covenant operation support.
Optional transport contract (verification is scoped below):
name native vehicles `extraction_pelican` and/or `extraction_spirit`. Declaring
either enables mandatory boarding for Extraction evacuation; invalid/duplicate
vehicle bindings refuse admission. Native player/human teams use the Pelican;
Covenant uses the Spirit; a missing faction craft never falls back to foot escape.
`extraction.c::{extraction_player_dropship,extraction_return_ready,extraction_at_return}`
requires a live, correctly named vehicle and a valid actual native seat while
the craft is in `extraction_zone`, then continued boarding until the craft reaches
`extraction_return` outside the LZ and the native cinematic has ended. Ship identity
uses the full salted handle; leaving/changing ships or factions cancels boarding
credit. The existing escrow/consumption/bank path then runs once per player.
Spark owns the real approach, usable seat entry, pilot/flight, camera/audio and
insertion/objective scripts. Keep the craft alive at the return hub through the
cinematic and settlement. Optional void scripts `forerunner_pelican_complete`
and `forerunner_spirit_complete` receive a wake only after that actual ship's
boarded crew has durably settled. Pending banking/loss delays release; unrelated
field players and another craft do not. Full salted handles prevent replacement
ships inheriting completion; actual LZ boarding rearms the next trip. Authored
handlers must remain wakeable for repeat trips; a finished one-shot script is not
restarted by native `wake`. These crew handlers may be dormant, or continuous
with `(sleep -1)` first on each pass. The whole-operation callback remains
dormant-only and one-shot. The aircraft template uses sleeping continuous crew/
camera handlers, one startup pilot setup and a continuous trip coordinator. It
keeps boarding closed at return until the previous passengers have disembarked;
the next insertion follows actual gear/seat readiness rather than a stale shared
deployment-control position. An already-cleared player on foot at the LZ may
reuse the powered native call control to recall a transport without another gear
transaction/countdown. Use an on-button call control for an idempotent script
signal; ordinary native switch behavior is preserved.
Source: `extraction.c::{extraction_transport_completions,extraction_wake_completion}`.
Windows64 compilation, six connected crew cases/two defect controls and native
fixture HSC compilation pass. Native insertion/boarding evidence is recorded below;
evacuation flight, durable aircraft-return callbacks and full repeat trips remain
unverified.
Missing faction craft bindings now refuse selection/recall without reserving gear
or promising a hub escape. Hub-only prototypes retain their existing role choices.
The repeat-trip continuation passes 19 focused workflows/two defect controls and
Windows64/native script/cache compilation. `build/native-delta-crew-cycle` verifies
two-account insertion plus three explicit presentation-only camera wakes: insertion
repeats and departure plays twice, with both clients entering/leaving cinematics.
Native settlement remains parked/false and gear stays at risk. Comparisons pass
for 73 player seconds/77 item snapshots. These camera probes do not prove aircraft
departure, return banking or a complete second flight.
Final missing-craft guards are compiled/automated only. Spark read-only revision
`d8136fabadb342725b652cb0e1dd387660a56549` adds native camera/patrol authoring and
rebuild workflows; its `src/nativeCutsceneCamera.ts` preserves native radian
orientation/FOV. Forerunner consumes native cameras through the existing path;
authored approach/landing/departure content is still required for flight acceptance.
Release/unload a returning craft after its crew callback; the owned Pelican script
now uses it. No new JSON, launcher, camera system or quest/reward writer. Current
hub-only prototypes retain their old path and are not finished dropship Extraction.
Optional native device groups `extraction_pelican_boarded` / `extraction_spirit_boarded`
provide host-derived boarding readiness to ordinary `device_group_get`. Author
unique dedicated groups, initially zero, with matching named ships; do not share
them with deployment/activation/loadout/faction controls' power or position groups.
At the hub, all deployed players still there must actually occupy the assigned
craft. At the LZ, all players requesting that faction's evacuation must finish
activation and hold valid boarding receipts/seats. Other factions and unrelated
field operations are excluded. Empty crews, pending death, incapacitation, invalid
ownership, cinematics and flight between pads do not authorize a new departure.
The signal only coordinates flight; it grants no equipment or banking authority.
Optional dedicated groups `extraction_pelican_landed` / `extraction_spirit_landed`
publish actual native ground contact (five ticks before parking), upright attitude
and settled linear/angular motion. Native hover preserves the actual grounded
collision flag and parked pose; it does not manufacture a touchdown. They are host outputs, initially zero, with the same
unique matching-vehicle and control-group isolation rules. Binding one also
requires that faction's aircraft to be landed for boarding readiness and seated
hub banking; maps without these optional groups retain the existing contract.
Spark must wait for both the correct pad volume and this landed output before
opening doors or reporting arrival. A volume, hover command or elapsed timer
does not establish a landing. Source: `source/units/vehicles.c::vehicle_is_landed`,
`extraction.c::{extraction_at_return,extraction_transport_boarded,extraction_transport_readiness}`.
No packet/save version or second pilot system is added. Native pilot support is
opted in through existing human-plane tag AI throttle (>0..1), positive avoidance
distance and pathfinding radius (at most 12 units): authored hull clearance,
slower final approach, bounded pitch and native stop/descent controls. Legacy
fighters and unconfigured/recorded planes retain their previous control.
Use native keep-moving cruise points followed by a stop-at-point landing route;
author cruise/final coordinates for the configured aircraft's native world origin,
not its offset pilot seat. `actor_moving.c::actor_move_sample_plane_position` is
called from the current native driver-input update; eyes/perception stay native.
Final grounded arrival uses the existing native `actor_destination_tolerance`,
horizontal aircraft-origin distance and actual collision contact. Idle descent
does not cancel a valid native recovery path. Author final stop radii appropriate
to the physical hull (four units for the owned Pelican), without enlarging hub/LZ
settlement volumes. The tracked `tools/fixtures/extraction_transport_native.hsc`
uses native object-relative insertion/departure cameras. Invader angle/FOV inputs
are radians; derive orientation using Halo's `matrix4x3_rotation_to_angles`
convention for the desired forward/up vectors rather than assuming yaw/pitch/zero
roll preserves a level camera. Spark's existing `engine_machinima.rs` and
`scenario_logic_readback.rs` already use native radian camera data.

Windows64/GL `build/native-delta-transport-complete-crew/result.json` passes two
distinct authenticated players' normal boarding, seated insertion, native pilot
return/recall/landing, evacuation boarding, departure camera, durable four-unique-
item banking across two owners and same-ID redeployment after fresh processes.
CE609 SHA256 `dccd58c9d9703ae5702aaf6df363d163f38b7dc4c7b28940fd6a417447ab2c2b`;
binary `54414ece225094553a99beb42345fcfc21eeaa4b120aa5ca1736e1d8951f10cd`;
source base `a3c820a142` plus the grounded-arrival/camera changes. Existing
comparison: 244/9 player seconds, zero divergences; 248/13 item snapshots, zero
out of step. Both departure PNGs were inspected; original credentials are unchanged.
This accepts the owned Pelican transport fixture, not Spark's full mission,
Spirit, other platforms, human gameplay or headset acceptance. No settlement
volumes, physics/boarding rules, owner identity or durable settlement gates were
bypassed. Current checkpoint and hosted CI/billing limits are recorded once in
`FORERUNNER_MASTER_LEDGER.md`.
The same tracked HSC now configures the craft, AI squad/team, exact driver-seat
label and matching boarded/called/landed groups without a second controller. Its
native vehicle getter resolves the name after placement and on later calls; do
not cache a vehicle in a global initializer before objects exist. Single-aircraft
arrival uses native `volume_test_object`, avoiding temporary object-list casts.
Both native bindings compile; Windows64/GL `build/native-delta-spirit-normal-controls`
also passes two authenticated Covenant/Grunt players' complete physical Spirit
trip, durable banking of two distinct items and fresh-process same-ID redeployment.
The current checkpoint records exact cache/binary hashes, synchronization figures,
the transient aiming mismatch and the distinction from full mission acceptance.

Pilot admission remains native `source/units/units.c::{vehicle_scripting_load_magic,
unit_set_or_test_seat_and_weapon_label}`. Its animation graph must contain the
vehicle's exact driver label. Stock Grunts have Spirit passenger poses, not
`c_dropship driver`. The owned probe uses an isolated pilot biped/actor variant
and a native passenger-seat animation alias in its copied graph, leaving original
tags and player Grunt animations untouched. Author an equivalent compatible pilot;
do not force players into seats or bypass graph compatibility. The owned Spirit's
AI throttle budget is 0.30 after measured coast at 0.65; tune each real aircraft/
route against its physics and actual pads instead of copying a universal speed.

Read-only Spark `a2d0aa0` already implements reviewed aircraft append/readback,
`src/haloScript.ts` `returnAfterInsertion` empty hub return/recall and native route/
landing-pad authoring/readback. Its current isolated composite cache is
`e5711810ca6b2deba4b7802fce4e07c9eff851029b554029c7c7482f61122e71`.
Reuse this source work. Its `docs/RUNTIME_INTEGRATION_20261107.md` explicitly
separates that fixture's route/endpoint/pad admission from observed landing and
a complete Black Echo cache. Next Spark delivery: compose these existing
controllers with the real mission, compatible pilots and seats, measured hub/LZ/
approach/departure routes, native relative cameras and mission objectives; fix
the reported off-BSP Flood start; compile and independently read back that combined
CE609 package. Reuse existing signal groups uniquely instead of appending duplicates.
Synthetic marker coordinates and syntax-only probes do not satisfy this handoff.
Previously accepted actual Black Echo combat/rescue/technology/Flood/defense and
on-foot banking/restart remain accepted; aircraft evacuation in that mission,
independent squads, other platforms and human/headset acceptance remain open.

Optional dedicated groups `extraction_pelican_called` / `extraction_spirit_called`
separate requests made at the shared activation control. The host derives each
output from living, correctly owned, same-faction activating/ready players still
at the LZ (on foot or in their receipted ship). Cancellation, departure, loss and
settlement clear the request; pending gear never requests flight. Use unique
groups initially zero, with matching ships and no control power/position aliases.
Source: `extraction.c::extraction_transport_called` and the existing readiness
update. The aircraft HSC consumes its Pelican request while native activation
continues; only the separate boarded signal authorizes departure. This optional
binding requires a Forerunner runtime containing that function; older runtimes do
not drive it. Existing maps without these groups keep their previous script path.
Windows64 and native HSC/cache compilation, 44 focused transport workflows and
two defect controls pass. `build/native-pelican-faction-call-deployed` verifies
one authenticated player's confirmed deployment, physical boarding, insertion,
disembarkation and real request consumed by the Pelican script. The corrected
probe requires the actual deployed gear record; the earlier stale prompt-based
driver boarded without deployment and correctly could not depart. The successful
probe stops at recall initiation, with gear still deployed and no bank/exception.
Spirit requests and independent factions have automated coverage only; completed
recall/evacuation, multi-client signal acceptance and other platforms remain open.
Source: `extraction.c::{extraction_transport_boarded,extraction_transport_readiness}`
and native `devices.c::device_group_find_named`. Existing device replication is
reused; no protocol, custom HaloScript opcode or map format is introduced.
The owned aircraft HSC now waits for this signal and recognizes all ten native
Pelican passenger labels. Two real Delta accounts in
`build/native-delta-crew-boarding` verify the first passenger waits 90 ticks with
output zero, then both board normally, insert, and disembark. Unchanged comparisons
pass for 54 player seconds and 58 item snapshots, with no native exception or item
mismatch. Windows64/native HSC/cache compilation, nine new connected cases and
three defect controls pass. Spirit and evacuation-flight readiness, six remote
crew members and other platforms remain unverified. Maps omitting these groups
retain their prior authored coordination; Spark can adopt the native bindings.
The native Pelican fixture now verifies normal passenger boarding, pilot-driven
insertion, LZ arrival, disembarkation and extraction activation. Its recall route
does not reach the LZ. Extraction boarding/departure/banking and Spirit flight
remain unverified; do not replace them with a timer or foot return.

Transport checkpoint, 16 November: recovery also rejects all three native
scheduled-death flags in `extraction.c::extraction_player_dropship`, shared by
return and crew-readiness checks (commit `51883fce31`). This is
compiled and automated-verified: 49 connected transport workflows and an
original-defect control cover dying ships, salted replacements, fresh physical
boarding, cinematic-gated settlement and redeployment. Native crash/rescue and
full aircraft banking are not accepted by those checks.

The latest owned `build/native-pelican-pilot-arrival/result.json` passes actual
deployment, passenger boarding, pilot-driven insertion and disembarkation, but
FAILS empty-craft hub arrival; pickup never begins and no gear is banked.
Both native go-to commands were accepted, but command completion did not imply
the vehicle entered the authored return volume. Spark must supply physically
flyable approach/landing/departure routes and volumes that the actual vehicle
reaches; do not weaken the runtime bank gate to accommodate a missed landing.
The opt-in existing AI readback in `network_actors.c::network_actors_log_units`
exposes driver type, throttle and facing. Existing command-list diagnostics in
`source/ai/action_obey.c::action_obey_command_begin` distinguish a rejected path.
These change neither network packets nor native aircraft control. Windows64
compilation, 12 readback cases and two ownership/authority defect controls pass.
Owned runtime and map staging are removed. This is not completed aircraft
Extraction, Spirit flight, human acceptance or cross-platform verification.

The crew-readiness implementation is committed
at `0c32af94de`, repeat-trip/missing-craft guards at `5442b04906`, and native
faction/seat admission at `265a333e20`. These are existing runtime code, not pending
Spark editor work. The two-account insertion and physical Pelican/Spirit boarding
results below are separate from automated settlement-gate coverage. Spark's next
flight handoff must supply native approach/landing/departure content, usable seats,
the dedicated boarding groups above and repeatable completion/camera handlers.
Keep the craft alive at the return pad until settlement releases it. No additional
discovery operation, protocol or launcher is required. The outstanding owned
fixture failure is its pilot's recall approach missing the LZ; the engine must not
bank that failed flight. This handoff has not yet passed end-to-end aircraft
evacuation, cinematic departure, banking and redeployment.

Optional faction controls (16 November): author unique native touched controls
(`triggers_when == 0`) named `extraction_unsc` and/or `extraction_covenant`, each
with a unique native starting profile of the same name. Keep their names distinct
from deployment, activation and loadout controls. Profiles are equipment-only:
zero health/shield modifiers, loaded native weapon references with valid ammo,
and the two native grenade types. Expanded MCC grenade profiles are refused.
Maps without these controls retain their existing behavior. Source:
`extraction.c::{extraction_bind,extraction_control_touched,extraction_loadout}` and
`game_modes.c::game_modes_inventory_starting_profile` in `port/linux/game/`.
An authenticated player chooses on foot in the hub with an idle stash. Selection
changes that player's native human/Covenant allegiance through existing replication;
it grants no gear and does not toggle shared device position or attach a new AI.
Deployment preserves selected recovered weapons first, then fills vacant kit slots.
Saved ammo is not replenished; grenades top up to the authored minimum without
stacking each run. The existing reservation/grant/confirmation/refund and successful
escape/bank paths own all equipment; Spark must not award a second copy.

Windows64 compilation, 100 connected workflows and a grenade-stacking defect control
pass. `build/native-delta-faction-workflow` verifies two distinct bound Delta accounts
choosing opposing factions through native controls, receiving plasma rifle/assault
rifle kits, escaping, banking, restarting and redeploying the same two unique items.
Player comparisons match for 97 operation and 14 restart seconds; item comparisons
have no mismatches and native logs have no exception. This component fixture uses
hub return, not transport evacuation. Models, faction objectives, independent squads,
legacy-peer/platform parity and completed aircraft extraction remain unverified.
Spark's next authored handoff should include these controls/profiles, compatible
native bipeds/equipment, faction objectives, and actual Pelican/Spirit flight, seats
and cameras. No new capability version, save format, launcher or multiplayer
packet is introduced.

Optional native character bindings (base `5442b04906`, 16 November): author unique
not-automatic biped placements named `extraction_unsc_body` and/or
`extraction_covenant_body`, with loaded native biped palette references and native
permutations. Each requires its matching faction control/profile above. The host
uses existing player attachment/replication to change the actual character on foot
at the idle hub, preserving native vitality, grenades, powerups and compatible
original weapon objects; incompatible carried weapons remain beside the terminal.
Selected stored gear is filtered through native weapon/animation admission before
both preview and reservation. Incompatible saved items stay in the same stash,
available again after a compatible character choice; record version/IDs do not
change. Native respawn uses the selected faction's body and starting profile.
Sources: `extraction.c::{extraction_bind,extraction_control_touched,extraction_open_gear,
extraction_player_spawn_profile}`, `game_modes.c::{game_modes_player_change_biped,
game_modes_inventory_weapon_for_unit}`, `gear.c::gear_selected`, and
`source/game/players.c::player_spawn`. No new character controller or inventory system.

Windows64 compilation, 134 connected workflows, 18 gear transactions and two
original-defect controls pass. `build/native-delta-faction-body-banking` verifies
two bound Delta accounts choosing cyborg_mp/Grunt with AR/plasma-pistol kits,
banking, process restart and redeployment of the same two saved item identities.
Both native views read back the actual bodies/allegiances/equipment; player
comparisons pass for 143 operation/55 restart seconds and item comparisons for
147/59 snapshots, with no mismatch or native exception. Original identity files
are unchanged; runtime/map copies and BMP scratch are removed, two lossless
frames retained. Earlier driver probes failed to finish: Spartan-height aiming,
then a stuck input phase; neither required an engine patch or candidate rebuild.
Stash filtering/rollback and respawn selection have automated coverage; bounded
native death/respawn acceptance is recorded below. Other bipeds, faction objectives,
completed aircraft flights, other platforms and VR remain unverified.
First-person hands/HUD stay map-native.
Spark must supply compatible character animations, native seat labels, faction
objectives and actual approach/landing/departure content for transport acceptance.

Physical boarding continuation (base `770f0c65b2`, 16 November): existing native
`unit_can_enter_seat` now enforces assigned Pelican/Spirit factions for actual
player bodies through `extraction.c::extraction_transport_can_board`. NPC crews,
ordinary vehicles and authoritative client replay retain their native paths.
For player-controlled campaign bodies with a missing entry clip but a valid exit
clip/seat marker, `units.c::unit_get_seat_entrance_point` uses the exit endpoint
as the approach. Native proximity, Use, actual attachment, seating and exit still
own the interaction; entry without a clip uses Halo's existing immediate attachment.
Authored entry clips and NPC behavior are preserved. Spark should retain native
seat labels/markers and compatible animation graphs; no new tag format or protocol.

Windows64 compilation, focused regression cases and two original-defect controls
pass. `build/native-delta-faction-boarding-sync` verifies two authenticated clients
choosing Grunt/Spartan, deploying their kits, normally boarding separate Spirit/
Pelican seats and disembarking. Both views agree on exact full parent handles and
seat indices; player comparisons pass for 92 seconds, item comparisons for 96
snapshots, with no mismatch/exception. Gear bytes remain deployed/unchanged, with
no banking. A single-owner Spirit smoke also passes. Earlier stale probe input
and the native exit-only entry defect are recorded separately. Original identity
files are unchanged; runtime/map copies and 393,220,320B of BMP scratch are removed,
two lossless frames retained. This accepts bounded Windows native boarding only;
approach/landing flight, cinematics, return banking, Spirit insertion, human play,
other bodies/platforms and VR acceptance remain open.

Native faction casualties/re-entry (base `265a333e20`, 16 November): faction
equipment profiles retain zero vitality modifiers. `players.c::player_spawn`
preserves the fresh biped's native health/shield around the existing equipment
reset; ordinary campaign profiles and the existing hub respawn override retain
their semantics. `build/native-delta-faction-respawn` verifies two bound owners'
normal grenade deaths, correct-faction healthy hub respawn, durable loss and
redeployment using real previously banked test records with shared native-cache
admission. The other player and unselected stash are preserved; issued gear is lost.
`build/native-delta-faction-loss-restart` verifies persistence/redeployment with
the same binary and retained records. Detailed comparisons/cleanup and the corrected
restart probe are in the current checkpoint and those run results. This is bounded
Windows native acceptance; full aircraft, other bodies/platforms and human/VR remain open.
Native follow continuation: admitted Extraction maps opt into nearest living,
owned-player selection in `source/ai/encounters.c::encounter_update_follow`.
Classic ordering, explicit unit/AI targets and the native eight-target limit remain.
`ai_follow_target_players` / `ai_follow_target_unit` bind the whole encounter
(`ai_script.c`), so separate escort groups need separate native encounters, not
an assumed per-squad follow API. Windows64/12 connected cases/two defect controls
pass. `build/native-delta-encounter-follow-ground` verifies two authenticated
clients, group choices and the same Marine physically walking toward the hub;
unchanged player/item/AI comparisons pass for 24 player seconds. CE609 ground
encounters need compiler-derived BSP-surface firing points, appropriate native
guard/search groups and guard state. The earlier owned fixture retained an
aircraft encounter's 3D firing-point flag; clearing it fixed movement with the
same engine binary. Do not invent surface indices or weaken the ground selector's
surface requirement (`actor_firing_position.c::actor_select_firing_position`).
Independent Spartan squad membership/objectives and complete rescue missions
remain open. No new compiler opcode, discovery capability or protocol.
The owned native CE fixture additionally demonstrates local saved-profile
deployment, combat/rescue/collection, countdown/return, durable bank, the native
completion callback and process-restart redeployment of exactly the banked gear.
This is not acceptance of Spark's Black Echo package or verified remote banking.
The owned `tools/fixtures/black_echo_native.hsc` technology objective now scans
the full native player list one entry per tick, rather than hardcoding two
entries. Spark should likewise use actual owned equipment and the intended
player/squad scope for collection objectives. A six-player/two-instance native
encounter reaches the authored extraction-available stage with a later player
collecting the technology. The subsequent reserve/death-loot repair now passes
unchanged native player/AI/item checks with three local players on each of two
peers. `network_objects.c::distributed_client_apply_inventory` accepts host reserve
pickups independently of predicted loaded rounds; `units.c::unit_drop_grenades`
and `mcc_grenades.c::mcc_grenades_drop_extra` create death loot only on the host.
Spark needs no replacement inventory writer or loot-spawning script. This adds
no discovery capability or independent squad quest interface. Split-screen HUD
and independent squads remain unaccepted. Two real Delta accounts now bank/restart
in the disposable native mission. The later one-host/five-independent-client
encounter also passes unchanged player/AI/item comparison after host-authoritative
NPC drops were completed. This is bounded encounter verification, not six
authenticated banks or independent squad outcomes.
`ce_models.c::ce_models_tags_unloaded` now handles a map with no temporary bitmap
copies during native disconnect/menu return. Precise native evidence and earlier
failures are in `TEST_EVIDENCE.md`.
`game_modes.c::game_modes_inventory_capture` now honors native stowed-item/player
ownership; Spark must not invent a separate writer or attach holstered models
just to satisfy persistence. `menu_functions.c::pc_menu_start_map_update` permits
passive `debug.network_test=log`; active host/join/server ownership is still refused.
For an authored outbreak, use native `ai_force_active` / `ai_allow_dormant` where
activation PVS would otherwise sleep the encounter, then release those overrides.
Keep spawn geometry inside valid collision/navigation. Observe initial spawn
counts every tick if fast combat could kill the entire wave between polls.
Two-player combat/AI/item evidence now reaches extraction availability with the
strict checks passing. `network_objects.c` distinguishes native feigned corpses
without changing object-state packet size/version; a controlled owned native
encounter also proves matching positive corpse vitality and later reanimation.
Spark needs no replacement AI director or new interface for this repair. Remote
banking, readable results UX and the complete multiplayer operation remain unaccepted.
Spark's next handoff must identify the compiled native map/resources/content
identity, rescue/escort actors, technology objects, trigger/terminal bindings,
spawn/region identifiers and mission/audio source; supply reviewed objective IDs,
ordering/branches, completion/failure rules and reward conditions through this
existing contract before Forerunner consumes extension data. Exact event identity,
reconnect/epoch semantics and squad/player/world cue scope need review together.
Spark owns authoring/readback; Forerunner owns bounded per-squad execution and
membership/alliance/loot authority. The shared native bank is implemented, but
quest execution, six-member squad operations and scoped mission audio remain
unfinished and are not advertised by discovery from this checkpoint.

Forerunner now implements optional native living incapacitation and held-Action
biofoam in `port/linux/game/extraction.c` (`extraction_medical_lethal`,
`extraction_medical_control`, `extraction_medical_update`). Native damage in
`source/objects/damage.c` retains explicit/catastrophic kills; the native player
control/pickup paths and `network_distributed.c` clamp incapacitated actions and
predictions. Actual death is never cleared, and only actual death triggers gear
loss. The host's map-start `game.revive` is off by default; treatment defaults to
five seconds and bleedout to thirty. Combat, release, walls, range, vehicle/state
changes and death interrupt treatment. Native friendly factions determine aid
until the separate squad/alliance rules are implemented. A body gets at most one
incapacitation; no medical equipment economy or new animation is claimed.
Runtime ally guidance now reuses `source/interface/hud_nav_points.c` and
`extraction_medical_ally_incapacitated`: up to four nearest friendly casualties,
with native `target_blue`/`default` HUD arrows when present. It consumes no authored
nav-point slot, new scenario marker, editor format or wire capability. Custom
HUDs without either arrow omit the marker; private medical instructions remain.

Desktop network use requires room-wide negotiated Delta `coop` and `object_sync`;
`network_objects.c` sends versioned epoch/tick metadata in existing reserved
UNIT_VITALS bytes, while native player streams retain health authority. Older
peers keep ordinary co-op; no new downs are admitted without both capabilities.
Native agreement 45 has an explicit 44/45 layout-compatible join bridge, not a
general claim of upstream compatibility. `docs/delta.md` specifies the bytes.
This is not a new Spark discovery capability or a mission-data handoff. Spark
should keep native encounters/scripts and export the reviewed Black Echo package
and mission bindings described above, rather than authoring a competing revive
or wave director. Compilation, focused checks and bounded native evidence are
recorded in `TEST_EVIDENCE.md`; complete operations gameplay remains unaccepted.

Optional controller-facing source now uses `source/game/player_control.c`'s
`handle_one_player_input` and `port/linux/game/vr_render.c`'s
`vr_render_player_aim`; `port/windows/src/win32_openxr.c::vr_openxr_tracking_read`
reuses the already located poses rather than owning another XR frame loop.
`vr.aim = "controller"` is opt-in; `"gamepad"` remains the default. Body-heading
separation and native-eye convergence adapt OpenH1 `baed77571d53abe32b27434b4ba869f11db00fe9`
(`halo_vr_aim`, `vr_aim_converge`, `aim_distance`, AGPL-3.0-or-later).
Native facing/throttle carry the action; no network fields or version changed.
Map lifecycle and `source/saved games/game_state.c::before_load_procs` release
outside-image tracking state. No new Spark export or content schema is required.
Optional controller snap turning now reuses the same pin's `vr.c` latch and
0.7/0.3 thresholds in `vr_render.c::controller_turn_delta`. `vr.turn` defaults
to native `"smooth"`; `"snap"` uses `vr.snap_turn_angle` (30 degrees, 1..180).
Only eligible local first-person controller aim consumes its selected physical
turn stick. Neutral rearming covers ownership, menu, focus/tracking and world changes;
vehicle/gamepad controls remain native. Existing facing carries the turn, without
new network fields, an XR frame loop or Spark data. Source only, unverified;
actual Quest 2/Index input, direction, comfort and mixed controls remain open.
Controller weapon posing now adapts the same OpenH1 pin's
`vr_render_weapon_camera` and `vr.weapon_offset` defaults.
`vr_render.c::prepare_weapon_camera` prepares one dominant-hand animation camera
for both eyes. `source/render/render.c::render_window` uses it only around
native first-person node building and restores the head camera before world
rendering. `view_fov.c::viewmodel_projection_begin` preserves headset eye
projection instead of applying flat-screen viewmodel FOV. Native weapon models,
animation graphs and visibility remain authoritative; no new Spark resource
format or hand/weapon renderer was introduced. The current Windows64 build
compiles; headset output remains unverified. `vr_render.c::vr_render_reticle_begin` reuses
`weapons.c::weapon_get_projectile_ray` and native projectile collision for one
finite pre-spread target per frame, projected separately through each eye's
native frustum. `hud_weapon.c::crosshairs_draw` scopes the existing aiming
sprites; `hud_draw.c::hud_draw_bitmap_with_meter` applies the per-eye placement
without moving warnings, meters or scope images. No second HUD renderer,
simulation update, firing action or network field is introduced. The gamepad
weapon camera now keeps native body aim before headset turning/leaning, also
following OpenH1. Grip/reticle alignment, vergence comfort, physical scopes,
standalone/two-hand interactions, vehicle aiming and headset acceptance remain
unverified or unfinished; Spark must not advertise full VR support from these
source interfaces. Existing native weapon HUD/model tags remain sufficient;
there is no new map-side VR schema or projectile-flight prediction.

Optional `vr.dominant_hand="left"` / `HALO_VR_DOMINANT_HAND=left` now follows
OpenH1 `vr.c::{hands_assign_input,hand_controller}` at that same retained pin.
`vr.swap_sticks` accepts auto/yes/no (auto follows the weapon hand). Shared
`xinput_sdl.c::{vr_openxr_weapon_hand,vr_openxr_turn_hand}` drive fire/grips,
movement/clicks, native convergence, weapon camera, snap turning and rumble.
Face-button labels, menus/Guide and named PTT bindings remain physical; gamepad
aim retains native halves. Live role changes wait for neutral controllers;
pause remains available. `vr_render_weapon_nodes` adapts `vr_hands_mirror` after
native interpolation, with scoped winding in both existing OpenGL/bgfx paths.
No renderer, animation asset, input/voice system, packet or Spark export added.
Fourteen connected native-C cases and three broken-integration controls pass.
Hardware calibration, mirrored model rendering, haptics and headset gameplay
remain unverified. Hidden OpenGL A30 multiplayer agreement fails in both the
handedness build and preceding `4bde81ed08` control; the native position-state
sampling/replication cause is unresolved. Keep this acceptance gate open rather
than treating headset absence or successful compilation as gameplay proof.

Native proximity voice now has optional `audio.voice_obstruction` source
and optional Windows64 `vr.push_to_talk` input source. The latter defaults off,
requires native PTT mode, and assigns one existing Touch/Index grip or stick
click through `xinput_sdl.c::openxr_gamepad_read` / `XInputGetState` to
`HALO_KEYBOARD_PUSH_TO_TALK`. `voice_game.c::controls` remains the action consumer;
native Delta/Opus capture, routing, channels and host permissions remain owners.
The selected XR gameplay button is reserved during tracking gaps; release
rearming covers focus, tracking, setting/reference and menu/Guide/chat/capture
ownership. Both-stick pause and native menu controls retain priority. D-pad vote
actions are never synthesized. No Spark schema, new VOIP/packet, input polling
loop or microphone SDK was added. Source only, UNVERIFIED for Quest 2 PCVR and
Vive/Index Knuckles; this is not a bridge to stock OpenCE voice or cross-region
audio acceptance. Native Linux/non-OpenXR paths retain their existing controls.

The existing `audio.voice_obstruction` option defaults to false.
`source/sound/sound_manager.c::sound_world_obstruction` calls
the inherited `game_sound.c::compute_sound_obstruction` and native millibel
conversion. `voice_game.c::local_update`, `voice.c::voice_environment` and
`dsound_sdl.c::dsound_voice_place` feed that result into the existing I3DL2 mixer;
no VOIP transport, microphone policy or network packet changed. The reused
query cache now keys BSP identity and clears on native map/restore paths.
Spark supplies ordinary native BSP sound/cluster/visibility and collision data;
no new acoustic sidecar is required. Team/party radio ignores obstruction.
Optional XR obstruction now adapts the pinned OpenH1
`game_sound.c::sound_vr_ears_get` / `sound_manager.c::sound_vr_listener_head`
policy. `vr_render.c::vr_render_listener` caches native eye-to-head clearance
and chooses a native-position fallback across solid geometry while retaining
head orientation. `sound_manager.c::refresh_listener` publishes its applied
origin/cluster; `game_sound.c::compute_sound_obstruction` consumes that snapshot
for both game sounds and optional proximity voice, without another XR query or
observer/player mutation. `sound_clear_obstruction_cache` clears native datum
results/head publication on map/restore paths; both cache levels distinguish
BSP and native/head modes. Native per-sound tick cadence remains in force.
This is UNVERIFIED source for the currently loaded BSP. Audible head/door/tunnel
transitions, multiple local VR listeners and connected-region acoustics remain
unverified or unfinished; neither source presence nor a GPU draft proves them.

All changes after the admitted executable remain source-only/UNVERIFIED. No
capability/discovery field was upgraded by this checkpoint. Spark's next useful
handoff is its exact compiled native encounter/cache and ordinary matching data,
with the finite director OFF, followed by the authored Extraction bindings and
two-region player/Warthog route. Neither project may claim gameplay or copy
personal stash data from this source review. Preserve copyright/license notices
if a later reviewed source adaptation is made; do not publish or push it.

## Native formats and extended data

Read-only Spark source refresh at `e472e79` found its existing Extraction authoring
path now checks **every** authored hub start (including other-mode/hidden starts),
fixing the earlier `f8213a5b` any-one-start gap.
`src/nativeLogic.ts::extractionLayout` also checks named controls/volumes, startup
power and relative placement;
`native_logic.rs::scenario_commands` writes ordinary rotational trigger volumes,
and `compile` reads back named Extraction controls' touched-by-player tag mode.
`verify_compiled` checks required scripts, not complete playable Extraction.
Forerunner `extraction.c::extraction_bind` now refuses duplicate names, oversized
short-index blocks, aliased terminals and incompatible native control modes
before taking map outcome ownership. Spark's next work is optional loadout-control
authoring/readback and the compiled solo cache handoff with matching native
dependencies, controls/volumes/loadout and a bidirectional route. Its
`crates/spark-core/src/runtime.rs` still pins Forerunner `firefight-build-002` /
`21a809b0`; current-project cache deployment/Live Rebuild are not completed there.
Adopt a reviewed current runtime identity through that existing path, not a new
launcher. Native terminal use, cross-client transfers, death/hub respawn, escape,
reopen and redeployment still need the authored-map integration acceptance.
Neither repository's source is a build dependency of the other; Spark was not
modified and no editor metadata, source file or personal gear was imported.

Preserve supported native scenario/tag formats. Extended runtime data belongs in
explicit versioned sidecars, with target runtime, schema version, map/content
identity and required capabilities. A runtime must reject an unknown required
feature before modifying a session. No proprietary Halo tag extension or legacy
conversion is introduced by this checkpoint.

Community compatibility lead (14 November): Mota supplied
[`SutlabSol/OpenCE-Forge`, main](https://github.com/SutlabSol/OpenCE-Forge),
initially inspected at `2467e708b67bdab994b4f3725516a72e31f629b3` and refreshed
after his update to `91099ce375632167e425edd7fd40dadf83ab93d2` (2.10.11). Its existing
`port/linux/game/forge_runtime_tags.c` contains the exact format entry points:
`forge_write_sidecar_state_file`, `parse_forge_state_text_buffer` and
`forge_load_saved_map_state_if_present`. Saves use `custom_maps/<name>.txt`,
`[OpenCE Forge Map]`, `Base Map:` and `[ForgeState]` with typed placement/edit
records. Reuse this upstream format rather than defining a competing writer.
The required runtime object/tag admission and full record semantics are not yet
implemented here; Spark must not advertise Forerunner as its sidecar consumer.
The fork's source selects stock protocol for ordinary games and Forge-specific
`0x4630` / game kind `0x46` for Forge at the refreshed pin (the README's older
`0x462A` description is stale); its messages and tag-streaming paths are
not automatically compatible with Forerunner. Watanoge's separate Forge/Firefight
snapshot remains pinned in [the upstream matrix](UPSTREAM_FEATURE_MATRIX.md).
A placement file does not supply missing client gameplay tags: every peer must
resolve compatible content and dependencies before spawning, including late
joiners. Existing visual-only loose-tag mods do not meet that requirement.
Keep proprietary base MCC data local; no asset-sharing capability is implied.

The refreshed branch includes compatible OpenCE transfer fixes now cherry-picked
here with original authorship (`ebfa05b7`, `796df195`). Existing map sharing asks
eight pieces of the host, two of peers, at most 32 overall; upload allowance and
rate calculation use 64-bit intermediates. Serving catalog files stay open only
through the serving frame, and the existing upload selector accepts up to
256000 KB/s without changing its default. Forerunner's private-join status also
uses the shared rate helper. No map bytes, packet layouts, hash validation,
Spark authoring schema or discovery capability changed. All local behavior and
throughput remain UNVERIFIED: upstream commit test claims are not our evidence.
No tests, builds, launches or pushes ran. Forge tag streaming remains separate
source-adoption work, not implied support.

UNVERIFIED campaign/co-op reuse from that pin: original commits `ac152e44`,
`8c70e633`, `511da10e`, `8903981f` and `958cd833` were cherry-picked with author
and provenance intact. `source/camera/director.c::director_unattended_scripted_camera`
and the native observer update let a host with no local player film scripted
cutscenes through existing co-op presentation messages. `network_coop.c` retains
the matching old-server fallback and existing new-game/recovery reset owners.
No camera/network payload was added. The fallback is presentation, not exact
reconstruction of a camera an older server never sent.

`source/hs/hs.c::main_set_map_name_evaluate` now distinguishes owning console,
script and global-initializer threads. Campaign scripts may request a native
next mission through `main.c::main_script_map_name`: MCC resolution remains in
`mcc_main.c` and requires the current package; CE uses this fork's existing
`halo_map_families.h` APIs and native solo-cache checks, resolving `<name>@ce`.
Known pack UI and Stubbs retain their existing native selection. Local/host
authority, Extraction and built-in Firefight outcome ownership are preserved;
unrelated scripts keep running. Pending destinations clear on loss, restart,
menu/map replacement and every native restore. Scripted CE transitions do not
award a stock mission selected earlier in the solo menu. Reduced MCC/CE builds
keep their feature gates. These ordinary campaign transitions do not implement
continuous MegaWorld traversal. Spark can use standard native `map_name` source
within these limits, without a new export schema or launcher. All local behavior,
camera placement, peer progression and platform parity remain unverified; no
tests/builds/launches ran, and discovery was not upgraded.

Legacy target conversion remains Spark's responsibility. The
[compatibility matrix](COMPATIBILITY_MATRIX.md) records the evidence limits for
Xbox, PC, CE, MCC and the community runtimes; unsupported gameplay is not a valid
export target merely because a file can be written.

Live-edit transactions remain a later gate. The immediate shared target is the
authored native Firefight loop above, without a generic RPC framework.
