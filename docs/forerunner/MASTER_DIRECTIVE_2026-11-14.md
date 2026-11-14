# PROJECT FORERUNNER — MASTER ENGINEERING DIRECTIVE
## Unified Halo 1 Runtime | November 14, 2026

**Mission:** Engineer Forerunner, a unified, open-source Halo 1 runtime integrating the strongest compatible, proven capabilities from the community's engine forks into one stable, extensible, cross-platform game engine.

**Product vision:** One engine that plays classic Halo and supports modern multiplayer, co-op, Firefight, Extraction, Zombies, Forge, VR, and large connected worlds.

Forerunner is developed independently alongside **Project Guilty Spark**, the universal Halo editing suite.

This is an implementation assignment, not a research-only assignment. Research must directly support verified engineering work.

### 1. Investigate and consolidate the ecosystem

Clone and inspect the latest relevant source from:

- https://github.com/OpenCommunityEdition/OpenCE
- https://github.com/ChupathingyCE/chupathingyce
- https://github.com/OpenH1/goath1
- https://github.com/FractumSeraph/OpenCE
- https://github.com/SutlabSol/OpenCE-Faber

Also investigate GulchCE, DamnationCE, relevant console ports, and other maintained forks providing worthwhile, compatible improvements.

For every repository:

- Read AGENTS.md, README, contribution policies, and license files.
- Record the exact upstream commit and common ancestry.
- Identify functioning features, architecture, outstanding bugs, regressions, and build requirements.
- Inspect the actual implementation, not merely README claims.
- Identify features already shared between forks to prevent duplication.
- Compare renderers, simulation, networking, AI, assets, game modes, platform support, and tooling.
- Build and test candidates whenever practical.
- Record which changes can be merged, adapted, independently implemented, or must remain optional.

**Select the strongest tested foundation.** FractumSeraph's existing combination of OpenCE and Chupathingy deserves particular consideration. Do not assume it is automatically the correct baseline.

Preserve upstream commit history, authorship, license notices, and traceability. Never describe another developer's work as original Forerunner code.

Maintain an upstream integration ledger and avoid reimplementing functioning features.

### 2. Establish the Forerunner engine

Create an independent, clean Forerunner repository in its own designated workspace.

Requirements:

- One authoritative engine source tree.
- Compatible integration of upstream features rather than blind repository concatenation.
- Modular renderers, platform support, game-mode systems, and optional capabilities.
- Reproducible builds, dependency pinning, automated tests, and release packaging.
- Desktop, dedicated-server, and VR execution modes sharing the same underlying gameplay engine.
- Reliable source-level synchronization with relevant upstream improvements.
- No nested source repositories or generated build archives committed to the main repository.
- A tested, playable baseline before advanced feature integration.

Forerunner should preserve the original 30 Hz simulation behavior where required for Halo compatibility, while rendering independently at higher refresh rates.

Treat deterministic simulation and network compatibility as critical design constraints.

### 3. Preserve the best existing features

Integrate or retain compatible features across these categories:

**Core Halo**
- Original campaign and multiplayer behavior
- Existing Halo game types
- Original physics, vehicles, weapons, AI, and interactions
- Split-screen and cooperative campaign
- Custom maps, supported asset formats, and mod packs

**Multiplayer**
- Up to 128 players where supported and validated
- Dedicated servers, browser, matchmaking/discovery, invites
- LAN and online play
- Cross-platform multiplayer
- Map downloading with validation
- Voice/text chat and moderation
- Server administration, statistics, and game voting
- Predictable reconnection and session recovery

**Graphics and audio**
- Vulkan and Metal renderer integration
- Existing OpenGL support where beneficial
- Modern resolutions and refresh rates
- Improved lighting, shadows, particles, and shaders
- High-resolution HUD and text
- Spatial audio and surround sound
- Performance optimizations without simulation regressions

**Creation systems**
- Faber's implemented Forge and Firefight functionality, subject to verification
- Runtime spawning and live object editing
- Tag inspection and supported editing
- Theater and replay systems
- Existing creator, debug, and diagnostic features

Retain classic rendering and gameplay options.

Do not replace stable implementations merely because another fork has a newer or more ambitious one.

### 4. Native Firefight system

Implement a complete, data-driven Firefight system.

Support:

- Customizable enemy waves, rounds, sets, and survival objectives
- Squad composition, difficulty, behavior, and patrol routes
- Covenant, Flood, Sentinel, and other available AI factions
- Dropships, reinforcements, bosses, and escalating difficulty
- Generator defense, territory defense, survival, and objective variants
- Friendly AI, ammunition drops, equipment, and vehicle support
- Respawns, shared lives, scoring, skulls, and difficulty modifiers
- Cooperative gameplay and synchronized AI
- Saveable Firefight configurations and map-specific encounters

Investigate and reuse Faber's actual implementations where appropriate.

**Acceptance:** Multiple players can finish a Firefight match with synchronized enemy movement, damage, deaths, reinforcements, scoring, and completion conditions.

### 5. Native Extraction system

Create a Halo 1 PvE/PvP/PvPvE extraction mode with a complete authoritative gameplay loop.

Implement:

- Insertion and extraction locations
- Squad deployment and cooperative objectives
- Searchable containers and collectible items
- Loot tables, inventory, and equipment management
- AI patrols, reinforcements, and hostile encounters
- Contracts, terminals, mission events, and objectives
- Extraction timers, activation conditions, and evacuation vehicles
- Risk/reward systems, persistent progression, and configurable loss-on-death rules
- PvE, PvP, and mixed PvPvE configurations
- Server-authoritative item ownership and objective state
- Configurable session duration and difficulty

Create a native objective state machine and shared AI/spawn infrastructure rather than embedding game-mode behavior in hardcoded map scripts.

**Acceptance:** Players can deploy, acquire objectives or loot, fight enemies, reach an extraction point, successfully extract, and have their results persist correctly. Death and failed extraction must behave predictably.

### 6. Zombies, Infection, and Firefight variants

Support both traditional multiplayer Infection and AI-driven Zombie Apocalypse gameplay.

Implement configurable infection rules, AI Flood waves, survivor objectives, barricades, dynamic spawn locations, equipment scarcity, and optional extraction-based endings.

Share systems with Firefight and Extraction without forcing their distinct gameplay rules into one monolithic implementation.

### 7. OpenH1 VR integration

Preserve and integrate the most useful OpenH1 enhancements.

Support:

- OpenXR headset tracking
- Motion controllers and VR input
- Two-handed weapon interaction where supported
- Physical reloads, holsters, aiming, and interactions
- Comfortable locomotion and vehicle operation
- Optional snap/smooth turning and comfort settings
- Correct HUD, audio positioning, and camera handling
- Desktop and VR players sharing gameplay sessions when compatible

Determine actual platform support from source and tests. Do not claim Windows VR support merely because desktop Windows works; OpenH1's documented VR builds currently focus on Linux and compatible Steam Frame/OpenXR systems.

Add native VR editing support through shared runtime object manipulation interfaces.

**Acceptance:** A VR player can join an appropriate desktop session and participate in real synchronized combat, with headset/controller tracking working correctly.

### 8. MegaWorld and connected environments

Develop Forerunner support for Guilty Spark's larger-world ambitions:

- Multiple connected Halo environments
- Up to 19-map MegaWorld layouts as a design target
- Traversable canyons, caves, tunnels, and mountain passes
- Safe BSP/world transitions and visibility management
- Loading/streaming strategies with measurable performance
- Stable physics, player movement, vehicles, AI, and world state across boundaries
- Persistent objectives and encounters
- World navigation, extraction regions, and scalable PvPvE

Use tested Halo BSP behavior and engine limitations to select the implementation. Do not pretend that ordinary independent maps already constitute one seamless world.

Prove two-way player and vehicle traversal before expanding the system.

### 9. Native integration with Guilty Spark

**Spark owns authoring. Forerunner owns execution.**

Design a documented, versioned Spark–Forerunner integration interface.

Forerunner must expose capabilities for:

- Runtime discovery, versioning, and feature negotiation
- Start, stop, restart, and connect-to-game operations
- Live object placement, movement, rotation, deletion, and inspection
- AI encounter creation and configuration
- Firefight wave, squad, and objective configuration
- Extraction zones, loot tables, and mission setup
- Multiplayer game-type markers and rule editing
- Lighting, fog, environment, and weather parameter changes where supported
- Runtime debugging, performance statistics, and error reporting
- Preview, test, validate, save, and export operations
- Undo/redo transaction support and conflict detection

Develop engine-side adapters and a reference integration harness. Coordinate interface changes through a versioned specification so the separate Spark Codex session can implement the editor-side client.

Prioritize native Halo tag and scenario formats where supported.

Use explicit, versioned sidecar data for extended features rather than inventing incompatible proprietary Halo tag extensions.

Provide a capability matrix showing what can be exported to each target, including Xbox retail, PC, CE, MCC, Chupathingy, OpenCE, and Forerunner.

Legacy export conversions belong primarily to Spark. Never claim an unsupported gameplay feature works on a target runtime without testing it.

### 10. Stability, networking, and compatibility

Treat these as release-blocking requirements:

- Deterministic simulation across supported platforms
- Stable AI synchronization and authoritative state
- No phantom actors, duplicated pickups, or conflicting objective ownership
- Correct player joins, disconnects, deaths, respawns, and reconnection
- Stable vehicle occupancy, physics, collision, and damage
- Correct save/load and scenario reset behavior
- Network version negotiation and explicit compatibility checks
- No silent downgrade or corruption of existing multiplayer protocols
- Graceful rejection of unsupported configurations
- Validated map downloads and secure handling of untrusted files
- Bounds checking, fuzz testing of parsers, and resource exhaustion protection
- Repeatable performance testing and regression coverage

Use separate network compatibility profiles where necessary. Advanced features may require Forerunner-capable peers; do not sacrifice functioning vanilla cross-play to falsely claim universal compatibility.

### 11. Sacred files, licensing, and repository safety

These requirements are mandatory.

- Never delete, move, overwrite, or modify users' existing Halo installations, disc images, original maps, runtime folders, saved games, or personal assets without explicit authorization.
- Never perform destructive cleanup based solely on directory location, file size, or an assumption that files are disposable.
- Work in isolated source, staging, and test directories.
- Build and compile disposable copies; never test destructive map operations on originals.
- Use explicit permissions for publishing or replacing output files.
- Avoid modifying the separate Guilty Spark repository.
- Preserve upstream licensing, credits, and source history.
- Maintain compliance with AGPL/GPL and all compatible third-party licenses.
- Do not distribute Microsoft's copyrighted game data or other unauthorized proprietary assets.
- Request human review before public releases, upstream submissions, or external publication.
- Do not push changes or open third-party pull requests without authorization.

Use source-only commits; generated binaries, caches, and temporary assets belong outside tracked source.

### 12. Engineering execution order

**Phase A — Foundation**

Inventory all repositories, build candidate baselines, perform licensing checks, compare real features, select the foundation, and establish Forerunner's clean build and test pipeline.

**Phase B — Unification**

Integrate missing core improvements incrementally. Prove normal Halo gameplay, multiplayer, cooperative play, map loading, and compatible platform builds still work.

**Phase C — Playable Firefight**

Build and test complete Firefight gameplay, including multiplayer AI synchronization.

**Phase D — Extraction and Zombies**

Build complete objective, persistence, inventory, extraction, and survival systems on top of the shared gameplay architecture.

**Phase E — VR and advanced rendering**

Integrate working VR and modern rendering without compromising non-VR behavior.

**Phase F — Spark integration and MegaWorld**

Implement the engine-facing editing API, live world modifications, extended map support, and large connected-world gameplay.

**Phase G — Product completion**

Finish installer/release packaging, documentation, automated tests, performance work, accessibility, regression repair, and user acceptance testing.

These phases establish dependencies, not an excuse to defer unrelated fixes. Work on independent improvements in parallel where safe.

### 13. Evidence-based completion

Maintain:

- `FORERUNNER_MASTER_LEDGER.md`
- `UPSTREAM_FEATURE_MATRIX.md`
- `COMPATIBILITY_MATRIX.md`
- `SPARK_INTEGRATION_CONTRACT.md`
- `TEST_EVIDENCE.md`
- `KNOWN_LIMITATIONS.md`

Every meaningful feature requires a source implementation, tests, and an observable acceptance result.

Distinguish clearly between implemented, compiled, automated-tested, gameplay-tested, partially working, blocked, and unsupported.

Never substitute mocked gameplay, generated status reports, or passing unit tests for a demonstrated playable feature.

Report concise evidence: changed files, commit identifiers, actual build/test results, gameplay proof, failures, and next blockers.

### 14. Immediate instructions

Begin by examining the available environment, upstream repositories, and current engineering state.

Choose a dedicated workspace that cannot disturb Guilty Spark or protected user data.

Build the strongest suitable baseline. Establish a passing regression suite. Start real integration immediately after the baseline decision.

**Do not stop after planning or documentation. Implement, compile, test, repair, and commit working improvements.**

The objective is a cohesive, functioning unified Halo runtime—not a collection of unfinished forks inside one repository.