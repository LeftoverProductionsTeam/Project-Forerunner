# PROJECT FORERUNNER — FINAL MASTER ENGINEERING DIRECTIVE
## ONE ENGINE. EVERY FORK.
### THE COMPLETE MODERN HALO 1 ENGINE

**Mission:** Finish Forerunner as one cohesive, stable, cross-platform Halo 1 engine combining the strongest compatible functionality from OpenCE, Chupathingy, FractumSeraph, Faber, OpenH1 and other relevant community implementations.

Not a new Halo clone, not a collection of disconnected engines and not just a Firefight or Extraction project.

### 1. Continue the established engine

Work only within `D:\Project Forerunner` on its existing local development branch. Reconcile actual HEAD and Git status; do not automatically change branches or rewrite history.

Preserve the complete master directive, Spartan Operations specification, engineering ledger and compatibility matrix.

Reuse functioning upstream code and preserve attribution. Do not merge entire incompatible forks or create duplicate gameplay, rendering or networking subsystems.

Spark remains read-only.

### 2. P0 — Repair CI and finish real dropship Extraction

**First repair the development gate.**

Recent GitHub Build/Web workflows are failing before usable build verification. The Build `gate` job fails without recording any execution steps.

Identify the actual workflow, runner or repository configuration failure. Do not guess or weaken required safeguards.

**Then close the major gameplay blocker.**

The existing Pelican test has passed native boarding, insertion and disembarkation. Return flight and valid landing remain unfinished.

The engine previously ejected its pilot after a grounded rollover. The aircraft also missed its required landing volume.

Finish the native aircraft round trip:

1. Select faction and equipment.
2. Enter Pelican or Spirit through normal vehicle interaction.
3. Fly to the insertion zone and land.
4. Disembark normally.
5. Complete actual combat, rescue, collection and mission objectives.
6. Request extraction and defend the landing zone.
7. Return the aircraft using authentic pilot behavior and physical navigation.
8. Land within the actual extraction area.
9. Physically board the correct aircraft.
10. Depart through native flight and cinematic behavior.
11. Bank recovered equipment, calculate casualties/rewards and display debrief.
12. Restart and redeploy using the same saved item identities.

Do not use player teleportation, fake boarding, unsafe AI shortcuts or bypassed collision/settlement rules.

Fix the native cause of flight failure, preserving ordinary Banshee and vehicle behavior.

First prove the loop in an isolated native environment. Then prove the Spark-authored Black Echo mission with two authenticated clients.

### 3. Complete UNSC/Covenant factions

Provide actual playable faction selection with:

- Compatible character models, interaction height and controls.
- Starting weapons, equipment, loadouts and inventory.
- Native allegiance, friendly-fire and AI relationships.
- Faction-specific objectives, mission markers and HUD.
- Pelican versus Spirit insertion and extraction.
- Correct persistent equipment ownership, death and redeployment.

Preserve verified faction swapping, waypoint updates and existing combat synchronization.

### 4. Complete independent Spartan Operations

Support real, authoritative multiplayer operations:

- Maximum six players per squad.
- Multiple independent squads and missions on the same server.
- Separate squad ownership, objective progress, loot and results.
- PvE, PvP and PvPvE configurations.
- Squad cooperation, rivalries and temporary alliances.
- Recoverable gear, looting and configurable loss rules.
- Downed-but-alive stabilization, timed biofoam revival and true death.
- Persistent weapons, ammunition, upgrades, loadouts and progression.
- Squad-specific audio, HUD messages and mission events.
- Reconnect-safe transactions and no duplicated rewards.

Reuse native HaloScript for ordinary map behavior. Add only the minimum authoritative per-squad state that shared scenario globals cannot safely represent.

Six-instance tests are not proof of six independent authenticated clients. Prove that separately.

### 5. Complete original Halo gameplay and native modes

**Classic Halo**
- Preserve campaign, physics, AI, vehicles, weapons and conventional multiplayer.
- Complete stable campaign co-op, including expanded six-player support.
- Repair co-op camera, cutscenes, checkpoints, death/revival and player loading.
- Preserve traditional game types, LAN, dedicated servers and compatible custom maps.

**Firefight**
- Real synchronized AI spawning, combat, damage and death.
- Waves, rounds, sets, bosses, dropships and reinforcements.
- Difficulty, skulls, shared lives, scoring, supplies and objectives.
- Working HUD, victory, failure, replay and multiplayer recovery.

**Zombie Apocalypse**
- Restore original zombie carrier/weapon/vehicle mechanisms and automatic release.
- Provide proper AI-driven survival, Flood hordes, scavenging, barricades and objectives.
- Support optional evacuation, rewards and survival progression.
- Keep traditional player Infection distinct from AI Zombie Apocalypse.

**Forge**
- Integrate compatible Faber/OpenCE Forge implementations where proven.
- Native object placement, movement, rotation, deletion, spawning and editing.
- Working multiplayer replication, persistence, loading and Spark integration.
- Do not create a second editor engine.

### 6. Complete MegaWorld

The target remains the 19 classic Halo maps integrated into a genuinely connected world.

First prove two concurrently active, physically connected regions with:

- Two-way walking and vehicle traversal.
- Valid caves, canyons, tunnels and mountain paths.
- Consistent collision, lighting, visibility and navigation.
- Persistent players, AI, vehicles, objects and mission state.
- Independent squads operating simultaneously in different regions.
- Authoritative cross-region networking and extraction outcomes.

Then expand toward all 19 regions.

No fake seamless teleportation or forced global BSP switching. Profile engine limits and build a working supported implementation.

### 7. Complete VR and enhanced graphics

Integrate appropriate OpenH1/GOAT H1 functionality through existing renderer/input interfaces.

**VR**
- OpenXR tracking and controller interaction.
- Two-handed weapons, aiming, physical reloads and holsters.
- Vehicle controls, locomotion, comfort options, spatial audio and HUD.
- VR plus desktop multiplayer where protocol compatibility is proven.
- Engine interfaces enabling Spark's in-headset Forge and level editing.

**Rendering**
- Preserve optional original Classic presentation.
- Support reviewed Vulkan/Metal/OpenGL paths and appropriate platform fallbacks.
- Enhanced lighting, HDR, ambient occlusion, ray-traced AO where supported, shadows, materials and effects.
- Antialiasing, filtering, LOD, FOV, VRR/frame limits and resolution controls.
- GPU lightmap compatibility and relevant source-authored environment changes.
- Maintain simulation correctness and measured performance.

Do not duplicate renderers or claim headset support without real hardware verification.

### 8. Complete multiplayer and ecosystem integration

Finish:

- Host-authoritative synchronized players, AI, vehicles, combat and loot.
- Join, leave, reconnect, host/session recovery and reliable inventory state.
- Dedicated servers, server browser, map voting and custom game hosting.
- Verified map downloading and compatibility checks.
- Native voice/text chat, squad channels and appropriate moderation.
- Cross-platform networking and supported cross-fork compatibility.
- Safe network version negotiation; no fake universal crossplay.
- Native Linux, Windows, macOS and other supported platform builds.
- Long-term 128-player/horde scalability, proven through actual performance and network tests.
- Replay/theater, recording, cinematic cameras and machinima support.

Keep engine execution compatible with genuinely supported Xbox, PC, CE, MCC and community content where their formats and protocols permit.

### 9. Make Spark integration complete

**Spark authors. Forerunner executes.**

Use the existing `SPARK_INTEGRATION_CONTRACT.md`, not a second interface or coordination server.

Provide stable, versioned support for:

- Runtime discovery, capabilities and launch.
- Authored scenario content, factions, AI and mission objectives.
- Firefight, Extraction, Zombies and Forge configuration.
- Live object operations and safe runtime readback.
- Lighting/environment changes where genuinely supported.
- Camera, cinematics, audio, HUD and mission event handling.
- Native validation, multiplayer state and meaningful error reporting.

Preserve legacy content compatibility. Use explicit optional sidecars instead of inventing incompatible native tags.

### 10. Eliminate engineering waste

The audit identified excessive repeated native setup, large test/evidence churn, failing CI and repeated attempts at unchanged aircraft behavior.

**New execution discipline:**

1. Finish complete gameplay features, not isolated infrastructure.
2. Reuse existing engine modules and test helpers.
3. Investigate root causes before repeating failed native runs.
4. After two unsuccessful approaches, reassess using captured evidence.
5. Run focused tests for changed behavior and broader checks at milestones.
6. Reuse unchanged, verified build inputs without weakening isolation.
7. Keep useful regression tests; do not endlessly expand the harness.
8. Preserve history without rewriting enormous documentation files every cycle.
9. Avoid duplicate state machines, quest engines, renderers, inventories or network stacks.
10. Commit small, coherent implementations and move forward.

If a major feature is genuinely blocked, advance independent engine work rather than repeatedly reproducing the same failure.

### 11. Safety and compatibility rules

Protect original game installations, user maps, ISOs, saves, accounts and assets.

Use owned disposable staging. Retain previous-good source and evidence. Delete only verified inactive generated output.

Preserve upstream licenses, authorship and contribution policies. No unauthorized proprietary asset distribution.

Never alter Spark, publish, push or submit upstream contributions without permission.

The current development policy permits focused compilation, automated regression tests and previously authorized bounded native tests. Human gameplay, hardware, stress and public-release acceptance remain distinct authorization gates.

### 12. Final acceptance

The engine is complete only when actual players can:

- Play classic Halo without regressions.
- Complete synchronized Firefight and Zombie matches.
- Create, deploy, fight, extract, bank and redeploy in Extraction.
- Use both UNSC and Covenant where supported.
- Complete independent squad missions.
- Traverse connected MegaWorld regions.
- Create/edit supported objects through Spark/Forge.
- Play in supported VR and desktop sessions.
- Install stable platform-native releases and join supported multiplayer servers.

Do not substitute compilation, fixtures or source presence for gameplay proof.

**ONE ENGINE. EVERY FORK.**