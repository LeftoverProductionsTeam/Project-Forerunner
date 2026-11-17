# Forerunner engineering checkpoint

Updated: 17 November 2026. The persistent engineering goal remains active.
This is an incomplete engine integration, not a release or gameplay acceptance claim.

Owner's current acceptance order: A, two independent clients using verified Delta
identities extract/bank, disconnect, restart and redeploy without duplication or
loss; B, Spark's delivered CE609 Black Echo and real mission outcomes; C, actual
UNSC/Covenant selection, compatible equipment/allegiances/objectives, Pelican/
Spirit insertion, physical boarding/departure, fly-out, recovered gear/casualties
and next deployment; D, independent six-member squads, separate outcomes and
opposing PvPvE, then connected MegaWorld/larger targets. The regression fixture
is not the finished mission. The owner authorizes Chupathingy's existing Delta
account/Steam setup for testing. The original account and the owner's confirmed
ForerunnerTest account now authenticate as distinct, bound players in private
native sessions. Original identity files remain unchanged; credentials and new
device recovery material stay in owner-only ignored storage.

VR input lifecycle repair (17 November, base `8e2ca5e0a2`): the existing OpenXR
controller bridge can resume held fire/movement after focus loss or a menu.
The retained focused reproduction fails on the original source. Reuse its existing
layout neutral-input gate for focus/render loss, disconnected/stopped sessions,
menu return and reference-space changes. Pause/menu remains available while
waiting; valid release restores native gameplay and opt-in PTT. No settings,
protocol, renderer, inventory or simulation changes. Windows64 incremental
compile/link and 62 focused existing input checks PASS, including both hands,
five resume boundaries and existing pickup/bracing/privacy regressions.
`build/native-vr-resume-fallback/result.json` PASSES the native OpenGL desktop
menu replay with grip/bracing enabled and XR explicitly disabled; final PNG
inspected. Binary `b64d2a518f38d099e255949c9ed912762c1fb9ee9433883a2584e2aa101f4807`.
This proves desktop fallback, not actual headset resume, interaction or stereo;
VR hardware/mixed-client/platform acceptance remains open.

Actual a10 mission progression attempts `native-campaign-a10-mission-approach`
and `native-campaign-a10-mission-exit` remain FAILED, not campaign acceptance.
Normal-input routes stop at cryobay geometry before weapons/combat/objectives;
the first progress observer counts jump height, the second accepts a doorway
corner too early. Neither establishes an engine defect. Stop repeating these
routes or modifying production collision to compensate. Both accounts and owned
maps unchanged; owned runtime/map copies removed. Byte-verified lossless PNGs
retain both failures; 22,587,544B BMP/duplicate PNG scratch removed across these
and the VR fallback. No live owned jobs, Spark writes, hosted jobs or push.
Spark `456fb33` documents an aircraft-only placement trial, explicitly disposed
and outside the original return volume, not a corrected delivered Black Echo.
The exact coordinated authoring dependency remains in the existing contract.

Native Forge cloning and product menu (17 November): 6abe3f8d56 connects
Faber-derived console clones to existing co-op appearance synchronization.
Native zero is the unscaled model; cloning and the existing receiver preserve
zero resets. Host/platform, scenery, attachment/objective and quota restrictions
remain. Existing vehicle checks pass (46 initially, then 14 affected final clone
cases); unchanged checks were not repeated. Windows64 compiles.
build/native-forge-scaled-clone-restore/result.json PASSES two verified Delta
accounts: both source/clone scales .500, source zero reset, a default-scale clone;
21 seconds/42 player observations agree. Binary:
13de17bf09a3486e32992d721faa66e463c323921067cafe4e94f4009b6c45ab.
Earlier bounding-center half-distance assumption remains a rejected observer,
not a runtime failure or a full Forge editor claim. Original identities unchanged.

The shared native main menu connects Campaign, Multiplayer, Extraction, Firefight
(authored/runtime waves), Zombies, Forge sessions, Spark missions, VR/settings,
profiles and Mods. Native profile creation, host/map/difficulty/setup, mode writer
and Back are reused. Exact action matching fixes an observed mode-prefix bug.
Original icon/banner and actual window/executable/About identity say Forerunner.
Upstream notices and profile/credential folders stay intact; no second launcher.
build/native-forerunner-menu-fixed/result.json PASSES actual Gearbox-style profile
creation, mode choices/exclusion, map choosers, persistence and back navigation.
Native Chupathingy-style root, setup/lobby, HWND title/icon and Windows product
metadata inspected. Twenty-one focused menu checks pass; wrapping corrected.
Menu binary bd5943287e3d094e91db1c7ab19f1b90b2459215393442b973631a2eb44521fa.
First menu run remains rejected for the fixed rule bug. Full mode/editor/VR/
platform acceptance stays separate. Owned copies/BMP scratch cleaned. No push,
hosted jobs or Spark writes. Newly retained Spark c388d3d6 is the next real Black
Echo candidate; do not repeat the older fixture as mission acceptance.

Forge placement undo (17 November): the host can remove the last unused
copy with forge_undo. Occupied, owned, named and stale objects are refused.
Map changes and save restores clear the handle. Windows64 and 27 focused
checks passed. Two authenticated clients observed the positioned copy disappear
and the original remain at (65, -80, 3.6) in build/native-forge-undo/result.json.
A separate native run confirmed Profiles opens with controller Start and the
Forerunner window title and icon load. Both isolated runtime copies were removed.

Zombies now has Host Infection beside the authored Flood Survival entry.
The native Windows path created a profile, chose Battle Creek, preselected the
built-in Infection variant, and reached the Infection lobby. Evidence:
build/native-forerunner-infection-menu-lobby/result.json and its two captures.
Windows64 and 18 focused mode/setup checks passed. Infection gameplay rules
were unchanged, so the previously accepted match was not rerun.

Extraction squad admission (17 November): a live operation now locks both
joining and leaving its roster. The focused reproduction failed on the original
runtime. Windows64 and 48 squad/objective checks pass, including sequential
deployment by existing members and membership reopening after settlement.
build/native-extraction-squad-roster-lock-live/result.json verifies two bound
clients: first squad deployed, second player received the host's SQUAD LOCKED
reply at that terminal, then joined and deployed with their own squad. 65 native
seconds and 130 player observations agree. The earlier driving attempt stopped
before terminal use. Both test sessions' disposable runtimes/maps were removed.

Forge crosshair workflow (17 November): forge_pick selects a loaded object,
forge_place copies it over clear BSP ground, and forge_undo removes that copy.
Vehicle placement uses native mass points and collision features. Windows64 and
37 focused checks passed. In build/native-forge-crosshair-copy/result.json both
authenticated clients saw the copied Pelican at rest at the same position and
saw its removal while the original remained. The slope did not meet the separate
dropship landing-attitude gate. Runtime copies and BMP scratch were removed.

Co-op campaign guest credit (17 November): the host's existing four-byte
game-over field now carries a stock mission win tied to the mission and
difficulty. Guests credit only active local profiles from a matching result.
Ordinary ends and non-stock workflows retain zero, and no packet size/version
changed. Windows64 and focused checks passed. Two independent native LAN
profiles each saved A10 Normal credit after a controlled win and both
returned to the A30 lobby. Evidence: build/native-coop-guest-campaign-award/
result.json and build/native-coop-campaign-credit-guards/result.json.

Campaign profile writes (17 November): desktop builds stage and flush the native
512-byte record before replacement using the existing whole-record writer. A
failed update no longer deletes the profile directory. Windows64 and three
focused storage checks passed. Two LAN clients reached A30 after the host's A10
Normal credit committed. Denying replacement on A30 left the committed profile
byte-for-byte intact. Evidence: build/native-campaign-profile-atomic-verified/
result.json. Original-Xbox, console and browser paths retain the legacy writer
with failed-update deletion removed.

Forge pause controls (17 November): host from Forge > Create Session and open
Forge Tools in the live pause menu to select, place, and undo a copy. Existing
authority, object restrictions, quota and replication apply. Windows64 and
seven focused permission/quota checks passed. Two independent LAN clients in
build/native-forge-menu-end-to-end/result.json saw the same Pelican copy and
removal, and the guest had no Forge Tools row. Controller and mouse actions
passed without console edits. Placement save/load and editor parity remain open.

Campaign co-op handoff (17 November): the next round preserves the host's
friendly-fire setting across the playlist reset. The real reset test reproduced
the loss on the original code; 14 focused checks and Windows64 passed. Co-op
clients now retain the mission choice and return to the ready lobby. A controlled
A10 game_won signal exposed the host's old map picker pausing A30.
build/native-campaign-round-ready-lobby/result.json verifies both authenticated
clients returned to the A30 lobby and loaded two equipped players into A30
through Start Now. The probe used disposable temporary profiles and did not
verify completion credit or a full playthrough. Runtime copies were removed.

Stock campaign credit (17 November): mission completion uses the exact stock
cache name through the existing campaign index. Scenario paths and .map names
resolve, while similarly named maps and CE/MCC identities stay outside the
stock progress table. Windows64 and 13 focused checks passed.
build/native-campaign-exact-credit/result.json verifies ordinary LAN hosting
with a fresh profile and a controlled A10 win. The saved Normal completion flag
changed from 0 to 2 and the other nine flags stayed zero. This covers the host's
local profile. Remote profile credit and full playthrough remain open. Owned
runtime/map copies and BMP scratch were removed.

Native menu completion and Black Echo crew repair (17 November):
63db9dd330 connects the existing headset, renderer, aim/hand, turning, grip and
two-hand settings directly to the native VR page, Save/Cancel and persistence.
build/native-forerunner-vr-menu-ack/result.json PASSES native settings readback;
binary 3f51171baf57051fda6a98c1147e977a07c434633849f23ced33e3e81cfff237.
This is desktop menu evidence, not headset acceptance. Non-OpenXR builds omit
that row. Product menus have clean feature names without development labels.

Actual Spark c388d3d69219ba5b99f53729905b81d9baa43a57582eb016cd1c636f2c636649
previously lost gs_pelican_pilot and gs_pelican_trip: 24 compiled unit-to-vehicle
casts were rejected. hs_runtime.c now accepts only that CE conversion, verifying
the live vehicle type and refusing stale/wrong objects. Other casts stay strict.
52 existing script-object checks plus the new live-type negative control PASS;
Windows64 incremental compile/link PASS. Subsequent native host readback found
two pilots instead of the authored one: default 50% enemy scaling ran before
the mission establishes player/human allegiance. Main native/Extraction entries
now select the existing no-extra-enemies option; ordinary co-op stays configurable.

build/native-spark-black-echo-authored-crew/result.json PASSES two authenticated
clients on the unchanged Spark cache: pilot count 1, Marine count 1, both physical
native seats true, gs_pelican_ready true; 28 seconds/56 player observations,
zero divergences. Binary e4de3ea12160e156745b2e4f8b18815dfb391f72b20845c1fdd72711cb0179b6.
Earlier coordinated/input/crew-readback attempts remain FAILED full-operation
verdicts; their input navigation is not evidence of a collision/terminal defect.
The full candidate's human boarding, round trip, objectives/combat, settlement
and redeployment remain open. Owned accounts/maps unchanged; runtime copies and
losslessly converted BMP scratch cleaned. No Spark writes, hosted jobs or push.

Actual Black Echo insertion (17 November, base 1269d65324):
Spark Missions > Host Extraction now reaches the real CE609 c388d3d6 hub from
map/difficulty selection, setup and lobby (native-forerunner-spark-black-echo-launch).
Extraction latches native script actions regardless of the saved legacy co-op
follower setting. The focused harness verifies individual recipients, map-start
latching and legacy restoration (20 checks). Windows64 compile/link PASS.

native-spark-black-echo-native-passengers used two authenticated players and
legacy adaptations configured true. Both selected UNSC and deployed, but only
the host boarded before takeoff (tick 1560). The native Pelican landed in the
field (73.760,-125.796,2.178) at tick 2460, up .992. The host and live Marine
disembarked. The pilot flew the empty passenger cabin back to the hub and
settled at (50.660,-107.165,1.774), up .990, by tick 4350. The second player
deployed at tick 2100 and reached the returned ship after the authored script
had locked it pending evacuation. 142 seconds/284 player observations, zero
divergences. Both must deploy before boarding in the next run. Marine rescue,
combat, two-passenger insertion, extraction banking and redeployment remain open.
Full result stays failed. Source binary 97d80b9318323c8bdfdf5bf2f8b25bea9d7f9fde3cdaf02f07aa09996fdbaf60.

Native vehicle-action repair (17 November, base `5f11113156`): read-only owned
a10 tag readback confirms the cryotube and its CT-driver seat both reference the
HUD table's placeholder entry zero. Missing/empty/placeholder vehicle names now
use generic board/exit/flip prompts through the existing custom-state text and
action binding. Valid authored names, including custom entry zero, stay native.
The observed literal `%action` exposed a second integration defect: custom text
only parsed control icons while a multiplayer engine ran. Custom state text now
uses the existing parser in campaign too; authored objective/message elements
retain literal rendering. No boarding, animation, collision, simulation, packet,
map or asset edits. Windows64 incremental build and two focused checks PASS.

`build/native-campaign-a10-vehicle-prompts-markup/result.json` PASSES two bound
accounts on unchanged a10: the native HUD shows the action icon plus "to board",
ordinary E input boards at tick 1200, another press starts the native exit
animation and the player is on foot at 1590, then walks clear. Prompt and both
final PNGs inspected. Player comparison: 58 seconds/101 living observations,
no mismatches, position maximum 0.124; 62 item snapshots agree, all nine new
host items seen; non-cinematic AI comparison passes 48 snapshots/2,108 units.
Binary `cc7aca56b51f916785734508068bd6e655a8b2368db09885f0d5dd60e6819f7e`.
Earlier attempts remain rejected for UI acceptance: first captures were obscured
by the objective/animation; the next displays literal control markup. Their
physical-round-trip and synchronization evidence stays separate. Exit/flip
fallback rendering, localization, solo/platform/hardware, full campaign combat/
completion and Black Echo remain open. Original maps/accounts unchanged; owned
runtimes/maps and duplicate BMP/identical PNG scratch cleaned. Spark read-only;
no hosted jobs or push. Advance actual mission gameplay, not another prompt run.

Native cinematic repair (17 November, base `a5c0869b72`): retained a10 fighter
positions trail the host by about four ticks. The existing unsteered path adds
gameplay interpolation while the cinematic camera follows the host timeline.
Co-op cinematics now use the existing sample's host timestamp/path velocity with
bounded stale-sample return; ordinary gameplay/non-co-op retains its glide.
The first focused native run fixes the positional discrepancy but its inspected
client image exposes oversized fighters. Existing `host_send_object_looks`
wrongly assumes the first observed appearance is unchanged: same-tick creation/
script scaling is never sent. First appearance now uses the existing scale/
permutation message and periodic recovery, including reused object slots.
No new wire format, protocol, renderer, flight or synchronization subsystem.

Windows64 incremental compilation and 34 focused existing vehicle checks PASS,
covering cinematic latency, gameplay/non-co-op glide, stale/stop/teleport behavior,
first scripted appearance, refresh and recycled ownership; the old glide fails
the negative control. `build/native-campaign-a10-cinematic-appearance/result.json`
PASSES two bound accounts on unchanged owned a10 through tick 960: 32 item
snapshots agree, all 15 new host items are seen, and all 24 original-interval
fighter comparisons are within 0.421 world units (previously about 2.0–2.8).
28 player observations agree; no native exception. Both final PNGs inspected:
the formation has matching apparent scale, without a pixel-exact camera claim.
Binary `a43c05f472c7deaeb6f426fe243a96920cdb37fc26c57d484b1662941b0ab164`.
The earlier path-only run remains rejected for its observed appearance defect;
prior failed natural opening is not promoted. Full campaign/control/combat on
this binary, audible/human/VR/platform acceptance and Black Echo stay open.
Cryopod interaction strings are resolved in the scoped repair above. Originals
and identities unchanged; owned runtimes/maps and 19,661,016B of duplicate BMPs
cleaned. Spark remains read-only; no hosted jobs or push.

Native campaign co-op repair (17 November, base `a90a098c02`): untouched owned
Xbox a10, two bound Delta accounts, exposed a client halt during the opening's
bridge-to-space BSP change. Matching PDB readback identifies
`players_compute_combined_pvs` using a retained activating cluster number from
the previous BSP, then `structure_bsp_get_cluster_pvs` asserting at line 36.
The existing object-disconnect step now clears numeric cluster activation;
object-based activation survives its normal reconnect. No assertion removal,
protocol, renderer, map edit or alternate lifecycle. Separately, the common
`game_is_cooperative` now recognizes native network campaign mode instead of
only the local split-screen spawn count. This reuses the MCC adapter's existing
condition; that adapter now calls the common check. Solo and split-screen keep
their existing result. Windows64 incremental compilation and 36 focused existing
BSP/script-player checks pass, including detection of the old stale activation.

`build/native-campaign-a10-opening-fixed/result.json` PASSES the bounded native
opening with normal two-machine skip voting, both players gaining control and
normal walking/jumping observed for 1,050 ticks. Player comparison: 51 seconds,
87 living observations, no mismatches, position maximum 0.0422; 55 item snapshots
agree and all nine new host items are seen; 41 non-cinematic AI snapshots/1,800
unit comparisons agree. Both final PNGs inspected. Exact binary
`23c99f3326b3be1f07043ddcf51cc1a8ad92652114b7ca4523f945abdb44f821`, original
a10 SHA256 `f7461e5b6eaf0314c4f03a1c6e43752b06ae3e4782b70b240a8c15e5f634b3ce`.

The same binary's no-skip natural introduction traverses BSPs 0/7/1/8/0 without
the original exception and gives both players control. That separate run remains
FAIL in `build/native-campaign-a10-natural-opening/result.json`: its 260-second
bound leaves only 300 controlled ticks before the unchanged 900-tick minimum,
and its item check finds 22 moving-fighter discrepancies during the cinematic
(about 2.0–2.8 world units). All 103 new host items are seen, player comparison
has no mismatches over 244 seconds/255 living observations, and non-cinematic
AI comparison passes 16 snapshots/655 units. Do not promote this failed run or
relax the item tolerance. Both final PNGs inspected; the host's cryopod boarding
prompt has missing vehicle/seat strings and stays open for review. First crashed
run stays rejected. Full campaign combat/completion, audible audio (dummy driver),
human/platform/VR acceptance and full Spark Black Echo remain unverified. Next
campaign cinematic diagnosis is resolved in the scoped repair above; do not
repeat an unchanged full opening. Original accounts/maps unchanged; stopped-job
runtime/map copies and 117,966,096B of duplicate BMPs cleaned. Spark is read-only;
its authored pad correction is still a dependency. No hosted jobs or push.

Native Firefight setup (17 November): existing Server Setup now links a native
Firefight Options screen for wave limit/endless, initial enemies, per-wave growth,
shared lives, resupply and desktop persistent gear. It reuses the existing option
writer and director; no new mode, configuration system or protocol. KEEP CURRENT
preserves custom configured integers. Co-op-only entry visibility and shifted help
indices are wired; generated XML matches its generator. Windows64 compilation and
focused real-source/generated-menu checks pass. Native option-screen Save,
reopen, Cancel, custom 999 preservation and returning to a preset PASS in
`build/native-firefight-options-menu-config/result.json`; stable later PNGs inspected.
An initial observer failed by requiring omitted default config keys; its run stays
rejected. Initial cold-open PNGs can show briefly blank spinner values; later
frames show the correct values. Full human/other-platform UI acceptance stays open.

The saved configuration was consumed unchanged by a two-account native encounter:
two then four Flood, score 600, resupply and three initial shared lives. Both
players die once, use the native paid respawn/loss kit, and finish alive with one
weapon each. Two checksum-valid escaped records each bank that one weapon with
distinct IDs and no reservation/escrow left. This is the intended death-loss
policy, not duplicated missing secondaries. Actual evidence is retained under
`build/native-firefight-options-growth/result.json`: 45 player seconds/67 living
observations, one transient alive mismatch/no persistent divergence; 49 item
snapshots agree (18/19 new host items seen); AI comparisons pass with all six
damage/death histories common. Its overall observer remains FAIL because it
required two-weapon banks after casualties; a missing post-stop debug helper also
raised, but outer cleanup retained the raw debug logs. No harness gate was relaxed,
native outcome overridden or engine patch invented for those observer mistakes.
Original accounts/cache remain unchanged; stopped-job runtime/maps and duplicate
BMPs cleaned. This connects host setup to actual bounded wave growth and casualty
banking, not a no-death two-pair run or the original Zombie Apocalypse experience.
Black Echo's authored hub/pad correction remains with Spark; the original Zombie
Demo still requires its reviewed native conversion. Linux native execution is not
available on this machine (no installed WSL distribution); no cross-platform or
large-horde claim is made. Full engine goal remains active; no hosted work/push.

Infection and partial-install hosting (17 November): the inherited mode passes
one bounded native Longest match with two verified Delta accounts, normal walking
and skull melee, human conversion, zombie victory, real postgame/lobby return and
a fresh countdown. No role/health/position/outcome setter or mode rewrite.
`build/native-infection-longest-complete/result.json` retains separate first-round
and restarted-round comparisons: 14/6 seconds, 28/12 observations, no mismatches;
25/10 item snapshots agree. Both native victory PNGs inspected. This uses accepted
binary `6744161c…1cc09`; timeout victories, late joins, other maps/platforms and
human acceptance remain open. Earlier attempts stay rejected: omitted lobby cache,
terrain-blocked walking, then a premature check that counted stdout/debug twice.
The corrected check requires two distinct starts plus actual next-game/game-2
transitions and both peers' reset. No prior result is promoted retroactively.

The missing-cache attempt exposed a real engine hosting dependency. Native fast
setup selected absent `carousel` before its caller could choose installed Beaver
Creek. `game_engine_playlist_next` now uses the existing native multiplayer list
only when the implicit desktop default is absent; installed classic defaults,
explicit choices/refusals and dedicated rotations stay unchanged. Windows64
incremental build, two focused checks and the exact host-only native reproduction
pass without carousel (Beaver Creek reaches tick 900). Binary `7413fc2b…59a36`;
evidence `build/native-host-default-map-fixed/result.json`. No Infection rerun or
broader multiplayer acceptance is attributed to this later binary. Original maps/
identities preserved and stopped-job runtime copies removed. Black Echo was
revisited immediately: Spark `17d6349` still retains the same `31505afb…3ebab`
cache. The existing contract now gives its exact blocked body/control/hull
coordinates, native landing thresholds and required clear spawn/pad/boarding
geometry, plus the crash's full offending datum and fixed runtime review boundary.
Full authored flight/objectives/audio/rewards/redeployment still awaits corrected
Spark content. No Spark edits, hosted work or push.

Two-squad combat/loot/aircraft slice (17 November):
`build/native-delta-squad-combat-salvage-roundtrip/result.json` PASSES two verified
Delta clients in separate one-member Covenant squads through physical Spirit
insertion/disembarkation, four actual Flood kills, distinct native dropped-weapon
pickups, private authored objectives, both first-attempt activation countdowns,
queued physical boarding/evacuation, independent banking, hub exits and
uninterrupted exact-ID re-equipping. Four serialized weapon IDs remain unique;
each durable record reserves its starter plus its recovered field weapon, matching
the native inventory. No forced boarding, objective setters, damage injection,
teleport, restart, settlement bypass or mandatory raid timer. Existing native
HSC/AI/transport/inventory systems run the operation; the finite director is OFF.
Native Invader compile/readback and the actual two-peer run use cache
`940a49c2…ca098`, binary `6744161c…1cc09`. Comparisons: 674 player seconds/1,348
observations, no persistent divergence under unchanged limits; transient aim,
position, riding and grenade differences remain recorded (position maximum 3.837).
All 678 item snapshots agree, all 12 new host items seen; 663 AI snapshots/2,652
unit comparisons agree, all four combat deaths common. Two final PNGs inspected.
Source modules are `tools/fixtures/extraction_transport_{objectives_native.hsc,
objectives_native.hmt,controls_native.txt}`; no commercial assets are tracked.
The earlier salvage observer failure mixed separate live reads across pickup;
retained complete snapshots prove distinct ownership. Using one immutable read
retains every acceptance assertion. Historical attempts stay rejected. Original
accounts/cache unchanged; stopped-job runtimes/maps and duplicate BMPs cleaned.
This accepts one bounded fixture operation and re-equipping, not another mission
playthrough, full Spark Black Echo, six independent clients or human/hardware/
platform acceptance. Spark's authored hub/pad clearance blocker remains open.

Campaign/profile crash repair (17 November): reproduced the owner's exact
`DeLa`/`schi` failure and all five stack offsets through Spark's native
`HALO_START_MAP=be_native@ce` path with a fresh private profile. Ordinary a10
startup alone did not reproduce it. Forerunner's close-all deletion opened the
profile-saving screen's successor difficulty menu; that widget retained old-map
tag indices after replacement. Close-all now suppresses successor openings while
retaining deletion cleanup and ordinary profile transitions. Windows64 incremental
build and 10 focused teardown/keyboard checks pass, including detection of the
old successor leak. The same unchanged Spark `31505afb…3ebab` cache now starts,
admits its native Extraction hub and renders without an exception in the bounded
repeat. Screenshot inspected. Binary `6744161c…1cc09`; exact logs/hashes and scope
are in `build/native-campaign-profile-crash/result.json`. This is an engine-side
startup repair, not full Black Echo acceptance or a Spark installation update.
Original accounts were not used in the crash reproduction. Owned runtime/map
copies cleaned; Spark remains read-only. The later authenticated operation above
also uses this repaired binary without a native exception.

Current native Spirit transport/gear acceptance (source `565401afb8`):
`build/native-delta-spirit-return-footpath/result.json` PASSES two authenticated
clients in separate one-member Covenant squads through two native insertions,
two queued physical evacuations, independent banks, normal hub exits and
uninterrupted re-equipping of each exact banked weapon ID. Both native weapons
and issued records agree; the two IDs remain distinct. No game/process restart,
forced boarding, teleport, flight/settlement change or relaxed acceptance rule.
The native cache is byte-identical to the previous `205f6a16…13dc8` candidate.
Normal walking follows the already observed outward footpath in reverse instead
of the earlier direct path into the parked hull/terrain. Player comparison:
698 seconds / 1,396 observations, zero mismatches; 702 item snapshots agree,
all five new host items seen. Binary SHA256
`b428588dd363b066fe276798bda74a65dae0fae9c44ee9414909d969d187e1a9`.
Two final native PNGs inspected. Original accounts and accepted source cache are
unchanged; owned runtimes/maps and 126,318,721B of duplicate debug/BMP/derived-cache
scratch removed. No engine build or unrelated regression run was repeated.

The earlier departure-readiness failure is explained by its retained host chat:
activation near tick 10,350, walking resumed at 10,470, then ACTIVATION CANCELLED
near 10,530, before the ten-second hold finished. Later boarding could not create
a receipt for that cancelled run. Current accepted calls/countdowns/receipts
require no gate bypass or speculative engine patch. This closes the aircraft/
same-session gear leg only: field combat/loot collection, a further playthrough,
full Spark Black Echo, six independent clients, human/headset/platform acceptance
and the whole engine goal remain OPEN. No hosted work, Spark writes or push.

Previous gameplay checkpoint (source base `8256a166c7`): Spark `8fdfe91` delivered
its retained combined Pelican/Black Echo review map, SHA256
`31505afb15da14ff32af694afecc28362ff911bd6df6b79778195493c6c3ebab`.
The unchanged Windows64 runtime loads it with two verified accounts and admits
the native hub. Full operation FAILS before deployment: the observed control
approach stops under the parked Pelican and offers its seat instead of the
deployment control. The grounded, stationary ship also has up 0.942 / landed 0,
below the existing 0.95 landing-attitude requirement. No gear deployment is
admitted. Spark owns hub/pad/control clearance corrections; exact coordinates,
cache identity and next revision requirements are in the existing contract.
`build/native-spark-black-echo-aircraft/result.json` retains the rejected attempt:
90 player seconds/180 observations and 94 item snapshots agree. Two inspected
PNGs show the obstructed approach. The optional overlength console query was
refused, so pilot readiness is unverified. No map/source/landing-gate bypass.

Independent Firefight defeat/loss/restart PASSES in
`build/native-firefight-defeat-restart/result.json`: two real Flood combatants
kill both idle players, use the one paid native respawn, then exhaust the team.
At tick 1350 both are dead; two durable records have lost outcome and zero stash,
reserved or escrow gear. No victory or recovery reward. Native lobby return
restarts the encounter with both living players, fresh issued runs and reset
lives. Comparisons: 45/8 player seconds, 56/16 living observations, zero mismatches;
49/12 item snapshots match, all 13/5 new host items seen. This reuses the accepted
CE609 and binary `f02fd1bf…145d9`; no rebuild for either native check.

The defeat PNGs exposed a separate spectator HUD defect: casualties with nobody
to watch were labeled JOINING THE GAME. `coop_spectate_draw` now uses native
death statistics to display WAITING FOR TEAM RECOVERY, retaining first-join
labels, own-body timing and faction/camera privacy. Windows64 incremental build
and all nine existing spectator checks PASS (`build/spectator-recovery-{build,
checks-configured}.log`). Initial check attempts lacked the existing MSVC header/
linker environment; configuring it resolved those setup failures, without new
tools or weakened checks. The changed label is source/compiled/automated verified;
the retained native screenshots show the prior defect, not a post-fix render.
Both native runs' original account/cache hashes remain unchanged; owned runtimes,
maps, 19,661,016B BMP scratch and 423,348B scenario-tag scratch are cleaned.
Full Black Echo, dropship redeployment, human/hardware/platform and the complete
engine roadmap remain open. No hosted jobs, spending changes, Spark writes or push.

Previous bounded native Firefight round trip (source base `3d432e2128`): two
authenticated clients clear two actual combat waves, return through the native
lobby and start the next encounter with reset lives, statistics and healthy
equipped bodies. The existing opt-in `network_test` rotation now remembers a
running co-op round, which has no multiplayer postgame scoreboard. No production
gameplay, packet, save or victory gate changes. Windows64 compiles; seven focused
existing log-parser checks pass. `build/native-firefight-roundtrip-fixed/result.json`
records the accepted bounded run: each controlled native death spends exactly one
life and produces a distinct healthy body with the native loss kit. Survivor
rifle reserves refill 240→638 and stowed pistol 72→132. First/second-round player
comparisons cover 59/10 seconds and 85/20 living observations, zero mismatches;
63/14 item snapshots match, including all 17/3 new host items. Two inspected PNGs
show native victory; disposable runtime/maps and 9,830,508B of BMP scratch removed.
Original account files and owned source cache unchanged. Binary SHA256:
`f02fd1bfabc9869e52d440eb734f28195d6124409ba1129d2fb62196718145d9`;
the CE609 hash remains the one below. The preceding round-trip probe is explicitly
rejected: its driver assumed `(players)` list order matched network player indexes
and watched the wrong victim. Native script lists are unchanged; the corrected
probe observes actual death/body/life changes. This closes bounded Firefight
victory/lobby/restart coverage, not human, full Black Echo, aircraft, late-join or
cross-platform acceptance. The complete roadmap remains active. Hosted execution
stays disabled automatically; no hosted run, spending change or push requested.

Previous built-in Firefight equipment/life repair (source base `3446ed13a6`):
`coop_enemies.c::firefight_resupply_players` reuses `game_modes_inventory_capture`
for its eligible slots instead of requiring a model parent. Both readied and
properly owned stowed weapons replenish; stale/transferred/dropped/objective gear
and pending deaths refuse admission. No second ownership implementation or grant.
`players.c::player_add_equipment` preserves native vitality for zero-health/zero-
shield equipment-only profiles while this director is active. Those kits previously
erased fresh respawn health. Explicit vitality profiles, ordinary campaign resets,
native weapon/grenade replacement and death flags keep their existing behavior.
The resupply tests now share the actual inventory world; the orphaned parent-only
fixture was removed after caller review. Windows64 compiles; the final related
inventory/faction/profile run passes 126 cases, including the resupply/respawn
checks (`build/firefight-life-regression-checks.log`).

`build/native-firefight-life-resupply/result.json` separately verifies two real
accounts' first-wave weapon combat and the surviving client's readied rifle
240→637 / stowed pistol 72→132 refill. A real host death spends one life; the fresh
body at tick 540 has health/shield 1/1, versus 0 health before this repair. Salted
body handles differ; this is a new native spawn, not a cleared dead flag. Player
comparison: 37 seconds / 67 living observations, zero mismatches; 41 item snapshots
match. Original identities/cache unchanged; owned runtime/map staging removed.
The overall probe remains UNACCEPTED: the optional sequential forced-respawn
command omitted the required unit cast and was correctly refused. Earlier probes
record native enemy-erasure abort, conflicting idle input, and loss of an original
secondary after actual death. They are not full two-loadout or full-match passes.
No more native reruns after these observations. Binary SHA256:
`fcf135cb8d972f66bbb1bbb66199aaf7b40f7bf2d1eb60e55879f1344aa5dd19`;
reused CE609 `bc2b80b52d519e166c318bd1d8c24a585c1b94a7965222a3fb10524ddc2b5f25`.
Full match/human/platform acceptance, aircraft redeployment and the expanded Spark
mission stay open. Native Linux execution is unavailable here (no installed WSL
distribution); no replacement environment or toolchain was installed. No hosted
jobs, pushes, Spark edits or new subsystem.

Previous unarmed aircraft-exit repair (source base `6c136ccbf5`): native
`units.c::unit_adjust_for_seat_change` now mirrors seat entry's existing
missing-unarmed animation fallback for living, detached, unarmed players.
Stock Grunts have passenger poses but no named unarmed ground weapon type;
after seated banking they retained the passenger class and could not walk.
The fix selects an existing native standing class, without granting gear,
moving bodies, editing animation assets or changing armed/NPC/death handling.
Windows64 incremental compilation and 15 focused existing-harness checks pass
(`build/unarmed-exit-{build,checks}.log`), including the removed-fix control.

Two real Delta accounts, separate Covenant squads and the same shared Spirit
complete two physical insertions, serial evacuations, independent durable banks
and normal hub exits in `build/native-delta-spirit-squad-roundtrip-fixed/result.json`.
Both now walk unarmed; the remote client redeploys its exact banked plasma-pistol
identity. The host's input route hits the parked hull, so complete same-session
redeployment is NOT accepted. Existing player comparison passes 701 seconds /
1,402 observations, zero divergence; 705 item snapshots, zero out of step.
Binary SHA256 `f0560bbd28197c2eb6a92da2181ebfbddf463422290649def0a2478e294b0384`;
owned CE609 SHA256 `205f6a16dbef7471a42e4829941b3aad561241dd005bc4d3385879f94c713dc8`.
The derived fixture reuses the existing native serial HSC controller and accepted
Spirit scenario, with two ordinary squad terminals; no second flight controller.

The earlier bounded `native-delta-spirit-squad-roundtrip-route` attempt corrects
the hub walking route, but stops earlier: both insert, the first player physically
boards at the LZ, and departure readiness never clears before the deadline.
No evacuation/banking/redeployment pass is claimed for that attempt. Later direct
readback of its retained host chat identifies the cancelled activation described
in the current checkpoint. Earlier pre-fix unarmed locomotion failure
and operator-stopped probe remain explicitly partial, not acceptance.

Follow-through on `978169e0c3`: `extraction_tell` now records actual host notices
only under the existing string-valued `debug.network_test` selector. No gameplay,
packet, authoring or settlement rule changed. The first diagnostic attempt used
the wrong boolean getter and was stopped before extraction; its result is rejected.
The corrected Windows64 build and 60 focused transport checks pass
(`build/transport-ack-final-{build,checks}.log`). The existing harness now rejects
reading this string selector as a boolean, preventing that diagnostic mistake.
`build/native-delta-spirit-control-ack-final/result.json` verifies both host-accepted
calls, completed activation countdowns, boarding receipts, serial physical flights,
separate banks and hub exits. No cancellation/retry occurred. The earlier cancelled
activation is now established from that older run's own retained host chat.
Both players walk unarmed, but neither reaches redeployment before the bounded
deadline: their walking routes encounter the aircraft/terrain. That run remains
partial; the later accepted reverse-footpath run above closes the gear leg.
Player comparison: 702 seconds / 1,404 observations, no sustained divergence;
one transient aim mismatch. All 706 item snapshots match. Binary SHA256:
`2f94e8631f7a6c5278a0f36eaa219cc9b7b89562c346827a231348e67a5682df`;
same CE609 hash above. Account originals unchanged; owned runtimes/staging and
the restored disposable cache are cleaned, compact logs/records retained.
Read-only Spark is now `40d115e`; its latest recovered Save/HUD cache was disposed
with QA26367/run-aVu7Oz. No retained expanded mission handoff is identified there.
Spark's retained combined mission handoff below remains required for actual
Black Echo aircraft acceptance. Private cinematics, six independent clients,
human/headset/platform acceptance and the full engine roadmap remain open.
All seven remote workflow blobs match the local zero-spend guards; no hosted
run, spending increase or push. Account balance/budgets are not readable here.

Previous native squad mission radio (source base `3ed2916704`): ordinary static
`sound` helpers `forerunner_squad_01_radio`, etc. now play native authored cues
only for eligible living/verified current-operation participants. Existing
Extraction queries choose the audience; machines with any other local player
are withheld. Existing co-op event resends carry host-only full-machine audience
metadata; other clients receive empty impulses with the same event number.
This preserves existing sequence/wrap and wire layout. Native impulse playback
and script timing share one implementation; radio has no microphone access or
second VOIP path. Discovery adds `extraction.squad_radio.v1`.

Windows64 compile/link PASS (`build/squad-radio-final-build.log`). Existing
harnesses pass 29 affected Extraction/query cases, 47 interpreter cases, 21 host
audio/event cases, 16 client audio cases and compiled discovery (114 total).
Coverage includes mixed local audiences, queue compaction/fullness, reconnect
handles, sequence wrap, no-op recipients, native timing and ordinary playback.
The new playback trace uses the existing debug.network_test setting only.
Initial trace compilation exposed game/platform header boundaries; the final
source uses the existing native logger declaration, without Linux host headers.

`build/native-delta-squad-mission-radio-final/result.json` accepts two verified
Delta accounts deploying through native controls and physically completing
their separate HSC objectives. Each squad's recovery/completion cue creates
exactly two native sound instances only on its own machine. The client never
plays Squad 01; the host explicitly withholds Squad 02 playback while preserving
host script timing. Player comparison: 119 seconds/238 observations, zero
divergence. This uses SDL's software dummy output, not physical listening or VR.
The first native attempt completed objectives, but guest console timer probes
were refused by host-authority policy; that attempt is explicitly audio-unverified.
No guard was weakened. Final native trace closes process-level playback routing.

Binary SHA-256: `96d92b7b7dcde476d4f4af950c06fad38d2e1053e8504496b660453fc7493091`.
Disposable CE609 cache SHA-256:
`432602b7e40f96891827c7e499156c1f5d3c3a0c012922a9bbd71816260e77e2`.
Original accounts/accepted cache unchanged. Owned processes/runtime/map copies,
derived cache and discovery scratch cleaned; exact previous/current four-file
native authoring snapshots and compact evidence retained.

Read-only Spark `54c1984` now records complete combined Pelican/Spirit source/
CE609 compilation, not merely the older radio recipe. Production revised hashes
`31505afb...c3ebab` (Pelican) and `2d6b31b5...0b176` (Spirit) belong to disposed
QA28356/run-tRm24j output. No retained final cache/saved build-input handoff is identified
in that checkpoint. The contract requests that exact reviewed handoff through existing
exports; no Spark rebuild or write. Full mission/aircraft human acceptance, six
authenticated clients, physical audio and other platforms remain open. No hosted
jobs, budget change or push; the complete engine goal remains active.

Previous squad-authored operation notices (source base `4727bd47ef`): standard
CE609 static `hud_message` helpers named `forerunner_squad_01_notice`, etc.
evaluate native HSC normally, then deliver that HMT message through the existing
authoritative squad query and personal notice path. Discovery adds
`extraction.squad_notices.v1`; no new opcode, packet, quest state or HUD renderer.
Only living verified current-operation participants receive text. Ordinary
scripts/global objectives remain unchanged; unsupported HMT messages refuse
delivery with one map diagnostic.

Windows64 incremental compile/link PASS (`build/squad-notice-final-build.log`);
20 affected Extraction/query checks, 46 interpreter/regression checks and one
compiled native squad-discovery check PASS. Existing Invader compiles native
HSC/HMT/cache. `build/native-delta-squad-notices/result.json` accepts two real
authenticated clients in independent squads, native control deployment and
physical objective traversal. All eight inspected PNGs show the correct recovery/
return cues only on intended host chat or remote hidden-chat HUD. The unchanged
player comparator passes 109 seconds/218 observations, zero divergence.
No console objective setters or notice injection; no banking rerun.

Binary SHA-256: `1ac46a5c8d20e739f08c8341acad35a3bd2fd198ef97670a059e5372ee356fdd`.
Disposable CE609 cache SHA-256:
`8b1b503cdb3039f09c2ba529e54a74ec2cca86476f4baec4af5fff106383bfa8`.
Original accepted cache/account files unchanged; Spark read-only. Owned runtime/
map copies, derived cache, BMP scratch and discovery runtime cleaned; authoring
inputs/compact evidence retained. Full Black Echo aircraft/cinematics, six
independent clients and other platforms stay open. The owner's billing annotation
is a provider refusal, not an engine failure: no hosted jobs, spending changes or
push. Full engine goal remains active.

Previous personal operation feedback repair (source base `0cec942dce`): existing
`P1 | ...` host notices now address their actual local view, including sparse
controllers and wrapped continuation lines. Missing/invalid recipients cannot
become broadcasts. Hidden-chat incoming personal notices use the native HUD;
Extraction and chat now share the existing wrapper in `hud_messaging.c` instead
of keeping it private to Extraction. Global notices and ordinary chat keep their
scope. No new packet, renderer, editor interface or inventory authority.

Windows64 compile/link PASS (`build/directed-notice-final-build.log`); 90 chat
checks PASS (`build/directed-notice-final-checks.log`) and 26 affected notice/
debrief checks PASS (`build/directed-notice-extraction-checks.log`). Native
`build/native-directed-operation-{target,hidden}/result.json` accepts actual
deployment-control feedback only in view 2, through chat and its HUD fallback.
The missing-profile guard still refuses deployment; no identity was invented.
`build/native-delta-directed-notice/result.json` accepts a real incoming personal
notice on the hidden-chat client with two verified Delta accounts. Three sampled
player seconds match; this is UI/receipt evidence, not a synchronization milestone.
Inspected PNGs confirm those scopes. Initial `sv_say` probes exercised global HSC
HUD text, and the first control probe mixed keyboard/gamepad view ownership;
these are rejected probes, retained with explicit reasons, not native acceptance.

Binary SHA-256: `3d0913afd9b40ccf8eebcb4c00d651784f3790e7b1ff708a195900cabc000bda`.
Original cache/accounts/Spark unchanged. Owned jobs/runtime/map copies and
39,322,032B BMP scratch cleaned; compact traces/PNGs retained. Private mission
cinematics, full Black Echo, six independent clients, VR and other platforms stay
open. Spark `4339020` advances recovered radio authoring but still records no
retained combined candidate. No hosted jobs or push. The full goal remains active.

Previous optional two-handed bracing (source base `45642bd8a2`):
`vr.two_handed` / `HALO_VR_TWO_HANDED`, off by default, adapts OpenH1
`baed7757`'s foregrip reach, grip hysteresis, smoothing, blend and one-handed
weapon exclusions through the existing OpenXR adapter. The support grip steers
both native eye-converged aim and native weapon posing. Input ticks alone advance
the filter; duplicate calls/eyes do not. Tracking, ownership, weapon/body/reference
changes and lost rendering release it; held input cannot reattach without release.
Support-grip WHITE action is reserved while enabled; assigned PTT retains priority.
No new renderer, hand mesh, recoil/damage change, inventory or network format.

Windows64 incremental compile/link PASS (`build/vr-two-handed-final-build.log`);
52 focused checks PASS (`build/vr-two-handed-final-checks.log`). Initial fixture
macro/positive-case/fault-control errors were corrected without weakening runtime
guards. Existing `build/native-two-handed-fallback/result.json` workflow PASSES
native menu/input replay with bracing enabled but XR disabled; inspected PNG
shows the menu. Binary SHA-256:
`bd03396236f1f249bed8fef4b5f93093a0506a03c03e37460a609dd045af45df`.
This does not prove physical bracing, stereo calibration, mixed-client VR or any
headset/platform acceptance. Reload/holsters and complete VR gunplay remain open.
Previous aircraft/banking/download evidence preserved, not rerun. Owned jobs and
disposable payloads stopped/removed; 9,830,508B BMP scratch cleaned, two PNGs kept.
Original assets, credentials and Spark unchanged; no hosted jobs or push.

Previous reviewed-cache distribution checkpoint (source base `0098e03c49`):
the existing `map_torrents.py` make/hash/share commands accept one explicit .map
file as well as folders. A creator can approve exactly a reviewed compiled cache
in a private native index without copying it into another project or approving
adjacent maps. Existing native trust, signed-index and payload checks remain;
no new registry, protocol, launcher or trust bypass. 14 tool checks PASS in
`build/map-explicit-approval-checks.log`, including real CLI index/torrent readback
and exclusion of an unreviewed neighbouring cache.

`build/native-delta-map-handoff-approved/result.json` PASSES: the client starts
without `fn_sqnav.map`, downloads all 110,032,772 bytes through native map sharing,
matches original SHA-256 `e71943d6...ae871c3` and spawns alongside the other verified
Delta account. Native join takes 21.485 seconds including startup; this is not a
throughput benchmark. 31 synchronized player seconds: zero position/aim error,
sampled mismatches or persistent divergences. Earlier `native-delta-map-handoff`
records the expected default refusal of a cache absent from the approved index;
its join failed, and the safeguard was not removed. Public seeding/index refresh
disabled only in the isolated private configuration; CE resources supplied locally.

Existing `88b70c81...d229f` Windows binary reused without rebuilding. Original
cache/account files unchanged; stopped runtime/map copies removed. Contract
records the exact existing settings Spark can adopt after reviewed cache approval.
This is private Windows co-op download/join acceptance, not the finished Black
Echo mission, resource redistribution, Internet performance or platform parity.
No hosted job, spending change, push or publication. Full goal remains active.

Previous authenticated aircraft continuation (unchanged engine `fe5fd79c86`):
`build/native-delta-squad-transport-redeployment/result.json` PASSES two verified
accounts' separate native insertions, queued Pelican evacuations, independent
durable banking, both crews' physical hub disembarkation and same-session native
redeployment. This closes the previously undriven second crew exit/redeployment
below. Each owner reserves exactly its two banked IDs, with the actual rifle and
pistol present on its native body. All four IDs remain distinct. First banking
does not settle the waiting squad. Both native squad completion callbacks fire.
No forced boarding, player teleport, synthetic settlement or engine patch.

Player comparison PASS: 439 seconds, zero sampled mismatches or persistent
divergences, position maximum 0.554902 and aim maximum seven degrees. Item
comparison PASS: 443 snapshots, zero out of step; all six sampled new host items
appear on the client. Inspected PNGs show both players back on foot at deployment
with recovered native weapons. Fixture device labels still contain authored
placeholder text; these captures do not establish finished mission UX.

Existing CE609 `23a8f945...cfa746` and binary `88b70c81...d229f` reused without
recompilation; original cache/account files unchanged. Owned processes stopped,
runtime/map copies and 9,830,508B BMP scratch removed; two PNGs and compact
records/traces retained. No hosted jobs, spending changes or push.
Scope is one Windows/OpenGL listen host plus one independent authenticated client
on the same machine. This is not six independent clients, Spirit serial dispatch,
private cinematics, a second flight after redeployment, VR, human acceptance or
the combined Spark Black Echo mission. Read-only Spark `aa7c7f3` has its next
native authoring check in progress, not a retained full-mission handoff.

Previous optional VR pickup checkpoint (source base `cfdad7dcfd`):
`vr.weapon_grab` / `HALO_VR_WEAPON_GRAB` is off by default. With controller aim,
the dominant grip can submit native pickup/swap for the currently offered world
weapon when the tracked hand is close enough. The native body range, exact object
ownership and host admission still decide pickup. No inventory grants, packet
changes, extra renderer or interaction manager. Adapts the credited OpenH1
`vr_grab.c` / `vr.c` policy at `baed77571d53abe32b27434b4ba869f11db00fe9`.

Release is required after focus/tracking/reference/body changes; held or firing
input cannot start pickup. Active grip push-to-talk retains priority. Enabling
pickup reserves the weapon-hand grip's native BLACK action during gameplay;
face-button pickup and the default disabled mapping remain available.

31 focused checks PASS in `build/vr-weapon-grab-final-checks.log`, including
reach, ownership, handedness, focus/reference loss, voice priority and negative
controls. Windows64 incremental compile/link PASS in
`build/vr-weapon-grab-final-build.log`. The initial missing-header build failure
was corrected before these final checks; no new toolchain was created.
`build/native-weapon-grab-fallback/result.json` PASSES the bounded offline native
Windows/OpenGL menu/input replay with XR explicitly disabled. Inspected PNGs
show the native menu; this is desktop fallback evidence, not VR interaction.
Binary SHA-256: `88b70c81fd210e5b271fb8c7c18e693c117df10b49aa05e6733604056b5d229f`.

Physical headset pickup/calibration, stereo, mixed-client VR and other-platform
acceptance remain UNVERIFIED. Physical reload/holsters and complete VR gunplay
remain unfinished. Prior banking/multiplayer evidence was preserved, not rerun.
Owned jobs stopped; disposable runtime/map copies and 9,830,508B BMP scratch
removed, two lossless PNGs and compact traces retained. No original game assets,
account material or Spark files edited; no hosted jobs, push or publication.

Previous authoritative rival-loot checkpoint (unchanged engine `55a4c63b22`):
two real verified accounts reverse host/client roles. The new Covenant/Grunt host
owns an actual previously banked pistol, reserves that exact ID and deploys it;
its incompatible rifle stays safely stored. The UNSC/Spartan rival in squad 2
kills it with actual projectiles, physically picks up that exact native weapon,
activates extraction, waits the native hold and walks back to bank it.

`build/native-delta-owned-gear-transfer/result.json` PASSES. The pistol's existing
ID appears once under the new owner and nowhere in the defeated owner's record.
Native object 21 moves from victim body 19 to the world and then survivor body 12.
The former owner's undeployed rifle remains byte-identical; all three owned IDs
are unique. No synthetic damage, forced pickup, teleport or settlement bypass.

`build/native-delta-owned-gear-redeploy/result.json` PASSES fresh processes:
the former owner opens its one-weapon/lost debrief, while the rival redeploys the
same two native weapons and transferred pistol ID. The old owner retains only
its protected rifle. Original accepted banks remain unchanged. Inspected PNGs
show the looted/redeployed pistol and the former owner's one-weapon lost report.

Player comparisons PASS 289 operation/63 restart seconds, no persistent
divergence or sampled state mismatches, position max 0.102903/0.067097 and aim zero.
Item comparisons PASS 293/67 snapshots, zero out of step; all ten/three sampled
new host items appear on the client. This is not latency/stress proof.
The earlier starter-kit baseline and observer failure remain in `9d84139f1a`
and unchanged `build/native-delta-pvp-loot{,-fixed,-redeploy}/` evidence.
No engine patch, rebuild or new harness was needed for this ownership case.

Read-only Spark `2a5839c` advances first-insertion NPC/escort authoring and
CE609 component readback. Its checkpoint explicitly disposes the test candidates;
a retained/reproducible full Black Echo aircraft/crew/mission handoff is still
needed for combined runtime acceptance. This is a content handoff requirement,
not an invented new engine interface or a claim its game sequence was tested.

Same binary `77781f69...d5e4a65d` and owned CE609 `e71943d6...ae871c3`; original
identities/map unchanged. Owned jobs/runtime/map copies and 29,491,524B of BMP
scratch cleaned; six lossless PNGs and compact records/traces retained.
This proves bounded Windows/OpenGL two-account serialized rival transfer and
redeployment, not full Spark Black Echo, simultaneous AI PvPvE, aircraft, six
independent clients, human, VR or other-platform acceptance. No new inventory/
network system, hosted job, push or publication. The complete goal remains active.

Previous campaign/co-op checkpoint repair (source base `389a685037`): successful
native saves now snapshot the existing guidance replay cache alongside the live
checkpoint metadata. A normal host revert restores that snapshot, resets replay
cursors and drops bindings to viewers/objects removed by current-roster repair.
Failed saves cannot replace it; a new map clears it. Both inherited checkpoint
layouts reuse one local marker-clear routine before restored host replay. No new
save layout, packet, protocol, quest system or inventory exception was added.

Both original defects reproduce in the existing guidance fixture. The affected
guidance/save/wipe checks PASS 53 cases; the existing queued-recovery check also
PASSES with its explicit client-clear observation. Its first compile attempts
lacked the installed MSVC include/link paths; the existing installation was
used, with no new toolchain. Windows64 incremental compile/link PASS:
`build/nav-checkpoint-paths-build.log`; focused logs share the checkpoint prefix.

`build/native-delta-nav-checkpoint-fixed/result.json` PASSES two authenticated
Windows/OpenGL clients: normal `game_save` writes a 20,971,520B native checkpoint,
the client changes from the saved flag to an object marker, and normal
`game_revert` restores only the saved flag. Both final PNGs are inspected.
Native revert is 175 ticks with clock held at 418; 15 player seconds and 19 item
snapshots PASS, zero position/aim difference or out-of-step items. Exact binary
`77781f69...d5e4a65d`, unchanged owned CE609 `e71943d6...ae871c3`.
The first native attempt (`native-delta-nav-checkpoint`) FAILS visual acceptance:
the flag returned but the later object marker remained. It exposed the second
inherited recovery entry point; its trace/PNGs and original driver claim remain.
The corrected check uses their shared clearing routine, not a packet workaround.

Original identities/maps unchanged; owned runtimes/maps/checkpoints and BMP
scratch cleaned, six lossless PNGs retained per attempt. No gear transaction;
saved-world banking stays suspended under the existing anti-duplication policy.
This is in-process native checkpoint guidance, not disk-resume, complete campaign,
full Spark Black Echo, human or other-platform acceptance. Read-only Spark
`01d108b` still lists unfinished NPC crew/seat/native compiled-mission handoff;
no new complete Black Echo aircraft candidate was accepted. No hosted jobs/push.

Previous co-op guidance checkpoint (source base `cdd004abbf`): native object
deletion now retires retained nav bindings for the exact viewer/target handle.
Flags and a reused slot's new salted handle survive. Cache compaction preserves
the existing eight-events-per-tick fair replay, so a moved tail is not skipped.
Client activation waits for a real target; deactivation still works after target
loss. Native callbacks, HUD capacity and ordinary HaloScript remain intact.

A native follow-up exposed a second concrete defect: named controls/scenery use
independent client-local handles. Nav events now reuse `object_find` and existing
optional name/definition fields, as scenery/device synchronization already does.
Legacy events retain exact-handle behavior; no packet layout/protocol change.
The old receiver cannot gain named-target resolution merely from compatibility.
Before-fix harness reproductions fail stale-body retention, pending-target
activation and named-handle resolution; all 40 focused checks PASS afterward.
Windows64 incremental compile/link PASS (`build/nav-named-{checks,build}.log`).

`build/native-delta-nav-lifetime/result.json` records two authenticated native
clients, three object replacements, separate flag/object views and real second-
client reconnect/requery. Initial/rejoined player comparison PASS 12/11 seconds;
17 initial item snapshots PASS. The unfiltered reconnect item comparison FAILS
seven differences before its authoritative clock/replica is ready; raw failure
is retained. From actual ready tick 930, 11 item snapshots PASS, zero out of step.
The rejoined body occupies slot 2 rather than old slot 1; this is not reconnect-
persistent squad ownership. Body-specific script calls must requery membership.

`build/native-delta-nav-named/result.json` PASSES final-source native named-target
guidance over three replacements: inspected PNGs show the remote object arrow
and a different host location flag. Player comparison PASS 13 seconds, zero
position/aim difference; 17 item snapshots PASS, zero out of step. Binary
`2c2a6e7d...4037fb61`, unchanged owned CE609 `e71943d6...ae871c3`.
The earlier combined-command attempt exceeded native telnet's line limit;
`native-delta-nav-ordering` retains that invalid result. The shorter-command
`native-delta-nav-supported` retains the genuine missing-remote-marker failure
that led to the named-object repair. Later print acknowledgments alone did not
prove either result. Forced event-before-replica ordering and capacity turnover
are harness evidence; native replacements are bounded, not same-tick proof.
No mission/banking repeat or persistence transaction. Original identities/maps
unchanged; owned runtimes/maps and BMP scratch cleaned, lossless PNGs retained.
Full Spark composition, six independent clients, human and other-platform
acceptance remain open. No hosted jobs, billing change, push or new framework.

Previous native carrier-reuse checkpoint (source `62bf118b47`, no engine rebuild):
two authenticated Windows/OpenGL clients ran the intact owned Library cache with
the existing authored `enc0` spawn selection. Seven carriers produced 26 native
infection forms in one wave; all 33 distinct tracked bodies reached final death,
score 3,300, and both survivors banked two weapons/four unique IDs. Thirty-three
cumulative births in the unchanged 32-entry table exercise actual entry reuse;
this is not 33 simultaneous tracked bodies or large-horde performance proof.
`build/native-delta-carrier-reuse/result.json` retains scoped native results.
Its overall driver FAILS on an observer regex that captured `c` from `child` as
another parent. Correct eight-digit handle readback and validated native records
prove those results separately; the mission was not rerun to hide that failure.

The preceding two-carrier check (`build/native-delta-native-carriers/result.json`)
records eight native children/all ten final deaths, score 1,000, one survivor's
two banked weapons and one casualty's native loss/zero reward. Its driver FAILS
because it demanded positive rewards for both players. No failed-run reward or
death override was added. Player comparisons PASS 77/54 seconds, no persistent
divergence; pressure has two transient aim differences (max 61 degrees). Item
comparisons PASS 81/58 snapshots, zero out of step; AI movement/damage/death PASS
47/24 snapshots and 767/535 unit comparisons. These sampled comparisons do not
claim every short-lived child's death was captured on both peers. No PNGs were
captured after the failed observers; human/visual/hardware acceptance stays open.

Owned native cache SHA256 `6560cd88...943d6b8`, unchanged binary `ae10c265...42c24f`.
Original identities/cache unchanged, voice/VR off, owned processes/runtime/maps
and 81,384,851B of cache/tag staging cleaned. Installed Invader failed to reconstruct
one native carrier particle-system tag during a minimal extraction attempt; the
intact cache was used instead, with no invalid-tag bypass or VFX substitution.
CE609 carrier conversion/Spark composition, full Zombies, human/other-platform
acceptance and large populations remain open. This cycle verifies the existing
source's native spawn/combat/recovery path; no new infrastructure/hosted jobs/push.
Read-only Spark `70e8366` advances native objective flags/personal squad guidance;
its moving-target, Windows and complete mission/game handoff remain open.

Previous Flood tracker checkpoint (source base `de7db7ac37`): native carrier
offspring can reuse completed dead entries in the existing 32-entry tracker.
Living enemies, unfinished native bursts and the current spawning parent keep
their slots. Each reused entry resets death/damager metadata; final-death score
and player kill credit remain once-only. Corpses stay with native garbage
collection. Invalid/partial/full-capacity spawns still abort. No larger arrays,
new spawner, packet or director; authored HaloScript ownership stays unchanged.

An existing-harness reproduction refused a valid child with an available dead
entry before the change. Nineteen focused pursuit/child checks then PASS; four
added partial-spawn/stale-handle/negative-control cases PASS. Repeated offspring
retain 32 entries and correct native kill totals; negative controls prove the
parent/pending-burst exclusions. Windows64 incremental compile/link PASS:
`build/firefight-children-build.log`; focused logs share that prefix.

`build/native-delta-flood-combat/result.json` PASSES two authenticated clients
against two loaded human Flood combat forms: native held-trigger projectile
combat using the existing aim diagnostic, two final deaths, one finite wave/
200 points and two independent escaped records with four unique banked weapons.
Player/item comparison PASS 18 seconds/22 snapshots, zero divergence/out-of-step;
AI movement/damage/death PASS 23 snapshots/48 unit comparisons, both deaths seen
by both peers. Two native PNGs inspected. Binary `ae10c265...42c24f`, unchanged
owned CE609 cache `053f24c1...328329c`; original identities unchanged, voice/VR off.
Owned runtime/maps and 9,830,508B of BMP scratch cleaned. The retained fixture
has combat forms but lacks carrier/infection dependencies: native reuse under
burst pressure, large hordes, full Zombies, human/other-platform acceptance remain
UNVERIFIED. This cycle delivers bounded offspring tracking plus a native Flood
combat/reward regression, without expanding infrastructure. No hosted jobs/push.

Previous endless-Survival acceptance checkpoint (source `46b23f5766`): the existing
director/Extraction path already implements optional evacuation; no replacement
or speculative engine patch was needed. `build/native-delta-endless-evacuation-ready/result.json`
now records two verified Delta clients, native endless wave start, separate real
control activations/ten-second holds, physical on-foot hub returns, two durable
escaped outcomes and four unique banked weapon IDs. The first escape leaves the
other living player's run active; only both completed banks end the director.
`build/native-delta-endless-redeploy-ready/result.json` PASSES fresh-process native
redeployment of those same four IDs and actual owned AR/pistol pairs, with stored
grenade ownership conserved and original bank seeds unchanged.

The operation driver overall FAILS after both native banks: it then requires the
second player to reach a point already occupied by the first. That failure stays
recorded; stopped logs/validated records separately prove the scoped native results.
Initial preparation and first restart drivers also fail premature wave readiness
and an incorrect zero-grenade assumption. Only unaccepted stages were retried,
without rebuilding/repeating the completed evacuation. Player comparisons PASS
62 operation/8 restart seconds (position max 0.075060, aim zero); item comparisons
PASS 66/12 snapshots, zero out of step. Native AI movement/state PASS 67 snapshots,
123 unit comparisons; no AI damage/death acceptance in this check. Binary
`6b1148df...e49ba6`; unchanged owned CE609 cache `053f24c1...328329c`.
Three retained PNGs inspected. Voice/VR off; originals/identities unchanged.
Owned runtime/map copies and 24,576,270B of BMP scratch cleaned. No hosted jobs/push.
This proves the existing on-foot endless evacuation/banking/redeployment slice,
not full Zombies, aircraft, Spark Black Echo, human or other-platform acceptance.
Read-only Spark `50837f2` advances native squad authoring; its combined mission
delivery/gameplay acceptance remains open. Full engine roadmap remains active.

Previous failed-BSP recovery checkpoint (source base `f729a63494`): the native
loader already restores the old BSP when its destination cannot load or fails
capacity validation. Its reconnect callbacks nevertheless consumed the pending
destination gather, ejecting/moving a remote passenger even though the source
area was restored. `players.c::players_reconnect_to_structure_bsp` now consumes
invalid or mismatched pending requests without relocating anyone, while still
invalidating player clusters. Matching successful classic co-op/split-screen
transitions retain their gather behavior. No new loader, world state or protocol.

The existing asset-free C harness compiles the real loader, reconnect and BSP
teleport functions together. Before the fix: refused-load passenger preservation
FAILS, forward co-op/split-screen PASS. Afterward: thirteen focused cases PASS,
including a negative control that removes the destination comparison; one added
failed-load-then-successful-retry case also PASSES. Logs:
`build/coop-bsp-baseline.log`, `build/coop-bsp-checks.log` and
`build/coop-bsp-retry-check.log`. Windows64 incremental compile/link PASSES
(`build/coop-bsp-build.log`, existing Expat import warnings). IO, collision and
movement are fixture boundaries; this is not native failed-load gameplay or
proof that all AI/world state rolls back. No unrelated native runs were repeated.
Spark's connected two-region delivery and bounded simultaneous residency remain
open under the existing contract. Spark/originals untouched, no hosted jobs/push.
This cycle fixes a campaign/connected-world foundation bug; no new infrastructure.

Previous controller-loadout checkpoint (source base `e0ed8fe766`): native controllers
share Reload/Use on X. The existing alternate hub actions now also accept held
Flashlight + Use: change the secondary at Loadout, or leave the stash safe at
Deploy. `player_queues_new.c::update_client_loadout_modifier_held` reads the same
existing queue's held Reload/Flashlight bits. Native flashlight toggling, ordinary
Use after release, crouch discard, host ownership and automatic Survival remain.
No packet, record, configuration, input state or XR binding layer added. Existing
Controls bindings apply; the existing OpenXR gamepad map is reused, hardware open.

Windows64 compiles (`build/controller-loadout-build.log`). Twenty-three existing
focused cases pass; twelve Flashlight cases/negative control pass after correcting
the test call to the existing single-case harness API. These reuse the same native
selection, retry, release, full-handle, death/disconnect, recovery and faction-kit
workflows. Existing SDL virtual-pad test input now accepts button chords such as
`pad1:lb+x`, refusing malformed chords, without changing normal inputs or sticks.

`build/native-delta-controller-loadout/result.json` records both verified accounts'
native SDL LB+X secondary choices, ordinary X after release, malformed-chord
refusal, exact private ownership and protected deployment. Its operation completes;
the overall driver FAILS later when its screenshot observer ignores a restarted
process overwriting a prior screenshot path. The actual frame is retained; no
capture/runtime repair was needed. `build/native-delta-controller-reopen/result.json`
then PASSES only fresh controller-X readback of those exact three/two-item records,
without repeating selection/deployment or missions. Native PNGs inspected.

Player/item comparisons pass 175/179 and 26/30 seconds/snapshots, zero divergences/
out-of-step items; position max 0.074471, aim zero. The interrupted first reopen
also has 11 seconds of agreement. Binary `bc42de27...ba141a`, unchanged objective
cache `0ee91a31...70616`; exact hashes remain in results. Original identities and
seed banks unchanged; voice off. Owned processes/copies/BMP scratch cleaned.
These are virtual controller actions with existing keyboard/stick approach, not
physical gamepad/OpenXR/Quest/Vive, other-platform or full mission acceptance.
Spark remains untouched. No hosted jobs or push. Complete roadmap remains active.

Previous stash-safe deployment checkpoint (source base `024e95820d`): Reload + Use
on the existing Deploy control leaves all stored weapons/grenades banked, using
the map's ordinary faction starter kit or currently carried native equipment.
Ordinary Use and automatic Survival still bring the saved loadout. The same
`gear_loadout` / `gear_reserve` now take an explicit choice; prepared reservations
remain authoritative across changed retry gestures. No record/schema/version,
packet, input state, inventory manager or Spark marker added. Existing loss,
refund, escrow and identity gates remain. Compact native help keeps both choices
visible. This lets successful runs accumulate loot beside safely stored gear.

Windows64 compiles (`build/starter-loadout-help-build.log`). Focused gear/terminal/
inventory regressions pass across the broad run and targeted follow-ups; the
initial two remaining fixture failures needed released Reload input before an
ordinary deployment and the updated deployment-signal negative-control call.
Both native faction kits and failed-kit rollback pass automated checks. Four
actual OS-lock/process-kill cases pass, including prepared/issued starter-only
records preserving banked items. Version-1 recovery is unchanged. Initial new
fixtures incorrectly assumed recovered items append and bypassed the existing
one-second refund retry; corrected the expectations, not runtime settlement.

Native `build/native-delta-starter-continuation/result.json` PASSES two verified
accounts' stash protection/recovery, host three-item secondary selection, exact
owned reservation and actual AR/pistol inventory, client native HaloScript
objective/activation/physical hub return/banking while the host stays separately
deployed, and fresh-process reopening of the client's three saved IDs. Player/
item comparisons pass 204/208 and 6/10 seconds/snapshots, zero divergences or
out-of-step items; position max 0.076085 and aim zero. Native PNGs inspected.
Binary `88320f7b...60405`, unchanged owned objective cache `0ee91a31...70616`;
exact hashes remain in the result. Original accounts and seed banks are unchanged.

Two earlier native drivers retain overall FAIL. `native-delta-starter-operations`
selected the older non-objective cache; both protected deployments passed before
it was stopped at the missing authored objective. `native-delta-starter-objectives`
then completed the first squad's native objective/escape/bank, but its driver
expected two new weapons instead of the actual one-weapon authored kit. The
first stash correctly grows two to three with both old items byte-identical.
Its real banks/unfinished second-account record seed the accepted continuation;
the first completed bank was not repeated. Their stopped comparisons also pass.
This is bounded acceptance across sessions, not one uninterrupted full mission.

Owned jobs stopped; runtime/map copies and preserved BMP/duplicate-log scratch
removed. No hosted job, push, original/Spark write or recreated toolchain. Full
Black Echo handoff, aircraft integration for this change, controller/VR and other
platform acceptance remain open. The complete engine roadmap stays active.

Previous loadout checkpoint (source base `20540dc0d9`): the existing hub terminal
now supports choosing the secondary independently with Reload + Use while keeping
the primary. Ordinary Use keeps cycling the lead pair; first use previews and
crouch-use keeps confirmed primary discard. The same compatible stash subsequence,
version-1 record/lock/atomic save, reservation, refund and recovery remain owners.
No new format, packet, menu manager or Spark marker. Two compatible items have
only one secondary for their primary, so that gesture then writes nothing.

The native input failure was real: device use precedes the body's current control
publication, and Reload is a one-shot latched action. Reading old body flags or
a temporary current-action snapshot both failed native held Reload + Use. Those
failures remain in `build/native-delta-secondary-loadout/result.json` and
`build/native-delta-secondary-current-input/result.json`. After two failures the
queue was traced; final `player_queues_new.c::update_client_reload_held` reads its
existing full-player held flags. The temporary snapshot/hook changes are removed;
ordinary native action latching and medical control are unchanged.

Windows64 compiles (`build/secondary-held-input-build.log`). Initial 120 focused
gear/terminal/inventory checks pass; seven final held-input/selection cases pass,
including a broken-body-input negative control. A further tightened queue absence/
full-handle case passes. Earlier 36 input/medical checks passed during diagnosis;
they are not additional distinct final tests. Larger-stash coverage proves all
12 ordered pairs of four owned items, exact identities/quantities, compatibility,
uncertain-save retry, reopen, reservation/refund, banking and redeployment.
The first four-item test setup omitted its fourth object handle; corrected that
fixture input without weakening runtime inventory admission.

`build/native-delta-secondary-held-input/result.json` PASSES two authenticated
accounts' actual keyboard Reload + Use, independent ordinary-Use choices and
fresh-process reopening of the exact selected records. Both native stashes have
two items: larger-stash selection/grant/recovery remains automated-only this cycle.
Player/item agreement passes 44/48 and 23/27 seconds/snapshots, zero divergences/
out-of-step items; position max 0.066873, aim zero. Native PNGs inspected. Binary
`e57f6edd...36b6b`, unchanged aircraft cache `23a8f945...fa746`; exact hashes in result.
Original accounts/seed banks are unchanged; voice off. Owned jobs stopped, runtime/
map copies and losslessly preserved BMP/duplicate-log scratch removed. No hosted
run or push. Controller/VR selection and full Black Echo remain open.

Read-only Spark `0f61442` now composes native faction kits, aircraft-relative
cameras, recovered mission radio and protected HUD with both mission transports.
Its checkpoint records compiled caches as disposed test output, not a retained
combined Black Echo handoff. Contract updated with that exact remaining delivery/
native-acceptance gap, without rebuilding or modifying Spark. Full goal active.

Previous operation-radio checkpoint (source base `a6f7b06bde`): native Delta TEAM
radio now uses existing authoritative squad membership, or native faction when
unsquadded, instead of broadcasting across all co-op operations. Unknown/pending
or restored membership and mixed-group split-screen machines are isolated.
Ordinary co-op, party/proximity, capture controls and native teams remain intact.
No new transport, packet, membership manager or Spark marker. Existing squad
notices/settings help explain the channel. Windows64 compiles; 35 focused checks
pass, including native membership/restore and actual inherited routing tests.

Two verified Delta accounts' synthetic-tone sessions prove faction/same-squad
TEAM reception, separate-squad isolation, live regrouping and TEAM beyond the
15-unit proximity range (minimum sampled separation 24.15). Existing evidence:
`build/native-delta-squad-radio/result.json` and `build/native-delta-squad-radio-regroup/result.json`.
Both drivers retain overall FAIL after those completed phases: respectively an
unlogged leave-message expectation and an unreached exact movement target.
No engine workaround was added for either driver. The final range-only run,
`build/native-delta-squad-radio-range/result.json`, PASSES nearby proximity and
refusal at 20.866 units with normal continued encoding. It uses the final restore
privacy fix, binary `30a989a0...b7169`; exact hashes remain in each result.
Player/item agreement passes 112/116, 124/128 and 28/32 seconds/snapshots,
zero divergences/out-of-step items; largest position difference is 0.067268.
The unchanged native cache is `23a8f945...fa746`. No duplicate transport/bank run.

Tone replaces capture: neither physical microphone opened; original identities
are unchanged. Owned jobs are stopped, disposable runtimes/maps/tone files and
2,386,971B of duplicate debug logs removed. Human sound, headset/microphone/PTT,
six independent clients, restored/mixed-local native behavior, browser, other
platforms and cross-region acoustics remain unverified. Spark authoring is
unchanged. Live GitHub workflows retain manual-only builds and guarded auxiliary
jobs; the owner's billing-block annotation does not authorize raising budgets or
starting hosted work. No hosted run, spending change or push. Full goal active.

Previous serial-transport checkpoint (source base `092ea348b3`): the unchanged
native aircraft cache now passes both authenticated one-player squads' separate
insertion, physical evacuation boarding, pilot return/landing and independent
durable banking in one session. Squad 1 banks while squad 2 stays ready in the
field; the shared pilot then accepts the waiting pickup without another insertion.
Both owner records are idle with two items each, four distinct item IDs, and both
native squad completion callbacks fire. This closes the earlier second-boarding
input blocker, not six-client, full Black Echo or private-cinematic acceptance.

The input correction preserves the previously working initial approach and stops
restarting its circle when the nearest seat changes. Final evacuation approaches
the native seat point rather than only its entry-animation endpoint, which was
below the local floor in the failed run. `units.c::unit_find_nearby_seat` already
accepts either point within the unchanged <1.0 distance. No engine range, collision,
seat admission, pilot, cache, transport/settlement gate or player placement changed.
`build/native-delta-squad-transport-seat/result.json` passes 398 player seconds/
402 item snapshots, no divergences/out-of-step items, position max 0.551157 and
aim max two degrees. Native final PNGs are inspected. Actual binary is the prior
`704e303e...c2ff`; exact binary/cache hashes remain in that result. The second crew
is still aboard after banking; its final hub disembark/redeployment was not driven.

The reproduced screenshot replay failure is repaired in the existing optional
test-input path: one-shot screenshot/capture/pointer actions reset when a new
script starts instead of inheriting the preceding script's last step number.
Three real-source baseline cases fail before the fix; all 17 existing/extended
input checks pass, including a duplicate-action negative control. Windows64
compiles (`build/input-replay-build.log`). `build/native-input-replay/result.json`
passes two identical step-zero screenshot commands in an isolated native main menu
on the new `65aeffbc...b27c8` binary. This is a test-control repair, not another
capture backend or rendering/gameplay feature; no gameplay simulation changed.

Original identity hashes are unchanged. Owned processes/copies are stopped and
removed; four losslessly preserved BMPs (19,661,016B) and duplicate debug files
removed. Earlier input failures and accepted banks remain preserved. Full Spark
mission, six independent clients, Spirit serial dispatch, private presentation,
other platforms/hardware and the complete engine roadmap remain open. No hosted
jobs or push; the persistent goal remains active.

Previous stash checkpoint (source base `5ffba191bf`): the existing native debrief
now pages through every owned stored weapon after the run report, including gear
unavailable to the current map/body. It shares `gear_item_available` with loadout
selection. Crouch-use reviews that exact item; a separate delayed crouch-use
confirms discard, while ordinary Use cancels/pages. Owner, full body, generation,
idle phase and native hub/life gates remain. Loadout filtering, at-risk gear,
grenades, record format/locking and network packets are unchanged. Published
deletions retry without selecting another item, including after preview expiry.
No new inventory/UI manager, editor operation or authored marker is required.

Windows64 compiles (`build/stash-browser-build.log`). Forty-five focused existing/
extended stash, debrief, notice and discard checks pass, plus one stale-preview
negative control. The initial six browser cases fail against the old source;
existing report-wrap expectations now include the appended stash pages. An older
test's blanket wrong-cache discard refusal is replaced with explicit owned-ID
deletion and unchanged incompatible-loadout refusal. Runtime admission is not
relaxed to make stored content equipable.

Native `build/native-delta-stash-browser-controls/result.json` verifies two bound
Delta accounts' separate stash views and normal crouch/Action confirmation removing
only the displayed unavailable weapon. The other owner's complete record stays
byte-identical. That driver's overall result FAILS at a later screenshot, whose
step index matched the preceding capture; completed deletion remains separate
evidence. `build/native-delta-stash-browser-reopen/result.json` then PASSES fresh
processes opening those exact saved records: zero/two stored weapons, unchanged
remaining item identities and grenades. No repeated deletion or reward grant.
Player comparison passes 68/35 seconds, zero divergences (position max 0.066250/
0.067083); item comparison passes 72/39 snapshots, zero out-of-step items.
Inspected PNGs show both stashes, explicit confirmation/completion and restart
state. Binary `704e303e6872af2e1880a45d9dcb48d8e8586a6d6fe36f592525122dd396c2ff`;
owned CE609 `063f0f5091b3f357bebd1711051c18afa7dc9b23ede430e38ca7d1d8d3ff548d`.

Seeds were isolated copies of prior accepted native Survival banks, not fabricated
identities/rewards. The first setup rejected still-deployed transport records
before launch; the first UI attempt separated crouch/Use tokens, paged instead,
and left records unchanged. Both failures remain recorded. Original identities,
prior banks and reusable source are unchanged; stopped runtime/map copies,
78,644,064B of losslessly preserved BMP scratch and duplicate debug files removed.
Deletion remains idle-only. Full Black Echo, rarity/upgrades, hardware and other
platform acceptance remain open. Read-only Spark `b24bac4` reports composed faction
kit compilation but explicitly not a complete delivered/gameplay-accepted mission;
relative cameras/radio and faction alignment remain its next authoring work.
No hosted jobs or push. The complete engine goal remains active.

Previous reinforcement checkpoint (source base `8efc4a537d`): the inherited co-op
dropship release loop removed queued extra AI before checking native actor
allocation. It now consumes a queued rider/placement offset only after success,
retrying at the existing four-tick interval. Craft destruction/stale handles still
discard that craft's queue; host authority and native encounter/squad/defaults
remain. Two real-source before-fix failures reproduce lost reinforcements; all
26 existing/extended vehicle checks now pass, including five release cases and
two negative controls (`build/rider-release-regression-checks.log`). Windows64
compiles (`build/rider-release-build.log`). The first test setup duplicated an
existing harness macro; that was removed before recording the gameplay failures.
Recovery under actual native pool pressure remains UNVERIFIED; this is not
large-horde or finished mission acceptance. No new queue/AI system or asset edits.

The bounded `build/native-delta-squad-transport-replan/result.json` attempt was
stopped early: one actual insertion/landing/disembarkation passed, but replanning
on nearest-seat changes repeatedly reset the second initial boarding approach.
Both serial recoveries remain unverified. This input adapter correction is not
accepted; no native boarding/collision regression was reproduced or patched.
The unchanged aircraft cache and prior accepted results remain. Original identity
hashes are unchanged; owned jobs/copies and redundant debug copies are removed.
Do not repeat the route unchanged. Read-only Spark `defc08c` now reports composed
seven-objective Pelican/Spirit CE609 variants and checked pilot graph bindings;
the contract replaces the obsolete technology/pilot-composition requirements.
Actual combined-map flight/gameplay delivery/acceptance remains separate. No
hosted jobs, push or publication. The complete engine goal stays active.

Previous Survival checkpoint (source base `5240fdad3b`): the existing director now
holds wave progression while everybody is incapacitated, pending native death or
awaiting a paid respawn. It verifies full player/body ownership. Only exhausted
actual deaths start the full three-second defeat grace; a standing survivor
resumes progress. Existing lives, spawn grants, outcome and gear owners remain.
Firefight global/equipment notices now reuse Extraction's chat/native-HUD fallback
and controller scope instead of duplicate overlapping HUD formatting. No new
director, UI system, packet, persistence format or Spark authoring interface.

Thirty-six focused Survival/evacuation/notice checks pass in the existing harness
(`build/survival-notice-checks-checks.log`), including real-source paid-spawn,
salted-owner, downed/death-grace cases and three negative controls. Five of seven
new wipe cases fail before the fix. Windows64 compiles; the final incremental
build is `build/survival-notice-build.log`. Existing Expat import warnings remain.

`build/native-delta-survival-wipe-fire/result.json` passes a finite native two-wave
Survival loop with two authenticated Delta clients: actual AI/projectile combat,
two enemy deaths, six seconds all-downed without victory or bank, controlled
authored aid, ordinary native biofoam revival, victory and both owners' durable
unique recovered gear. The host then returns to pregame (native debug line 1366).
Controlled authored damage/aid and diagnostic aim are fixture inputs, not human
play or the finished Zombie map. Player comparison passes 27 seconds with zero
divergences; 31 item snapshots have zero out-of-step items and all seven new host
items appear. Native actor comparison passes 32 snapshots/65 unit comparisons,
maximum position difference 0.242188, both peers seeing two damaged/killed enemies.
Core binary `a9ae29054fc64bac1343ed3c981d625a75c2386dd3a085599d69b1ee9b5906ad`;
owned CE609 cache `053f24c19f53e560c2ce4adc836d59077246942154f67ddbc0c724024328329c`.

The first native setup (`build/native-delta-survival-wipe/result.json`) did not
fire: its menu-wait input overrides the held-trigger diagnostic. It timed out
without clearing waves; correcting ordinary trigger input, not engine behavior,
produced the accepted run above. Its captures exposed overlapping native status
text. After sharing the existing notice path, the short display-only followup
`build/native-delta-survival-notices/result.json` passes: both inspected PNGs show
readable wave/equipment chat, nine player seconds have no divergences, and no
banking/restart acceptance was repeated. Final binary
`694420187afef45ee28e7ca7d5754b3709c7565ee651f9685b596faa681b81d1`.
The native device prompt's placeholder string belongs to the owned fixture.

Original identities and reusable source inputs are unchanged. Owned processes
are stopped; disposable runtime/map payloads and losslessly converted BMP scratch
are removed, compact logs/PNGs retained. Native paid-respawn acceptance, full
Zombie Apocalypse, late joins, human/hardware and other-platform acceptance remain
open. Existing Spark aircraft-composition and second-squad steering limits below
are unchanged; this cycle advances independent gameplay rather than rerunning them.
No hosted runner, paid service, push or publication was requested.

Previous shared-transport checkpoint (source base `0f73f18e05`): existing boarded
outputs now derive participating squads from physical passengers/current operation
epochs. Another unboarded squad of the same faction cannot veto departure at the
hub or LZ. Requesting members of an aboard squad still wait for activation/seats;
mixed actual crews participate, stale operation passengers refuse departure and
unsquadded crews retain the previous whole-faction behavior. No flight registry,
packet, gear authority or native boarding/collision limit was added or weakened.

The existing native HSC fixture reuses its insertion/extraction legs as static
helpers. Its existing loop can serve another insertion while a squad stays in
the field, or an outstanding pickup after return without a fresh insertion. It
keeps the call control available and still waits for durable banking and actual
disembarkation before reuse. This is one shared aircraft, not concurrent private
aircraft/cinematics. Seventy focused transport cases pass (69 in the main run,
one added stale-operation case); Windows64 compiles and the owned native HSC/CE609
cache compiles. Initial new fixture setup bypassed the ordinary owner-open step,
which correctly cleared unowned membership; the fixture now opens gear before
assigning its test membership. The corrected baseline reproduces three departure
veto failures before the runtime fix; same-squad waiting already passed.

`build/native-delta-squad-transport-fixed/result.json` has PARTIAL native acceptance:
two authenticated squads physically complete separate insertion trips. Squad 1
then physically evacuates/banks two items while squad 2 stays ready in the field.
After the first crew's normal disembarkation, the pilot accepts the waiting second
call and lands for pickup without another insertion. Squad 2's boarding/banking
is UNVERIFIED: its steering retained a seat-4 approach after the native nearest
seat changed to 1 and stopped outside the preserved <1.0 interaction distance
(>=1.10). The aircraft is landed/unlocked/healthy and the diagnostic's native seat
admission succeeds; no engine interaction regression is established. An exhausted
approach will be replanned when the selected seat changes; that adapter correction
is unverified. No third unchanged route run or speculative boarding-radius patch.
An earlier partial run was stopped to fix driver phase accounting before later
evacuation, rather than treating old insertion counters as an exit instruction.

Stopped-log comparison passes 582 player seconds and 586 item snapshots, zero
divergences/out-of-step items, position max 0.866070 and no aim mismatch beyond
one degree. Durable records show one owner banked and the other still deployed;
overall workflow completion remains false. Binary SHA256
`d67daf2ea6bd9cde361c53caedc32be6d0862bccd5b2961fd535654b8b1d2f33`;
cache `23a8f945f5b0d056d7b693cc82651a66b40a8111c28b39abc82d9773ebcfa746`.
Original identities/source are unchanged and stopped runtime/map copies removed.
Full Black Echo, both squads' completed serial recovery, six independent clients,
Spirit dispatch replay, private presentation and other platforms remain open.
The complete engine goal stays active; continue independent engine work rather
than repeating this unchanged steering case.

Accepted crowded-rescue checkpoint (source base `f7bdca9d02`): held Action keeps
an existing valid treatment stable and selects the nearest available allied
casualty for a new treatment. Lower player slots no longer divert it. Busy
caregivers, native faction/collision/range, damage, pending death, interruption
and once-per-tick progress remain protected. Existing medical/inventory owners
remain; no squad-only rescue restriction, new state manager or packet was added.

Native execution exposed another real defect: scripted current-vitality repair
still reached every player despite the existing authored-action policy, silently
repairing the first casualty during second-casualty preparation. The current
setter now respects that policy; default legacy party-wide repair is preserved.
Maximum-vitality scaling remains campaign-wide, not a newly supported per-player
replication capability. Actual before-fix failures cover both targeting defects
and the repair-routing defect. Thirty focused medical cases (including corrected
negative controls) and eighteen existing co-op action cases pass. Windows64
compiles (`build/medical-target-vitality-build.log`). The initial nearest-target
negative mutation caused an unused-variable compile failure; its corrected fault
keeps the variable read and reaches the intended failing gameplay assertion.

`build/native-medical-target-selection-fixed/result.json` records three local
native split-screen bodies. Ordinary held Action treats player 2 first despite
player 1 becoming eligible closer to the healer, then treats player 1. Both
five-second revivals preserve the original bodies. Tick 630 reads healer health
1.00 and both recovered players 0.25. Original identity hashes are unchanged;
no credentials were copied into this offline probe. The screenshot wait FAILS:
the probe omitted its destination directory. Its overall runner result remains
false, while health/treatment-log acceptance is separate; no image or independent
client acceptance is claimed. Future probe setup creates that directory; no
third native run merely repeats these completed rescues. The first failed probe
also incorrectly assumed an offline runtime could not generate its own local
identity file; cleanup was completed and that assertion removed. Stopped runtime/
map copies are removed. Native fixture source is retained; the reusable squad
source is restored to its prior accepted hash. Existing banking and authenticated
two-client evidence remain accepted without repeating them.

Read-only Spark `4249bb5` now reports a composed seven-objective native core,
with repaired Flood/route BSP diagnostics, rather than the earlier two-beat work.
It still explicitly requires creation of the not-automatically-placed technology
weapon after retiring the original startup, plus full pilot/crew and aircraft
controller composition before delivering the combined CE609 mission. The existing
contract is updated; Spark files were not modified or rebuilt. Full mission,
independent six-client, maximum-health-per-player and other platform/hardware
acceptance stay open. The complete engine goal remains active.

Accepted squad-guidance checkpoint (source base `b27833e172`): the existing co-op
nav cache now retains the configured players' and native teams' four-marker
budgets. Two six-member squads' two markers each previously retained only 16/24;
both host/client failures are reproduced before the fix. The native four-marker
HUD/save layout is unchanged. Existing event replay sends at most eight markers
per tick, keeps its cursor across refreshes and retries queue congestion without
starving later squads. No new packet, marker manager, quest or identity system.

Windows64 compiles (`build/squad-guidance-build.log`). Twenty-five existing
marker-harness cases cover the enlarged bounded cache, per-view isolation,
deactivation, busy-queue retry/fairness and three new negative controls; ten
medical-guidance regressions pass after sharing the native constant through
`hud.h`. These bounded source checks are not twelve independent clients.

`build/native-delta-squad-guidance/result.json` passes two authenticated clients'
separate objectives, durable banking and first-squad same-ID redeployment:
170 player seconds/174 item snapshots, zero divergences/out-of-step items.
Its visual guidance check FAILS: a waypoint on the shared collectible disappears
after the first squad picks that weapon up. The exact cause belongs to this
owned fixture's authoring, not Spark or an engine regression. Original HSC and
failed client screenshot remain. Only that authored target was corrected to a
native cutscene flag; no runtime rebuild or repeated banking test was needed.

`build/native-delta-squad-guidance-flag/result.json` passes the focused native
Windows/OpenGL check. Inspected PNGs show only the host's extraction marker
(15.1m) and the client's still-active artifact marker (22.4m), after the first
artifact pickup. Player/item comparison has zero divergences/out-of-step items.
Both sessions use binary SHA256
`1c38a681660ea2baab3e1e7e6ca578224c30abc9c26d2f9beacb05d3ba399d53`;
corrected cache `e71943d678379b44decc2373e39ad89afde535ae42ab6c0ca9fd8b20aae871c3`.
Original identities and accepted squad/objective caches are unchanged. Stopped
runtime/map copies and raw BMPs are removed; four PNGs, compact evidence and the
owned reusable fixture remain. Spark's contract now gives native marker calls,
limits and the stable-location requirement. Full Black Echo, six independent
clients, scoped audio/cinematics, native late joins under a full marker budget,
VR and other platforms remain unverified. The complete engine goal stays active.

Accepted return-feedback checkpoint (source base `8ed77de466`): normal network
Extraction notices and debrief pages share the existing chat layout. Complete
text/controller prefixes fit the existing notice packet; chat owns wrapping.
The local native HUD remains the fallback for offline/hidden chat, with its
record budget and opposite display order preserved. Global chat notices are
queued once per machine, while native HUD fallback still addresses each local
view. Return/squad summaries are shorter; the duplicate report formatter is gone.
No new renderer, notification manager, packet, save or Spark input was introduced.

Windows64 compiles (`build/extraction-notice-fallback-build.log`). Thirty-six
focused notice/debrief/medical/identity cases are covered by the main run and
the corrected native-prefix word-boundary expectation; tail fault controls pass.
`build/native-delta-extraction-feedback/result.json` passes two authenticated
clients' normal return displays: inspected host/client PNGs have readable reports
without the overlapping large HUD text. Comparison: 94 player seconds/98 item
snapshots, no divergences/out-of-step items. Its binary SHA256 is
`c6019e385f93d88496410b6ee8a526412ccb97c08bf9dd60f52933b4c4c566fd`.
`build/native-delta-extraction-feedback-fallback/result.json` separately verifies
the hidden-chat host's four native HUD rows in correct order and the visible-chat
client's report. Comparison: 123 seconds/127 snapshots, no divergences/out-of-step
items; current binary `2a741d7f3861c4d1ee4c141fcd3ae31fc4309586ec1b5d947d8ef74ae59a20be`.
Both use the unchanged accepted `25d28447...` cache. Original identity files are
unchanged; stopped runtime/map copies and raw BMPs are removed, four lossless PNGs
and compact evidence retained. These UI probes exercised normal banking; accepted
restart/redeployment, objective and aircraft evidence was retained. Standalone offline
startup, split-screen/VR/human acceptance remain open; remote hidden chat retains
its inherited console behavior. The complete engine roadmap remains active.

Accepted authored squad-objective checkpoint (source base `39c45fc17b`): ordinary
static HSC helpers select only living, verified participants in the squad's current
operation. Existing authored co-op policy is required. Optional native readiness
groups gate extraction independently, cancel revoked readiness before escrow and
rearm at a new confirmed deployment. Native HSC still owns objectives, with no new
quest/save/packet system. Native list allocation now reports reference exhaustion
without inventing members, allowing queries to discard incomplete lists safely.

Windows64 compiles (`build/squad-objective-build.log`). The existing Extraction,
co-op gate/action and stale-object harnesses cover 322 cases across the regression
run and focused corrections. Test adapter signatures, Windows CRT naming and an
ambiguous fault anchor were corrected; no production protection was weakened.
The real native accessor verifies the safe fallback index 32767; negative indices
select the first object and are not used. Native HSC/cache compilation passes.
`build/native-delta-squad-script-objectives/result.json` passes two authenticated
Delta clients' separate native volume objectives, readiness, physical return,
banking, callbacks and same-ID redeployment while the other squad remains deployed.
Each owner banks one distinct item. Player comparison: 166 seconds, zero
divergences, position max 0.075134, no aim mismatch. Items: 170 snapshots, zero out
of step, all four new host items seen. Original identity files are unchanged.
Cache SHA256 `0ee91a31da3ef28725f98e4a1dbf0d3a4fa172766b09c9dc3f0baf9355470616`;
gameplay binary `4ffa99f1bdb0fffa7e2983005b3458457d657099e3d8a173bd6fc1dfa7a60b87`.
Existing discovery/reference client now negotiate `extraction.squad_objectives.v1`;
the actual preceding executable lacking its advertisement is refused. Twelve
discovery cases pass across the main run and corrected stale network expectation.
Advertised protocol 45/native layout 44 remain unchanged. Metadata-only final
build SHA256 `20259a4d2143d83cdb37b6d153da6e52cff865338232f23650fee91fddad6310`;
gameplay was not repeated for that metadata change. Stopped native/discovery copies
are removed; owned source/reusable fixtures remain. This accepts two single-member
squads' trigger objectives/settlement, not full Black Echo, six clients, rescue/
combat here, alliances, scoped presentation, per-squad aircraft, persistent squad
membership, other platforms or hardware. The complete roadmap remains active.

Accepted squad checkpoint (source base `b10d91264e`): optional native hub terminals
select host-owned six-member squads inside existing Extraction runs. Membership
requires existing gear identity/locking, matching factions and ordinary powered
control use. Gear remains per owner. Active members cannot switch squads/factions
or redeploy over unfinished squad operations. Reusable squad completion callbacks
follow actual durable survivor settlement; another deployed squad does not block
them. Disconnect/slot reuse and owner changes do not inherit membership.

Windows64 compiles (`build/native-squad-build.log`). The existing Extraction
harness covers admission, capacity, pending writes, loss, independent callbacks,
redeployment and legacy behavior. The regression run and focused corrections/
negative controls cover 232 cases; compact logs are `build/native-squad-*.log`.
Test adapters now declare fatal exits correctly, and the old departed-refund fault
removes both existing refund protections instead of falsely expecting one removal
to lose equipment. No production refund/transport protection was changed.

`build/native-delta-squad-settlement/result.json` passes two real authenticated
Delta clients normally selecting different squads and deploying. Squad 1 banks
two unique items, wakes its native HSC callback and redeploys those same saved IDs
while squad 2 remains deployed. Squad 2 then banks its own distinct item and wakes
its own callback while squad 1 remains deployed. Player comparison: 122 seconds,
zero divergences, maximum position difference 0.086464, no aim mismatch. Item
comparison: 126 snapshots, zero out of step; all five new host items seen.
Cache SHA256 `25d28447a49aa4ddf849729f1890b1ccaad78bb4b9c55be30c2bbc4ba1a45196`;
binary `06f50ed1253d89334f81e767219467938fa94afc2a58b0dacec4fb61d8c1e696`.
Original identity files are unchanged; all stopped runtime/map copies are removed.
One reusable compiled fixture remains with its existing owned source tags.
Earlier input attempts exposed a terminal beside a parked aircraft, cabinet
approach and an over-eager log assertion; corrected in disposable authoring/input,
with collision, normal interaction and all settlement gates preserved.
This accepts two single-member squads' native selection/settlement/redeployment,
not six independent clients, squad objectives/alliances, scoped cinematics/audio,
same-faction aircraft scheduling, persistent squad membership, full Black Echo,
other platforms or human/headset acceptance. Those requirements remain open.

Accepted usability checkpoint (source base `9eb95de7e8`): Server Setup -> Co-op
Options -> GAMEPLAY now selects CLASSIC, FIREFIGHT or EXTRACTION through the
existing settings writer. The latter two are exclusive host choices; Classic
preserves authored HaloScript encounters. Extraction still requires a compatible
native solo map. No new config format, director, launcher or packet was added.
Fourteen focused existing menu cases pass (`build/operations-mode-menu-tests.log`),
and the incremental Windows64 build passes (`build/native-operations-mode-build.log`).
The actual offline native menu saves Extraction, reopens it after process restart,
preserves it on Cancel, then saves Firefight and Classic with the competing mode
disabled. Rendered selection/help and restart captures were inspected.
`build/native-operations-mode-menu/result.json` records binary SHA256
`d52975701a076279b6132e33980dfc5036f704db07ccf62b7ce509d87ccbb60e` and unchanged
original UI inputs. This verifies menu/settings only, not a new mission or
multiplayer acceptance. Stopped runtime/input copies and BMP scratch are removed;
compact PNGs/logs remain. Accepted transport checks below were not repeated.

Accepted transport checkpoint (source base `39ca3cd9a7`): the existing native transport
HSC now shares one configurable flight/boarding/settlement loop for Pelican or
Spirit. It resolves the named craft when used (globals initialize before objects
are placed), tests the single aircraft directly, recognizes both native passenger
seat families and exports both existing durable-completion callbacks. Rejected
hub bindings now report the actual faction/profile/body/signal indices; no
admission, identity, flight, collision or bank gate was weakened.

Windows64 compiles (`build/native-spirit-admission-build.log`); both native HSC
bindings compile. `build/native-delta-spirit-normal-controls/result.json` passes
two distinct authenticated Delta players selecting Covenant/Grunt, preparing real
equipment, normal boarding, seated insertion, native landing/disembarkation, empty
hub return, recall, pickup landing, physical evacuation boarding, departure camera,
loaded hub return and banking one unique item per owner. Fresh native processes
redeploy the same two saved IDs with matching actual weapons. Existing comparison:
390/89 player seconds, no divergences under its existing tolerances; one transient
18-degree aiming mismatch in the operation, none after restart. Item comparison:
394/93 snapshots, zero out of step. Both departure PNGs were inspected.
CE609 SHA256 `bc2b80b52d519e166c318bd1d8c24a585c1b94a7965222a3fb10524ddc2b5f25`;
binary `a6360793da5abebfc251268bf1a95959c9bb5335432419d5b3906c1730d12650`.
This accepts the owned transport fixture, not the complete Spark mission, combat/
rescue in this cache, opposing simultaneous squads, other platforms or human/VR
acceptance. The previously accepted Pelican cache and original fn_native are unchanged.

The owned Spirit uses its actual native model/animation/collision/physics, a
compatible Grunt pilot variant whose graph supports its exact driver label, and
a measured AI throttle budget of 0.30. The initial 0.65 approach coasted outside
the pad. Hub/LZ volumes were not enlarged. Authored duplicate groups and a missing
pilot animation label were corrected in disposable content. Input-driver repairs
(crew preparation order, ordinary button approach and stick-step tolerance) are
verification overhead, not engine features; no new test framework was added.
All stopped runtime/map staging and BMP scratch are removed; compact logs, PNGs,
owned source tags and gear evidence remain. Original identity files are unchanged.
No hosted CI run, push or publication was requested. Full roadmap remains active.

Accepted Pelican checkpoint (source base `a3c820a142`): two distinct authenticated Delta
clients complete the physical Pelican roundtrip in the owned Windows64/GL fixture
`build/native-delta-transport-complete-crew/result.json`. Both normally board,
fly/land/disembark, request extraction, board the returning aircraft, depart through
the native camera, arrive at the actual hub and bank four unique items across two
owner records. Fresh processes redeploy both owners with the same saved item IDs.
Player comparison: 244 player seconds in the operation and nine after restart,
zero divergences under the existing tolerances. Item comparison: 248/13 snapshots,
zero out of step. Original identity files are unchanged. Native CE609 SHA256
`dccd58c9d9703ae5702aaf6df363d163f38b7dc4c7b28940fd6a417447ab2c2b`;
binary `54414ece225094553a99beb42345fcfc21eeaa4b120aa5ca1736e1d8951f10cd`.
This is bounded native transport execution, not the complete Spark mission,
combat/rescue in this fixture, Spirit flight, human acceptance or VR proof.

Configured human-plane movement uses the aircraft's native world origin while
preserving pilot perception. Final grounded settling uses the existing authored
destination tolerance, not an invented clearance multiplier, and does not interrupt
a valid recovery path. The fixture's four-unit final radius matches its native
aircraft physics radius; unchanged hub/LZ volumes and real collision/boarding/
settlement checks remain mandatory. Camera offsets are object-relative; native
Invader angles are radians, with orientation derived through Halo's native Euler
convention. Departure frames from both clients show the aircraft. Earlier mistaken
degree/angle assumptions are corrected in the existing owned source profile;
historical failed evidence remains. No taxi controller or new flight engine remains.
Windows64 compiled (`build/native-plane-native-radius-build.log`); previous 13
actor/readback and 51 transport checks remain accepted, not repeatedly rerun.
The earlier serial-boarding probe banked gear but failed full acceptance because
one player's input path left the hub before insertion; the final ordinary-input
route keeps both initial boarding approaches within the unchanged hub.
Stopped runtime/map staging and BMP scratch are removed; compact logs/PNG/gear
evidence remain. Original fn_native SHA remains
`09c939b9b440dd25a8c139dea7283253dcb9ea227bb7f0bfd845bee82ba2e4b3`.

CI startup is externally blocked, not a compiler failure: the owner supplied
GitHub's failed-payment/spending-limit annotation for Build `43918366727`,
gate job `133478596699` (runner 0, no steps). No account spending is authorized.
Local commit `569487db0d` makes Build/Web/Server Image manual-only, excludes
Forerunner label runners and blocks its hosted cache writes, fixing inherited
unguarded/duplicate cache actions. Twelve focused workflow cases pass, with the
Web cache follow-up passing; the complete trust gate is unchanged. No hosted job
was dispatched, rerun or pushed. These source restrictions only affect GitHub
after the founder publishes them. The latest inspected repository state has no
running/queued jobs or successful runs and no artifacts from the two inspected
failed runs; account-wide charges and retained storage are not verified.
The owner must retain an account Actions $0 budget with Stop usage enabled;
the repository connection cannot inspect or change billing. Do not bypass this
external block or claim hosted verification. Continue local engine development.

Read-only Spark `bb8e906` adds recovered objective/native mission composition;
its checkpoint still requires full Black Echo composition and fixes to out-of-BSP
AI/flight points. It also includes reviewed aircraft append/cache readback,
`returnAfterInsertion` empty-return/recall controller authoring and native route/
landing-pad authoring/readback. Geometric endpoint admission is not observed flight.
Its current handoff still explicitly lacks the complete combined Black Echo CE609 flight
package; marker caches and controller-syntax probes are separate evidence. Exact next authoring requirements and source references are
in `SPARK_INTEGRATION_CONTRACT.md`. Previously accepted actual Black Echo two-account
combat/rescue/technology/Flood/defense and on-foot banking/restart remain accepted;
they do not prove aircraft evacuation. Next native milestone: the real aircraft-
equipped Spark mission, then independent squads
and the complete roadmap. Preserve this passing fixture rather than rebuilding
or rerunning it without a changed dependency or reproduced regression.

Previous accepted checkpoint (base `b813f9611e`): faction selection now refreshes only that
local viewer's native objective markers from the existing co-op HUD cache.
Clients retain received markers in the same cache; no packet, squad manager or
quest state was added. First attachment, same-team respawn and recovery preserve
the native HUD rather than clearing checkpoint-restored guidance. Windows64,
12 connected cases and four defect controls pass. Final two-account native
`build/native-delta-faction-guidance-aim` verifies normal Covenant/UNSC selection
and swapped roles: each view replaces the previous faction's marker, without
deployment or banking. Comparisons pass (54 player seconds/58 item snapshots),
no exception; original identities unchanged. A preceding armed/banked swap also
passes (190/194); the first final-input probe missed the high human control while
aiming horizontally as a Grunt, corrected without rebuilding the binary/map.
Runtime/map staging, 49,152,540B of BMP scratch and the 110,030,744B derivative
cache are removed; original faction cache unchanged. Native checkpoint/respawn,
other platforms, independent squad membership/objectives and dropship flight
remain separate acceptance gates. Continue the gameplay roadmap, not UI polish.

Accepted debrief checkpoint (base `ea5409a3c6`): personal native debrief now connects
confirmed recovery/loss, item details, combat totals and next deployment through
the existing gear transaction and native control/chat UI. No save or packet
format changed. `build/native-delta-debrief-complete` passes two verified Delta
owners banking, reading separate report pages without changing gear, redeploying
the same saved items, and native death/loss with the other stash unchanged.
Comparisons pass: 275 player seconds, 279 item snapshots, all nine new items
seen, no divergence or exception. The earlier UI probe used the wrong distance/
aim for the Grunt; normal close-range input renders all four rows. Native readback
exposed plasma battery misclassification from a placeholder magazine; the native
age-limited flag fixes it. Final Windows64 compilation, 12 debrief cases, three
defect controls and 49 transport workflows pass. That last battery correction
is compiled/automated, not yet visually rechecked. Runtime/map copies and
44,237,286B of converted BMP scratch are removed; original identities preserved.
This accepts the hub-return results component, not full Black Echo, aircraft
return/banking, human play or platform parity. The pilot left its seat during the
failed aircraft return; keep native collision/ejection and the bank gate intact.

Accepted transport checkpoint (base `818c305537`): transport safety gates are already
committed, including scheduled-death rejection at `51883fce31`; the Spark
contract now identifies their exact verification boundary and native flight
handoff. Existing pilot diagnostics expose driver/throttle/facing and native
go-to rejection without changing control or packets. Windows64 compilation,
12 readback cases and two authority/ownership defect controls pass. Latest
single-account `build/native-pelican-pilot-arrival` passes physical boarding,
insertion and disembarkation, but empty-craft return misses the hub volume.
No pickup, aircraft banking or redeployment is accepted; no native exception.
The native go-to completed without actual vehicle arrival, so further work
must address physical flight/landing rather than weaken the gate. Owned runtime
and map staging are removed. Pending debrief source remains separate; continue
playable dropships rather than extending documentation or repeating this route.

Accepted follow checkpoint (base `4ec42be17a`): bounded two-account native Marine
following/return now passes. `build/native-delta-encounter-follow-ground` verifies
the nearby field player retains the same Marine while another player stays at
the hub, then the Marine changes group and walks from (54.843,-85.471) to
(49.138,-87.785). Unchanged player/item/AI comparison passes (24 player seconds),
no exception; original identities stay unchanged. The earlier fixture inherited
the aircraft encounter's 3D firing-point flag. Host AI readback proved active guard
state with no destination/path; clearing that flag on the owned ground encounter
let native compilation provide usable BSP-surface firing points. Existing guard
contexts/state were retained. No pathfinding rejection, motion or mission gate
was bypassed. The existing opt-in readback now exposes host action/destination/
path state with full ownership checks; its private network-action count is named
separately from the native AI enum, without changing wire values. Windows64,
12 readback cases and two defect controls pass. Owned runtime/staging and derivative
cache are removed. This accepts Windows two-client follow/navigation only, not
full Black Echo rescue objectives, independent squads, six clients or dropship
flight. Pending debrief source remains preserved and native-UI unverified.

Earlier follow-target checkpoint (base `51883fce31`): opted-in Extraction encounters using native
`ai_follow_target_players` now choose the nearest living, correctly owned body.
Classic campaign target ordering and explicit unit/AI targets remain unchanged;
the native eight-target bound is retained. Windows64 compilation, 12 connected
follow/migration cases (six simulated players, death/respawn, stale ownership,
ties/tolerance and legacy behavior) and two defect controls pass. No squad manager,
quest engine or network format was added. `build/native-delta-encounter-follow-native-guard`
proves two authenticated clients and the expected field/hub encounter-group choices;
unchanged player/item checks pass for 69 player seconds. Physical Marine return
FAILS: net displacement 0.0108 units, and the unchanged actor comparator returns
ERROR for no common tracked AI movement. Firing-context and guard-state fixture
corrections did not solve movement; cause remains unresolved. Do not rerun this
candidate without new navigation/state evidence. This is not a completed escort,
independent squad objective system or six-client acceptance. Originals/identities
are preserved; owned runtime/map staging and derivative follow cache are removed.
The existing pending debrief source remains uncommitted and native-UI unverified.

Accepted transport-crash checkpoint (base `f01c33a11b`): existing passenger receipts now reject
all three native scheduled-death flags on their ship, matching physical admission.
This closes an automated reproduction of banking a dying craft at the return pad.
Windows64 compilation, 49 focused transport workflows and an original-defect
control pass: no premature bank/consumption/loss for a surviving passenger, no
receipt inherited by a salted replacement at the hub, fresh physical LZ boarding,
cinematic-gated recovery once and redeployment. Crew readiness also clears on a
dying ship. Build evidence: `build/native-transport-crash-build.log`; connected
cases: `tools/harness/tests/test_extraction.py`. These are compiled/automated
results, not native crash/rescue or completed flight acceptance. Read-only Spark
`6c5e9dc` adds native command-list editing; no completed flight handoff was found.
Do not repeat the unchanged failed recall route. Preserve the unfinished debrief
work; return to a different independently actionable engine requirement.

Accepted faction-call checkpoint (base `66092de992`): optional native Pelican/Spirit call groups
now distinguish real faction requests at the shared extraction control. They reuse
the existing device-group binding/replication and host run/seat ownership; no new
state manager or protocol. The existing aircraft HSC consumes the Pelican request
while native activation continues, retaining the separate physical-crew departure
gate. Windows64/native HSC/cache compilation, 44 focused transport workflows and
two defect controls pass. `build/native-pelican-faction-call-deployed` verifies
authenticated deployment, physical boarding, insertion, disembarkation and the
native pilot script consuming the request. It intentionally stops at recall
initiation: no aircraft bank, no exception, gear still deployed. One earlier stale
driver mistook the script prompt for deployment and correctly remained parked;
the corrected probe uses the actual durable deployment record, same binary/cache.
Owned runtime/map staging is removed. Spirit/multi-client request signals and full
recall/landing/departure/banking remain unverified. Unfinished debrief source is
preserved separately; it has not passed native UI acceptance. Return to actual
aircraft flight/content work rather than extending the report UI.

Accepted casualties checkpoint (base `265a333e20`): equipment-only faction profiles no longer
clear a freshly spawned biped's native health/shield values. The existing
Extraction hub respawn override is preserved, with no new respawn controller.
Windows64 compilation and actual two-account native casualties/re-entry pass:
`build/native-delta-faction-respawn` uses previously banked real test gear and
shared native-cache admission, normal grenade deaths, same-faction Grunt/Spartan
hub respawns with fresh native health, confirmed losses and redeployment. The
other player's body/gear stays intact. Incompatible stored gear survives unchanged;
issued UNSC gear is lost and failed runs award nothing.
`build/native-delta-faction-loss-restart` resumes the same binary/retained in-flight
records and proves loss/stash preservation after process restart and redeployment.
The first restart probe mistook old deployed records for a new deployment before
faction selection; only that stage was repeated, with no rebuild. Player comparison
has no persistent divergence (122 operation/50 restart seconds; one transient
alive/grenade difference each in the death run); items match for 126/54 snapshots.
Original identity files unchanged; owned runtime/maps and 235,932,192B of BMP
scratch removed; two lossless frames retained. Native two-faction Pelican/Spirit
boarding remains accepted at `265a333e20`. Full flight/departure/banking, other
bodies/platforms, human play and VR remain open. Spark `11f5849` was inspected
read-only: newer area authoring, no new flight handoff in its integration notes.

Accepted transport repeat checkpoint (base `0c32af94de`): repeatable aircraft scripts now separate
one-time pilot setup from the trip loop, park continuous camera/crew handlers until
woken, wait for old passengers to disembark and reuse native gear/seat readiness.
Cleared players can recall through the powered LZ control without another gear
transaction/countdown. Legacy whole-mission completion stays dormant-only/one-shot.
Missing faction craft bindings now refuse selection/recall without reserving gear
or falsely promising hub escape; those final guards are compiled/automated only.
Windows64/native HSC/cache compilation, 19 focused workflows and two defect controls
pass. `build/native-delta-crew-cycle` verifies two-account insertion and presentation
handler reactivation (three explicit camera wakes); both peers enter/leave each
cinematic. Settlement stays parked/false and gear remains at risk. Captured player/
item comparisons pass (73 seconds/77 snapshots), no native exception, original
identity files unchanged. Runtime/maps and 103,220,334B of BMP scratch are removed;
two lossless probe frames remain. Actual recall flight, departure/bank, full second
trip and Spirit remain unverified. Spark `d8136fa` was inspected read-only at this
milestone: native camera/patrol authoring advances; no Spark rebuild/write occurred.

Accepted boarding checkpoint (base `d201275d55`): native dedicated device groups
now coordinate boarding with existing HaloScript. A first passenger cannot depart
without other deployed hub crew or same-faction requested evacuation crew actually
aboard. The existing aircraft script consumes the signal and covers all ten native
Pelican passenger seats. Windows64/native HSC/cache compilation, nine new connected
cases (115 workflows across this cycle) and three defect controls pass.
`build/native-delta-crew-boarding` proves two bound Delta accounts deploying,
first passenger waiting 90 ticks with native output zero, second physical boarding,
synchronized insertion and both disembarking. Player/item comparisons pass (54
seconds/58 snapshots), no native exception, original identity files unchanged.
Runtime/map copies and BMP scratch are removed; compact evidence remains. This
advances real two-player insertion, not recall/departure, Spirit, six independent
clients, VR or full aircraft banking acceptance. The failed recall route was not
rerun. Continue actual authored approach/landing and departure content next.

Accepted faction/loadout checkpoint (base `f1cea96764`): native hub faction controls now connect
per-player allegiance, authored starting kits, recovered-loadout priority and the
existing reservation/bank/redeployment transaction. Windows64 compilation, 100
connected workflows and a grenade-stacking defect control pass. Two real bound
Delta accounts choose Covenant/UNSC through normal controls, deploy with plasma
rifle/assault rifle, escape/bank, restart and redeploy the same two unique saved
items. Captured player comparisons pass (97 operation/14 restart seconds), item
comparisons show zero mismatches, and no native exception occurs. Evidence:
`build/native-delta-faction-workflow`; original identity files are unchanged.
This is hub-return component acceptance, not aircraft, changed character models,
faction objectives or full Black Echo acceptance. The first readback attempt had
a probe regex error, corrected without rebuilding or modifying the native map.
Contract updated in place; continue playable transport boarding/departure next.

Transport continuation: native recorded-flight chaining now preserves the unit's
original controllability and clears the previous flight's kill/delete/hover flags
when reused before cleanup. Four connected lifecycle checks and both original-
defect controls pass; incremental Windows64 compilation passes. Source:
`source/cutscene/recorded_animations.c`; build log `build/native-recorded-flight-build.log`.
This is compiled/automated evidence, not native flight acceptance. The fixture's
AI recall still misses the LZ; actual authored flight/landing data remains needed.

Transport crew continuation (base `4ab2ec2c31`): native Pelican/Spirit settlement
wakes are now independent of whole-operation completion. Real boarded receipts
group each crew; pending durable settlement/loss waits, and another craft or field
player cannot hold its release. The owned aircraft HSC consumes the Pelican wake.
Windows64 compilation, six new connected cases (106 total workflows), two defect
controls and native HSC compilation pass. Build evidence is
`build/native-transport-release-build.log` and the existing aircraft fixture's
`transport-release-script.log`. Actual flight/callback gameplay remains unverified;
the unchanged recall route was not rerun. No new protocol or flight manager.

Accepted Black Echo checkpoint (base `71b761be02`): Spark `ac3f144` supplies a reproducible
CE609 candidate. Forerunner reproduced its exact `08389ed4...` cache and independent
scenario/HSC readbacks, then completed the two-account native operation with fresh
isolated gear: deployment, combat, Marine rescue, actual technology ownership,
optional Flood, defense, both activations/returns/banks, settled callback, process
restart and the same four unique saved items redeployed. Player/AI/item comparisons
pass; see `build/native-spark-black-echo` and `TEST_EVIDENCE.md`. This accepts the
current candidate's bounded automated native path, not expanded Black Echo,
human play, revival in this session, dropships or independent squads.

Transport source now extends the existing Extraction settlement: optional native
`extraction_pelican`/`extraction_spirit` names require a valid faction-assigned seat
at the LZ, continued boarding to the return hub and an ended native cinematic.
Walking/timing alone, stale ships, wrong seats/factions and disembarkation cannot
bank. Existing hub-only maps and finite Survival rewards retain their paths.
Windows64 and 89 connected workflows plus a cinematic fault control pass; actual
transport gameplay is UNVERIFIED. The tested Spark candidate has zero vehicles
and zero vehicle-palette entries. Next: native transport flight/seat/camera content
under `SPARK_INTEGRATION_CONTRACT.md`, plus faction selection/bipeds/loadouts and
insertion. No new inventory, director, protocol or launcher. Owned runtime and
source-build scratch is removed; compact native evidence and six PNGs remain.

Native transport checkpoint (base `67b9b42fac`): the contract and mandatory
boarding/arrival/cinematic banking gate are committed. The original native
`tools/fixtures/extraction_transport_native.hsc` reuses Halo AI, seats, controls
and cameras. Windows64 compilation and native script/cache compilation pass.
`build/native-pelican-recall` verifies an authenticated player boarding normally,
pilot-driven insertion, concurrent camera/arrival monitoring, LZ arrival,
normal disembarkation and activation of the extraction control, with no native
halt. The pilot's recall route fails to reach the LZ; extraction boarding,
departure, banking, Spirit flights and faction gameplay remain UNVERIFIED.
The earlier camera halt was a fixture FOV/radian authoring error; the missed
initial arrival was a blocking camera script. Both are corrected without
weakening engine validation. Activation now remains available while the pilot
repositions. Existing actor readback reports native seat geometry without
moving/boarding players. No repeat of the accepted Spark mission or gear suite.
Continue the native flight/landing path; do not substitute a timer or foot return.

Faction spectator checkpoint (base `bb83c1c7a7`): co-op selection, cycling and
HUD now require mutually non-hostile native teams and recheck live body ownership.
Mixed/unknown factions cannot fall through to the host's joining camera, including
a dead enemy host. Ordinary allied co-op retains its joining view and death delay.
The existing connected spectator checks pass, including seven defect controls;
Windows64 compiles. Native probes under `build/native-faction-spectator-*` expose
the real director target through existing opt-in readback. The final probe assigns
native teams and kills the intended player, but hub respawn precedes the expected
spectator observation: mixed-faction camera gameplay acceptance is INCONCLUSIVE.
Earlier setup probes hit an empty initial clock, console buffering and incorrect
player-list/timing assumptions; no simulation or respawn safeguard was weakened.
Native faction selection/bipeds, independent squads and transport recall remain
open. Advance their existing paths; do not repeat this unchanged hub probe.

Faction recovery checkpoint (base `0df008ab10`): existing co-op anchor search,
majority grouping, placement retries and stranded-player rescue now filter live
owned friendly bodies. Each native faction gets its own anchor in a respawn
batch; a faction without a survivor cannot hold back another. The shared native
team policy also serves spectating; no new protocol, roster or save format.
Focused connected respawn/placement and spectator checks, defect controls and
Windows64 compilation pass. `build/native-faction-respawn-active/result.json`
verifies three local native network-co-op players: ordinary movement separates
the opponent, a human death respawns 0.40 units from the ally and 15.28 from the
opponent, the Covenant death stays out with no ally, its actual director keeps
the corpse rather than either enemy, and another human death recovers while that
faction waits. No inventory/account or mission outcome acceptance is implied.
The first probe's paused-AI death did not complete; normal AI execution passes
without changing death rules. Independent clients/squads, faction UI/bipeds,
per-squad recovery rules and dropship recall remain open. Disposable runtime and
screenshots are cleaned after stopping; compact native evidence is retained.

Native allegiance handoff (base `1b5bbaaa67`): existing reliable object
descriptions now announce live team changes and apply them to reused objects.
Native co-op player attachment and authored `ai_attach`/`ai_set_team` reconcile
the matching player's team with its actual body, preserving classic MP handling
and rejecting invalid co-op team values. No new packet or faction manager.
Focused replica/ownership regressions, original-defect controls and Windows64
compilation pass. `build/native-delta-allegiance-rejoin` observes both verified
accounts receiving distinct native teams, reversed teams, live rejoin retaining
the host's allegiance/body, reassignment of the new guest and restored ordinary
co-op. This is a bounded native HaloScript command smoke, not authored faction
mission acceptance. Faction selection, equipment/objective/transport rules and
independent squad outcomes remain open; see `TEST_EVIDENCE.md` for exact limits.

Authenticated operations: local host and remote players use verified, bound Delta
accounts; offline profile owners remain separate. `37bdd12f3a` fixes host lookup
and NPC loot authority; `f9b6d8dc5f` retains authored hub starts on live rejoin while
preserving campaign/intro/cinematic gates. Existing refund, pending-death, inventory,
medical and disconnect safeguards remain; no identity fallback or new protocol.
`build/native-delta-extraction-readback` verifies two-account combat/escape/bank/
process-restart/redeployment. `build/native-delta-hub-reconnect` verifies bank
preservation on hub disconnect, hub rejoin while the host stays in the field,
deployment, disconnect loss and another rejoin without refund.

`build/native-delta-death-bank` extends the owned native fixture through actual
grenade death, explicit loss, hub respawn/new deployment, survivor pickup of the
original dropped object, both physical escapes/banks, process restart and native
equipment readback. That item's saved identity changes accounts exactly once;
four unique banked item records redeploy byte-identically. Unchanged player/AI/item
comparisons pass (103 player seconds, 107 item snapshots, zero mismatches).
The necessary source repair is diagnostic: native holstered items have no scene
parent, so the existing logger now resolves their actual inventory carrier while
leaving readied orphans and bad/ambiguous ownership visible. Thirty-five related
native helper cases, two fault controls and incremental Windows64 pass. Gameplay
rules and packet formats are unchanged. Earlier failures remain in `TEST_EVIDENCE.md`.
These runs seed earlier owned native bank records, not original user inventories;
the bankable technology weapon is a regression placeholder, not Spark's accepted
quest content. Original identity files stay unchanged; runtime/map/BMP scratch is
removed with compact native evidence retained. Spark's actual mission, broader
crash/reconnect, factions/dropships, independent squads and other-platform/VR
acceptance remain open.

Six independent instances: the NPC drop repair now passes the unchanged native
player/AI/item comparisons on all five clients in
`build/native-six-operation-world-drop`: 111 player seconds, 116 actor snapshots,
115 item snapshots, zero item mismatches and all 40 new host items observed per
client. Replica NPC death timers retain host-owned equipment until a validated
authoritative world state releases it from a dead NPC; live/player/vehicle/local
inventories retain their existing paths. Twenty-six related native helper cases,
eight retry cases and both drop fault controls pass. The earlier retain-only
attempt failed natively and remains recorded; no tolerances or packets changed.
CE unload and actor cadence fixes remain intact. This proves the bounded encounter
through extraction availability, not six authenticated banks, independent squad
objectives, Spark mission acceptance, cross-platform parity or larger rosters.

Larger native combat repair (base `c7f7a61ce4`): reserve-ammo settling is now
independent of loaded shots, so sustained fire no longer hides host pickups.
Reload protection, absolute host counts and existing packets remain. Replica
deaths clear grenade counts without creating duplicate loot, including MCC's
extra types. The original failures reproduce through real-code native cases;
18 related cases, four fault controls and incremental Windows64 builds pass.
`build/native-coop-six-loot` now passes unchanged player/AI/item checks through
combat, Marine rescue, later-player technology collection, Flood containment,
defense and extraction availability. Both peers observe the 180-round pickup;
maximum rifle ammo difference is one round, with zero items out of step across
141 snapshots. This is three local players per native peer, not six clients or
independent squads. Split-screen HUD overlap, authenticated remote banking and
the complete multiplayer escape/bank/redeploy loop remain unaccepted. Earlier
failed native runs remain evidence; detailed metrics are in `TEST_EVIDENCE.md`.

Native stowed-refresh integration (base `638dd8b1d9`): repeated host inventories
no longer forget/re-add a healthy holstered player weapon with native item flags,
full player owner and no world connection. The old helper reproduces a lost
pending weapon choice and cleared use history. The repair preserves both while
still applying authoritative ammo. Readied orphans, wrong-owner/ground items and
ambiguous NPC slots still use the existing repair. Twenty-three related replica
cases, six fault controls and Windows64 compilation pass. Actual native two-peer
switching passes player/item checks; rifle/pistol captures prove normal switching,
not a native injected pending-state fault. No packet or persistence format change.
The existing authenticated Delta overlay/binding prerequisite remains unchanged;
a test issuer alone does not enable remote banking on a plain LAN session.

Native local results milestone (base `e370889f4e`): Extraction's bank report now
renders its full stash totals and next-run instruction. Native HUD records hold
only 62 characters plus NUL; local notices retain the existing record/save layout
and enqueue word-bounded parts in headline-first display order. Plain HUD text
uses the inherited wrapper within the existing row budget; item/icon lines keep
their path. Windows64, 74 connected Extraction cases, a lost-tail fault control
and the existing Unicode parser cases pass. An actual owned local mission again
fights/rescues/collects/extracts/banks and captures the complete two-line report.
Remote/split-screen/VR results, broader UI typography and Spark's final terminal
labels remain unverified. Disposable runtime/cache/BMP files are removed.

Native Flood feigned-death repair (base `761de4e44f`): the existing object-state
format now distinguishes a nonplayer biped feigning death and carries its packed
positive vitality in the field used for vehicle prediction time on vehicles.
Packet size/version stay unchanged; players and vehicle prediction are excluded.
Client corpses retain authoritative health without an independent wake timer;
later living actor states still own reanimation. Eighteen focused replica cases,
three fault controls, actual old-receiver helper comparison and Windows64 build
pass. The default two-player mission reaches extraction availability and passes
unchanged player/AI/item checks. A separate controlled native cache changes only
Flood feign eligibility from 40% to 100%: real pistol hits produce two matching
positive-health corpses and reanimations at the same ticks on both peers. Its
unchanged AI/item/player checks pass. Original mission cache is unchanged; the
earlier failed and non-exercising runs remain evidence. Full mixed-fork native,
cross-platform and large-horde acceptance are still open.

Native local operation milestone (base `f036b81913` plus this change): a genuine
native Settings profile now completes the owned CE fixture through deployment,
Covenant combat, physical Marine rescue, technology-placeholder pickup, Flood
combat, extraction defense, activation countdown and walking back into the hub.
The runtime banks two real weapons and wakes the authored completion once.
A second engine process reopens that stash and ordinary terminal deployment
issues both: exact item identities/quantities are unchanged, stash/reserved
counts move from 2/0 to 0/2. This is local native acceptance, NOT Spark's Black
Echo package or two-player banking acceptance. Shared inventory now recognizes
Halo's detached stowed weapons through native item/player ownership; ground,
wrong-owner and wrong-parent claims remain refused. All 73 connected Extraction
workflows, ownership fault controls and Windows64 compilation pass. Passive
logging no longer blocks the existing private CE startup; the existing aim
helper supports solo encounters without replenishing ammunition. Fixture waves
use native encounter activation and one-tick spawn observation.

The two-player authored mission reaches extraction availability (102 compared
seconds, zero divergent prediction ticks; full item check passes). Its strict AI
check initially FAILED on a Flood feigned-death body value: both bodies are dead, host health
is positive and the client's kill replay is negative; both later reanimate with
matching health. That original failure is retained; the bounded native repair
above now passes without changing checker tolerances or packet layout/version.
Remote banking still needs verified host-bound
Delta identities. Remote HUD results, revive-with-banking, larger squads, actual
Spark content, VR and connected-world acceptance remain separate unfinished gates.

Current client ownership repair: a repeated authoritative inventory now checks
the actual weapon parent even when its slot already remembers the same full
handle. Detached/wrong-owner items reuse the existing release/attach helpers;
no object is cloned and missing host objects still wait. The old source fails
both real-helper orphan/transfer cases; the repair passes them, related replica
recovery and late-inventory checks. Windows64 compiles. A native two-player run
passes 87 compared seconds plus the broader item check (91 snapshots, zero
items out of step). Existing AI diagnostics now include dormant world bipeds
and their actual slots; the full-world checker passes. Client console detachment
was refused by the existing host-authority guard, which remains intact: direct
native fault-repair acceptance is UNVERIFIED. Earlier intermittent Flood rifle
failures remain evidence, not a claim that their initiating race is eliminated.

Current operations handoff: Extraction can wake the optional native dormant-void
`forerunner_extraction_complete` once all living survivors have durably banked
and physically returned. It reuses Survival's return checks and native HaloScript;
no new writer, protocol, launcher or quest VM. Connected settlement/signature/
restore/redeployment checks and Windows64 compilation pass. Native callback now
passes locally above; full multiplayer banking remains UNVERIFIED. The existing death-timing harness
again compiles with current mode adapters and preserves spectator ownership.

Native CE fixture now demonstrates two equipped players, real walking/Action,
authored Covenant combat/clear, surviving Marines physically reaching rescue,
and a real technology-placeholder weapon pickup advancing HaloScript. The final
run passes 117 compared seconds, maximum player separation 0.055, no drift;
tracked actor movement/damage/death also passes unchanged limits. HOWEVER the
earlier broader item check FAILS after the optional Flood script activates: rifles 18/20
are held by host units 17/19 but appear loose on the client. Tracked-AI PASS does
not certify those Flood bodies. The later world census confirms those bodies
exist on both peers; the attachment repair above addresses a proven stale-slot
ownership gap, with initiating intermittent ordering still unresolved;
the later native runs above add Flood combat and local banking, not Black Echo
or full multiplayer Extraction acceptance. The tech
item is subsequently dropped, not banked. A separate actual terminal attempt
correctly refuses the test session's missing saved profile. Remote banking still
needs the existing verified host-bound Delta identity, not synthesized LAN IDs.

Spark d50b6fa has authored/compiled Black Echo evidence, superseding the old
absence claim, but its disposable MCC13 cache was removed and does not satisfy
the existing CE609 startup path by renaming. Exact next handoff, manual objective
4/7, native allegiances and escort squad requirements are in the existing Spark
contract. Keep Spark read-only. Original UI/carousel/SDK hashes are unchanged;
owned peer/map staging, SDK subset and bulk captures are removed. One 106 MB
compiled fixture, its authored scenario and compact evidence remain for reuse.
Next: verified remote identities for two-player banking, then the connected
revival/banking and multiplayer results workflows. Advance independent source
work if that identity prerequisite is unavailable. Revisit the earlier intermittent rifle
failure with its retained evidence if it recurs; do not repeat unchanged audits.

Current co-op spectator repair: next-player cycling keeps the sole survivor
selected and validates live full player/body ownership, departure and death.
The existing HUD follows the same eligibility; invalid local slots are safe.
The native director returns to the viewer's body when the last target vanishes;
the old dead-camera auto-cycle yields to co-op's existing selection owner.
Connected C selection/input/body-delay/HUD checks and three broken controls pass;
Windows64 compiles. Hidden OpenGL zero-lives Firefight B40 passes 66 compared
seconds, with the joining eliminated player visibly watching the same survivor
before/after two next presses. Original ordinary-respawn preparation passed
synchronization but missed its short spectator window; that gap remains recorded.
The misleading eliminated-player respawn hint is corrected, compiled but not
recaptured. Native disconnect/last-target fallback, other platforms, larger
squads and hardware acceptance remain open. Original maps retain their hashes;
copied maps/runtimes and bulky captures are removed; two lossless PNGs remain.

Current native death repair: all three forced-death requests now block equipment
capture, deployment/terminal actions, escape settlement and Survival evacuation
completion until Halo resolves them. Medical admission, guidance and treatment
also honor silent/no-statistics requests, including a healer killed on the final
treatment tick. Native death/drop processing remains authoritative; a refused
request can clear normally. The old source reproduces improper admission and
revival; 62 connected checks and three broken controls pass. Windows64 compiles.
Hidden OpenGL B40 co-op passes 77 compared seconds: silent death interrupts the
healer, no revival completes, and the same rifle drops on both peers. The casualty
later bleeds out natively. An initial helper assumed script/network player order
matched; corrected readback validates the existing logs without another run.
Native death-to-stash recovery, authored operations, other platforms and hardware
acceptance remain open. Copied maps, peer runtimes and scratch are removed.

Current inventory repair: unissued loadouts now have a prepared reservation,
distinct from confirmed native deployments. If grant/confirmation fails, only
its new objects and added grenades are revoked; a prepared record refunds on
reopen even when the outage lasted through disconnect, slot reuse or map exit.
Published confirmations remain authoritative while their flush retries, and
issued gear retains normal death/disconnect/crash loss. The actual prior source
reproduces ungranted departure loss; 45 connected workflows, 18 record cases and
three broken controls pass. Windows64 compiles. Two real Windows-backend process
termination cases prove prepared refund versus confirmed loss, lock exclusion
and repeated recovery without duplication. No user stash or game map was opened.
Version-1 fields and phase 0/1/2 values remain; old readers safely reject new
in-flight phase 3, which needs this updated runtime to recover. Legacy deployed
records keep their old conservative loss policy. Native authored-map grants,
physical power loss and other-platform execution remain unverified. Existing
carried drops retain the prior rollback behavior. Scratch/child records are
removed; one writer/lock and the existing native file harness are reused.

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
native-map acceptance remains open. The earlier close-time refund outage is
resolved by prepared-grant recovery above; confirmed deployments retain their
normal interrupted-run loss policy. No new writer, protocol or subsystem was added, and owned
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
