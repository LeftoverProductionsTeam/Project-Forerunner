#!/usr/bin/env python3
"""Build the Windows x86 baseline from a private export of tracked source.

Requires Python 3.9+, Git, Ninja, LLVM, the x86 MSVC libraries and Windows
SDK. No game data is needed or copied. The work directory must be new and
outside the source checkout. Outputs and a source/toolchain manifest stay
there; this tool never cleans, replaces or publishes an existing build.

    python tools/forerunner_build.py --work C:/Temp/forerunner-build-001 \
        --toolchain C:/Tools/LLVM --ninja C:/Tools/ninja.exe

Use --export-only to inspect the exact source snapshot without compiling.
"""

import argparse
import hashlib
import json
import os
import re
import shutil
import stat
import subprocess
import sys
from pathlib import Path, PurePosixPath

ROOT = Path(__file__).resolve().parent.parent
CONFIGURE_ARGUMENTS = ("--portable", "--pgo=off", "--lto=off", "--without-ffmpeg", "--without-bgfx", "--packaged")
MANIFEST_NAME = "forerunner-build.json"
GAME_DATA_SUFFIXES = {".map", ".iso", ".xiso"}
COMPILED_SUFFIXES = {".exe", ".dll", ".obj", ".o", ".pdb", ".a", ".lib", ".so", ".dylib"}


def linked_path(path):
    """A link or junction at the file or any of its parents."""
    for part in (path, *path.parents):
        if part.is_symlink():
            return True
        try:
            attributes = getattr(part.lstat(), "st_file_attributes", 0)
        except FileNotFoundError:
            continue
        if attributes & getattr(stat, "FILE_ATTRIBUTE_REPARSE_POINT", 0x400):
            return True
    return False


def git(source, *arguments):
    return subprocess.run(["git", "-C", str(source), *arguments], check=True, capture_output=True).stdout


def work_directory(source, work):
    source, work = Path(source).resolve(), Path(work).resolve()
    if work == source or source in work.parents or work in source.parents:
        raise ValueError("the work directory must be separate from, and outside, the source checkout")
    if work.exists():
        raise ValueError(f"work directory already exists: {work}; choose a new directory (nothing was removed)")
    return source, work


def tracked_files(source):
    """The index's ordinary tracked files, with current worktree contents.

    Untracked and ignored files never enter the export. Conflicts, gitlinks,
    symlinks and paths escaping the checkout are refused, not dereferenced.
    """
    result = []
    for record in git(source, "ls-files", "--stage", "-z").split(b"\0"):
        if not record:
            continue
        metadata, raw_name = record.split(b"\t", 1)
        mode, _, stage = metadata.decode("ascii").split()
        name = raw_name.decode("utf-8")
        relative = PurePosixPath(name)
        if (relative.is_absolute() or not relative.parts or ".." in relative.parts or
                any(part.lower() == ".git" or ":" in part or "\\" in part for part in relative.parts)):
            raise ValueError(f"unsafe tracked path: {name}")
        if stage != "0" or mode not in ("100644", "100755"):
            raise ValueError(f"export requires ordinary, resolved source files; {name} has mode {mode}, stage {stage}")
        if relative.suffix.lower() in GAME_DATA_SUFFIXES:
            raise ValueError(f"game data is excluded from source builds: {name}")
        if relative.suffix.lower() in COMPILED_SUFFIXES:
            raise ValueError(f"compiled binaries are excluded from source builds: {name}")
        original = source.joinpath(*relative.parts)
        if linked_path(original) or source not in original.resolve().parents:
            raise ValueError(f"tracked path links outside ordinary source: {name}")
        if original.exists():
            if not original.is_file():
                raise ValueError(f"tracked source is not a file: {name}")
            result.append((name, mode, original))
    return sorted(result)


def export_source(source, work):
    source, work = work_directory(source, work)
    top = Path(git(source, "rev-parse", "--show-toplevel").decode().strip()).resolve()
    if source != top:
        raise ValueError(f"source must be the repository root: {top}")
    commit = git(source, "rev-parse", "HEAD").decode().strip()
    status = git(source, "status", "--porcelain=v1", "-z", "--untracked-files=no")
    files = tracked_files(source)
    if not any(name == "configure.py" for name, _, _ in files):
        raise ValueError("source checkout has no tracked configure.py")
    work.mkdir(parents=True, exist_ok=False)
    exported = work / "source"
    exported.mkdir()
    digest = hashlib.sha256()
    records = []
    for name, mode, original in files:
        content = original.read_bytes()
        target = exported.joinpath(*PurePosixPath(name).parts)
        target.parent.mkdir(parents=True, exist_ok=True)
        target.write_bytes(content)
        target.chmod(0o755 if mode == "100755" else 0o644)
        checksum = hashlib.sha256(content).hexdigest()
        # Length-prefixed names keep the snapshot identity unambiguous.
        encoded = name.encode("utf-8")
        digest.update(len(encoded).to_bytes(8, "big") + encoded + mode.encode("ascii") + bytes.fromhex(checksum))
        records.append({"path": name, "mode": mode, "sha256": checksum})
    # Another editor may be working on the checkout. Build one identified
    # snapshot, rather than silently combining files from different edits.
    if (git(source, "rev-parse", "HEAD").decode().strip() != commit or
            git(source, "status", "--porcelain=v1", "-z", "--untracked-files=no") != status or
            [(name, mode) for name, mode, _ in tracked_files(source)] !=
            [(name, mode) for name, mode, _ in files] or
            any(file_checksum(original) != record["sha256"]
                for (_, _, original), record in zip(files, records))):
        raise ValueError("tracked source changed during export; preserved the partial export, choose a new work directory")
    manifest = {"schema": "forerunner.source-build/1", "source_root": str(source), "commit": commit,
                "dirty_tracked_source": bool(status), "tracked_status_sha256": hashlib.sha256(status).hexdigest(),
                "snapshot_sha256": digest.hexdigest(), "files": records, "state": "exported"}
    write_manifest(work, manifest)
    return exported, manifest


def write_manifest(work, manifest):
    temporary = work / (MANIFEST_NAME + ".tmp")
    temporary.write_text(json.dumps(manifest, indent=2) + "\n", encoding="utf-8")
    temporary.replace(work / MANIFEST_NAME)


def version_key(path):
    return tuple(int(part) for part in Path(path).name.split("."))


def newest_version(folder):
    versions = [path for path in folder.iterdir() if path.is_dir() and re.fullmatch(r"\d+(?:\.\d+)+", path.name)]
    if not versions:
        raise ValueError(f"no versioned toolchain directory in {folder}")
    return max(versions, key=version_key)


def msvc_environment(vctools=None, sdk=None, sdk_version=None, vswhere=None):
    """Read installed tools; never install or change the compiler/SDK."""
    vctools = vctools or os.environ.get("VCToolsInstallDir")
    if not vctools:
        vswhere = vswhere or shutil.which("vswhere") or str(
            Path(os.environ.get("ProgramFiles(x86)", "C:/Program Files (x86)")) /
            "Microsoft Visual Studio/Installer/vswhere.exe")
        result = subprocess.run([str(vswhere), "-latest", "-products", "*", "-requires",
                                 "Microsoft.VisualStudio.Component.VC.Tools.x86.x64", "-property", "installationPath"],
                                check=True, capture_output=True, text=True)
        installation = result.stdout.strip()
        if not installation:
            raise ValueError("vswhere found no MSVC C++ workload; supply --vctools")
        vctools = newest_version(Path(installation) / "VC/Tools/MSVC")
    vctools = Path(vctools).resolve()
    sdk = sdk or os.environ.get("WindowsSdkDir")
    if not sdk:
        import winreg

        try:
            with winreg.OpenKey(winreg.HKEY_LOCAL_MACHINE, r"SOFTWARE\Microsoft\Windows Kits\Installed Roots") as key:
                sdk = winreg.QueryValueEx(key, "KitsRoot10")[0]
        except OSError:
            sdk = Path(os.environ.get("ProgramFiles(x86)", "C:/Program Files (x86)")) / "Windows Kits/10"
    sdk = Path(sdk).resolve()
    sdk_version = sdk_version or os.environ.get("WindowsSDKVersion", "").strip("\\/")
    if sdk_version and not re.fullmatch(r"\d+(?:\.\d+)+", sdk_version):
        raise ValueError("SDK version must be a dotted numeric directory name")
    sdk_version = sdk_version or newest_version(sdk / "Include").name
    includes = [vctools / "include", *(sdk / "Include" / sdk_version / name for name in ("ucrt", "um", "shared", "winrt"))]
    libraries = [vctools / "lib/x86", *(sdk / "Lib" / sdk_version / name / "x86" for name in ("ucrt", "um"))]
    for folder in includes + libraries:
        if not folder.is_dir():
            raise ValueError(f"required x86 toolchain directory is missing: {folder}")
    return {"INCLUDE": ";".join(map(str, includes)), "LIB": ";".join(map(str, libraries))}, {
        "vctools": str(vctools), "sdk": str(sdk), "sdk_version": sdk_version}


def executable_path(value, name, search_path=None):
    value = value or shutil.which(name, path=search_path)
    if not value or not Path(value).is_file():
        raise ValueError(f"missing {name}; supply its explicit path")
    return Path(value).resolve()


def file_checksum(path):
    checksum = hashlib.sha256()
    with open(path, "rb") as source:
        for block in iter(lambda: source.read(1024 * 1024), b""):
            checksum.update(block)
    return checksum.hexdigest()


def build_environment(args, manifest):
    environment = dict(os.environ)
    compiler = args.compiler
    if not compiler and args.toolchain:
        root = Path(args.toolchain).resolve()
        compiler = (root / "bin" if (root / "bin").is_dir() else root) / "clang.exe"
    compiler = executable_path(compiler, "clang")
    if compiler.name.lower() != "clang.exe":
        raise ValueError("the native build invokes clang.exe; select that executable with --compiler")
    python = executable_path(args.python, "python")
    ninja = executable_path(args.ninja, "ninja", search_path=str(python.parent) + os.pathsep + environment.get("PATH", ""))
    environment["PATH"] = os.pathsep.join([str(compiler.parent), str(ninja.parent), str(python.parent), environment.get("PATH", "")])
    tools = {"compiler": compiler, "python": python, "ninja": ninja}
    for name in ("lld-link", "llvm-rc", "llvm-objcopy"):
        tools[name] = executable_path(None, name, search_path=environment["PATH"])
    includes, msvc = msvc_environment(args.vctools, args.sdk, args.sdk_version, args.vswhere)
    environment.update(includes)
    for key in list(environment):
        if key.startswith("HALO_") or key in ("GITHUB_REPOSITORY", "GITHUB_REF", "GITHUB_REF_NAME"):
            del environment[key]
    environment["HALO_RELEASE_BUILD"] = "0"
    environment["HALO_COMMIT"] = manifest["commit"][:12] + (
        "-dirty." + manifest["snapshot_sha256"][:12] if manifest["dirty_tracked_source"] else "")
    metadata = {"msvc": msvc, "include": includes["INCLUDE"], "lib": includes["LIB"], "tools": {}}
    for name, tool in tools.items():
        record = {"path": str(tool), "sha256": file_checksum(tool)}
        if name in ("compiler", "python", "ninja"):
            version = subprocess.run([str(tool), "--version"], check=True, capture_output=True, text=True)
            record["version"] = (version.stdout or version.stderr).splitlines()[0]
        metadata["tools"][name] = record
    return environment, tools, metadata


def main(argv=None):
    parser = argparse.ArgumentParser(description=__doc__, formatter_class=argparse.RawDescriptionHelpFormatter)
    parser.add_argument("--source", type=Path, default=ROOT)
    parser.add_argument("--work", required=True, type=Path)
    parser.add_argument("--toolchain", type=Path, help="LLVM installation or its bin directory")
    parser.add_argument("--compiler", type=Path, help="exact clang.exe to put first on PATH")
    parser.add_argument("--ninja", type=Path)
    parser.add_argument("--python", type=Path, default=Path(sys.executable))
    parser.add_argument("--vctools", type=Path, help="versioned MSVC directory with include/ and lib/x86/")
    parser.add_argument("--sdk", type=Path, help="Windows SDK root containing Include/ and Lib/")
    parser.add_argument("--sdk-version")
    parser.add_argument("--vswhere", type=Path)
    parser.add_argument("--jobs", type=int, default=min(os.cpu_count() or 1, 8))
    parser.add_argument("--export-only", action="store_true")
    args = parser.parse_args(argv)
    if args.jobs < 1:
        parser.error("--jobs must be positive")
    if os.name != "nt" and not args.export_only:
        parser.error("this baseline builds on native Windows; use --export-only to inspect its source elsewhere")
    work = args.work.resolve()
    manifest = None
    try:
        exported, manifest = export_source(args.source, work)
        print(f"Exported {len(manifest['files'])} tracked files from {manifest['commit']}", flush=True)
        print(f"Snapshot SHA-256: {manifest['snapshot_sha256']}", flush=True)
        if args.export_only:
            return 0
        environment, tools, toolchain = build_environment(args, manifest)
        commands = [[str(tools["python"]), "configure.py", *CONFIGURE_ARGUMENTS],
                    [str(tools["ninja"]), "-j", str(args.jobs), "windows"]]
        manifest.update({"state": "building", "toolchain": toolchain, "commands": commands,
                         "configure_arguments": list(CONFIGURE_ARGUMENTS)})
        write_manifest(work, manifest)
        for number, command in enumerate(commands):
            log = work / ("configure.log" if number == 0 else "compile.log")
            print(f"Running {subprocess.list2cmdline(command)}; log: {log}", flush=True)
            with open(log, "w", encoding="utf-8") as output:
                subprocess.run(command, cwd=exported, env=environment, stdout=output, stderr=subprocess.STDOUT, check=True)
        binary = exported / "build/windows/halo.exe"
        if not binary.is_file():
            raise ValueError("build reported success but produced no build/windows/halo.exe")
        manifest.update({"state": "compiled", "executable": str(binary), "executable_sha256": file_checksum(binary)})
        write_manifest(work, manifest)
        print(f"Compiled {binary}; SHA-256: {manifest['executable_sha256']}")
        print("Compilation does not establish gameplay or network acceptance.")
        return 0
    except (ValueError, OSError, subprocess.SubprocessError) as error:
        if manifest is not None:
            manifest.update({"state": "failed", "error": str(error)})
            write_manifest(work, manifest)
        print(f"forerunner_build: {error}", file=sys.stderr)
        return 2


if __name__ == "__main__":
    sys.exit(main())
