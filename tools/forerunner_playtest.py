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
MEGAWORLD_REGIONS = 19
REGIONAL_POOLS_FEATURE = "extraction.regional_pools.v1"


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


def check_pooled_megaworld_receipt(receipt, name):
    features = receipt.get("requiredNativeFeatures", [])
    pools = receipt.get("extractionPools")
    has_pools_feature = isinstance(features, list) and REGIONAL_POOLS_FEATURE in features
    if pools is None and not has_pools_feature:
        return False
    if (receipt.get("mode") != "extraction" or receipt.get("scenarioType") != 1
            or not has_pools_feature or not isinstance(pools, list)
            or len(pools) != MEGAWORLD_REGIONS):
        raise ValueError(f"Keep partial Extraction pools in private staging: {name}")

    region_ids = set()
    assigned_rows = {field: set() for field in ("playerStarts", "weapons", "vehicles")}
    for region in pools:
        if not isinstance(region, dict):
            raise ValueError(f"Invalid Extraction pool region: {name}")
        region_id = region.get("regionId")
        if not isinstance(region_id, str) or not region_id.strip() or region_id in region_ids:
            raise ValueError(f"Extraction needs nineteen distinct region identities: {name}")
        region_ids.add(region_id)
        for field, minimum in (("playerStarts", 2), ("weapons", 3), ("vehicles", 1)):
            rows = region.get(field)
            if (not isinstance(rows, list) or len(rows) < minimum
                    or any(type(row) is not int or row < 0 for row in rows)
                    or (field == "playerStarts" and len(rows) != minimum)
                    or len(set(rows)) != len(rows) or assigned_rows[field].intersection(rows)):
                raise ValueError(f"Missing, invalid, or shared {field} for Extraction region {region_id}: {name}")
            assigned_rows[field].update(rows)
    return True


def promote(root, stage, pins, *, production_megaworld=False):
    build = root / "build"
    latest = root / "Latest"
    previous = root / "Previous"
    # Older checkouts use a junction to the original managed directory.
    current = latest.resolve()
    if current not in (latest, build / "windows-playtest-current") or not current.is_dir():
        raise ValueError("Latest must be the managed Windows playtest")
    if linked_path(build) or linked_path(current):
        raise ValueError("Build and the managed playtest must be physical directories")
    stage = stage.absolute()
    if stage.parent.resolve() != build or linked_path(stage):
        raise ValueError("Use a fresh disposable stage directly under build")
    if stage.resolve() == current or "halo.exe" not in pins:
        raise ValueError("Pin the staged executable")
    complete_pools = False
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
            complete_pools |= check_pooled_megaworld_receipt(receipt, name)

    if production_megaworld and not complete_pools:
        raise ValueError("Production MegaWorld requires a staged, complete Extraction pool receipt")

    temporary = build / "playtest-promotion"
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
        if recoverable and temporary.resolve().parent == build and not linked_path(temporary):
            shutil.rmtree(temporary)
    print(root / "Latest" / "halo.exe")


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("stage", type=Path)
    parser.add_argument("sha256")
    parser.add_argument("--file", action="append", default=[], metavar="PATH=SHA256")
    parser.add_argument("--production-megaworld", action="store_true",
                        help="require a staged Extraction receipt covering the whole MegaWorld")
    args = parser.parse_args()
    try:
        pins = dict(entry.rsplit("=", 1) for entry in args.file)
        if "halo.exe" in pins or len(pins) != len(args.file):
            raise ValueError("Supply each staged file pin once")
        promote(ROOT, args.stage, {"halo.exe": args.sha256, **pins},
                production_megaworld=args.production_megaworld)
    except (ValueError, OSError, RuntimeError) as error:
        parser.exit(2, f"forerunner_playtest: {error}\n")


if __name__ == "__main__":
    main()
