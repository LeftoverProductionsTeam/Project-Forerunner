# Forerunner engineering checkpoint

Updated: 15 November 2026. The persistent engineering goal remains active.
This is an incomplete engine integration, not a release or gameplay acceptance claim.

Current revival guidance: the existing medical state now drives the native HUD's
four nearest friendly casualty arrows, without consuming scripted mission-marker
slots. Native occlusion/off-screen/distance drawing and render interpolation are
reused; custom HUDs fall back from `target_blue` to `default`, then omit missing
art safely. Host-private notices announce a downed ally and bounded bleedout
reminders. Thirty-five connected checks and five broken controls pass; Windows64
compiles. Actual hidden OpenGL B40 co-op demonstrates host and joining-player
held-Action treatment, the same body recovering to 25% health, and marker removal.
Both acceptance runs compare 77 seconds at unchanged synchronization limits.
Client capture exposed tiny positive health quantizing to zero: guidance now
uses native death/incapacitation authority instead of requiring positive health.
The corrected client marker is visually confirmed. Earlier stimulus failures
and missing-marker captures remain as compact evidence; copied maps/A/B runtimes,
bulk screenshots and compile scratch are removed. Final shorter casualty-caption
text compiles but is not recaptured. Enemy/ownership/budget gates have source
coverage; dense squads, native hostile encounters, other platforms, human and
headset acceptance remain open. No new renderer, wire state, timer or writer.
Next: native death-loot ownership and persistent recovery through the existing
inventory paths. Do not expand this accepted guidance slice or repeat old
protocol/revival investigations without a new defect.

Current equipment repair: Extraction and Survival now test each recovered
weapon with Halo's existing `unit_can_use_weapon` before dropping any carried
gear. Loaded/native-compatible tags can still be unusable by a different biped's
seat/weapon animation or active gametype. Either refused weapon now leaves the
carried objects and selection untouched, deletes provisional grants and refunds
the same saved identities. A compatible retry uses the existing deploy/escape
loop. Twenty-three connected checks pass; three original-defect controls detect
carried-equipment displacement. Windows64 compiles incrementally. Actual native
alternate-biped deployment, authored hub gameplay and other platforms remain
unverified. Later unforeseen attachment failures retain the existing rollback
behavior (old drops remain world loot); no stronger atomic-grant claim. No new
storage, UI, protocol or Spark interface. Compile scratch is removed.

Current multiplayer repair: a capped host frame could discard elapsed time while
clients kept advancing, leaving them permanently ahead. Client pacing now uses
the existing validated host tick, receipt age and bounded measured latency. No
clock rewind, wire change or second synchronization owner; stale evidence stops
holding the client, and host/local/menu clocks retain their behavior. Ten actual
clock workflows and four broken controls pass; Windows64 compiles incrementally.
An isolated hidden OpenGL A30 session with a forced 2.203-second host pause passes
36 compared seconds at unchanged limits (maximum separation 0.116 world units).
The same controlled pre-fix pause fails and leaves the client 39 ticks ahead;
the fix returns it to two ticks. Failed results remain intact. Native Linux,
WAN/loss, longer sessions, larger squads and hardware VR remain unverified.
Owned maps/peer copies/scratch are removed; compact logs and one baseline binary
remain. This resolves the diagnosed rendered co-op clock defect, not complete
multiplayer acceptance. Resume the highest-value independent gameplay slice.

Current VR gameplay source: OpenH1 dominant-hand reuse now connects controller
aim/fire/grips, optional move/turn stick roles, model camera/reflected native
nodes, scoped OpenGL/bgfx winding and rumble. Physical buttons/PTT and native
menu/gamepad behavior stay consistent; live role changes require neutral input.
Fourteen connected checks and three negative controls pass; Windows64 compiles.
Headset, mirrored GPU output, rumble and full VR gameplay remain unverified.
Hidden OpenGL A30 player-state agreement initially failed both the handedness
build and unmodified `4bde81ed08` baseline. The clock defect above reproduces and
repairs that failure class; original failures remain intact. This is desktop
co-op evidence with XR disabled, not VR hardware acceptance.

Current Survival recovery: the built-in encounter now reuses native Extraction
activation/return controls, escrow and hub redeployment. An individual can recover
and rejoin while others fight; every living player recovered and physically home
ends the existing director through its result/lobby path. Endless gear requires
valid hub bindings before reserving a stash; finite completion without a hub
remains. Victory/defeat during activation, pending writes, stale ownership and
zero survivors are covered by 20 connected checks plus two negative controls.
Windows64 builds; native endless B40 passes 77 compared seconds, two clears/200
points and third-wave progression, with AI/damage/death agreement. This is stock
combat, not authored evacuation or Zombie outbreak acceptance. Existing Spark
contract specifies the same native markers; no new save/director/protocol/editor.
Copied maps/peer runtimes/scratch are removed; compact evidence remains.

Current authored co-op handoff: optional host `network.coop_script_adaptations`
defaults true for legacy campaigns. False preserves exact authored player lists,
ALL/visibility tests, individual transport/equipment actions and native script
save safety. Existing host execution, replication, explicit teleports and empty
player wait protection remain. Policy is captured at map start so live setting
changes cannot split a cutscene's suspend/resume. Twenty-seven connected cases
and two ignored-policy controls pass; Windows64 builds. Final native A30 co-op
with only the host setting false passes 37 compared seconds at unchanged limits.
Maps/peer copies/scratch are removed; originals retain their hashes. The existing
Spark contract names the launch option. Authored operations, independent squads,
Black Echo objectives, cutscene acceptance and other platforms remain unfinished.

Current Extraction repair: failed native equipment grants retain their refund
policy through disconnect, player-slot reuse and map exit. Actual deployed
departures still lose gear. Native weapon admission now matches distributed
inventory's authored starting-ammo limit. Eighteen connected workflows and two
original-defect controls pass; Windows64 compiles incrementally. The existing
fixture checks identities, grenades, cleanup, redeployment and recovery; authored
native-map acceptance remains open. A storage outage lasting through session
close still cannot guarantee a refund; its failure is logged. Abrupt-crash loss
policy is unchanged. No save format, protocol or subsystem was added, and owned
compile scratch is removed. Details are in TEST_EVIDENCE.md.

Current co-op voice repair: native co-op has no multiplayer engine, so the old
Chupathingy engine-pointer gate incorrectly selected lobby routing. Both existing
voice providers now share the native client-session phase predicate. Chupathingy's
positional context accepts only a live, owned unit and valid native coordinates.
Seventeen connected context/router checks and two original-defect controls pass;
Windows64 builds. A private native A30 run with lobby voice disabled exchanges
and decodes the built-in synthetic tone on proximity in both directions, with
37 seconds of player agreement. Physical microphones stay unopened. Copied maps,
peer runtimes and generated tone WAVs are removed; compact logs remain. Actual
microphone/PTT/headset, spatial-hearing/occlusion, cross-region, cross-session party
and OpenCE/Delta voice-bridge acceptance remain open. No voice packet or codec
change and no new capture/transport system.

Current Firefight AI repair: Faber-derived pursuit now respects the actor's native
hostility table and current live player ownership. It stops reclassifying allies
as enemies and ignores departed/recycled bodies. Ten focused cases and two
negative controls pass; Windows64 builds incrementally. A private B40 one-wave
run observes the tracked enemy alive, moving, damaged and dead on both peers,
100 team points, victory and lobby return. Existing AI comparison passes at
unchanged limits. The generic 90-second duration gate reports ERROR because the
finite encounter ends after 50 seconds; its result is retained, not relabeled.
Native alliance-change, creator-map, defeat/lives and other-platform acceptance
remain open. Copied maps and peer runtimes are removed; compact evidence is in
TEST_EVIDENCE.md. No new director, target-state owner or wire format.

Current shared equipment: optional host `game.shared_gear` enables compatible
weapons across Extraction/Survival maps and rebuilt caches. Selection, confirmed
discard, reservation/refund and recovery reuse the existing version-1 owner bank.
One native admission function checks loaded paths, objective flags and exact
ammo/battery bounds; unavailable items stay stored. Target-map weapon behavior
is authoritative; this does not import tags or prove mod-content equivalence.
Default off preserves per-cache choices. Fifteen gear and twelve connected source
workflows pass, including cross-cache Survival reuse and preserved item identity;
native-admission and uncertain-choice negative controls are detected. Windows64
bgfx/OpenXR compiles incrementally in the retained non-LTO environment. Actual
authored cross-map gameplay remains unverified; no repeat of the unchanged map
handoff blocker or earlier native protocol/revival investigations was needed.

Current gameplay handoff: accepted Extraction deployment and activation now use
the native control-group transition and effects, allowing authored HaloScript to
observe them. Refused/storage actions stay consumed; the host rechecks native
power, usability and one-use locks before touching gear or activation. Six
connected source workflows pass with MSVC, and restoring the original swallowed
deployment signal makes the regression fail. Windows64 with bgfx/OpenXR compiles
in the existing non-LTO mode. A private A30 two-peer native smoke compares 37
seconds at unchanged tolerances (largest position difference 0.1184 world units).
Both players move/use weapons; this does not prove AI encounter progression or
the authored controls/gear loop. Spark-authored mission and headset acceptance
remain open. Copied maps/peer runtimes are removed; compact logs and one reusable
root-local build cache remain. Unfinished HDR work is preserved in a named local
stash and deferred in favor of core gameplay. TEST_EVIDENCE.md records the native
commands, input correction and LTO limitation; do not repeat those investigations
without a new failure. The existing dependency downloader now restores the same
pinned licensed renderer sources without creating nested Git repositories.

Current stash management: the existing optional loadout terminal can now preview
and explicitly discard one owned stored weapon to make capacity. Crouch-use
shows the exact weapon/ammo; a second use after one second, within ten seconds,
confirms that item's identity and record generation. Stand-use cancels a preview;
an already published confirmed deletion is finished rather than falsely reported
as cancelled. Body/owner/hub/life changes cancel stale previews. Grenades, other
owners, other cache gear and deployed reservations are protected. Fourteen gear
cases and 25 connected source workflows compile/pass with installed MSVC; actual
native terminal gameplay remains unverified. No new save format, marker or writer.

Current inventory slice: hub loadout choices now persist through the existing
locked version-1 gear record. Compatible item order stores the choice; no new
save fields, writer or packet is needed. Each native cache's order rotates
independently, uncertain published choices retry without cycling twice, failed
native grants restore the selected pair, and successful recovery leaves returned
gear ready for the next deployment. The existing terminal reports saved choices
and clears a recovered save error. Eleven real gear-source cases and 21 connected
Extraction/Survival/medical workflows compile and pass with installed 32-bit
MSVC; the saved-choice flow also crosses into finite Survival with recovered
ammunition unchanged. A broken uncertain-retry control is detected. This is
focused harness evidence, not a full engine build or native gameplay acceptance.
Only tiny repository-local scratch was used and removed; original saves/assets
and other repositories were untouched. Rarity, upgrades, armor/equipment and the
full stash/hub interface remain unfinished.

Current medical milestone: optional native living incapacitation, continuous
held-Action biofoam and negotiated client input suppression are implemented.
Windows64/OpenXR compiles; 45 connected medical/gear/resupply/co-op checks and
nine checkpoint checks pass. A private two-peer B40 smoke proves downed movement
suppression, same-body revival and matching 25% native health, with 66 seconds
compared at unchanged tolerances. Native Action is edge-latched, so treatment
reuses its existing unlatched long-hold gesture. Medical metadata now uses the
active native checkpoint epoch, fixing an observed client movement divergence.
Missing optional scripted damage resources now no-op instead of asserting.
The probe pauses AI and sets up the injury; it is not Black Echo, human play,
death-looting acceptance, physical VR or complete multiplayer operations proof.

Current postprocessing: bgfx now reuses the OpenGL FXAA source through the
existing pre-HUD hook, compiler queue/cache, fullscreen quad and copy helper.
The native AA reader's old FXAA-to-MSAA remap was still disconnecting that pass;
the connected saved-setting regression reproduced it, and the remap now covers
only unsupported SMAA/SSAA. Ten real settings/pass/lifecycle cases compile/pass.
One source texture is reused across ordered windows/eyes; copying and filtering
are clipped to each view, native destination alpha is preserved, and resize or
disable retires the old copy. Pending/failed shaders and unavailable resources
leave the native picture intact. Nine real pass/lifecycle cases and three
backend shader-source rewrites compile/pass; broken alpha, resize retirement
and texel-bound controls are detected. Driver compilation, real captures, GPU
cost and hardware stereo remain unverified. No simulation, packets or assets
changed. SMAA/SSAA and HDR still need bgfx adapters; the inherited model Light
effects now have a source connection below, with real driver acceptance open;
OpenXR MSAA now has source implementation through the same renderer, below.

Current stereo anti-aliasing: the existing MSAA setting selects a common eye,
native intermediate-color and depth sample count. Two owned eye targets reuse
bgfx's resolve/copy pass into single-sample runtime images; size/format changes
retire their framebuffers. Unsupported capabilities, missing programs and failed
allocations retain single-sample rendering without allocating every frame. Menu
canvases remain single-sample, and disabling MSAA releases the owned targets.
Shader preparation uses the known headset format. Material target lookups now
select the same owned eye target as draws; a connected regression caught the old
lookup returning the unresolved runtime image. Nine actual-source lifecycle
cases compile/pass on MSVC x64, three broken controls are detected, nine FXAA
regressions and five format checks pass. GPU calls are fixtures: actual Vulkan
shader compilation, hardware stereo, image encoding and GPU cost remain open.
No headset, map, original asset or network packet was touched.

Current custom lighting: the twelve inherited GulchCE effects now have individual
saved switches under Video's Custom Light Effects view in both existing menu
styles. Advanced > Light Effects > Custom uses them; Auto/Low/High/Off keep their
previous behavior. Classic still turns enhanced lights off, and per-effect
choices survive preset changes. The existing selector caches the mask instead
of rereading twelve settings per draw. Five actual-source choice/reload/cache
cases, three unsupported-target guards and six native menu cases compile/pass;
rendered UI, visual effects, driver/hardware and dense-encounter cost are open.
The inherited model effects now have a bgfx connection below; actual rendering
remains unverified. The source audit
now includes pinned Faber, OpenCE-Forge and GulchCE setting declarations and their
remaining semantic gaps. No second effect, renderer, menu engine or writer.

Current graphics integration: the existing native Video screen now previews
Classic/Enhanced shading, shadows, model detail and filtering, with Custom for
manual combinations. OK uses the individual settings' existing writer; Cancel
writes nothing. No preset file or second settings system was introduced.
The shared bgfx path now connects inherited per-pixel model lighting, native
light registers, matched shader pairs, async/failed-link Classic fallback and
shader preparation. Its missing MCC bump-material key is also connected.
Ten graphics-source cases, five native-menu workflows and the MCC combiner
compile/pass; four deliberately broken controls are detected. Windows/Linux/
Metal driver compilation, actual UI/gameplay, GPU timing and VR remain unverified.
Video help states bgfx's HDR/SMAA/SSAA limits and unverified model-effect path.
The owner's Universal Halo Graphics directive explicitly authorizes adapting
OpenH1's World/ray-query AO in the existing renderer. No approval remains pending
for that work, but no AO implementation or working AO option is claimed yet.
Exact option classifications and renderer requirements are in the existing
UPSTREAM_FEATURE_MATRIX.md; native baked lightmaps must not be darkened twice.

Current model Light effects: bgfx reuses GulchCE's native selected-light mask,
vertex/per-pixel falloff and collision-filtered volume fields. It binds those
fields through the ordinary native texture cache at fixed units 6/7, preserving
material stages 0..3. The existing shader cache/preparation retains distinct
immediate/streamed, vertex/per-pixel and effect variants; pending/failed effects
fall back to ordinary lighting and retry their cached jobs. Unsupported vertex
volume/sampler capabilities or optional startup failures keep the native path.
Missing fields use white visibility; partial startup resources are released.
Fifteen actual-source resource/variant/preparation/rewrite workflows compile/pass
on MSVC x64, ten existing lighting regressions pass on x86, and three broken
controls are detected. GPU/OpenXR calls are fixtures, not driver compilation or
gameplay. Real Vulkan/Metal/D3D11, stereo, map transitions and GPU cost remain
open. OpenH1 World/ray-query AO, its world-light-list/normal-map path and bgfx
HDR remain unfinished. No new lighting system, collision field, cache or writer.

Previous graphics milestone: Faber's optional soft shadow budget is connected through
the native renderer, setting persistence and generated Video/Advanced menus.
Default zero preserves stock selection/fade arithmetic. Bounded per-window
history resets on map load; repeated passes in the same native frame share one
threshold, including stereo eye passes. Windows64/OpenXR compiles and ten focused
shadow/menu checks pass. Hidden Vulkan gameplay frames were inspected, but the
native comparison is INCOMPLETE: 288 on-foot drift samples fall short of the
existing 300-sample minimum. Keep that result open; do not relax the judge.
GPU savings, dense-crowd appearance, Linux execution and hardware stereo are
unverified. OpenH1 AO/enhanced graphics parity remains unfinished.
Latest scope: further work/files remain only in this root repository on master;
do not access external staging or other repositories without renewed direction.
The [complete directive](docs/forerunner/MASTER_DIRECTIVE_2026-11-14.md) is retained;
none of its unaccepted requirements is superseded by this checkpoint.

## Authoritative source and provenance

- Source: `D:\Project Forerunner`, branch `master` (renamed at the user's request;
  existing commits and working edits retained).
- Foundation: FractumSeraph/OpenCE `eae91445333bc8ebfb1604e08fcedffbd5c3a558`.
  Its history contains OpenCE `1519c65facbb068e44b956da2769198a83a17847`
  and Chupathingy through `a42702df52416de4008e91fe18c49f94d9efabb0`.
- Latest gameplay implementation: `21a809b004a057548b641d023df24a613cf14f47`,
  a finite Faber-derived Firefight encounter using existing co-op replication.
  Campaign variant repair: `cf7cb64a`; actor readback: `5c3266fc`.
  Seven verified repair/tool commits follow `b2bd604329c71f06c1109c632ff19ea4c29e4027`,
  the author-preserving cherry-pick of Milenko's Chupathingy commit
  `78d28a70839998775322af8aa571fa494966c0c8`.
- All upstream history, licenses, credits and notices remain in place.
  The configured upstream remotes have disabled push URLs; no publication was authorized.
- [Source comparison](UPSTREAM_FEATURE_MATRIX.md), [target compatibility](COMPATIBILITY_MATRIX.md),
  [test evidence](TEST_EVIDENCE.md), [limitations](KNOWN_LIMITATIONS.md),
  [Spark contract status](SPARK_INTEGRATION_CONTRACT.md).

## Current engineering gate

Current primary milestone: the connected host/local Extraction weapon/grenade
loop is implemented using native pickup ownership, a shared owner gear record,
hub loadout selection, reserve-before-grant deployment, physical escape,
consume-before-bank settlement, loss, hub respawn and redeployment. The owner
explicitly approved native gear records and locking. Verified remote account
proof is inherited from Delta; clients never submit stash inventories. Interrupted
deployed reservations lose only at-risk gear; saved escape escrow rolls forward
once. Checkpoint/core restores quarantine banking until a fresh map.
Windows64/OpenXR compiles; 29 focused checks pass, including real Windows file
locking/process death and connected source workflows. A private x64/x86 B40
compatibility smoke passes; it has no Extraction markers and creates no gear
records. This does NOT prove native multiplayer Extraction. Spark must supply
the compiled authored hub/cache through its existing handoff and adopt a reviewed
current runtime identity. Its optional loadout control/readback is specified in
the existing contract. Finite built-in Survival now optionally shares those same
owner sessions and native gear transactions: spawn reservations, true-death loss,
actual finite-victory recovery and lobby wait/retry. Default off; endless waves
do not reserve gear without an authored recovery path. Existing shared lives and
script ownership are unchanged. Windows64/OpenXR compiles and 37 focused checks
pass. Private x64/x86 co-op reaches director preparation and passes comparison,
but no native winner bank or successful Survival session is proven.
Armor/equipment, rarity/upgrades, arbitrary cross-cache asset equivalence and the
original full hub/menu remain unfinished. Optional native-compatible cross-cache
weapon reuse is source-implemented above; it does not make missing tags usable.
No personal inventories, original maps, Spark source or remote configuration
were modified. This cycle delivered runtime gameplay source and necessary
durability/ownership fixes, not a new editor or coordination framework.

Previous runtime milestone: Windows64 compiles with the existing optional
OpenXR/bgfx Vulkan path enabled, after repairing native script-string conversion
and missing declarations. With VR requested and its loader unavailable, it renders
native desktop gameplay and completes a 32-second private comparison with the x86
peer. Real frames were inspected. Shared controller/PTT checks and the existing
Spark discovery client pass; stereo headset play and physical interactions remain
unverified. Sparse peer comparisons now fail instead of passing on one sample;
native logs sample at absolute simulation seconds without changing wire data.
The observed large position difference is at the scripted test gather boundary,
and shield differences occur across scripted-hit boundaries; no tolerance changed.

Inventory retry/resupply ownership repairs in `c6f9255c2` and `3d5f28dcc` remain
compiled and covered by focused/native checks. Historical results, exact build
identities and limitations stay in `TEST_EVIDENCE.md`; only redundant current-status
prose is consolidated here. Durable reservation/settlement, verified owner
admission and failed-run reward protection now have connected source coverage;
native authored-map multiplayer and crash/gameplay acceptance remain open.

CI safety source now keeps master/main development artifacts and existing
Windows/Linux/macOS packaging, but removes inherited automatic native publication/
pruning and browser web-latest replacement. Manual owner-repository requests may
only prepare unique review drafts; Forerunner packages retain development updater
identity, without inherited signing/update manifests. Map-index branch replacement
is disabled; the inherited container publisher already restricts itself to OpenCE.
All native-workflow jobs now wait for its existing commit gate, which admits
master/main and release-* history; packaged jobs use the same selected source.
Five focused guard checks pass. Two inherited cache-policy checks also fail on
the unmodified baseline; no cache framework rewrite was added. Shell gate execution
and actual multi-platform CI packaging remain unverified on this Windows host.
Local `master` still tracks `origin/main`; no branch/remote was changed. Spark's
existing build/release workflows were read only. No CI, draft, release, signing
key, tag, registry, map index or update channel was created/activated remotely.

The owner's Development & Verification Directive supersedes the earlier source-only
restriction. Implementation remains primary; use focused builds/tests after
meaningful changes, isolated native smoke checks for material runtime changes,
and multiplayer checks at integration milestones. Fix regressions before adding
dependent systems. Full playthroughs, stress tests, VR hardware/MegaWorld acceptance
and release certification remain final-phase gates. Historical deferred/unverified
entries below retain their evidence limits; authorization alone verifies nothing.

The latest Engineering Correction prioritizes the complete multiplayer Extraction
loop (authority, loot ownership, persistence, loss, escape and redeployment).
Keep one primary deliverable in progress and finish connected work before switching,
unless genuinely blocked. Firefight and the connected
19-region MegaWorld as permanent flagship requirements alongside Firefight,
VR and the other approved runtime work. Keep the anti-overengineering rules;
these additions do not authorize unrelated rewrites or duplicate editors.

The later [Persistent Spartan Operations directive](docs/forerunner/PERSISTENT_SPARTAN_OPERATIONS_2026-11-14.md)
is preserved in full. First operations acceptance is Spark's Operation Black Echo,
then independent squads (maximum six members), cooperative/hostile alliances,
bounded server-owned quests, downed-alive biofoam revival, actual-death looting,
scoped mission audio and reconnect-safe persistent rewards. Native HaloScript,
co-op, gear, AI and audio remain owners; only necessary per-squad extension state
is authorized. Spark's reviewed quest export/Black Echo package was not found in
the inspected current source; exact handoff requirements are in the existing
contract. Optional native living incapacitation/held-Action biofoam is now
source-implemented using the existing Extraction, native damage/control and
negotiated Delta object-sync paths. Actual death, native physics and gear loss
retain their existing owners; no corpse resurrection, quest VM or second bank
was added. Native factions are not independent squads. Complete operations and
concurrent connected regions remain unaccepted. Continue independent
engine work while that authoring handoff is missing.

Extraction acceptance requires an original Marathon-inspired hub/menu, inventory
and loadouts, deployment, AI with optional PvPvE, collectible loot, an interactable
extraction point, successful escape, death/loss, durable rewards, a post-run report
and redeployment. There is no mandatory raid timer; activation countdowns are
allowed. Complete one genuinely playable map before expanding that loop across
MegaWorld. Source presence is not gameplay acceptance.

MegaWorld acceptance requires all 19 original multiplayer regions connected by
natural bidirectional walk/drive/combat routes, with no boundary teleports or
map-loading screens. Preserve players, vehicles, weapons, inventory, AI and
mission state; allow multiplayer occupants and synchronized modes in different
regions. Order: two-region player/Warthog traversal, preserved simulation/network
state, bounded multi-region resource residency, all 19 regions, then world-wide
timerless Extraction. Do not load 19 whole caches or remove unrelated limits.
Spark owns geometry/layout; Forerunner owns residency, simulation and replication.
Stock Xbox support is not implied. Every stage remains unverified until gameplay.
The updated complete goal is in the user's attachment
`C:\Users\patri\.codex\attachments\767bbb4b-5e23-4cb8-91e5-4ba24b240624\pasted-text-1.txt`.
It retains the current foundation and makes Windows Vive/Knuckles VR the next
major milestone after playable multiplayer Firefight, followed by native Forge/Spark.
The repaired Fractum foundation is selected for the Windows x86 baseline:
four native OpenCE/Forerunner movement sessions pass across Blood Gulch and
Beaver Creek, with each runtime hosting. Scripted combat sessions also pass
both directions. The shader/loading crash is repaired; hidden OpenGL gameplay
renders and exits cleanly. These bounded automated results do not establish
complete campaign/match, human, VR, or cross-platform acceptance.
Snapshot 005 fixes a real campaign-host bug: the zero-valued co-op variant was
ignored and stale Slayer initialization asserted on campaign data. B40 now passes
92-second mixed-peer sessions in both hosting directions, and native actor readback
confirms nine moving units, seven damaged units and six deaths on both peers.
A10 no longer asserts, but its automated opening yields no on-foot acceptance.
The first B40 Firefight slice now completes three waves with two rendered native
clients: six real enemies move and die on both peers, team scores reach
200/400/600, both receive victory and return to pregame. Actor and player
comparisons pass. Inputs were automated; full Faber and human acceptance remain open.

## Work and acceptance

| Item | Implementation / verification | Remaining gate |
| --- | --- | --- |
| Foundation comparison | Exact histories inspected; repaired Windows x86 baseline compiled; 4/4 mixed-peer movement sessions pass | Campaign/co-op, complete matches and wider platform regression |
| Upstream Workshop filename validation | Imported production parser, upstream tests and fuzz regressions in `b2bd6043`; integrated build and restored suite pass | Broader map/gameplay coverage |
| Workshop test coverage | Removed inherited blanket skip; repaired disabled-folder scan; 135 tests pass, 8 skip; actual parser/filesystem code compiled | Non-Windows platform coverage |
| Windows native comparison tool | Owned process trees/private copies; tests and real swapped-host sessions pass | Wider modes, transitions, physical machines |
| Isolated Windows build | Tracked-source export, source/tool identity, MSVC/SDK discovery implemented; isolation tests and integrated compilation pass | Other platforms and release pipeline |
| OpenGL loading/rendering | Desktop shader prologue and immediate draw declaration repaired; native gameplay captures and clean exit | Human play, performance, audio and other GPUs/backends |
| Firefight / Forge | Faber-derived actor spawning/pursuit and finite three-wave B40 encounter pass native two-client combat; Forge not integrated | Human play, creator map, full HUD/settings/lives, late join/restart and remaining Faber features |
| Extraction / AI Zombies | No native integration completed | Complete authoritative gameplay loops and persistence tests |
| OpenH1 VR | Shared Windows64 OpenXR/Vulkan device, eye targets and native stereo camera/FOV traversal connected in source, unverified | Headset menus, tracked gameplay input/physical interactions and hardware + desktop-peer combat |
| Spark / MegaWorld | Version-1 read-only discovery and reference client tested against real engine; no editing/world traversal | End-to-end editing and two-way player/vehicle traversal |
| Releases | No public release or push performed | Supported-platform, installer, regression and human acceptance |

## Workspace and data ownership

Research clones, portable tools, build outputs, copied maps and test logs are outside
the source tree at `C:\Users\patri\AppData\Local\Temp\forerunner-20261114`.
They belong to this engineering run. They are not committed or distributed.
Do not remove an active build, a unique source edit, an original input or a
previous-good runtime as cleanup.

The user authorized copying data from Guilty Spark. Xbox map copies came from
`D:\Project Guilty Spark\Runtimes\OpenCE\maps`; input hashes were checked against
the copies. The editor repository and original runtimes were not changed.
Only test processes started by this run may be stopped.

## Exact next action

Object-sync source correction: native creation excludes scenery. `forge_clone`
now refuses host scenery copies before allocation/quota changes, avoiding host-only
geometry/collision; local scenery and the existing scale refusal remain. Dynamic
scenery/scale needs a negotiated extension: `object_sync` covers vitals/names,
not these properties. No wire/capability number was repurposed. No tests/builds/
queries/games ran; refusal, local cloning and peer compatibility remain unverified.

Independent co-op/campaign source repair: network mission completion now credits
every non-quit local participant on the host, using the hosted map's campaign
identity. Previously it credited only local player zero through the last solo-menu
selection, which may name another mission. A new explicit-level entrypoint reuses
the existing level/difficulty guards and native profile writer; solo callers retain
their existing wrapper. Finite Firefight/MCC completion still takes its earlier
branches, and unknown CE missions are not written into stock campaign flags.
No tests/builds/queries/games or profile writes ran. This is unverified source;
split-screen/dead/quit/absent local players, stale solo selection and difficulty
credit remain native gates. Remote-client profile credit needs an explicit
authoritative completed-mission/difficulty signal distinct from loss/abort/lobby
reset; no new wire message or inferred victory was introduced here.

Independent Forge source increment: `forge_clone` is an appended, console-only
native object operation based on Faber's `port/forge/src/forge_3d_mode.c` placement
recipe at `e4ffe0fe9874870b4fca1fca76beb83514c793f4`. It returns an unnamed fresh
scenery/weapon/equipment/empty-vehicle copy with placement/variant/colors/team/scale,
using existing host-cheat gates, console quota and Xbox index limits. Attached or
dead objects, bipeds and objective weapons are refused. Native creation already
carries variant/colors but not scale, so scaled network copies are refused rather
than silently diverging. Existing script indices are preserved by appending; map
admission forbids the new call. No new packet, tag injection, UI, save/undo system,
Spark write API or file modification is introduced. Full Faber Forge still needs
its runtime-tag/editor/network integration; native scale transport is a separate
negotiated feature. No tests/builds/queries/games ran. This is unverified runtime
source, not accepted Forge or multiplayer compatibility.

The owner's new permanent goal explicitly authorizes independently actionable
source work across the full engine while tests/native execution remain deferred.
Independent AI/campaign increment: optional `game.solo_skulls` reuses the existing
Chupathingy/OpenCE MCC skull parser and 17 implemented gameplay hooks for native
local solo content, including CE. Empty is stock; network, multiplayer-scenario,
editor and MCC ownership remain excluded. Map start/restart initializes before
actor placement; disposal clears local state. Bandanna uses the existing ammo
helper. Unsupported names warn and are skipped. No parallel skull system, packet,
scoring multiplier or Spark rules importer was added. No tests/builds/queries/games
ran; new functionality is unverified. Native Firefight, creator-map, network-skull
and physical-VR acceptance remain open. AGENTS.md was not modified.

Resumed source development: Spark `3329a4a5ab12b6f02c75b82e054f75b0896f1e2a`
now has receipt-backed owned CE609 solo staging and an exact Forerunner profile;
its current admitted binary still uses native map selection. Forerunner now keeps
an explicit `game.start_map` request while a fresh private engine profile is created
in normal menus, then starts it once in the same process. Validation is retained
without per-frame cache reads/log spam. Desktop discovery adds optional local
startup metadata, with no new operation, launcher, network traffic or map-loaded
claim. Offline campaign startup cannot run the network-host-only finite director;
Spark also explicitly sets it OFF. Existing custom-solo completion and same-revision
Restart Level remain native. No tests/builds/queries/games ran; all new source is
unverified. The contract names Spark's exact next identity/parser/environment
changes. Continue independent Firefight source work; do not wait for a finished
Zombie map or call the overall goal complete/stalled on that content dependency.

Follow-up source repair applies the existing Firefight spawn-eligibility predicate
to network co-op respawn iteration and per-player retry scheduling. Eliminated
players without an allowance/grant no longer repeatedly request a refused spawn;
paid final-life and failed funded spawns still retry through existing paths.
Ordinary co-op remains allowed by that predicate. No timers, grants, packets or
spectator subsystem were added. No tests/builds/queries/games ran. Shared-life
exhaustion, funded/unfunded simultaneous deaths, placement failure and terminal
lobby transitions remain unverified. Private CE startup is preserved in
`018c67313`; continue from this source rather than waiting for Spark content.

Owner-requested companion check-in, source only: Spark is now at
`9bfb746549f93363e60572423438c7dd59d774ee`, with six dirty controller/catalog/UI
files preserved. It has bounded encounter/squad authoring and reviewed appended
script-module source, plus unfinished exact-encounter controller bindings. The
recovered-source writer/attachment still targets MCC/H1A; CE source admission and
identity-reviewed Forerunner private launch remain the next connection. The
existing `SPARK_INTEGRATION_CONTRACT.md` records the updated source findings and
ownership. No Spark files were changed and no tests/builds/queries/games ran.
Forerunner was inspected on root master at `d831ee13d`; the pending generator,
binding and generated-menu edits were preserved, unbuilt and untested. That cycle
produces integration requirements, not new gameplay or acceptance evidence.

Current source increment exposes finite Firefight OFF/ON through Server Setup →
Co-op Options, binding `network.firefight_encounter` through existing settings
init/save/cancel and row fitting. Only the existing server-settings XML was
regenerated from `tools/port_settings.py`; no new menu system or runtime controller.
Help keeps authored map waves separate. No tests/builds/queries/games ran. Native
layout, mouse/controller navigation, save/cancel persistence and normal hosted
encounter acceptance remain deferred. This cycle adds a gameplay entry control,
not gameplay acceptance; Spark CE content/launch and the synchronized creator-map
gate remain open.

Current source repair holds finite wave transitions while the existing
three-second unfunded team-elimination check is pending. Previously the final
wave's clear countdown could finish before that later wipe deadline and announce
victory with no living player or paid respawn. A live player/paid grant clears the
wait; an expired wipe uses the existing defeat/arena-return path. No new timer or
protocol was added. No tests/builds/queries/games ran. Final-clear versus last-player
death timing, funded respawn recovery and ordinary wave progression remain native
acceptance gates. This is a necessary gameplay repair in source, not accepted play.

Actor-path source audit at `d85d7929c`: finite spawning passes the exact selected
variant to native `actor_customize_unit`/`actor_create_for_unit`, matching Faber's
creation parameters while retaining real swarm attachment. Native `actor_combat.c`
still applies campaign difficulty values. Existing departure code detaches/deletes
or kills departing units. No additional rank/difficulty/leave patch was justified.
This cycle supplies source evidence only, with no runtime functionality added and
no tests/builds/queries/games. Next integration evidence still needs Spark's reviewed
CE candidate/launch route and authorized native execution; the broader HUD, creator
map, synchronized gameplay and hardware acceptance requirements remain open.

The owner selected Zombie Apocalypse as the authored Firefight candidate and asked
to use Spark's conversion workflow. The existing integration contract now records
the concrete Spark handoff, reusable code, ownership and missing engine operations.
Spark was inspected read-only at `b57cb654`; its unfinished work was not changed.
No Forerunner importer for Spark Firefight JSON exists, and no converted candidate
was produced by this coordination update. Do not relabel the B40 proof as Zombie play.

Code-only work is now requested: no new tests/builds/native runs until the owner
resumes execution. Shared-life/defeat source is preserved in `c8196f81`: real death
accounting, host-only respawn gate and no campaign-state rewind on a Firefight wipe.
Source checkpoint `2e9c17e57` selects an exact loaded actor variant and optional named
native squad spawn, preserves faction/native swarm attachment, refuses blocked
placement and excludes feigned Flood deaths from wave credit. It removes the
actor-count-limited setup removal list in favor of native body iteration. These
changes are unbuilt/untested; prior B40 evidence does not cover them. Existing small
harness edits predate the no-tests instruction and were not run.

Source checkpoint `1e791b589` records exact tracked wave deaths in native final
`unit_died`, adapting Faber's kill notification. Destruction damage can delete a
body before the director's polling tick; a recorded death now survives that removal.
Feigned deaths, duplicate notifications, clients and untracked/live deletions do
not award points. `actors_spawn_from_unit` also now reads its selected swarm or
individual binding, fixing the original wrong-binding lookup. Neither repair has
been built or tested. The original Zombie physics crash is not claimed repaired.

The requested Spark check-in inspected source read-only at `1f16760`, plus its
unfinished Firefight source-reconciliation edits. Spark now has unverified installed
Forerunner registration/discovery and prepared native-round source export. Its
OpenCE build preflight and exact-hash shared-data launch policy still block a
direct authored Forerunner loop. The existing contract records the exact modules,
path settings, CE scenario route and next handoff; no extra coordination system
was added. No Spark file, original game data, remote or branch was changed.

Continue the actual Zombie actor/spawner and authored encounter lifecycle in
Forerunner; Spark owns reviewed conversion/authoring and CE build receipt routing.
Source checkpoint `5bd98b76f` adapts Faber's Outbreak encounterless-child gate for
tracked Firefight parents only. Native children join the existing wave table,
bounded to Faber's 32 slots total per wave, with the same pursuit/final-death path.
Wave completion waits for a dead carrier's pending native spawn and all actual
children; partial, unsupported or over-capacity creation aborts explicitly. This
is unbuilt/untested source. Normal object deletion was inspected and is deferred;
no speculative destruction-order patch was applied and no physics-crash fix claimed.
Source checkpoint `19db0047d` sends a newly loaded client the host's current
wave/phase, remaining enemies, pending carrier spawns, team score and shared lives
through the existing reliable notice path. It retains the terminal outcome for a
join during victory/defeat/abort. Only the existing first-ready branch sends this;
repeated object-resync requests do not repeat it. This is unbuilt/untested text
presentation, not full late-join AI/gameplay or a structured Firefight HUD packet.
Source checkpoint `1cf570365` makes a full world revert restart the finite encounter
with wave/score/timers/handles cleared and its map-start life allowance restored.
Actor/spawn settings remain frozen for that map. Respawn-only Firefight wipes do
not call this reset and continue preserving progress/spent lives. Preparation also
retires old pending hostile carrier bursts from a restored world, without score.
Source review only; no tests/builds/native runs and no new recovery framework.
Source checkpoint `f96bf1d23` adds optional desktop `game.start_map` /
`HALO_START_MAP`: one installed CE solo cache selected once through the normal
campaign/profile/loader path, fresh instead of an older saved checkpoint. It
checks native cache kind/resources and refuses competing startups/sessions;
failure leaves normal menus available. This supplies Spark's missing source-side
exact-map selection without a new launcher, RPC or rules importer. It is unbuilt/
untested and absent from accepted binaries; Spark's build/staging identity and
shared-folder integration remain its responsibility. Native HaloScript owns the
authored solo encounter; keep the separate finite co-op director off for that path.
The original Demo multiplayer map, missing original audio, carrier gameplay,
multi-group waves, full HUD and creator-map/two-peer acceptance remain open. Do not
call the actor selector or the accepted B40 proof a completed Zombie encounter.

The companion follow-up inspected Spark read-only at
`20acb45fb84bd4789732a5f99c41c6a0acaafadb`, including its current native encounter
inspection/layout-readback edits. Its actor/weapon closure inclusion, exact native
round-layout checks and retained-build readback can feed Forerunner's existing CE
loader without duplicating authoring or simulation. Spark still owes a preserving
recovered-scenario append writer plus reviewed CE receipt-to-Forerunner launch
routing; OpenCE build preflight and the exact-executable shared-folder policy
remain closed. The existing integration contract records these concrete next steps.
Forerunner remains the runtime/playtest companion; the initial solo content gate
does not accept synchronized gameplay. Only these Forerunner documentation files
were changed in this follow-up. No tests, builds, queries, games, Spark writes or
remote/branch changes were performed.

Source checkpoint `e16b0f20f` adapts Faber's tracked AI kill credit to
the existing native player kill counter. The host retains each tracked body's last
actual damaging owner, including NONE for AI/environmental/no-statistics damage;
only the existing final-death callback awards once. Initial bodies and native
children start uncredited, full player handles reject departed/reused players,
and the short counter saturates rather than wrapping. Existing statistics
replication carries the change; team points remain unchanged. No tests, builds,
runtime queries or games ran. Individual point totals, assists, medals and HUD
integration remain open. This cycle adds gameplay source, not infrastructure.

Source checkpoint `d2fec18a6` removes the finite director's unnecessary
two-player start gate. One network co-op host may start; zero players, an opening
cinematic or any present player dead/unspawned/in a vehicle still waits. Ordinary
solo HaloScript playback is separate and the Demo multiplayer scenario remains
unsupported. Static review did not establish the retained Zombie crash's cause:
normal deletion is deferred and ordinary object allocation does not trigger GC.
No speculative physics/lifetime patch was made. No tests/builds/queries/games ran;
solo and two-peer gameplay acceptance remain open.

Source checkpoint `4266cbbfe` reuses the late-join status formatter
for changed combat counts: wave, remaining tracked enemies, pending carrier
spawns, team points and shared lives. The host samples at most once per second
and sends only changed text through existing local HUD/reliable peer notices.
Wave clear/terminal messages keep their existing path; new maps/full reverts
clear the notice cache. No new packet, capability, renderer or persistent HUD was
added. No tests/builds/queries/games ran; visual and peer acceptance remain open.

Source checkpoint `ce9bf1cf0` retains the respawn bought by the final shared life.
Death-time charging now reserves one spawn against the full player handle; only
a successful owned live spawn clears it. Per-player gates reject unfunded deaths,
while the team-wipe/defeat gates allow pending paid spawns at zero. Failed all-dead
spawns retry through the existing death timer; quit/reused slots cannot transfer
grants. New maps/full world reverts clear them. This fixes a source-visible
off-by-one and early-defeat path; it adds no packet or recovery subsystem.
No tests/builds/queries/games ran. Final-life solo/two-player deaths, failed spawn,
quit/slot reuse and later unfunded defeat still require native acceptance.

Source checkpoint `4a40f576b` removes a dead tracked parent from the native
garbage list while its animation still owes a child-spawn attempt. Native
`unit_test_spawning` releases that hold after setting its completion flag; actual
children continue through the existing tracker. This prevents automatic corpse
collection from silently cancelling the pending burst and clearing the wave.
Direct native destruction and zero-child rolls remain native; no synthetic spawn,
new tracking table or timeout was added. No tests/builds/queries/games ran.
Memory-pressure, carrier-child and creator-map gameplay remain unaccepted; the
historical Zombie damage/physics crash is not claimed repaired.

The retained C47 receipt gives nearest crash symbols, not a proven cause. Spark's
current `docs/SOURCE_ONLY_STORAGE_POLICY.md` records retirement of old raw QA
collections; the corresponding raw crash/symbol files were not found in the known
roots. Do not recreate that retired environment or infer a physics fix from the
receipt alone. A later authorized native run needs a fresh exact failure record.
Source checkpoint `2938c5798` adapts Faber's actor/unit reference prerequisites to
`actors_spawn_from_unit`: incomplete variants report their path and return no
actors; tracked Firefight requests also abort through the existing failure path.
The current empty-tag fallback already limits unsafe reads, so this is explicit
spawn rejection, not a claim to repair the old crash. No tags, originals or Spark
files were changed. No tests/builds/queries/games ran; native acceptance is open.

Source checkpoint `67a10a144` adapts Faber's reserve-ammunition refill
to an optional finite resupply before waves two and three. The existing module
refills only living players' carried weapons, bounded by native magazine capacity;
battery weapons regain charge. It preserves loaded rounds, reload state, heat,
weapons and grenade counts. `network.firefight_resupply` defaults off and freezes
at map start. Existing inventory snapshots carry reserves/age; no packet, inventory
framework, equipment spawning or infinite-ammo trait was added. No tests/builds/
queries/games ran. Reserve/battery behavior, reload timing, owner-client agreement
and creator-map acceptance remain open.

Source checkpoint `7f3cd6fbd` moves the existing finite Firefight
arena-return guard ahead of both campaign-completion paths. It previously followed
MCC package completion, whose `mcc_progress_complete` could record a mission win
and select the next mission even on a Firefight defeat/abort. The existing host
co-op lobby return now runs first; ordinary campaign order is unchanged. No new
result packet or transition mechanism was added. No tests/builds/queries/games or
profile writes ran. Terminal/lobby and MCC progress protection remain unverified
in gameplay; this does not establish MCC Firefight acceptance.

Companion milestone review reached Spark `3e47ad0403f1b4ea7023286fedb972018d861c9f`
plus its unfinished count/grid/native-operation UI edits, read-only. A preserving
single-variant/squad append now exists in source; its actual working-tag entrypoint
still requires `mcc-cea`. The original Zombie CE route, recovered multi-group
controller attachment and reviewed Forerunner launch routing remain incomplete.
The existing contract now points to the precise append/admission code and the
existing CE-aware owned-source checks to reuse. No additional engine API was
justified; no duplicate writer or simulation was added here. This cycle produces
integration evidence and requirements, not new runtime functionality. No tests,
builds, queries, games, Spark writes or remote/branch changes were performed.

Authored-solo source audit: CE suffixes are excluded from Xbox campaign identity
and profile completion already checks level/difficulty bounds. Fresh startup
skips disk checkpoints; native Restart Level resets the loaded world rather than
loading a revised cache. Spark revision acceptance therefore needs a fresh owned
process. Its current native controller's private score/result globals do not
provide human HUD/outcome/replay controls automatically. The existing contract
records those remaining content requirements. No engine bridge, arbitrary global
reader or extra source patch was justified. No tests/builds/queries/games ran;
the next executable gate remains deferred and Spark's reviewed CE controller/
launch handoff remains incomplete. Broader milestones are not substituted for it.

Approved VR source cycle: the owner explicitly authorized one optional Windows64
OpenXR implementation for Vive/Index Knuckles and Quest 2 PCVR, then selected
performance and reuse of existing runtimes. The implementation targets the
existing bgfx Vulkan renderer rather than a second renderer or an OpenGL-only
VR branch. `win32_openxr.c` adapts pinned OpenH1 device-creation callbacks,
session/frame lifecycle, pose/action reads, haptics and runtime-owned eye images.
Touch and Index profiles share actions. The existing bgfx patch path connects
runtime-required device creation and headset GPU selection; `--with-openxr`
and off-by-default `vr.enabled` keep ordinary builds/flat-screen selection.
Khronos header provenance remains pinned in the existing Windows build script.
This is necessary VR source progress, not working headset gameplay or a measured
performance result. Next: wire borrowed eye images and Vulkan synchronization
into the existing renderer, stereo camera/FOV rendering and tracked game input,
then adapt compatible OpenH1 physical interactions without changing simulation
or silently changing the network protocol. Session events/frame APIs have no
game-loop caller yet. All new functionality is UNVERIFIED. No configure, header
download, compiler, tests, native query, game, headset session, Spark mutation,
remote change or push ran. The full engine goal remains active.

Next VR source cycle adds the renderer eye adapter in the existing
`d3d8_bgfx.inl`: acquire/import runtime-owned Vulkan textures with bgfx's native
external-image argument, redirect the existing back buffer, scale screen/depth
intermediates to each eye, mirror the right eye, then release/end only after
same-thread bgfx frame submission. Its queue patch skips unacquired cached XR
images and returns acquired ones in COLOR_ATTACHMENT_OPTIMAL. Headset sessions
disable desktop vsync/hidden-window pacing; ordinary flat-screen paths retain
their settings. No new renderer, CPU image copy or per-eye device-idle wait was
introduced. The device patch was regenerated from exact pinned raw source after
inspection exposed whitespace differences in the earlier web representation.
Patch application itself remains unverified. Unused native format/size getters
were removed in favor of the single eye-image metadata interface. This cycle is
necessary runtime source progress; the game-side stereo camera/FOV caller,
headset menus, full pacing/input/interaction wiring and hardware acceptance
remain open. No tests, configure, builds, patch application, launches, Spark
changes or pushes ran. All new functionality remains UNVERIFIED.

Stereo caller source cycle connects `vr_render.c` to the existing native
`render_frame` traversal: two eye cameras and their interface views share one
native frame, retain the actual local-player index and use OpenH1-derived pose
axes/metre scale/asymmetric frustum normalization. Native camera structs,
simulated players, 30 Hz game time and network payloads are unchanged. Local
reference/session changes reset the render anchor. Active XR bypasses desktop
frame caps and legacy render throttling; menu/non-stereo callers pump a
zero-layer XR frame. A conditional Vulkan queue hook flushes pending resource
commands and drops the projection when a resize skips native submission,
without adding a per-eye GPU wait. All source remains UNVERIFIED; native
patch application, build, render, hardware, color/occlusion, camera collision,
headset menu and controller/physical interaction acceptance remain open. No
tests, configure, builds, launches, native queries, Spark mutations or pushes.

Mota subsequently supplied `SutlabSol/OpenCE-Forge` main, now pinned at
`2467e708b67bdab994b4f3725516a72e31f629b3` in the existing upstream matrix and
Spark contract. Its actual text-sidecar reader/writer and stock-versus-Forge
session protocol are identified; no competing format or reader was invented.
The user also relayed per-player BSP and server-selected third-person biped
work as follow-up source-comparison leads; those reports are not acceptance
evidence and do not establish first-person arms or protocol compatibility.

Controller source cycle adapts OpenH1's basic Xbox mapping into existing
`xinput_sdl.c`: Touch/Index face buttons, sticks, triggers and grip bumpers
join native port zero, with two-stick-click Start for Index and Touch's menu
action. XR keeps runtime focus and merges after SDL's desktop-focused remapper,
before existing guide/chat/capture ownership. Hints read one combined raw pad
snapshot; keyboard/SDL fallback stays merged. Fresh actions reuse the latest
located poses, with focus/stale/tracking neutralization and no second pose
query set. Existing rumble feeds both hands and zero feedback stops haptics.
This is necessary runtime source progress, UNVERIFIED. No tests, configure,
builds, native queries, launches, Spark changes or pushes. Headset menu rendering,
tracked weapon aim, physical interactions/collision, remote tracked poses and
both-headset multiplayer acceptance remain open; native simulation/wire format
are unchanged. Advanced SDL remap editing remains SDL-owned.

Menu source cycle reuses OpenH1's head-locked OpenXR quad approach on the
existing left-eye swapchain. Native menu ownership selects the ordinary single
game render; the current bgfx blit and overlay compose once into that image,
then a static full-quad copy mirrors the completed canvas to the desktop.
Desktop mouse/layout coordinates and physical-image cutouts are kept aligned.
XR menu submission does not depend on the desktop being due for presentation.
No additional renderer, swapchain, protocol or editor interface was introduced.
This is necessary runtime source progress, UNVERIFIED. No tests, configure,
builds, patch application, native queries, launches, Spark changes or pushes.
Headset menu appearance/comfort, ownership transitions, resize/loss and both
headsets still need authorized validation. Tracked weapon aim, physical
interactions/collision and remote tracked poses remain unfinished.

Replay source cycle extends the existing HREC object records to non-player
bipeds and their corpses, with an optional dead-state flag in the existing flags
byte. The viewer shows these unit tracks and death labels using its current
placeholder/local-map model path. Attached object snapshots now use native world
origin/orientation helpers; change detection and serialized positions agree.
No record size or network protocol changed. This is necessary runtime source
progress, UNVERIFIED; no tests, builds, launches, Spark changes or pushes. This
does not add AI animation/weapon/action history or native in-engine Theater.

Extraction source cycle adds a small native gameplay module using existing map
controls, trigger volumes and simulation ticks. The opt-in local-only path binds
live deployment/activation controls, records the full player/unit handles, runs
only a cancellable activation countdown, and rejects death/handle replacement or
checkpoint rewind. Readiness is not evacuation; loot, persistence and the full
hub/report/redeployment loop remain open. This is runtime source progress,
UNVERIFIED: no tests, builds, launches, Spark mutations or pushes. The updated
Spark contract records exact current bindings and the two-region MegaWorld
handoff, source identities and outstanding residency/replication requirements.

Voice is also a permanent integration requirement: preserve inherited proximity
chat including enemies, team/party radio across regions, range/PTT/mute/volume
controls and user-controlled microphone privacy. Review Windows headset capture
and Halo spatial playback before adding features. Connected cave/tunnel audio
must use the real world coordinate/portal context; no second VOIP stack or
unnecessary legacy packet change. Source review and hardware/gameplay acceptance
are separate; no headset microphone or multi-region audio pass is claimed.

Owner-added shared inventory requirement: Zombie Apocalypse Firefight Survival
and Combat Evolved Extraction must share loot/equipment/persistence, compatible
cross-mode recovered gear and pre-run loadouts. Include weapons, ammo, armor,
equipment, rarity, capacity and upgrades. Firefight recovery uses evacuation or
defined survival milestones; Extraction recovery uses successful extraction into
a personal stash, with meaningful configurable death loss and no mandatory raid
timer. Multiplayer ownership/settlement must be authoritative. Durable saves must
survive crashes and prevent duplicate awards, loadout duplication and unintended
overwrites. Reuse native/Faber inventory and existing profile storage before
adding anything else. These requirements are open; no persistent gear is claimed.

Voice source cycle reviewed inherited capture, Delta host routing, party hooks
and Halo playback. Native proximity includes enemies; in-session team/party radio,
PTT/mutes/volumes and named Windows device selection are connected in source.
Cross-session party transport has no caller; OpenCE/Delta bridging remains planned;
portal occlusion, region contexts and XR tracked-head listener integration are open.
Repaired explicit microphone selection: no silent system-device fallback, and
mode/device changes close capture, clear queues/gate and reset codec history.
Permission denials stay denied; capture remains off by default and opens only when
talking is authorized. Necessary bug fix, UNVERIFIED. No packet change, duplicate
VOIP stack, tests, builds, audio/runtime queries, launches, Spark edits or pushes.

Owner-prioritized roadmap continues native Linux alongside Windows/VR. Rendering
and lighting must consume Spark's target-compatible GPU-baked native content
within each target's real limits. Zombie Apocalypse must become distinct Flood
survival with outbreaks, changing objectives, scavenging, optional extraction,
persistent rewards and scalable hordes, reusing shared systems. Generic campaign
co-op/three-wave Firefight is not that acceptance. Genuine 19-region play and VR
remain flagship requirements; 128 players and huge Flood populations are targets
requiring eventual performance/replication proof, not established capabilities.

Shared equipment source cycle reuses `game_modes.c` for host/local native gear
capture (paths, slots, magazines, battery age, grenades and map provenance), with
ownership/death/duplicate checks and objective exclusion. Extraction now requires
an authored physical return volume after signal activation, records its carried
loadout once on arrival, reports it and permits redeployment using the same live
player/inventory. Firefight uses the same reader for per-owner wave-clear reports
before optional resupply. No gear is minted, restored, awarded or written to a
stash. This is necessary runtime source progress, UNVERIFIED; no builds, tests,
launches, Spark changes or pushes. Durable settlement/reservations, crash and
cross-process protection, stash UI, rarity/upgrades and multiplayer Extraction
remain open. Shared game code introduces no Windows-only inventory dependency,
packet change, global limit increase or new storage framework.

Positional-audio source cycle repairs the inherited VOIP coordinate boundary:
Halo's native sound listener transforms world sources before its canonical-origin
DirectSound mixer. Proximity voice now uses that same matrix, is positioned before
mixing, and stays silent while unlocated; radio remains non-positional. Optional
XR sound uses the midpoint/orientation of actual rendered eyes, once per frame
after camera preparation, with native acoustic-location lookup and stale/invalid
fallback. Observer state, tick velocity, simulation and wire formats are unchanged.
This is necessary runtime source progress, UNVERIFIED. No tests, builds, audio
queries, launches, headset sessions, Spark edits or pushes. Direction/gain, latency,
head motion, tracking loss and both headset/controller families need validation;
voice portal occlusion and multi-region audio contexts remain unfinished.

Cross-project source/ownership cycle follows the owner's explicit full-roadmap
AGENTS reconciliation; safety, source-only, attribution and approval rules remain.
Spark was inspected read-only at `d56354bee4813ea3a311afe0ac48a9b378505c3a`,
with its three unfinished GPU-draft/view files left untouched. Existing native
round/profile verification, runtime bindings, world assembly and light draft
responsibilities are compared with Forerunner and pinned upstream implementations
in the Spark contract; no editor code or preview director was copied into runtime.

Critical Firefight repair removes the blanket `hs_update` pause, preserving host
doors/triggers/objectives and existing client co-op authority. The optional finite
director checks Spark's actual compiled `gs_firefight`, `gs_ff_reset` and
`gs_ff_bootstrap` bindings before taking control, then disables itself if any is
present. Presence is not cache validation or generic controller detection; other
authored controllers still require the finite setting OFF. Finite result ownership
also prevents a now-running campaign script from bypassing its waves/result notice.
All other active/lives/death/spawn callers were reviewed and retained. Necessary
runtime bug fix, UNVERIFIED: no builds, tests, launches, Spark mutations or pushes.
Historical B40 evidence predates restored ambient mission scripting and does not
prove the current finite encounter or Spark native round. Durable loot, authored
Extraction, region residency and remaining full-engine gates stay open.

Inventory precondition source cycle reviewed pinned Faber `e4ffe0fe` profile
stats/identity/writer code before introducing storage. Its in-place Win32 text
writer and stats records do not supply portable atomic item ownership or run
settlement; no writer, identity secret or new storage framework was imported.
Reused the native active saved-profile getter to bind Extraction's run owner.

Necessary repairs: native checkpoint/disk/core before-load callbacks now clear
Extraction state explicitly, rather than relying solely on tick rewind. Profile
or controller-owner changes invalidate an active run, and failed native equipment
capture can no longer publish RUN COMPLETE or enable redeployment as a success.
The pending return retries without repeated failure messages or gear mutation.
UNVERIFIED; source/callers/diffs only, with no builds, tests, launches, user-save
access, Spark mutations or pushes. Durable reservations/item identity/settlement,
cross-mode stash/loadouts and authenticated multiplayer ownership remain open;
these safeguards do not establish persistence or duplication-exploit acceptance.

UNVERIFIED OpenH1 reuse cycle: optional Windows64 `vr.aim = "controller"`
connects cached right-hand aim to existing native first-person on-foot facing,
with native-eye convergence, separate body heading and body-relative throttle.
The default remains `"gamepad"`. Existing input/camera/menu/seat ownership gates,
map lifecycle and before-load callbacks release tracking; no new renderer,
network layout, source dependency or Spark schema was introduced. OpenH1 pin and
AGPL attribution remain in source and the Spark contract. Native Linux paths
remain outside HALO_OPENXR. This cycle is runtime functionality source, not
accepted gameplay; no tests, builds or launches. Tracked weapon rendering,
reticles, physical interactions, vehicle aiming and hardware proof remain open.

UNVERIFIED Extraction outcome repair: admitted native hub/run maps now own
completion through extraction_owns_map, including a deferred main-loop recheck
for first-tick HaloScript wins before binding. Campaign game_won cannot bypass
physical return/redeployment; general scripts and ordinary campaign completion
remain unchanged. This is a necessary gameplay repair, with no new subsystem.
Death recovery, durable settlement and combined finite-director outcomes remain
unfinished. Source/caller review only; no tests, builds or launches.

UNVERIFIED native voice reuse cycle: audio.voice_obstruction (off by default)
connects inherited Halo BSP sound/visibility/collision queries and millibel
conversion to existing Delta/Opus proximity playback and I3DL2 muffling. Radio
ignores the effect; microphone privacy and wire formats are unchanged. The
reused native query cache keys BSP identity and clears for maps/restores.
This is runtime source plus a necessary cache repair, with no second VOIP or
acoustic subsystem. Native Linux/Windows share the code. Only the current BSP
and native observer are covered; XR-origin and connected-region acoustics,
multiple local listeners and hardware behavior remain unfinished/unverified.
No builds, tests, launches or audio-device queries.

UNVERIFIED Extraction wipe recovery: the existing native player respawn loop
now shares one implementation for checkpoint relocation and authored starts.
Extraction uses native starts after a real local wipe, discards the failed
run/equipment result, and preserves the current world instead of reverting a
save. Native starting-equipment profiles remain authoritative; no recovered
gear is restored/granted. Failed allocation uses the existing loss timer for
retry. Live-player scripted failures and finite Firefight keep their existing
outcome owners. Spark must author all selectable solo starts in the safe hub,
with native equipment and collision clearance. No hub placement is verified.
This cycle adds gameplay recovery code, not a new respawn/storage system.
Persistent settlement, per-player multiplayer loss and combined director modes
remain open. Source/callers/diffs only; no tests, builds, launches or save access.

UNVERIFIED OpenH1 weapon-pose reuse: optional controller aim now supplies a
right-hand camera only while native first-person weapon nodes are built, then
restores the head camera for world rendering. Both eyes share that pose; native
models, animations and visibility rules remain authoritative. Reused OpenH1's
vr.weapon_offset setting/defaults with finite bounded parsing and kept headset
eye projection in control instead of flat-screen viewmodel FOV overrides.
This is runtime source plus a necessary stereo projection repair, with no new
renderer, game data, network fields or Linux dependency. Attribution/pin remains
in source and the Spark contract. Grip calibration, correct VR reticles,
standalone/two-hand/physical actions and vehicle aiming remain unfinished or
unverified. Source/caller/diff review only; no tests, builds or headset launches.

UNVERIFIED VR sight/aim source: reuse native weapon_get_projectile_ray (the
passenger preview path), native projectile collision and world-to-screen HUD
projection for one finite pre-spread target shared by both eyes. Scoped native
aiming sprites project per eye; warnings, meters and zoom images keep their
anchors. Invalid/behind/outside targets hide only aiming pieces. Flat reticle
FOV scaling is bypassed only inside that scope; world/fog FOV queries stay
unchanged. OpenH1's gamepad weapon-camera behavior now keeps the native body
pose before headset turning/leaning. No new HUD renderer, assets, simulation,
packets or Linux dependency. Source/caller/diff review only; no builds/tests or
launches. Reticle/grip alignment, vergence, physical scopes, vehicle aiming,
physical interactions and hardware performance/acceptance remain open.

UNVERIFIED settings protection: config_write_file honors SDL_FlushIO failure
before replacement, retaining its existing close/cleanup path. Pinned SDL header
and source review only; no tests, settings/save access or gear writer.

UNVERIFIED local inventory-owner repair: Extraction records the native saved
profile directory on deployment and compares it before publishing a return,
covering reuse of an enumeration slot. Missing return identity stays pending
with one-second retries; mismatched directory loses the result. No profile path
is logged/exported or gear written/granted. Source review also confirms native
CreateFileA ignores sharing, so it cannot supply a stash transaction lock.
This is a necessary ownership safeguard, not a new persistence subsystem.
Durable item/run identity, exclusivity and settlement remain open; no tests,
builds, launches or personal profile/save access.

UNVERIFIED Firefight survival source reuses the existing host director and
pinned Faber's zero-limit progression policy: map-start wave_limit defaults to
three, zero repeats until defeat/exit; initial_enemies defaults to two, valid
1..32. The shared 32-body/child tracker and native pools remain unchanged;
allocation/placement/capacity failure still aborts. Host counters saturate rather
than overflow. Dead bipeds no longer block spread as standing actors, while full
occupancy queries remain conservative. No corpse/loot deletion or new director.
Spark-native script ownership still wins. Network co-op authority and existing
status transport are unchanged. This is runtime source/necessary placement work,
not full Zombie Survival, local Extraction integration or large-horde proof.
Source/callers/diffs only; no tests, builds, launches, save access or pushes.

UNVERIFIED OpenH1 audio reuse: obstruction consumes the listener origin/cluster
actually applied by native audio. The existing XR adapter now shares eye-to-head
clearance and native-position fallback across solid geometry, retaining head
orientation; it retries changed poses/ticks/BSPs. Native datum/query caches key
BSP and native/head mode and clear with existing map/restore callbacks. Game
sounds and optional proximity voice share this path; radio, mic policy, native
observer/player state and packets are unchanged. No new audio/VOIP system.
This is runtime source/necessary cache work, not hardware or MegaWorld audio
acceptance. Source/callers/build-source gates reviewed only; no tests, builds,
launches, audio queries or save access. Multi-region acoustics remain open.

UNVERIFIED controller snap turning adapts pinned OpenH1's physical-right-stick
latch inside existing VR controller aim. Native smooth turning remains default;
optional snap steps use 0.7/0.3 thresholds and bounded 30-degree default settings.
Ownership, menu/focus/tracking, recenter and world changes require neutral before
rearming. Existing body heading, facing, renderer and packet paths are reused;
vehicles/gamepad aim remain native. Source/callers/diffs reviewed only; no tests,
builds, launches or hardware access. This is runtime source, not VR acceptance;
headset direction/comfort, bindings, mixed inputs and platform parity remain open.

UNVERIFIED Extraction handoff repair: read-only Spark f8213a5 now has native
terminal/volume authoring and touched-by-player source readback. Runtime binding
refuses duplicate markers, oversized short-index blocks, aliased terminals and
non-use control modes before claiming campaign outcomes. Reuses native controls
and volumes; no metadata importer or new subsystem. Spark still needs every
selectable hub start checked (currently any one suffices) and the compiled map.
Source/callers/diffs only; no tests, builds, launches, saves or pushes. This is
a necessary gameplay admission repair, not Extraction/persistence acceptance.

UNVERIFIED survival progression: optional host map-start wave_growth adds
initial enemies between cleared waves through the existing director/spawner.
Zero default retains fixed waves; 0..32 growth saturates at the current 32-body
tracker with bounded arithmetic even in endless runs. Native carrier children
still share capacity; placement/allocation/capacity failure aborts. Authored
HaloScript ownership, native pools/AI/replication and finite outcomes are kept.
This is runtime source, not complete Zombie outbreaks or huge-horde proof.
Source/callers/diff review only; no tests, builds, launches, saves or pushes.

Mota's latest lead refreshed OpenCE-Forge to 91099ce3 (2.10.11), 13 commits after
the old pin; source Forge protocol is now 0x4630 (README stale). UNVERIFIED reuse:
cherry-picked original OpenCE ebfa05b7 + slow-peer follow-up 796df195 with -x and
bnunu's authorship. Existing map sharing now uses eight host/two peer requests,
32 overall, 64-bit rate/allowance arithmetic and serving-frame catalog streams;
upload choices extend to 250 MB/s, default unchanged. The Forerunner private-join
status now uses the same rate helper, avoiding its remaining overflow. Existing
payload/hash/sandbox paths are retained; no new protocol or Forge tag admission.
Review/callers only; no tests, builds, launches, fetch, save access or pushes.
This is runtime performance/necessary arithmetic work. Source-only upstream
validation claims do not establish local throughput. Forge-specific tag fixes
and OpenCE campaign/cutscene source adoption remain independent next work;
file sharing is still unchanged and cannot yet supply an inventory bank lock.

UNVERIFIED campaign/co-op runtime progress: reused original ac152e44/8c70e633/
511da10e/8903981f/958cd833 with -x and bnunu/chestahh attribution. Dedicated
hosts can source native scripted camera presentation; older-host fallback shares
existing spectator/presentation paths. Campaign map_name uses owning-thread
permission and native destination lookup. Local source adapts CE to existing
map families, preserves MCC campaign order, known UI/Stubbs and local credit,
refuses client or mode-owned gameplay transitions, and clears pending names on
loss/restart/menu/map/restore. CE scripts do not grant unrelated stock credit;
reduced builds keep stubs. No new launcher, camera renderer, protocol or editor.
Existing upstream fixture source was adapted to CE names, without execution.
Source/callers/diffs only; no tests, builds, launches, saves or pushes. These
campaign transitions are not continuous MegaWorld regions or accepted gameplay.

UNVERIFIED VR/voice runtime bridge: optional vr.push_to_talk reserves a selected
Touch/Index grip or stick click and emits the existing PTT action for native
voice. Default off, native PTT mode required; focus/tracking, setting/reference
and menu/Guide/chat/capture changes require release. Both-stick pause wins.
No D-pad voting, new VOIP, packets, microphone SDK or extra XR sync was added;
native capture/device/channel/host/privacy gates and Linux controls are reused.
Source/all action consumers reviewed only; no tests, builds, launches, audio
queries, hardware/save access or pushes. Actual mic/controller behavior, peer
voice and cross-region audio remain acceptance gates. This is runtime input
integration, not full VR or native-voice interoperability proof.


Owner graphics requirement, 15 November: retain all compatible graphics options
from the reviewed runtime forks in the shared settings/menu/renderer paths.
Read-only option comparison against retained OpenCE, Chupathingy, Faber, Fractum
and OpenH1 found substantial existing overlap, with names that are not always
equivalent (Chupathingy's 70-degree FOV and inherited viewmodel behavior must not
be blindly renamed to Forerunner's 16:9 FOV/independent viewmodel control).
OpenH1 baed7757 contains real Vulkan ray-query ambient occlusion in
port/linux/vulkan/xgpu_vulkan_rt.c, world-space SDF/object-proxy AO in
docs/world-ao.md, and enhanced dynamic lighting/per-pixel model lighting.
Its ray tracing is ambient occlusion, not evidence of full path-traced global
illumination. It preserves the BSP's existing baked lighting by applying the
new occlusion only to the appropriate ambient term. Preserve that distinction
for Spark GPU-baked native content.

Graphics integration remains OPEN: the current bgfx Vulkan path does not yet
provide OpenH1's acceleration-structure/early-command/ambient-shader interfaces.
Do not copy its full guest/host renderer or expose a ray-tracing switch that
does nothing. Reuse its compatible algorithms, licensed source and fallback
behavior through the existing renderer after reviewing resource/device ownership.
Required parity also includes HDR/paper-white controls, anti-aliasing/filtering,
render scale/supersampling, shadows/model detail, frame pacing/latency, world,
vehicle and viewmodel FOV/offsets, projection and reticle options. Existing
options count only where their renderer consumer works. Classic defaults,
Linux/Windows support, VR eye consistency, bounded GPU use and resource-limited
targets remain mandatory. Source presence is not hardware/performance proof.
