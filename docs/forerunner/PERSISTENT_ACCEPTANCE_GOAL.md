# FORERUNNER — PERSISTENT ENGINEERING GOAL

## Product Completion V2

- **P0 — Shared foundation.** Complete authoritative AI across campaign,
  Zombie Survival, Firefight, Extraction and Forge. Extend existing native
  neighboring-BSP rendering, preloading, collision and residency for Spark's
  compact MegaWorld. Audit the 16 gameplay slots, 32 reader entries and
  16-bit membership before any expansion. Share verified limits and fixtures
  through the existing Spark contract.
- **P1 — Flagship gameplay.** Preserve released ground Extraction. Build out
  synchronized combat, independent squads, inventory, loot, objectives and
  revivals. Restore original Zombie Apocalypse, complete Firefight and
  Reach-style Forge, finish local ragdolls, third-person aiming and player
  customization, and reuse native recorded aircraft behavior.
- **P2 — Unified runtime.** Complete six-player campaign co-op, cinematics,
  checkpoints, independent Operations, optional graphics and performance,
  cross-platform rendering, OpenXR, servers, voice, session interoperability,
  Theater, machinima, 128-player validation and compatible restored content.
- **P3 — Spark.** Extend existing integration into supported live editing,
  content loading, gameplay configuration, validation, transactions and
  runtime readback. Spark owns authored content and Forerunner owns execution.

Keep independent gameplay and creator tracks moving. Batch meaningful changes
with proportionate native checks, preserve Latest and one Previous rollback,
and use the existing contract for cross-project coordination. The following
accepted checks and requirements still apply.

## Prior Product Completion priorities and retained gates

1. **Playable MegaWorld.** Accept Spark's compact hex-cluster world, including
   underground interiors. Prove independent multiplayer traversal, combat, AI,
   vehicles, spawning, equipment and persistent world state through normal game
   controls. Review the complete 19-region overhead and underground Atlas
   before further construction. Pin original BSP identities, transforms, surface
   cover and connections, and compare native readback with the reviewed plan.
   Keep corridor maps as engineering fixtures.
2. **Flagship Extraction.** Launch from ordinary menus inside MegaWorld. Verify
   both factions and chosen colors, intuitive bases, exploration, objectives,
   loot, revivals, native aircraft evacuation, banking, persistence and
   redeployment without mandatory raid timers. Black Echo is an operation in
   MegaWorld. Preserve standalone evidence without tuning or replaying its
   waypoint routes. Reuse Halo's recorded animation, hover, seat and departure
   systems, with correctly authored flight recordings. Include Engineers,
   wildlife, roaming patrols, dynamic faction encounters and Flood threats.
3. **Survival and Firefight.** Compare against Fonzie's original Demo before
   enhancing Survival. Verify Flood behavior, carriers, infection, Demolishers,
   weapon and flame effects, and audio. Support the original maps and MegaWorld.
   Firefight needs configurable waves, rounds, sets, factions, bosses, skulls,
   lives, generator defense and scoring, with host-authoritative AI, damage,
   animation, deaths and objectives synchronized across clients. Keep Survival,
   Firefight and Extraction distinct, with locked gametypes and editable host rules.
4. **Unified runtime.** Preserve all remaining engine goals, including
   interoperability, classic performance, optional graphics, campaign/co-op,
   Forge, live editing, machinima, VR, dedicated servers, verified 128-player
   support and cross-platform releases. Reach-style Forge acceptance covers
   Monitor/player switching, searchable categorized palettes with previews,
   duplication, deletion, precision transforms and snapping, and normal, fixed
   and phased physics. Verify AI, vehicles, weapons, objectives, doors,
   teleporters and gameplay properties through multiplayer editing, save,
   reopen and immediate playtest.
5. **Optional ragdolls.** Review Halo Content Evolved's existing implementation
   before adapting it. Expose the choice in Gameplay across campaign, co-op and
   multiplayer. Verify the original death animations with it off. Preserve
   authoritative collision, Flood infection and legacy network compatibility.
   Halo Content Evolved already uses native collision and separates the
   displayed pose from host physics. Use its presentation path to keep
   corpse position and infection authoritative in every mode.
   [Reviewed upstream implementation](https://github.com/Watanoge/halo-content-evolved/blob/dcdfdd23524cd65b4f7849c238481acdbd416cee/port/linux/game/biped_ragdoll.c)

Before writing equivalents, review Halo Content Evolved, Faber, OpenCE-Forge,
Chupathingy, OpenH1, Invader and the Halo Blender toolset as relevant. Validate
related changes together in normal editor and game workflows. Promote playable
feature batches through Latest with one Previous rollback. Preserve asset safety,
licensing, attribution, security and existing platform requirements. No pushes
or paid CI without authorization.

## Complete the Unified Halo 1 Engine

Your assignment is to carry Project Forerunner from its current state to a genuinely playable, stable, unified engine.

**Success means all selected, compatible best-in-class capabilities from OpenCE, Chupathingy, OpenH1, FractumSeraph, Faber, and other relevant forks are integrated into one working runtime.**

Do not consider repository creation, successful compilation, or a large number of passing unit tests sufficient proof of completion.

The final acceptance target is:

1. **Unified engine:** One Forerunner executable architecture launches classic Halo and retains the selected functionality from each upstream project.
2. **Classic compatibility:** Campaign, multiplayer, co-op, supported maps, and conventional game types function without regressions.
3. **Firefight:** Multiple players successfully complete synchronized enemy waves, objectives, and victory conditions.
4. **Extraction:** Players deploy, collect objectives and loot, encounter synchronized AI, extract, and receive correct persistent results.
5. **Zombies:** Playable Infection and AI-driven zombie survival with configurable rules.
6. **VR:** A supported VR player can play alongside desktop players, with correctly synchronized gameplay.
7. **Forge and live editing:** Objects, spawns, objectives, and supported tags can be manipulated in-engine with validated persistence.
8. **Spark integration:** The versioned editing interface supports end-to-end create → configure → launch → test → revise → save workflows.
9. **MegaWorld:** Connected environments support verified two-way traversal and persistent world behavior.
10. **Releases:** Supported platforms have installable builds, documented limitations, and successful regression and gameplay acceptance testing.

**Working method**

- Pick the highest-priority unresolved blocker.
- Trace it to the actual source and root cause.
- Implement the smallest coherent production-quality solution.
- Build and run the relevant tests.
- Perform native gameplay validation where possible.
- Fix regressions and commit verified changes.
- Update the evidence and feature ledger.
- Continue to the next unresolved requirement.

Do not repeatedly rewrite plans, manufacture placeholder implementations, or claim success from code inspection alone.

When upstream projects change, evaluate worthwhile updates without disrupting working functionality.

Respect other developers, credit their contributions, and preserve all licensing and user-data safeguards.

If a requirement is technically impossible, legally restricted, or incompatible with existing protocols, document concrete evidence, implement the best safe alternative, and explain the limitation rather than pretending it is complete.

When a session ends or reaches an execution limit, leave a durable checkpoint containing the current commit, passing and failing tests, unresolved blockers, and the precise next engineering action. Never claim work continued when execution stopped.

**Your operating objective is continuous, measurable progress toward a finished product—not continuous planning.**

### The ultimate test

Launch Forerunner.

Load a map created in Guilty Spark.

Join with desktop and supported VR players.

Fight synchronized Firefight waves.

Transition into an Extraction objective.

Collect loot, survive, reach an extraction point, and successfully complete the mission.

Return to Spark, change the encounter and extraction configuration, relaunch, and verify the changes in gameplay.

Preserve ordinary Halo multiplayer throughout.

**When these workflows function reliably, and the accepted feature matrix has passed its required tests, Forerunner has achieved its primary mission.**

Build the engine. Prove the gameplay. Finish the integration.

**FORERUNNER — ONE ENGINE. EVERY FORK.**
