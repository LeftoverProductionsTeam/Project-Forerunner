# Windows

`ninja windows` compiles the game with clang for 32-bit x86 Windows. The
result is a native executable, `build/windows/halo.exe`, with `SDL3.dll`
next to it. You must build on Windows.

The Windows build uses the platform layer of the Linux build
(`port/linux/src`). The graphics, the sound, the input, the settings and
the multiplayer functions are the same as on Linux. Refer to
[port/linux/README.md](../linux/README.md).

## Requirements

You do not need the Xbox SDK.

- Visual Studio or the Visual Studio Build Tools, with the C++ workload.
  The build uses the 32-bit (x86) MSVC libraries and a Windows 10 or 11
  SDK. It does not use the MSVC compiler.
- LLVM (clang and lld), Python 3 and ninja in the `PATH`. For example,
  enter `scoop install llvm python ninja`.
- `llvm-rc` (LLVM's resource compiler, with the LLVM above) in the `PATH`.
  It puts the icon and the version information of `port/windows/halo.rc`
  in the executable. `python tools/windows_icon.py` makes the icon,
  `port/windows/halo.ico`, from `port/art/icon.svg` (needs `rsvg-convert`);
  it is committed, so a build does not need it.
- A network connection for the first build. `configure.py` downloads the
  Visual C++ development package of SDL 3.4.16 to
  `build/windows/third_party`.

## Build the game

1. Go to the root folder of the repository.
2. Enter `python configure.py`.
3. Enter `ninja windows`.

`configure.py` generates the Windows build only on Windows, and with
`--windows-cross`, which cross-compiles it from Linux: the same clang and
lld build against an MSVC toolchain, whose directories
`--windows-vctools-dir` (the versioned folder under `VC/Tools/MSVC`, the
one with `include` and `lib`) and `--windows-sdk-dir` (the SDK folder with
`Include` and `Lib`) name, or `VCToolsInstallDir` and `WindowsSdkDir`. The
build passes them to clang in `INCLUDE` and `LIB`. `--windows-cc` names
the cross compiler (a run-of-the-mill clang by default).

    python configure.py --windows-cross \
        --windows-vctools-dir "$VCToolsInstallDir" \
        --windows-sdk-dir "$WindowsSdkDir"
    ninja windows

The toolchain can be an [msvc-wine](https://github.com/mstorsjo/msvc-wine)
install or an unmodified MSVC from a Windows install (from an NTFS volume,
for instance). The SDK must have a versioned folder in each of `Include`
and `Lib` (`Include/<version>/um`): a plain `xwin splat` does not, a splat
made with `xwin splat --use-winsysroot-style` does.

The build uses all the instructions of the processor of the computer that
builds it (`-march=native`). Such a build does not always start on a
different computer. To make a build for other computers, enter
`python configure.py --portable`. Refer to the build options in the main
[README](../../README.md#building-it-yourself).

For a new optimization profile (`--pgo=train`), the build compiles the
profile runtime of LLVM for 32-bit x86 (`pgo/halo_profile_runtime.c`).
LLVM for Windows supplies this runtime only for x86-64.

### 64-bit

`ninja windows64` builds the same game as native x64 code,
`build/windows64/halo.exe`, with `SDL3.dll` for x64 next to it. It needs the
x64 MSVC libraries of Visual Studio instead of the x86 ones.

It is the 64-bit build of the other systems (`HALO_64BIT`: see
[port/macos/README.md](../macos/README.md)), with these differences:

- Windows keeps `long` 32 bits wide on x64, as the Xbox compiler did. The
  game sources need none of the `long` rewrite of the Linux and macOS
  64-bit builds (`tools/lp64_rewrite.py`).
- The platform layer reserves the 4 GB Xbox address space at 1 TB with
  `VirtualAlloc`, through the `mmap` of `src/win32_posix.c`. The game stops
  if it cannot get that address. The executable does not use high-entropy
  address randomization, which could put the first allocations of the
  process in that range.
- The write tracking and the crash reports of `src/win32_memory_watch.c`
  convert between Xbox addresses and the pointers of the process. A crash
  report gives the address of `halo.exe` in that run, and the calls come
  from the unwind data of x64 code, not from a chain of frame pointers.
- It is not optimized with a profile. The committed profiles are those of
  the 32-bit builds.

Like the 32-bit build, it plays the Custom Edition maps of Halo PC
(`maps_ce/`), HaloMD's maps (`maps_md/`) and Halo PC's own (`maps_pc/`). It plays with the 32-bit builds
and the other ports over the network, and it is a dedicated server too
(`server/README.md`). Its releases are a separate download,
`chupathingyce-windows64-release.zip`, which its self-updater asks for.

## Start the game

Enter `build\windows\halo.exe`.

The game finds the game data as on Linux. Refer to "Start the game" in
[port/linux/README.md](../linux/README.md#start-the-game).

| Item | Location |
| --- | --- |
| Settings | `config.toml` next to `halo.exe` |
| Saved games | `%APPDATA%\ChupathingyCE`, or `paths.saves` in `config.toml`. The first start copies the saves from `%APPDATA%\halo`, which earlier versions used, and leaves that folder as it was |
| Log | `logs\debug.txt` in the data root (the folder that contains `maps\`), or `paths.logs`, or in `paths.saves` when that is set |
| Log of the port | The console. The release build has no console: `halo.log` in the same `logs\` folder |
| Crash reports | `logs\crashes\`, until the game sends them |

Before 0.8.0, `debug.txt` was in the data root itself, and `halo.log` and
`crashes\` were next to `halo.exe`. The game leaves the old files where
they are; crash reports still waiting there are sent from there.

## How the port operates

The game is 32-bit code, as on Linux, because its data contains 32-bit
pointers (the 64-bit build keeps that data in a 4 GB Xbox address space:
see "64-bit" above). The clang target `i686-pc-windows-msvc` gives the ABI of MSVC:
the structure layout, the 16-bit `wchar_t` and the calling conventions.
Thus the Windows build needs fewer changes than the Linux build.

The executable is large-address-aware, because the platform layer reserves
the Xbox memory at `0x80000000`.

The game plays the Custom Edition maps of Halo PC (`maps_ce/`), HaloMD's
maps (`maps_md/`) and Halo PC's own (`maps_pc/`), as the Linux build does (`HALO_CUSTOM_EDITION`). Their
tags are linked to `0x40440000`, outside the Xbox memory. When the game
starts, the platform layer takes those 23 MB at that address
(`port/linux/src/xbox_memory.c`, with `VirtualAlloc`). The executable is
at `0x00400000` and the system libraries are near the top of the lower
2 GB, so the address is normally free. If it is not (`cannot reserve Custom
Edition maps' tag cache` in the log), the game does not open those maps,
and the Xbox maps play as usual.

`SDL3.dll` is delay-loaded with the MSVC `delayimp` helper. This lets
`mcc_window_initialize` reserve MCC's `0x50000000` tag window before the
first SDL call, including the early settings read for Custom Edition.
The shipped SDL DLL keeps ASLR enabled. A reservation failure is reported
once logging is connected, and map loading retries without replacing other
allocations. See [MCC map limits](../../docs/mcc_maps.md) for the remaining
address-space constraints.

### Headers

- The game and the platform layer use the Xbox SDK declarations of
  `port/include/xdk`, not the Windows SDK declarations with the same names.
  The compiler reads `port/include/xdk` before the Windows SDK.
- The C runtime is the static UCRT of Windows.
- `include/halo_windows_prefix.h` is the first header of each file. It gives
  Windows names to the Xbox SDK functions of the platform layer (for example
  `CreateFileA`, `ReadFile`, `Sleep`). Thus the code for Windows gets to
  Windows. The list of names is in `include/halo_windows_api_names.h`.
- `include/crt` changes some C runtime headers. The file functions of the
  game (`fopen`, `open`, `mkdir`) go through the translation of Xbox paths
  (`src/windows_crt.c`).
- `include/posix` declares the POSIX functions that the platform layer uses.

### Code for Windows

These files use only the Windows SDK:

| File | Contents |
| --- | --- |
| `src/win32_files.c`, `src/win32_net.c` | The file and socket functions of `port/linux/src/posix.h`. |
| `src/win32_posix.c` | The POSIX functions on Windows threads, critical sections, condition variables, `VirtualAlloc` and the performance counter. |
| `src/win32_memory_watch.c` | The write tracking of textures, with a vectored exception handler. |
| `src/win32_crash.c` | The crash reports. Refer to "Crash reports". |
| `src/win32_mbedtls.c`, `mbedtls/threading_alt.h` | The locks of Mbed TLS (`MBEDTLS_THREADING_ALT`), slim reader/writer locks that it gets before `main`. The DTLS of the game thread and the HTTPS of the other threads use Mbed TLS at the same time. |

`port.json` gives the Linux files that these files replace, and the Windows
libraries of the link.

### Inline functions

MSVC makes a COMDAT copy of an external `__inline` function where it does
not inline a call. Some files call such functions through a prototype.
clang inlines more calls. Thus a function can have no copy. The build
prevents this:

- `port/linux/game/msvc_comdat.c` gives one weak definition of each header
  inline function.
- A wrapper takes the address of each `__inline` function in a `.c` file.
  Thus the compiler makes an external copy (`inline_export_wrapper` in
  `tools/windows_build.py`).

MSVC gives file scope to a structure tag in a prototype. clang does not.
Thus the build also includes the declarations of the Linux build
(`build/windows/halo_msvc_tags.h`).

## Crash reports

A crash writes the faulting address and the calls that led to it to
`debug.txt` and to the log. Releases and nightlies also send a crash report
to the Chupathingy site (`network.browser_url`, `POST /v1/crash`), which
groups the crashes and tells the developers:

1. The game starts a second copy of `halo.exe` (`halo.exe --crash-report`).
   This copy writes a minidump of the game to `logs\crashes\` (until
   0.8.0, `crashes\` next to `halo.exe`), and walks the stack of the thread that crashed. Then the
   game stops. A minidump contains the stacks and the registers of the
   threads, and the list of the modules. It does not contain the other
   memory of the game.
2. The copy sends the report when `crash_reports.upload` in `config.toml`
   is `"yes"`, this fork's default. With `"ask"`, at the first crash the
   copy asks the player whether to send crash reports and keeps the answer:
   `"yes"` sends this report and all the reports after it, and `"no"` sends
   no reports and does not ask again. The answer `"no"` deletes the report.
3. The copy sends the report, then deletes it. If it cannot send it (for
   example, with no network connection), the game sends it when it starts
   the next time (`halo.exe --crash-upload`, in the background).
   `logs\crashes\` keeps at most 8 reports.

A report is JSON (`port/linux/src/crash_report.h`). It contains:

- The version, the channel and the commit of the build, the platform and
  the architecture, and the version of Windows (and of Wine, under Wine or
  Proton).
- The exception code, and up to 32 calls that led to it. Each call is a
  module and an offset in it, for example `halo.exe+0x1a2b3c`.
- The last 200 lines of `halo.log`. The game replaces each public IP
  address in them with `[address]` before it sends them.
- The minidump, if it is 1 MB or smaller.

A report contains no player names other than the names in the log, and no
hardware IDs.

Each release has the PDBs as `halo-windows-<configuration>-symbols.zip`.
`tools/symbolize_crash.py` uses them to add the function names and the
source lines to the crash lines of a `debug.txt` or a `halo.log`, and to
the calls of a report (`halo.exe+0x...`):

```
python tools/symbolize_crash.py debug.txt halo.exe
```

Give the `halo.exe` of the build that crashed, with its `halo.pdb` next to
it. The tool needs `llvm-symbolizer` (LLVM).

Local builds and the other builds of the `dev` and `custom` channels send
no crash reports. To test the crash reports with such a build, set the
`HALO_CRASH_REPORTS_ANY_BUILD` environment variable.

The crash reports also include the crashes of `abort()` and of an invalid
argument to a function of the C runtime.

## Limits

The optional `--with-openxr` configure flag includes an **unverified, incomplete**
Windows64 OpenXR backend. It is off at runtime unless `vr.enabled` / `HALO_VR`
is enabled. That path selects the existing bgfx Vulkan renderer; flat-screen
defaults remain unchanged. One backend has Touch bindings for Quest 2 PCVR and
Index bindings for Vive/Index with Knuckles. It is not standalone Quest support.
The device/session, renderer, stereo camera/FOV caller and basic controller
input are connected in source through existing native frame/window and Xbox
input paths. They remain unverified, incomplete VR: physical scopes and OpenH1
interactions still need wiring. No compilation or headset
validation has been performed under the owner's source-only instruction.

Native menu frames now have a source-only head-locked OpenXR quad path. It
reuses the left-eye image, composites the existing menu and overlay once, and
mirrors that canvas to the desktop. Mouse layout remains in desktop coordinates;
gameplay resumes stereo when native menu ownership ends. Hidden desktop frames
can still submit the menu quad. Tracking, layout/cutouts, colors, transitions and
comfort remain unverified; tracked pointers and physical UI interaction are open.

`win32_openxr.c` and its interfaces adapt OpenH1/goath1's `host_vr.c`,
`host_vr_vulkan.h` and `vr_host.h` at
`baed77571d53abe32b27434b4ba869f11db00fe9` (AGPL-3.0-or-later).
The existing bgfx patch mechanism connects runtime Vulkan device creation and
GPU selection without importing OpenH1's Linux/Vulkan host or changing netcode.
Khronos headers use OpenH1's `OpenXR-SDK` `release-1.1.63` pin and SHA-256
digests in `tools/windows_build.py`, fetched unchanged into ignored build data
only when configured later. Their SPDX licence is Apache-2.0 OR MIT; see the
[Khronos SDK licence](https://github.com/KhronosGroup/OpenXR-SDK/blob/release-1.1.63/LICENSE).
The installed Windows `openxr_loader.dll` selects the active PC runtime; no
loader/runtime or proprietary SDK is copied into this repository.

Basic controls reuse OpenH1's Xbox-style mapping: right A/B map to native A/B;
left X/Y (Touch) or A/B (Index) map to native X/Y. The left/right triggers are
native LT/RT, grips are White/Black, sticks and individual clicks retain native
movement/look and thumb-click actions. Both stick clicks together provide
Start/pause for Index; Touch also binds its left menu button. Native profile
button layouts still determine gameplay actions. Keyboard/SDL gamepads remain
merged; XR focus is separate from the desktop-only SDL remapper. The existing
guide/chat input owners still process the final pad. Unfocused, stale or
untracked gameplay input is neutral; native rumble also feeds the two hands.
All mappings, focus/ownership transitions and haptics are unverified. No tracked
weapon pose, physical reload/grenade gesture or hand-model acceptance is implied.

`vr.aim = "controller"` / `HALO_VR_AIM=controller` optionally routes the right-hand
aim pose through native first-person, on-foot facing. The default `"gamepad"`
keeps native stick aiming. Source adapts OpenH1's separate body heading and
eye-to-controller hit-point convergence; movement is rotated into native facing
so pointing the hand does not steer walking. Menus, cinematics, vehicle seats,
focus/tracking loss and map/restore transitions release the tracked path. Existing
player-action fields carry facing/throttle; no remote hand poses were added.
With controller aim, optional `vr.turn = "snap"` / `HALO_VR_TURN=snap` adapts
OpenH1's physical right-stick latch: push past 0.7, release below 0.3 before
another step. `vr.snap_turn_angle` / `HALO_VR_SNAP_TURN_ANGLE` defaults to 30
degrees (1..180, invalid values use 30). Native profile turning remains the
default (`"smooth"`), including vehicles and gamepad aim. A centered XR stick
retains native look; while deflected it owns snap-mode turning. Ownership,
menu/focus/tracking and world transitions require neutral before rearming.
This uses the existing body heading and facing fields, not another input or
network path. Snap direction, comfort, bindings and headset behavior are
unverified; no hardware checks, builds or tests have been run.
Controller weapon posing now adapts OpenH1's `vr_render_weapon_camera`: only the
native first-person animation/node build sees the right-hand camera, then the
head camera is restored for world rendering. Both eyes use the same posed weapon;
flat-screen viewmodel FOV overrides are skipped during stereo eye rendering.
`vr.weapon_offset` / `HALO_VR_WEAPON_OFFSET` reuses OpenH1's `"0.15, -0.22, 0.30"`
approximate grip offset in metres (right/up/forward). Components outside -1..1,
non-finite or malformed values use those defaults. Original models, animations
and visibility rules remain in use; no independent hand renderer was added.
The gamepad path poses the weapon from the native body camera before headset
turning/leaning, following OpenH1's gamepad weapon-camera behavior. Unzoomed,
on-foot VR reticles use the existing native pre-spread firing ray (the same
preview used by passenger aiming) and a finite collision target, shared by both
eyes. Each eye projects that target into native HUD coordinates. Existing aiming
sprites move together without drawing a second reticle; warnings, meters and
scope images keep native placement. A target behind/outside an eye hides only
its aiming pieces. Headset placement bypasses flat-screen reticle FOV scaling;
the world/fog FOV queries retain their existing behavior. No shot is created and
no simulation or network fields are changed by this rendering query.
This is unbuilt, untested source, not complete VR gunplay. Grip/reticle alignment,
vergence comfort, physical scopes, standalone/two-hand interactions, physical
reload/grenade actions and vehicle aiming remain open or unverified. The preview
is an initial straight firing reference, not projectile-flight prediction.

Audio obstruction now adapts OpenH1's applied-head listener and eye-to-head
clearance policy. The existing centered-head adapter chooses a native-position
fallback across solid geometry while keeping head orientation; obstruction reads
the same already applied audio origin/cluster, including optional proximity
voice. The clearance cache retries for changed poses, native ticks and BSPs;
existing per-sound obstruction cadence remains tied to native ticks. Map/restore
paths clear both cache levels and the published head origin. No observer/player
state, voice transport or microphone policy is changed. This is unverified source
for the current loaded BSP, not connected-region acoustics or headset acceptance.

- The movies play through FFmpeg's Bink decoder
  (`port/linux/src/bink_ffmpeg.c`), built for Bink only and linked
  statically with the same clang (`tools/ffmpeg_build.py`, under MSYS2's
  shell); `configure.py --without-ffmpeg` leaves it out and the game skips
  the movies. The disc extraction at the first start copies the disc's
  `bink/` movies (only the ones that are missing) next to `maps\`.
- The game uses only the ANSI file functions. A path to the data or the
  saved games with characters that are not in the code page of the system
  does not always operate.
