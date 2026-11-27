# Forerunner upstream feature matrix

Reviewed 2026-11-14. These are pinned source snapshots, not a claim that every
advertised feature has passed Forerunner acceptance. Source presence, compilation,
automated checks, and gameplay evidence are separate gates. See
[TEST_EVIDENCE.md](TEST_EVIDENCE.md) for run results.

## Foundation and provenance

The selected Windows x86 foundation is FractumSeraph's combined OpenCE/Chupathingy tree at
`eae91445333bc8ebfb1604e08fcedffbd5c3a558`. It contains the inspected OpenCE
HEAD as an ancestor; their left/right divergence is 0/3,734 commits. Its common
ancestor with inspected Chupathingy HEAD is
`a42702df52416de4008e91fe18c49f94d9efabb0` (1/1,760 commits).
Ancestry establishes traceability, not behavioral equivalence.

The sole missing Chupathingy commit, `78d28a70839998775322af8aa571fa494966c0c8`,
was cherry-picked with `-x` as `b2bd604329c71f06c1109c632ff19ea4c29e4027`,
retaining Milenko's authorship. It validates Workshop filenames and preview paths
in `port/linux/src/steam_workshop.c`, with parser/fuzzer regressions. The overlap
with Fractum's narrower dot-name check was resolved using the stronger upstream
component validator. Validation of the resulting tree is recorded separately.

| Repository | Inspected commit | Common ancestor with inspected OpenCE | Decision |
| --- | --- | --- | --- |
| [OpenCE](https://github.com/OpenCommunityEdition/OpenCE) | `1519c65facbb068e44b956da2769198a83a17847` | Same commit | Retain current fixes through the combined foundation; use as comparison build. |
| [Chupathingy](https://github.com/ChupathingyCE/chupathingyce) | `78d28a70839998775322af8aa571fa494966c0c8` | Shared ancestry is preserved through Fractum; see comparison above | Retain combined implementations; import missing changes individually with author credit. |
| [FractumSeraph/OpenCE](https://github.com/FractumSeraph/OpenCE) | `eae91445333bc8ebfb1604e08fcedffbd5c3a558` | `1519c65facbb068e44b956da2769198a83a17847` | Selected after repairs, Windows x86 compilation, swapped-host native movement/combat and hidden OpenGL gameplay. Other acceptance gates remain open. |
| [OpenCE-Faber](https://github.com/SutlabSol/OpenCE-Faber) | `e4ffe0fe9874870b4fca1fca76beb83514c793f4` | `da8d455623cf50002e6a798b1980068fd3ca0292` | Adapt Firefight and object editing incrementally. Whole-branch adoption would replace newer core fixes. |
| [Watanoge/halo-content-evolved](https://github.com/Watanoge/halo-content-evolved) | `61fd80aafc3e1af551124516c4c6915cafd16511` | Pinned commit inspected read-only from the local Git object database | Tag import, bundles, profiles, Forge, Firefight, Floodfection, randomizer and client-side Delta baseline. See the [implementation audit](#halo-content-evolved-at-61fd80aa). |
| [Mota's OpenCE-Forge, main](https://github.com/SutlabSol/OpenCE-Forge) | `91099ce375632167e425edd7fd40dadf83ab93d2` | Source refresh, 13 commits after the prior `2467e708` pin | Version 2.10.11 source uses Forge protocol `0x4630`, separate from stock sessions. Reused compatible OpenCE transfer commits `ebfa05b7` and `796df195` with original authorship; Forge streaming/placement and live compatibility remain unimplemented/unverified here. |
| [OpenH1/GOAT H1, dev](https://github.com/OpenH1/goath1) | `baed77571d53abe32b27434b4ba869f11db00fe9` | `6ec42856d73e258e5a8c7e9cd19161f46d89d0e4` | Separate optional VR/renderer adaptation; do not merge its protocol and simulation changes wholesale. |
| [GulchCE](https://github.com/chestahh/GulchCE) | `66b4df0bc9afeb09189a7f9c0fff0c5b1f5d51b2` | `1519c65facbb068e44b956da2769198a83a17847` | Compare individual MCC fixes against already inherited MCC modules; preserve map-family boundaries. |
| [DamnationCE](https://github.com/xshxdex98/DamnationCE) | `d31736d3748f1156995e6bba13126f87aeff40ee` | `0b2c6759bb4cb3ee5dcc3d650de2dcda74d79620` | Older CE/co-op reference; avoid copying AI/network systems already present in newer ancestry. |
| [OpenCE-PS4](https://github.com/iHaiDeeZ/OpenCE-PS4) | `dcf1b836085829999a004afef86bb3c45d361642` | No common Git ancestor: patch/toolchain repository | Optional console adapter. `patches/BASE.txt` separately pins engine base `9662f0f6cb60746444bee9549e4271e387fdc659`. |

## Optional path tracing source audit

- [OpenH1 master at 1b00fcfb](https://github.com/OpenH1/goath1/blob/1b00fcfb1210cbed44e3495f6292a747ca2c6899/docs/rt-plan/4-path-tracing.md) documents an optional opaque path tracer sharing Halo material records, scene acceleration structures, native sky and lights, raster transparencies and the HUD. Its performance figures are targets and have no Forerunner measurement.
- [NVIDIA DXVK-Remix direct SDK](https://github.com/NVIDIAGameWorks/dxvk-remix/blob/c63f9dd9eb81e7351e4d5823882952e49753f4f5/documentation/RemixSDK.md) can accept camera, mesh, material, and light data without intercepting Forerunner's legacy draw calls. The zlib-licensed renderer's Windows SDK uses a versioned C interface in the runtime library and owns its acceleration structures. Review bundled binary terms separately. Compare a thin optional adapter over Forerunner's scene data with the native Vulkan path before deciding which renderer to ship.
- [TheGreatHMMMM's Halo compatibility mod](https://github.com/TheGreatHMMMM/halo-rtx-compat/tree/2eaf31db8a5580dd812a38649f153696a65523e3) targets the 2003 PC executable (GPL-3.0). Use it to compare Halo material and effects handling. The [Halo DXVK-Remix fork](https://github.com/TheGreatHMMMM/halo-dxvk-remix/tree/d35f037e463ca7bb17809f0e7764c4c151d5cf09) is zlib-licensed, and [OpenRemix](https://github.com/TheGreatHMMMM/OpenRemix/tree/13c3a1001be4d5eb40a8037c136ca6efca8461eb) offers a Remix API-compatible vendor-agnostic candidate under Apache-2.0. Runtime compatibility, visual fidelity, hardware coverage, frame time, VRAM and Spark controls remain unverified.

### Direct integration decision

- First test the official Remix C SDK with one stock scene on Windows x64. Submit the resident render BSP and one native model, camera and light through the versioned API. Prove GPU-only output handoff into the existing compositor before adding another scene or shipping controls. The SDK exposes an external Vulkan swapchain path, but sharing images and synchronization with Forerunner's bgfx device is unproven. Its public API takes mesh data and instances and builds its own acceleration structures. The native Vulkan route is the one that can share Forerunner-owned structures when its render-geometry ray-tracing support exists. Keep the two paths on the same native geometry, material and light inputs. [Pinned SDK header](https://github.com/NVIDIAGameWorks/dxvk-remix/blob/c63f9dd9eb81e7351e4d5823882952e49753f4f5/public/include/remix/remix_c.h)
- Use the Halo PC compatibility mod as a fidelity reference for materials and effects. Its D3D9 proxy is tied to the PC executable. Keep the Halo DXVK fork as a comparison when the official SDK reproduces a Halo-specific gap. Test OpenRemix as the alternative provider if the official path cannot meet interop, licensing or device support. Its advertised cross-platform builds and vendor-agnostic design still need driver and API feature checks. OpenH1's native plan remains the fallback architecture, including shared render-geometry acceleration and native raster passes for unsupported effects. Its Metal proposal covers hybrid ray tracing. [OpenRemix platform notes](https://github.com/TheGreatHMMMM/OpenRemix/blob/13c3a1001be4d5eb40a8037c136ca6efca8461eb/docs/platforms.md)
- The Remix C header carries MIT terms and the DXVK-Remix source carries zlib terms. Check the exact runtime and third-party binary redistribution terms separately, especially optional NVIDIA AI components. Retain upstream notices and authorship for any actual reuse. [DXVK-Remix notices](https://github.com/NVIDIAGameWorks/dxvk-remix/blob/c63f9dd9eb81e7351e4d5823882952e49753f4f5/ThirdPartyLicenses.txt)
- Accept with identical native inputs in Classic, Hybrid and Path Traced, starting with native resolution and frame generation off. Record rendered FPS, median and 95th-percentile frame time, peak dedicated VRAM and system RAM, and repeated BSP transitions to check resource retirement. Compare sky and baked lighting mood, emissives, foliage cutouts, glass and water, decals and particles, weapon silhouettes and HUD. Test native and upscaled modes and denoiser settings independently. The Windows host enumerates an RTX 4070 Ti and AMD integrated graphics. Enumeration proves no backend support or performance. A second RT-capable vendor needs its own run. Missing capabilities must leave Classic and Hybrid playable. Publish only verified graphics settings and native-tag material overrides through Spark's existing discovery, launch and readback.

### Native direct SDK startup result

NVIDIA RTX Remix 1.6.1 x64 release archive SHA-256
`7dd1d44492a609050e44b0e3581cec31135d1ed88b4a91123b4faae91955716a`.
The packaged DLL `81b7c6407361d376ed9274f9c52f6de2b09dfcee385cd2c3e651baca538b0321`
reports `remix-main+5a7eec84`; the public release tree pins source
`0a6cd5b1ee814ec390c358e0c9c32ece8895ddc0`. Keep these identities separate.
The matching public 0.6.5 C SDK sample initialized against the package and
returned success from 120 direct Present calls at 800×450 on an RTX 4070 Ti.
It used NVIDIA's sample geometry and direct window presentation. The runtime
logged a shutdown warning about 41 device objects. Default crash reporting
initialized with no probe-specific settings file. No consent was granted.
Retained logs and package identity: `build/remix-1.6.1-direct-probe`.
The temporary download, expanded runtime and probe executable were removed.
[Official release](https://github.com/NVIDIAGameWorks/rtx-remix/releases/tag/remix-1.6.1)
· [Sample source](https://github.com/NVIDIAGameWorks/dxvk-remix/blob/0a6cd5b1ee814ec390c358e0c9c32ece8895ddc0/tests/rtx/apps/RemixAPI_C/remixapi_example_c.c)

The public implementation returns a raw image and two binary semaphores
owned by the Remix Vulkan device. Forerunner's independent bgfx device cannot
use those handles directly. The next interop test needs a supported
cross-device memory/synchronization export or shared device integration.
Reserve the provider choice until this path has evidence. This probe did
not load Halo geometry, verify compositor output or measure gameplay frame
times and memory. No provider, binary or Spark capability was added.
[External presenter source](https://github.com/NVIDIAGameWorks/dxvk-remix/blob/0a6cd5b1ee814ec390c358e0c9c32ece8895ddc0/src/d3d9/d3d9_swapchain_external.cpp)

**Native D3D11 sharing result.** The same pinned 1.6.1 package accepted a
caller-created DXVK D3D9Ex device and returned a handle for an 800×450 shared
render-target texture. DXVK reported that it could not write the shared
resource metadata, and a native hardware D3D11 device rejected
`OpenSharedResource` with `0x80070057`. No final-color copy or compositor
frame was verified. The public D3D9 texture code uses an opaque Vulkan Win32
KMT handle for this path. A different consumer must prove import of that
specific handle type and synchronization on the selected adapter, or use a
provider with explicit shared-device or external resource support. Do not
treat the returned handle alone as proof of compatibility. Evidence and
package identity are in `build/remix-1.6.1-shared-image`.
[Shared-texture source](https://github.com/NVIDIAGameWorks/dxvk-remix/blob/0a6cd5b1ee814ec390c358e0c9c32ece8895ddc0/src/d3d9/d3d9_common_texture.cpp)

**Windows Vulkan sharing result.** Native driver queries accept export and
import of the tested mutable BGRA8 opaque Win32 KMT image and a Win32 binary
semaphore on both the RTX 4070 Ti and AMD integrated adapter. On the RTX, a
producer and consumer on independent Vulkan logical devices shared a
32×32 image using that memory handle and an exported semaphore. The producer
released queue ownership to EXTERNAL and the consumer acquired it before
copying. A verification readback matched all 1,024 pixels. Logs are in
`build/vulkan-win32-image-preflight` and `build/vulkan-win32-image-roundtrip`.
This proves GPU/driver interop for an explicitly described allocation, not
a Remix frame or AMD path-tracing support. Vulkan requires the exporter's
allocation size and memory type for opaque handle imports. The official Remix
public handoff reviewed here supplies neither with its KMT handle and does
not supply compatible exported semaphore payloads for that texture.
[Win32 memory import specification](https://docs.vulkan.org/refpages/latest/refpages/source/VkImportMemoryWin32HandleInfoKHR.html)

OpenRemix already exposes a shared-frame contract with allocation size,
memory type, GPU UUID, and paired acquire/release semaphores. The pinned
implementation exports Linux file descriptors. Prefer a provider using that
existing synchronization contract for the first compositor integration.
On Windows evaluate adapting the same contract to Win32 external handles
using the verified transport above. Then submit an owned Halo BSP and model,
validate composition and compare gameplay frame times, memory and fidelity
against the stock paths before adding settings or Spark capability discovery.
[OpenRemix shared-frame API](https://github.com/TheGreatHMMMM/OpenRemix/blob/13c3a1001be4d5eb40a8037c136ca6efca8461eb/public/include/remix/remix_editor_ext.h)
· [OpenRemix implementation](https://github.com/TheGreatHMMMM/OpenRemix/blob/13c3a1001be4d5eb40a8037c136ca6efca8461eb/src/native/runtime/shared_frame.cpp)

**OpenRemix Windows exporter.** The [pinned provider patch](tools/openremix-patches/0001-export-shared-frames-on-win32.patch)
extends the existing headless frame allocator and queue synchronization.
The Windows ABI returns caller-owned opaque NT handles for the RGBA8
image and paired semaphores with allocation size, memory type and GPU UUID.
The Linux FD ABI remains intact. Patch application and Windows x64 syntax
checks passed on the pinned provider's `device.cpp`, `runtime.cpp`, and
`shared_frame.cpp` using their actual transitive headers. The matching NT
RGBA8 memory and semaphore transport passed a native RTX 4070 Ti pixel
round trip (1,024/1,024). Logs: `build/openremix-shared-frame-win32` and
`build/openremix-win32-transport-roundtrip`.

The same patch now links in one private native Windows build. Provider
`498d633959daff7cb66a9034c3e5e641f0b70c1b7cdcbe1a3f448f942df531a2`
rendered three successive path-traced frames of OpenRemix's upstream sample
at native 320×180 on the RTX 4070 Ti. An independent Vulkan device matched
the exported GPU UUID, imported the NT memory and both semaphore payloads,
then waited, acquired, copied for verification, released, and signaled each
frame. The initially blank frames preceded provider readiness. The provider
reported path tracing active on frames 9–11, with three different RGB
checksums from the imported image. Upscaling and denoising were disabled.
Evidence and the single incremental build are in `build/openremix-win64`
(`consumer-cold-start-runtime.log`). Next import these frames into the
Forerunner compositor and render Halo geometry; no Spark graphics capability
is advertised yet.

**Forerunner shared-frame import.** The Windows64 Vulkan build now includes
the native image importer and EXTERNAL queue ownership barriers. With the
same pinned OpenRemix provider, the native probe ran those functions on an
independent Vulkan device. It rejected mismatched GPU identity and allocation
size, imported the exact provider image, and acquired and released three
successive path-traced frames with distinct checksums. Evidence is in
`build/openremix-win64/consumer-forerunner-bridge-runtime.log`.
The Windows64 engine build `133B8775` initially linked the importer.

The next Windows64 build `C63CD3AC61D6AA8299FB82F6DF39C453D450D60250E920785B05A45AF2283419`
adds GPU-only transfer into a regular bgfx RGBA8 texture. The same-thread
Vulkan renderer waits on ready, copies, releases ownership and signals the
provider. A separate native Win32 window running the game's patched bgfx
library imported and copied three active OpenRemix sample frames with
different checksums from the bgfx-owned destination. Setup and warm-up
frames are recorded in `build/openremix-win64/consumer-bgfx-window-runtime.log`.
The probe used readback only to verify the GPU copy. Draw the resulting
texture through Forerunner's presenter with Halo geometry before exposing
the renderer option or Spark capabilities.

**Stock Halo geometry and camera.** Private Windows64 build
`639324A139D4E768E96A3BE846674D46F549EEFE46C7A607279E7A824BB612D0`
captured the loaded Xbox Blood Gulch render BSP and the same-tick raster
frame. The native map SHA-256 is
`50FE52406F075D975E24100A65B26FF696458023DD3509878953052AB0EF858F`.
The scene snapshot SHA-256 is
`D41DA4C4D917DF0C48A04E1D34D6DE5EA783A76C82C6187ACC3B8984FFC8EB13`.
The private adapter traced 5,503 triangles from 79 source material groups
at 640×480 and copied the result through bgfx Vulkan. The adapter converts
camera handedness and reverses triangle winding for the reflected basis.
The matched images show the same base entrance and canyon silhouette.
Evidence is in `build/native-bsp-pathtrace-stock-bloodgulch/window00684.bmp`,
`build/openremix-win64/native-bloodgulch-matched-handed-bgfx.bmp` and
`build/openremix-win64/consumer-native-bsp-matched-handed-runtime.log`.
The probe uses a neutral material and a test light with upscaling and
denoising disabled. Live in-game presentation, stock materials, lighting,
sky, dynamic objects, HUD composition, hardware coverage and real gameplay
frame time and memory measurements remain open. No renderer mode or Spark
capability is advertised from this probe.


**Native BSP material data.** The native scene visitor now supplies each
bounded material section directly from the loaded render BSP, including
the shader and permutation, the baked lightmap group and page, and both
UV sets. The stock Blood Gulch smoke run on Windows64 build `29972727`
matched all 79 material groups, 5,790 vertices and 5,503 triangles from
the earlier capture exactly. It also read 5,534 secondary UVs. The
snapshot SHA-256 is
`4f38e623f0c9a02c01e68692d9dac1e5907a72e2ba2a718d01be6dfde33e36ec`.
Evidence is in `build/native-bsp-pathtrace-stock-bloodgulch/process-lightmap-stream-one-player.log`
and the adjacent `data/bloodgulch-lightmaps.obj`. Provider-side material
uploads and live HUD composition are pending.


**Stock material upload.** Windows64 build `CC6DEF98` captured 25
distinct native bitmaps through Halo's texture cache and CPU sampler.
The stock Blood Gulch geometry and UVs match the earlier capture. The
private OpenRemix probe uploaded all 10 resolved base textures for the
35 supported environment material sections and used a neutral material
on the other 44 sections. Three active frames reached bgfx Vulkan with
different checksums. The reference view shows the original red-base
walls and cliff colors. The probe uses the same earlier camera and a
test light. It captures baked lightmap bitmaps, but their contribution
is pending, as are terrain detail layers, live HUD composition,
cross-hardware coverage and gameplay performance measurements.
Evidence is in `build/native-bsp-pathtrace-stock-bloodgulch/process-materials.log`,
`build/openremix-win64/consumer-native-bsp-matched-textured-runtime.log`
and `build/openremix-win64/native-bloodgulch-matched-textured-bgfx.bmp`.
No runtime renderer option or Spark capability has been released.

**Live in-game preview.** Windows64 build
`6328B8BBB4F043B899C3A153480EC8BFDD8577B7A905D66564CFE5AD97A28FB6`
submitted the resident Blood Gulch BSP, the current player camera and
native base textures straight to the pinned OpenRemix C SDK. Forerunner
copied the active Vulkan frame at an 800×600 render extent and drew the
native HUD afterward. The in-game view shows the traced canyon, weapon
reticle, ammo, shields and radar. The provider bound 35 of 79 material
sections and uploaded 5,505,024 bytes of decoded base textures. The other
sections used the neutral preview material. This is upload payload size,
separate from actual GPU memory use.
Evidence is in
`build/native-bsp-pathtrace-stock-bloodgulch/process-live-pathtrace-vulkan-textured.log`
and `live-pathtrace-vulkan-textured/window00146.bmp` beneath the same folder.
The screenshot SHA-256 is
`a12695d35872f9fbc15f485107e3cf57ff1f7188b08056fe65e04a89230d8c60`.

The preview is private and requires the pinned OpenRemix SDK include
directory during Windows64 Vulkan build configuration and an explicitly
selected trusted local DLL. Release builds need no provider dependency.
It currently replaces the raster world color behind the HUD, so the
sky, viewmodel, dynamic objects, transparencies, baked lighting and
terrain detail layers are missing or incomplete. It uses a test light
and native resolution with denoising disabled. An isolated scripted-look
run kept the HUD over the updated view, and a missing provider returned
to the native raster view with simulation continuing beyond tick 150.
Evidence is in `process-live-pathtrace-vulkan-motion.log` and
`process-live-pathtrace-missing-provider.log` beside the earlier log.
Alternate GPUs, motion fidelity, performance, VRAM usage, resize and
long-run acceptance remain separate gates for a supported mode. Latest,
Previous and Spark are unchanged.

**Native sky and first person.** Windows64 build
`E84F12A5E1CBE2071A17857B136811F660E5A5C2F66FB5B581B784AF71C7BB38`
keeps the original draw order. The preview uses the native first-person
stencil and far-depth coverage, painting the traced color only over
covered world pixels. Blood Gulch shows the stock sky, weapon, hands, ammo
display and HUD around the traced terrain. It was visually verified
with antialiasing off and with 2× MSAA. The viewmodel visibility setting
still hides the weapon over the traced scene. The final runtime log is
`build/native-bsp-pathtrace-stock-bloodgulch/process-live-pathtrace-vulkan-layer-verified.log`
and its visible frame is
`live-pathtrace-vulkan-layer-verified/window00156.bmp` in the same folder.
The sky is raster-composited. It still needs to illuminate the traced
world alongside the baked lightmaps and native lights. Dynamic scene
coverage and performance certification are still open.

**Authored outdoor lighting.** Windows64 preview build
`15E62ABE8152D7072366E5A441E9DEC6D657C335C676A210B62F20B63B6BF950`
uses the current native sky slot. It submits that sky's strongest outdoor
directional light and ambience to the pinned OpenRemix provider. Blood
Gulch's live sun direction was `(-0.754, -0.643, -0.133)` in provider
coordinates. Sidewinder used `(0.668, -0.438, 0.601)` on the preceding
integration build `594A3DB8`. Both showed visibly distinct traced lighting
with the native sky and first-person viewmodel. Chill Out stayed in
raster at its indoor spawn without a trace mesh upload in the final build.
Evidence is under `build/native-bsp-pathtrace-stock-bloodgulch` in
`process-live-pathtrace-authored-sky-*.log` and the adjacent capture folders.
Baked lighting, indoor clusters and the provider's single-sun limit remain
open. Sidewinder showed white scenery silhouettes at the sky edge where the
current depth-based composite needs a BSP coverage mask.
[Invader HEK sky tag layout](https://github.com/SnowyMouse/invader/blob/master/src/tag/hek/definition/sky.json)

**Opaque BSP coverage.** The private preview now composites over the visible
opaque foreground BSP depth owner. Native scenery, vehicles, sky, viewmodels,
and later transparency and decals keep their existing raster passes. In the
matched stock Sidewinder pair under `build/native-bsp-pathtrace-stock-bloodgulch`,
tree-trunk and Ghost pixel samples matched the native image exactly while the
snow showed traced lighting. The final Win64 executable
`5b63063e2ed96b4ba0ecef1a5b86c4f8daee4c2383482bb3221b7660245ba7ca`
also captured these elements with 2× MSAA in
`live-pathtrace-coverage-sidewinder-msaa/window00149.bmp`. These are private
renderer smoke checks. Dynamic objects still do not participate in traced
lighting, and baked indoor lighting, regional scenes and performance remain open.

**Native BSP emitters.** The private OpenRemix preview now sends opaque Halo
shader radiosity power and emitted RGB as material emission, with separate
material identities for shaders sharing a base texture. Stock Xbox Chill Out
`81819e9d7037a6b6c4e9a7472a431ebb53ed25ea00d8af634ac8384f1fe13d8b` on
Win64 `945fcefd8cd8fc3ea086f6e372b9213125751ff4d96e6b8369d3dcc7c146b264`
uploaded two emitting materials and drew with zero sun and sky ambient. At
the same spawn, the traced and Classic frames retain the same HUD and weapon.
The traced lighting is darker and noisier than the original. Logs and
`window00320.bmp` are under `build/native-bsp-pathtrace-stock-bloodgulch`,
in the `pathtrace-radiosity-chillout-{traced,raster}` trial sets. This does
not transfer baked irradiance or shader-animation masks. An indoor BSP with
no supported emitters still uses the native raster frame.

**Native-resolution sample budget.** Private Win64 preview
`91a8700dec8531a6193eb9ce43b989d5a1f1495e72957cd63dd725f6d8ceb3e9`
exposes `debug.pathtrace_samples` (1–16, default 1) through the existing
runtime config. It forwards the value to OpenRemix without changing native
resolution. The originally retained provider was configured with `nrd=false`.
NRD 4.18.0 carries a separate NVIDIA RTX SDK license. It remains disabled
pending license review and native validation.

Stock Chill Out at the same fixed spawn on Windows Vulkan, RTX 4070 Ti,
VSync off and a 200 FPS ceiling. FPS is the existing engine frame-time log
over eight steady-state seconds. Memory is Windows process dedicated GPU
usage (peak of six one-second samples), with working set recorded separately.
These private renderer runs do not cover combat, camera motion or simultaneous
regions. Four samples still leave visible noise.

| Resolution | Mode | Mean FPS | Median / p95 ms | Dedicated GPU MiB |
|---|---|---:|---:|---:|
| 800×600 | Classic | 200.0 | 5.000 / 5.019 | 422 |
| 800×600 | Traced, 1 sample | 199.6 | 4.991 / 5.406 | 1053 |
| 800×600 | Traced, 4 samples | 103.3 | 9.607 / 10.395 | 1110 |
| 1920×1080 | Classic | 200.0 | 5.000 / 5.006 | 471 |
| 1920×1080 | Traced, 1 sample | 48.3 | 16.050 / 40.479 | 1642 |
| 1920×1080 | Traced, 4 samples | 28.8 | 34.605 / 35.286 | 1696 |

Raw frame times, memory samples, logs and captures are retained under
`build/native-bsp-pathtrace-stock-bloodgulch` in the `pathtrace-performance-*`
trial sets. [Pinned NRD license](https://github.com/NVIDIA-RTX/NRD/blob/fca9486957dececb7a7d5b5646d0829c667a01cf/LICENSE.txt)

**Optional FSR 3.1.** The private OpenRemix preview accepts
`debug.pathtrace_fsr` with Off (default), Quality, Balanced or Performance.
It reuses OpenRemix's existing Vulkan FSR 3.1.4, verifies the active backend
and internal resolution with the provider's reconstruction diagnostics,
and composites the native weapon and HUD at output resolution. The existing
BSP upload generation is sent as the camera world epoch for temporal history.
The FidelityFX MIT notice is retained with the pinned provider dependency;
it must accompany any distribution of that provider.
[FidelityFX SDK license](https://github.com/GPUOpen-LibrariesAndSDKs/FidelityFX-SDK/blob/c6efa6bf7f2027b3ec94f28578bb5965eabb9e55/LICENSE.txt)

Same fixed stock Chill Out view, RTX 4070 Ti and 1920×1080 output, one
sample per pixel, denoising and frame generation off. First build
`d8036d6420dcb30e23a0a7d9ee2c5c84c2c16220deff5ec5a890ac9609848788`.
Frame times are engine submissions over eight steady-state seconds and
memory is peak sampled Windows process dedicated GPU usage. Captures,
frame times and memory logs are under `build/native-bsp-pathtrace-stock-bloodgulch`
in the `pathtrace-fsr-1080p-*` trial sets. FSR retained the HUD and weapon but
indoor sampling noise remains visible. This fixed view does not verify
motion, BSP-transition history, or another adapter.

| Mode | Internal resolution | FPS | Median / p95 ms | Dedicated GPU MiB |
|---|---:|---:|---:|---:|
| Off | 1920×1080 | 62.6 | 14.371 / 21.756 | 1696 |
| Quality | 1280×720 | 115.8 | 8.429 / 9.855 | 1485 |
| Balanced | 1129×635 | 141.3 | 7.026 / 7.767 | 1357 |
| Performance | 960×540 | 166.1 | 5.973 / 6.840 | 1329 |

Final executable
`eed25fb13921ed5bd70d23a7668cc1fb97ee34d591608d5ff10afc7ff2e6ae67`
reports reconstruction only after the traced backend becomes active. The
`pathtrace-fsr-final-*` native repeat verified Quality at 1280×720 and
confirmed that an invalid preference retains native 1920×1080 tracing.

**Native temporal filter.** The private preview's
`debug.pathtrace_temporal` setting defaults to OpenRemix's existing temporal
resolve. Off exposes raw native-resolution tracing. Stock Chill Out at
1080p on the RTX 4070 Ti reported 16 history frames with the filter active,
and zero in the raw run. NRD was inactive in both. In a matched wall crop,
mean adjacent-pixel luminance difference fell from 4.70 to 3.59 on a 0–255
scale. Grain remains visible. The raw cold run required about 12 seconds
of provider configuration preparation before the native trace became ready.

The rotating-camera harness on the same map verified FSR Quality at
1280×720 → 1920×1080 with the FSR resolve active and the native filter
inactive. It averaged 143 FPS in this scripted view, with the weapon and
HUD retained. This is same-BSP motion evidence. Cross-BSP history is open.
Private log and capture sets `pathtrace-temporal-*` are under
`build/native-bsp-pathtrace-stock-bloodgulch`. Filter and motion build
`520833e05a557688afaca42d1bda934dd08fba06dd48dabd8d9049046c4db3e4`.
Final raw build with provider status diagnostics
`137f11e9adc7881a04d6019039db4cf3c9834f5bf1f364523514b4734db6da18`.

**Adapter preflight.** Win64
`1efd81f8890d03d570c6ebc3779fddc433c48ff927d316aff481730dfdb03b12`
checks the provider's ray-tracing capability after device startup, before
exporting a shared image or uploading BSP assets. While the provider
prepares, the engine displays its native raster view. It still requires an
exact adapter UUID match before importing any provider GPU resource.

Stock Chill Out at 800×600, with process-local selection of each installed
Vulkan driver, imported successfully on both tested adapters. RTX 4070 Ti
activated the trace with 936 MiB peak process working set. AMD Radeon
integrated (1002:164e, driver 26.3.1) reported tracing capability and
continued raster gameplay through tick 1110, but did not activate tracing
within 44 seconds. Limiting the provider to one pipeline worker still
reached 19.5 GiB peak process working set. These are CPU working sets, not
dedicated GPU memory. AMD tracing is unverified and this configuration
fails the release resource check despite the positive capability bit.
The driver/compiler memory cause is unconfirmed. Native logs are in
`build/native-bsp-pathtrace-stock-bloodgulch/process-live-pathtrace-adapter-workers-*.log`.
A Vulkan device reporting the ray-tracing capability absent remains
untested.

**Camera and map history.** Win64
`fc03ac491d6066a6011b876c5d80ecf6ff6c13115c37c364ac4791dae0f24970`
passes native director cuts and sky changes to the provider as history
epochs. This covers bookmarks, camera-mode changes, restored games and
instant scripted cuts. A separate world epoch still identifies each BSP
upload. In the private native round trip log
`build/native-bsp-pathtrace-stock-bloodgulch/process-live-pathtrace-history-roundtrip.log`,
two Chill Out bookmark cuts kept world epoch 1 and advanced history to
2 and 3. Switching to Sidewinder and back to Chill Out re-uploaded 80
then 45 sections and advanced world epochs to 2 and 3. The provider
confirmed resets 1 through 5 in order. The bounded harness ended the
process after the console quit request failed to close it. The renderer
checks had already passed. This checks changing maps in one process.
A physical BSP seam and FSR history across that seam remain open.

**MegaWorld base textures.** The private preview now resolves MCC bitmap
identities through the same converted texture registry as the native
renderer and reuses its ARGB decoder, including DXT and BC7. The prior
Win64 preview displayed the released MegaWorld `fcfe9d44` spawn with zero
uploaded texture bytes. Win64
`e3984d6dbc22d3afd567804ce5d1d4fa96edeb8d5bec3500d65caaeb7708fdf4`
on the same map/receipt `d7c07e86` and BSP 0 uploaded 33,423,360 bytes
across 90 of 152 opaque sections and composited at the same player spawn.
The 32 MiB upload budget remains. Some surfaces still use fallback
materials, and this dark spawn is insufficient to accept visual fidelity.
Logs and matched screenshots are under `build/native-bsp-pathtrace-stock-bloodgulch`
in the `pathtrace-megaworld-released-{spawn,textured-final}` sets.
These are scripted startup renderer checks. Physical seam traversal and
matched lighting comparisons remain open.

**MegaWorld texture budget.** Win64
`593ad2131fc6d646ec2c248b3b2c5f4e377780f22a460eebba08b404cd1601b8`
preplans unique BSP textures and fits them using the map's authored mips.
On released `fcfe9d44` BSP 0, 54 textures used a coarser mip and 152/152
opaque sections bound textures within 33,423,360 bytes. The matched stock
Sidewinder control retained 80/80 textured sections and 8,650,752 bytes
with no mip reduction. The private Vulkan logs and views are in
`build/native-bsp-pathtrace-stock-bloodgulch`, trial sets
`pathtrace-megaworld-released-mip-final` and
`pathtrace-megaworld-stock-mip-control`. This measures coverage at the
tested BSPs. Visual quality and cross-BSP streaming costs remain open.

**Physical Infinity seam.** Win64
`4b9175e24be19e9cf7e0c9ebb7707b14555fcb8bdeeefdc807e0b7144002a7c1`
keeps the native frame and skips preview mesh and texture uploads in
a BSP with neither sky lighting nor a supported emitting shader. In a
private solo trial on the retained `9eb74c92` map and `f50dc937` receipt,
one setup placement preceded normal inputs for both legs of BSP 0→12→0.
The player kept full vitality. BSP 12 was the held foreground at tick 300.
BSP 0 held again at tick 600. FSR Quality resumed at 533×400 → 800×600,
all 149 opaque sections were textured again, and the provider confirmed
the new history epoch. The original dark tunnel remains visible in the
raster capture. This does not accept the old tunnel art, a traced
interior, the released map's geometry, or multiplayer. The log is
`build/native-bsp-pathtrace-stock-bloodgulch/process-live-pathtrace-megaworld-infinity-9eb74c92-fsr-return.log`.
Adjacent capture `window00904.bmp` is the tunnel and `window01667.bmp`
is the return outdoors.

**Released MegaWorld seam check.** The same preview executable also
passed BSP 0→12→0 on the released map `fcfe9d44` with receipt `d7c07e86`.
The private solo trial used one setup placement followed by normal
movement. Holds changed from BSP 0 at tick 150 to BSP 12 at tick 300 and
back to BSP 0 at tick 600. The weapons and shields stayed stable during
both switches. Halo's native lighting made the tunnel floor, walls and
ceiling visible. FSR Quality resumed outside at 533×400 → 800×600 with
history reset 5 and all 152 opaque sections textured. The process
exited normally. In `build/native-bsp-pathtrace-stock-bloodgulch`, see
`process-live-pathtrace-megaworld-infinity-release-fsr-return.log` and
the matching capture folder. `window01151.bmp` shows the tunnel and
`window01994.bmp` shows the traced return. This verifies local renderer
fallback and restoration on released content. Multiplayer and traced
indoor lighting remain open.

## Implementation and acceptance boundaries

| Capability | Actual source examined | Current integration decision and evidence |
| --- | --- | --- |
| Classic simulation and independent rendering | `source/game/game_time.c`; `port/linux/src/port_config.c` (`display.interpolation`); `tools/windows_build.py` (`-ffp-contract=off`); vendored musl math | Retain 30 Hz behavior and stock options. OpenCE and Fractum Windows x86 candidates compiled and ran local Blood Gulch player ticks. Full campaign/physics/rendering acceptance remains open. |
| Multiplayer, co-op, dedicated servers | `network_distributed.c`, `network_actors.c`, `network_coop.c`, `network_rules.c`, `server_*.c`; `halo_port_limits.h` | Retain host authority and existing per-peer serializers. Capacity is 128 players/machines in source; a 128-player session has not been demonstrated. Cross-play requires its own native gate. |
| Infection and conventional extra modes | `port/linux/game/infection.c`, `game_modes.c`, `gun_game.c` | Retain the inherited modes. Two verified native clients pass one Longest Infection match: normal movement/skull melee, conversion, zombie victory, lobby return and fresh next countdown (`build/native-infection-longest-complete/result.json`). Human timeout victory, other maps/modes/platforms and human acceptance remain open. |
| Xbox, PC, CE, HaloMD, MCC assets | `map_families.c`, `ce_*.c`, `mcc_*.c`, `steam_workshop.c`; `docs/mcc_maps.md` | Preserve distinct loaders/resource ownership. Xbox smoke tests do not establish CE or MCC fidelity. Workshop parser fix imported; broader format/gameplay testing remains open. |
| Forge and live object editing | Faber `port/forge/src/forge_3d_mode.c`, `forge_network.c`, `forge_hooks.c`; build-time symbol replacement | Real upstream code, not integrated. Windows input/hooks and its separate Forge agreement require adapters. Save/export claims need native readback and target-runtime tests. |
| Firefight | Faber `firefight.c`, `firefight_spawner.c`, `firefight_generators.c`, `firefight_stats.c`, `firefight_network.c`; Forerunner `coop_enemies.c` and native callbacks | Faber-derived actor/wave handling extends existing co-op. Bounded two-account native combat, resupply, healthy paid respawn, victory, defeat/loss, lobby return and restart pass in the current master checkpoint. Authored HaloScript encounters keep the separate finite director OFF. Other content/rules/platforms and human acceptance remain open; no foreign native-struct packet was imported. |
| Extraction and AI zombie survival | No corresponding native mode found in the reviewed foundation/Faber trees; Forerunner `extraction.c`, `gear.c`, existing native inventory/HaloScript/AI/transport | Shared authoritative gear and Extraction are implemented. Two verified clients in separate squads pass an owned native combat/loot/physical Spirit evacuation/independent banking/hub exit/same-ID redeployment slice. Full Spark Black Echo, another mission playthrough, distinct Zombie Apocalypse, larger squads/platforms and human acceptance remain open. Original zombie content must reuse this foundation; asset-extraction utilities are unrelated. |
| Vulkan/Metal | Inherited `tools/bgfx_build.py`, `port/linux/src/d3d8_bgfx.inl`; OpenH1 host renderers | Keep existing optional bgfx path before replacing stable rendering. Windows64/OpenXR compiles and bounded native desktop fallback/interoperability are recorded in TEST_EVIDENCE.md; headset and full backend acceptance remain open. |
| OpenXR, tracked input, hand interaction | OpenH1 `port/linux/host/host_vr.c`, `port/linux/src/vr.c`, `port/linux/game/vr_*.c`; Forerunner `vr_render.c`, `xinput_sdl.c`, `win32_openxr.c` | Optional Windows64 OpenXR uses the existing bgfx/Vulkan backend, tracked aim/stereo, handed input and PTT. Opt-in offered-world-weapon grip pickup adapts OpenH1 `vr_grab.c` / `vr.c` at `baed77571d53abe32b27434b4ba869f11db00fe9`, through native pickup admission. Windows64 compiles; 31 focused input checks and XR-disabled desktop fallback pass (current master ledger). Headset, mixed-session and platform parity remain unverified; full physical reload/holsters are not integrated. |
| Audio, HUD, input | Inherited SDL3 platform/audio/input and HUD modules | Retain existing implementations and artwork notices. Headless dummy-audio runs prove no audible/spatial audio result. |
| Recording and replay | Inherited `network_recording.c`, `guide_record.c`; Fractum viewer/service code | Retain source; replay fidelity and production playback acceptance remain open. Do not treat every recording format as deterministic Theater. |
| Spark adapter and MegaWorld | Forerunner `server_mode.c` capability contract v1, `tools/spark_runtime.py` reference client and native regional transfer code. Spark authors and compiles the connections. | Read-only discovery verified on Win64. Latest ships the ground preview. Private builds have native walking and banking evidence on specific map hashes. Regional transfer log context is implemented, discoverable and verified during native foot crossings at both repaired seams in the private Win64 build. Compact vehicle traversal and complete editor workflow remain open. |

## MegaWorld and authored gameplay upstream review

| Source reviewed | Reuse decision |
| --- | --- |
| [OpenH1 BSP cache at 4549a984](https://github.com/OpenH1/goath1/blob/4549a984dc3a87708c652a77b449af7731686e59/port/linux/game/structure_bsp_cache.c) | Keeps checked byte images by map checksum, file range and original address, with a 128 MiB cap and 32 entries. Restore writes one active window and uses bounded offset reads for inactive materials. Forerunner already reuses resident MCC geometry without copying or revalidation on a warm hit. Use the bounded-read approach where cold-load profiling shows repeated work in that loader. A copied image cannot serve as an independent physics scene. |
| [SutlabSol OpenCE-Forge at 084da2a6](https://github.com/SutlabSol/OpenCE-Forge/blob/084da2a60c9408b460575c95898511acb9d810c5/port/linux/game/forge_network.c) | Holds custom placement until its dependency arrives. The stream bounds offers and chunks, verifies the decompressed digest and schema, and heals placement/object update ordering for late joiners. Adapt that readiness and ordering behind existing Forerunner replication, retaining native HMV. The fork's separate wire protocol needs explicit compatibility. |
| [Watanoge Halo Content Evolved at 9c50f047](https://github.com/Watanoge/halo-content-evolved/tree/9c50f0472dcc17a09d97cc726f628d167eb58913) | Source still separates CE and MCC tag dependencies, native CE export and MCC package export, host Firefight objectives, Floodfection's rendered forms, and deterministic placement rolls after a seed is selected. Keep the feature-level reuse decisions below. [Milenko's merged Delta layout bridge](https://github.com/Watanoge/halo-content-evolved/pull/5) is a separate interoperability comparison with Forerunner's existing bridge. |

Spark can surface supported layouts, dependency profiles, objective roles and seed choices in its existing editors and export dialog. Forerunner owns cache admission, dependency readiness, authoritative objectives and replication. Each reviewed repository carries the AGPL v3 license. Preserve per-file authorship and notices when adapting source, and keep runtime code out of Spark's MIT editor.

## Halo Content Evolved at 61fd80aa

Source audit of [61fd80aafc3e1af551124516c4c6915cafd16511](https://github.com/Watanoge/halo-content-evolved/tree/61fd80aafc3e1af551124516c4c6915cafd16511).
Reuse individual implementations at the owning milestones. MegaWorld stays first.

| Capability | Upstream source finding | Forerunner and Spark handoff |
| --- | --- | --- |
| Native tag import | `port/linux/game/tag_import.c` queues selected roots, walks dependencies and admits loaded tags. Its donor path reads local Xbox v5 caches and compact packs, including compressed Xbox spans. | At the tag-loading milestone, compare the importer with our checked cache schemas and resource ownership. Preserve map, resource and root identity. Spark's current asset library selects an owned source and a root. Keep the loader in Forerunner. |
| Tag bundles and profiles | `docs/custom_edition_caches.md`, `docs/tag_injector.md`, `tag_bundle.c`, `tag_injector.c`, `mcc_tag_bundle.c`, and `port_tag_profiles.c` implement closure, schema validation, dependency bytes and persistent profile selections. CE uses its external resource maps. MCC has its own donor, texture and audio adapters. Import packs and profile `.tagbundle` files serve different paths. | Reuse the bundle reader/writer and validation in the runtime when adopted. Spark already extracts loose tags through `crates/spark-core/src/tag_exchange.rs`. Expose profile selection in the asset library and preserve creator/source/resource identities. Use a supported runtime operation or validated file exchange. Do not add a second bundle writer or profile database. |
| Forge injection | `port/forge/src/forge_runtime_tags.c` imports selected profiles into the Forge palette. `forge_network.c` supplies a separate live protocol. `forge_engine.h` selects Forge protocol `0x4777` at this pin. | Reuse compatible importer, catalog and ordering logic behind Forerunner's existing Forge/SparkLink interfaces. Keep HMV and the supported Mota sidecar path for base placements. Require dependency agreement before spawning imported tags, including on late join; accept a live operation only after a private mixed-runtime test. |
| Standalone map rebaking | `forge_cache_builder.c::forge_export_scenario_map` and `forge_export_mcc_scenario_map` emit native CE and MCC outputs. The MCC path writes a Forge map package and can still rely on the base MCC resource banks. Forge collects saved AI tags before export. | Use those writers where they fit the runtime Forge export milestone. Spark's existing Build & Playtest remains the authoring compiler and can validate a returned map identity, required resources and target support. Test CE and MCC separately. Preserve originals and do not assume a standalone map includes redistribution rights or every resource. |
| Firefight | `firefight.c`, `firefight_spawner.c`, `firefight_objectives.c`, `firefight_generators.c`, `firefight_network.c` and `forge_ai_routes.c` contain a director, lives, skull/vitality scaling, objective progress and authored routes. | Select compatible objective, wave and scaling code where the existing Forerunner director has a gap. Preserve native HaloScript ownership of authored encounters. Spark exposes supported variants, spawns, routes and objectives through its existing Firefight and scenario editors. |
| Floodfection | `infection.c` calls `floodfection_forms.c` for player-controlled infected forms. It blends Flood presentation from existing movement and melee state. | Audit and adapt the forms against Forerunner's existing Infection implementation when that milestone is active. Spark exposes the supported form and tag choices under game-mode authoring. Preserve the original Zombie Apocalypse carrier, projectile and survival mechanics as their own workflow. |
| Campaign randomizer | `randomizer.c` hooks native Xbox, CE and MCC load paths and selects tags for encounters and loadouts. It also exposes pools used by Firefight. | Reuse its reviewed pool selection and actor hooks through the native campaign lifecycle. Spark can author the eligible pool and launch a supported runtime configuration through Build & Playtest. Co-op must prove that the seed, selected content and replacements agree before gameplay. |
| Delta merge | [Milenko's merged client commit](https://github.com/Watanoge/halo-content-evolved/commit/ce9ea00a441275fc79a25058bc27df6ca1df5415) adds advert detection, a join mark and client HELLO/WELCOME/BYE via `network_delta.c` and the MIT `delta_ref.c/.h`. The game still uses the selected native protocol. | Keep Forerunner's existing Delta implementation. Compare reference behavior only where it fills a gap. Verify compatible ordinary joins separately from Forge/Firefight sessions. The merge alone establishes neither mode interoperability nor authenticated identities. |

**Xbox extraction.** The audited version-5 importer is a starting point for a
checked Xbox donor adapter, while the upstream profile export UI and
`tools/ce_tags.c` command cover CE/MCC workflows and CE bundles, respectively.
Spark already discovers
Xbox sources and routes selected tags through its Invader extraction workflow.
First read back one owned v5 weapon with dependencies through that workflow.
Where the toolchain misses a required Xbox resource, reuse the donor header
and inflate logic with the bundle schema walker. Verify compressed and
uncompressed input, tag bounds, bitmap/model/sound resources and unchanged
source hashes. Record capability separately for each cache family.

**License boundary.** This pin's root license and Forerunner's are AGPLv3.
Retain WataNoge, Mota/Faber and other authors' notices and relevant history
when adapting their actual code. The separately marked reference files retain
Milenko x Horrible Dev's MIT notice and the supplied `LICENSES/MIT.txt`.
Spark remains MIT and can drive the separately built runtime or consume data
through its existing interfaces. Keep AGPL engine implementations out of
Spark's MIT source tree. Tag provenance and permission to distribute assets
remain separate requirements. This was source inspection only.

## Licensing and repository rules

Mota's 14 November community lead identifies Watanoge's Forge/Firefight additions
and describes Mota's newer streaming-tag/placement-sidecar work. The inspected
Watanoge snapshot credits Mota and contains `port/forge/src/forge_runtime_tags.c`,
`forge_network.c`, `forge_cache_builder.c` and `port/linux/game/firefight*.c`.
Its documented pack path imports tags on demand, preloads network actors in
matching order, and its saved-map reader also reads placement text from a CE
cache trailer. Mota subsequently supplied his separate OpenCE-Forge main branch;
its sidecar reader/writer and format are now identified in the existing Spark
contract. Watanoge's Forge session selects a distinct network version and uses
fork-specific messages; CE-file loading alone is not live Forge/Firefight
protocol compatibility. `port/linux/TAG_INJECTION.md` requires matching import
roots/order and locally available packs across peers. Our existing `mods.c`
is visual-only replacement, not gameplay-tag admission. No upstream code was
imported or executed in this audit. Follow the existing Spark contract before
adding a placement-file reader; preserve original maps and locally owned data.

Mota's later update refreshed the pin to `91099ce3` (2.10.11). Its Forge-specific
changes cover client-to-host compressed bundle retention/re-serving, held-object
slot mapping, missing-tag cache handling, reliable backlog pacing and MCC preview
decode bounds/swizzle output. Those modules are absent here; do not import a
second editor/preview layer or call them accepted Forerunner capabilities.
The compatible OpenCE map-transfer commits already present in local object
history were cherry-picked with `-x`: `ebfa05b7` and its required slow-peer
follow-up `796df195`. No fetch/remotes/branch changes were needed. Requests stay
within existing wire limits (eight host, two peer, 32 in flight); transfer
arithmetic uses 64-bit intermediates and catalog streams close each serving
frame. Forerunner's additional status caller now shares the same rate helper.
The upstream author reports its own tests in commit messages; **no Forerunner
tests, builds or launches ran**, and all local behavior/performance is UNVERIFIED.

Campaign/co-op source from that same pin now also reuses `ac152e44`, `8c70e633`,
`511da10e`, `8903981f` and `958cd833` via `cherry-pick -x`, retaining bnunu's and
chestahh's authorship. Native unattended scripted cameras, old-server fallback
and owning-thread/campaign destination handling share existing engine systems.
Local adaptation uses Forerunner's CE map families, preserves MCC order/menu and
mode safeguards, clears pending destinations through native restore/lifecycle
hooks and retains reduced-feature stubs. Source only, UNVERIFIED; upstream
validation is not local acceptance and no network version/capability changed.

Voice audit (owner-requested, source only): native desktop Chupathingy voice is
already connected through `voice_game.c`, Delta Peer, `voice.c`/Opus and
`dsound_voice_*` in the existing Halo mixer. Host proximity routing admits nearby
enemies as well as teammates; team/party radio is global within the session.
Range, PTT, open-mic opt-in, mute and channel/master volumes already exist.
Capture defaults off and uses SDL recording-device selection on Windows; no
Vive/Meta microphone SDK or second VOIP implementation is needed for that path.
None of this source inspection proves current audio or headset gameplay.

Outstanding connections: the cross-session `voice_game_set_party_path` interface
has no external caller, and `docs/delta.md` still marks the OpenCE/Delta voice
bridge planned. Proximity streams use Halo's 3D/HRTF mixer, but do not acquire
portal/collision occlusion from the native sound-source path. Host routing uses
world positions and one primary player per machine, not a region audio context.
New UNVERIFIED source converts proximity voice through the native listener
matrix before Halo/DirectSound playback, instead of passing world coordinates
to the canonical-origin mixer. Optional OpenXR builds derive a centered listener
from the actual eye cameras before their single sound pass, with native acoustic
location lookup and stale/invalid fallback. Multi-region cave/tunnel acoustics,
VR PTT ergonomics and actual Windows headset capture remain unverified/open.

Focused Forerunner source repair keeps an unavailable explicitly named microphone
closed instead of substituting the system mic. Mode/device changes close old
capture and discard buffered/encoded voice and encoder history. Existing settings
and codec/network framing are retained; hardware/privacy transitions are untested.

The combined foundation uses [AGPL-3.0-or-later](LICENSE), retaining
Chupathingy's [GPL-3.0-or-later notices](LICENSE.GPL-3.0). This is inherited work;
its authorship/history must remain intact. OpenCE, Faber, OpenH1, GulchCE, and the
PS4 repository carry AGPL notices at the inspected snapshots. Damnation's root
`LICENSE.md` is CC0; imported and third-party files still need individual review.

Read each candidate's agent/contribution guidance before adapting it. Keep
third-party versions, checksums, notices, fonts, and art credits. OpenH1's
`port/assets/hud/NOTICE.md` explicitly places Aerocatia/Jessie art outside its
engine license. Console SDK permissions do not grant rights to SDK/game assets.
No proprietary game data belongs in this repository or release packages.

Future integration uses source comparisons and small author-preserving commits.
No public release, push, or upstream submission is authorized by this ledger.


Graphics parity review, 15 November 2026: retain compatible options from every
reviewed fork using the existing setting/menu/render paths. Names alone do not
establish parity: Chupathingy's FOV defaults/units differ, while OpenCE/Fractum
display declarations already overlap the foundation. OpenH1 `baed7757` contains
actual Vulkan ray-query AO (`xgpu_vulkan_rt.c`), world SDF/object-proxy occlusion,
enhanced dynamic lights, per-pixel lighting, HDR controls and projection/view
options. Its ray-traced mode covers ambient occlusion; full path-traced global
illumination is not established. These paths are NOT imported merely by recording
them. The owner's subsequent Universal Halo Graphics directive authorizes the
shared-renderer AO extension. A second OpenH1 renderer remains outside that scope.

Faber shadow-budget reuse adapts SutlabSol/OpenCE-Faber commit
`d67d7922a96e9a24e16cbbe4015c3fe8abc1473f`,
`source/render/render_objects.c`, under AGPL-3.0-or-later. The original
screen-size ranking/fade behavior is retained in Forerunner's native shadow
path, with stock-off default, bounded history, map reset and one shared
per-window threshold per frame for stereo passes. Existing generated graphics
menus expose it. Focused/native evidence is in TEST_EVIDENCE.md; this is not
measured large-horde performance or physical VR acceptance.

## Universal graphics source audit, 15 November 2026

Current local source and retained Git objects were read directly, without a
second checkout. OpenCE `1519c65f` declares 62 `display.*` settings and Fractum
`eae91445` 127: every declaration is present locally. Chupathingy `78d28a70`
declares 93; its three differently named FOV/reticle controls require semantic
mapping, not blind aliases. OpenH1 `baed7757` declares 83, of which 36 names are
absent locally; their capabilities are classified below. Declaration counts
include UI options and prove no renderer behavior. Local source now declares 140
display settings; `graphics.preset` is a computed menu action, not another one.
Their `port_config.c` files were also read through GitHub at the existing pins:
Faber `e4ffe0fe` (blob `b30f66dd0a73b5888723021d48ccdfabee019c9d`),
OpenCE-Forge `91099ce3` (`163da52b5e9c991ba9494340a7250f94b6c638fb`),
and GulchCE `66b4df0b` (`ff81603972d3dae67ed8486a28fc5b99b58ea816`).
They declare 64, 62 and 86 distinct display names respectively. All Forge
names and 63 Faber names exist locally; Faber's `xbox_buttons` is addressed
below. Twelve GulchCE effect names now connect to the inherited effects through
Custom; its fourteen remaining absent names are classified below. Same-name
presence does not establish full consumer/default parity. Their old boolean
`match_timer` differs from Forerunner's string-valued clock selection.
No checkout or upstream/Spark file was created/changed. Earlier provenance-query
lazy fetch attempts were stopped; these reads did not fetch local Git objects.

Implemented means a connected source consumer; verification remains a separate
column. Partially implemented identifies actual backend/semantic gaps. Missing
means no connected equivalent. Unsupported means unavailable on the named current
backend. Unverified means the required current source or execution is unavailable.

| Graphics capability/options | Source classification and current consumer | Verification/remaining work |
| --- | --- | --- |
| Classic/Enhanced/Custom (`OpenH1 display.graphics`) | Implemented as native `graphics.preset`, `menu_functions.c::graphics_preset_*`, existing generated Video controls/writer. Groups AA, model lighting, light effects, shadows/budget, door light blocking, LOD, filtering, far clip and HDR-off. Resolution, FOV, timing and simulation remain individual choices. | Source menu preview/Cancel/Custom/save/reopen pass; actual native UI and identical-camera comparisons open. This is not an alias for OpenH1's broader simulation-inclusive preset. |
| Per-pixel model lighting | Implemented on inherited OpenGL and now shared bgfx via `bg_model_program`, twelve native light-register uniforms and existing NV2A translators. Prepared variants match draws, asynchronous/failed programs fall back together. | Focused C/shader-source checks pass; Vulkan, Metal, D3D11, stereo and real model appearance unverified. Android direct Vulkan still lacks this connection. |
| Enhanced shots/plasma/muzzle/emitters (`OpenH1 display.dynamic_lights`) | Partially implemented: inherited GulchCE-derived effects and native OpenGL paths. `display.light_effects` selects off/low/high/auto/custom with twelve individual saved controls. bgfx now shares model vertex/per-pixel falloff, selected native lights and collision-filtered 3D fields through `bg_model_program`, `bg_model_effects_*` and the existing texture cache. Fixed sampler units 6/7 preserve material stages; capabilities, missing fields and pending/failed shaders retain ordinary lighting. Both native Video styles explain source/hardware limits. | Fifteen actual-source resource/variant/preparation/GLSL-rewrite workflows and ten prior lighting regressions compile/pass; three broken controls detected. Existing choice/menu evidence retained. No live driver/full engine/stereo/cost proof. OpenH1's newer world-light-list/normal-map path remains unfinished. Collision-filtered light visibility is not ambient occlusion or ray tracing. |
| Dynamic/specular-light quality/budgets | Implemented source `display.dynamic_light_limit`, `specular_light_limit`, `light_effects`, native ranking/culling. | Existing limits are reused; dense Firefight/MegaWorld profiling and platform parity unverified. |
| World-space and hardware ray-query AO (`display.ambient_occlusion`) | Missing runtime integration. OpenH1 `world_ao`, `world_scene`, `world_sdf`, `world_proxies`, `world_rt` and `xgpu_vulkan_rt` were reviewed at the retained pin. No inert setting/menu switch added. | Requires ambient-only shader slot, scene/geometry upload, bounded cancellable SDF resources, collision BLAS/TLAS, Vulkan capability/descriptor/barrier/retirement adapters. Must preserve baked BSP self-occlusion and fallback. No RT GI claim. |
| HDR/exposure/tone curves/output | Partially implemented: OpenGL `hdr_read`/`halo_screen_tone_map`, `xgpu_post_tone_map`, existing calibration. Current GT7/neutral/ACES/Reinhard curves and exposure are retained. | bgfx renders SDR; Video help now says so. Real HDR displays, calibration and targets unverified. |
| OpenH1 `hdr_mode` | Partially implemented intent through existing exposure/tone curves, without an asserted faithful/punchy equivalent. | No silent rename; OpenH1 transforms still need shader comparison. |
| OpenH1 `hdr_paper_white_nits` | Implemented equivalent control `display.hdr_paper_white` on the OpenGL HDR path. | Actual output/calibration unverified for this change. |
| OpenH1 `hdr_ui_nits` and `gamma` | Missing separate UI-nit control and configurable final SDR gamma path. | Must preserve HUD brightness, output encoding and VR exclusions. |
| Shadow quality/budget | Implemented native shadow resolution and Faber-derived soft `shadow_limit`; shared selection per native frame/window. | Prior source/build/render evidence retained; sample shortfall and dense/stereo performance remain open. |
| MSAA | Implemented source on OpenGL/bgfx; OpenXR now uses two owned multisampled eye targets resolved into single-sample runtime images. Common eye/BGRA8 intermediate/depth sample masks, cached failure fallback, resize/off retirement and prepared alpha variants share existing renderer machinery. | Nine source lifecycle cases compile/pass on MSVC x64; three negative controls detected. GPU/runtime APIs are fixtures. Real Vulkan, headset rendering/encoding, memory and GPU costs unverified; Video help states that limit. |
| FXAA | Implemented source on OpenGL/bgfx through `halo_screen_anti_alias` / `bg_screen_anti_alias`, shared GLSL and one bounded source copy. The native reader now keeps FXAA rather than remapping it to MSAA; only SMAA/SSAA retain that fallback. Clipped per view, RGB-only output, cached jobs and resize/off retirement. | Ten connected setting/pass/lifecycle cases pass, alongside retained shader-rewrite evidence. The old native remap is reproduced before repair and guarded. Actual driver compilation, split/stereo appearance, encoding and GPU costs open. |
| SMAA/SSAA | Implemented OpenGL postprocess/supersampling, unsupported on bgfx and explicitly explained. | Retain pre-HUD ordering, split-screen bounds and real GPU validation when adapted. |
| OpenH1 `supersampling`, `render_scale` | Partially implemented through existing SSAA2x, `resolution_scaling`, `resolution_limit`, `dynamic_resolution`. Arbitrary supersampling and OpenH1's mobile fractional policy are missing. | Different units/target policy; do not alias silently. |
| Texture filtering/detail/transcoding | Implemented anisotropy, native mip/LOD and existing decode controls. OpenH1 `astc_textures` is only partially equivalent to `dxt1_astc`/current decoding. | No invented texture-quality setting or altered assets; full compressed-format/target comparison open. |
| Model LOD, particles, visibility | Implemented native `model_detail`, `full_model_detail`, `particle_limit`, `particle_ticks`, far clip, performance controls and structure culling. | No measured huge-horde/19-region performance claim. |
| Widescreen/ultrawide/FOV (`draw_distance`) | Implemented source with existing aspect/FOV/reticle and `far_clip_distance` consumers. | Preserve Forerunner's 16:9 degrees; Chupathingy's `field_of_view`, `viewmodel_field_of_view`, `reticle_scales_with_fov` are partial semantic mappings. |
| OpenH1 `vehicle_fov`, `scale_scoped_fov` | Missing independent vehicle/scoped projection controls. Existing FOV is not an equivalent. | Native seats, zoom transitions, cinematics and VR require separate proof. |
| OpenH1 `panini`, `panini_vertical` | Missing connected projection/postprocess and reticle/viewmodel compensation. | Needs reuse in existing postprocess; off/HUD/stereo behavior must be preserved. |
| OpenH1 `viewmodel_offset_x/y/z`, `viewmodel_pitch/yaw/roll` | Missing configurable six-axis flat-screen viewmodel adjustments. Native/VR weapon pose is not an equivalent setting. | Preserve authored animations and physical VR ownership. |
| OpenH1 `lowered_reticle`, `lowered_reticle_amount` | Missing configurable lowered flat-screen reticle/projection pairing. Existing FOV reticle scale does not implement lowering. | Requires aim/projection alignment; never move gameplay aim just to change the HUD. |
| Frame interpolation/V-Sync/VRR/limits | Implemented source through current interpolation, pacing, direct camera and frames-in-flight settings. | Native determinism remains separate; display/VR/backend timing needs target-specific validation. |
| OpenH1 `low_latency`, `vrr_cap_margin` | Partially implemented intent, missing its just-in-time pacing and configurable VRR margin. | Not equivalent to turning V-Sync off or changing packet/simulation rates. |
| OpenH1 `battery_saver` | Missing automatic power-aware preset/pacing policy. Existing performance mode is partial manual control. | No headset throttling policy imported. |
| Lens flares/plasma effects/CRT | Native lens-flare tests, inherited plasma paths and existing CRT option retained. | Backend-specific light effects and resource pressure remain unverified. |
| OpenH1 `show_frame_rate` | Implemented equivalent reporting through `display.show_fps`, `frame_rate`, existing stats. | Does not prove GPU cost measurements or a performance increase. |
| OpenH1 `movies`, `menu_pictures`, `menu_engine`, `menu_fade` | Partially implemented presentation intent through current movie/loading/menu systems; newer OpenH1 menu engine/pictures/fade are absent. | No additional menu engine or upstream art copied. |
| OpenH1 `frame_style`, `picture_shape`, `picture_position`, `orientation` | Missing those picture-frame/mobile-orientation policies. Existing widescreen/round-screen/window sizing are not exact equivalents. | Target-specific; no authoring or gameplay capability implied. |
| OpenH1 `overshield_layer_color` | Partially implemented related shield color/glow/overshield controls; separate newer layer-color semantics unverified. | Preserve existing option meaning and licensing. |
| OpenH1 `spartan_link` | Missing that presentation option; unrelated to Spark capability discovery. | Requires actual HUD behavior review before adoption. |
| Faber `xbox_buttons` | Partially implemented intent through existing per-controller `hud_button_style_*` and `menu_button_glyphs`/button appearance workflow. No blind boolean alias. | Exact all-prompt console override semantics remain unverified. Existing controller assets/attribution retained. |
| Faber/Forge/GulchCE `match_timer` | Partially equivalent: existing Forerunner string-valued clock selection also supports off. Upstream booleans are not drop-in config values. | Preserve local menu/writer behavior; no automatic rewrite of user configs or silent type alias. |
| GulchCE `third_person`, `third_person_smooth`, `third_person_left_shoulder`, `third_person_distance`, `third_person_height`, `third_person_offset`, `third_person_crosshair_smooth`, `third_person_crosshair_size` | Missing connected shoulder-camera/cover-aware reticle workflow. Passenger third person and VR cameras are different responsibilities. | Requires camera/aim/zoom alignment, host permission and native/stereo proof; no speculative settings added. |
| GulchCE `shield_look` | Partially implemented related shield color/glow/arm/overshield controls; missing its classic-with-sprinkles, honeycomb and CMT shield profiles. | Custom assets require individual license/ownership review. No CMT assets or copied shader look claimed. |
| GulchCE `shader_prepare` | Partially equivalent to inherited `display.prepare_shaders`, `render_enumerate.c`, shader-key preparation/cache. | Its observed/bounded material recipe policy is not established as identical; no second preparation/cache manager. |
| GulchCE `texture_prepare_mb` | Missing general Xbox/CE texture preparation budget; existing `mcc_texture_prepare_mb` applies only to MCC. | Keep separate map-family/resource limits; do not alias MCC preparation to native caches. |
| GulchCE `dynamic_lighting_reset` | Unsupported migration marker, rather than a visual feature. Custom reuses effect names without importing its one-time reset. | Existing saved preferences are not reset or overwritten. |
| GulchCE `changelog`, `menu_frontend` | Missing those optional presentation workflows; existing menus remain authoritative. | No second frontend or changelog subsystem added. |

The existing bgfx Vulkan XR patch exposes queue/image handoff, not OpenH1's
ray-query device features, acceleration-structure descriptors, early build
commands or safe frame-serial resource retirement. Those must be extended in the
current backend, not replaced with OpenH1's guest/host renderer. AO must affect
ambient light alone, keep authored/baked BSP lighting intact, exclude first-person
weapons/transparents as upstream does, share scene preparation across eyes and
reset/cancel safely at BSP/map/device transitions. Actual World/Ray Traced fallback,
hardware execution, screenshots, GPU costs and individual target acceptance stay
open. No graphics change in this checkpoint changes network data or simulation.
