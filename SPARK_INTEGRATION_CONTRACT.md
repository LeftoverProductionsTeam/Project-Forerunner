# Spark–Forerunner integration contract status

Version 1 discovery, 14 November 2026. Spark owns authoring; Forerunner owns
execution. Native read-only discovery is implemented and tested on the Windows
x86 desktop/headless executable. Live editing is not implemented yet.

Authoritative source: `D:\Project Forerunner`, local `master`. Spark is
`D:\Project Guilty Spark`, inspected read-only. This existing document is the
handoff; each team updates only its own source/checkpoint at meaningful milestones.
No separate coordination service or repeated cross-project polling/builds.

## Community credits and package handoff

Forerunner's [contributors](CONTRIBUTORS.md) and
[third-party notices](THIRD_PARTY_NOTICES.md) record runtime provenance and
asset permissions. Spark `db01563` connects its existing Preservation
records to native Storage map exports. Each export carries a credits
sidecar with the map hash, recorded creator/source/permissions and
matching imported native asset provenance. Keep that sidecar with
map packages and credit the distributed content in release notes.
The native map format stays compatible. The CMT Elite FP Arms were
used in disposable private builds, and public redistribution permission
remains unverified.

## Current Black Echo candidates

The current UNSC dedicated-pad export is
`releases/black-echo-elite-unsc-pelican-pad-20261123/be_native.map`,
SHA-256 `d113e9dbec1bae9554e7d4df3843ecc2bb6d40b4ee2c7cdf5f81831fc7b29b09`.
Spark requires `extraction.pelican_return_pad.v1` in `native_features`. The
reviewed Windows x64 test executable is `build/windows64/halo.exe`, SHA-256
`9486e339ee2a06f5ef7a0cae14c4798e553fcc157e26ccb93750ee52694cd550`.
It advertises the marker in contract v1 and passes 14 discovery checks.
Its generated build ID reports base `f94fc443`; the hash identifies the
build including the discovery change.

Native solo Windows deployment, field insertion, combat, technology pickup,
defense, recall and physical evacuation passed on that exact cache. Occupied
boarding was confirmed at tick 11790. At tick 14730 the Pelican's center was
inside the dedicated pad at (56.327, -105.464, 1.182), grounded at up.k .989
and slowing. The native landing and settlement gate then banked the rifle
and technology. After clean exit and restart in the same private save root,
the record was byte-identical. E at the hub reissued the two exact item IDs
and the player held the technology. Both processes exited normally.
Receipt: `build/native-black-echo-d113-pelican-pad.json`. Two authenticated
humans, repeat mission reset, and Covenant loaded return remain open.

Local comparisons are pinned in `build/black-echo-transport-baseline/comparison.json`.
It records the exact preserved `d113e9db` and surveyed `b947dc53` copies, the
unchanged Windows baseline, the occupied pad-brake variant and the prior
successful executable hash. Only the hash and receipt remain for that older
executable. Spark also reproduced a missed pad on the previously successful
build and map. A broad braking experiment stalled before field recall and
was removed. The replacement applies ground braking only to a powered AI
aircraft during an occupied evacuation inside its dedicated return pad.
The existing transport harness reproduced banking refusal at motion .0070
and accepted settlement only after the production brake force crossed the
native .0050 gate. Both faction-pad cases and 89 transport checks passed.
Reproduce the landing with the isolated native transport map before another
whole-mission run. Native flight/braking belongs here; the route and pad
geometry remain Spark authoring data.

Native recorded flight uses Bungie's playback, unit controls and collision.
Managed aircraft with a landing output now wait for playback cleanup before
publishing landed or crew-ready, accepting boarding or exit, and banking.
Starting native playback clears both outputs immediately on the host, so
HaloScript can wait on the next touchdown even when it starts the clip and
checks the device groups in the same tick. The Windows x64 build passed 96
focused checks and a native Extraction hub startup. The exact executable
and fixture hashes are retained in the same comparison record.
Boarding-only maps now also clear the crew output as playback starts and
keep it clear through native thread cleanup. Their authored landing rules
still govern boarding and exit. The focused two-leg regression reproduced
the stale group and then passed with the other 96 transport and recording
checks. Windows x64 SHA-256 `406051a5a8263d7ca6feb8f738bf2bf6f161ebc3028ac2c5c934a31e7ae4a1bf`
compiled. The combined recorded flight and crew-only route still needs a
live authored cache.
On return, the same native thread gate now protects passenger banking and
the successful or aborted aircraft callback even if only a boarded output
is configured. Existing route landing requirements still apply. Two
regressions reproduced premature inventory consumption and abort release.
Both passed after cleanup gating, with 99 focused checks in all. Windows
x64 built as SHA-256 `f80831ef378cd54c6d72ebb3f35a3374783ea33ccf73a1bb50370acd433b15eb`.
A final `recording_time` of zero alone can precede that cleanup. Spark should
use `recording_play_and_hover` for an authored clip ending with real ground
contact and wait for the faction's landed output. Release hover with
`vehicle_hover` before the next clip and preserve the native seat, cinematic,
pad and settlement gates. An airborne hover still cannot bank.
Crew-only transports now also wait for the native playback thread to
finish before voluntary boarding or exit. The existing authored seat
locks still control touchdown rules after playback. Landing-output
routes still require native touchdown, and damaged craft can release
passengers. The two crew-only regressions reproduced both gaps before the
fix. All 94 focused transport checks passed. Windows x64 SHA-256
`10f2792bb24bf7480d5d2451548f5c4ffe2295af716fdd1dc6266f7116b87002` compiled.

On that Windows build, the pinned Spirit fixture
`bc2b80b52d519e166c318bd1d8c24a585c1b94a7965222a3fb10524ddc2b5f25`
admitted a fresh Covenant profile. Near the landed hull, the native
readback identified unlocked seat 5 and its entrance at
(50.405, -106.966, 1.098). Normal movement toward that marker and E
boarded the Spirit at tick 5010. The player stayed seated as the aircraft
began insertion and reached (52.516, -111.847, 2.824) at tick 5070.
The process exited zero, input hashes held, and staging was removed.
This verifies ordinary boarding on the existing AI-scripted fixture.
Live authored recorded flight and Linux settlement remain open.

Insertion completion and new evacuation boarding receipts also wait for
native recording cleanup on crew-only transports. Older routes retain their
authored landing rules. The focused test reproduced a false field arrival
during a fly-through, then verified that arrival and boarding resume after
playback. All 77 native transport settlement cases passed. Windows x64
`08adb364b18a77e38f7bf024e9a88de1ef86376100fec2ddffa07f8777768f68`
admitted unchanged `fn_transq`
`23a8f945f5b0d056d7b693cc82651a66b40a8111c28b39abc82d9773ebcfa746`
and ran to tick 450 in a private hub smoke. Arrival timing is harness-verified.

On transports using an authored crew-ready group, successful native seat
entry clears the group immediately. Readiness and a new evacuation receipt
wait for the entry animation to finish. Maps without the crew-ready group
retain their existing evacuation receipt timing. Windows x64 compiled and
89 focused checks passed. The isolated E-key Pelican insertion observation
and exact hashes are in the existing transport comparison record.
When playback possesses a plane, native seat power and wake inputs take
priority over an attached suspended AI pilot's parking logic. Suspension
governs parking again when playback releases the plane. Windows x64 passed
39 focused checks and a bounded stock B30 passenger-flight control. Black
Echo's authored routes still need native acceptance.

Windows x64 compiled. 96 focused recording and transport checks passed,
including the final-frame/hover transition and both faction return pads.
The exact Windows executable, stock b30 and isolated be_native fixture
hashes are in `build/black-echo-transport-baseline/comparison.json`. Both
short runs exited cleanly, and the fixture exposed a native landed aircraft.
The combined recorded Extraction flight remains to be tested with authored
clips on the Black Echo map. No full mission was replayed for this change.

On the same pinned be_native map, both builds accepted boarding and landed
at the field. The handoff build also accepted a normal E-key disembark and
landed back at the hub empty at tick 3120 (52.437, -110.089, 1.832). The
readback in `build/native-isolated-empty-return-handoff.json` covers the
existing AI route. The same map and handoff exe also passed physical recall,
occupied evacuation, solo banking and disembarkation. The private gear record
read back two unique stashed weapons with a valid checksum. The sampled hub
pose after banking had ground contact but did not capture the settlement tick.
Receipt: `build/native-isolated-occupied-return-handoff.json`.
Spark 25a2cdc can assign recorded legs for both factions. Black Echo still
needs embedded Pelican and Spirit recordings and authenticated acceptance.

The same exe also passed Covenant selection, insertion, terminal recall,
physical Spirit evacuation, banking and normal hub exit on pinned be_spirit
bc2b80b52d519e166c318bd1d8c24a585c1b94a7965222a3fb10524ddc2b5f25. A
fresh process read the bank byte-for-byte and redeployed the same plasma
pistol item ID through the hub controls. Both processes exited cleanly.
Solo AI-route evidence: `build/native-isolated-spirit-redeploy-handoff.json`.

Stock Halo campaign co-op can spawn its second opening player before the
first script tick when a separate authored start is clear in the active BSP.
The existing wait still governs extra players, late joins and blocked starts.
The stock B30 script assigned two separate Pelicans. On two Windows x64
loopback OpenCE sessions, both boarded, rode and disembarked. The exact
tested map and executable are pinned in the transport comparison record.
Extraction uses its authored hub start policy.

Six independent loopback processes verified the stock B30 opening. The two
named players rode and disembarked, then the four additional players spawned.
The normal co-op teammate placement uses the native collision search and
rejects a placement supported by a player body. All six settled at separate
ground-level positions and responded to local input. Sixty focused checks
passed. The comparison record retains the hashes and two host movement
refusals seen during the run. Authenticated mission acceptance remains open.

The preceding UNSC split-return export is
`releases/black-echo-elite-unsc-split-return-20261123/be_native.map`, SHA-256
`571f9479c47c1105c5f4a1e7d10187b5b8996e613aa20ba6ec37084e1ae6dd0f`.
Spark verified two offline banks and byte-identical gear after restarting.
Their Windows build `eefe6b83` then refused deployment at profile resolution.
Forerunner independently matched the hash and verified a fresh profile launch,
3,680 tags, native Extraction hub admission and solo player spawn on Ubuntu
WSL2 x64 with software OpenGL. Cache unload now waits for the startup profile
scan, which was responsible for both the cache and lightmap draw assertions.
With the startup wait, an ordinary E-key deployment passed before and after
a clean process restart on fresh Windows x64 and WSL2 x64 builds. Each pair
reopened the same native profile and FRNG gear record in its private save root.
The initial restart checks began with an empty stash. Receipts:
`build/native-windows64-black-echo-restart-profile.json` and
`build/native-linux64-black-echo-restart-profile.json`.

A separate Windows x64 solo run on the exact split export banked the rifle and
technology after physical Pelican insertion, combat, collection, defense,
recall and evacuation. The occupied return took roughly three minutes after
boarding and had substantial drift. At tick 17490, the last recorded Pelican
sample before the bank was (59.561, -81.212, 1.912), landed but still moving.
After clean exit and fresh launch the idle, checksum-valid bank was
byte-identical. E at the hub moved both weapon entries from stash to
reservation (generation 4 to 6), and the technology was physically held
again. The item-ID check compared a tuple with a list and did not run the
ID comparison. Receipt: `build/native-black-echo-571f-banked-restart.json`.

The same exact export also reached solo combat, technology pickup, defense,
recall, physical evacuation and a checksum-valid two-item bank on WSL2 x64
using software OpenGL. Loaded boarding was at tick 10620. The Pelican passed
near the authored approach at tick 13470 and subsequently drifted to y -79
before the bank. The restart harness lost its PID, and its recovery copied
the bank and profile payload without native save-directory metadata. The
second process deployed a fresh profile and left the recovered bank
byte-identical. Linux reissue and item-ID continuity remain open. The
next run needs the intact private save root across both processes.
Receipt: `build/native-linux64-black-echo-571f-banked-profile-recovery.json`.
A bounded WSL2 x64 check also admitted the isolated Spirit cache
`bc2b80b52d519e166c318bd1d8c24a585c1b94a7965222a3fb10524ddc2b5f25`
on the same Linux build used for the split-export bank
(`0ea3bb127700dccb6d05c25d03c67d0c6ef4484f1f4b54e850a649fc69aef08c`).
With copied CE resource banks and a fresh native profile, normal input
selected Covenant at the live control and pressed E at Deploy. The
Covenant player started the run and held plasma pistol tag 243 at the hub.
Using its native entrance marker, a solo Covenant player boarded seat 5 at
tick 3960, landed in the field at 4500 and disembarked at 4560. E at the
field terminal called the Spirit at tick 5640. The player boarded again
at tick 6420 and returned aboard the aircraft. The game banked the
plasma pistol and two of each grenade, then E disembarked at the hub at
tick 7110. The Spirit reported ground contact and native landing there.

The first process exited zero. A second native process reopened the same
intact private profile and byte-identical generation-4 bank, SHA-256
`c6903bcd947293db7620b391ffc026fe642243d7c1a0a75d2d1db157018ecfd0`.
Normal Covenant and Deploy controls advanced the record to deployed
generation 6 with the exact same pistol ID `1fb32dcd30490a2f1319e2fc8e055a5d`
in its single issued slot. Both processes exited zero. The map,
executable and resource hashes held, and staging was removed.
This verifies the solo AI-scripted Spirit loop on the pinned WSL2 build.
The split-export bank and authenticated Black Echo mission remain open.

Reliable pad touchdown and two-authenticated-human acceptance remain open.

The compiled shared `extraction_return` covers x 36 to 76, y -119 to -81,
and z -1 to 7. The last aircraft center (-80.978 on Y) was just beyond its
north edge before banking. Keep that volume for player services. In the next
export, add a dedicated `pelican_return` around the clear touchdown surface
near the occupied target (55, -102.915, 1.8), sized to the settled aircraft
center. Point both Pelican hub arrival scripts at that pad, use the existing
native landing output, and measure the approach through touchdown. The
optional runtime binding raises the banking requirement only on maps that
author that pad. The flight path and deceleration still need native proof.

The preceding aligned UNSC route export is
`releases/black-echo-elite-unsc-hub-aligned-20261120/be_native.map`.
Windows x64 independently matched SHA-256
`40d2ce850682d1e3ed871e28481eadc0513251fe37e8b95987126cf1809299da`,
loaded all 3,680 tags and admitted the native Extraction bindings. Spark's
readback adds airborne waypoints (45.9, -100.95, 6.6) and (49.8, -101.77, 4.75)
and an aligned final approach. A separate Windows x64 run on that exact cache
used a temporary saved profile and normal keyboard/mouse input. The Spartan
deployed at tick 660, occupied Pelican seat 1 by 1320, reached native field
landing at 3030 and exited on foot at 3090. The empty Pelican reached native
ground, rest and landed at tick 5640, origin (52.399, -100.737, 1.546),
up.k 0.993 and zero motion. Its observed center (52.546, -100.546, 0.989)
is inside the authored return volume and outside both field volumes.
The run exited cleanly and the staged and protected hashes matched.
Receipt: `build/native-black-echo-40d2-aligned-flight.json`.
A follow-up Windows x64 mission on the same hash used a temporary profile
and native input. The Spartan reached the rescue rally with the Marine
alive, killed the facility guard, and collected the technology. The standing
pickup pass ended with the item unowned. From (54.276, -94.644, 0.005),
level aim and crouch gave swap action 6 for object 14 at tick 6120. E moved
the technology to player unit 11 by tick 6210. Invader independently found
byte-identical technology tags in `7026f5fa` and `40d2ce85`. Rifle fire cleared
defense. Native E at (70.628, -128.413, 0.747) activated the field terminal and
the countdown completed. The recalled Pelican landed at tick 10410 with
up.k 0.957. The Spartan physically boarded seat 4 and departed by tick 10860.
At the 135-second observation limit, the loaded Pelican was still rolling
near the hub at (50.341, -104.634, 1.815), with ground 1, rest/landed 0,
up.k 0.969 and motion 0.0444. No recovery was credited in that run.
Compact receipt: `build/native-black-echo-40d2-mission-pickup.json`.

A longer solo Windows x64 run on the same cache and executable completed
loaded return and native banking. The Spartan boarded at tick 10230. The
pilot trace reached `hub_route` command 11, `go to (p6) and face (p3)`,
and the aircraft circled the stop before settlement around tick 16950.
The host reported stash 2/32 and two frag grenades. Independent readback of
the actual checksum-verified gear file found version 1, idle phase, escaped
outcome, generation 4, the assault rifle and the technology, and zero
reserved or escrow items. The first E after banking did not disembark the
player. By tick 17250 the aircraft reported native ground, rest and landed
at (53.026, -100.860, 1.226), with zero motion and up.k 0.950. At shutdown
the player was still seated.
Compact receipt: `build/native-black-echo-40d2-mission-settlement.json`.

The subsequent uninterrupted solo run verified the rest of the native gear
loop on `40d2ce85`. The same parked pose repeated with native ground, rest
and landed at ticks 18240 and 18270. Normal Use detached the passenger by
18330. He walked to the hub terminal and used it at 19770. The verified stash
changed from idle generation 4 with two banked items to deployed generation 6
with those exact two IDs reserved and no duplicated stash items. Player unit
11 held the reissued technology at tick 19800. The engine and map were
unchanged and the private staging was removed.
Receipt: `build/native-black-echo-40d2-mission-redeploy.json`.

A second authored mission still needs authoring work and native verification.
Independent Invader readback found one wake of dormant `gs_mission`, in
startup `gs_mission_bootstrap`. The continuous Pelican settlement callback
sets only `gs_pelican_settled`. A repeatable Spark mission must rearm its
objective and encounter state after a new deployment, using the existing
native control signals or per-squad operation hooks. Leave banking and
equipment ownership with Forerunner. Loaded approach time, an exact
post-bank process restart, and two authenticated human runs remain open.

Spark's isolated run at `4249988` on the preceding UNSC cache `7026f5fa`
verified technology pickup, rifle selection, defense and field-terminal
activation. The empty Pelican circled the hub. Radius-only diagnostic
`3564bca8` stopped the circling but settled at (51.842, -101.543, 1.216)
with up.k 0.938–0.939, below the unchanged native 0.95 gate. Preserve the
authored near-pad, east-southeast approach and native contact checks.

The separate CE609 faction exports before the UNSC pickup correction were
derived from the preserved Elite start layout. The UNSC export at
`releases/black-echo-elite-unsc-20261120/be_native.map` has SHA-256
`700af26afbb9e7d63d86fb55b4683240187a0d4f6edecc8184a3b98e326a7b16`.
The Covenant defense-cover export at
`releases/black-echo-elite-defense-cover-20261120/be_native.map` has SHA-256
`88a13c271422a4f480f91e43ec31f8f1948e8ed6730969fbafbd03f188ad6e7f`.
Use the corresponding export for each faction route.

Forerunner ran the exact Covenant defense-cover export with one offline
player. Native seat 5 boarding, field landing, voluntary dismount, the
rescue rally and facility combat passed. The defender died near
(81.763, -93.901, 1.788). The Elite reached (55.276, -86.547, 0.061) at full
vitality, about 0.58 units from the authored technology at (55.846, -86.472,
0.078). The sampled aim and Use inputs did not obtain a native pickup offer.
Defense never spawned and the extraction control stayed locked. Spark can
check access around the parked Spirit and reuse its UNSC grounded-pickup
placement workflow on the Covenant route if another approach remains
obstructed. Keep the exact technology tag and its artifact-volume trigger.
The scoped native receipt is `build/native-black-echo-88a1-covenant-route.json`.

Spark's separate Covenant grounded-pickup export
`releases/black-echo-elite-covenant-grounded-pickup-20261120/be_native.map`
was verified on Windows x64 at SHA-256
`a051468999467f5c5e95e73b1215b6006da748a4cea7d684e6570487063e0fc0`.
One offline player selected the native Elite, deployed, rode the Spirit,
disembarked at the field, and reached the rescue rally. The player killed
the facility defender, obtained swap action 6 after a crouched approach at
(54.236, -94.629, 0.009), and acquired the exact technology on Elite unit 9
by tick 10080. Defense cleared and normal E started the field countdown.
The recalled Spirit landed at (93.938, -132.030, 2.104), up.k 0.991, at
tick 14340. From (89.440, -129.704, 0.769) the player received no evacuation
seat offer. Native seat 8's entrance was (89.785, -130.439, 0.482), about
1.05 units away by native readback. The approach needs to close that gap
before concluding a seat or animation defect. Loaded Spirit return and
banking remain open. Receipt: `build/native-black-echo-a051-native-spirit-route.json`.

On the Windows pad-discovery build, the same exact Covenant export confirmed
Elite insertion, on-foot disembarkation, recovery of the technology on unit 9
at tick 9660, and a usable field control after defense. The replay driver's
chase left its expected route during combat, and its subsequent straight-line
approach stalled at (53.882, -102.901, 0.005). No field use or evacuation seat
was attempted in range during this run. The first check remains the physical
approach to the recalled Spirit's seat 8 from the near side. The next replay
should keep the approach to the earlier verified field route and explicitly
track player death and respawn during pursuit.
Receipt: `build/native-black-echo-a051-native-spirit-loaded.json`.

Forerunner independently ran the earlier UNSC export with one offline player. Normal
deployment, Pelican seat 4 boarding, field landing and voluntary dismount
left the Spartan at full health and shields. The Marine and player reached
the rally and the native `be_covenant` facility guard spawned near
(58.86, -89.41, 0.05). The field control at (70.928, -128.997, 0.820)
remained locked before the facility encounter was cleared. With the revised
crew-readiness check, a second run passed deployment, boarding, landing
and voluntary dismount. The input driver then stopped at (72.77, -124.191,
0.713) outside its 0.8-unit intermediate waypoint tolerance. That stop
did not demonstrate a collision or mission-script failure. Combat,
technology, evacuation, banking, redeployment and paired authenticated
play remain open. Native receipts are
`build/native-black-echo-700a-unsc-field.json` and
`build/native-black-echo-700a-facility.json`.

The preserved 20 November CE609 candidate (Spark `8e4f912`) is
`releases/black-echo-elite-separated-starts-20261120/be_native.map`,
SHA-256 `04d9ccd199fef0bba6d30a54e7c7f3682f9766ddc0f94ee347cde6c9677c113f`.
It combines the reviewed Elite faction placement with four grounded hub starts.
The exact cache admitted and deployed a fresh offline Windows x64 profile
through native movement, aiming and Use. The player spawned at
(43.002, -110.999, 1.076), walked around the terminal, and received the
native device offer from (40.313, -110.229, 1.428) while looking toward the
control center. Use opened one disposable gear record and advanced the
operation to deployed. The prior probes stopped farther away. This checks
the hub deploy control. Field calls, paired faction play, evacuation and
banking still require gameplay acceptance. See
`build/native-black-echo-04d9-close-terminal/result.json`.

Native test recordings can use `aim:<dx>:<dy>` for relative mouse input
through the existing input script. The original map currently supplies a
placeholder name for the deploy terminal. Forerunner's existing HUD
fallback now shows a usable device prompt for that case. The Windows native
readback shows **Press [E] to use device** above the deployed notice.
Evidence: `build/native-black-echo-04d9-terminal-label`.

A fresh offline profile used the Covenant control from
(37.423, -104.510, 1.028). The runtime changed the native body from the
Spartan to the authored Elite, then retained the Elite on deployment.
The native inventory trace showed the starter plasma pistol attached to
that Elite and the sampled player had full body and shield vitality.
Evidence: `build/native-black-echo-04d9-elite-selection/result.json` and its
native trace.

An offline Windows x64 input run walked to the Spirit and boarded seat 5
through the native passenger offer. The pilot flew to the field and landed
at (93.934, -132.037, 2.106) at tick 6150. The Elite used the native exit
and disembarked at (91.337, -133.815, 0.740) with full health and shields.
The Spirit began its scripted return after disembarkation. Evidence:
`build/native-black-echo-04d9-spirit-first-landing/result.json` and its trace.

The offline run confirmed `gs_spirit_player_arrived`. A player and the
living Covenant rescue actor both reached `be_survivors`, and the facility
encounter spawned one opponent. Native actor readback located the opponent
near (81.99, -94.00, 1.78). A rock face blocked the direct approach.
The verified foot route from the rally goes south via (55.6, -94.2),
(58.0, -96.6), (60.4, -98.6), (61.6, -99), (65.6, -99), (70.8, -95.8)
and (76.4, -95.8). Native combat killed the defender and spawned
`technology_grunt` at (55.84, -86.44, 0.07). The Elite returned, used
the physical pickup and acquired the exact tag, setting `gs_collected_4`
true. The pickup replaced the active pistol and the defense actor kept
full vitality while the technology occupied the equipped slot. The other
inventory slot retained the plasma pistol. The native keyboard switch
equipped it with full battery. The firing capture from the pickup position
shows the landed Spirit in front of the defender at (59.85, -85.47, 0.10).
The inner flank left the defender at full vitality. The outer route via
(58.17, -90.69), (62.13, -90.62) and (62.45, -87.78) exposed both sides
to fire. The Elite removed the defender's shields and took damage in
return. The bounded run ended with the defender alive and
`gs_extraction_armed` false. Field evacuation and banking remain open.
Evidence: `build/native-black-echo-04d9-elite-bsp-approach` and
`build/native-black-echo-04d9-elite-defense-keyboard`.

The follow-up outer flank via (58.14, -90.79), (61.93, -90.68) and
(62.44, -87.90) reached the fight at full vitality. The defender moved
from (59.72, -85.31, 0.10) toward cover near (58.25, -85.47, 0.06);
the Elite damaged it to 72% health. One defender survived and
`gs_extraction_armed` stayed false. Evidence:
`build/native-black-echo-04d9-elite-outer-defense/result.json`.
Both faction routes still need two consenting players using independent
Delta identities and physical controls. This local run did not exchange
account proofs or reach the field call, banking or redeployment.

A follow-up offline input run cleared the facility, collected technology
and approached the defense guard from south (62.36, -88.20, 0.50) and north
(61.67, -83.46, 0.43). The guard lost its shields and retreated to
(58.44, -85.25, 0.06). It recovered to full shields at 95% health. The
field terminal remained locked. Readback of the exact Spirit root collision
mesh with native center-of-mass adjustment and the recorded parked pose
(59.04, -84.50, 1.52) intersects both tested torso-height firing lanes in
the static reconstruction. Spark should check the guard’s fallback firing
positions against the parked hull and place them in reachable cover with
a clear approach. Preserve the native guard-death and boarding gates.

The same exact cache also admits native default UNSC deployment. An
offline Spartan boarded Pelican seat 1, landed at (78.091, -124.922, 1.633),
and voluntarily disembarked at (78.549, -124.952, 0.409) at full health and
shields. The player walked into the compiled be_survivors bounds and stayed
there at (50.492, -89.956, 0.185). The living Marine remained at
(78.170, -124.909, 0.472), the facility encounter was absent, and the field
terminal was unusable through tick 6495. Compact native receipt:
build/native-black-echo-04d9-unsc-objective-readback.json.

Invader recovery from this exact cache independently confirms that gs_mission
waits exclusively on gs_spirit_player_arrived, then starts the Covenant rescue
encounter. gs_pelican_insertion_complete reports the authored troop sequence.
Spark needs a UNSC mission entry that observes the living Pelican passenger’s
grounded disembarkation, using the existing Spirit passenger check as a
reference, and drives the Marine rescue route before the field terminal is
powered. Keep the Covenant branch and native gear/boarding requirements.

The managed Pelican/Spirit call checks incapacitation before accepting a
completed request from outside the landing zone. Revival can restore that
request without a new countdown or gear write. The regression failed before
the repair and passed after it. Windows x64 and 38 focused checks passed.
The exact cache spawned a healthy offline player in the native smoke run.
Chupathingy source 9990568 has native map and vehicle paths. Forerunner
owns this map’s Extraction aircraft signals and gear settlement. A native
Chupathingy run of this exact cache is unverified.

The preceding Grunt CE609 candidate was
releases/black-echo-native-covenant-technology-grunt-20261120/be_native.map,
SHA256 d4ca73c8f3250501efc1448b81f86fa5e150e53f9833125f366977b91fd15213.
The compiled weapon forerunner/be_native/technology_grunt uses pp and the
native unit_has_weapon objective tests that same definition. Two Windows
clients joined the private session and matched the exact cache. The hub
admitted the map. Account proofs were not exchanged with profile sharing
off. Live technology pickup and authenticated banking remain unverified.

Black Echo's large HUD is present in the cooked CE609 tags. At 960x540,
the radar is about twice the width of stock CE with the same runtime.
The candidate ui/hud/cyborg shield background and health background have
scaling flags 0 and offsets (0,2) and (-10,36). The stock tag uses the
high-resolution scale flag (4) with (0,1) and (-5,18). On the assault
rifle HUD, the candidate static and meter flags are 0 at (0,42) and
(-2,40), where stock uses 4 at (0,20) and (-1,20). Match the radar panel
and blip positions together. The candidate's single-player blip anchor
is (104,103) versus (41,41) in stock. Preserve the artwork and align the
native placements and scaling flags for the intended on-screen size.
The 960x540 captures are build/black-echo-hud-before.png and
build/stock-ce-hud-reference.png.

The same cache has one native player start at (40, -110, 1.43069).
After a controlled two-client wipe, the original engine reused it for both
bodies. At tick 4020 both bodies had zero health and the same XY at Z 6.331
and 6.431. They continued rising six world units per second. Hub respawn now
waits for a free native start and preserves base health when the authored
equipment profile has two zero vitality modifiers. The exact-map retest
spawned both at full body health after the first moved clear. Spark should
place separate, grounded hub starts for all intended players, with clear
routes to the controls and passenger doors. Isolated controlled wipe evidence
is in build/native-spark-black-echo-d4ca-hub-repro/checkpoints.json and
build/native-spark-black-echo-d4ca-hub-respawn-fixed/result.json.

The hub deploy control is at (41, -110, 1.42002). The field call control is
at (71, -129, 0.78456). Check the actual control model bases against the BSP
and verify the front markers are reachable from the routes players use.
The visual terminal floor gap and the first unexpected death are unverified
in the controlled run. Normal input from either probed position produced no
host touch offer. Persistent deployment needs two bound Delta accounts, with
each participant opting into Share My Profile in Delta Profile > Privacy.

The compiled gs_mission powers extraction_activate off and sleeps on
gs_spirit_player_arrived before starting its objectives. A UNSC Pelican
insertion cannot satisfy that transition. The Pelican's existing
gs_pelican_insertion_complete tracks the authored troop disembarkation.
Use the existing Spark passenger-arrival tracking for both craft and start
each faction's objectives from its own living, previously seated passenger
after grounded disembarkation. The field controls must remain reachable
and become usable when their faction's objectives are met. Use the existing
operation and transport callbacks to rearm repeat deployment.

For reusable insertion, use native passenger seats and the existing Spark
transport controller for the Pelican or Spirit approach, landing and exit.
Opening the mission from the occupied craft can use its current passenger
view or an optional aircraft-relative camera. Support creator-supplied
briefing, radio, pilot, landing, departure and extraction cues through
native sound tags, subtitles and timed HaloScript. Missing cues and skipped
shots must leave the same aircraft, selected gear, physical arrival checks
and gameplay outcome intact. Keep each cue optional and keep camera cleanup
independent of flight. Use original or user-supplied media. Verify both
faction paths with two authenticated people using their own controls.

Spark's previous facility-cover CE609 cache is
releases/black-echo-native-covenant-facility-cover-20261118/be_native.map,
SHA256 ce892c715ccdb5eaaccb9c44a1a60b6148bc52b092244e12bc4632489a89b7bf.
The facility guard moved. The compiled technology weapon still uses hp,
which the Covenant player cannot equip with the supplied Grunt graph.
This candidate has compiled-cache readback only. The Covenant strand still needs
the compatible weapon definition and matching objective described below.
The extracted gs_mission begins by waiting on gs_spirit_player_arrived.
For a shared two-faction delivery, author a UNSC entry that follows the
Pelican insertion and live disembarkation, then use the correct faction's
rescue and objective flow.

Spark's Covenant rally and Spirit return CE609 cache is
releases/black-echo-native-covenant-rally-return-20261118/be_native.map,
SHA256 dfa84b9590929393e16a36b59a568b7ec0b1420e7cd7e2526505fb4a7a36d0b0.
Two authenticated Covenant players flew in and exited alive. The parked
Pelican stayed at (53.339, -102.537, 1.348). The empty Spirit traversed the
revised route and both clients saw it landed in spirit_return at tick
13680, near (59.049, -84.495, 1.535).

The living Covenant ally executed be_covenant_rally, reaching
(49.312, -87.728, .088) inside be_survivors. The host's arrival opened the
facility objective. Both peers saw the Armored Marine defender spawn and
fight. During the first attempt the guest was still approaching from
(54.010, -92.068, .170) when the defender's bullets and explosion killed
that player at tick 16170. The ally and host remained alive. Evidence:
build/native-spark-black-echo-dfa8-facility/result.json and both peer logs.
The paired approach then brought both living players into be_survivors at
tick 15390. Host readback confirmed the escort gate and living facility
opposition. While the guest was still turning to aim, the Marine
incapacitated and killed the guest near (56.493, -89.446, .077) at tick
15600. Both peers saw the ally survive and the same hostile actor. The
paired run is recorded in build/native-spark-black-echo-dfa8-paired-facility.
With both controllers aimed before contact, the next paired run cleared
be_cov_facility at tick 14910. Both players remained at full health. Both
peers saw the unowned technology at (55.84, -86.44, .07). The guest
reached (55.797, -86.632, .055) with no native pickup action. The item
stayed unowned and gs_collected_4 remained false.

The compiled forerunner/be_native/technology weapon uses the hp label.
The compiled Grunt standing graph accepts pr, ne, pp and fr. The
Spartan standing graph accepts hp. Native unit_can_use_weapon checks
the seat and weapon animation labels before offering pickup. For the
Covenant export, use a distinct authored technology weapon with the
animations and label of a supported weapon such as pp. Point the native
object and unit_has_weapon objective at that same definition. A
dual-faction candidate needs animations compatible with both bodies
or each faction's own correctly tagged objective. Preserve the native
eligibility and ownership checks. Defense, recall, boarding and
settlement remain open. Paired combat and pickup evidence is in
build/native-spark-black-echo-dfa8-native-mission.

Spark's pilots-hold CE609 cache is
releases/black-echo-native-both-pilots-hold-20261118/be_native.map,
SHA256 eed5856bc7d785522130b1b633a59266d1d23724ff1c7a5452a81cf180e47ef9.
The first two-account run found the old short walking line obstructed near
the parked Pelican, at (54.341, -103.839, .014). Both clients took the east
side around the hull in the next run, boarded the Spirit, flew and landed
at (94.063, -131.952, 2.170), and exited at full health. Both peers saw the
Pelican remain at (53.339, -102.537, 1.348) through the Spirit insertion.

The arrival latch spawned the Covenant ally. Both players reached
be_survivors on foot at (50.746, -89.466, .152) and (50.361, -89.568, .153).
Host readback showed one living Covenant ally, players inside the rally,
the ally outside, and zero facility defenders. The ally stayed in native
Guard with no movement destination at (58.811, -89.541, .046).

The compiled be_covenant encounter has a single native_squad, eight zero
firing-group masks, and one firing point near its spawn at
(58.846, -89.472, .078). ai_follow_target_players selects players for
native squad migration in encounter_update_follow. With these masks it
has no destination. Author a Covenant return squad with compiler-derived
ground firing positions inside be_survivors and native guard/attack
groups, following the existing be_marines/be_return pattern. Alternatively
attach an authored ground command list that physically leads the ally
into the rally. Retain the living-ally and player-volume checks.
Facility combat, collection and evacuation remain open. Evidence is in
build/native-spark-black-echo-eed5-east-corridor, including both peer logs
and the host console readback.

Spark's previous Spirit occupancy CE609 cache is
releases/black-echo-native-covenant-spirit-occupancy-20261118/be_native.map,
SHA256 7b3940f61ee960a158ee2281219a92cead6709a90f9b36b6c11bd950a1f0ba9d.
The extracted script now checks player bodies in the actual seat labels.
Two authenticated Windows clients selected Covenant, deployed as Grunts
with plasma pistols, and physically boarded seats 5 and 6. Both saw the
Spirit fly and land in spirit_zone at (88.563, -130.765, 2.070).

The host used the native exit from seat 5. At tick 11370 they were outside
at (85.221, -132.173, 3.549) with full health. At tick 11400 both clients
reported zero health near (85.056, -133.194, .396). The guest stayed in
seat 6. The Pelican had left its berth around tick 8100 without a logged
native flight command. At tick 11400 it was near the exit corridor at
(85.993, -128.701, 2.153), with its pilot and Marine still seated. The
trace does not identify the lethal damage source.

Spark should suspend the staged Pelican pilot using the same native
ai_braindead pattern already used by the Spirit, wake it for its own
departure or recall order, and keep its hull, approach and parking area
clear of both Spirit exits. Verify the route with both Grunt passengers
disembarking alive using native seats and collision. Retain the revised
player-seat checks. The failed exit is recorded in
build/native-spark-black-echo-7b39-west-approach. The initial automated walk
stopped before boarding at (55.703, -103.951, .003). The west-side route
cleared that approach.

A targeted replay on the same SHA enabled native player damage logging.
Both clients saw the Spirit land at (94.684, -131.898, 2.180). Both exited
with full health at (91.388, -133.917, .731) and (91.441, -132.490, .803).
gs_spirit_player_arrived became true, spirit_home began, and be_covenant
spawned one living actor. At tick 11130 both peers saw that actor alive near
(58.827, -89.509, .046), with the host on foot at (68.668, -117.017, .558).
The automated walking route stopped there before the rescue rally. The
compiled terrain block matches the earlier 0541 rally export byte for
byte. Its successful native footpath used (69.346, -120.815, .028),
(68.098, -117.549, .320), and (66.152, -114.892, 1.637) through that slope.
No damage was logged, so this replay does not identify the earlier lethal
effect.
Both live logs and the capture are in
build/native-spark-black-echo-7b39-exit-damage. Rescue completion, facility
combat, collection and evacuation remain open on this export.

Spark's elevated Spirit route CE609 cache is
releases/black-echo-native-covenant-spirit-flight-20261118/be_native.map, SHA256
8e8d33127c1ca3936e9f0c907f894884ccec3f8ba4d843ae3e55bb2d180e3121.
Two authenticated Windows clients selected Covenant, deployed, physically
boarded, and observed the native Spirit climb and fly the revised route.
The host reported the native landed signal inside spirit_zone at
(94.026, -131.955, 2.170). Both passengers physically exited alive at
(90.871, -134.083, .750) and (90.806, -132.688, .812).

The script never reached its passenger-arrival latch or spirit_home order.
At tick 9270 both players were on foot inside spirit_zone, the hull was
landed, the pilot occupied seat 0, and the built-in turret occupied seat 9.
The retained actor handle e2770008 resolves through its definition
e2ff018b to vehicles/c_dropship/cd_gun/cd_gun.biped. Native vehicle_riders
includes every seated unit, so list_count stays above the authored 1
after all passengers leave. Filter out installed vehicle components
when checking occupancy, for example by intersecting the current riders
with the current player bodies. Keep the existing live pilot, native
landing and actual former-passenger checks. Review the recall and return
waits that also compare the raw Spirit rider count to 1. Covenant rescue,
combat, extraction and settlement remain open. Evidence is in
build/native-spark-black-echo-8e8d-spirit-flight.

Spark's pilot-hold CE609 cache is
releases/black-echo-native-covenant-pilot-hold-20261118/be_native.map, SHA256
88ff10400e68b6e6773bdcf20f90f7228c0392c2d591d0721190e79e07a0091d.
Forerunner fe0a2a4f5c keeps a suspended native pilot seated while the
aircraft settles under gravity. Both authenticated Windows clients deployed
as Grunts with plasma pistols and saw the Spirit hold (57.281, -87.291, 1.415)
through the foot approach. They physically boarded native seats 5 and 6 on
both peers. The host confirmed the live pilot, crew-ready group and Spirit
return pad.

The resumed native pilot rejected both spirit_field movement atoms from
(57.281, -87.291, 1.416). The compiled BSP ray to p0 (65, -105, 1.825)
meets surface 2868 at (61.185, -96.247, 1.623). The direct ray to p1
(94, -128, 1.228) meets surface 2827 at (62.436, -93.006, 1.390).
Spark's newer grounded-pilot cache 77682fd39675886ae3efefaaf5744538cc7805a386f11f8fb8e5cce9ac593a5c
keeps these same points and intersections. Author a clear climb from
the parked berth, an elevated transit leg and a separate landing approach.
Check the hull clearance on the return legs too. Preserve the actual
boarding, landing and settlement gates. Spirit flight, field disembarkation
and Covenant mission completion remain open. Two-account receipt and
command trace are in build/native-spark-black-echo-88ff-boarding.

Spark's troop-standby CE609 cache is
releases/black-echo-native-troop-standby-20261118/be_native.map, SHA256
a5bf668050e6470e750dc20b75b1ccfe458c52de1b9d8b50782f792a4457f9f2.
It uses native AI suspension on the Pelican Marine before seating, then
resumes that same living Marine on his first field exit. The previous build
loaded it with two authenticated clients and confirmed both pilots and the
Marine in their native seats. The Covenant host had no native deploy prompt
at (40.620, -108.908, 1.273). The rebuilt runtime on the exact same cache
then verified both authenticated players deployed as Grunts with their own
attached plasma pistols. Both stayed at full health through the approach.
The guest stopped at (50.839, -102.842, .047), before a boarding attempt.
The landed Spirit was already outside both its authored pad and the shared
hub when both players deployed. At tick 8940 it was at (49.170, -71.942,
1.590) with its pilot seated, alert and at zero throttle.
Evidence from build/native-spark-black-echo-a5bf-spirit-return.

Forerunner now preserves the native rest state of a landed, idle,
AI-flown plane while its driver remains powered. Flight orders and
collision wake-ups retain the normal path. Windows64 built and 117
focused checks passed. On the exact a5bf cache, two authenticated
clients reported the same parked Spirit origin (57.335, -87.223, 1.446)
from tick 300 through 6270, after both players had deployed. At the
approach the host read the Spirit landed inside both return volumes.
The pair reached (55.451, -89.254, .111) and (56.196, -88.868, .087)
via the east side of the Pelican apron.

The parked Spirit still held at tick 7560. The Pelican pilot entered
native Charge at tick 7530, and the Spirit pilot entered native Fight at
tick 7620. The Spirit then moved while still reporting landed. The host
came within 1.131 world units of the nearest available passenger marker
and received no native boarding prompt. Both players remained alive and
neither boarded. Keep both standby pilots out of autonomous combat during
the shared approach and leave clearance to the actual passenger entrance
near (54.2, -87.7, .64). Use the native authored crew controls and preserve
the flight and passenger checks. The separate Covenant mission entry,
Spirit flight and settlement remain open. Evidence from
build/native-spark-black-echo-a5bf-parked-rest.

A separate two-account exterior approach reached the west side of the
authored berth at (53.425, -89.000, .169) and (53.516, -88.287, .116).
Both clients had verified a stationary Spirit during deployment.
The Pelican pilot entered Charge at tick 4530 and the Spirit pilot entered
Charge at tick 4620. By the exterior approach the Spirit had landed at
(55.899, -99.385, 1.333), inside the shared return volume and outside its
authored spirit_return. The host had no eligible seat marker. Test
passenger clearance after both standby pilots stay in their authored
berths through the walk. Evidence from
build/native-spark-black-echo-a5bf-exterior-seat.

The optional-Flood predecessor 70641e813f97e4e8bf741489425ebe3bcd41b5053e3c3adcf8acf3f0f0bcecae
puts be_containment east of the technology and records visits during the
ownership poll. Both native two-account attempts boarded, flew, landed,
disembarked and reached the living Marine rally. Foot movement stopped
before Covenant clearance at (52.089, -88.013, .092) on the direct approach
and (51.521, -90.375, .238) on the southern approach. A native screenshot
records a solid face on the southern approach. The exact obstacle is open.
Spark's compiled rays for the direct approach cleared the BSP and its
approximate parked Spirit hull. The Spirit pilot entered native Charge
while the UNSC players flew and later died. The grounded Spirit settled at
(51.076, -84.398, 1.476) in the first run and (51.770, -86.585, 1.521) in
the second. Review the Spirit standby route and park away from both foot
approaches. The optional branch, defense and settlement remain unverified.
Evidence from build/native-spark-black-echo-7064-direct and
build/native-spark-black-echo-7064-southern-route.

Spark's Covenant-body predecessor is
releases/black-echo-native-covenant-body-20261118/be_native.map, SHA256
2c8990af61f091ab524413cdc0ae41b0c1c5f2861955836ba96a9c8ba06569ec.
Two authenticated Windows clients selected Covenant at the native control
and deployed. Both peers read the actual Grunt bodies and each player's
plasma pistol attached to that body. The Spirit pilot occupied its native
driver seat. The Spirit reported ready, landed and inside spirit_return
when the pair began walking. The first player died near (49.432, -104.133,
.094) by tick 4650 and the second near (48.298, -103.584, .216) by tick 4980.
Both clients saw the deaths and the host reported both runs lost. The damage
source was not captured. Neither player reached a Spirit seat, and the
landed Spirit drifted to (52.684, -76.772, 1.562) by tick 5280.

Review the foot approach past the Pelican apron at those casualty coordinates
and the Spirit's touchdown surface. Keep armed standby crews out of combat
along shared hub approaches while preserving normal field allegiance. Author
a Covenant objective entry after Spirit field touchdown and disembarkation.
This cache's gs_mission waits on gs_pelican_insertion_complete. Preserve
the existing UNSC objective gate. Evidence from build/native-spark-black-echo-2c89-spirit-pair and peer logs.

On the same export, a UNSC run boarded the first player before the alert
Pelican pilot's facing reversed at tick 1530. The pilot had zero throttle
and no route. The hull rolled and settled by (56.866, -100.654, 1.491).
The guest stayed outside the second seat's native interaction range.
Forerunner now applies the parked hull heading after native AI look orders.
Windows64 built and 119 focused actor and vehicle checks passed. The exact
map on the rebuilt executable held its heading through tick 1530, when
the previous build turned. At tick 2070 both peers saw both players in
native seats 4 and 1. The Pelican took off, landed (78.656, -125.629, 2.011)
inside the field volume, and both players disembarked.

The host reached the authored rally with a living Marine. Both clients saw
the cleared Covenant encounter and the technology attached to the same
player by tick 6990. The Flood spawned, survived the attempted player fire,
and the collector died before the defense and extraction gates opened.
Physical evacuation, settlement and redeployment remain open on this map.
Evidence from build/native-spark-black-echo-2c89-flood-defense and
build/native-spark-black-echo-2c89-parked-look-override.

Spark's primary CE609 rally cache is
releases/black-echo-native-rescue-rally-20261118/be_native.map, SHA256
0541509920a71bbd753fd279e6650542d967fc70628b3b2b9ebb415b2e76bede.
Its rescue script waits for a player to share be_survivors with the living
Marine before the Covenant encounter. The first authenticated run boarded
both players. The Pelican's idle pilot look reversed before its hull did,
with zero throttle and the Spirit still far away. The Pelican rolled and
both pilot and Marine left their seats at tick 1320. The authored departure
gate stayed closed. Evidence from build/native-spark-black-echo-0541-rally-operation.
Forerunner now uses the hull heading when a grounded alert pilot has no route.
Windows64 and 30 focused checks passed. On the repaired build, the exact same
candidate held its heading through tick 1320 with both crew members seated.
Both authenticated players boarded, flew, landed at (78.670, -125.638, 2.008),
and exited normally. Both clients still saw the Marine alive at the rally at
tick 6000. The collector stopped before entering the authored volume.
Evidence from build/native-spark-black-echo-0541-parked-look and peer logs.
A follow-up on the corrected build reached the rally with the host at
(49.830, -88.968, .123), rescued the living Marine, cleared the Covenant
encounter and picked up the technology at tick 7410. Both peers reported the
same weapon attached to the collector. The authored pickup gate advanced,
then spawned the optional Flood encounter. The collector died during that
fight. Defense, physical evacuation, banking and redeployment remain open.
Evidence from build/native-spark-black-echo-0541-native-rally and peer logs.

Spark's alternate CE609 cache is
releases/black-echo-native-field-alignment-20261118/be_native.map, SHA256
539f6fa01a219ded36a412551248c3948fe11f1fa5fe239ad020d270c28c6303.
Both authenticated players boarded and took off. The pilot stayed on the
new airborne go-to-and-face waypoint (76, -121.75, 6.2). Its closest sample
was 1.780 units away while airborne. The touchdown command and landing volume
remain open. The final observed position
was (79.245, -118.817, 9.992). Command status 3 means an active native
command. Evidence from build/native-spark-black-echo-539f-paired-approach.

## Prior Covenant terminal candidate

Spark's revised Covenant terminal is in CE609 cache
releases/black-echo-native-covenant-terminal-20261118/be_native.map, SHA256
833f708130008508caab07fe2c6dd899a28b96e328c60039282d9c0e3a5a3648.
Both authenticated players used the revised control at (38.003, -104.528, .999),
deployed, and saw Covenant allegiance and plasma pistols on both clients.
The current cache uses cyborg bodies. Author a nonautomatic
extraction_covenant_body placement to exercise Covenant biped selection.
The Spirit reported ready, landed and inside both hub volumes at deployment.
The walk stopped at (46.371, -101.147, .774) before boarding. The Spirit
had settled near (55.565, -80.870, 1.550). Evidence:
build/native-spark-black-echo-833f-spirit-pair/result.json and both peer logs.

On the same export, two authenticated UNSC players boarded, flew, landed
and disembarked. In the settlement run the host read one living Marine in
be_survivors and zero remaining Covenant. Both clients saw the technology at
(55.84, -86.44, .07) by tick 6570, and both saw the living Marine beside the
rescue area. At (56.165, -86.769, .057) the collector had no native pickup
action and the item stayed unowned. The extracted weapon and cyborg bounds
read .10 and .45 world units. The older accepted cache offered a normal weapon
swap from the closer (56.003, -86.664, .052) approach. Pickup, evacuation,
banking and redeployment remain open. The authored gs_mission waits at
gs_pelican_insertion_complete. The Spirit path needs its own authored
objective entry for Covenant operation acceptance. Evidence:
build/native-spark-black-echo-833f-settlement/result.json and both peer logs.

One later insertion marked its cleared path as arrived at
(77.023, -115.954, 1.856), more than eight units from (79, -124, 1.85)
and outside the field volume. HaloScript command status 3 was still active.
Forerunner now checks the authored arrival radius for a configured aircraft's
stop before reporting completion after a cleared path. Windows64 and 28
focused checks passed. On the corrected build both clients landed and exited,
and the host confirmed the living rescue and Covenant clearance. The native
replay did not encounter the early-clear branch. The collector stopped at
(51.322, -87.272, .080) while the technology remained unowned at its authored
position. Evidence in build/native-spark-black-echo-833f-physical-pickup
and build/native-spark-black-echo-833f-arrival-fixed.

Previous Black Echo handoff: Spark's CE609 apron cache
releases/black-echo-native-apron-20261118/be_native.map, SHA256
7074cffcd9b637912c0f0b947fe83d0ee40b41fdb7a43393b6a3bf10249d2714.
In the first paired approach with a seat marker, the Pelican turned sharply
around tick 960 while its pilot had zero throttle. The Spirit's pilot began
native Charge at tick 1110 and later reached (53.214, -101.711, 1.391).
The guest saw a seat prompt at tick 4860 while the host reported the guest alive
below the hub volume. Departure then locked the seat before the guest's loss.
Keep the standby Spirit crew out of autonomous combat until its authored route.
Review dynamic clearance at the apron with both players approaching slowly.

Forerunner now keeps deployed teammates in the hub crew until field arrival
or loss, even across the return volume edge. The existing regression reproduces
the early departure on the old code. Windows64 and 73 focused checks passed.
On the fixed build, two authenticated clients physically entered seats 4 and 1,
flew to (78.746, -125.713, 1.991), and exited normally. Both clients saw the
living Marine, Covenant combat and death, and the technology spawn at tick
6660. The empty Pelican landed near the apron at (53.233, -100.705, 1.520).
The guest's input route stopped at (53.021, -92.560, .203); the live run ended
before technology collection. Continue with one collector and one player
near the field call point. Mission settlement and redeployment remain open.
Evidence: build/native-spark-black-echo-7074-apron-approach/result.json and
build/native-spark-black-echo-7074-crew-gate/result.json with peer logs.

Earlier Black Echo handoff: Spark's CE609 cache
releases/black-echo-native-escort-20261118/be_native.map, SHA256
51ba08d64c12c2ea27f0eeb3e79b5b75db2ff5710c37443547251e5c82dcfa0b.
On the existing Windows build, two authenticated clients boarded, flew, landed
at (77.706, -125.025, 1.878), and exited on foot. The living Marine followed the
new native be_rescue_route into be_survivors. Both peers saw the live Covenant
actor at tick 4260, its subsequent death and the technology pickup at
(55.84, -86.44, .07) from tick 4590. The late observer timed out waiting for
a live Covenant actor after the encounter had cleared. The host's true rescue
volume and living-Marine readback, the actor history and the authored pickup
spawn establish progression through the Covenant objective. Pickup remains open.
Evidence: build/native-spark-black-echo-51ba-marine-escort/result.json,
client-0.log, client-1.log and mission-console.txt.

A second run exposed intermittent staging interference. The Spirit pilot entered
native Charge at tick 930 before its authored deployment and left its pad.
At Pelican boarding the Spirit rested at (50.159, -103.640, 1.427), next to the
Pelican at (50.016, -104.082, 1.536). The Pelican's first leg stopped short.
Native navigation then rejected field points 1 and 3 from
(50.323, -101.451, 2.003). Both players remained seated. Spark next should
keep staged pilots and armed crew out of autonomous combat while awaiting
their script command, and verify pad separation through a slow two-player
boarding sequence. Retain native command-list, collision, boarding and
landing rules. Technology pickup, defense, evacuation, settlement and
redeployment are still open on this export.
Evidence: build/native-spark-black-echo-51ba-technology/result.json,
client-0.log and debug-0.txt.

Preceding Black Echo handoff: Spark's CE609 cache
releases/black-echo-native-pelican-route-20261118/be_native.map, SHA256
0ec05e4139e4f9a910eaee0527c6c25574ab363ee21b0aa4e8f49acb62d8bfd6.
Two authenticated Windows clients loaded the exact cache, deployed as UNSC,
boarded in native seats 4 and 1, and saw the Pelican lift off near
(49.871, -104.219, 1.558). The original executable orbited the intermediate
(72, -118, 6.2) waypoint at a minimum throttle of .325. The existing plane
controller now separates minimum turn thrust from aligned cruise in
source/ai/actor_moving.c. Windows64 and 27 focused actor checks passed.
The turn fix reached the final go-to-and-face command at (79, -124, 1.85),
radius 1.8, but the pilot kept rolling outside the stop. Forerunner now applies
native low lift and level steering on the close, grounded approach to a stop.
The authored radius, native collision and physical landed check are retained.
Two subsequent runs of the unchanged cache reached the field volume with
landed and gs_pelican_insertion_complete true, at (77.729, -125.122, 1.870)
and (80.789, -124.342, 1.884). The named Marine left the aircraft alive.
The host and guest each used normal exit input, and both replicas showed
both players alive on foot. A later paired run completed the empty hub_route. At tick 8250 both
clients reported landed 1 and the Pelican at (53.318, -106.21, 1.678), inside
the compiled extraction_return bounds. Both players walked into be_survivors
but the living Marine remained at (77.790, -124.739, .545) after disembarking.
The host reported be_return squad, Alert, no active path and no Covenant spawn.

The 0ec0 rescue handoff called for a ground guard state and an authored
sequence of compiler-resolved ground firing positions from the field toward
be_survivors. The compiled be_marines encounter has only two firing points,
(54.846, -85.472, .078) and (50.846, -86.472, .078), both near the rescue end.
Verify a reachable first guard position from the actual disembark point and
intermediate migration groups along the walked corridor around
(70.97, -122.72), (64.56, -113.66), (58.98, -102.94) and (51.79, -93.42).
Use native collision and keep the existing living-Marine rescue gate.
Rescue, combat progression, evacuation, settlement and same-session
redeployment remain unverified on this export.
Evidence: build/native-spark-black-echo-0ec0-paired-disembark/result.json and
build/native-spark-black-echo-0ec0-marine-escort/result.json, client-0.log and
client-1.log in the latter.

The Covenant terminal is powered at (37.997, -105.072, 1.105), radius .350.
At the tested front approach (38.140, -104.482, .784), the engine still
reported no Use target before faction admission. Check the front marker,
reachable standing area and native interaction bounds in the next export.
The Spirit pilot and parked landed state passed startup readback. Paired
Covenant deployment and Spirit flight remain open. Evidence:
build/native-spark-black-echo-0ec0-spirit-native-use/result.json and client-0.log.

Preceding Black Echo handoff: Spark's CE609 build e0264213-06ba-499a-a70e-0e29fa8a752d
is releases/black-echo-native-pilot-landing-20261117/be_native.map, SHA256
dee2d9a79a6e9ca3cb13828852c340896d6ceba7985f798f6e97a9c5efb8dc94.
Both authenticated clients loaded the exact cache. The new spirit_driver
occupied c_dropship driver and the Spirit registered landed at
(57.362, -87.189, 1.441), up.k .987. The Pelican pilot and Marine occupied
P-driver and P-riderLM.

On maps with an explicit UNSC faction control, Forerunner now assigns the
native Human team to a confirmed default UNSC deployment, matching explicit
UNSC selection. Windows64 and 131 focused checks passed. Both authenticated
clients agreed on both players' Human allegiance, and both players stayed
at full health in the hub.

Paired boarding is now verified on dee2. With the aircraft displaced from
its authored pad, the guest used the southern ground approach around
y = -114 to reach P-riderLF, while the host took P-riderRF. Both peers agreed
on the same Pelican and seats 1 and 4 for 49 samples from ticks 2130 to 3570.
That run did not reach the field.
Evidence: build/native-spark-black-echo-dee2-south-approach/result.json.

The follow-up paired run verified gs_pelican_insertion_started, the live
P-driver and P-riderLM occupants, exact Marine identity, the hub volume and the
native landed signal. gs_pelican_insertion_complete stayed false. The Pelican
remained grounded near (54.032, -112.004, 1.913). The pilot stayed Alert
with no movement destination and zero throttle. The native command trace confirms that gs_pelican_trip
issued field_route to pilot/source_squad after both passengers boarded.
The radius commands ran. Native navigation rejected both go-to atoms from
(54.075, -111.809, 1.900): waypoint 0 (66, -121, .123) and waypoint 1
(76, -128, .587). The list then had no active command. Author a stable parked
pose and a short, clear initial climb from that pose before the low field
waypoints. Validate each leg against the compiled BSP and the aircraft
footprint, and retain the existing native boarding and landing gates.
Keep both normal boarding approaches clear. Flight, rescue, combat and
settlement remain unverified on dee2.
Evidence: build/native-spark-black-echo-dee2-command-atoms/result.json and
debug-0.txt in the same folder.

Previous Black Echo crew handoff: Spark's CE609 build b3c5bc47-159c-4b05-a109-79197fbe232b
is releases/black-echo-native-crew-20261117/be_native.map, SHA256
fdc6ec89f9aac3e4068c3cbeee55fa391514988995bf2e70765425b63731ea39.
Two verified Delta accounts loaded this exact map. Native seat queries confirm
the Pelican pilot at P-driver, Marine at P-riderLM, and no Marine at P-riderLF.
The Spirit pilot is in c_dropship driver. Its hub touchdown at
(64.624, -87.927, 5.729) registered landed with up.k .982. These are actual
startup results for fdc6 and supersede the 7ec2 crew and landing findings below.

Both players deployed on foot. Before boarding, the Pelican pilot entered
native Fight and the Pelican moved from (54.423, -107.867, 1.858) at tick 1350
to (42.915, -111.107, 3.138) at tick 1500. The guest was incapacitated near
(45.427, -110.080, 1.108), and neither player flew. Damage attribution remains
unconfirmed. In the shared hub, keep the pilots and armed crew out of combat
and the craft stationary at their authored pads through crew preparation and
boarding. Use the existing HaloScript AI controls and retain the native
boarded/landed gates before issuing the route command lists. Verify both front
Pelican seats with live passengers before measuring the Marine's route from
the LZ into be_survivors (45.846, -90.472, -1.922), dimensions (7, 7, 6).
The released source enables both migration flags. Their gameplay outcome,
Spirit flight, combat, extraction and banking are still open.
Evidence: build/native-spark-black-echo-crew-fdc6-shared/result.json.

17 November grounded Spirit handoff: Spark's exact CE609 map is
releases/black-echo-spirit-routing-20261117/grounded/be_native.map, build
3339d69c-f68b-4789-acad-bc4adcfdbc1d, SHA256
7ec2a25af96ce53db421b875458b3b256166ad53ba7aa34f54f4302ca4ee1cb7.
Two authenticated Windows clients matched the cache, spawned with equipment
and admitted both transports. The Pelican produced a native landed signal.
The Spirit stayed grounded at (64.818, -87.908, 5.082) with up.k .918.
Author a level, clear hub touchdown surface and route inside spirit_return
(55, -94.472, -1) to (75, -80.472, 7) to reach up.k >= .95.

The Spirit's native driver seat is c_dropship driver. The spawned Grunt at
tag index 0x849 uses the graph
gs_assets_dbc466fb114a4fa7b92243697f64b4d4/characters/grunt/grunt.model_animations,
which lacks that driver label in the compiled cache. It stayed on foot from
tick 30 through 5700. Seat 9 belongs to cd_gun, the built-in turret. Bind the
actual encounter actor to a graph with the driver's animation and usable
weapon-class/type entries. Confirm the actual pilot remains seated and keep
it from engaging UNSC players while the hub crew is preparing. The host took
damage and died in the hub in this run. Exact damage source is unconfirmed.
The Pelican Marine still uses P-riderLF and both rescue migration flags
remain zero. Apply the existing P-riderLM and rescue requirements below.
Faction selection, physical insertion/evacuation, objectives and banking
remain unverified on this export. Evidence:
build/native-spark-black-echo-grounded-authenticated/result.json.

17 November actual revised Black Echo handoff: retain the delivered cache
releases/black-echo-review-20261117/Spark-d9ba1102-014a-4223-804c-e06bf7000ac3/be_native.map,
SHA256 c388d3d69219ba5b99f53729905b81d9baa43a57582eb016cd1c636f2c636649
(110,033,988B), unchanged. The revised Pelican rests inside its hub gate at
(49.031,-109.844,1.754), up .952, native landed 1. Older .942-attitude results
describe the previous candidate and are not a current landing failure.

Forerunner source/hs/hs_runtime.c::{hs_ce_unit_vehicle_cast,hs_can_cast,hs_cast}
repairs its rejection of 24 CE-compiled (unit aircraft) expressions used as
vehicles. The real pilot/trip scripts now survive load. Wrong/stale live object
types still return none; no general type-check bypass. In the existing private
launch profile, set network.coop_enemies_mode = "none" for this fixed-count
mission. Default per-player scaling created two pilots while the authored
startup correctly waits for exactly one. Forerunner's native/Extraction menu
paths now apply that existing setting through gametype_option_setting_write;
ordinary co-op menus retain optional scaling. Retain native encounter control
with network.firefight_encounter=false.
Spark must pass the no-extra-enemies setting on its own launch path. The
in-game Spark Missions > Host Extraction path starts this unchanged map through
the native map, difficulty, setup and lobby screens. Its hub admission is
verified in build/native-forerunner-spark-black-echo-launch/result.json.
The runtime now latches native script actions for game.extraction=true even
when the saved legacy co-op adaptation setting is true. Older builds still
need the explicit network.coop_script_adaptations=false setting.

build/native-spark-black-echo-authored-crew/result.json PASSES two actual Delta
accounts: one pilot and one Marine, native p-driver/p-riderlf occupancy and
gs_pelican_ready all true. Windows64 binary SHA256
e4de3ea12160e156745b2e4f8b18815dfb391f72b20845c1fdd72711cb0179b6.
This is crew readiness, not complete mission acceptance. Retained full-operation
attempts stop during input-driven faction selection. Do not infer a terminal or
collision defect from that route; human boarding, flight, authored objectives,
combat/audio, physical escape, banking and redeployment still need proof on
this exact candidate. The later native readback below identifies two authoring gaps.

Black Echo c388d3d6 needs two authored changes before two-player acceptance.
The native shared-flight run got both players deployed, then player 1 boarded
seat 4 (P-riderRF) at tick 6150. The host reported no eligible seat afterward.
The pilot occupies P-driver; the Marine occupies P-riderLF. The shipped Spartan
animation graph characters/cyborg/cyborg supports only P-riderLF and P-riderRF
among the Pelican's passenger labels (entry/exit indices 18/19 and 21/22).
Move the Marine's scripted load and its matching seat tests in gs_pelican_pilot
and gs_pelican_trip to P-riderLM. The Pelican has the passenger left middle
marker and the shipped Marine graph has matching entry/exit clips 16/17.
Keep both player seats open and retain the actual boarding/landing checks.

For rescue, enable automatic migration on both be_marines/native_squad and
be_marines/be_return. Both flags are zero in the delivered cache. The native
encounter_update_follow only considers squads with that flag. Retain valid
firing groups and check the ground route from the native field touchdown
around (73.760,-125.796,2.178) to be_survivors. Verify the Marine reaches the
rescue area by normal movement. The previous one-passenger flight delivered
and disembarked the living Marine; it remained at the LZ. Runtime escort
selection now excludes opposing players in Extraction, using existing
allegiance. The runtime still uses the cache's authored squad flags.

Native shared-flight evidence: build/native-spark-black-echo-shared-flight/result.json.
298 seconds, 596 synchronized player observations, zero divergences.
Two-passenger departure and the full mission did not pass on this cache.
The newer Spark export 530f12c0-2a02-4ef4-abc5-bce979c0526d (CE609,
SHA256 09bb4034f0bdc40f26b2c9977ae08aec7a0dba18c1fcf5c0e6bf743cc19ae4d4)
revises the seat-test expression types. Readback still finds the Marine
loading into P-riderLF and both rescue squad migration flags equal to zero.
The same two authoring changes apply to that export.

Spark's subsequent CE609 Spirit pilot export, build 09b19c08-47a9-4670-aebc-e70179c1c32a,
SHA256 ef98e780c041a95c7014f60a4bc68dcb86fefe0ef073e14a2768a174013004b2,
loads on two authenticated Windows clients (2,887 tags, 10 scripts and 15 globals).
The test observer stopped on a log-location mismatch before native crew queries.
Direct scenario readback still finds P-riderLF and no P-riderLM script reference.
Both be_marines squad migration flags remain zero. The new Spirit pilot is a
deferred encounter. The previous seat and rescue requirements still apply.
Evidence: build/native-spark-black-echo-spirit-crew/result.json and
build/native-black-echo-spirit-rescue-flags/result.json.

17 November startup repair, Forerunner `21603426c4`: the Windows64 binary
SHA256 `6744161c52be41a0c0d73cc8d83bba7245bc869ee8eaef00d37e2a8044e1cc09`
passes fresh native profile creation followed by `HALO_START_MAP=be_native@ce`
on Spark's unchanged `31505afb…3ebab` CE609 candidate. The preceding
`b428588d…e1a9` binary reproduces `DeLa`/`schi`, offending datum `e28f0119`,
in `process_ui_widgets` (the stale index resolves to
`vehicles\pelican\shaders\lights flashing` after replacement):
closing the saving-profile screen opened difficulty selection on old-map tags.
`source/interface/ui_widget.c::{ui_widgets_close_all,ui_widget_delete}` now
retains deletion cleanup without opening successors during close-all. Ten focused
checks and the native rendered-hub repeat pass; full Black Echo/other targets
remain unaccepted. Exact evidence: `build/native-campaign-profile-crash/result.json`.
Spark next: review this executable through its existing registration/identity
checks before launching it; do not bypass those checks or reuse the old frozen
binary for this startup regression. No schema, capability or network change.

Windows64 integration milestone: the enabled OpenXR/bgfx Vulkan executable compiles
and passes the existing contract-1 reference discovery client. A private native
Vulkan/x86-peer comparison and missing-loader desktop fallback pass; headset
behavior remains unverified. `source/hs/hs.c::main_set_map_name_evaluate` now decodes
the native string address on x64 using the same `xbox_pointer` boundary as other
string evaluators. Existing script/console ownership remains unchanged. No schema,
editing operation, native-map authoring requirement or protocol version changed.
Spark can consume the x64 discovery response; this is not VR/editor-map acceptance.
Exact source/binary identity and sparse-sample rejection are in `TEST_EVIDENCE.md`.

Historical native gameplay checkpoint, 15 November: two real Delta accounts complete
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
| `network` | Compiled advertised version 45 and native layout 44; preserve the accepted 44/45 bridge |
| `operations` | Currently only `runtime.discover` |
| `native_features` | Optional native desktop CE feature identifiers. These include `extraction.squad_objectives.v1` and `character.biped_hands.v1`. Older or unsupported builds omit features they lack. |
| `map_targets` | Optional native build-target list with `cache_version`, `tag_space_bytes`, `invader_engine`, `default` and, for expanded MCC13, `required_feature`. The MCC13 target can report `max_player_starts`, `max_netgame_equipment` and `max_netgame_flags` from the native loader. |
| `process_entrypoints` | Inherited CLI entrypoints listed below; not live-edit RPC operations |
| `startup_map` | Optional desktop CE startup metadata: setting/environment, `basename@ce`, cache version 609, solo scenario, native-menu profile creation, fresh start and no network hosting. Windows64 fresh-profile/native-hub startup passes on the exact binary above; other targets remain unverified. Older and non-desktop/server builds may omit it. Not a new operation or loaded-map acknowledgement. |
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
Maps requiring squad objective gates must also use
`--require-feature extraction.squad_objectives.v1` (or require that exact string
in `native_features` in Spark's existing discovery client). Missing fields mean
unsupported; the actual preceding executable is refused. Contract version and
RPC operations are unchanged. Version-1 clients may ignore the additive field.
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
| `game.start_map` / `HALO_START_MAP` | One-shot desktop selection of an installed `basename@ce` solo cache through the normal campaign path and current difficulty. With no engine player profile, retains the validated request while the user creates one in native menus, then starts in the same process. Windows64 native startup passes on the exact repaired binary above. Skips loading an older saved checkpoint; ordinary human input and native HaloScript remain. Not hosting, a live RPC or JSON import. |
| `network.firefight_encounter` / `HALO_NET_FIREFIGHT_ENCOUNTER` | Opt-in host setting. Unverified menu source exposes OFF/ON under Server Setup → Co-op Options → Firefight. Leave OFF for authored wave controllers. New source also yields when compiled `gs_firefight`, `gs_ff_reset` or `gs_ff_bootstrap` is present; that prevents a second finite director without pausing general HaloScript. Recognition is not validation, generic controller discovery or native co-op acceptance. Historical B40 evidence predates this repair. |
| `network.firefight_lives` / `HALO_NET_FIREFIGHT_LIVES` | Host source, 0–99, frozen at map start. Deaths spend available lives for player-bound respawns; failed native spawns retain grants and departed/reused player slots cannot transfer them. `coop_enemies.c::firefight_check_defeat` now holds progression for incapacitated bodies or pending paid respawns; exhausted actual deaths receive a fresh three-second defeat grace. Focused real-source ownership/last-life checks pass; native paid-respawn acceptance remains open. Two authenticated native clients verify the all-downed hold, recovery, victory, durable gear and lobby return; exact bounded evidence is in the current master checkpoint. A full world revert still restarts wave one, score and initial lives; not saved mid-wave continuation. |
| `network.firefight_resupply` / `HALO_NET_FIREFIGHT_RESUPPLY` | Optional built-in-director host setting, off by default and frozen at map start. Uses native inventory admission for readied and owned stowed weapon reserves/battery charge before each continuing wave. Keeps native magazine/reload state and equipment; refuses stale ownership and pending deaths. Windows64 and focused checks pass; bounded native combat/refill evidence and remaining acceptance limits are in the current master checkpoint. Does not enable Spark's native HaloScript resupply option or import JSON. |
| `network.firefight_actor`, `network.firefight_spawn_encounter` | New unverified host source: exact loaded actor-variant path and optional named native encounter's squad start in the current BSP. Retains faction/swarm attachment and final-death credit. Three waves of two initial bodies plus native carrier children, bounded to 32 tracked bodies per wave on a co-op scenario; not Spark's authored rules. |
| Spark `FirefightConfig` schema `1` | Authoring/preview only. **Forerunner has no importer for this JSON.** |
| `game.solo_skulls` / `HALO_SOLO_SKULLS` | New unverified, empty-by-default source reuses the existing 17 implemented skull hooks for local native solo content, including CE. Applied at map start/restart; cleared on disposal. Unsupported names warn and are skipped. No automatic mapping from Spark rules, network support, scoring multiplier or new discovery operation. MCC retains its separate existing setting. |
| Standard actor palettes, encounters/squads, player profiles, HaloScript | Existing native authoring/runtime boundary. Spark emits source; the inherited engine executes supported compiled caches. Specific generated maps still require gameplay acceptance. |
| `network.coop_script_adaptations` / `HALO_NET_COOP_SCRIPT_ADAPTATIONS` | Host setting, default true for legacy campaigns, frozen at map start through `network_coop_new_game` / `coop_scripts_new_game`. Extraction uses native actions automatically. For other authored modes, Spark may set this to false in its private launch configuration: native player lists/ALL gates, visibility recipients, individual transport/equipment actions and script save safety retain their authored semantics. Host script execution, existing replication, explicit native teleports and empty-player wait protection remain. Source: `port/linux/game/coop_scripts.c::coop_scripts_adaptations_active`, existing `source/hs/hs_library_external.c` callers, `port/linux/src/port_config.c`. Connected source checks and Windows64 compilation pass; specific authored mission/squad acceptance remains unverified. No new RPC, quest executor, squad membership or discovery capability. |
| Firefight combat/late-join status text | Changed host wave/phase, enemy/pending-spawn counts and score/lives, at most once per second. `coop_enemies.c::{firefight_notice,firefight_equipment_reports}` reuse `extraction.c::{extraction_notice,extraction_player_notice}` for chat, per-controller reports and the existing native-HUD fallback, independent of gear being enabled. Two authenticated native clients' wave/equipment rendering and focused hidden-chat fallback checks pass. Existing ready-message summary/terminal replay remains; late-join gameplay is unverified. No persistent panel, query RPC or structured HUD protocol. |
| Tracked Firefight AI kill credit | New unverified host source credits the last damaging player's existing native kill counter once on final death. AI/environmental/no-statistics damage clears individual attribution; departed/reused player handles are refused. Existing statistics transport only; no individual point totals, assists, medals or new discovery operation. |
| Live edits, runtime-state query, transactions/undo | Not implemented by this adapter. No private-memory/debug-console fallback. |
| Native console `forge_target` and `forge_clone` | A living local host can select a visible loaded object by aim and call `forge_clone (forge_target)`. The existing clone uses native creation and console quotas. Map scripts cannot call either tool. Network scenery creation remains refused. Scaled copies use existing native co-op appearance sync on supported peers, and stay refused in ordinary multiplayer and with original Xbox peers. Windows64 native two-account selection and Pelican clone passed. The console command `forge_clone_at (forge_target) x y z` validates the active BSP position and creates the copy there using the same quotas and authority. Both native clients observed the same separately placed Pelican, with the source unmoved, in `build/native-forge-positioned-clone/result.json`. The host can run console-only forge_undo for the last unused copy. Map changes and checkpoint restores clear selection and undo handles. Console forge_pick selects by aim and forge_place creates a copy over clear active BSP ground using native vehicle contact checks. Windows64 two-account pick, rest and undo evidence: build/native-forge-crosshair-copy/result.json. Two authenticated Windows clients observed the removal with the originals intact in build/native-forge-undo/result.json. Desktop Forge Tools save and restore the base-map placement subset below. Spark write operations remain open. |

Accepted discovery identity: `7c65d0865c34-dirty.d990a0d29b19`, Windows/x86,
upstream version `0.9.1-dev`; exact executable/source hashes are in TEST_EVIDENCE.md.
Matching contract, cache version and peer network version are separate checks.
No version-1 discovery field grants gameplay, live editing or universal cross-play.

Personal operation feedback already sent by `extraction_player_notice` uses its
existing `P1 | ...` host-notice envelope. `network_chat.c::network_chat_host_notice`
now applies that controller to every wrapped line and drops missing recipients;
hidden-chat personal receipts reuse `hud_messaging.c::hud_print_notice`. Ordinary
chat, global host notices and native HaloScript `sv_say` retain their own scope.
No new authoring command or packet is required. Native Windows two-view operation
feedback and an authenticated hidden-chat client receipt pass in the current
master checkpoint; this does not provide general squad-scoped HSC cinematics or
audio, and other runtime/platform clients require their own acceptance.

Reviewed private CE-map distribution reuses the native approved-map index and
map-sharing path. `tools/map_torrents.py make --maps <reviewed.map> --torrents <private-output> --index <private-index.txt>` now accepts one compiled cache,
without approving neighbouring files or editing that cache. Explicitly select
the reviewed local index with `HALO_MAP_TORRENT_INDEX`; for isolated private
sessions use `HALO_MAP_INDEX_URL=` and `HALO_MAP_SEED=off`, with normal
`HALO_NET_MAP_SHARE=true` / `HALO_NET_MAP_DOWNLOAD=true` and the owner's chosen
download prompt policy. Do not derive approval from an untrusted host's offer,
replace the bundled/public index or publish private torrent metadata. Native
default approval, signed-list and content-hash checks are unchanged. Spark's next
integration is to present this reviewed-cache choice through its existing private
launch configuration, not a new launcher or an automatic trust bypass.
`build/native-delta-map-handoff-approved/result.json` verifies two real Delta
accounts: client starts without the mission cache, receives the exact CE609 bytes
and spawns in native co-op. The preceding unapproved attempt correctly refuses.
Local Halo resource maps were supplied separately; complete Black Echo, other
platforms and Internet transfer performance remain unverified.

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
The repaired Windows64 setting has compiled/native fresh-profile startup evidence
above; full mission acceptance remains separate. Discovery still advertises only
`runtime.discover`, not a live map-control operation.

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

The recorded automatic-release counterexample is CE609 `1d6bb622` and the
commanded-exit control is `79589da5` (Spark C51). Spark’s preservation index
places both and the C57 Timberland–Sidewinder cache `c84a1ed4` in
`recovery-archives/C62-expanded-cold-history/payload-003.zip`. The
archive volume was unavailable in the inspected checkout. A read-only
handoff of those caches would let Forerunner run the original carrier
comparison and attempt Timberland–Sidewinder on foot and in a Warthog.

Spark 19633a6 compiled the recovered tags with the official MCC H1A Tool.
Cache SHA-256: 4b8c1dea007ef8ed7dd3915309db571cdfa20b820b84c64a17eb5e38f4ff84f0.
Readback preserved the Pistol melee flash and damage. Forerunner regenerated
the exact cache and pinned it with the x86 executable under
`build/zombie-mcc-baseline`. The native run admitted one player and passed
HUD, script and geometry conversion after preserving the custom title in
score-hint slot 100. Audio conversion then refused double_kill (codec 3,
tag 1158, first range/permutation), before spawn. CE609 remains a separate gate.
Forerunner reconnected Milenko's inherited 64-bit MCC loader (upstream
`9e19b2f744`) and its map catalog. The pinned Windows x64 executable
`halo-x64-mcc.exe` has SHA-256
`0039b82749430fb1978330729d9ed54f3383b2489c91b2d8602fa0546861e058`.
It admitted the same cache on loopback and reached the same audio rejection.
Stock MCC controls rejected unmatched sound banks. Successful MCC spawning
and cross-platform acceptance remain open.

The 31,651-byte `double_kill` permutation is identical in the Demo and MCC
cache (SHA-256 `04c3946a26f8b2cef14b27f683d1f574faa2a755c7e427d8d70f432e027d8769`).
The production decoder accepted 847 of the 852 local Vorbis permutations.
Recomputing only the final page CRC did not recover `double_kill`. The following
are the longest complete prefixes accepted by the same decoder when closed at
their last verified page. Each failure is range 0, permutation 0 under
`sound\dialog\multiplayer1`.

| Cue | Tag index | First failing CRC page at byte | Accepted prefix bytes |
| --- | ---: | ---: | ---: |
| `double_kill` | 1158 | 31420 | 31420 |
| `30_seconds_to_win` | 1528 | 271644 | 267259 |
| `blue_team_30_to_win` | 1532 | 16639 | 16639 |
| `slayer` | 1545 | 24995 | 12214 |
| `capture_the_flag` | 1546 | 24995 | 12214 |

Spark owns audio review and re-export. Retain every original stream. These
prefixes are diagnostic evidence and require listening and source comparison
before any recovery is accepted. The runtime still checks every page CRC.

A disposable prefix-only MCC copy, SHA-256
`43142d4383d692efe7a689ac88591447385c105bc84c7bc286bce37aaea982d5`,
converted all 852 Vorbis permutations and exposed two independent engine
faults. A map without authored HaloScript can now use native console threads
when MCC's expanded stack array cannot fit after the reserved checkpoint
regions. Scripted maps still require the expanded array. The three carrier
variants share `fzero\zombie\zombiephysics`, whose compiled inverse inertia
was entirely NaN. The native tag validator now reconstructs a finite inverse
from that tag's authored mass points and center of mass, leaving valid matrices
and source files untouched. The diagnostic map spawned one native player and
sustained 16 actors through tick 315 after both repairs.

In Windows x64 loopback the same diagnostic copy then spawned two players
on separate native processes. The guest walked about 3.6 world units, and
both peers reported the same final player positions and 16 living actors
through tick 780. Both exited cleanly after repairing optional HUD-font
and model-shader references and keeping mounted actor placement host-owned.
The exact executable SHA-256 is `5b59a9ec2e1c292d2322c9b49bd5750d5806a40c470c59eed12993ce90d8929c`.
The standard zombie container (vehicle tag #1251, `mgm\fonzie\sparky\tags\zombie container`)
is refused by native `unit_new` because its animation graph reference is NONE.
The Evolved and Alpha variants retain their graphs. Spark's existing C45
same-hierarchy animation binding must reach the next compiled handoff.
Keep the original tags and all five audio findings above.

Spark `711aa74` supplies the original standard carrier's rig repair.
Rebuilding the audited Demo inputs with its opt-in recipe and the pinned H1A
Tool reproduced MCC13 cache SHA-256
`d520d509aa6a23a3fa67d37c28cc74459835ae53c0a4dae615f910e87b60b958`.
Forerunner Windows x64 SHA-256
`b84fab8268b13adfdf7425d36a06cb49041426b1dfb5101bb20bba96a490fdcb`
admitted the exact cache, then refused the existing `double_kill` codec-3
failure before player spawn. A disposable copy with only the five audited
Vorbis prefixes and corrected header CRC hashed
`7a496b9f908d54743373fc8f902ff8a9156d188d8a861555cf0010f6ac3861e5`.
Two native Windows peers joined on the MCC map's negotiated OpenCE version
43. Both spawned and agreed on 18 standard carrier objects across initial
delivery and the 40-second refresh. They matched 27 living actor, carrier,
and seat tuples and 12 of those actors on foot after automatic release,
within three ticks across peers. Their last full player sample at tick 1770
matched. Both exited zero, input hashes held, and staging was removed.

For the standalone CE609 handoff, preserve the reviewed direct spawner
behavior, the reconstructed inertia and the five-node standard carrier
binding. Strict Invader still rejects the original Pistol's active lighten
flash intensity of 3. Keep that authored behavior and separate damage
values when selecting a CE compiler or compatible equivalent. The five
damaged cues and two missing cues still need authoring review. The
audio-prefix copy verifies native carrier delivery and synchronization
only. Standalone CE, a complete survival round, and mixed Forerunner and
Chupathingy play remain open.

Spark `8005860` restored the original five Vorbis payloads at full length.
The audited recipe reproduced the exact MCC13 export
`126491917a68c50e14a9465a8b066c40f56a9cd3eb933b66f5ee188f36ebedb1`.
Two current Forerunner Windows x64 processes used an unchanged copy on
OpenCE version 43. Executable SHA-256
`e82a58dbddb0f65ae603961c81090e01ab46052e55f614eb29b9298905723b9c`.
Both converted all 852 permutations to 39,912,660 ADPCM bytes and spawned.
The peers agreed on 18 standard, 6 Alpha, and 4 Evolved carrier objects,
including nine Evolved riders in the same native seats. Both saw at least
nine standard riders leave their carriers alive and the same three Alpha
zombies alive on foot at tick 1806. The guest walked, fired two rounds and
took damage. Both peers agreed on the player positions through tick 2280,
exited zero, and retained the original hashes. Disposable staging was
removed.

This verifies engine audio conversion and native network gameplay on the
full-length export. The run used muted output. Evolved egress, completion
of a survival round, both absent external cues, standalone CE609, and
Chupathingy interoperability remain open.

Spark `8d3ac06` supplied typed projectile/effect spawner wrappers. The
native compiler reproduced its MCC13 witness
`2620136544a8118ffe6ac05c21ec453705c59e6de8bef1c6ecad131669612917`.
Applying the previously audited five complete audio repairs, title
termination and finite inverse inertia in a disposable copy gave SHA-256
`19188689239f92441f8e2ba6e1a10ec98b6ff044c4254b8fbbbe66e3f8578186`
with matching native CRC.

Chupathingy `c16096a8` hosted on OpenCE version 44. Forerunner `e82a58db`
joined the same cache. Both converted 852 audio permutations, spawned, and
saw the same 18 standard, six Alpha and three Evolved carrier slots.
The guest saw three new living Alpha bipeds on foot at tick 1821.
The legacy host did not expose individual actor samples. Native guest
movement synchronized to (97.460, -158.993, -0.287) on both processes, which
exited zero without the earlier projectile-group assertion. Original hashes
held and disposable staging was removed. This verifies scoped mixed-runtime
MCC play. Standalone CE609, matching individual zombie exits, Evolved
egress and a complete survival round remain open.

On the exact full-audio cache `126491917a68c50e14a9465a8b066c40f56a9cd3eb933b66f5ee188f36ebedb1`,
both native peers observed 33 matching live Standard releases through tick
2670. Both also saw nine Evolved riders in four carrier slots and no observed
Evolved exit. With the host nearby, Evolved carrier `e3460072` settled at
(40.28, -78.54, 0.05) with native up.z 1.000 and zero angular speed.
The stock rollover rule correctly kept the upright riders aboard. The next
Spark handoff needs a native automatic Evolved placement or launch that causes
a physical rollover and grounded contact. Validate the wrapper's initial pose
and motion against that result. Preserve the original carrier definitions and
full audio. Full Evolved egress and a completed survival round remain open.

Original Demo tag readback confirms a native `human_jeep` Evolved carrier.
Its `W-driver`, `W-passenger` and `W-gunner` seats contain the built-in
Evolved, standard and Alpha variants, respectively. The original weapon
directly references that vehicle, and the Alpha carrier shares its physics
tag and animations. The Demo's malformed unused syntax-table capacity was
normalized on a disposable copy for extraction. Independent readback found
zero scripts, globals, source files, encounters and AI command lists.
OpenCE `fa752834f` and the Fractum snapshot `872f338ca` both use
`vehicle_scripting_unload` through native seat exit. Both also check
ground contact for automatic rollover ejection. Preserve the existing
native paths when authoring an automatic Evolved release.

MCC hosting now honors a valid OpenCE announcement pin with a known MCC
layout. A direct two-peer launch of the same Zombie cache at pin 43 matched
both player positions through tick 420 and both processes exited zero.
Windows x64 SHA-256
`87606ab108d796632914925d6e22d25f2c7ffbe4aeca0fef3df3f31732270d51`.
The run used the normal pin setting. This is scoped native admission evidence.
The exact full-audio cache is held privately under `build/zombie-mcc-baseline`
for the next targeted test.

A finer two-peer capture sampled nine living riders in the valid carriers.
Six stayed in their native seats on the host but never attached on the
guest. Forerunner now publishes AI seat transitions and keeps idle seated
actors in the existing actor stream even when optional deck walking is off.
Both peers matched all nine unit, parent and seat tuples through tick 612 on
the same diagnostic cache. The Windows x64 executable SHA-256 is
`cff418e234acf3f37256862f332c967f419c126d6a315256b5647776a05d4d75`. Both processes exited normally.
No living rider release, combat or round completion was observed.

The co-op checkpoint path also restores tracking for already-seated AI.
In stock B30, two local native peers matched all nine passenger, vehicle and
seat tuples after a tick-62 save, AI erasure and a tick-212 revert. The B30
map SHA-256 was `a6e2ae628f96b666922d6429541068c3e117e8aa5031d00d187b3c853630be0f`
and the Windows x64 executable SHA-256 was
`6946f41bc97a86bff42079a9d4a9068ea5f3276105bf648d99f06a4ee4aca3dc`.

A two-process native Windows test loaded the original Evolved spawner into
Slayer's normal starting slot on a disposable copy of the audio diagnostic
cache. Guest fire created carrier `e3600095`. Native grounded rollover
released its three authored actors at host tick 71, and the guest observed
the same three living handles on foot by tick 73. Both peers also saw the
carrier, and both processes exited normally. The original cache was unchanged.
The loadout probe SHA-256 was
`f50dfbb76c6f99f73a89e8bc157a303beb68b2bf5936bb78cac2c3401da0b4d0`
with executable SHA-256
`6946f41bc97a86bff42079a9d4a9068ea5f3276105bf648d99f06a4ee4aca3dc`.
The unchanged authoring handoff still needs the standard carrier graph and
five repaired streams. The original spawner pickup, full survival round
and Chupathingy acceptance remain open.

On the original audio-prefix diagnostic cache, without loadout changes or
player fire, the three Alpha carriers released living actors at their
authored 60-second interval (tick 1801). Both native peers matched the
three salted actor IDs on foot through tick 1899 on the same executable.
The standard 40-second weapon collection refreshed at tick 1200 but
reproduced the missing-graph carrier refusal.

A separate native combat run kept the same audio-prefix map and used the
fixed x64 executable below. The released Alpha `e35d00e1` advanced from
(73.07, -152.06) into melee range and injured the guest. Guest fire and
melee made the Alpha feign death at tick 699, recover at 750, then die
from a separate hit by tick 948. Both peers observed the same actor,
recovery and persistent corpse through tick 1218. Both exited normally.
This verifies the combat exchange using scripted player input in two
native processes. It leaves survival victory, restart and human play open.

Joining the original diagnostic after the initial release exposed a client
access violation near tick 3240 on Windows SHA-256
`f80831ef378cd54c6d72ebb3f35a3374783ea33ccf73a1bb50370acd433b15eb`. Fault offset 0x1f315d resolved
to the model pill probe in native biped placement. A carrier with no
collision tag left that probe uninitialized. Egress now honors the existing
constructor result and still checks world clearance and both sight lines.

On the same pinned audio-prefix map, Windows SHA-256
`98593413e8900251fca7e3fa77be6ff4d9db2cd79c7075fffe21d931a9e46edd` let the guest join
after host tick 1854. The guest saw the three existing Alpha handles on
foot at tick 2097 and matched twelve carrier seat assignments. The guest
ran through tick 3492 and both processes exited zero. The follow-up did
not sample the same second rollover transition, so its physical release
still needs live confirmation. The standard carrier graph rejection
persisted. Original hashes held and the staged copy was removed.

A second Windows loopback test kept this exact audio-prefix cache on
executable SHA-256 `c135fe65ccb78cf583fafcf127f9a1251f70298a5367f4095694fda5c534497d`.
A guest launched at host tick 1863. Both peers observed the authored
120-second Alpha refresh at tick 3603, with actor count rising from 18 to
21. The three new living Alpha handles matched at tick 3606 (e68c010a,
e68d010b and e68e010c), already on foot in the first sample. This verifies
timed spawning after a late join. Individual seat-exit frames were outside
the sample. Both processes exited zero after tick 4458, pinned hashes held,
and staging was removed.

On that same cache and executable, two weapon probes selected the moving
Evolved spawner object 127 and failed to collect it. The other original
Evolved spawner, object 128, stayed at (40.51, -78.99, 0.95).
After camera-based staging with the camera settled, the guest observed the
host's weapon-swap target 128 at ticks 333 and 336. The test issued E but
neither peer observed an inventory transfer or a shot from that weapon.
The follow-up placed the host at (40.419, -79.079, 0.999). The target
appeared only at ticks 330 to 336, before camera control was restored at
tick 344. Holding E from tick 387 while standing and then crouched never
produced another target or a transfer.
That probe stopped at the transition from the brief native
swap prompt to actual ownership. The held-start Evolved firing result
above remains valid. Both peers exited zero, hashes held, and staging
was removed.

The host now checks that a proposed weapon still exists on the ground
before dropping the player's held weapon. Focused cases reproduced a
second player losing its weapon after a shared pickup was claimed, and
a player losing its weapon after the target was removed. Both now pass,
with 15 focused pickup checks in all. In a private Windows x64 Blood
Gulch Slayer session, ordinary E transferred the rocket launcher object
77 to the same guest body 53 on both peers at ticks 540 and 541.
Executable SHA-256 `9298630f9265f3dfb0bc6aaaf5c39946c64a2483876468af470acc8320f936fe`.
Stock map SHA-256 `50fe52406f075d975e24100a65b26ff696458023dd3509878953052ab0ef858f`.
Both processes exited zero, pinned hashes held, and staging was removed.

The rebuilt executable above also verified the original Evolved pickup
on the same audio-prefix map. The guest held E from tick 630, before
the existing staging helper moved the guest into reach. Both peers
observed authored object 127 held by the same guest body 224 at tick 720
and again through tick 1590. No loadout grant was used. A follow-up used
the same executable and map. Both peers showed zero rounds in weapon
tag 0x534 from the first held sample at tick 720, before the bound fire
key was sent at tick 750. The recovered tag has one initial reserve round,
a one-round magazine and a native trigger that toggles and sticks when
dropped. Its projectile reference names the Evolved carrier vehicle.
This explains the spent pickup and preserves the authored automatic
carrier behavior. A fresh held shot is covered separately above. Both
processes exited zero, protected hashes held and staging was removed.

ChupathingyCE 0.9.1a, source `2e5eac00`, Windows x64 SHA-256
`c16096a841422b2597a3c5f26d8b16be7dd6a2914f678c7ea4b36e31ec3a3050`,
listed and hosted the same audio-prefix diagnostic at OpenCE version 44.
It stopped at MCC tag conversion. Native readback found `ui\multiplayer_game_text`
tag 1597, slot 100, contained a custom 40-byte UTF-16 title with trailing
space padding and no terminator. The inherited score-hint parser at that
revision rejects the payload. Replacing its last padding byte at file
offset 89032460 with zero in a disposable copy yielded SHA-256
`1c25fc9c419c98095d8934bdece351cdbce195f6402b633ca358e2042ec02d38`.
The same Chupathingy executable converted all 2164 tags and spawned a
player through tick 570. Both processes exited zero. The next export
should include a terminated title alongside the audio and animation
repairs. Original hashes held and all staging was removed.

On that exact title control, the same Chupathingy host and Forerunner
Windows x64 executable `9298630f9265f3dfb0bc6aaaf5c39946c64a2483876468af470acc8320f936fe`
joined a private OpenCE session. The Forerunner guest spawned at tick 360
and used ordinary movement. Both peers reported the same guest position
(99.015, -157.204, -0.287) at tick 1530, approximately 2.59 units from the
guest's first observed position. Both exited zero. This verifies mixed
runtime admission and movement for the diagnostic copy. Original map
release and full cross-runtime survival acceptance remain open.

A longer private run on the same one-byte title control used Chupathingy
host `c16096a8` and Forerunner Windows x64
`10f2792bb24bf7480d5d2451548f5c4ffe2295af716fdd1dc6266f7116b87002`.
Both peers agreed on the guest's position through tick 2640 and exited
zero. Forerunner's guest trace through tick 2643 contained no living
on-foot Alpha with definition `e6fd0589` in the 60-second release window.
The host had no per-actor trace in this build. Protected hashes held
and staging was removed.

The Alpha carrier (tag 1414) references `fzero\zombie\zombiephysics` tag
1007. Rebuilding just the nine non-finite inverse inertia floats at file
offset 86351418 from the tag's original mass points and center of mass
produced diagnostic SHA-256
`1d08e177faeee32ba8fece2e571a0aacee70f4be71c38f9a2b46452c824e9a89`.
This was the sole authored change from the one-byte title control.

The same Chupathingy host and Forerunner guest then observed matching
initial carrier indices 208, 209 and 210, and matching refresh indices
196, 197 and 200 at tick 1950. Forerunner observed three living Alphas on
foot on join and three more at tick 1821 (e37800ca, e37900cb, e37a00cf),
still living at tick 2646. Both processes exited zero. Protected hashes
held and staging was removed. The next standalone export needs a
finite stored inverse inertia alongside the title, audio and standard
carrier animation repairs. Full mixed-runtime survival remains open.

In the reverse hosting direction on that same diagnostic, Forerunner
Windows x64 SHA-256 `6f02d8a8fab2687b24a83da8ca79e06c516a54a0938701137a55c4afaeb3b5a5`
hosted with the pinned Chupathingy guest. The host observed three newly
released living Alphas on foot at tick 1806. Both processes agreed on the
guest's position through tick 2190 and exited zero. The legacy guest has no
per-actor trace and logged tag-group and invalid-index assertions
(`cache_files.c` 298 and 522), so mixed-runtime combat is still unaccepted.

The exact legacy executable and map also reproduced the tag-group assertion
while hosting alone, then reported invalid block access at `tag_groups.c`
3089/3090 and exited with Windows status `0xC0000005` before its first
sampled tick. The tag-group failure occurs without a Forerunner peer.
Separately, standard carrier tag 1251 references five-node model
`weapons\gravity rifle\gravity rifle` (node checksum `5029bde0`) with no
animation graph. This cache contains no five-node animation graph. The
authored C45-style same-hierarchy binding is still required. Original hashes
held and all staging was removed.

Legacy native debugger readback resolved the group assertion to
`weapons.c::weapon_precache_projectile_trails` at Chupathingy RVA `0x73c972`.
The first trigger on spawner tags 1332 (Evolved), 1413 (Alpha), and 1419
(standard) points directly to vehicle tag 996, 1414, and 1251 respectively.
Chupathingy `2e5eac00` reads those references as `proj` and gets `vehi`.
The same run subsequently stopped in collision-model node lookup.
Forerunner already type-checks the projectile-only path in `34af72440`.

A disposable control cleared only those three trigger handles. Cache
SHA-256 `6c2bdfb44d9c2bf595163859a4083455b6e059c88086111269354e35642126cd`
loaded in the same legacy executable, reached tick 150 with zero assertions,
and exited normally. Clearing the handles disables the three carrier
launches. For a legacy build, Spark can test its existing native projectile
wrapper with a real effect location and the C46 hierarchy and collider
repairs. The wrapper's carrier delivery and legacy round remain unverified.
Original hashes held and both debugger stages were removed.

The initial egress-fix executable also passed native seat exit in solo
and loopback co-op. A disposable CE609 copy of the existing Spirit fixture
cleared only its named Pelican's collision-model reference and updated the
header checksum. Its SHA-256 was
`8585e7506543d6b373ac63e6b924eaee8654fa166ee8905dd36b94c588a362d2`.
After scripted seat setup, normal E put the solo player on foot by tick
180. The co-op guest launched at host tick 98. Both peers matched Pelican
e2710002 seat 1 at tick 330 and the on-foot guest with the same kit at
tick 390. The final guest positions agreed to the displayed precision.
All processes exited zero, protected hashes held, and staging was removed.

A separate capacity comparison reused the valid Alpha weapon in the
standard collection, leaving its nine placements and timer intact. On
the identical disposable cache SHA-256
`0b214f0c101061f1d6ae2e90fcd63af28741a5c384494473e03f11c35bf5e057`,
the prior build dropped mounted requests and both peers saw 19 living
actors. The existing native queue now retains overflow in saved unit
flags. Both peers saw 25 living actors and the guest saw all 15 riders
released through native rollover. Both processes exited normally. The
fixed Windows x64 executable SHA-256 was
`baee970f04530fce9e02f22141a5361efe8ffdd790038d0efb7a4dede9096d4f`.

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
stash ownership. Active operations lock membership and faction selection for the
squad. Choose a squad before its first member deploys. Members already selected
can deploy in sequence, and a settled squad can admit members and redeploy
independently of another. In the same live Delta match, Deploy can restore the
returning verified account's prior selection if the squad is idle, has room,
and matches the player's native faction. Offline players free their places.
A fresh match clears these choices. Disconnect equipment loss still applies.
Native Windows evidence: build/native-delta-squad-choice-reconnect-ready/result.json.

`forerunner_squad_01_complete`, `forerunner_squad_02_complete`, etc. are optional
native void callbacks after that operation's survivors physically return and
finish durable banking. Other squads and hub members who have not deployed in
that operation are excluded. A member with unsettled banking or a pending loss
keeps the squad open. A durable bank counts after its owner leaves the return
area or disconnects. If the remaining members lose their runs, the callback
can fire with nobody left at the return point. An operation with no durable
escape never produces success. The map-wide forerunner_extraction_complete
callback requires the living players to be physically at the return point.
Native two-account evidence includes departure from the return area
(build/native-squad-return-roam/result.json) and disconnect after banking
(build/native-squad-bank-receipt/result.json). Reuse a continuous script beginning with
`(sleep -1)` for later operations, as in the existing
`tools/fixtures/extraction_factions_native.hsc`. These callbacks never award gear.
`extraction_update` uses the same active squad membership gate as redeployment.
The optional void callback `forerunner_squad_01_aborted` (and the same
pattern for other squads) lets the author reset objectives after a failed
operation. It fires once after all members finish and no member banked. An
escape escrow that could not finish writing before a disconnect suppresses
the abort. Existing bank, transport and map-wide callbacks retain their
own gates. This path is compiled and harness-verified; native scripted
acceptance remains open.
A reconnect's Deploy interaction can close the previous player slot and
finish escape escrow before the next host update. Starting another squad
operation in that interaction used to erase the prior outcome before its
authored callback. Deployment now waits for the host to report the closed
operation. The regression first reproduced the missing callback, then
verified a single completion followed by redeployment of the same banked
item ID. All 124 focused squad, objective and transport checks passed.
Windows x64 SHA-256 `928ad339b7a2775bb69edd64346cb3fa6773eb33f3d192cc2774d8c933aa8421`
admitted the squad fixture `25d28447a49aa4ddf849729f1890b1ccaad78bb4b9c55be30c2bbc4ba1a45196`
in private co-op startup and ran to tick 390. The ordering fault was
exercised in the harness. Originals held and staging was removed.
If the escrow still cannot finish at disconnect, the map remembers its
exact run ID with the verified owner's existing squad choice. Reopening the
stash can report that escape once if its receipt matches the current
operation. An older receipt or already-reported escape cannot change the
current result. The 127 focused checks include deferred recovery, stale
operations and duplicate completion. Windows x64 SHA-256
`b84fab8268b13adfdf7425d36a06cb49041426b1dfb5101bb20bba96a490fdcb`
loaded the same squad fixture in private native runs. Without a profile,
the Deploy device reached the saved-profile requirement. A fresh run
created a profile through the native menus, opened `fn_squads@ce` via
`HALO_START_MAP`, and used the Deploy device at tick 120. The native
save root held one profile and one gear record, and deployment waited
for squad selection. The run reached tick 1095 and exited zero. Escrow
recovery was exercised in the harness. Original hashes held and all
staging was removed.
The same pinned Windows executable and CE609 squad fixture verified physical
solo deployment and recovery with a fresh native profile. Walking and Use
selected squad 1 at device 9 and deployed at device 3. The live stash was
FRNG v1, generation 2, deployed. The player walked to the field device,
activated at tick 2910, and waited until extraction was ready at tick 3210.
Walking back into the authored return volume banked one weapon and two
frag grenades. The stash checksum matched, generation 4 was idle, and the
authored squad completion woke once. The first run's time limit expired
while the player walked back toward Deploy. A repeat with a longer limit
kept the same map and binary hashes. It activated at tick 2940, recorded
the checked bank at tick 3870, displayed the debrief, and used Deploy at
tick 5280. At tick 5370 the checked generation-6 record was deployed with
one reserved weapon, whose ID exactly matched the generation-4 bank. The
process exited zero at tick 5925. Originals held and disposable staging
was removed. This verifies the solo on-foot fixture loop.
The two-account native fixture proves separate selection, callbacks, banking and
same-ID redeployment (`build/native-delta-squad-settlement/result.json`). It does
not add alliances, scoped presentation or multiple aircraft per faction. Existing discovery version
1 and legacy clients do not imply those capabilities; maps requiring this binding
must target a runtime containing these helpers. No new packet/save format or
editor subsystem is requested. Spark's next full mission handoff remains below.

Squad objective extension (`extraction.squad_objectives.v1`): declare an ordinary
`(script static object_list forerunner_squad_01_players (list_get (players) 32767))`
for each authored squad's two-digit number. `hs_runtime.c::hs_script_evaluate`
delegates those exact static/object_list names to `extraction_script_players`;
unrelated scripts and `(players)` are unchanged. The fallback selects beyond the
bounded player list; negative indices select its first object. Extraction now
latches native script actions at map start. Older runtimes require `network.coop_script_adaptations=false`, otherwise queries return
NONE with one diagnostic. Guard ALL objectives with a nonzero `list_count`.
Queries are snapshots: requery after deployment, respawn or membership changes.
Native object-list element positions, including `(players)`, are not network
player indexes or authenticated owner identities. Use the scoped membership
queries for squad objectives; never infer an account from list position.
They include living current-operation participants, including downed players and
returned survivors for callbacks; exclude unissued hub members, lost/old runs,
stale bodies, pending death and changed/unverified owners. Allocation failure
refuses partial lists. A restored world cannot query.

Forerunner advertises `extraction.squad_begin.v1` for optional native
`forerunner_squad_01_begin` and the matching two-digit script names for
other supported squads.
Use a continuous void script that starts asleep and sleeps again between
operations for repeatable
setup. The first confirmed deployment of each squad operation wakes that
script after the operation roster is visible and its previous objective
gate has cleared. Further deployments into the current operation keep its
state. Spark supplies the reset body for that squad's actors, objectives,
terminals and transport. The established ready group still belongs to
objective completion. Map-authored reset and Black Echo acceptance
remain open.

Optional native `extraction_squad_01_ready` groups (and corresponding numbers)
admit extraction only at exactly 1. Each requires its squad terminal; duplicate
groups and aliases to other squad-ready groups, known hub inputs or transport
outputs refuse admission. The runtime clears readiness on binding and a new
confirmed squad operation. Native HSC sets it to 1 after real objectives.
Revocation cancels activation/ready escape before escrow; native consumption
already in escrow finishes its existing durable transaction. Missing groups retain
previous maps' behavior. Native globals, triggers, actors, rescue and terminals
remain the mission tools; no quest format or reward writer is added. Two real
Delta clients pass separate HSC volume objectives and gated banking/redeployment
in `build/native-delta-squad-script-objectives/result.json`. This is a fixture,
not full Black Echo or scoped presentation acceptance. Spark next: require this
feature through existing discovery and emit these declarations/groups through
its existing mission HSC path for independent squad objectives.

On maps with a managed aircraft crew signal, an authorized call or recall
starts the ten-second activation for eligible deployed squadmates on foot in
the same faction landing zone and current operation. The ready caller keeps
their completed countdown. Each countdown cancels
independently on leaving the zone. Each player still needs a valid native
passenger seat and a durable bank in their own stash. Other operations and
unmanaged maps retain their individual calls.
The focused harness covered one call through two separate banks, another
squad in the same zone, and a squadmate leaving during the countdown.
Windows x64 compiled and the existing `fn_sqops@ce` fixture admitted the
managed hub through tick 330 in a private native smoke. Live two-account
activation and flight on the final Spark mission remain open.

17 November connected native reference:
`tools/fixtures/extraction_transport_objectives_native.hsc` composes with the
existing `extraction_transport_native.hsc` controller; its companion HMT contains
scoped field-contact/recovery notices. Two verified clients now pass actual Flood
combat, distinct corpse-weapon pickups, these per-squad readiness gates, physical
Spirit evacuation, independent durable banking and same-session exact-ID
re-equipping. Evidence: `build/native-delta-squad-combat-salvage-roundtrip/result.json`.
This is one bounded owned fixture operation, not Spark Black Echo or a second
mission playthrough. Spark can reuse the original HSC pattern through its existing
authoring path: native initially-unplaced Flood encounters, navigation, compatible
loaded weapon tags, existing squad terminals, distinct zero-initialized readiness
groups and HMT. Native co-op scaling remains active; do not grant/bank equipment in
scripts or enable a second encounter director. Its native control-name text source
is `tools/fixtures/extraction_transport_controls_native.txt`; use standard native
Unicode strings, not a competing placement/save format. Original assets remain
private. No editor/source dependency or new runtime interface is introduced.

Squad-authored text extension (`extraction.squad_notices.v1`): require this
optional feature through existing version-1 discovery for private mission cues.
Declare an ordinary zero-argument native HSC helper:
`(script static hud_message forerunner_squad_01_notice sq01_recover)`.
Author `sq01_recover` in the scenario's native `hud_messages` HMT tag. A helper
may return a conditional message, as in the existing faction fixture's ready-group
example. Call it at actual deployment/objective transitions, not every tick.

`hs_runtime.c::hs_script_evaluate` delivers after reserved static/hud_message
evaluation, preserves its return value and respects disabled scripts.
`extraction.c::extraction_script_notice` reuses `extraction_script_players` for
host authority, admitted map, native co-op policy, operation epoch, identity and
living-body filtering, then `extraction_player_notice` per participant. Temporary
native lists are freed. Ordinary scripts/global campaign objectives stay native;
there is no new opcode, script parameter, quest state, reward writer or packet.

The existing carrier accepts one nonempty plain-text HMT element, printable
ASCII and at most 127 characters. `hud_messaging.c::hud_messaging_notice_text`
validates indices, bounds, termination and complete capacity before copying.
Icons, non-ASCII, empty, overlong or malformed text refuse delivery with one
per-map diagnostic; global objective/help state is unchanged. The two-account
Windows/OpenGL fixture passes distinct recovery/completion cues on host chat and
remote hidden-chat HUD (`build/native-delta-squad-notices/result.json`, eight
inspected PNGs, 109 synchronized seconds). Full Black Echo, older clients,
localized/icon text and other platforms remain unverified. Spark next: emit
these standard HSC helpers, native HMT text and transition calls through existing
mission authoring; the complete combined aircraft mission handoff stays open.

Squad mission-radio extension (`extraction.squad_radio.v1`): require this feature
through existing discovery and emit a standard zero-argument static sound helper,
e.g. `(script static sound forerunner_squad_01_radio "sound\\dialog\\owned_cue")`.
Call it at the authored mission transition, alongside the corresponding notice.
Native HSC evaluates the tag normally; `hs_runtime.c::hs_script_evaluate` then
calls `extraction_script_radio`. The return value remains the original sound tag.
Other scripts and ordinary `sound_impulse_start` remain unchanged. No new opcode.

The existing living/verified/current-operation query owns recipients. Each full
remote-machine handle receives one existing numbered impulse event, not one per
member. Every active local player on a receiving machine must be in the eligible
audience; mixed split-screen machines are withheld with one map diagnostic.
This is a shared audio-output limit, not independent audio per local viewport.
Invalid/unloaded sound tags and unavailable audiences do not play.

`network_coop.c::{network_coop_private_sound,host_send_events}` keeps audience
metadata host-only, preserves ordinary broadcast's fast path and the existing
three-send bounded event queue. Nonrecipients receive an empty impulse with the
same number, preserving sequence wrap without exposing the sound tag. Existing
clients ignore it. Reused/disconnected machine handles cannot inherit private
cues. This is the inherited best-effort event transport, not a new acknowledged
audio protocol. `game_sound.c::scripted_sound_new_internal` remains the one native
playback/timing implementation. The host updates native per-tag script time for
remote-only cues without playing them locally. Native per-tag timing is shared;
this does not create separate per-squad copies of one sound's script timer.

Two authenticated Windows/OpenGL processes pass actual recovery/completion cue
routing and native sound-instance creation (two intended instances per machine,
none on the opposite machine, 119 synchronized player seconds):
`build/native-delta-squad-mission-radio-final/result.json`. The trace is enabled
only by existing `debug.network_test`. SDL dummy output was used; physical audio,
final faction dialogue, other platforms and full Black Echo stay unverified.
No microphone, capture, voice settings, renderer or inventory path is added.

Latest actual Black Echo handoff: read-only Spark `54c1984` documents the complete
combined source/native milestone in `docs/RUNTIME_INTEGRATION_20261107.md`,
parent `5a7a53a29f803a8832bf0008efa10a164473b049`. Production revised CE609 hashes:
Pelican `31505afb15da14ff32af694afecc28362ff911bd6df6b79778195493c6c3ebab`;
Spirit `2d6b31b57f43a33a244884a0e1dbfd702b1df9ae26e24f3358e03d989df0b176`.
Spark records QA28356/run-tRm24j as closed with its root absent. Those caches and
saved browser build inputs were disposable; no retained final export path is
identified in that checkpoint. This does not establish a delivered runtime candidate. Spark next: make the reviewed final cache plus its exact
mission/controller source and build inputs available through the existing native
export/handoff, with those hashes or a clearly identified newer reviewed revision.
Forerunner must load that delivery; it must not recreate Spark's editor, rebuild
Spark or substitute its regression fixture for the complete mission.

Squad guidance reuses native unit-targeted nav points, not team markers (two
squads may share a faction). For example, after checking the query is nonempty:
`(activate_nav_point_flag default (unit (list_get (forerunner_squad_01_players) 0)) sq_artifact 1)`.
Apply this to each returned member, at most six; remove the matching flag/object
marker at the objective transition and durable-return callback. Requery after a
body changes. Native HUD capacity remains four markers per viewer, shared with
ordinary authored markers; this does not reserve four more for each squad.
Use a stable scenario cutscene flag for a shared location objective. A marker
on a collectible weapon follows that object and may disappear when another squad
collects it; a location flag survives that pickup. Movable aircraft and other
live targets may still use native object markers.

`hud_nav_points.c::hud_unit_activate_nav_point_with_flag/object` and their native
deactivation functions own local presentation; `network_coop.c::network_coop_note_nav_point`
retains/relays the existing events. The current cache accommodates the configured
player and native team budgets instead of the previous 16 bindings. Existing
`host_send_hud_state` replays at most eight bindings per tick, retaining its cursor
when the shared event queue is busy. Packets, protocol 44/45 compatibility,
per-viewer HUD/save layout and ordinary script execution are unchanged. These
markers are presentation targeting, not secret mission data or scoped audio/
cinematics. Earlier query-capable builds still have the 16-binding cache; the
current runtime checkpoint is required for larger authored marker sets.
`source/objects/object_deleted_procs.c` now calls
`network_coop_nav_point_object_deleted` for exact native viewer/target deletion.
This retires dead bindings without discarding flags or a new salted handle;
`coop_remove_nav_point` preserves bounded replay across compaction. Scripts must
still requery changed bodies and reactivate markers on recreated targets.
`send_nav_point` / `client_apply_nav_point` reuse existing optional event name/
definition fields and `object_find` for named scenery/devices, whose local
handles can differ. Unavailable targets wait for existing host replay; native
deactivation/deletion clears local bindings. Earlier receivers retain their
previous behavior; protocol compatibility does not supply this repair.
Forty focused checks and Windows64 compilation pass. Final-source two-account
native replacement/remote-marker evidence is
`build/native-delta-nav-named/result.json`; deletion/reconnect/requery evidence
and its explicitly retained pre-ready item-comparison failure are in
`build/native-delta-nav-lifetime/result.json`. Native same-tick ordering, full
mission composition and other-platform acceptance are not claimed.
Normal in-process campaign checkpoints now retain the last successfully saved
guidance in existing runtime checkpoint metadata. `network_coop_reverted` restores
it after roster repair; both inherited checkpoint hooks use
`client_clear_nav_points` before replay. Failed saves/new maps do not inherit a
discarded timeline. Native two-account save/change/revert evidence is
`build/native-delta-nav-checkpoint-fixed/result.json`. No save/packet format or
authoring interface changes. This does not make saved-world gear bankable;
Extraction's existing suspension after world restoration remains mandatory.
Windows64 compiles; the existing marker harness covers 24 independent unit
bindings and bounded full-cache replay under congestion. Two authenticated native
clients show separate objective-stage markers in
`build/native-delta-squad-guidance-flag/result.json` (inspected host/client PNGs).
The preceding object's-pickup failure and accepted banking/redeployment evidence
are retained in `build/native-delta-squad-guidance/result.json`. This is not
full Black Echo, six independent clients, scoped audio/cinematics or another
platform's acceptance. Spark next: emit per-member calls through its existing
HSC authoring path, using a stable flag for a shared location objective and
deactivating the old marker when that squad advances.

A candidate needs the native Use control `extraction_deploy`, the
`extraction_return` hub volume, and a field route. The shared route uses
`extraction_activate` and `extraction_zone`. Controls must be powered, usable,
reachable and have `triggers_when = 0`. `extraction.c::extraction_control_touched` rechecks native power,
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
The same terminal now appends per-owner stash pages, also after restart, covering
all stored items rather than only currently compatible loadout choices. Native
Use advances pages; crouch-use on a displayed item requests its explicit discard,
then a separate crouch-use after one second confirms within ten seconds. Ordinary
Use cancels the uncommitted preview. Deletion requires the same owned idle record,
generation and living native body on foot in the hub; active runs/reservations
remain protected. A stale page requires review again. Unavailable weapons can be
deliberately removed from owned storage without becoming deployable or imported.
`gear.c::gear_item_available` is the shared read-only compatibility predicate;
`gear_discard` retains its locked atomic owner/ID/generation transaction. Pending
published deletion retries finish that exact write. No save/protocol/schema or
new Spark operation/marker. Two authenticated native clients verify separate
stash rendering, one confirmed unavailable-item deletion and unchanged other-owner
gear; fresh processes reopen zero/two stored weapons without item resurrection.
Exact binary/cache, capture-step failure and scoped verification are in the
current master checkpoint. Other platforms and full mission acceptance remain open.
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
reconnects global collision, objects, AI, renderer and audio state. The existing
loader can restore the source BSP after a refused destination load. Players'
`players_reconnect_to_structure_bsp` now discards a pending switch whose index is
invalid or whose destination differs from the actually loaded BSP: that recovery
must not eject or relocate passengers. Player clusters are still invalidated;
matching successful classic transitions keep their gather behavior. Real-source
loader/player harness coverage and Windows64 compilation pass; native failed-load
passenger preservation, complete world/AI rollback and multi-region gameplay
remain unverified. No authoring format or discovery capability changed. It is not
multi-region streaming or simultaneous residency. OpenCE main inspected at
`1519c65facbb068e44b956da2769198a83a17847` is already inherited; community
multi-BSP reports do not prove a newer implementation here. Two joined regions
within current limits are the first native traversal candidate, not proof of
19-region capacity. Concurrent residency, bounded resource loading/unloading,
cross-region authority/replication, persistent world state and portal-aware
render/audio contexts remain engine requirements. Keep all limits and original
installations intact; no stock Xbox support or seamless BSP switching is claimed.

The loader now refuses an invalid destination reference before disconnecting
the current region. It clears the pending gather while preserving the live BSP,
objects and cluster activation. Twenty-five focused checks passed. A private
Windows x64 stock B40 run switched 0 to 1 to 0 through the native console at
ticks 0, 15 and 30 and exited cleanly. Executable SHA-256 `7f24bdd8e46d1650fb5dc239deab147267a1494b87bf82ba0641d25a83bdfca6`.
Control map SHA-256 `0a7b26d6d828e7b773d36d9d5c798b9bf4b143332301a0c0e7a3d685782eab48`.

A newer accepted host snapshot now cancels an obsolete queued client switch
when the called-for BSP is already loaded. The message format and checkpoint
ordering are unchanged. Thirty-five focused checks passed. Two Windows x64
loopback processes using OpenCE protocol followed the same B40 console round
trip at ticks 0, 15 and 30 and exited cleanly. Executable SHA-256 `8b7f4b0432d0f706d9fd7ae6410a5ce78e1835a120053f68995763fdfb9edd85`.
The map hash is the B40 control above. The test switched during the opening
cinematic and logged a flying diagnostic. Physical connected-region traversal
remains unverified.

For a continuous co-op crossing, Spark can leave the native switch's
cutscene flag unset and place its trigger in the shared collision overlap.
Already-arrived infantry and vehicle roots must be inside that trigger and
classified in the destination BSP. Forerunner keeps their positions, seats
and gear. A vehicle root also provides the gather anchor when its occupied
seat straddles the seam. Explicit gather markers and stranded recovery keep
their existing behavior. Windows x64 compiled and 41 focused checks passed.
Physical testing still requires the connected native map.

Continuous co-op admission also tests the vehicle root. This aligns the host
switch, dedicated-server vote and client waiting hint with reconnect. The
focused suite checks root-inside/seat-outside, the inverse, infantry, local
play and marked transitions. Windows x64 compiled and 43 checks passed.
The stock B40 fallback loaded BSP 2 on both peers but the camera-based setup
left both players at the opening coordinates. No physical crossing was run.
The B40 hash is recorded above. The tested executable SHA-256 was
`4fd0fa96437af30380c2a2f12e0487a7a947ac43e639533967969aaaf0d1db67`.

An unmarked crossing now also rejects a stranded vehicle as a gather
anchor when its passenger seat overlaps the destination. A focused
regression first reproduced that bug, then passed along with 43 existing
checks. Arrived riders keep their seats and gear while stranded players
recover beside a valid destination root.

On unchanged stock D40 (`7c6dccc9f928f4a44db992c4d6f5fa6191e9cfd331bb0d0b631540f0fc9bc796`),
the native co-op skip vote admitted both players. Setup placed the host
at authored flag `s2`. Physical input then walked x=-24.651 to -31.326
and back to -24.726 at y≈21.4. BSP 1→0→1 switched on host ticks 204/372
and guest ticks 207/378. Both peers retained the crossing player's body
handle and weapon tags, and the host's ammo stayed unchanged. The guest
observed zero rounds for the host's stowed secondary. Both processes
exited normally and staged assets were disposed. Windows x64 executable
SHA-256 `3f9c74bd55f89fe76b8adc2af2aced5b2df829551746b14762446cb50623c1a5`.
This verifies one host walking through the stock passage with a connected
guest. Stock vehicle traversal is covered below. Independent regions and
Timberland–Sidewinder remain open.

D40's stowed tag 734 is the plasma rifle. Its native magazine has zero
initial, maximum reserve and loaded capacity. The previous 100/0 display
compared an unused counter. Shared gear now banks native battery charge and
ignores the placeholder when reissuing older records or granting authored
faction equipment. Real magazines still enforce their authored capacity.
The focused Extraction harness reproduced the failure and then passed
85 checks, including reopen, reissue and rebank with the original charge.

Windows x64 SHA-256 `8a75fda064117a81a61bb194ee72745fbefdf9a32bd2d32e11b20c97365217a6` built successfully.
The local Spirit smoke used a disposable CE609 fixture SHA-256
`28edb09683dc5d6b31d575f051e9295b040b3a636b682206d6b7e26face631b6`, with the Covenant plasma starter’s
unused reserve changed to 100. Native profile creation, map launch and
Covenant selection passed. Aiming at the deploy control's live bounds
(center 49.624, -87.469, 0.714, radius .350) offered use at tick 791.
Keyboard E started deployment and the player held plasma pistol 243:0
at tick 840. This verifies a native starter grant. Banking and reissuing
an older battery stash remain covered by the focused harness. The process
exited zero, protected map and bank hashes held, and staging was removed.

Stock A30 vehicle staging used the same executable and unmodified map
`4875fa105e6c517589a796cfdcc7fcaa07e98df13d14e601bfc9eb1eab7e30b5`.
Both native peers kept the same Warthog `e62a00ed` and distinct driver
and passenger seats after setup at (-338.003, 20.501, 37.0) in BSP 1.
The alignment check withheld throttle because the seated driver's aim
stayed at -142 degrees after the scripted forward-facing setup.

HaloScript object_set_facing now uses the native player-input frame for seated
local players. Third-person seats retain world heading. First-person seats
invert the same vehicle-up rotation that native input applies. Windows x64
executable SHA-256 `73a9af5f05626bd4a2848face3cdff134b450b40c6b8dcb36e3ea1f33c903644` on the same stock A30 map
put the driver camera behind the 0-degree approach. Ordinary throttle moved
both seated players from (-338.008, 20.503, 37.001) along the upper approach
and both peers showed the driver at 0 degrees. The Warthog turned south to
about (-331.37, 15.60, 35.35) and stayed in BSP 1. Aligning the Warthog
during setup kept the intended
heading and reached (-325.559, 15.319, 35.353), still outside the authored
BSP 1 to 0 volume whose minimum y is 18.442. Both peers kept their seats,
exited cleanly, and the unmodified map and executable hashes held.

The stock named light bridge is at (-189.350, -5.000, 30.162), well east
of the seam. Cache readback identified a flat lower route near y=14. With the
same pinned A30 cache and Windows x64 executable above, two Forerunner peers
on OpenCE protocol used scripted staging at (-338.005, 14.002, 35.354).
Normal throttle and mouse steering then drove through both authored switch
volumes, with no further position changes by script. Host BSP 1→0→1 was
observed at ticks 1026 and 2007, and client at 1029 and 2010. Both kept
Warthog e62a00ed with driver seat 0 and passenger seat 1. Both players retained
weapon/ammo readings 47f:180 and 731:76. The host stopped at
(-341.400, 18.489, 35.537). Both processes exited zero, the pinned hashes
held, and staging was removed. Timberland–Sidewinder remains open.

On the same stock A30 cache, Windows x64 SHA-256
`b84fab8268b13adfdf7425d36a06cb49041426b1dfb5101bb20bba96a490fdcb`
verified guest driving on OpenCE protocol. Two local network
processes staged the Warthog at (-338.000, 14.000, 35.309). Once the staging
flag was restored, ordinary guest throttle and mouse steering drove across
the native BSP 1 to 0 switch with the listen host riding as passenger.
At tick 930 both peers reported BSP 0, Warthog `e62a00ed`, guest seat 0
and host seat 1. Their positions were within .014 world units. Both
retained those seats and agreed at (-315.860, 22.225, 35.843) at tick 1200.
The car traveled farther north after crossing. It did not reach the test's
eastward stopping target. Both processes exited zero, the pinned inputs
held, and disposable staging was removed. Guest-driven return and the
authored Timberland–Sidewinder route remain open.

### Authored Timberland–Sidewinder road traversal

Spark's `fabf859` authoring recipe regenerated the three-route CE609 map and
its native Warthog trial. Parent cache SHA-256
`81e3c047987c5d5a5752ff63805a29e9734079a22502f1373496f15926d8c664`.
The trial preserves its single resident BSP byte-for-byte and independently
verified 68 native starts. Its separately pinned SHA-256 is
`ce043863cd80e26ce0b95f5449a33d80c6e796909f90ce1ae668bae817a993c2`.
The test input is retained privately at
`build/native-megaworld-inputs/gsmegaroad.map`. Windows x64 executable SHA-256
`e82a58dbddb0f65ae603961c81090e01ab46052e55f614eb29b9298905723b9c`.

Two local OpenCE network peers launched from the authored trial starts. The
guest walked the road from Timberland (37.472, 34.431, -21.293) to Sidewinder
(73.280, 34.505, -21.075) and back (37.103, 34.394, -21.305). The listen
host's matching samples were within .075 world units. In the vehicle trial
the guest used the native seat interaction to enter Warthog `e26f0000` as
driver. Normal throttle drove from (38.134, 33.599, -20.753) to
(72.341, 33.993, -20.723) and reverse returned to (38.047, 34.507, -20.838).
Both peers matched the driver, vehicle and seat at the crossing samples
and agreed on the final occupied position at tick 2910. Both processes
exited normally and all input hashes held.

A second two-peer session on the same executable and cache verified both
side routes at the same time. The listen host walked the ridge from
Timberland to Sidewinder and back, reaching the raised plateau near
z -19.108 in both directions. The guest walked from Sidewinder through the
cave to Timberland and back, following both interior corners near y 47.449
at z -21.075. Both peers recorded all 15 ridge and 11 cave waypoints. The
worst position deltas were .094 and .076 world units, respectively. Both
processes exited normally, inputs held their hashes, and staging was removed.

The Warthog tests used opposing Slayer teams and only the driver rode.
Teammate passengers, AI and objectives across regions, and the complete
19-region world remain open.

### Three-region Blood Gulch connector

Spark's reviewed CE609 extension at `099a63e` adds Blood Gulch while
preserving the Timberland–Sidewinder routes. The exact cache SHA-256 was
`4733768fb91989cc5db1edb96dfc49091946da0198de3e51c92fa2617b33a286`.
Windows x64 executable SHA-256
`6929b8d3ffb7bf80f713b6094bcdf25989bdb9fe8f50aa4d5fca210579af2226`.

The debug camera placement command now selects the same first local player
as the camera loader. In the previous native attempt the host was slot 1
and the guest was slot 0, so placement changed neither player's position.
With the fix, one local placement staged the host at each approach while
the guest retained the original spawn. Ordinary keyboard movement then
took the host from (167.292, 27.139, -17.975) to
(278.507, 27.142, -18.543), and in a separate session from
(278.229, 27.141, -18.172) to (164.587, 27.140, -18.369). Both peers
matched 50 outbound and 51 return samples with maximum position deltas
of .0290 and .0381 world units. The guest stayed at its spawn while the
host traversed. Both clients exited zero in both sessions. The protected
handoff, staged caches and executable kept their hashes.

This establishes two-way infantry traversal of the new connector on one
resident BSP. Vehicle traversal, AI, objectives and the four-region
candidate remain open.

### Four-region Battle Creek connector

Spark `90c784a` adds Battle Creek to the same resident BSP. The exact CE609
cache SHA-256 `f66931cdd82bb15655868a39d3b1a5682d8baa088f1da01bd3efafd47a280251`
ran on Windows x64 executable
`6929b8d3ffb7bf80f713b6094bcdf25989bdb9fe8f50aa4d5fca210579af2226`.
After one native placement at the approach, ordinary keyboard movement
took the host from (370.834, -33.779, -17.912) to Battle Creek
(460.093, -33.778, -18.274), then back to (369.064, -33.670, -18.250)
in the same session. Both clients matched 86 traversal samples with a
maximum position difference of .059 world units. While the host crossed,
the guest moved independently by 4.5 units near (60.500, 34.124, -21.075).
Both processes exited zero. The 2.2-unit-wide Battle Creek connector has
infantry evidence. Its vehicle clearance, persistent AI and objectives
remain open.

The vehicle-ready Timberland–Sidewinder cache was also used to check
a shared Warthog in Team Slayer. Native team change refused the host as
the last player on its team. The stock seat check admitted the guest only.
Both clients agreed on the guest's occupied position and exited zero.

A later private three-client Team Slayer trial used that same road cache
and Windows x64 `6f3129053c03d645d8b79b9efe4ac1dda5c65ca07aabff02441d62659b51074e`.
Two separate clients on team 1 occupied Warthog `e2710002` in driver seat 0
and passenger seat 1. The inherited seat diagnostic staged both riders at
the Sidewinder entrance (68.099, 36.049, -20.689). Normal throttle and
mouse steering then drove to Timberland (38.525, 36.455, -20.807) at
tick 630 and reversed to Sidewinder (72.761, 35.278, -20.725) at tick 1050.
All three clients, including the opposing player, observed the same Warthog
handle and both seats at both crossings. All exited zero and the pinned cache
held its hash. Physical boarding and a shared guest-driven crossing remain open.

### Five-region Rat Race connector and cross-runtime traversal

Spark's reviewed CE609 cache SHA-256
`b88e52011c8cd513b5f5f9741a0fa49f67e8bf8c9b6940ebd3c26adb278d4bfe`
was hosted by Forerunner Windows x64 SHA-256
`87606ab108d796632914925d6e22d25f2c7ffbe4aeca0fef3df3f31732270d51`.
The Chupathingy guest used SHA-256
`c16096a841422b2597a3c5f26d8b16be7dd6a2914f678c7ea4b36e31ec3a3050`.
On private OpenCE 44, one debug placement staged the host at the approach.
Normal mouse and keyboard input took the host through the tunnel from
(-17.568, 65.034, -17.752) to Rat Race (-62.831, 143.432, -17.752)
and back to Timberland (-16.071, 62.437, -17.752) in the same session.
The peers matched 42 samples in each direction. Maximum differences were
.1245 outbound and .0458 returning. Both exited zero and the exact map
and executables held their hashes. This verifies infantry traversal on a
single resident BSP with the legacy guest. Vehicle clearance, AI and
further regions remain open.

### Six-region Wizard traversal with a guest in Timberland

Spark revision `68392cd` adds Wizard to the same resident BSP. Exact CE609
map SHA-256 `ee2c7b1fc264cdba0f55233ff0b77dffd5455dfdbd079034e3caa489cd982995`.
Forerunner Windows x64 SHA-256
`cb1063473de096a952c83f32fa946efb08d0e70ab2678af8ece0d91e957d6f4b`
hosted private OpenCE 44 with Chupathingy Windows x64 SHA-256
`c16096a841422b2597a3c5f26d8b16be7dd6a2914f678c7ea4b36e31ec3a3050`.
One debug placement staged the host at the approach. Normal input walked
from (-39.828, -58.680, -12.437) to Wizard (-82.796, -133.306, -12.437)
at tick 1290 and back to Timberland (-38.073, -56.251, -12.672) at tick 2550.
The guest independently moved 6.75 units in Timberland and stayed there
through the host's crossing. The peers matched 40 position samples in each
direction. Maximum differences were .0278 outbound and .0386 returning.
Both runtimes exited zero, the map and binaries retained their hashes, and
the disposable staging was removed. The run covers native infantry and
simultaneous player locations on one resident BSP. Wizard vehicle routes, AI,
objectives and independent play across multiple active BSPs remain open.

### Seven-region Wizard–Longest round trip and reverse hosting

The exact Spark CE609 cache SHA-256
`1d05ce00c146f6f7a7d7ba0cb0bbe714ba86eda792455ca9d04884f925f08705`
ran on a Forerunner Windows x64 host SHA-256
`374fdf0073a9ec65f43d2279c6606ade9bfd3e0929a923aefc4f3bb0dbee2c60`
with Chupathingy guest `c16096a8` at private OpenCE 44. One debug placement
staged the host inside the Wizard approach. Native keyboard and mouse
input then took the host from (-98.565, -130.385, -12.437) to Longest
(-171.694, -130.385, -12.437) at tick 1080 and back to Wizard
(-95.857, -130.024, -12.437) at tick 2160. The guest moved 6.64 world
units independently in Timberland. Across 33 outbound and 34 return
samples the maximum peer position differences were 0.119 and 0.1231.
Both runtimes exited zero, the map and binaries retained their hashes,
and disposable data was removed.

A separate private session reversed the host and guest using the same map
and executables. Both players spawned and moved under native input, with
maximum peer position difference 0.075 across five matched ticks per player.
This verifies reverse hosting and movement in Timberland. The round trip
verifies the Longest infantry route on one resident BSP. Vehicle traversal,
regional AI and objectives, and simultaneous play across native BSPs remain open.

### Eight-region Rat Race–Chill Out round trip

Spark's CE609 cache SHA-256
`d2f2b2e6c0da30120d323d86efb459182d783d547b3084f2386456a0fb12bac3`
ran on Forerunner Windows x64
`6f3129053c03d645d8b79b9efe4ac1dda5c65ca07aabff02441d62659b51074e`
with Chupathingy guest `c16096a8`, using private OpenCE 44. One debug
placement staged the host in the approach. Normal input walked from
(-72.714, 168.469, -14.731) to Chill Out (-111.225, 235.179, -14.731)
at tick 1170 and back to Rat Race (-71.215, 165.872, -14.731) at tick 2310.
The guest moved independently by 6.75 units in Timberland. The peers
matched 36 position samples each way with maximum deltas of 0.1165 and
0.1223 world units. Both exited zero and the tested map and executables
retained their hashes. This proves infantry traversal on one resident BSP.

### Nine-region MegaWorld cache targets

Spark's nine-region CE609 export uses 16.92 of 23 MiB for tag and BSP data.
Keep CE609 as the default. The optional Forerunner target uses the existing
MCC13 format and 64 MiB tag window. Build the same authored scenario with
Invader's `-g gbx-custom` for the legacy target or `-g mcc-cea -T 64M`
for the expanded target. Keep each target's native resource banks and
package identity. Report actual tag data, largest resident BSP, remaining tag
headroom and raw resource bytes separately. [Invader compiler options](https://github.com/SnowyMouse/invader/blob/696830ff80af227e84e7237c2ef26eb2301ed110/src/build/build.cpp#L1574)

Spark can match the cache header version to each selected runtime's
`map_targets`. For expanded builds require the advertised
`maps.mcc13_tag_space_64m.v1` feature, or specific verified support recorded
for an upstream runtime without Spark discovery. Treat unknown runtimes as
unverified. Forerunner opens the result through its existing MCC catalog and
package path, including existing multiplayer package checks. The private
`game.start_map` entry remains CE609-only. Spark already exports both targets and reports their compiled budgets.
Its existing compatibility display still needs to consume the selected
Forerunner executable's discovery metadata.

OpenCE retains its linked CE609 arena and refuses maps requiring OpenSauce
memory upgrades. OpenSauce declares those upgrades in its Yelo header.
Chimera's CE map caching and Large Address Aware check retain the CE tag base.
Forerunner reuses the inherited Chupathingy MCC loader with its independent
window at `0x50000000` for this cache, clear of the legacy CE window and
conversion scratch. [OpenCE loader](https://github.com/wxm0394/OpenCE/blob/main/docs/custom_edition_caches.md) · [OpenSauce header](https://github.com/HaloMods/OpenSauce/blob/40c7715a82a3b438d5e04f4f5e5a9edeb59e2618/OpenSauce/shared/Include/YeloLib/Halo1/cache/cache_files_structures_yelo.hpp#L59-L71) · [Chimera loader](https://github.com/SnowyMouse/chimera/blob/e0b13af4d02a6b8b36191e585e7885cd6a2ef878/src/chimera/map_loading/map_loading.cpp#L203-L228) · [Chupathingy loader](https://github.com/ChupathingyCE/chupathingyce/blob/fa752834f2528e6b9021812d1dcf4a7aa08ad00f/port/linux/game/mcc_cache.c)

Invader's nine-region MCC13 build used 11.58 MiB of tag and BSP data.
A disposable pressure fixture retained the same geometry and added valid
native Unicode tag data. Its SHA-256 was
`078bc6c779f9dfcb8ac415f4e32e879689c3c2032ab2288992465e85e3f1b86d`.
Invader and independent readback verified CRC `784bc2cf`, 24.05 MiB of tag
data, 4.99 MiB of resident BSP data, and 29.04 MiB combined. Both Forerunner
Windows x64 peers, executable SHA-256
`4a4474d5a5361f2f5f12e38984eaa97d0369e6ada451340f238c4e5fad50a23d`,
matched the cache identity on private OpenCE 43 and loaded 25,647,424 runtime
tag bytes. Both independently moved 4.124 world units. Four matched samples
per player differed by at most 0.075 units, and both processes exited zero.

The first native load exposed insufficient space in the inherited MCC
checkpoint tail for 256 script stacks at 4 KiB. MCC now retains 4 KiB when
it fits and falls back to 3,456 bytes each when it needs room. The session
retains 256 threads, the existing checkpoint slots, and the same native
bounds checks. CE state and on-wire layout are unchanged. A focused test
covered capture, restore, overflow rejection and cleanup. Native proof is
Windows x64 on one resident BSP. Independent active BSPs, regional AI,
vehicles and objectives require separate gameplay acceptance.

### Nine-region CE609 and guest-driven traversal

The original nine-region cache `548e3c1c99b2032e7fbd72204b900139cee829d94bbe1e9978e0f343240a4c1c`
ran on the same Windows x64 Forerunner binary above with Chupathingy guest
`c16096a841422b2597a3c5f26d8b16be7dd6a2914f678c7ea4b36e31ec3a3050`
at private OpenCE 44. After one debug placement at the Beaver Creek approach,
normal input took the host to Prisoner at (569.760, -34.512, -18.326) and
back to Beaver Creek at (487.542, -34.512, -18.206). The guest moved 6.75
units independently in Timberland. Outbound and return position differences
were at most 0.044 and 0.032 units across 36 and 37 samples. Both peers
exited zero and the pinned cache was unchanged.

On the preserved Timberland–Sidewinder road CE609 cache
`ce043863cd80e26ce0b95f5449a33d80c6e796909f90ce1ae668bae817a993c2`
with Forerunner Windows x64 `6f312905`, a guest driver and host passenger
physically boarded the same Warthog using E in a private three-process
session. The guest drove from the authored Sidewinder approach (x=68.100)
to Timberland (x=37.218) and back to x=68.628 in Sidewinder. Seat and vehicle
identity persisted, and the passenger and observer agreed within 0.082 world
units. The run ended before the harness's farther x=70 destination.

### Danger Canyon and safe MCC region loading

Spark's separate nine-region CE609 branch
`7d2039da75783b138b9b796cf1adf84535f98d572a6eac49cb28ea1d72960445`
adds Danger Canyon to the Chill Out branch. Windows x64 Forerunner
`3b8aef6c2dd37168f04f5f680ba8c762d17aefea4dc83a1db708ae0c7f4ab26d`
and Chupathingy `c16096a8` verified both directions in separate private
OpenCE 44 sessions. After staging at each tunnel approach, native input
reached Danger Canyon (-151.361, 320.591, -17.122) and Chill Out
(-108.813, 246.899, -17.113). On the return leg, 39 matched ticks
differed by at most 0.0492 world units. The Chupathingy guest also moved
independently in Timberland during the outbound run. This branch and the
Prisoner branch remain separate, each with one resident BSP.

MCC now checks a destination's geometry header and native visibility budget
before disconnecting the current BSP. Focused regressions cover oversized
cluster counts, incomplete visibility, short reads and out-of-range report
offsets. The checks leave active players, vehicles, equipment, and visibility
state intact. On the same Windows binary, two native peers loaded the
full-audio MCC13 cache `126491917a68c50e14a9465a8b066c40f56a9cd3eb933b66f5ee188f36ebedb1`
and synchronized regular movement within 0.0765 world units under private
OpenCE 43. This exercises valid native loading. Separate active native BSPs
and a live transition into a deliberately invalid second BSP remain unverified.

### Combined ten-region CE609 and MCC lobby admission

Spark revision `3dfc79e` supplied the combined Prisoner and Danger Canyon
CE609 map `34776aa34e9ee7c0dd280ae4af3431e249739bd8fef903359bb73c98debc3c62`.
On Windows x64 Forerunner `3b8aef6c2dd37168f04f5f680ba8c762d17aefea4dc83a1db708ae0c7f4ab26d`
with Chupathingy `c16096a8` over private OpenCE 44, the host walked from
Chill Out to Danger Canyon (-151.361, 320.592, -17.122) at host tick 1230
and back to Chill Out (-109.706, 248.444, -17.113) at tick 2490 in the same
session. One debug placement staged the host at the start. The guest moved
6.675 units independently in Timberland and saw both ends of the crossing.
Both games exited zero. The runner's final synchronization check missed its
sample minimum because the clocks sampled different ticks. Four matched
outbound ticks differed by at most 0.0264 units. This proves the route
round trip on one BSP and leaves full tick-matched verification open.

Both synchronous and asynchronous MCC admission now share the native
initial BSP geometry budget check. On Windows x64
`590fb19d2ba9062a18d7ba28c1954f6109f13f1f2825c4deb3ef0966287d4b2b`,
two Forerunner peers admitted Spark's nine-region MCC13
`04940e2873e3b707c69fa9bcdb3e7e129d5e1e4937bfd4dc39d31add55e63f9c`,
loaded 3,332 tags each and moved with a maximum measured position delta
of 0.15 units on private OpenCE 43. A disposable copy with an empty first
BSP cluster count was rejected by the runtime and the lobby kept Carousel.
This checks native admission scope. Independent active BSPs, region AI and
objective persistence remain open.

MCC13 BSP validation clears the previous BSP's claims in bulk and keeps
the base tag claims, including a shared boundary byte. The native Windows
x64 build `4e036f6165a33b972f3ddfb9e53005e749fc2928dfbd08a2975afab23baa9643`
loaded the same nine-region MCC13 map on two private peers. Both loaded
3,332 tags and 7,840,752 tag bytes and synchronized movement within 0.15
units. Both exited zero. The median bitmap reset in 12 paired samples
decreased from 32.144 ms to 0.102 ms for a 64 MiB arena. This measures
bitmap reset time only. The 134 focused tests include repeated BSP loads
and preservation of an unaligned base-tag boundary.

During native BSP switches, the engine relinks world objects before
evaluating activation with the destination's player visibility. The
existing activation pass runs once even when both regions have the same
numeric visibility mask. Focused checks verified that a visible projectile
survives, an off-screen disposable object is removed, and an attached
child stays attached. The native D40 smoke used stock cache
`7c6dccc9f928f4a44db992c4d6f5fa6191e9cfd331bb0d0b631540f0fc9bc796`
and Windows x64 `e7aa4d212e8be16fc9824d3e6b8d62fc8b6152c047067abc1bda7ec6fdd1b397`.
After staging, normal input crossed BSP 1 to 0. Both OpenCE 44 peers
reported BSP 0 by tick 366 and exited zero. This verifies the native
transition. Projectile survival during live replicated play remains
unverified.

### Actor snapshots and cross-region combat

After a co-op client receives the host's BSP announcement, periodic actor
and player snapshots wait until that BSP is loaded. The receiver rejects
movement packets older than the transition tick and releases held actor
controls from the previous BSP. This uses the existing message timestamps
and sequence handling, with no wire layout change. A repeated announcement
leaves the transition tick intact. The focused BSP and actor suites passed
79 checks.

Native Windows x64 `83a626f4255539a8635b7af008ec94caad100a1b692b694370c1071e8df1da97`
ran two private OpenCE 44 peers on stock D40. After staging, normal input
crossed BSP 1 to 0. Both peers reported BSP 0 by tick 366 and exited zero.
The native run verifies the transition. The delayed actor packet case was
verified with the focused harness.

On the same Forerunner build and the reviewed CE609 road map `ce043863`,
normal mouse aim and trigger input fired from (64.000, 34.774, -21.075)
on the Sidewinder approach to the Chupathingy `c16096a8` guest at
(45.712, 34.774, -21.075) on the Timberland approach. Both peers reported
the guest's shield at 0.67 and body at 1.00 at tick 90. Both exited zero.
This verifies native projectile damage between authored regions sharing
one resident BSP. Projectile/actor persistence across separately active
BSPs and authored regional AI remain open.

### Twelve-region Hang 'Em High tunnel

Spark `7f6d360` added the Hang 'Em High infantry route to the connected BSP.
The pinned native caches are CE609
`da222bf1524ec69cc53eae8bd6fe50f63e1ffd830966efd3655eaace6c298388`
and MCC13 `040b5da5de0a0e0bb7e03285933eaf807940e3c83c1fa1b76955d2982202d691`.
The connection links (-178.228, 377.958, -23.882) to
(-201.227, 463.793, -23.882). Authored width is 2.0 and clearance is 1.6.
Both formats share one resident BSP.

On private OpenCE 44, Windows x64 Forerunner
`512878b3aeab5d3088ae6718758dcf3ddd5e7c32beab8bc5bc244c9ed977e93a`
and Chupathingy `c16096a8` verified CE609 outbound to
(-201.942, 466.455, -23.882), with the guest independently moving 6.750 units
in Timberland. A separate reverse run on Forerunner
`09c9f353582a4269dabc612e280ed4d7d4a05b7fdc8deb611518c7543e0cf0f1`
and the same Chupathingy build reached (-176.614, 376.822, -24.073).
The reverse match reached 44 exact ticks with at most 0.0444 unit difference.

Two Forerunner peers on the latter executable walked the MCC13 route
both ways in one session. The host reached (-201.960, 466.524, -23.882)
and returned to (-176.838, 377.279, -24.121). Host–guest position difference
was at most 0.1279 outbound over 42 samples and 0.0488 returning over 45
samples. The other player moved independently in Timberland. Both native
processes exited zero and disposable copies retained their pinned hashes.
These runs verify infantry travel on the Hang 'Em High connector.
Independently resident BSPs, regional AI/objectives and later regions remain open.

The BSP reconnect path now preserves alternate markerless arrivals to the
same destination. It checks the actual vehicle root or infantry body and
keeps **marked** or differently routed players on the existing gather path.
The same check keeps AI passengers aboard equivalent routes. Both new
infantry and vehicle cases reproduced displacement before the fix. All 54
focused BSP checks passed afterward.

A resting pickup's support surface now belongs to its original BSP.
The native item update wakes mobile pickups that reconnect inside a
different BSP and leaves off-region and immobile pickups in place. All
55 BSP checks passed, including a test that previously read another
BSP's plane. A private two-peer stock D40 smoke at executable SHA-256
`4bd6e157368068a88e7e2b556363bfa933ba9c7fb5b613f96d6fc0e6700a13e7`
placed an unowned assault rifle at (-22.23, 22.22, 0.01) in BSP 1.
Both peers loaded BSP 0 at tick 195 and returned to BSP 1. They saw the
same unowned, resting rifle in slot 5 at (-22.15, 22.32, 0.01) and exited
zero. This used scripted BSP selection to exercise the native unload
and reload, since the manual input fixture did not reach the switch.
Live seated AI traversal remains open.

### Independent BSP residency

Use a native two-BSP scenario for the first separate-region handoff. Keep
each BSP's clusters, portals, collision, lighting, weather and sound data.
Use matching world coordinates at the road or foot seam and paired native
markerless switch volumes. Verify the two CE609 and MCC13 builds independently.
H1A Tool can import and compile native content, and Invader documents its
separate cache builder and the newer CE Tool's BSP generation improvements.
[H1A mod tools](https://learn.microsoft.com/en-us/halo-master-chief-collection/general/overview-tools)
[Invader](https://github.com/SnowyMouse/invader/blob/master/README.md#the-hek-says-my-bitmap-tag-is-too-large-when-opening)

The stock D40 cache above links BSP 0 at [0x814fa000, 0x819a6000) and
BSP 1 at [0x8121b800, 0x819a6000). Both cannot occupy those linked
addresses at once. The native runtime first needs disjoint or validated
relocated BSP ranges. The MCC geometry adapter now owns buffers by BSP
handle. Two disjoint linked ranges can hold geometry at the same time.
The geometry adapter refuses overlapping runtime ranges before copying
bytes. Failed conversion releases only the candidate's buffers. Cold
switches release the exact active cache reference and any retained neighbor.
The validator retains claims of other live BSPs and clears only the released
or rejected range.
Geometry checks are restricted to the BSP handle being walked. Model
allocations retain their shared owner. The native collision trees accept
explicit BSP pointers. Higher-level location,
collision, AI, visibility and network snapshots still use the currently
selected BSP. Independent occupancy requires region ownership through
those callers and per-peer replication. The legacy global switch path
remains available. The present 16-BSP scenario ceiling also requires
explicit budgeting if authoring maps one region per BSP.

The geometry verifier covers two disjoint native cache references,
overlap refusal, partial buffer failure, and unload/reload without
touching another BSP or shared model buffers. The focused Windows suite
passed 49 checks. The optional read-only test also converted the exact
twelve-region MCC13 above. Two native Windows peers on executable
`26f8d6c01c716862c7110f074b165dbade425a80a856be865073324178cb9dad`
loaded 3,517 tags and 8,019,232 tag bytes each from that same cache.
They synchronized native movement within 0.15 units and exited zero.
This native smoke exercised a single active BSP.

The next Windows build, SHA-256
`28857e676f93dd9aaf86aa2ebf4d17eb60a71fe4f1a72eeb6483d9daf5544fb8`,
passed 184 focused schema and geometry checks with one POSIX-only skip.
The tests cover adjacent BSPs sharing a bitmap byte, failed validation,
release and reload, and foreign geometry rejection. Two Windows peers
loaded the same twelve-region MCC13 revision and synchronized movement
within 0.075 units. Stock MCC D40
`51b4a64993e4f2a02a6c942af628270edb3abb42e69fc25c42d10efb710940e0`
passed geometry readback, but the private lobby stayed on Carousel because
the installed assets lacked a compatible `sounds.map`. That prevented
native MCC D40 switch acceptance.

The native scenario switch now prepares and validates a disjoint MCC13
destination before disconnecting the active BSP. Refusal restores the
clock and cancels the pending gather while the active scene and cache
header remain intact. After successful preparation, the switch publishes
the destination and retains the former BSP's validated geometry with its tag
binding cleared. Returning to it reuses geometry and validation claims.
Preparing a third BSP evicts the inactive neighbor first. Cold switches,
checkpoint reloads and map teardown release both.

The Windows x64 executable
`de6da5376ecb3ae01935c4d7c5cf5eb83053434f8d4890baa75166d89278990a`
passed 59 focused switch tests. These cover the preparation transaction,
early refusal or budget rejection, and overlapping fallback. Two native
D40 Xbox-cache peers agreed on BSP 1 at tick 45, BSP 0 at tick 60 and
BSP 1 at tick 75 using scripted switches. Separate twelve-region native
smokes verified synchronized movement on MCC13 within 0.049 units and
on CE609 with Chupathingy within 0.075 units. All peers exited zero.
The disjoint preparation path was exercised by the focused harness.

Windows x64 executable
`788d90d2836451a1fecf913a09f3157cb6a06031863cd11797cd29ee58ca55a4`
passed 60 focused switch checks, including cached return, third-region
eviction, cold cleanup and refusal while the active scene remains bound.
Two isolated native peers switched stock Xbox D40 in order 1, 0, 1.
Two peers also loaded and moved on the exact twelve-region MCC13 cache
`040b5da5de0a0e0bb7e03285933eaf807940e3c83c1fa1b76955d2982202d691`,
with observed position differences up to 0.075 units. All peers exited zero.
Disjoint neighbor reuse is covered by the harness. Each native session
still simulated one selected BSP at a time.

On a prepared markerless co-op crossing, the host now tests crossing
players and vehicle roots against the destination's native leaf and cluster
data before disconnecting the source. An invalid destination keeps the source
active and the validated neighbor cached for retry. Windows x64 executable
`104debd5eaaaced06af656c837957d5e7a3668d10eeb52ec4b4b44df3316e95c`
compiled and passed 61 focused switch checks. Two native stock D40 peers
agreed on scripted BSP 1, 0 and 1 and exited zero. The physical approach
smoke stalled at the same host coordinates on this build and on the
comparison with the previous scenario and player objects. That run
did not exercise a live prepared MCC13 seam.

The MCC13 loader can now prepare an overlapping neighbor in the unused
arena space above converted static tags and below the lowest linked BSP.
Geometry conversion uses the on-disk offsets and schema validation
translates BSP-local block/data addresses. Actual resident addresses own
buffers and validation claims. The cache identity and references remain
unchanged. When there is insufficient space or optional relocation fails,
the original cold switch is available. CE609 keeps its existing loader.

The MCC13 loader can now use free space on either side of the active and
destination-linked windows. It still reserves the converted static tags
and rejects overlap with live geometry. A cold markerless MCC13 co-op
switch tests the crossing roots against the loaded destination before
publishing it. If a root misses, native rollback reloads the source
and cancels destination gathering. The occupied vehicle stays attached.
Windows x64 compiled. The 69 BSP and four geometry checks covered both
relocation directions, full arenas and cold rollback/retry. On executable
`5374d94e06bf5b844e84280cb3ed54a2a7664c47c86f22afbe1b08bf884bb5ad`,
two private peers joined the existing synthetic MCC13 two-BSP fixture
`154074ed26aee9938095d59a65ee9e6657fd9a21a419295055e47a84fa9be43e`.
The host loaded BSP 1 at tick 45. Both peers exited normally. That native
test exercised the loader. Cold physical crossing is covered by the focused
harness and still needs a playable authored route.

Windows x64 executable
`66691caaf73c98c54ccef1d530c73b2a86cb732da860106bd0f72b823cd44783`
passed 244 focused switch, conversion and validation checks with one
platform-only skip. The fixtures exercised relocation of an overlapping
BSP, retained return, insufficient space, and pointer bounds. Two private
native peers loaded and moved on the exact twelve-region MCC13 cache, with
observed position differences up to 0.075 units and zero exit statuses.
That native map exercises a single BSP.

Spark revision `9d89bfc` compiled both Chiron corridor BSPs with native
two-way switch volumes in CE609 and MCC13. The probe used overlapping BSP
windows and its outputs were disposable. The next runtime handoff needs
the lit, playable caches for physical traversal. The ordinary overlapping
MCC13 export can exercise native relocation if its converted static tags and
both BSPs fit the arena. Simultaneous regional simulation remains open.

The existing MCC13 pressure fixture loaded 25,647,424 tag bytes on two
native Forerunner peers, while the twelve-region CE609 and MCC13 traversal
is verified separately above. Both currently exercise one resident BSP.

The Chiron gameplay handoff uses Spark's retained MCC13 export and its
dependency receipt from the existing Guilty Spark traversal handoff.
The receipt must identify the cache path, SHA-256, Spark revision and
required resource banks with their hashes, or record that resources are
self-contained. Preserve that exact package through Forerunner's
forward and return traversal acceptance. The verified Normal Build
`73aadc284e134ed1e11c01eadaf5623194ff5c2875dfc3bd34dbdd7f5c666fdc`
was retired after Spark's readback. The retained package is recorded below.

#### Regional native teleporter flags

Spark's reviewed base and Chiron source have 284 native netgame flags.
Forerunner now admits up to 4,096 rows in MCC13's existing
`scenario.netgame_flags` block. Larger blocks are rejected. CE609 keeps the
classic 200-row limit. Forerunner discovery reports MCC13's `max_netgame_flags`
as 4,096 when that target is available. Spark can consume the optional value
alongside the target's Invader schema. Where a runtime omits the field, retain
its reviewed schema budget.

`scenario_uses_regional_gameplay` scopes native teleporters, starts, pickup
pads and vehicle homes to MCC13 multiplayer scenarios with multiple native BSP
references, regardless of flag count. Teleporter endpoints need to belong to
exactly one BSP. Reuse channels in separate BSPs with a unique target for each
local channel. The host and incoming multiplayer corrections use the same
lookup. CE609, campaign and single-BSP maps retain global teleporter pairing.

The start (256), equipment (200) and BSP (16) ceilings are scenario-wide.
CTF has two team slots, Race has 32 checkpoints, King has 64 hill IDs and
Oddball supports 16 balls. These are game-mode limits separate from the
flag block's capacity. Native desktop and constrained profiles have 8,192
and 2,048 object slots respectively. Network object allocation shares datum
indices across peers and uses the upper half for local effects. Per-BSP
placement does not increase these budgets.

Windows x64 `e40fdbf5da349e3b4d1a7b2482942d1f6ee8a7ef14ce47ffa2798b498a24370b`
passed 200 focused checks. A disposable single-BSP copy of Spark's twelve-region
MCC13 cache had 284 flags and Invader-checked CRC `900619c6`. Both private
native peers reported slot 0 at (68.049, 34.124, -21.075) on tick 90, then
(55.250, 35.424, -21.075) on tick 120 after using rows 282 and 283 (zero-based).
Both processes exited normally. The runtime trace captured the player after
they continued moving, past the smoke wrapper's target-radius assertion.
The fixture SHA-256 was
`5d928df40009586bf56a89ab34e3f8ac78d7b4fb911ce45cc32385225dd9b5a8`.
The revised gate passed the BSP harness with five flags, including reused
regional channels, and kept global pairing in the CE, single-BSP MCC and
campaign cases. The BSP and vehicle harnesses passed 162 checks. The Windows
x64 discovery check reported 4,096 for MCC13 without writing files. Spark
now records a complete Chiron pickup import in the retired collision
probes CE609 `f87759f9` and MCC13 `bcea054d`. Readback found all 42 donor
equipment placements, the all-powerups collection, and unique collision
membership for the MCC13 endpoints. Spark `94967e2` reports lit diagnostics
CE609 `bb310615` and MCC13 `64e3feca`. The next handoff is one retained lit
MCC13 candidate with those placements and all 127 reviewed regional starts.

Native MCC13 multiplayer maps with multiple BSPs and authored switch rows
now share the campaign BSP announcement and input acknowledgment. Existing
packet layouts are unchanged. All machines must negotiate Delta
`regional_bsp` before the host tries a physical crossing. Use markerless
native switches and a collision overlap large enough for every living
player and occupied vehicle root. The host checks that all roots are inside
and uses the existing crossing preflight. It leaves players seated and
does not gather them by teleport. Only the host can schedule the switch.
Regional matches use the seam triggers for live transitions. Campaign and
ordinary single-BSP multiplayer keep their original path. A room without
the capability can play BSP 0. A peer without support cannot stay in a
match already on another BSP.

Windows x64 executable
`58f0775de2da4ff8ee657ec7a2b323b77363f199132f7eae43db34f63b0e8f48`
passed 71 BSP, 14 input and two Delta registry checks. Two private native
Delta peers on the disposable two-BSP fixture
`9341679f0fd4d6e248d5055c87512997a588c37f3bec7bacd85677b96bb519be`
stayed in BSP 0 with one player outside the seam. Automated keyboard
movement brought that player into it. Both peers reported BSP 1 at tick
165 and agreed on the new and stationary positions. The OpenCE-protocol
control with both players inside the seam stayed in BSP 0. Both runs
exited normally.

For a regional late join, the host sends its BSP reliably before the initial
object snapshot. A client with negotiated support waits for that announcement
and the local BSP load before applying spatial snapshots or predicting
movement. The host waits for the existing input acknowledgment before
spawning the newcomer in a noninitial BSP. No packet layout changed.
Windows x64 executable
`6494a6e3c9713ec472a38087b4340fbfe587e279950f364c14d577ae62080863`
passed 86 focused BSP and queue checks. On disposable fixture
`9f126a7619ef699e28b2e8f3c98d3cdd1d94881b06b63bb64792d9cbe8a3ad59`,
a third private Delta peer loaded BSP 1 at tick 496 and all peers agreed on
its spawn at tick 540. After it left, a replacement reused slot 2 in BSP 1
and its normal aim input reached the host at tick 1080. The two original
players stayed in the match. All processes exited normally. Live Chiron
and independent simulation of occupied BSPs remain open.

On a negotiated regional switch, the host applies the existing BSP
announcement tick to incoming hit reports. Reports stamped in the old
BSP or on the switch tick are skipped before collision and moderation
checks. The shooter must have acknowledged the active BSP before later
reports resume. Resends and late-join announcements leave the room's
boundary in place. The packet layout is unchanged. Windows x64 executable
`06277526476e0c3accfb98eaa9d6d8359c43b86c04009a1e62a6e346f53e0d26`
compiled and passed 75 focused BSP checks. This verifies report
admission in the harness.

On a real regional switch the host also clears its position and sight
history before recording the first tick in the new BSP. This prevents
current hit reports from rewinding into the previous collision world.
Periodic announcements, late joins and classic co-op preserve their
history. Existing packet layouts are unchanged. Windows x64 executable
`4a8c78cc03b3de0312a1095a915edaca350d2966e3bd777538aa0a9093ec375a`
passed 106 BSP and stale damage checks and 117 checkpoint checks.
On the exact Chiron cache below, both native peers loaded BSP 1 at tick
896. Normal guest fire damaged the host after the switch. The host
recorded three guest reports applied with zero refusals. Both peers
exited zero. The regression harness checks rejection of older spatial
history. Independent regional combat remains open.

Forerunner now records the simulation BSP for each host combat history sample.
Rewind checks both BSP and the latest entry tick before reusing positions, vehicle
contacts or sight. Returning to the same BSP starts a fresh visit. This replaces
clearing both history rings at every switch. Windows x64
`2b74c044228ecf83202f2e337db0469c8f0209a1358a9bb5e4d7ec2ed28dd109`
passed 94 focused checks. On the exact fourteen-region cache, two private peers
traversed BSP 0 → 2 → 0 and the host applied three fresh guest hits after returning,
with zero refusals in that check. Both peers exited normally.

For a warm regional multiplayer crossing, the host checks each unseated
biped's native collision capsule against the source and destination BSPs
using their own breakable-surface state. A destination-only obstruction
refuses the switch. Seated players continue to use their vehicle's native
point membership.

Windows x64 executable
`7192848e8fb89ff693041fbfde4e1d6664c544e447cf992cc832fee0a493a14d`
passed 82 focused BSP and object collision checks. Two private Delta peers
on disposable MCC13 fixture
`9bfbf4b97b5cbb9a5ce56f2a8df68fcf5262d5f6477d9646d8198bd5c9e724d6`
saw the same Warthog and driver seat at tick 60 in BSP 0, entered BSP 1 at
tick 75 and still agreed on the vehicle and seat at ticks 90 and 120. The
driver was staged by the native diagnostic and both processes exited
normally. Both BSPs used the same geometry. Guest driving, a live Chiron
crossing and combat in independently occupied BSPs remain open.

When only one BSP fits, the host measures each on-foot unit's source
capsule before releasing the old BSP. It compares that result against
the destination before activating it. A new obstruction restores the
source through the native rollback. Shared terrain contact still permits
the switch.

Windows x64 executable
`407d38d80f0526ec6cb19636729291c7ab4741ff0f1f4b7426e4dd49969375d6`
passed 74 focused BSP checks, including obstruction rollback and retry
with shared contact. Two private Delta peers loaded the disposable MCC13
cache `1b9187fed53e96ba19230497cca71317e74e426dca9a45c823ab70b03f8c9f8a`.
Its 58,408,480 runtime tag bytes and two 7,334,552-byte BSP windows could
fit only one window in 64 MiB. Both peers switched at tick 75, matched both
live players at tick 90 and agreed on the guest's keyboard-driven position
at tick 150. Both BSPs used the same source geometry. Both peers exited
normally. The separate guest vehicle approach showed no native seat action
at the sampled position 1.153 units from the driver's entrance. Physical
guest boarding and driving across the seam remain open.

Occupied vehicle roots use the native physics mass points against both
BSPs. Existing contact can carry across the seam. A destination-only
contact at any individual mass point refuses the shared switch. A cold
load keeps the source contact mask and exact root identity through the
existing rollback path. The Windows x64 executable
`cbf42951d423987d11e45e87371cf81194d3b56205719cdebe380c53515eb888`
compiled and passed 76 focused checks, including warm refusal and cold
rollback with a separate new mass-point contact.

Spark's retained MCC13 Chiron handoff is
`%TEMP%/GuiltySpark-Traversal-Handoff/thirteen-mcc13-YTz6Uc/ready`.
Its self-contained `gsmegaroad.map` is 118,840,788 bytes with SHA-256
`ba57480a30c5121da3be111acc7d605a277b6809f5604b6e182bbf97c3f205af`.
Forerunner retained matching copies and both authoring receipts under
`build/native-mcc-chiron-ba57480a`. Both peers loaded 3,890 tags and
9,293,888 runtime tag bytes. No external resource banks were needed.

The previous seam checks rejected both players inside the outbound
trigger. Native diagnostics found that the destination capsule contact
disappeared with a 0.001-world-unit upward probe. Regional clearance
now ignores contacts up to that depth only on near-horizontal support.
Walls, ceilings and deeper contacts still count. The normal collision
selector and the native position and trigger tests retain their rules.
Windows x64 executable
`73a9e7d61fbad6a285350f27879a938c4842ec9ca24f4671b3948a2228be04f3`
passed 76 focused BSP checks and five contact selection checks.

Two isolated native Delta peers used the exact cache on that executable.
Debug staging placed them on the base approach. Normal keyboard
movement triggered both switches without moving either player across
a seam by command. The host and guest reported BSP 1 at ticks 889 and
896, and BSP 0 again at tick 1281 on both. Both exited zero. This is
automated native gameplay with a shared active BSP. Independent regional
occupancy and human acceptance remain open.

The same exact Chiron map passed the guest-driven Warthog round trip on
Windows x64 `6406abe7424c194acfca4a9a367b63f55d03fa6dd8d2c5f89d3142773bb23da7`.
The host used normal E boarding, then staged the map's original Warthog
entirely within BSP 0. The guest used E to take the driver seat and
drove the native vehicle through both switch volumes. Both peers saw
BSP 1 at tick 1512. The host and guest saw BSP 0 again at ticks 1862 and
1869. Both retained Warthog handle `e26f0000` with the guest in seat 0
and exited zero. The host walked to each shared switch. The map,
same-build executable and native logs are retained under
`build/native-mcc-chiron-ba57480a`. Passenger and independent
regional acceptance remain open.

Spark's fourteen-region Derelict cache is
`%TEMP%/GuiltySpark-Traversal-Handoff/fourteen-mcc13-ubQ0YV/ready`.
The self-contained MCC13 map has SHA-256
`71fc164876cafcece4de49c2b0563c79c6f772abfff308c7dc61d5a411b79e2b`.
Its three original BSP slots and four switch rows were left intact.
Using the same Chiron executable, the private peers stepped through
the new authored switch to BSP 2 at tick 889. The host and guest
reported BSP 0 again at ticks 1274 and 1281. Both exited zero. The
same debug approach staging and normal keyboard movement apply.
Retained copies and receipts are in `build/native-mcc-derelict-71fc1648`.

Regional MCC13 multiplayer scenarios with three or more BSPs can keep the
active BSP and up to two previous BSPs inside the existing 64 MiB tag
window. The loader selects disjoint ranges for retained geometry and can
reuse the oldest window. If there is only room for two BSPs, the loader
uses the existing two-window path. Only the active BSP is bound to native
simulation.

If a disjoint window fits within the existing tag arena, preparation
converts and verifies a new BSP while leaving the active BSP and both
retained neighbors in place. A refused candidate releases its own
geometry. A successful switch can then evict the oldest neighbor to
stay within the existing retention count. The shared-switch fallback
can still reuse an inactive window under a tight budget.

Windows x64 `4b0c7962ce8291b778225963427957ddb6a45755e2d83285c6fe3831e556f29d`
passed 77 focused BSP checks. The fourth-region regression verifies
failed preparation, a successful retry, and final resource release.
The original whole-cache rejection path fails that case. On the exact
fourteen-region cache above, two native peers traveled BSP 0 → 2 → 0 → 1
in one session. Both the Chiron and Derelict crossings used normal
keyboard movement after debug staging within BSP 0. Both peers exited
zero. These checks verify retained geometry and shared switches.
Independent simulation of occupied regions remains open.

Forerunner records which BSP owns loose equipment and empty vehicles when
they leave the active scene. Objects stay with their source BSP even when
the destination has geometry at the same coordinates. Carried gear and
occupied vehicles continue to follow native crossings. This uses the
existing host object synchronization and requires the same Spark cache
format and authoring data.

Windows x64 `ca308c72236b5f380ab8e7232b1506451c58414de8c1abbfba43f65010d3b50d`
passed 78 focused BSP checks. Two private peers on the exact fourteen-region
cache saw a staged rifle disappear from the guest in Derelict and return
at the same position in BSP 0. The host retained the same rifle throughout.
Both processes exited normally.

The MCC13 cache now protects BSPs held by the native simulation within
the existing tag budget. If the next BSP would overwrite held geometry,
the load is refused before the current scene disconnects. The cache can
reuse a free or unheld window after a hold is released. The current
gameplay path holds its shared active BSP. Independent player occupancy
and per-region collision, AI and networking remain open.

Windows x64 `4de0028172d008d142109f223abc3d13e5e4f71828309700065e1dc9c6de5926`
passed 79 focused BSP checks, including the full-cache refusal and retry
with two held neighbors. Two private native peers on the retained
fourteen-region cache crossed BSP 0 → 2 → 0 and both exited normally.

Windows x64 executable
`0ea3d89d42682873e1387da0412f88e92b6d47c040b59bfbda513220b286b57c`
passed 73 focused BSP checks. The cache regression verifies reuse, a tight
budget fallback and unloading all three regions. Two private Delta peers
followed native switches 0 → 1 → 2 → 0 on disposable MCC13 fixture
`136d31a88423758fbbd61012da8fd3587e776d0289987c35e802cb1ff75c4cd4`.
The three BSPs used the same source geometry and linked address. Both
players began in a shared test volume and both processes exited normally.
Independent simulation and Chiron gameplay remain open.

#### Region-bound encounters

MCC13 multiplayer encounters with a valid compiled BSP assignment retain their
actor attachments and remaining activation window during the native global BSP
switch. The existing boarding path carries AI physically seated in a crossing
vehicle. On return, the encounter resumes through native actor activation.
Campaign and unbound encounters keep their native follow paths. The encounter
harness passed 21 checks including the park-and-resume case. Stock Xbox D40
completed BSP 1 → 0 → 1 with two private native peers on Windows x64 executable
`801b16423799bc44775d1492263dddd9be71a22f106dfb60d54a876b9b74ca26`.
That legacy smoke and the focused harness leave authored MCC13 AI combat and
simultaneous squad occupancy open. Host physics, AI updates and general world
snapshots still follow one active BSP.

Native on-foot actors and swarm bodies now use their attached encounter's
authored BSP for collision parking and host damage validation. This prevents a
parked actor from reconnecting into overlapping geometry. Players, riders and
detached actors keep their native crossing behavior. Spark uses the same native
encounter assignment. Windows x64
`ab5fe2d8f79442a60913b38fa47c882c8db11232896c668cb69ba2a39bebfbf5`
passed 101 focused checks. On the retained fourteen-region map, two private peers
crossed into Chiron and the host accepted the guest's hits. Native AI combat
with independent players in different BSPs remains open.

Forerunner can query the active and retained MCC13 geometry by scenario
BSP index without activating the neighbor. Retained lookup is enabled for
MCC13 campaign co-op, where glass refresh uses the native per-BSP fracture
tables. The existing
surface event carries the BSP index. A fracture in a retained region changes
only its collision state and produces effects only in the active region.
Refreshes continue to send both resident regions' fracture state to peers
that missed an update. This uses the existing wire format and requires no
new Spark data. Windows x64 compiled and 66 focused BSP checks passed,
including same-index glass in two regions, resend and eviction.
A private stock Xbox B40 smoke let the native insertion finish and observed
both player slots. Both peers held BSP 4 then BSP 0 across refreshes and
exited normally. The B40 map hash is recorded above. Windows x64 executable
`d757a5c8312c57d2623801ab1aefa3b7304b87864fd815f03797defba86610ae`.
Full simultaneous simulation and native two-BSP fracture playback remain
open.

#### Expanded MCC13 scenario placements

Spark can use the MCC13 discovery fields `max_player_starts` (1,024)
and `max_netgame_equipment` (512). These are validated counts for the
native `scenario.players` and `scenario.netgame_equipment` blocks.
The MCC loader refuses larger blocks before reading their entries.
CE609 and Xbox keep 256 starts and 200 equipment rows. If those fields
are absent, keep the selected runtime's verified limits. Equipment uses
the existing host scheduler, regional ownership and object budget.

Windows x64 `6406abe7424c194acfca4a9a367b63f55d03fa6dd8d2c5f89d3142773bb23da7`
passed 143 MCC validator, 76 BSP and 15 discovery checks. The MCC
validator preserved 563/485 and 1,024/512, rejected 1,025/513 and
verified the original legacy limits. On a separate disposable test
cache `fec0447b87b69b7757553be5053ad4a1c1148ad0deebd12ada28f1e1cc4c8915`,
two private native peers agreed on spawns from rows 562 and 563 and
the same assault rifle from equipment row 485 (object 608 at tick 30).
Both exited zero. The test placed only the high rows needed for proof.
The complete populated 19-region world still needs gameplay and
performance acceptance.

#### Regional native player starts

On multi-BSP MCC13 multiplayer maps, a player start must belong to the
active BSP. Set its native `structure_bsp_reference_index` to the authored
BSP, or use `NONE` for a start classified by its position. Team-spawn
selection and solo preview fallback honor the same rule. A membership probe
just above the floor admits starts and native teleporter markers authored
exactly on a collision plane while retaining their coordinates. Classic
and campaign spawn rules remain unchanged.

Windows x64 `edaf5b56726390297d33760a447e09a66c2ad4b3997cb6e47881c09e0dc27736`
passed all 64 focused BSP cases. Forerunner also loaded a disposable two-BSP
MCC13 layout derived from the verified twelve-region cache. Its two BSPs
shared linked address `0x53901568` and used the same source geometry. Two of
68 starts belonged to BSP 0. Both private peers agreed on those two spawns
at tick 30 and recorded the floor-level regional teleporter at tick 60.
The host activated BSP 1 at tick 75, and both processes exited normally.
The fixture CRC was `481a70fa` and its SHA-256 was
`eca7b415df4fb503b333565b6898c148dd09e8efbed1b292fa6b4a9a6d065d1d`.
This verifies the placement rules and a host switch on the overlapping
layout. Chiron traversal and simultaneous regional occupancy remain open.

#### Regional native equipment pads

The host binds each multiplayer equipment row to the first active MCC13 BSP
that contains its position. A pad in a shared seam keeps its first owner.
The first visit stocks newly bound pads and later restocks use the map's
native timer in that BSP. An unclaimed pickup near its pad keeps its object
identity and avoids age-based cleanup while the BSP is parked. Carried and
displaced items return to the normal ownership and cleanup paths. Single-BSP
and legacy maps keep the original item scheduler.

Windows x64 `99d7d2129607ecbef12ee0f648536cfe85559d01632f61f16ff1db7519fddd39`
passed 65 focused BSP cases. On the disposable two-BSP MCC13 fixture
`a7538d9603877e1cffdc5fcbda8670f4eb2ff9d839eae034d7c0998f0eb83072`, both
native peers reported one unclaimed rifle (slot 552) from tick 30 through
210 despite the one-second interval. The host activated BSP 1 at tick 135.
The out-of-world control row never appeared in either peer's item trace.
Both processes exited normally. Independent log readback verified the
post-switch samples because the wrapper's fixed interval excluded tick 210.
The BSPs in this fixture use the same geometry. First activation in a
distinct region, shared-seam ownership, and carried-item restock are
covered by the focused harness. A live Chiron crossing remains open.

An unclaimed pickup still at its bound pad belongs to that BSP even when
a neighboring BSP contains the same coordinates. On a shared BSP switch,
the host disconnects the parked item from collision clusters and withdraws
its replica using the existing object lifetime messages. It restores that
same host datum when the owning BSP becomes active. Carried and displaced
gear follows ordinary item rules. No authoring field or packet changed.

Windows x64 executable
`2b7ff657738542c3298bb3779c534f4f67bd6fe8d9ea2b3d8a623400ef7aeb24`
passed 73 focused BSP checks. Two private Delta peers on disposable MCC13
fixture `8ecb01644739f8dbd038a993ada9b89b82e2f7522d948b15ac452f065cd3a1ff`
saw rifle slot 552 on its pad at ticks 30 and 60 in BSP 0. The host kept
slot 552 while the guest saw no rifle on that pad at ticks 90 through 180
across BSPs 1 and 2. At tick 210 in BSP 0 both peers saw slot 552 again.
The three BSPs used identical source geometry. Both processes exited normally.

#### Regional native vehicle homes

On hosted multi-BSP MCC13 multiplayer maps, each vehicle home binds to the
first BSP containing its pad. An armed timer stays recorded while that
BSP is parked. The host checks the vehicle's condition again before
respawning it after returning to the home. An occupied vehicle follows the
existing passenger rules, and a failed replacement allocation leaves the
old vehicle and its scenario name intact.

The Windows x64 vehicle harness passed all 97 checks, including an occupied
vehicle and allocation failure after returning to a home. On executable
`0ce46b08787d6d4909745fd28c039f3d058ea3e8eff9c35a6e7968e8809cc670`, a
private Windows dedicated server and native guest used a two-second vehicle
timer. The guest saw the Warthog move from its active pad at tick 60 and
its replacement (slot 545) appear at tick 120. The replacement retained
its scenario name and moved again before the host activated BSP 1 around
tick 150. Slot 545 stayed away through tick 270 with no replacement at its
parked home. A typed scenario object-name row was required for the scripted
moves. The disposable fixture uses identical geometry in both BSPs and has
SHA-256 `d4a1608374db71be93acda9106b7f9a58ed52e0dc804c655612402fd96a88c8f`,
CRC `ff631a4e`. The guest exited normally and the harness stopped the
dedicated process after sampling. Physical traversal and live return to a
parked home on the authored Chiron corridor remain unverified.

Regional vehicle homes also register when the gametype disables respawn.
An empty vehicle still at its assigned home stays out of the neighboring
BSP's collision and object replication, even in overlapping geometry.
Player boarding checks honor the same ownership. The vehicle keeps its
host identity and returns through native object reconnect. Occupied,
carrier-attached and displaced vehicles continue through the native path.

Windows x64 executable
`bbc47830a33e15d6ff4bfc5bfdf36d8b7b1195ed122a4b16e3608b646dfa8f22`
passed 13 focused vehicle and seat-admission checks. In the three-BSP
equipment fixture above, the host kept Warthog slot 0 across BSPs 1 and 2
and the guest received slot 0 again on return to BSP 0 at tick 210. The
rifle also retained slot 552. In the two-BSP occupied fixture above, the
native diagnostic staged the host driver. Both peers saw Warthog
`e2710002` and driver seat 0 at tick 60 and after the BSP switch at tick
75. Both runs exited normally. Guest-driven traversal and independent
simulation in distinct BSPs remain open.

Windows x64 executable
`407d38d80f0526ec6cb19636729291c7ab4741ff0f1f4b7426e4dd49969375d6`
passed a native guest-driven crossing on disposable MCC13 fixture
`e99d077d507ee1feb804d8ec76ed76e53b08f73545bb7846a2991d89d441b1d9`.
At 0.482 units from the driver entrance the guest received the enter
action and used E. Both peers saw Warthog `e26f0000` and guest driver
seat 0 at tick 150. The host stayed on foot in the seam. Guest keyboard
throttle carried the Warthog through the markerless switch at tick 343
on both peers. Both recorded the same vehicle, guest and seat at tick
360. The host saw it at (60.853, 34.880, -20.723) and the guest at
(61.228, 34.922, -20.723). Both processes exited normally. The fixture
used identical BSP geometry and two modified test starts. Return attempts
drew the farther guest start and stopped at the approach before boarding.
Guest-driven return, teammate passengers across a native BSP seam and
the actual Chiron map remain open.

### Eleven-region Damnation route and regional object state

Spark revision `8750c40` expanded the ten-region CE609 build to Damnation.
Exact cache SHA-256 `9531f157ddb0ff429e738a5b6f77835f3d18348194908556ecec9f0716101b53`.
Its single BSP uses 21.15 of 23.00 MiB of native tag memory and contains
54,188 collision triangles. On Windows x64 Forerunner `83a626f4` and
Chupathingy `c16096a8` (private OpenCE 44), one placement staged the host
before the Prisoner entrance. Normal input reached Damnation at
(659.370, -26.734, -18.206) and returned to Prisoner at
(579.410, -26.732, -18.206) in the same session. The guest moved 6.675 world
units independently on the main road. Maximum peer position differences
were 0.121 outbound and 0.113 returning over 35 and 34 samples, with guest
interpolation bounded to 31 ticks where exact ticks were unavailable.
Both processes exited zero and staging was removed.

The host now announces its BSP before streaming periodic world positions.
The client waits for the announced BSP before applying object, vehicle and
device snapshots, discards interpolation history from the previous BSP
and stops recording vehicle predictions while loading. On acknowledgement,
the host resends changed scene transforms and device positions that may
have settled during the load. The focused tests cover delayed packets and
duplicate acknowledgements. Native Windows x64
`46eb09e3b47b52e94d01414fe7ff3bea720c051f52b5351b05be812391c48dee`
ran the stock D40 foot crossing in both directions with two private OpenCE 44
peers. Both agreed on BSP 0 by tick 366 and BSP 1 by tick 432 and exited zero.
The native run verifies round-trip co-op on a real BSP switch. Live moved
devices across a boundary and independently resident BSPs remain open.

Continuous co-op BSP transitions now include native AI in the same vehicle
as a living player when the vehicle enters the selected switch volume.
The host uses the existing encounter detach/reconnect path. All 49 focused
BSP checks passed, including an unrelated vehicle, an unseated or dead
player, and a marked classic gather. Windows x64 build
`512878b3aeab5d3088ae6718758dcf3ddd5e7c32beab8bc5bc244c9ed977e93a`
ran private two-peer A30 on the unchanged stock cache above. A native Marine
occupied the Warthog gunner seat in BSP 1. A separate attempt put both
players in driver/passenger seats, but vehicle staging stopped before the
switch volume. A later disposable A30 copy put both players and a Marine
in the same Warthog, outside the approach. A corrected marker setup did
not keep the crew seated through staging. Live synchronized AI passenger
traversal remains open.

Stock D40 late-join playback used the unchanged cache above and Windows x64
executable SHA-256 `c135fe65ccb78cf583fafcf127f9a1251f70298a5367f4095694fda5c534497d`.
After the host walked through BSP 1→0→1, the first guest occupied slot 1.
A replacement guest reused slot 1 with a different biped after the first
guest disconnected. The replacement loaded BSP 1 at tick 1366 before its
first observed spawn at tick 1380. Normal guest input moved the biped from
(-24.501, 21.410, 0) to (-24.240, 21.019, 0), matching the host.
The host then walked across the seam into BSP 0 at tick 1536 and the guest
loaded it at tick 1539. Setup used scripts once to stage the host. The
crossings and guest movement used ordinary input in private loopback.

On a player queue slot change, the host clears the prior owner's pending
controls, input history and BSP acknowledgment. The client clears relayed
actions for that slot. Repeat registration of the current owner preserves
its active state. Both slot-reuse regressions failed before the fix and the
focused queue and BSP harness passed 49 checks afterward.

Co-op clients now skip periodic player snapshots while a host BSP change
waits for the existing main-loop load. The regression reproduced application
of destination state in the old BSP. The 36-case BSP harness then passed,
including snapshot resumption after load and after host cancellation. On the
same stock D40 cache, Windows x64 executable SHA-256
`9d77a862dc8aa0026cb45da0de23358422a7862fd805e68a8b49d00af265597d`
loaded BSP 1 before the late guest spawned. Ordinary movement took the host
across the next seam. Both peers switched to BSP 0 at reported game tick 741,
and the guest stayed on the floor near (-26.942, 21.399, 0). Both exited zero.
Map and executable hashes held and disposable staging was removed.

If a client load fails and rolls back to the source BSP, periodic player
snapshots now stay gated until its loaded BSP matches the host again. The
existing BSP and checkpoint harnesses passed all 46 checks, including failed
load and retry. Native loopback on the same stock D40 cache with Windows x64
SHA-256 `6f02d8a8fab2687b24a83da8ca79e06c516a54a0938701137a55c4afaeb3b5a5`
crossed normally at host tick 426 and guest tick 429. The guest remained on
the floor. Both exited zero, pinned hashes held and staging was removed.
The forced load failure was covered by the harness.

The host also checks queued infantry and vehicle prediction ticks against
each player's first input from the current BSP. A late source-region
prediction can no longer pass on the strength of a newer acknowledgment.
The packet layouts are unchanged. All 60 focused queue, BSP and checkpoint
checks passed. Native loopback on the same stock D40 cache with Windows x64
SHA-256 `c95f79a3fe88bde0fae08d3b1c08b60b7f2f1f9c5106fc9cb6ffa61c3ed6e70b`
switched both peers to BSP 0 at tick 450. The guest then walked from
(-26.942, 21.399, 0) to (-29.192, 21.417, 0), which both peers reported at
tick 570. Both exited zero, hashes held and staging was removed. The
late-sample rejection was exercised in the harness.

A later regression exposed the native fatal error before BSP rollback. Failed
destination loads now reach the existing source reload or return to the menu
if neither BSP is usable. Initial load policy is unchanged. Three refusal
cases failed with a fatal-aware harness before the fix. All 38 focused cases
passed afterward and Windows x64 linked.

The current Windows x64 engine (`6c935c9c6a7f11171f48f4a41e1ed3b1c6c2f0352b61b65545edb03227255db3`)
also exercised the refusal in a private native D40 co-op session. A disposable
copy with only the decoded BSP 1 header signature cleared rejected the request,
restored the opening cinematic BSP 8, advanced through tick 90, then loaded
valid BSP 0 on request. The engine exited zero. Stock D40 retained its hash.

The multiplayer HaloScript selector now records a requested BSP only after a
successful load or an explicit selection of the resident BSP. A failed menu
choice leaves the previously selected region available. The existing BSP
harness reproduced the first-selection failure and then passed all 39 checks.
The Windows x64 build linked. Competitive recovery is covered by the harness.

When authored exit volumes overlap, the co-op host now keeps the first
accepted route in scenario order and its gather marker through the deferred
load. A refused route still allows a valid alternative. The focused harness
reproduced replacement of the queued route and passed all 40 checks after
the fix, including legacy local priority. A Windows x64 build
(`abc5b2b1b4da3c46c1be48c9d3585407318ed0fb325d9508696d6f3b218cd3e4`)
also passed an unchanged stock D40 crossing with two private peers. After
one approach placement the host walked from x=-24.378 to -27.377. Both peers
reported BSP 0 at tick 366 and exited zero. The stock crossing exercises
the normal native path. Overlapping exits were exercised in the harness.

Dedicated co-op majorities now count any occupied route to the proposed BSP.
An overlapping exit to another BSP no longer hides that player's vote. The
existing vehicle-root, away-player and loaded-BSP checks still apply. The BSP
harness reproduced the missing vote and passed all 41 checks after the fix.
Windows x64 `58a684cbf62780cd2b81531da5637060693aa7d599facd476c8b08fcf753e855`
also started a private dedicated D40 campaign. One native client reached
tick 90 in BSP 8, and both processes exited zero. A live dedicated majority
crossing remains open. The inherited majorities are from `cf9fad600`.

The dedicated `coop_bsp_switch` event and player notice now follow a
successful native load. The host keeps the vote roster with its queued
destination and drops it on refusal or rollback. The existing event schema
is unchanged. All 41 focused checks passed, including no event on invalid
references or failed loads and one event on success. Windows x64
`6f3129053c03d645d8b79b9efe4ac1dda5c65ca07aabff02441d62659b51074e`
also passed a stock D40 listen-host crossing. Both native peers reported
BSP 0 at tick 366 after normal host movement and exited zero. The dedicated
event timing is harness-verified.

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
Endless built-in Survival also consumes the same native activation/return controls,
without a mandatory match timer. Windows/OpenGL owned-fixture evidence at
`build/native-delta-endless-evacuation-ready/result.json` records two authenticated
clients' physical on-foot returns, four unique banked weapons and a team outcome
only after both recovered. Its driver FAILS on a stricter occupied-point approach
after banking; validated records/native logs prove those scoped results separately.
`build/native-delta-endless-redeploy-ready/result.json` PASSES fresh-process
redeployment of the exact IDs and actual native weapons/grenade ownership.
Player/item/AI movement comparisons pass; original identity/seed files are
unchanged. Native aircraft evacuation in endless Survival, full Zombie outbreak/
progression, Spark mission composition, other platforms and human/hardware
acceptance remain open. This changes no authoring format or capability token.

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
forced-death treatment interruption and replicated drops. The existing Windows
fixture now also proves two verified accounts choosing opposing native factions
and separate squads, actual projectile kill/death/loss, the victim's physical
weapon pickup, on-foot extraction/banking and fresh-process same-ID redeployment:
`build/native-delta-pvp-loot-fixed/result.json` and
`build/native-delta-pvp-loot-redeploy/result.json`. The weapon's native ownership
trace distinguishes it from another item with the same tag. This is not full
Spark Black Echo, simultaneous AI PvPvE, aircraft or six-client acceptance.
The starter-kit baseline is now extended by
`build/native-delta-owned-gear-transfer/result.json` and
`build/native-delta-owned-gear-redeploy/result.json`: verified accounts reverse
host/client roles, deploy a real previously banked pistol, transfer that exact
serialized ID through native PvP/drop/pickup/escape, then redeploy it under the new
owner after process restart. The defeated owner's incompatible stored rifle stays
byte-identical and all three owned IDs remain unique. Existing native owners,
account binding and record locks remain authoritative; no editor reward writer.
Source: `extraction.c::extraction_unit_pending_death`,
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
Reload + Use, or the bound Flashlight + Use, cycles the compatible secondary subsequence, preserving
the selected primary; ordinary Use keeps cycling the lead pair. The first use
previews without changing either choice. Two available weapons have only one
secondary for the current primary, so the alternate gesture changes nothing and
writes no redundant save. `gear_cycle_loadout(session, slot)` reuses the same
locked record/order; no version, reservation policy or inventory packet changed.
`player_queues_new.c::update_client_loadout_modifier_held` reads the native queue's
existing full-player held Reload/Flashlight state after action latching; device interaction precedes
the body's current control publication. No duplicate input snapshot remains.
The terminal displays the pair and controls. Native keyboard interaction and
two-account ownership/reopening are checked separately from larger-stash
combination/deployment tests; physical-controller/VR acceptance remains open.
The existing Deploy control accepts either alternate gesture to leave all stored weapons
and grenades banked. Ordinary Use and automatic finite-Survival deployment keep
the selected stored loadout. `extraction.c::{extraction_loadout,extraction_deploy}`
passes that choice to the existing `gear.c::{gear_loadout,gear_reserve}`. A prepared
reservation always wins over a changed retry gesture. The version-1 record,
locking, settlement, death/disconnect loss and host identity requirements remain.
Native authored faction profiles still supply their ordinary kit; maps without
those profiles retain carried native equipment. No missing starter is invented.
Spark needs no new control, schema or writer: preserve its existing native kits,
pickups and objective/boarding/return gates. Successful escape can now accumulate
new carried items alongside the weapons left in the stash.

Bounded Windows64 native checks now cover two verified accounts protecting their
stored gear, a real three-item secondary choice and actual selected weapon tags,
independent objective/escape outcomes, accumulation and process-restart reopening.
The first bank and second-account continuation span separate sessions after two
driver failures; this is not uninterrupted Black Echo or aircraft acceptance.
Exact scopes and failures are in the current master checkpoint. Larger-stash
combinations and both faction-kit rollback remain automated evidence; controller,
VR and other-platform acceptance remain open. A later bounded Windows64 check
uses actual SDL virtual controller LB + X through both verified accounts, including
secondary choice, release/ordinary X, protected deployment and fresh reopening.
This is native controller-action evidence, not physical controller or headset
acceptance; approach still uses the existing keyboard/stick driver. The existing
`xinput_sdl.c::openxr_gamepad_read` reaches the same native actions without a new
XR binding/polling layer. Flashlight keeps its native toggle behavior; only a held
button modifies these two hub interactions, not a lamp merely remaining lit.
Default controller Flashlight is the white/LB action, but current native bindings
remain authoritative. Spark supplies no new marker/schema or controller state.
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

Current resupply ownership supersedes the historical parent-only rule below:
`coop_enemies.c::firefight_resupply_players` uses `game_modes_inventory_capture`,
including native unparented stowed items. During the built-in director only,
`players.c::player_add_equipment` treats a reset profile with both vitality
modifiers zero as equipment-only, preserving the biped's native vitality.
Classic and explicit-vitality profiles retain their old semantics; authored
HaloScript waves still own their rules and must not enable a second director.
No tag schema, authoring operation, gear record or packet changed.

Historical shared-equipment follow-up on `c6f9255c2`: native optional Firefight resupply
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
`coop_enemies.c::coop_enemies_firefight_track_spawned_unit` can now reuse a
completed dead entry when the table fills. It preserves the currently spawning
parent, every living entry and every pending native carrier burst; death/damager
metadata resets without repeating score or removing native corpses. Windows64
compilation and focused real-source checks pass. Two authenticated Windows/OpenGL
clients now exercise reuse in the intact owned native Library cache: seven
carriers, 26 native children, 33 cumulative tracked births/final deaths, score
3,300 and two independent banks/four unique weapons. Existing player/item/AI
comparisons pass. `build/native-delta-carrier-reuse/result.json` distinguishes
those native results from its FAILED observer regex (which counted `c` in
`child` as a parent). This does not prove 33 simultaneous tracked bodies or
large-horde performance. A preceding two-carrier run also preserves native
casualty loss with no reward. Neither encounter was rerun to hide driver failures.
Carrier authoring still needs real unit/actor tags, native death-animation spawn
behavior and infection/swarm dependencies. The retained CE609 fixture lacks that
complete set. Installed Invader cannot reconstruct the native Library carrier's
`characters\floodcarrier\effects\body destroyed.particle_system`; that conversion
failure was preserved, not bypassed/substituted. A valid target-specific compiled
CE609 handoff remains required for Spark carrier acceptance; no authoring/engine
format or discovery capability changed, and no Xbox hardware support is claimed.
Two authenticated Windows/OpenGL clients do pass native combat-form fighting,
final deaths, finite victory and four unique banked weapons in
`build/native-delta-flood-combat/result.json`; the existing aim diagnostic is used.
This is not full Zombie, huge-horde, human or other-platform acceptance. No new
authoring format/capability or global native population limit is introduced.
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
and `forerunner_spirit_complete` receive a wake after an actual boarded passenger
has durably banked and the connected crew has settled. The banked arrival
survives its owner's disconnect. The ship must be at the hub with the cinematic
finished and satisfy the native landing check if a landed group is authored.
Unrelated field players and another craft do not delay release. Full salted
handles prevent replacement ships inheriting completion. Actual LZ boarding
rearms the next trip. Authored
handlers must remain wakeable for repeat trips; a finished one-shot script is not
restarted by native `wake`. These crew handlers may be dormant, or continuous
with `(sleep -1)` first on each pass. The whole-operation callback remains
dormant-only and one-shot. The optional `forerunner_pelican_aborted` and
`forerunner_spirit_aborted` callbacks rearm a living aircraft after it returns
from real field boarding with no successful bank or unresolved connected crew.
Use these callbacks to release the flight script after the loss or departure of
the entire boarded crew. The aircraft waits for physical boarding at the field
landing zone while a completed managed call is active. The template uses
sleeping continuous crew/
camera handlers, one startup pilot setup and a continuous trip coordinator. It
keeps boarding closed at return until the previous passengers have disembarked;
the next insertion follows actual gear/seat readiness rather than a stale shared
deployment-control position. An already-cleared player on foot at the LZ may
reuse the powered native call control to recall a transport after the host
checks its current gear owner. Use an on-button call control for an idempotent script
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
At the hub, deployed members of the participating crew still there must occupy the assigned
craft. At the LZ, requesting members of a participating crew must finish
activation and hold valid boarding receipts/seats. With authored squad selection,
the actual passengers identify participating squads and their current operation
epochs. An unboarded independent squad of the same faction cannot hold this
departure at either pad. Squads that actually share the craft still participate;
stale operation passengers refuse departure. Unsquadded crews retain the previous
whole-faction wait. The runtime derives this through existing run/seat state,
without a flight registry or new packet. Other factions and unrelated
field operations are excluded. Empty crews, pending death, incapacitation, invalid
ownership, cinematics and flight between pads do not authorize a new departure.
The signal only coordinates flight; it grants no equipment or banking authority.
A ship with this group also reserves its native passenger seats for players with
a confirmed deployment. At the hub the player must deploy before boarding. At
the matching field landing zone, the player must complete the extraction
countdown and any authored squad objectives before boarding. If a matching
landed group is authored, the craft must satisfy the native landing check.
Keep the named hub, field and return volumes distinct. Managed ships reserve
seats marked Driver in the native vehicle tag for the authored AI pilot.
Players receive boarding and recovery credit from valid passenger seats.
Halo still checks occupancy, animation and physical access. Native AI crew
can board through ordinary scripts.
Seventy focused transport cases and Windows64/native HSC compilation pass.
`build/native-delta-squad-transport-seat/result.json` now verifies two authenticated
one-player squads' separate insertions and both serial evacuations/banks. The
first banks while the second stays ready in the field, then the pilot serves
the waiting pickup without another insertion. Both owners bank two distinct
items each and their own squad completion callbacks fire; 398 player seconds/
402 item snapshots pass existing synchronization checks. Earlier fixed/replan
attempts remain partial input evidence. The corrected controls preserve their
initial route and approach the actual native seat point for evacuation; native
`units.c::unit_find_nearby_seat` accepts either that or the entrance-animation
point within the unchanged range. No boarding, collision or bank gate bypass.
The second crew's final hub disembark/redeployment, full Black Echo, six clients,
Spirit serial dispatch and private presentation remain unverified in this run.
Optional dedicated groups `extraction_pelican_landed` / `extraction_spirit_landed`
publish actual native ground contact (five ticks before parking), upright attitude
and settled linear/angular motion. Native hover preserves the actual grounded
collision flag and parked pose; it does not manufacture a touchdown. They are host outputs, initially zero, with the same
unique matching-vehicle and control-group isolation rules. Binding one also
requires that faction's aircraft to be landed for native player seat admission,
credit for field boarding, and seated hub banking. The landed and crew-ready
outputs use the same native craft and landing sample. Crew-ready also requires
the eligible passengers to be aboard. Maps without the optional landing
group retain their existing crew-readiness contract.
Managed crew readiness checks each still-attached player before authorizing
departure. At the hub it waits for any banked passenger from the prior trip
to disembark. At the field it waits for a seated passenger whose operation
became ineligible to recover eligibility or disembark. Boarding and exit use
the inherited seat state and native `vehicle_unload` path.
Both premature-departure cases reproduced and passed after the fix, with
142 focused checks in all. Windows x64 admitted `fn_sqops@ce` through tick
330 in a native smoke. Live two-account Black Echo remains open.

Spark must wait for both the correct pad volume and this landed output before
opening doors or reporting arrival. A volume, hover command or elapsed timer
does not establish a landing. Source: `source/units/vehicles.c::vehicle_is_landed`,
`extraction.c::{extraction_return_ready,extraction_at_return,
extraction_transport_crew_ready,extraction_transport_readiness}`.
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
reuses its insertion and extraction legs as native static helpers. At the hub its
existing loop serves a seated insertion crew or a pending field call, allowing
another insertion while a squad remains deployed and a later pickup without a
new insertion. Keep the call control available to deployed squads; do not reset
it every time another trip starts. Native readiness still precedes departure,
and durable banking plus actual disembarkation precede reuse. This is a shared
aircraft sequence, not concurrent per-squad aircraft or private cinematics. It
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

Unarmed player exit now uses `units.c::unit_adjust_for_seat_change`'s existing
native standing-class lookup when the graph lacks a named unarmed ground type,
matching native seat entry. No Spark graph rewrite, granted weapon, tag format
or capability change is needed. Windows64 and focused checks pass; the two-account
Spirit squad fixture now verifies two normal hub exits and both players'
uninterrupted same-ID re-equipping after separate native insertions/evacuations
and independent banks (`build/native-delta-spirit-return-footpath/result.json`).
It uses the unchanged reviewed cache/controller and the observed outward footpath
in reverse, preserving collision and every boarding/ownership/settlement assertion.
The older departure failure is explained by its own retained host chat: activation
near tick 10,350 was cancelled near 10,530 after movement resumed before the
ten-second hold. Boarding that cancelled run correctly could not authorize flight.
Existing `debug.network_test` exposes the actual host notices; no new diagnostics
or authoring interface is required. Exact accepted and rejected scopes/identities
are in the current master checkpoint. This fixture does not prove field combat,
loot collection, another mission playthrough or the actual Spark mission.
Preserve boarding/landing/settlement gates;
this does not accept the full authored Black Echo mission or private cinematics.

Read-only Spark `defc08c` supersedes the `4249bb5` composition gap. Existing
`src/haloScript.ts::exportHaloCeTransport` composes actual pilot seating,
insertion, optional empty hub return/recall and durable settlement callbacks with
the seven-objective mission. Reviewed deferred creation now covers the named
`be_technology` weapon and optional pilot. `crates/spark-core/src/native_transport.rs::pilot_bindings`
and the Windows native reader check effective actor/biped animation graphs for
the exact driver label; incompatible stock Grunt pilot bindings are refused.
Source ownership, original native indices and unique groups remain protected.
Its `docs/RUNTIME_INTEGRATION_20261107.md` records composed CE609 compile/readback:
Pelican `e122b840495e9ec8ad6288f3bbc1a500963d93d77a6612f144bab95224f42b9c`,
Spirit `985df7845e18d27194984ec52029bd80d1a757a6fef0eda3621c4de230287d8f`.
These are Spark's bounded compilation records, not Forerunner execution or full
Covenant/faction mission acceptance. Do not reimplement the completed authoring.
Read-only Spark inspection now reaches `0f61442`: native faction terminals/kits,
aircraft-relative camera points and recovered mission radio are composed with
the seven-objective mission and both transport controllers. Its source/CE609
compile/readback milestones include protected existing HUD and sound reuse.
These close the earlier camera/radio authoring gaps in scope; actual framing,
audible playback, faction objective/actor alignment and complete native gameplay
remain open. Its checkpoint records the compiled verification caches as disposed,
not a retained delivered mission package. Do not rebuild Spark or duplicate its
completed authoring to conceal that handoff gap. Spark source remains untouched.

Read-only Spark `8fdfe91` now supplies the retained Pelican mission review cache:
`releases/black-echo-review-20261117/Spark-871c0ad7-52b6-46d6-99df-982e1f12ddc4/be_native.map`,
110,033,808 bytes, SHA256 `31505afb15da14ff32af694afecc28362ff911bd6df6b79778195493c6c3ebab`.
Its internal scenario path is `be_native`, not the older fixture's `levels/forerunner/…`.
Forerunner's unchanged current Windows64 binary loads it for two verified Delta
accounts and admits its native hub. Player comparisons pass 90 seconds/180
observations; 94 item snapshots match. Full operation acceptance FAILS before
deployment: the observed normal approach to control 0 at (49.624,-87.469,0.714)
stops under the parked Pelican at (48.921,-87.505,1.319). Native action 8 offers a
passenger seat instead of control action 10; no deployed gear records are created.
The grounded, stationary Pelican also reports up 0.942 and landed 0, so the
authored landed gate cannot currently clear. Screenshots and raw native evidence
are in `build/native-spark-black-echo-aircraft/`; the cache was not edited/rebuilt.
The diagnostic's overlength console query was refused, so pilot readiness is
unverified. Two-account flight/combat/rescue/collection/settlement remain OPEN.

Spark's next revision must move the hub spawn and `extraction_deploy` control
out of the parked hull/boarding approach. The observed blocked body is
(48.767,-87.553,0.076); it is not enough to move only the control while leaving
the spawn under the aircraft. Keep a walkable collision-tested route from both
faction starts to the terminal and then to the intended native passenger entrance;
normal terminal use must offer action 10, with action 8 reserved for boarding.
Author aircraft-scale hub/LZ pads with clear hull, tail and passenger entrances.
`source/units/vehicles.c::vehicle_is_landed` requires grounded contact, up >=0.95
(the observed 0.942 fails), then native hover or at least five grounded ticks with
velocity squared <=0.000025 and angular velocity squared <=0.0001. A stable
upright touchdown must meet those existing values inside the unchanged native
return/LZ volumes; a script-written landed flag cannot substitute for collision.
Keep collision, boarding, landing-attitude and settlement gates;
do not bypass them or claim an arbitrary alternate approach was disproved.
Read-only Spark `456fb33` records a vehicle-only trial: Pelican (49,-80,0.35),
yaw 14.323945 degrees, scenario SHA256
`649a2118f085893d3d7395431710f205b6c261732a4c1119819d499d5129e143`,
compiled SHA256 `92b0b4ae9df5b1202575b2984a5acb9961f042b1167802e144b6f0a013a2f9d5`.
Its own checkpoint says Y=-80 lies outside the original return volume and that
the trial/source/compiled outputs were disposed. This is not a delivered fix or
flight acceptance. Spark next: coordinate hub/spawns, all five distinct controls
(deploy, activate, loadout, UNSC, Covenant), both craft/pilots, approach/departure
routes and matching return/LZ volumes; retain the resulting source-matched native
review package for Forerunner. The original `31505afb…3ebab` remains unchanged;
do not retest it or rebuild this incomplete trial as the final mission.
Supply the revised cache with matching
resource/source identities, native player/faction equipment, physically valid
hub/LZ/routes, and returning camera/audio cues. Exercise normal pilot entry,
flight/landing, collection/rescue, real passenger boarding, departure, settlement
and repeat trips in Forerunner. Native source admission does not prove hull
clearance or a playable faction mission. Reuse the existing matching groups and
callbacks; no JSON importer, launcher or new protocol is requested. Spark's
current empty-return controller serves one dispatched crew; additional concurrent
squads must use the already supported boarded/called signals without resetting a
field request. Earlier regression-fixture steering attempts failed:
the earlier `build/native-delta-squad-transport-replan` also reset its initial
approach when the nearest seat changed. The newer seat-point run above closes
both serial banks within its stated fixture scope; no engine interaction defect
or relaxed admission was needed.

`coop_enemies.c::coop_enemies_update` now retains queued extra dropship AI when
native `actor_place` fails, retrying at the existing four-tick cadence without
skipping its placement offset. Real-source vehicle/release checks and Windows64
compilation pass; actual native allocation-pressure recovery remains unverified.
Native encounter/squad/variant references and destroyed-craft behavior remain.
Previously accepted actual Black Echo combat/rescue/technology/Flood/defense and
on-foot banking/restart remain accepted; aircraft evacuation in that mission,
independent squads, other platforms and human/headset acceptance remain open.

A runtime reporting `extraction.faction_landing_zones.v1` accepts a matched
`extraction_pelican_activate` control and `extraction_pelican_zone` volume for
the named Pelican. Use `extraction_spirit_activate` and `extraction_spirit_zone`
for the named Spirit. Each control must be inside its own zone. Hub starts must
be outside every field zone. Omit the shared pair when every named transport
has a complete route. Keep it as the fallback for any transport with no override.
The host uses the matching zone for calls, countdowns and boarding receipts.
For named aircraft, optional unique native volumes `pelican_return` and
`spirit_return` supply the corresponding insertion and settlement pads.
Author each aircraft's script against the same pad and keep it outside the
field zones. The runtime checks the pad together with the existing native
landing gate for boarding, crew departure, settlement and completion callbacks.
Forerunner advertises `extraction.pelican_return_pad.v1` in desktop CE
Spark discovery. `extraction_return` remains the player deployment hub and the fallback for
aircraft without a dedicated pad. Dedicated pad fixtures cover both factions,
landing, cinematics and the unchanged foot deployment boundary. The Pelican
override passed the exact private Windows solo run above.

Two authenticated Windows clients loaded the compiled fixture and agreed on
both placements. The existing harness verifies independent calls, cancellation
and banking. A live dual-route flight remains open.

Optional dedicated groups `extraction_pelican_called` / `extraction_spirit_called`
separate requests made at shared or faction-specific controls. The host derives each
output from living, correctly owned, same-faction players. A countdown needs
players inside their landing zone. For Extraction maps with a matching
`*_boarded` group, a completed call stays active while its valid caller
approaches on foot or aboard another vehicle. After physical boarding, the
request lasts until the aircraft leaves its field pad. Other maps use the
landing-zone presence rule for calls. Loss, disconnect, revoked objectives and
settlement clear the request. Pending gear never requests flight. Use unique
groups initially zero, with matching ships and no control power/position aliases.
Source: `extraction.c::extraction_transport_called` and the existing readiness
update. The aircraft HSC consumes its Pelican request while native activation
continues. The separate boarded signal authorizes an occupied departure. If an
empty craft reaches the pickup pad after all active callers are gone, the
template takes the native route to the hub without an evacuation or bank.
Use the empty-return branch with a runtime that retains completed requests
outside the landing zone. Earlier call-group implementations require the
field script to wait for boarding.
Windows64 and native HSC/cache compilation, 44 focused transport workflows and
two defect controls pass. `build/native-pelican-faction-call-deployed` verifies
one authenticated player's confirmed deployment, physical boarding, insertion,
disembarkation and real request consumed by the Pelican script. The corrected
probe requires the actual deployed gear record; the earlier stale prompt-based
driver boarded without deployment and correctly could not depart. The successful
probe stops at recall initiation, with gear still deployed and no bank/exception.
Spirit requests and independent factions have automated coverage only; completed
recall/evacuation, multi-client signal acceptance and other platforms remain open.
Source: `extraction.c::{extraction_transport_crew_ready,extraction_transport_readiness}`
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

Use a native Elite as the default Covenant player body in new operations.
Bind it through the existing extraction_covenant_body placement and pair it
with a compatible extraction_covenant starting profile. Explicit author
choices can still bind other species. The current Black Echo cache binds
`gs_assets_04f1039aa23e4ef98e06328f629bf34a\characters\elite\elite` through
the native Covenant body placement. It retains the technology weapon's
`pp` label, which the imported Elite animation graph has for stand and
crouch. Spark's seat readback found all eight exact Spirit passenger labels
in the graph. Verify physical pickup and Spirit boarding in a live match.
The released cache lacks the matching cooked `_fp` model. Spark `8fc1914` exposes
**Include authored first-person arms** in the native faction review and
validates the carrier palette and model dependency. The inspected CE SDK
archive supplied no compatible Elite first-person model. Prepare that asset
using the exact namespaced biped path and the scenery palette binding
described below. Confirm hands, HUD, collision, animation, shields and sound
on the delivered character.

Forerunner validated the CMT Elite arms locally. The archive credits CMT,
Teh lag, and MMAMONSTERRR-D92209. All 37 model nodes match the exact
starter plasma-pistol graph in `04d9ccd1`. A disposable CE609 derivative
`fbf24551` cooked the exact namespaced `elite_fp` model and scenery carrier,
accepted native Covenant and deploy interaction, and rendered the arms
holding the plasma pistol. Use Spark's existing authoring path to prepare
its own cache for the paired run. The CMT asset and derived map were kept
out of source and distribution. [CMT Elite FP Arms](https://www.halomaps.org/hce/detail.cfm?fid=5764).
Evidence: `build/cmt-elite-arms-native-build/result.json` and
`build/native-black-echo-cmt-arms-render/result.json`.

The existing Player Setup color picker stores the selected profile color.
Faction replacement and respawn apply it to native armor channel A after tag
permutations initialize the body, using the existing replicated appearance.
The stock Elite's torso, arms, legs and head shaders already read channel A.
Custom armor should keep that binding and preserve function-driven effects.
The focused check exercises two distinct profile colors and
preservation of a secondary channel. Lobby readback, reconnect, transitions
and rendered two-client Elite colors require native Elite acceptance.

### First-person arms for authored characters

Forerunner desktop CE builds advertising `character.biped_hands.v1` can
select arms by the player's actual biped definition. Name the authored
hands model after that exact cooked biped path followed by `_fp`. For
`characters\elite_mp\elite_mp_special.biped`, the matching CE source tag is
`characters\elite_mp\elite_mp_special_fp.gbxmodel`. Include the model through
an ordinary scenery tag in the scenario's scenery palette. A world placement
is unnecessary. Check the exported cache for the exact path, including any
namespace introduced by asset import.

The existing weapon graph supplies the animation and bone matrices.
Each arms model node must match that graph by name. Keep the default hands
in Globals for other characters, unsupported runtimes and weapons whose
graphs cannot remap the authored arms. Spark can expose this as an optional
First-person arms selection in its existing native faction-body workflow
and check the chosen starter weapon graphs and cooked model at build time.
This binding has no network packet or save-format extension.

The native Windows test used Invader to retain a renamed existing Spartan
hands model through an unused scenery palette entry. Forerunner rendered
its arms and weapon without the global hands reference. The capture is
`build/native-first-person-authored-model/viewmodel.png`. The CMT test
above also verifies Elite arms with the starter plasma pistol. Mixed-faction
first-person multiplayer and VR remain unverified.

The selected approach extends the existing mapper in
`source/interface/first_person_weapons.c`. Reviewed OpenCE
`a1a4dfd0a1abc751f179eb6dc99b88bc401b5a41` (GPL-3.0), Fractum/Chupathingy
`cd7fd91f376a18da21674872c8da9d693d9dea02` (AGPL-3.0). The inspected
draw/remap code in OpenH1 `96aa534b860d`, SlipstreamCE `75dd78a44c53`,
Faber `e4ffe0fe9874` and Halo Content Evolved `aff7a5d1b8ef` also uses a
map-wide hands reference (AGPL-3.0). The native build used Invader
`0.55.0.r4134.33518109` (GPL-3.0-only). DarkZealot's 2008
[Elite MP Tags 2.0](https://www.halomaps.org/hce/detail.cfm?fid=3739)
documents the map-wide player biped assignment. The listing does not specify
redistribution terms. No assets from the release were added.

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
allied, non-incapacitated player selection in `source/ai/encounters.c::encounter_update_follow`.
The Extraction search uses the full active roster without allocating a larger
target array. Classic ordering and the eight-target legacy player/AI cap remain.
Windows64 and the focused 20-case follow suite pass, including ninth-slot and
downed-ally coverage. Larger live sessions remain unverified.
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

Crowded treatment now keeps an existing valid healer/target binding stable and
selects the nearest available allied casualty for a new hold. Another healer's
active treatment remains reserved; damage, walls, range and true-death checks
remain authoritative. Existing authored `network.coop_script_adaptations=false`
also keeps `unit_set_current_vitality` repairs specific to the named unit through
`network_coop.c::network_coop_set_players_vitality`. Default legacy current-health
sharing is preserved. Maximum-vitality scaling remains campaign-wide; this does
not add per-player maximum-health replication or a new packet/capability.
Focused checks and Windows64 compilation pass. Three local native players in
`build/native-medical-target-selection-fixed/result.json` verify two timed
rescues in order with original bodies and the healer's health unchanged. The
probe's screenshot wait failed because its output directory was omitted; native
health/treatment logs are accepted, images and independent-client acceptance of
this change are not. No competing revival controller is requested from Spark.

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

16 November operation-radio update: native TEAM follows existing named squad membership, with
native UNSC/Covenant faction fallback for unsquadded players. No new marker,
membership authority or packet is required. `extraction.c::extraction_voice_group`
supplies read-only host labels; `voice_game.c::{player_place,host_world}` feeds
them into the inherited `delta_peer.c::delta_peer_voice_route`. Admission-pending,
missing/restored membership and mixed-group split-screen machines are isolated
instead of broadcasting across operations. Ordinary co-op and party/proximity
retain their existing rules. Privacy requires this updated host; older hosts keep
whole-co-op TEAM behavior. This is player VOIP, distinct from Spark-authored
mission dialogue/radio, vehicle cameras and cinematic work.

Windows64 compiles and 35 focused existing/extended checks pass. Two real Delta
accounts exchange synthetic Opus/Delta tone in native sessions: same-squad TEAM,
separate-squad isolation, regrouping and distance-independent TEAM pass; nearby
proximity passes and out-of-range proximity is refused. The two longer drivers
fail after their completed radio phases at a missing log expectation and exact
movement target; the final range-only run passes. Exact evidence/scopes remain
in the current master checkpoint. No physical microphone was opened. Human
audibility, headset, cross-region, browser and other-platform parity remain open;
restored/mixed-local privacy is automated-only. Spark needs no authoring change
for these routing labels; its next mission handoff requirements remain unchanged.

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
Desktop Forge Tools accept base-map OBJECT rows with zero source and baked-cache
flags. See source/game/cheats.c::forge_sidecar_parse and cheat_forge_sidecar.
Use the matching installed map and an empty Forge layout. The per-map slot is
the virtual save path z:\custom_maps\forge-<map>.txt (CE uses <map>@ce in the
filename and custom_maps\<map> for Base Map). Save preserves files authored by
other tools. The Forerunner-only OBJECT_SCALE row retains supported scaled
copies. Windows LAN verification covers Pelican restoration after restart and
rollback when a batch exceeds the host object quota. Streamed tags and other
record kinds remain outside this placement subset.
The fork's source selects stock protocol for ordinary games and Forge-specific
`0x4630` / game kind `0x46` for Forge at the refreshed pin (the README's older
`0x462A` description is stale); its messages and tag-streaming paths are
not automatically compatible with Forerunner. Watanoge's separate Forge/Firefight
snapshot remains pinned in [the upstream matrix](UPSTREAM_FEATURE_MATRIX.md).
A placement file does not supply missing client gameplay tags: every peer must
resolve compatible content and dependencies before spawning, including late
joiners. Existing visual-only loose-tag mods do not meet that requirement.
Keep proprietary base MCC data local; no asset-sharing capability is implied.

### Chupathingy HMV layout handoff

Desktop builds advertising `forge.hmv_base_placements.v1` expose **Save Layout
(.HMV)** and **Load Layout (.HMV)** in the existing host Forge Tools. The
directory is `<save root>/forge/<scenario short name>/`. Save writes
`forerunner.hmv`. Load prefers a supplied `import.hmv`, then the saved layout
when the import file is absent. **Load Saved (.HMV)** opens
`forerunner.hmv` directly, so the original `import.hmv` can stay in place.
Both actions validate the complete layout before applying a batch to an
empty Forge layout and show the title and creator on the host HUD.
Forerunner saves carry forward the HMV title, author, description and date.
To revise an existing credited save, open that save in the current Forge
session before editing. Forerunner never writes to `import.hmv`.

Use the matching compiled cache, including the native map checksum. This
placement interface accepts up to 128 unscaled base-map vehicle, weapon,
equipment or locally supported scenery records. Each is an ordinary single
object placed at start with its native tag path, team and pose. Respawn timers,
variants, AI, game markers, packs and special physics require support beyond
this interface. A file requiring such features is refused intact. Host limits
and gameplay-object restrictions still apply. Scenery is currently excluded
from sessions that accept remote connections, as it is for the text sidecar.

Desktop discovery feature `forge.hmv_local_devices.v1` adds **Forge → Local
Forge**. Use the matching multiplayer cache and the same `import.hmv`
slot. This local host accepts stock scenery and plain `mach` and `ctrl`
HMV records placed at start, with normal physics and the existing record
limits. The added objects use native creation and keep their tag defaults.
Set up linked switches, power groups and scripts in the native scenario.
Stock Silent Cartographer control 3 and machine 50 share position group
1 and power group 2. Control 3 has the scenario's
`not_usable_from_any_side` flag, and the native runtime reported it unusable.
This pair can check bindings. For physical actuation, export a CE609
scenario with a touched-by-player control and position-driven machine
sharing a repeatable position group. Give them powered device groups or
the default powered private groups. Use an animated platform, or a
door with the authored nonautomatic setting when door behavior is wanted.
Set positive powered position transition and acceleration times, place
the control within reach of a safe start and leave the machine room to move.
In the tested native implementation the stock Wizard alert light is a
gear and advances the shared position group on its own. Its converted
platform copy stayed still at zero acceleration. Keep the original gear
for continuously driven motion or author a separate powered on/off path.

On Windows x64 an independently rebuilt Danger Canyon CE609 fixture used
the stock control with a disposable Wizard machine set to platform and
0.125 seconds of powered position acceleration. Two keyboard E (Use)
presses moved the group and machine phase from 0.25 to 1 and then to 0. A separate
Slayer loopback host and connected client reached the same three states.
The input was on the host and the guest was still awaiting spawn.
Receipts are in `build/native-ce-position-switch-ramped/result.json` and
`build/native-ce-remote-switch/result.json`. Both were rebuilt fixtures.
Spark's original `b7f3b855` cache was disposed of after its compile check.
Stock inputs were preserved. Spark revision `ad73c05` records editable
switch settings, Save and Undo/Redo, independent CE609 readback, and the
same E-key host and connected-client phase sequence.

A two-start CE609 variant verified direct guest keyboard E on Windows x64.
The guest received the native Use offer. Both peers recorded the group's
0.25 to 1.0 to 0.0 sequence and intermediate machine animation phases.
In a second variant with an owned collidable A10 sliding door, the closed
panels stopped the guest at x 1.125. Guest E opened the door. Both peers
recorded matching open panel transforms, and the guest crossed the same
lane to x 2.843 with the host observing x 2.743. The corresponding
receipts are `build/native-ce-guest-switch/result.json` and
`build/native-ce-guest-door-collision/result.json`. The source caches
were preserved and disposable tags, maps and runtime folders were removed.

For a recovered control, clear Initially Off or assign a powered group as
well as enabling interaction. Map readback should verify the control and
machine share the intended position group. Provide separately usable starts
for network players. Slayer rejects a start within two world units of a
live opponent, so use separation beyond that floor and provide enough
valid starts for the variant. The full reopened Spark desktop
project-to-game workflow and human play remain open.
The HMV writer saves these devices with the original classes. Device
records stay outside the text sidecar and remote-hosted HMV path. Spark
can expose the local device export when the runtime advertises this
feature. Keep the base-placement export for older runtimes and remote
sessions. The [Chupathingy Forge placement
path](https://github.com/ChupathingyCE/chupathingyce/blob/0ec9360d7dcf96d4a99636fde6d2c5c1388ff56d/port/linux/game/forge.c#L2126-L2178)
uses the same native object creation. Windows x64 native placement and
save evidence: `build/native-forge-local-devices-play/result.json`
(Danger Canyon control) and `build/native-forge-local-machine/result.json`
(Wizard machine). Both runs bound UDP to loopback.

Desktop discovery feature `forge.hmv_local_physics.v1` extends the same
**Local Forge** session and HMV slot to version 7 placements with one Fixed
or Phased flag. The reviewed runtime must advertise both local Forge
features before Spark enables this export. Fixed copies return to the saved
position and orientation when loose. Existing Forge transform edits,
undo and redo set the updated pose. Phased copies use native non-collision
and can still be selected with the Forge aim tool. Phased pieces do not
block the occupied-space check for moving, rotating or restoring a nearby
solid copy. The same check still rejects solid blockers. Both flags survive
HMV save and reload. In Local Forge, select an unused copy and choose
**Change Physics** in Forge Tools to cycle Normal, Fixed and Phased. Clearing
Phased requires room for solid collision. Forge uses its own phase flag, so a
HaloScript collision setting survives the edit. Save the layout as HMV to
retain the mode. The existing plain HMV and text sidecar paths keep their
previous limits.

The implementation adapts BirchWood's
[ChupathingyCE Forge physics](https://github.com/ChupathingyCE/chupathingyce/blob/1b732f52d73f214d4f0068178d45753cca56dcf3/port/linux/game/forge_physics.inc)
under GPL-3.0, through `forge_hmv_placements` and `cheats_forge_update` in
`source/game/cheats.c`. Native scenario switch wiring uses the
position-driven machine settings above.

Desktop discovery feature `forge.hmv_host_physics.v1` enables the same
version 7 layout for a remote Forge host when every connected machine
negotiates Delta Forge placement physics. Use only the supported base-map
vehicle, weapon and equipment records. The host validates the whole HMV
before creating anything. An incompatible machine already in an ordinary
Forge match keeps playing and the special layout does not load. Once the
host activates these properties, later guests must negotiate support
before they spawn. Ordinary Forge layouts still accept legacy peers.
The feature uses spare fields in the existing object creation and state
messages. No HMV or message size changed.

A Windows x64 loopback host loaded a Spark-generated layout with suspended
Fixed and grounded Phased Warthogs. The existing guest and a late joiner
received matching object identities, fixed positions and phase flags.
Spark read both properties back from the host save. A legacy-protocol
arrival was declined before gameplay in the active layout. The match
and the compatible players continued. See `build/native-remote-forge-physics/result.json`.
A second
native run kept an existing legacy guest connected through a refused
version 7 import. See `build/native-remote-forge-legacy-gate/result.json`.

Desktop discovery feature `forge.guest_rotate.v1` describes the native
**Forge Tools → Rotate Aimed Copy** action. In a private Forge session,
the host opts in with `network.forge_guest_edits = true` or
`HALO_NET_FORGE_GUEST_EDITS=true`. A single-player client negotiates
Delta `forge_guest_edits` with that host. The live player aims at an unused
Forge copy within 20 world units and requests a quarter turn. The host checks
player ownership, current aim, the observed pose and native placement clearance.
The edit uses ordinary object replication and host Undo, Redo and Save HMV.
Spark can read the resulting HMV using the existing format. The action is
in-game. Host authorization and handling are in `source/game/cheats.c` and
`port/linux/game/network_distributed.c`.

Windows x64 loopback verified a guest turning a Spark-authored Fixed Warthog
from 0 to 90 degrees, host Undo and Redo, three matching object identities
and poses after a late join, and Spark readback of the saved Fixed flag and
yaw. See `build/native-guest-forge-rotate/result.json`.

Desktop feature `forge.guest_placements.v1` adds **Select Aimed Object**,
**Place Copy**, **Move Selected Copy**, and **Remove Aimed Copy** to the
same in-game menu. It uses the same host opt-in and negotiates Delta
`forge_guest_placements` alongside `forge_guest_edits`. The host keeps
each guest's selection for its current player and checks the source pose
before placement or movement. It uses the guest's live aim and native
ground clearance to determine the destination. Removal requires an
unused, aimed Forge copy. The host retains Undo, Redo, and HMV saving.
Spark can discover the actions and read the host's existing HMV output.

Windows x64 loopback verified rejection before selection, placement,
movement, and removal across host and guest, plus a late joiner receiving
the surviving copy. Spark read back the surviving Fixed placement and
creator metadata. A follow-up verified host Undo and Redo across both
players, rejection of a stale guest selection after Undo, and reloading
the saved layout after restarting Forerunner. See
`build/native-guest-forge-placements/result.json` and
`build/native-guest-forge-recovery/result.json`.
Authenticated human play remains open.

Desktop feature `forge.live_physics.v1` exposes **Change Physics** in the
existing hosted Forge menu for eligible vehicle, weapon and equipment copies.
The host cycles the selected copy. With the same private guest opt-in, a guest
with Delta `forge_guest_physics` can cycle the aimed copy. The host checks the
observed mode and pose before applying the edit. All joined players must support
Forge physics. Restoring Normal checks solid clearance, and can be retried
after moving the copy. Save HMV retains the existing format and physics flags.

Windows x64 loopback verified host and guest live edits, late-join Fixed and
Phased state, and a return to Normal on all three instances. The host blocked
restoring solidity beside an overlapping boulder. Moving the copy to clear
ground allowed it. Spark read the Fixed and Phased flags in the host save.
See `build/native-remote-forge-live-physics/result.json`. A separate
OpenCE-protocol guest stayed in the ordinary match while the host's
attempted change left the selected copy Normal on both machines. See
`build/native-forge-live-legacy-gate/result.json`.

The host's **Undo Edit** and **Redo Edit** restore the mode and pose of
the latest move, rotation, or physics edit, including an approved guest
edit. A blocked restoration of solid physics leaves the edit available
to retry when the space is clear. Windows x64 loopback verified a guest
edit, host Undo and Redo, a late client's matching state, and Spark HMV
readback after each save. A separate local test refused Undo into an
overlapping boulder, then restored Fixed after removing the blocker.
See `build/native-forge-physics-history/result.json` and
`build/native-forge-physics-history-clearance/result.json`.

Windows x64 tested a version 7 HMV generated by Spark's exporter at
`d4050d1`. Stock Blood Gulch held the fixed Warthog at z 6 across 53
observations spanning import and restart. The normal and phased Warthogs
fell. Save and restart kept all modes and the creator metadata. A focused
collision check covered ordinary point and ray pass-through, Forge
selection, attached objects, and existing HaloScript non-collision behavior.
Human collision playtesting remains open. Local receipts are in
`build/native-local-forge-hmv-physics/`.

Spark `630f91ab15bb3592047724caadd0be8a6ea852d2`'s Local Forge export
also passed direct playback. A fresh Level Editor export with SHA-256
`7eddb139014709a62868457a5a6a23e58581bae6e05b725bd3c6739aaffa3e62`
placed the native holo command control at (3.774, 16.842, 1.434) on the
matching Danger Canyon CE609 cache. Forerunner saved a byte-identical
layout, removed the copy through **Undo Copy**, and restored it through
**Load Saved (.HMV)** with the original import still present. The control
had its tag defaults and no native switch binding. UDP listeners stayed
on loopback and the original cache and import matched their input hashes.
Evidence: `build/native-spark-local-forge-7eddb139/result.json`.

Chupathingy nightly `7e2024d2` uses a stock Xbox multiplayer map list for
its local Forge launcher, which excludes Danger Canyon. The verified
shared case for that nightly remains the Blood Gulch SparkLink HMV below.
[Chupathingy Forge map list](https://github.com/ChupathingyCE/chupathingyce/blob/7e2024d2/port/linux/game/forge.c#L363-L378)

Reuse source: ChupathingyCE `1b732f52d73f214d4f0068178d45753cca56dcf3`
`port/linux/game/forge_files.c` and `forge_files.h`, retained verbatim under
[GPL-3.0](LICENSE.GPL-3.0). The original Forge integration is credited to
[ChupathingyCE contributors](https://github.com/ChupathingyCE/chupathingyce/commit/81d5526eb7716eaed82c13737b91ce3553f36231).
This adapter uses existing Forerunner host object creation, replication and
text sidecar rollback. It does not negotiate a new network protocol.

Spark `7c31919` exports the Forerunner placement subset as `import.hmv`
through its existing Forge layouts editor. Its history tracks pose edits
and preserves unsupported records for Chupathingy export. A Forerunner
playtest must match the runtime cache and require
`forge.hmv_base_placements.v1`. On Windows x64, Forerunner loaded the
production Spark export through Forge Tools and spawned a Warthog at the
edited Blood Gulch position (103.19, -160.85, 0.48). The import hash and
source maps were preserved. Evidence: `build/native-spark-hmv-forerunner/result.json`.
The HMV codec stays in Forerunner under its upstream license. A contribution
for upstream review can isolate this runtime placement adapter from Spark's
authoring code.

Windows x64 native Forge Tools saved a 180-byte version-1 layout with one
Pelican, then loaded it after process restart. The same host refused an
import with a 30-second respawn property before placement and retained its
bytes. Spark discovery returned the capability without writing files.
Evidence: `build/native-forge-hmv-verified/result.json`.

An official Chupathingy Windows nightly at `7e2024d2` saved a one-Warthog HMV
on Blood Gulch. Forerunner loaded it through Forge Tools, observed an added
native Warthog at (102.32, -160.85, 0.47), and exported it with the same
map checksum (56503715) and tag. The same nightly loaded that Forerunner
file and applied one placement with zero skipped. The harness's final
ordinary item sample timed out in Chupathingy's monitor mode. Its native
layout-application log records the outcome. Source maps and the imported file
were preserved. Evidence: `build/native-hmv-chupa-forerunner-roundtrip`.
Spark `cfe0a2cc517d1067451d6a7de027268885afd717`'s production SparkLink
exporter reproduced the recorded acknowledged revision 35 exactly. Its HMV
SHA-256 is `1dab054291db7851a29fce60ca2bdb06f6d584d7a57fff7d7a4a66bd22edeb59`.
Both Windows Forge runtimes loaded these bytes against Blood Gulch SHA-256
`50fe52406f075d975e24100a65b26ff696458023dd3509878953052ab0ef858f`.
Forerunner showed “SparkLink revision 35” and “Created by Équipe Forge” on
the HUD, spawned the edited vehicle at (104.25, -160.85, 0.45), and saved
team 0 with intact metadata. Chupathingy nightly `7e2024d2` reported one
placement and zero skips. Its native object inspection showed the Warthog
at the same XY with elevation 1.01 at that sample, facing (0.732, 0.682, 0),
team 0 and normal physics. The two samples do not establish identical
settling or simulation. Both imports and original map hashes matched after
the runs. Evidence: `build/native-sparklink-1dab-forerunner`
and `build/native-sparklink-1dab-chupa`. See the [pinned Chupathingy Forge
command reference](https://github.com/ChupathingyCE/chupathingyce/blob/7e2024d2/docs/forge.md).

In a native private Windows x64 OpenCE-protocol session, **Load Layout
(.HMV)** placed an armed Warthog on Blood Gulch for an already-connected
guest and an independent late joiner. Both clients received the Warthog
and its attached gun. **Undo Copy** removed both objects on all three
instances. Forge now distinguishes the vehicle's own attached factory
weapon from riders and independently owned attachments. This uses the
stock object lifecycle and existing multiplayer channel. This trial
covered ordinary object physics. Guest editing and live SparkLink
control remain open. Evidence is in `build/native-remote-forge-hmv/result.json`.

A separate private Windows x64 OpenCE-protocol run on the same cache
verified **Move Selected Copy**, **Undo Move or Rotate**, and **Redo Move
or Rotate** for one armed Warthog through the native Forge menu. Positions
on the host and connected guest matched within 0.02 world units at the
sampled edits. Spark read the moved position and original title and
author from the saved HMV. A late joiner received the edited position.
This establishes translation of a normal base-map vehicle. Evidence is
in `build/native-remote-forge-transform/result.json`.

A second native session verified **Rotate Selected Copy**, undo, redo
and late join on an armed Warthog. Read-only runtime samples taken from
the same native object identity showed the final heading on all three
instances to within 0.003 degrees. Spark read 88.08 degrees of yaw and
the original creator metadata from the saved HMV. This used Windows x64,
the OpenCE protocol and loopback. Evidence is in
`build/native-remote-forge-rotate/result.json`.

On Windows x64, two consecutive offline native host sessions showed the
decoded title and accented author, saved all four metadata fields,
reopened the Warthog through **Load Saved (.HMV)** after restarting, and
saved again. Original map hashes and import bytes matched afterward.
Evidence: `build/native-forge-hmv-creator-credits-final/result.json`
and `reload.jpg` in the same directory.

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
