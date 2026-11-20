# Third-party notices

Forerunner's combined source and builds use [AGPL-3.0-or-later](LICENSE). The original [GPL-3.0 text](LICENSE.GPL-3.0), source-file notices and upstream licenses accompany inherited code. Keep each contribution's original terms, attribution and corresponding source requirements. The [runtime provenance matrix](UPSTREAM_FEATURE_MATRIX.md) identifies reviewed revisions and what was reused.

## Code, art and tools

| Component | Terms and provenance |
| --- | --- |
| Fractum, OpenCE and ChupathingyCE | The combined foundation and modern OpenCE are AGPL-3.0-or-later. ChupathingyCE code from 0.8.0, including native HMV Forge and Delta code, retains GPL-3.0-or-later terms. See [the inherited license history](README.md#license) for earlier releases and decompilation provenance. |
| OpenH1 and Faber | Preserve the adapted files' license and contributor notices. OpenH1 audio and VR attribution and Faber references are recorded in [the runtime provenance matrix](UPSTREAM_FEATURE_MATRIX.md). The [inherited credits](README.md#credits) describe Mota's permission for the Faber-derived career record. |
| Packaged libraries, fonts and data | The [component manifest](port/notices/components.json) lists each package's shipped components, source links, copyright notices and license texts. Native packages generate `THIRD-PARTY-NOTICES.txt` from that manifest, and web builds generate `licenses.txt`. |
| Halo PC Restored HUD artwork | Aerocatia and Jessie retain their credits under the applicable permission. See the [artwork-specific notice](port/assets/hud/NOTICE.md). |
| CMT Elite FP Arms | CMT, MMAMONSTERRR-D92209 and Teh lag are credited. The archive readme requests credit. Redistribution permission is unverified. These arms and derived maps are outside public bundles. [Original archive listing](https://www.halomaps.org/hce/detail.cfm?fid=5764) |
| Invader | Optional external tool under its own [GPL-3.0 license](https://github.com/SnowyMouse/invader). Private test use does not incorporate its binary or source into Forerunner. |

Halo game data, music, maps and trademarks retain their owners' rights. The code licenses grant no redistribution rights in those assets. Use locally owned files for private development and playtesting.

For map packages and release notes, carry forward the creator, feature or asset, original source, license or express permission, and any required notice. Preserve unverified permission status in the authoring record. Public exports may include third-party assets only where the applicable terms authorize distribution. The runtime [integration contract](SPARK_INTEGRATION_CONTRACT.md) records the existing Spark handoff.
