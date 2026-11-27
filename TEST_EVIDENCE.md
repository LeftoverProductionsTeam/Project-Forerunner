# Forerunner test evidence

## Product Completion V2.1 Windows release

The prior Win64 `eecbbd645b46986dc23b11383a2a8c54d5fc95efca74668a4fea83e4ca21a035`
opened campaign and a normal-menu two-player Zombie Survival LAN match. The
local Ragdolls choice rendered in Gameplay settings. Earlier focused checks
below cover actual Flood get-up replication and third-person aim. The prior
executable is in Previous. The released MegaWorld map, mode receipt, Survivor
map and user configuration passed exact checksum readback after promotion.
The experimental camera inside Extraction exposed a clipped spawn view in an
isolated trial and is outside this release.

## Original Zombie Apocalypse native checks

Windows Latest `eecbbd645b46986dc23b11383a2a8c54d5fc95efca74668a4fea83e4ca21a035`
loaded installed MCC13 `12649191` and Spark's collider candidate `05a4268a`
through the ordinary Survival menu. The map-authored vehicle preset placed
both Undead Demolishers in both runs, with their attached turrets. The
world pickup at (40.17, -75.32, 0.11) was the exact authored custom weapon.
Isolated checks of the existing weapon-selection code preserved the stock
Xbox fallback and explicit weapon-set remaps. Normal Slayer defaults and
the authored placement flags retain their prior values.

Both maps showed living seated Evolved actors. The opening sample through
tick 3600 did not capture an Alpha or Evolved live transition from seat
to foot. Spark's separate `7a0672ff` Hiding Marine diagnostic removed
the Marine from its spawn, and the separate Play Solo run saw Standard
riders leave through the native path. The installed-map and collider runs
used LAN, so this does not establish the effect of the Marine on release.
A finer native Play Solo trace on released `f9907675` resolved the Alpha
outcome. On exact `12649191`, the three automatic spawner sites produced
living on-foot Alpha actors by tick 2 and three new living actors by
tick 1804. Spark's exact `05a4268a` collider comparison showed the same
two waves by ticks 7 and 1813. Both used normal Survival menus and no
player fire. No sampled frame captured the same Alpha in a seat. The
result verifies automatic Alpha delivery in the converted maps. The
physical seat-to-foot transition and original Demo comparison remain open.

The same exact controls also showed a living, unparented Evolved variant
near the authored row-9 automatic spawner (86.29, -99.15, 6.28), by tick 2 on
`12649191` and by tick 7 on `05a4268a`. A second normal-menu launch on the
installed cache reproduced it by tick 2. Another living Evolved actor stayed
seated at the row-10 carrier. These samples establish on-foot Evolved
delivery at one automatic spawner and preserve the other as a separate
case. A normal-input run found the moving row-9 weapon on the ground at
(88.00, -169.46, 0.08), but the Survivor died while trying to leave the base.
The native debug file from the second installed-cache launch records the
physical transition. Carrier `e3530085` invoked grounded rollover at tick 1,
with up.z -0.026. Its exact riders `e36100f0`, `e36300f2` and `e36400f3`
were alive and on foot at tick 2. It also recorded all three Alpha exits at
tick 1 and the refreshed three at tick 1801, with the same refreshed riders
alive on foot by tick 1806. The run enabled existing rollover logging only.
Handheld pickup, firing and the original Demo comparison remain open.

The diagnostic's 30-second mixer captured the map music, Slayer and nearby
effects. Normal input collected and threw an authored grenade. These
checks do not establish original flame appearance or the complete audio mix.

A private Windows candidate
`b8430b21deedf845451022ff70114661cfb38cf6e1412e4648cef762e0bb74a6`
lets local-only games apply native tag-authored healing without the
multiplayer wire cap. Focused checks covered local and shared healing,
and Play Solo booted the exact installed map through normal menus. With
two local split-screen profiles on `12649191`, the first player collected
and threw an authored grenade. The second player's normalized vitality rose
from 1.00 to 166666.02 at tick 1350, matching the original tag's healing
amount. The thrower stayed at 1.00. This run used no remote client. A native
original Demo comparison and full original match remain open.

Latest Win64 `f99076756e2251cb57a2b917a7343e53d112557eda5e1667c2de52a3952fef5c`
uses the previously released runtime sources plus the `42fab1256` local
healing change. A second normal-menu split-screen run on exact `12649191`
reproduced the same heal at tick 1380. The stock Xbox fallback, CE and MCC
weapon selection, and local/shared healing decisions passed a focused
rule check. Only the executable was promoted. MegaWorld and Zombie maps,
mode receipts, and the installed configuration matched their pre-promotion
hashes. Previous holds `eecbbd64`.

## Private neighboring BSP rendering candidate

Win64 `f2e4d85ee1613d18ad6f010825c27241b61a8c844ddc2b23a0c292293309fe10`
opened the released MegaWorld ground preview `fcfe9d44` via normal menus
and spawned in its UNSC hub. The renderer can draw two already-resident
neighbors using their own lightmaps and a shared 4,096-surface cap. A
focused check exercised two neighbors, an invisible second neighbor,
allocation failure, mirror exclusion and low remaining budget. Native
junction visuals and matched frame times remain open.

## V2.1 private Forge and ragdoll candidate

The Win64 candidate `1df2ab6ce735d49eaf82aaad744582dcfe890f42fad1e446e10ed7e8b9a7071e`
built with the upstream HMV 16 validator and the revised ragdoll constraints.
The Windows-compatible portion of Chupathingy's validator tests passed,
including legacy round trips, new record validation and malformed inputs.
The native placement backend still refuses unsupported records as one batch.
Focused C checks passed for reversed knees, excess flexion, crossed and
coincident limb points, and separated limbs. Silent Cartographer launched
through its normal menu and ran the beach firefight with ragdolls enabled
and no runtime exception. Its original frame-time trace is diagnostic. A follow-up replay used
Win64 `f2e4d85e` and stock B30 `a6e2ae62` with the same saved settings and
normal movement, shots and grenades, first with the revised ragdolls on
and then off. Both began the input at tick 1230. In the 18-second
beach firefight window, capped frame times were 16.67 ms median in both
runs, 17.49/17.58 ms at the 95th percentile and 45.21/41.64 ms at the
99th (on/off). Both had 21 dead bipeds at tick 2205. The live combat
paths diverged slightly and the trace measures whole frames, so the
tail difference does not isolate solver cost. Custom rigs, contact during
full falls and solver performance at the current corpse budget remain
open. Latest retains the released build.

The existing engine profiler now records a separate `ragdoll_step` section
around awake-corpse updates, including collision gathering, both substeps,
pose reconstruction and the sleep decision. The Win64 candidate
`e64c7fbee` compiled successfully. Direct scope timing at the corpse cap,
custom skeleton acceptance and release validation remain open.

On exact stock CE609 Rat Race `03a483b5`, Forge Tools failed before the CE
name fix and opened afterward on Win64 `b7f811e0`. Normal Local Forge import
and save preserved a native spotlight record and its credited metadata.
Native HMV readback gave one `lifi` record at (16.669445, 0.421251, 0.123938)
and a byte-identical save `9240fb81`. The title and operation are local-only.
This checks the shared placement transaction, not every HMV 16 record kind.

Latest Win64 `963313f2` includes the CE name fix, bounded HMV reader, local
light fixtures and the right-side Forge Tools panel adapted from Chupathingy.
On the exact Rat Race fixture, normal menu input opened the panel with a
complete selection border and a clear reticle. The match tick advanced
while a held movement key left the editing player in place. The layout
page imported and saved the same `9240fb81` bytes. The match and original
profile data were untouched by installation. Previous is `f9907675`.
The Backquote control hid the reticle, ammo and vitals while preserving
the viewmodel and motion tracker in the private Win64 panel test.

Private Win64 `132534c8` compiled two speculative BSP previews in the
existing warm-slot budget. Each viewport's requests are considered before a
single load attempt. The focused check covered filling both slots, preserving
visible and ordinary warm entries, replacement throttling, failure suppression
and one-slot fallback. Actual multi-neighbor junction rendering and foreground
selection for split-screen terrain remain open. The existing
`scenario_illumination_at_point` is an unused constant-output placeholder.

## Product Completion V3 — Live Machinima

The exact Win64 camera build `e15ab1455839a1426e6525132d35eb05efa41c9e5bd12c11891e09f7849ca81b`
loaded Zombie Apocalypse `12649191` through the ordinary Play Solo menu.
Keyboard controls drove the HUD-free native camera. The player remained
at (97.170,-159.023,-0.287) while the view reached (94.0,-159.8,0.6).
After parking, the player moved to (98.271,-158.463,-0.287) while the view
held. The saved private bookmark was recalled after moving the camera again.
When a second local process connected, before its player joined, the host's
camera returned to the player (render samples at ticks 2280 and 2310).
The earlier private candidate also rendered voluntary exit with the classic
weapon and reticle and saved a rolled pose and a widened 90-degree FOV.
Existing map-script restrictions on bookmark access passed focused checks.

Live flight reuses native director and collision controls and adapts easing
from thelinkin3000's SlipstreamCE `75dd78a44c53` under AGPL-3.0-or-later.
The isolated Windows delivery excludes the pending two-neighbor work.
Promoted the exact tested executable to Latest. Previous is `963313f2`.
The installed MegaWorld ground preview, Zombie Apocalypse and saved
configuration matched their prior hashes.
Controller chords, separate simultaneous devices, campaign-native acceptance,
regional camera ownership, HREC tracks and the Theater player remain open.

Win64 `97a90a99b416319c1cbdb13b1ab4dbe66175efc05f1b0ae6c6eb1ff0a93a9f96`
captured its private Machinima view during an ordinary Zombie Apocalypse
Play Solo session with Record My Games enabled. The HREC retained its
ordinary player and object tracks. It also held 360 consecutive camera
samples at ticks 198–557 and a stop at 558. The captured up vector changed
with roll and the projection changed with the lens input. Leave Game
closed the recording. The public filter removed both kinds of camera record
and preserved neighboring game records. Its injected-fault control failed
when the filter was deliberately removed.

The existing local browser viewer opened the actual native HREC, selected
Machinima, scrubbed, stepped backward and forward by one frame, advanced
nine ticks over 1.2 seconds at quarter speed, and downloaded a 282208-byte
WebM through Record video. The custom Zombie map displayed via the viewer's
collision-wall fallback. Native Theater, native campaign recording and
full native map presentation in playback remain open. Promoted the
validated native writer to Latest with Previous `e15ab145`. Installed maps
and configuration matched their prior hashes.

## Native camera review

Win64 `ff104636916a076663a558e2094ccdc9582a547aff8d5b25e0b62d49c6b633b7`
loaded the installed Zombie Apocalypse map through the normal Play Solo
menus. Native review skipped a recent film with no camera frames and found
the completed private run for map SHA-256 `126491917a68c50e14a9465a8b066c40f56a9cd3eb933b66f5ee188f36ebedb1`.
It rendered the first and last camera poses with stock geometry and no
first-person weapon overlay. The native controls stepped from frame 240 to
239 and back, sought to the first frame, and played at quarter speed to
frame 16. Simulation ticks resumed after exiting review. The test log and
rendered frames are under `build/machinima-native/native-review-release`.
The installed executable matches the tested hash. Previous holds `97a90a99`,
and the ground Extraction map, its mode receipt, the Zombie map and
configuration match the pre-promotion hashes. This review plays camera
motion over the current native scene. Native match replay remains open.

## Main-menu emblem and installed MegaWorld check

Win64 `4ed9dcfee28458e8cb36a5e193b4f888a8be3c89d769946d978a33581cf5cdd2`
renders the existing Forerunner emblem beside the wordmark below the original
Halo logo. The checked native window showed the selection outline and tagline.
The same executable, launched from Latest with isolated profiles, opened
`fcfe9d44` from the normal Extraction menus and spawned in the UNSC hub.
Previous holds `ff104636`. The released map, its mode receipt, Apocalypse,
and the installed configuration retained their hashes. Evidence is under
`build/megaworld-ground-preview/latest-v3-emblem-installed`.

## MegaWorld menu and regional death camera

Win64 `ab43d580f0ea7d37937843dfb1344712b3167c667ec66201bf65857f3c283b98`
launched the released `fcfe9d44` map through both Extraction > Play Solo and
Extraction > Choose Map & Host from Latest. The native map identity matched,
and each route spawned a healthy UNSC player in the Extraction hub. The LAN
check used the full installed map catalog. With only the CE cache available,
Play Solo showed Be Native in the campaign chooser. These runs are under
`build/megaworld-ground-preview/v3-latest-{solo,lan,ce-fallback}`.

On a private LAN match with the same executable and retained combat variant
`bf748855`, native Flood killed the deployed UNSC host. The camera stayed
near the body through the full countdown and the player respawned at the UNSC
hub. The Covenant guest stayed healthy at their own hub with an independent
camera. The failed operation settled and displayed the redeploy prompt. This
test covers a regional death and settlement, with durable gear and concurrent
combat in multiple regions still open. Logs are in the `v3-death-fixed-host`
and `v3-death-fixed-guest` runs under
`build/regional-native-trial/megaworld-hunt-bf748855`.

## Regional drop persistence

On the installed MegaWorld map `fcfe9d44`, a private Win64 run `370bae30`
used Play Solo and normal movement through the Infinity entrance. A grenade
death in BSP 12 dropped assault rifle 927. The player respawned in BSP 0 at
tick 16980. The same ground item was still present at tick 21450, over two
minutes later. The return to BSP 12 at tick 21600 raised the player's rifle
ammo from 240 to 600 and consumed the ground item. The plasma cannon from
the same death stopped appearing at tick 17400. Its drop ownership is open.

The component check covers valid regional ownership, rejected and retried
spatial registration, recycled handles, and preserving the remaining
lifetime of parked gear. Native evidence is under
`build/megaworld-ground-preview/v3-loot-native`. The promoted build `6fa0c1e3`
retains the existing projectile activation rule and passed a native map-launch
smoke before promotion. Destroyed and transported vehicles and independent
guest loot recovery have separate acceptance gates.

## Product Completion V2 Windows checkpoint

- Win64 `63e69238ddb4ec1f590c24a30fa20a0cf0b389fbe82813b30c5f3518ec96e1d8`
  hosted Zombie Apocalypse through the normal LAN menu on exact MCC13
  `126491917a68c50e14a9465a8b066c40f56a9cd3eb933b66f5ee188f36ebedb1`.
  Two observed Flood recoveries from feigned death matched on host and guest.
  A late-joining third machine received matching common actor movement,
  vitality, seating and deaths. The baseline showed a recovered Flood alive
  on the host while both guests still rendered its corpse. See
  `build/zombie-survival-sync/fixed-actor-comparison.json` and
  `fixed-late-actor-comparison.json`. Original Demo fidelity and online
  Survival remain separate acceptance gates.
- The same Win64 candidate launched the released MegaWorld ground preview
  `fcfe9d449aed9683421e22ed80c412e74ebb05fdcb121ae988b13b3ea5e7f01f`
  from the normal Offline Extraction menu. Ordinary controls confirmed UNSC
  deployment, firing and an authored Plasma Pistol pickup. The revised neighbor
  rendering still needs a rendered cross-BSP seam check, a vehicle crossing and
  independent multiplayer occupancy. Latest retains the existing released pair.
- Private Win64
  `ec2bf1438219b2faf5cb4cb3ad6da95eb667d22a0e573c545e39993233ec48ad`
  exposed distance, shoulder and height in Camera & Viewmodels. The settings
  saved at 800×600. Ordinary controls in Silent Cartographer verified firing
  with the adjusted reticle. The existing camera mode restrictions remain.

## Windows solo Extraction

### Solo Blood Gulch transport replay

On Win64 `c289a2ecfe5a7960ef3bb52fa7440ebca423687266f793a8aa47467620e47ef2`,
the normal Extraction → Play Solo menu loaded CE609 Be Native `d113e9db`,
whose native BSP is `levels\test\bloodgulch\bloodgulch`. Normal movement
and E deployed and entered Pelican side seat 4. After powered flight the
native contact solver reported ground, landed and rest at (78.154, -124.854,
1.588), with zero measured motion. The Marine moved from seat 2 at tick 2640
to the field alive at tick 2670. The player exited on E after tick 3510 and
the AI pilot lifted off at tick 3540. No debug movement or seat changes were
used. Repeated insertion, Spirit and multiplayer acceptance remain open.

An earlier run on the installed executable suffered one native grenade death,
respawned as a Cyborg, and moved on both axes using W+A and S+D afterward.
The reported Grunt respawn was not reproduced on this exact cache.

The installed Win64 executable SHA-256 is
`cf6a00e01c2a7f586c2c39e3852961e0d0a9caa9c6ea2f21a148de16cf4917dd`.
Normal menu input created a local profile, opened Extraction → Play Solo,
selected Be Native [CE], and launched the unchanged Black Echo cache at
`d113e9dbec1bae9554e7d4df3843ecc2bb6d40b4ee2c7cdf5f81831fc7b29b09`.
The native hub admitted the local player with OpenCE protocol selected,
Delta and public discovery off, and the saved host Firefight setting on.
The deployed binary retains the prior recovery build as
`halo.before-solo-menu.exe`. 18 focused menu checks passed.

A follow-up through the same installed executable and map used ordinary walking,
mouse aim and E with passive native logging. At (40.308, -110.355, 1.441), E
activated the authored deployment control and issued the local starter kit.
Walking to (53.625, -103.680, 0.025) showed the Pelican side-seat prompt.
E boarded, the occupied Pelican flew the field insertion, and E disembarked
at (77.740, -125.738, 0.518) at full health. No forced placement or seating
was used. The low blue deployment control was difficult to see from spawn.
Covenant insertion, field combat and objectives, evacuation, banking and private
LAN guest persistence remain open on this build.

A subsequent Windows candidate
(`9bfaac5c26f123cba3401c50fdb85adc7d6f754453d4c1eb6c97fc67e880b866`)
used the same map and disposable local profile. Clicking the difficulty OK
button on the previous-good executable and the first candidate attempted to
read the world snapshot after confirmation. The fixed button reached the hub
without that read, and both scratch save files remained byte-identical. The
fixture slot was empty, so this comparison does not establish recovery from
a populated checkpoint or durable banking. Twelve focused menu checks passed.

On the fixed candidate, normal movement and E activated the hub control,
displayed the new boarding guidance, entered the native Pelican side seat,
received the landing cue, and disembarked at (77.740, -125.738, 0.518) with
full health. The direct approach met the parked Pelican near (56.8, -99.9).
An on-foot eastern detour reached (59.373, -94.567, 0.432) and
(61.668, -86.158, 0.314); turning west met the Spirit at
(59.493, -85.117, 0.093). Facility objectives were not reached on this run.
The installed map and executable hashes were unchanged.

The native pause panel on the same pinned map rendered the entire opening
objective over three lines at 900 × 675 with Windows candidate
`e846c57912cbbcd3aa5feabda89381b01bc6eb7f65abc73e90c402e19908b4b1`.
The stock inherited pause panel had clipped the sentence after “Disembark.”
The map’s original text and HUD art were untouched. The screenshot is under
`build/windows64/screenshots/black-echo-objective-wrapped.png`. Two existing
text-fit checks passed; other display modes and the oversized authored HUD
art remain separate acceptance checks.

## Windows playtest recovery

The Windows x64 testing executable at `build/windows64/halo.exe` used SHA-256
`35b85de487e621b8f353f203ce845133e61164678e034c9ae2307dfbaa9169f1`.
The exact delivered executable was retained as `halo.before-menu-repair.exe`.
Both it and the earlier XR baseline reproduced missing Vulkan menu artwork.
The GL-only readiness gates blocked PNG artwork and the font atlas before either
could reach Vulkan. Valid CE scenarios in the main maps folder had also reached
the misplaced-cache dialog before the menu could open.

Visible Win64 menus exercised using native input and disposable saves loaded stock
A10 and Blood Gulch, spawned in the staged Zombie Apocalypse and fourteen-region
MegaWorld maps, and started wave 1 on stock B40 with the OpenCE protocol selected.
The installed executable listed and loaded the unchanged Black Echo CE609 cache
(`d113e9dbec1bae9554e7d4df3843ecc2bb6d40b4ee2c7cdf5f81831fc7b29b09`) under
a disposable new filename in `maps` and reached its native Extraction hub with
Delta disabled. The map picker issued no startup warning. 81 focused checks passed.
The retained runtime data, maps, previous binaries and user saves were preserved.
The test does not cover LAN guest stash ownership, a complete Black Echo run,
physical terminal and boarding acceptance, or consumption of Spark's new export
mode receipt. Monitor grab compiled and existing Forge regressions passed without
a full native placement session.

Staged Win64 inventory, rehashed on 25 November. The installed executable is
`cf6a00e01c2a7f586c2c39e3852961e0d0a9caa9c6ea2f21a148de16cf4917dd`.

| Menu/playtest | Staged file and SHA-256 | Last applicable native result |
|---|---|---|
| Extraction | `maps_ce/be_native.map` · `d113e9dbec1bae9554e7d4df3843ecc2bb6d40b4ee2c7cdf5f81831fc7b29b09` | Installed executable, local profile, normal menus through ground deployment, UNSC boarding and field egress. |
| Zombies | `mcc_maps/fonzie's zombie apocalypse.map` · `126491917a68c50e14a9465a8b066c40f56a9cd3eb933b66f5ee188f36ebedb1` | Installed executable, Internet play off, normal Zombies menu through local Slayer lobby and armed spawn. Survival completion was outside this smoke. |
| MegaWorld | `mcc_maps/gsmegaroad.map` · `71fc164876cafcece4de49c2b0563c79c6f772abfff308c7dc61d5a411b79e2b` | Fourteen-region baseline. Native menu spawn on the earlier recovery executable. Private later-region tests use separate maps. |
| Firefight | `maps/b40.map` · `0a7b26d6d828e7b773d36d9d5c798b9bf4b143332301a0c0e7a3d685782eab48` | Stock-map wave 1 via the earlier recovery executable and OpenCE protocol; no authored evacuation on B40. |

Repeat the applicable visible Windows menu and basic gameplay checks before
updating the testing installation at each major checkpoint and every ten
meaningful local commits. Keep the previous tested binary and map available.


15 November 2026. Source inspection, compilation, automated native simulation,
rendered gameplay, human play and cross-platform acceptance are distinct here.
Bounded native multiplayer, rendering and read-only Spark discovery are verified;
full gameplay, Firefight, Extraction, VR, editing and MegaWorld acceptance remain open.

The owner's later Development & Verification Directive now authorizes focused
compilation/automated tests and targeted isolated native smoke/synchronization
checks. Earlier source-only entries remain unverified until actual results are
recorded. Full gameplay, stress, VR hardware, MegaWorld and release acceptance
remain separate final-phase gates. Current work prioritizes native operations
gameplay, authoritative gear, revival and co-op stability.

15 November, exact Spark candidate and transport source (base `71b761be02`):
Spark revision `ac3f1440fad9d7a854f1627202efeb481b2b996c` records CE609 candidate
`08389ed43814960ddfdfcb401746f277f6a038e2e413e2a26ac8339703cec514` (106,368,008B).
Existing Invader tools reproduce it exactly inside Forerunner, including Spark's
Flood-start correction and three-second defense preparation. Independent scenario
and recovered-HSC SHA-256 match Spark's `032e9462...`/`93fe5527...` readbacks.
`build/spark-black-echo-native/provenance.json` retains exact inputs and hashes.

PASS: `build/native-spark-black-echo/result.json` verifies two bound Delta accounts
starting with fresh private gear records, actual spawning/equipment, control-driven
deployment, AI combat, Marine navigation/rescue, native technology pickup, optional
Flood containment, defense, both native ten-second activations and physical hub
returns, durable banking and the authored settlement/completion callback. Fresh
processes redeploy the same four unique serialized saved items, with both native
views reading equipment. The unchanged comparators pass: 139 player seconds,
AI damage/state comparisons and 144 item snapshots; all 12 new host items appear
on the client, zero item mismatches, no native exception. Native starter-ammo
pickups can change live ammunition after grant; saved item records match exactly.
No console objective setters or forced mission success. Original identity files
are unchanged; runtime/map copies and private invites are removed. Six lossless
PNGs remain; 191,694,906B of BMP scratch and 143,509,738B of build-input scratch
were removed. The existing owned regression cache and exact Spark candidate remain.

FAILURES: no engine/gameplay failure in that operation. Setup initially asserted
a guessed SDK particle-tag count and then assumed MP globals were absent; both
assumptions were rejected before mutation. Native CE extraction already supplied
valid globals/particle sources, and the native rebuild reproduced the known
`91151b1a...` baseline before applying Spark's two edits. No speculative engine
patch was made for setup assumptions. A transient diagnostic script variable
error affected its summary print only, not successful dependency readback.

UNVERIFIED: human acceptance, rescue-failure/alternate branches, revival in this
session, repeated mission objectives after redeployment, faction UI/bipeds,
dropship insertion/boarding/flight/cinematics, independent squads and other targets.
This candidate derives from the earlier owned fixture through Spark's actual
native editing/build workflow; it is not the complete expanded Black Echo vision.
Its native vehicle and vehicle-palette counts are both zero.

Transport implementation then adds optional native named-vehicle boarding to
`extraction.c`, reusing the existing run, gear escrow and native callback. No
packet/save-format change. MSVC passes 89 connected Extraction/gear/medical and
transport workflows, including both native factions, invalid/duplicate bindings,
bad seats, stale handles, wrong/missing faction transports, destroyed ships,
disembarkation, changed allegiance/replaced ships, in-flight/local coordinates,
cinematic delay and exactly-once recovery. Removing the cinematic gate is detected.
Windows64 passes (`build/native-dropship-build.log`), SHA-256
`f77c5958dadef144165780d75090519df0787573de9d77e960028b19af96aada`.
The actual Spark operation above used the prior native-tested `1fcc7563...` binary;
do not claim that it exercised these new aircraft gates. No actual dropship
gameplay acceptance, new renderer or replacement test framework is claimed.

15 November, native co-op allegiance handoff (base `1b5bbaaa67`):
`network_objects.c` uses the existing reliable object description for changed
native teams, including the same salted handle; replay now applies that field.
`players.c::network_player_reconcile_coop_team` requires matching full owner/body
links and a native campaign team. Attachment and native `actors.c` team/attach
callers use the same reconciliation. Classic MP assignment remains separate.
The existing replica harness passes 76 replica/allegiance cases, related focused
inventory/drop cases and three controls restoring the original send/replay/attach
defects. Incremental Windows64 passes (`build/native-allegiance-build.log`),
executable SHA-256 `1fcc7563e10c713cdf36d77fed9be494b3970b912f83267f4b4e6cb9e60e5f7a`.

`build/native-delta-allegiance-rejoin/result.json` verifies actual native player
and unit teams on both peers after ordinary host HaloScript `ai_attach` and
`ai_set_team` commands: network slots 0/1 are Covenant/Human at tick 300, then
Human/Covenant at 360. The guest disconnects/rejoins the same host; the host's
body/allegiance survive, the new guest initially has the ordinary player team,
native reassignment matches on both peers at 1290 and both return to player
team at 1380. Three bound account proofs cover the two distinct accounts and
rejoin. No native exception; original identity hashes unchanged; owned runtime,
copied credentials, map staging and invite removed. Compact evidence remains.
The unchanged player comparator passes the first leg. The reconnect leg has no
player divergences but is shorter than its 10-second acceptance window; do not
call that a movement-comparison pass. Item comparisons pass on both legs (13/7
snapshots, zero mismatches). `build/native-delta-allegiance` retains the initial
driver failure: it incorrectly equated HaloScript list order with network slots;
host/client actually agreed on the reversed assignment. No engine patch was
needed for that driver correction. This smoke does not prove combat allegiance,
revival/radio policy, persistent faction selection, distinct biped/loadout assets,
dropship missions, independent squads, other platforms or mixed-version peers.

15 November, authenticated native death/loot/bank (base `f9b6d8dc5f`):
`build/native-delta-death-loot` uses normal controller grenade inputs and observes
real death, loss, hub respawn/new deployment and survivor pickup. Its input driver
initially mistakes an unparented holstered weapon for a ground item, then the
bounded run stops before the second bank. Player/AI comparison passes, but the
unchanged item checker fails: at tick 360, held technology item 5 appears 100.2
world units apart because the old log prints scene parent NONE for both copies
and compares their meaningless stowed positions. This is retained failure evidence.

`network_test.c::network_test_item_holder` now reads the actual native inventory
for holstered weapons, requiring attached/nonworld state, matching player ownership
and a unique carrier. A readied weapon without its scene attachment remains unheld;
ground items, stale full-owner handles and ambiguous claims are not normalized away.
No gameplay state, comparison tolerance, packet or protocol version changes.
The existing replica harness passes 35 related cases including nine holder cases;
old-parent-only and hidden-readied-orphan fault controls both fail as required.
Incremental Windows64 passes (`build/native-item-holder-build.log`), executable
SHA-256 `ebe2064750c43173b3126c18b44e3215e833a3cc304b6259743a5ad5af09ef65`.

`build/native-delta-death-bank/result.json` then verifies two distinct bound Delta
accounts through controller-caused death at tick 1110, authored hub respawn at
1200, explicit new deployment and survivor ownership of the original dropped
object at 1530. The original failed run is lost; both new/current runs physically
activate extraction, return, bank and wake the native settled-survivor callback.
The original technology item's saved ID occurs once under the survivor's different
account and nowhere in the former owner's bank. Fresh engine processes reserve the
same four unique serialized item records; both native views show both equipped
living players with matching ammo after the grants. Comparison passes unchanged:
103 player seconds, 108 actor snapshots/510 unit comparisons, 107 item snapshots,
zero item mismatches, 17/19 new host items sampled; no native exception. Original
identity files remain unchanged. Runtime/map copies and 595.3 MiB of BMP scratch
from both runs are removed, retaining six exact PNGs and compact logs/records.
This seeds previously banked disposable fixture records and uses a bankable empty
technology weapon placeholder. It does not certify fresh Spark quest collection,
revival, opposing PvPvE/factions, broad crash recovery or other platforms/VR.

15 November, Extraction live rejoin (base `37bdd12f3a`):
`build/native-delta-live-reconnect` proves hub-disconnect bank preservation and
authenticated live rejoin, but its movement driver is blocked by the host's body.
Moving the host aside in `build/native-delta-live-return` exposes the actual
first-life spawn defect: the returning client appears beside the field host at
y=-91, away from the authored hub/terminal at y=-87.472. Death/wipe hub handling
already exists; the first-spawn path was still applying campaign relocation.
`players.c` now retains authored Extraction starts and exempts an admitted hub
from teammate-room waiting, while preserving intro/cinematic and ordinary campaign
gates. The added behavior case in existing `tools/test_death_timing.py` fails on
the old helper. The repaired suite passes its 15–1,000 FPS timing/camera/respawn
checks plus hub/campaign/cinematic policy. Incremental Windows64 passes
(`build/native-hub-rejoin-build.log`), executable SHA-256
`17828536829ce3f42731b8c926da674e8700c834e6f25f5f09bf67b9dfa0bab1`.

`build/native-delta-hub-reconnect/result.json` passes the real two-account live
continuation with previously banked native records copied into disposable saves.
The same host advances from tick 210 to 1950 without restart: hub departure keeps
both records byte-identical; rejoin places the client at 48.845,-87.472,0.078 while
the host remains at 48.444,-91.270,0.426; deployment preserves the saved pair's
item bytes; departing while deployed settles phase idle/counts 0/0/0/outcome lost;
a third signed-in connection grants no refund and preserves the host's stash.
There are three fresh bound test-account proofs, two equipped living players in
both final native views, no native halt, and unchanged original identity files.
Owned processes/runtime/map copies are disposed. This is live reconnect/loss and
authored-start verification, not a new escape/bank run, true-death proof, Spark map
acceptance, Linux/VR verification or a general reconnect certification.

15 November, authenticated host ownership (base `25767db94e`): original
`extraction_owner` selects a local-profile stash even for a verified Delta host,
while that same account joining remotely selects an account stash. The connected
real-code reproduction fails before the fix. Delta hosts now use the same bound
account lookup as remote players, and missing/unbound proofs cannot fall back to
profile ownership. Offline/local profile records retain their existing owner.
All 77 connected Extraction cases pass, including host/account identity,
unbound-host refusal and host recovery followed by remote redeployment with the
exact same recovered item ID. Existing refund, pending-death, transfer, survival,
medical and completion cases pass unchanged. Windows64 builds incrementally
(`build/native-account-owner-build.log`), SHA-256
`9543508eced47d7237d69310915f4f78344b248b83686c7e2cafe82ba9e6b17b`.
The identity proof/transport is a fixture boundary here. Read-only installed
Chupathingy metadata confirms existing device key/certificate files and original
runtime account/Steam-link log entries; no private contents were printed or
credentials copied. Actual authenticated Delta banking/restart remains open.

15 November, actual Delta deployment and NPC ownership (base `839a1a7962`):
`build/native-delta-two-account-linked` enrolls the owner's confirmed ForerunnerTest
device through the inherited service and joins the original authorized account by
private native invite. Host readback has two distinct verified bound player cards.
Each process has its own saves and local handoff secret; no account IDs, issuer
roots or identity safeguards are substituted. Original identity-file hashes remain
unchanged. Copied original credentials, invites and runtime/map payloads are removed;
the new account's recovery material remains in owner-only ignored storage. No Steam
token is copied or second Steam linkage claimed.

`build/native-delta-extraction` then reproduces a real deployment refusal despite
successful authentication. `distributed_host_tick` populates the player-machine
lookup from remote clients only; the old Extraction fixture incorrectly supplied
a host entry. The corrected boundary reproduces that refusal. The host now uses
`player->network_player_data.machine_index` while remote players retain their
distributed lookup; both still require `identity.bound`. All 77 connected cases
pass, including a nonzero host machine/controller, unbound refusal and role changes.
Windows64 incremental build: `build/native-host-machine-build.log`, executable
SHA-256 `b31e104f53c9e128a13e71f19cdc80a75454c602a405af6d46254f693db95822`.

`build/native-delta-extraction-membership` reaches authored combat/rescue/technology/
Flood/defense and banks the host's pair, but the input driver fails to complete the
second player's control interaction. Its unchanged player/AI/item comparisons pass;
it is not a complete multiplayer banking pass. The next ordinary-controller run,
`build/native-delta-extraction-inputs`, physically returns and banks both players,
wakes the settled-survivor HaloScript callback, then stops/restarts both processes
and deploys the same four unique item records. Each native owner record goes from
generation 4 idle 2/0/0 to generation 6 deployed 0/2/0, with identical serialized
item identities/cache/quantities. Unchanged comparisons pass: 100 item snapshots,
zero items out of step, all 14 new host items observed; player/AI checks pass and
no native exception occurs. Its final grant precedes the last periodic equipment
sample, so final client equipment readback is a separate continuation, not inferred
from the record bytes. Actual death/reconnect/power-loss, faction dropships and the
Spark-authored mission remain separate acceptance gates.

`build/native-delta-extraction-readback` closes that sampling gap: after banking
both accounts and restarting both processes, the driver waits three additional
native seconds after both saved grants. Both clients then observe both living
players carrying the same redeployed weapon types and matching ammo. The four
unique saved item records remain byte-identical in reservations; neither owner
retains another stash/escrow copy. This run also observes a refused host confirmation
write: existing revoke/refund/retry succeeds, ending at host generation 8 / client
6 instead of 6/6. No identity fallback or second reward is used. Native pickup of
dropped starter ammo can increase carried ammo after the grant; it does not change
the serialized reserved quantities, and byte-identical live ammo is not claimed.
Player/AI/item comparisons pass unchanged: 127 item snapshots, zero out of step,
all 12 new host items observed, no native exception. Original account files remain
unchanged and disposable runtimes/maps are removed. This proves the bounded real
Delta bank/process-restart/redeployment path; it does not certify arbitrary I/O
failures, live reconnect/true-death recovery, all Extraction gameplay or Spark's map.

The larger encounter's NPC ownership repair suppresses autonomous replica NPC
weapon drops and reuses `distributed_client_release_item` for validated host world
states, restricted to dead tracked nonplayer bipeds. Native player-owned equipment,
living NPCs, vehicles, local-only objects and hosts retain their existing behavior.
A retain-only attempt (`build/native-six-operation-npc-drop`) failed with 15 item
mismatches and is retained as failure evidence. The complete release path passes
26 related native helper cases, eight retry cases, both drop fault controls and
Windows64 compilation. `build/native-six-operation-world-drop/results.json` then
passes all five independent clients: 111 player seconds; 116 actor snapshots /
1,025 unit comparisons each; 115 item snapshots with zero items out of step and
40/40 new host items observed each. Actual combat reaches rescue, technology
collection, Flood containment, defense and extraction availability. No comparison
tolerance, packet, protocol version or authoring interface changes. This is one
host/five clients in an owned regression encounter, not six authenticated banks or
independent squad objectives. Original failed-run logs remain; bulky runtime/map
copies are removed and selected native pixels retained as lossless PNGs.

15 November, independent native peers and CE unload (base `4564360a48`):
`build/native-six-instances` runs one host and five distinct clients, one local
player each, on isolated loopback endpoints. All five pass unchanged player
comparison for 41 seconds and item comparison for 45 snapshots, zero out of step,
10/10 new items seen; each process has 41 snapshots of six living equipped bodies.
`build/native-six-operation` reaches extraction availability after real authored
combat/rescue/collection/Flood/defense, with all five player/item comparisons
passing. Clients 4/5 then halt on host-disconnect map unload: `ce_models.c:929`
passes NULL to Halo's debug allocator when no dual-use bitmap copies exist.
Original source reproduces in the existing native harness; the guarded release
passes empty, populated, repeated and later-map cleanup, releasing all nine
fixture allocations exactly once. Windows64 incremental compilation passes.

`build/native-six-operation-unload` removes the assertion and all five clients
return to the native menu. Player/item checks pass, but AI checks fail on host
readback ticks 3304/3335 versus client 3300/3330: interval scheduling accumulates
late-frame offsets. The real-code delayed-frame case reproduces that defect.
`network_test_log_actors` now anchors intervals to simulation tick buckets and
still logs actual observed ticks. Eleven existing native diagnostic cases pass;
its stale harness boundaries are updated for existing weapon/control readback.
Neither gameplay nor comparator tolerances changed. Final binary SHA-256
`bec5db33f28371a98b73a352ad76a445e36952202652fa1fbafc02a676c87582`.

`build/native-six-operation-cadence` reaches the same authored extraction-ready
stage and passes player/AI checks against every client: 111 compared player
seconds, maximum separation 0.1205; AI 116 snapshots/1007 unit comparisons per
client. No native unload exception remains. Item checks pass for four clients,
but FAIL for client 2: rifle 42 is already detached at 1470 and 1500 while host
NPC 41 retains it until its death/drop delay ends by 1530. Host/client AI death
states agree; the earlier weapon detachment is still a real ownership/presentation
finding. All peers see 37/39 new items across 115 snapshots. Keep the failed run
and strict check; this is not accepted complete six-client gameplay, authenticated
banking, Spark Black Echo, independent squads, dropship or cross-machine acceptance.
All owned peers/staging are removed; compact logs and lossless captures remain.

15 November, combat reserve/death-loot repair (base `c7f7a61ce4`): the existing
per-local-player inventory record now tracks reserve changes/reloads separately
from loaded shots. The host's absolute reserve updates can arrive during sustained
fire without refunding predicted loaded rounds or undoing a reload. Original code
reproduces the withheld pickup. Native `unit_drop_grenades` now clears a replica's
counts without allocating loot; `mcc_grenades_drop_extra` likewise drains extra
types without creating client-only equipment. Original client death allocation
also reproduces. Eighteen related real-code native harness cases pass, including
reload grace, a second local player, repeated inventory/death, host drops, base/
extended grenade counts, stowed ownership and checkpoint inventory. Four fault
controls detect lost reload/loaded-round/death authority guards. No new harness,
wire format, pickup message or persistence format was added.

Windows64 incremental builds pass (`build/native-reserve-pickup-build.log`,
`build/native-death-loot-build.log`); final binary SHA-256
`94b29b5f7ed7c8a0f549a43eebba83e8d1ba2abc83cfc3054ffd0030070fec23`.
The intermediate `build/native-coop-six-reserve` passes native player/AI checks
and exercises the reserve pickup within one round, but FAILS items with six extra
client grenade objects. Those failures drove the drop-only authority repair.
Final `build/native-coop-six-loot` passes the unchanged player comparator for
137 seconds, mean/max separation 0.002/0.051, maximum aim difference one degree,
zero of 11733 divergent prediction ticks and 84 corrections. One transient alive
mismatch remains below the existing persistence threshold; no tolerance changed.
AI passes 142 snapshots/1353 unit comparisons, max separation 0.1898, twelve
common live/damaged units and eleven common moved/dead units. Items pass 141
snapshots, zero out of step, 37/39 new host items observed. Both peers record over
120 snapshots with six living equipped players. Player 0 gains 180 rifle rounds
at tick 2610 with equal totals on both peers; maximum rifle ammo difference is one.
Native authored stages again reach extraction availability after deployment,
Covenant combat, physical rescue, technology collection, Flood and defense.
This is three local players per peer, not six independent clients/squads; remote
banking, full mission completion, split-screen HUD, Linux/MCC native gameplay and
hardware VR remain unaccepted. Compact logs and lossless native captures are
retained; disposable runtime copies and bulky BMPs are removed after jobs stop.

15 November, larger native co-op (base `448650a8e8`, unchanged compiled engine):
`build/native-coop-three-player` passes player comparison for 37 seconds,
mean/max separation 0.013/0.041, no mismatched seconds, no divergent prediction
ticks or corrections. Both peers have 37 snapshots with three living equipped
players. Items pass 41 snapshots, zero out of step, 4/4 new items seen.
`build/native-coop-six-player` similarly passes 37 seconds, separation
0.013/0.067, no mismatched seconds, 51/3324 divergent prediction ticks,
maximum drift 0.000488 and zero corrections. Both peers have 37 snapshots with
six living equipped players; items pass 41 snapshots, zero out of step, 10/10
new items. Existing local-player and virtual-controller inputs are reused.

The tracked owned fixture now scans one native player-list entry per tick for
actual technology ownership, including crews beyond its former two entries.
Invader compiles four scripts/four globals and builds candidate CRC E52C2E4E.
`build/native-coop-six-operation` uses three local players per peer. Native
console stages reach deployment, Covenant elimination, physical Marine rescue,
technology pickup, Flood containment, defense and extraction availability.
Host and client identify network player 3 holding the unique technology weapon;
`be_collector_index=3` and `gs_manual_4_done=true` confirm the authored scan ran.
The extra unit_has_weapon console probes used doubled path separators and all
returned false; those probes are not ownership evidence. Actual item/player
snapshots and the native objective transition establish the pickup instead.
Both peers record over 100 snapshots with six living equipped players.

This combat run FAILS synchronization and is retained without tolerance changes:
137 compared seconds, separation mean/max 0.003/0.060, 74 corrections,
22 ammunition-mismatched seconds, one aim and one alive mismatch. Player 0
automatically picks up 180 reserve rifle rounds on the host while continuing to
fire locally; the existing inventory grace defers them until tick 2550.
`network_objects.c::distributed_client_note_own_inventory` refreshes use time
while firing/reloading; `distributed_client_apply_inventory` waits for settled
ownership before applying ammo. A safe repair must preserve prediction and avoid
duplicating pickups through reordered inventory/pickup events. Items FAIL on
plasma pistol 61 (tick 2100, client still held/host dropped) and extra client
plasma grenade 4100 (tick 2130), 36/37 new items seen across 141 snapshots.
AI FAILS because host tick 1111 has no nearby client snapshot; 35 snapshots and
16 units compare, maximum separation 0.0753, four common damaged/dead units.
Do not turn these findings into relaxed comparators or a full gameplay claim.
Native split-screen captures also expose overlapping HUD placement. Six separate
clients, independent squad quests, remote banking and complete operations remain
unaccepted. Owned runtime copies and bulky captures are cleaned after peers stop.

15 November, stowed inventory refresh (base `638dd8b1d9`): the actual old
`distributed_client_apply_inventory` forgets/re-adds a legitimately parentless
holstered player item, resetting pending desired weapon and last-use time. The
new connected real-helper case reproduces that failure before the source fix.
The fast path now admits only a non-readied slot with native attached/player
flags, the full player owner and no world connection. Readied orphans, changed
owners, ground objects and nonplayer ambiguity retain their existing repair;
ammunition synchronization still runs. Twenty-three distinct related replica/
checkpoint/Flood cases pass across the focused runs. Three new owner/slot/ground
fault controls and the three existing orphan/transfer controls detect bypasses.
No duplicate ownership manager, object, protocol or storage format was added.
Windows64 builds incrementally (`build/native-stowed-refresh-build.log`), SHA-256
`9144a81aa93ec173613a6f5ad680b2d6975373091a76487e5dc6db20a2c392b9`.

`build/native-stowed-refresh-switches`: existing isolated native co-op driver
requests 14 ordinary keyboard switches per player at independent offsets, without
deploying the authored encounters or enabling Extraction. Actual rifle/pistol
captures are retained. Player comparison passes 37 seconds, mean/max separation
0.013/0.086, no mismatched seconds, 2/370 divergent prediction ticks, mean/max
drift 0.000001/0.000222 and zero corrections. Unchanged item check passes 41
snapshots, zero items out of step, 2/2 new items seen. This is native switching/
ownership regression coverage; the exact pending-choice/history fault is proven
in the source-coupled fixture, not injected into the native client. Other platforms,
larger squads and complete authenticated multiplayer banking remain unaccepted.
Owned peers/staging are disposed by the existing runner; bulky BMPs are removed,
with compact native logs and lossless rifle/pistol captures retained.

15 November, native local results UI (base `e370889f4e`):
`source/interface/hud_messaging.c` now measures and wraps plain transient text
inside the existing physical row budget. Short-line spacing and item/icon paths
remain; the existing wrap flags are shared through `draw_string.h`. First native
capture (`build/native-extraction-hud/captures-operation/wrapped-but-truncated.png`)
exposes the second limit: `hud_message_definition.text[63]` had already truncated
the notice. Its ABI/save size remains unchanged. `extraction_local_notice` now
publishes bounded word-sized records in reverse enqueue order so the headline
draws first. Existing remote notice transport and persistence are unchanged.

All 74 connected Extraction workflows pass with the actual 62-character record
limit enforced at the HUD adapter, including stash/report/next-run content and
the existing ownership/refund/death/settlement controls. A deliberately dropped
report tail is detected. Three existing native Unicode parser cases pass after
sharing the wrap constants. Windows64 compiles incrementally in
`build/native-extraction-notice-build.log`, binary SHA-256
`16da2d098205ace79404c0646ba03a53c7dc8b98c497e12099e4471407a9a594`.

The real hidden-OpenGL local mission with an owned saved profile repeats native
deployment, combat, rescue, technology pickup, Flood/defense clearance, signal,
physical return and durable bank/completion (`native-notice-complete-operation.json`).
`captures-complete/complete-bank-report.png` visibly shows the complete report:
stash 2/32 weapons, 2 frag, 1 plasma, followed by next-run readiness, in order
without clipping/overlap. `complete-activation-notice.png` preserves all activation
instructions; the inherited wrapper still splits "Leaving" across rows. The
fixture's SDK action label is still a placeholder. No broad typography, remote,
split-screen, VR or final Spark UI acceptance is claimed. The old partial result
is retained. After both native processes exited normally, 2,659,723,770 bytes of
owned runtime/save-cache/BMP data were removed; compact captures/logs/receipt and
the small banked test record remain. The reusable accepted mission cache is untouched.

15 November, Flood feigned-death replication (base `761de4e44f`): existing
`network_objects.c` object states add an optional feigned bit; only for a dead
nonplayer biped with a positive native feign timer/body, `time` holds the existing
16-bit packed vitality. Vehicle prediction and player bodies keep their original
semantics. The client reuses quiet native death, retains the host's vitality and
clears its own wake timer; existing living actor states still resurrect it. A
later ordinary dead state removes positive feigned vitality. Layout remains
0x2C bytes; no advertised version, actor packet or checker tolerance changes.
Eighteen connected replica/legacy/player/vehicle/invalid/checkpoint/damage cases
pass, as do three deliberately broken encoding/restore/player-guard controls.
The actual previous receiver helper ignores the new optional bit/time and keeps
its grace period; it fails the new feigned-state expectation. This is a source-
coupled legacy check, not mixed-fork native acceptance. Windows64 compiles in
`build/native-flood-feign-build.log`, SHA-256
`5563ddc136eee88a6c61a640ac3b2f77afcbd15a2c466a917f034871af730421`.

`build/native-flood-feign-repair`: actual two-player native mission reaches
extraction availability, passing 102 compared seconds, mean/max player separation
0.019/0.057, zero/3060 divergent prediction ticks and 109 corrections. Unchanged
AI checker passes 107 snapshots/364 comparisons, maximum position difference
0.089222. Full item check passes 106 snapshots, zero out-of-step items and 13/13
new items seen. This run had no sampled positive corpse and alone does NOT prove
the feigned lifecycle. Two short attempts are retained: the first shot hits the
teammate; unobstructed shots then damage Flood, but native 40% spawn eligibility
does not exercise feigning. Their player checker ERRORs on insufficient on-foot
prediction samples; do not present either as synchronization acceptance.

`build/native-flood-feign-controlled/receipt.json` records a separate owned CE
cache with ONLY `characters/floodcombat_human/floodcombat_human.biped` feign chance
changed 0.4 to 1.0; threshold 0.3, native time 2.5s, damage and original accepted
cache remain unchanged. Ordinary sidestep/pistol/two clicks then no fire produce
two actual positive corpses at 0.625 on BOTH peers, with identical sampled wakes
at ticks 420 and 480 (`native-reanimation.json`). Unchanged AI checker passes
32 snapshots/50 comparisons, maximum difference 0.102736; no corpse normalization
is used. Full items pass 31 snapshots, zero out of step, 6/6 new items seen.
Player comparison passes 27 seconds, mean/max separation 0.015/0.041, 28/810
divergent prediction ticks, mean/max drift 0.000345/0.020852 and six corrections.
This targeted native repair is proven; mission completion/banking, human visual,
mixed-fork, other-platform and stress acceptance are distinct. All owned processes
are terminal before disposal of controlled cache/tags, peer payloads and bulk BMPs;
compact original failures, receipts, native logs and lossless captures remain.

15 November, native local operation/gear milestone (base `f036b81913`):
`game_modes_inventory_capture` previously refused a legitimate stowed weapon
whose native model parent is NONE. It now accepts that state only with native
attached-to-unit/player-item flags, the full player owner and no world connection;
another parent remains refused. The existing connected Extraction harness passes
73 workflows, including stowed capture, deployment, bank, redeploy and reopen,
plus wrong-owner/ground/unmarked/nonplayer/other-parent refusal. The old capture
fails the stowed loop. Deliberately bypassed player-owner and parent guards are
both detected. Existing transfer negative-control mutation is updated, not removed.
Windows64 compiles incrementally (`build/native-authored-aim-build.log`), binary
SHA-256 `86fc08ec8a56ef89cbd6463164a45433ba02c0bbecea5ada91e3e76fc6b7a02c`.

Actual hidden-OpenGL native evidence in `build/native-extraction-profile`:
native Settings creates a disposable saved profile; old binary refuses CE startup
with passive `debug.network_test=log`. The repaired existing startup path loads
`be_native@ce` with the profile and admits Extraction's controls; competing session/
server guards remain. `deployment.log` reproduces real stowed-ownership refusal;
the corrected build deploys. The inherited aim helper now targets AI in local solo
as in co-op, with no ammunition replenishment in either encounter mode. An early
external input-driver file-open race and the unaimed solo combat failure are
retained; neither is gameplay success. The fixture's 15-tick positive spawn wait
missed a short-lived defender; one-tick observation preserves the nonzero gate.

`native-escape-spawn-operation.json`, its native log and console show deployment,
real Covenant/Flood/defense combat, physical rescue, actual technology-placeholder
ownership, extraction activation at tick 1170, ten-second signal wait, physical
return and durable bank/completion. Gear was consumed from the native body.
`restart-operation.json` then proves a new engine process reopens two stashed
weapons and ordinary deployment issues them. `native-bank-redeploy.json` compares
the existing record bytes: version 1, generation 10 idle counts 2/0/0 to generation
12 deployed counts 0/2/0; the exact two item records (identity, cache, quantities)
are identical. Recovered items are the empty technology pistol placeholder and a
normal pistol, not Spark's final technology assets. This verifies local native
banking/callback/restart/redeployment, NOT remote banking or the full multiplayer
Deploy/Fight/Loot/Revive/Extract acceptance. Readable HUD/results UX remains open.

Two-player `build/native-black-echo-flood-defense/results.json` passes 102 compared
seconds, mean/max player separation 0.014/0.059, zero/2883 divergent prediction
ticks, 101 corrections and one sampled alive disagreement within the existing
player checker. Actual authored script reaches extraction availability. Full
item check passes 106 snapshots, zero out-of-step items, 16/18 new items seen.
However unchanged `actors.json` is FAIL: tick 2280, Flood full handle e3470012,
host dead body 0.625 versus client dead body -1.9375. Both are alive at tick 2310
with matching 0.625 health. Native `units.c` feigns death with positive vitality;
the existing object-state fallback quiet-kills the client, while `network_actors.c`
sends living control states and later resurrects it. No checker weakening, packet
or gameplay replay patch was included at that checkpoint. The repair above keeps
this original failure and adds direct native positive-corpse/reanimation evidence.
The earlier bad Flood spawn fell outside the world; the owned fixture now uses
the proven Covenant XY, z=1, and encounter-scoped force-active/no-dormancy commands.
Compiled fixture provenance remains in its existing `receipt.json`; no original
map or SDK changed. Disposable peer/runtime/cache/BMP payloads are cleaned after
terminal processes; compact evidence and one reusable compiled fixture remain.

15 November, client inventory attachment (base `bfa9c0c56e`): the existing
same-slot fast path now checks the weapon's full native parent. A fresh host
inventory repairs a detached item or stale claim by another unit through the
existing forget/release/attach operations, without creating a weapon. The actual
old source fails both added cases in `coop_checkpoint_recovery.c`, which extracts
real replica, datum, inventory and unit-network helpers. Both pass after repair,
along with 11 related recovery cases and all eight existing retried-inventory
cases. Two deliberately broken fast-path controls are detected. World graph,
renderer and transport remain fixture boundaries. Windows64 compiles in
`build/native-inventory-attachment-repair-build.log`, binary SHA-256
`f2097fc200190a53d9d65f5a371affd4eb3ae203ddbd0afc7b2e8494db89b61b`.

The opt-in existing AI readback now also samples dormant world bipeds and their
actual inventory slots. `build/native-flood-owner-census` reproduces loose
rifles while confirming Flood bodies exist at matching host/client positions;
`native-flood-inventory-census` later has matching attachments without a gameplay
patch, establishing that the earlier failure is intermittent. The stale-slot
repair is proven by the connected old/new source cases; the initiating native
ordering race is not claimed fully explained or eliminated.

Actual corrected-engine hidden OpenGL co-op:
`build/native-inventory-attachment-repair/results.json` passes 87 compared seconds,
mean/max player separation 0.018/0.057, no mismatched seconds, 0/2610 divergent
prediction ticks and 76 client corrections. Existing broader item comparison
passes 91 snapshots, zero items out of step and 8/8 new items seen (`items.txt`).
Full-world actor comparison passes 92 snapshots/241 unit comparisons, maximum
separation 0.099239; dormant Flood bodies are included rather than omitted.
Native walking, terminal activation, combat, Marine rescue and technology pickup
repeat. The controlled client `objects_detach` attempt is REFUSED by the existing
"host decides the game" guard (`client-console.txt`); no orphan was injected and
that attempt is NOT native repair acceptance. Guard remains unchanged. Outbreak
completion, extraction/banking, reconnect/restart gameplay and other-platform
acceptance remain OPEN. Disposable peers, map staging and bulky captures are
removed; compact original failures and one reusable owned fixture remain.

15 November, authored-operation handoff (base `06f474a05d`): optional dormant-void
`forerunner_extraction_complete` wakes once per fresh map only after all living
owned survivors return and finish durable equipment settlement. Existing native
`hs_wake_by_name` and Survival return checks are reused. The connected MSVC world
passes the existing 66 workflows plus the added return-type case: partial-team
return, pending writes/death, wrong signatures, restored worlds and redeployment
cannot produce premature/repeated completion. A deliberately broken survivor
gate is detected. HS execution/identity/OS adapters remain fixtures. The existing
death-timing test also passes 15-1000 FPS timing, co-op blocking and spectator
ownership after restoring its current mode adapters. No new test framework.

Windows64 compiles incrementally (`build/native-mission-final-build.log`), SHA-256
`aa76e05b254918f08ab783d3346c13f0e4a5fe0975aca34c6e08d7fee8106d17`.
Spark read-only revision d50b6fa records a Black Echo MCC13 compilation, but its
cache was disposed. The owned HCEEK subset produced one CE609 native fixture,
`tools/fixtures/black_echo_native.hsc`, ordinary controls/volumes/encounters and
a distinct pistol-definition technology placeholder. Original SDK archive hash
remains `6c9c156237c78ab1b983080495fdcff698a3f15fe0baaadb1c663654d27f7bd4`.
The retained final cache hash is
`0dce8f4a570c76349d64ea745c733ed09d5cf624409fe066cac9731d71b88010`;
source/readback provenance is `build/native-black-echo-fixture/receipt.json`.
No proprietary map/tag payload is committed. Initial fixture trials corrected
angle units, cliff/terminal placement, missing player-human allegiance and the
native rescue squad route. Original behavior/interaction reach were preserved.

Actual hidden OpenGL two-player CE co-op, existing director OFF:
`build/native-black-echo-native-loot/results.json` passes 117 compared seconds,
mean/max player separation 0.014/0.055, aim 0 degrees, no mismatched seconds,
0/3507 divergent prediction ticks and 6 client corrections at unchanged limits.
Real native Action deploys, HaloScript places hostile/friendly actors, actual
weapon fire clears Covenant, `ai_migrate` walks Marines to the rescue volume,
and actual technology weapon ownership completes objective 4. The script then
reaches optional Flood activation; outbreak completion is NOT verified. Native
actor checker passes 122 snapshots/271 unit comparisons, maximum separation
0.046945, common movement/damage/death. This covers its tracked units only.

The broader existing item checker FAILS (`items.txt`): at tick 1770, Flood rifles
18/20 have host parents 17/19 while the client shows them loose. Do not interpret
the tracked-actor PASS as complete Flood/body/inventory synchronization. Native
technology pickup and later drop are observed separately on both peers; no bank
transaction or reward is demonstrated. Earlier placement/input runs and runs
with insufficient prediction samples retain their compact PASS/ERROR evidence.
`build/native-black-echo-live-approach` passes 47 compared seconds and its actual
terminal capture reports missing saved-profile refusal. Remote persistent owners
still require existing verified host-bound Delta proofs; no fixture identity or
LAN-security bypass was added. Native callback, extraction, full mission, larger
squads, other platforms and hardware acceptance remain OPEN. SDK/map originals
rehash unchanged. Peer/runtime staging, SDK subset and bulky BMPs are removed;
one reusable 106 MB fixture and compact native evidence remain.

15 November, co-op spectator recovery (base `50a24056c2`): the existing
`tools/test_coop_spectate.py` now compiles actual selection, input-edge,
body-delay and HUD functions together. Its MSVC x86 `/O2 /W3 /WX` workflow
passes sole-survivor wrap, two-survivor cycling/held keys, quit players, stale
body/player handles, changed ownership, native dead flags, HUD departure races,
three-second own-body delay, respawn and invalid local slots. Three deliberately
broken versions detect missing wrap, departure and ownership checks. World and
HUD adapters remain fixtures. No replacement spectator manager or protocol.

Windows64 builds: `build/native-spectator-recovery-build.log` and final caption
compile `build/native-spectator-recovery-final-build.log`; final executable
SHA-256 `6f20bf7ee666fcdd3d93ebbc8d6ef6f250ff35f17d82c4954b73052df1931c84`.
Native hidden OpenGL zero-lives/endless B40 Firefight:
`build/native-spectator-elimination/results.json` passes 66 compared seconds at
unchanged limits, mean/max position separation 0.029958/1.349904, aim 0 degrees,
one position-boundary mismatch; drift 61/398 samples, mean/max
0.000319121/0.006441, 144 client corrections. Script kills the joining player at
1306, dead state is observed at 1320, and two existing next-key inputs follow at
1440. Visually inspected `captures/spectating-before-next.png` and
`captures/spectating-after-next.png` retain the same `SPECTATING Player 2` view.
An initial ordinary-co-op run (`build/native-spectator-recovery`) passes 67
compared seconds but respawns before the helper's delayed input; its stimulus
remains ERROR, not cycling acceptance. Existing zero-life rules supplied the
corrected spectator window; gameplay respawn timing was not altered.
The final non-promissory waiting caption is compiled, not recaptured; retained
images show the old text. Native disconnect/last-target fallback, large squads,
other platforms, human and VR acceptance remain open. Pixel identity was checked
on PNG conversion; owned BMPs/maps/peer payloads/input replay are removed and
original map hashes remain unchanged. No unrelated engine suite was repeated.

15 November, native forced-death ownership (base `a9c8551a0c`): the actual prior
functions in the existing C fixture reproduce pending-death equipment capture,
silent-death medical guidance and revival by a doomed healer. The repair passes
62 connected workflows under MSVC x86 `/O2 /W3 /WX`; three broken controls detect
missing inventory admission, missing silent/no-statistics handling and missing
final-tick healer recheck. Cases cover all three native kill requests, refused
deployment/escape without a stash write, legitimate retry after a request clears,
native death followed by survivor recovery of the same item identity, and
late treatment interruption. Native drops/identity/storage in this fixture are
adapters, not full multiplayer inventory acceptance.

Windows64 incremental build: `build/native-pending-death-build.log`, executable
SHA-256 `4ba14da262106cd78d86aa344308a0fe41336eb8e66a98b8f819ea12b0844be4`.
Actual hidden OpenGL B40 co-op: `build/native-medical-forced-death/results.json`
passes 77 compared seconds with unchanged limits: mean/max position separation
0.020791/1.349904, one position and one alive-boundary mismatch, aim 0 degrees;
drift 65/623 samples, mean/max 0.000206058/0.006441, 201 client corrections.
The casualty downs at tick 1200; held-Action treatment starts at 1500. HaloScript
`unit_kill_silent` at 1576 kills the healer (network player 1), both peers show
that death at 1590/1650, and no revival completes. Rifle object 238 moves from
holder 228 at 1560 to world parent -1 at 1590 on both peers. The casualty later
dies by native bleedout (2130). An initial helper incorrectly expected network
player 0 to die because it assumed HaloScript list order matched network order;
its final assertion failed despite the native comparator passing. Corrected
readback checks both completed logs and the exact rifle transition without a
rerun; the initial error and corrected acceptance are retained in `guidance.json`.
Maps/peer copies/input replay and compile scratch are removed. This verifies a
bounded desktop interruption/drop path, not native death-loot banking, authored
Extraction, physical VR, WAN or other-platform acceptance. No wire/save change.

15 November, prepared native grant recovery (base `614d2db342`): compilation of
the actual prior committed `gear.c`, `extraction.c` and `game_modes.c` in the
existing connected fixture reproduces lost ungranted equipment after a refund
outage survives disconnect. No old checkout or full baseline build was needed.
The current production functions pass 45 connected inventory workflows and 18
direct record cases with MSVC x86 `/O2 /W3 /WX`. Added cases cover outage through
disconnect/recycled slot/map exit, uncertain preparation publication, failure
after native application, rollback of only new grenades/objects, compatible
retry, confirmed loss, crash reopen, exact item identities and published-confirm
retry without a second commit. Three broken integrations detect missing reopen
refund, missing native rollback and wrongful revocation after published intent.
Game/identity/OS-failure adapters in these checks are fixtures.

The existing native-file test now also compiles the actual gear codec/state
machine with the existing Windows file backend. The exact process-death test
body from `tools/test_gear_files.py` runs against that MSVC x86 driver:

- Kill the owned process after durable prepared phase 3: a second opener is
  refused while the lock is held; after termination, the same two weapons,
  quantities and grenades return to the stash. Reopening again preserves them.
- Kill after confirmed phase 1: the same exclusion holds, but reopening loses
  issued gear and awards nothing. Repeated reopening cannot refund it.

These are actual Windows files, locking, atomic replacement and process
termination; gameplay/tag lookup still uses bounded adapters. They do not prove
physical power loss or native authored-map deployment. No actual Halo map, user
profile or saved inventory was opened. Prepared rollback may leave previous
carried equipment as existing native world drops, as on the old grant-failure
path; it does not promise an atomic swap of every preexisting carried object.

Windows64 final incremental build: `build/native-prepared-grant-final-build.log`,
SHA-256 `c85193fe30fb7e39b0c7033dc2f07a151dc3f444504edf7c33b848f7713b4d41`.
Version-1 binary layout is unchanged; phase 3 is additive. Older readers reject
an in-flight prepared record without modifying it; this updated runtime must
finish/refund it. Existing phase-1 records retain their prior loss semantics,
so this is not a retrospective refund of ambiguous legacy runs. Protocol and
native saved-world layouts are unchanged. Child processes, test records and
compile scratch are removed. Other platforms and full native gear acceptance
remain open; unrelated gameplay/hardware checks were not repeated.

15 November, connected native revival guidance (base `6a0d131f7e`): the existing
Extraction C harness compiles/passes 35 workflows with MSVC x86 `/O2 /W3 /WX`.
New cases exercise the actual medical eligibility and native HUD selection/draw
helper: friendly/hostile/self/dead/pending-death/quit/ownership gates, invalid
local views, body salt and epoch changes, client receive/recovery, zero-quantized
living health, four nearest of seven casualties, native occlusion/interpolation,
missing-arrow/bitmap fallback, private five-second reminders/final one-second
urgency, and recovery/restore cleanup. Five broken controls detect the captured
positive-health regression, enemy leakage, unbounded draws/traces, disconnected
native drawing and reminder spam. Existing inventory, evacuation and medical
cases remain passing. OS, identity, collision, HUD draw and vitality transport
adapters are fixtures; these checks are not native multiplayer acceptance.

Native Windows64/bgfx/OpenXR compilation uses the existing cache. Rendered
acceptance uses existing `net_compare.py --window`, B40/co-op, seconds 75, skip 5,
start 8, timeout 130, hidden OpenGL windows and XR explicitly false. Both copies
stand still using `HALO_TEST_INPUT=menu:wait`; only the host enables `HALO_REVIVE`.
Existing native cues wait for two bodies, pause AI, and prepare low vitality;
the existing 40-second shot simulator produces one casualty. An event watcher
uses the existing input-file interface for screenshots and nine seconds of held
E starting at tick 1500. No new engine probe or testing framework. On both host-
and client-healer runs, logs show incapacitation at tick 1200, treatment from
1500 and completion at 1650; both peers show the same body's 0.25 health and
native medical epoch 1 entry/exit. Input comes from replay, not a human player.

- `build/native-medical-guidance-revival/results.json`: **PASS**, host healer,
  77 compared seconds, tick offset 0, mean/max position separation 0.018023/1.349904,
  aim max 0 degrees, one position boundary mismatch within unchanged limits.
  Drift 61/815 samples, mean/max 0.000155817/0.006441; 216 corrections. Native
  captures show the blue casualty arrow and its disappearance after treatment:
  `captures/downed-ally.png` and `captures/revived-ally.png`. This binary predates
  the message shortening and client eligibility correction.
- `build/native-medical-guidance-client-healer/results.json`: synchronization
  **PASS**, 77 seconds, but client HUD presentation **FAIL**. A living healer's
  tiny positive health rounds to zero in existing native vitality encoding;
  requiring positive health hid its ally marker. The missing-marker PNG and
  original logs remain. Native client treatment itself succeeds; that does not
  establish a correct client HUD. No vitality encoding or damage rule changed.
- `build/native-medical-guidance-client-fixed/results.json`: **PASS**, joining
  player heals host, 77 seconds, tick offset 0, mean/max separation
  0.011057/1.349904, aim max 3 degrees, one position boundary mismatch. Drift
  23/1607 samples, mean/max 0.000025348/0.003364; 223 corrections. Actual inspected
  `client-downed-ally.png` shows the corrected native arrow; the matching
  `client-revived-ally.png` shows completed treatment and marker removal.
  `casualty-entry.png` preserves the host warning and its remaining text clipping.
  The final source shortens that casualty caption further; it compiles but is
  not recaptured. Both successful native verdicts use unmodified judging limits.

Failed preparation is preserved separately: `native-medical-guidance-rendered`
passes ordinary synchronization for 37 seconds but never reaches its injury
cues after B40's delayed second-body spawn; it is not casualty proof.
`native-medical-guidance-casualty` passes 77 seconds but its stimulus assertion
**ERRORs** because shields recharged before the shot. After those two approaches,
exact native ticks/shield samples and native shield-depletion source established
the cause. Preparation at cue offset 4.4 (native tick 1198) immediately before
the tick-1200 shot produces the accepted casualty. No speculative engine damage
patch, relaxed gate or relabeled failed presentation was used.

`build/native-medical-guidance-client-fix-build.log` compiled the final native
client acceptance binary, SHA-256
`629fbd277f1fa6020fdd080129191f642eea95d012b2c5183336ae9df3bc22d5`.
Final text-only casualty shortening compiles in
`build/native-medical-guidance-caption-build.log`, SHA-256
`1254fc986a3425966864fe21c5a6b50a63925796b33e2c8bd760d558a8c73dd5`.
Compact PNG conversions preserve identical pixels. Bulk BMPs, copied maps, peer
data and compile scratch are removed; protected ui/B40/carousel hashes match
`build/native-firefight-pursuit-inputs.json`. Existing native HUD resources,
medical metadata and private notices are reused; scripted nav-point slots and
saved layouts are untouched. Native dense/multiple casualties, hostile PvPvE,
other platforms, full operations and VR hardware remain unverified.

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
