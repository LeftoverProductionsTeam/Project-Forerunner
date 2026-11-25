"""Rotate the local Windows playtest payload after its focused smoke test.

Use a staged halo.exe and its tested SHA-256. Optional --file PATH=SHA256
arguments name verified map payloads or SDL3.dll in the same staging folder.
Latest's settings and persistent save directories are outside the rotation.
"""

import argparse
import json
import os
import re
import shutil
import subprocess
from pathlib import Path, PurePosixPath

from forerunner_build import ROOT, file_checksum, linked_path


MAP_FOLDERS = {"maps", "maps_ce", "mcc_maps"}
RECEIPT = ".forerunner-playtest.json"


def payload_name(name):
    path = PurePosixPath(name)
    if path.as_posix() != name or any(part in {"..", "."} or ":" in part for part in path.parts) or "\\" in name:
        raise ValueError(f"Invalid payload name: {name}")
    if name in {"halo.exe", "SDL3.dll"}:
        return path
    if len(path.parts) == 2 and path.parts[0] in MAP_FOLDERS and (
        path.suffix.lower() in {".map", ".txt"} or name.lower().endswith(".spark-mode.json")
    ):
        return path
    raise ValueError(f"Unexpected playtest payload: {name}")


def payload(directory):
    paths = [directory / name for name in ("halo.exe", "SDL3.dll")]
    for folder in sorted(MAP_FOLDERS):
        path = directory / folder
        if path.is_dir() and not linked_path(path):
            paths.extend(file for file in path.iterdir() if file.is_file())
    result = {}
    for path in paths:
        name = path.relative_to(directory).as_posix()
        payload_name(name)
        if linked_path(path) or not path.is_file():
            raise ValueError(f"Missing or linked payload: {path}")
        result[name] = path
    return result


def seal_rollback(previous):
    files = {name: file_checksum(path) for name, path in payload(previous).items()}
    (previous / RECEIPT).write_text(json.dumps(files, indent=2) + "\n", encoding="utf-8")


def check_rollback(previous):
    if not previous.is_dir() or linked_path(previous):
        raise ValueError("Previous must be the managed rollback directory")
    files = payload(previous)
    receipt = previous / RECEIPT
    config = previous / "config.toml"
    if linked_path(receipt) or linked_path(config) or not config.is_file():
        raise ValueError("Previous has a linked settings or receipt file")
    expected = set(files.values()) | {config, receipt}
    for path in previous.rglob("*"):
        if path == previous / path.name and path.name in MAP_FOLDERS and path.is_dir() and not linked_path(path):
            continue
        if path not in expected:
            raise ValueError(f"Keep the unexpected file before rotating Previous: {path}")
    if json.loads(receipt.read_text(encoding="utf-8")) != {
        name: file_checksum(path) for name, path in files.items()
    }:
        raise ValueError("Previous contains modified payloads")


def close_playtest(root, current):
    if os.name != "nt":
        return
    environment = dict(os.environ, FORERUNNER_LATEST_EXE=str(root / "Latest" / "halo.exe"),
                       FORERUNNER_CURRENT_EXE=str(current / "halo.exe"),
                       FORERUNNER_PREVIOUS_EXE=str(root / "Previous" / "halo.exe"))
    command = (
        "$ErrorActionPreference='Stop'; "
        "$paths=@($env:FORERUNNER_LATEST_EXE,$env:FORERUNNER_CURRENT_EXE,$env:FORERUNNER_PREVIOUS_EXE); "
        "Get-Process -Name halo -ErrorAction SilentlyContinue | Where-Object {$_.Path -in $paths} | "
        "ForEach-Object { if (!$_.CloseMainWindow() -or !$_.WaitForExit(15000)) "
        "{ throw 'The playtest could not close normally.' } }; exit 0"
    )
    result = subprocess.run(["powershell.exe", "-NoProfile", "-Command", command], env=environment,
                            creationflags=subprocess.CREATE_NO_WINDOW, capture_output=True, text=True)
    if result.returncode:
        raise RuntimeError("Close the running Forerunner playtest before promoting.")


def promote(root, stage, pins):
    current = root / "build" / "windows-playtest-current"
    previous = root / "Previous"
    if not current.is_dir() or linked_path(current) or (root / "Latest").resolve() != current:
        raise ValueError("Latest must point to the managed Windows playtest")
    stage = stage.absolute()
    if stage.parent.resolve() != current.parent or linked_path(stage):
        raise ValueError("Use a fresh disposable stage directly under build")
    if stage.resolve() == current or "halo.exe" not in pins:
        raise ValueError("Pin the staged executable")
    for name, digest in pins.items():
        path = stage / payload_name(name)
        if not re.fullmatch(r"[0-9a-fA-F]{64}", digest) or linked_path(path) or not path.is_file():
            raise ValueError(f"Invalid staged pin: {name}")
        if file_checksum(path) != digest.lower():
            raise ValueError(f"Staged checksum mismatch: {name}")
        if linked_path(current / name):
            raise ValueError(f"Linked playtest destination: {name}")
        if path.suffix.lower() == ".map":
            sidecar = PurePosixPath(name).with_suffix(".spark-mode.json").as_posix()
            if (current / sidecar).is_file() and sidecar not in pins:
                raise ValueError(f"Include the paired Spark mode receipt: {sidecar}")
        if name.endswith(".spark-mode.json"):
            map_name = name.removesuffix(".spark-mode.json") + ".map"
            map_path = current / map_name
            native_digest = pins.get(map_name) or file_checksum(map_path)
            receipt = json.loads(path.read_text(encoding="utf-8"))
            if not isinstance(receipt, dict) or receipt.get("fileName") != PurePosixPath(map_name).name or (
                str(receipt.get("cacheSha256")).lower() != native_digest.lower()
            ):
                raise ValueError(f"Spark receipt does not match the map: {name}")

    temporary = current.parent / "playtest-promotion"
    temporary.mkdir()
    replacement = temporary / "replacement"
    rollback = temporary / "rollback"
    retired = temporary / "retired"
    installed = []
    moved_previous = False
    recoverable = True
    try:
        for name, digest in pins.items():
            target = replacement / name
            target.parent.mkdir(parents=True, exist_ok=True)
            shutil.copyfile(stage / name, target)
            if file_checksum(target) != digest.lower():
                raise ValueError(f"Staged file changed while copying: {name}")

        close_playtest(root, current)
        check_rollback(previous)
        files = payload(current)
        for name, file in files.items():
            backup = rollback / name
            backup.parent.mkdir(parents=True, exist_ok=True)
            os.link(file, backup)
        shutil.copyfile(previous / "config.toml", rollback / "config.toml")
        seal_rollback(rollback)
        for name in pins:
            target = current / name
            target.parent.mkdir(parents=True, exist_ok=True)
            os.replace(replacement / name, target)
            installed.append(name)
        previous.rename(retired)
        moved_previous = True
        rollback.rename(previous)
    except BaseException:
        try:
            if moved_previous:
                retired.rename(previous)
            for name in reversed(installed):
                if name in files:
                    copy = replacement / name
                    os.link(rollback / name, copy)
                    os.replace(copy, current / name)
                else:
                    (current / name).unlink()
        except OSError as restore_error:
            recoverable = False
            raise RuntimeError(f"Restore the interrupted promotion from {temporary}") from restore_error
        raise
    finally:
        if recoverable and temporary.resolve().parent == current.parent and not linked_path(temporary):
            shutil.rmtree(temporary)
    print(root / "Latest" / "halo.exe")


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("stage", type=Path)
    parser.add_argument("sha256")
    parser.add_argument("--file", action="append", default=[], metavar="PATH=SHA256")
    args = parser.parse_args()
    try:
        pins = dict(entry.rsplit("=", 1) for entry in args.file)
        if "halo.exe" in pins or len(pins) != len(args.file):
            raise ValueError("Supply each staged file pin once")
        promote(ROOT, args.stage, {"halo.exe": args.sha256, **pins})
    except (ValueError, OSError, RuntimeError) as error:
        parser.exit(2, f"forerunner_playtest: {error}\n")


if __name__ == "__main__":
    main()
