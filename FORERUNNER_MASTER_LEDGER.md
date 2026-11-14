# Forerunner engineering checkpoint

Updated: 14 November 2026. The persistent engineering goal remains active.
This is an incomplete engine integration, not a release or gameplay acceptance claim.
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

Advance one native synchronized Firefight encounter using compatible Faber code.
The user's anti-overengineering directive narrows this milestone; Extraction,
Forge redesign, MegaWorld and broad rewrites are outside its scope.
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
| OpenH1 VR | Source/platform/protocol investigation completed | Optional adapter integration and physical headset + desktop-peer combat |
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

The next source-only carrier repair removes a dead tracked parent from the native
garbage list while its animation still owes a child-spawn attempt. Native
`unit_test_spawning` releases that hold after setting its completion flag; actual
children continue through the existing tracker. This prevents automatic corpse
collection from silently cancelling the pending burst and clearing the wave.
Direct native destruction and zero-child rolls remain native; no synthetic spawn,
new tracking table or timeout was added. No tests/builds/queries/games ran.
Memory-pressure, carrier-child and creator-map gameplay remain unaccepted; the
historical Zombie damage/physics crash is not claimed repaired.
