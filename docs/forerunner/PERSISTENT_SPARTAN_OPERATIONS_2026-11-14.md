# FORERUNNER — PERSISTENT SPARTAN OPERATIONS ENGINE

Implement complete cooperative and PvPvE Extraction missions using existing Halo engine systems and Spark-authored maps.

### Reuse existing implementations

Build on `extraction.c`, `gear.c`, native co-op/network replication, player inventory, AI encounters, Firefight, HaloScript, HUD messaging, native audio and existing mission transitions.

Do not build a second game engine, inventory authority, quest runtime or networking stack where existing systems can be extended.

### Squads and world

- Maximum six players per squad.
- Multiple independent squads on one shared server.
- Target up to 128 simultaneous players after actual capacity and performance verification.
- Squads may spawn in completely different regions.
- Each squad receives independent quests, objectives and progress.
- Squads can cooperate, form temporary alliances or fight each other.
- Allow configurable PvE, PvPvE and friendly-fire rules.

Server authority must control membership, alliances, objectives, loot and extraction outcomes.

### Quest execution

Consume the reviewed mission data produced by Spark through the existing integration contract.

Reuse native HaloScript for ordinary map and campaign behavior.

Add only the necessary bounded, server-authoritative per-squad mission state for operations that cannot safely use shared global scenario scripts.

Support:

- Ordered and branching objectives
- AI kills, defense, rescue and escort completion
- Technology collection and object interaction
- Trigger and terminal activation
- Reinforcement waves and escalating enemy activity
- Squad-specific failure and success
- Optional shared-world consequences
- Mission rewards, replay and redeployment

Track event identities so retries, reconnects and duplicate network messages cannot award objectives twice.

### PvPvE, looting and alliances

Players may fight Covenant and Flood while competing with other squads for recovered equipment.

Rival squads can attack each other, steal recoverable gear after death, or cooperate to complete difficult objectives.

Support agreed-upon temporary alliances without merging persistent ownership or mission progress accidentally.

Use existing native weapon and object ownership wherever possible.

All drops, pickups, transfers, losses, returns and settlements must be controlled by the host. Protect against duplicate items, stale ownership, disconnect exploits and checkpoint rewinds.

### Halo-appropriate revival

Implement a downed-but-alive state, inspired by Halo 5, with MJOLNIR emergency stabilization and biofoam treatment.

- A critically injured Spartan becomes incapacitated.
- Teammates can stabilize and revive them through a timed interaction.
- Reviving is interruptible by combat, range, state changes and death.
- Configurable medical equipment and repair abilities may help.
- Distinguish actual death from incapacitation.
- Dead Spartans cannot be medically revived.
- Death releases eligible carried loot according to configured PvPvE rules.
- A subsequent deployment uses the persistent gear/redeployment system, not resurrection.

Reuse native damage, HUD, interactions, animations and audio where feasible. Do not replace the combat damage system.

### Persistent progression

Extend the existing approved native gear records.

Support saved loadouts, recovered weapons, ammunition, inventory expansion, armor upgrades and weapon modifications where the engine genuinely supports them.

Preserve classic Halo weapon physics and behavior by default.

Use bounded, optional extension data when upgrades cannot fit native Halo tags. Never invent compatibility with retail runtimes or modify protected maps.

### Audio and mission presentation

Use existing game sound/HUD systems to execute Spark-authored:

- Radio messages and voice dialogue
- Objective notifications and subtitles
- Localized ambient sounds and alarms
- AI combat warnings and distress calls
- Extraction signaling
- Briefings, scripted sequences and cinematic cues

Scope audio correctly to players, squads, nearby listeners or the shared world. Prevent duplicated cues after reconnect or state recovery.

### MegaWorld

Reuse and extend campaign BSP technology where practical, but do not assume classic single-active-BSP behavior can support independent squads.

First prove two concurrently active, physically connected regions with independent missions, shared state and preserved players/vehicles/AI.

Then expand toward the full 19-region MegaWorld.

No fake seamless transitions, global squad teleportation or mandatory synchronized BSP switching.

Profile CPU, memory, networking, AI and replication as player and region counts increase.

### First acceptance target

Run Spark's **Operation Black Echo** in Forerunner.

Verify a complete deployment, rescue mission, enemy encounter, technology pickup, objective escalation, extraction, durable reward and redeployment.

Then verify two independent squads, cooperation, hostile encounters, downed-state revival, death looting, reconnects and correct ownership.

Use focused builds and isolated multiplayer smoke tests during development. Do not claim full gameplay acceptance until actually demonstrated.

Keep Spark read-only, protect original assets, preserve attribution and never push without authorization.

**Deliver a playable, persistent multiplayer operation—not just source code that describes one.**