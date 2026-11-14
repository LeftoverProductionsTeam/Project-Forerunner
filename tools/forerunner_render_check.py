#!/usr/bin/env python3
"""Capture a real native Windows map in a hidden OpenGL window.

The fresh work folder holds independent runtime/data/save copies and bounded
BMP captures. No original runtime or map is changed. Frames require visual
review; this is not a visible-window performance or human-play benchmark.
"""
import argparse
import hashlib
import json
import os
from pathlib import Path
import sys
from types import SimpleNamespace

import net_compare

ROOT = Path(__file__).resolve().parent.parent


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--engine", required=True, type=Path)
    parser.add_argument("--data", required=True, type=Path, help="authorized fixture containing maps/")
    parser.add_argument("--work", required=True, type=Path, help="new external work directory")
    parser.add_argument("--map", default="bloodgulch")
    parser.add_argument("--seconds", type=int, default=40, choices=range(10, 121), metavar="10..120")
    args = parser.parse_args()
    if os.name != "nt":
        parser.error("this native rendering check has been verified on Windows only")
    work, engine, data = args.work.resolve(), args.engine.resolve(), args.data.resolve()
    for original in (ROOT, engine.parent, data):
        if work == original or original in work.parents or work in original.parents:
            parser.error("the work directory must be outside the source, runtime and data directories")
    if work.exists():
        parser.error("work directory already exists; preserve it and choose a fresh directory")
    if not engine.is_file() or not (data / "maps/ui.map").is_file():
        parser.error("provide a compiled engine and an authorized Xbox map fixture with ui.map")
    work.mkdir(parents=True)
    copy = net_compare.prepare_copy(work, "game", engine, data)
    frames = work / "frames"
    frames.mkdir()
    settings = SimpleNamespace(start=4, skip=0, seconds=args.seconds, events=False, window=True, display=None)
    extra = ["HALO_EXIT_AFTER=" + str(args.seconds), "HALO_FULLSCREEN=false", "HALO_WINDOW_SIZE=960x540",
             "HALO_MAX_FPS=60", "HALO_SCREENSHOT_DIR=" + str(frames), "HALO_SCREENSHOT_EVERY=240",
             "HALO_USAGE_STATS=false", "HALO_MAP_TORRENTS=false", "HALO_NET_MAPS_URL=", "HALO_WORKSHOP_MAPS=false",
             "HALO_NET_REPORT_HOSTED=false", "HALO_NET_REPORT_GAMES=false", "HALO_NET_REPORT_EVENTS=false",
             "HALO_CRASH_REPORTS=no", "HALO_DISCORD_REGISTER_LAUNCH=false"]
    environment = net_compare.copy_environment(copy, "127.0.0.240", "127.0.0.241", "local:" + args.map,
                                               7, settings, extra)
    environment.pop("SDL_VIDEO_DRIVER", None)
    log_path = work / "render.log"
    net_compare.stop_on_signals()
    with open(log_path, "w", encoding="utf-8") as log:
        process = net_compare.start(copy["command"], cwd=copy["folder"], env=environment,
                                    stdout=log, stderr=net_compare.subprocess.STDOUT)
        try:
            code = process.wait(timeout=args.seconds + 35)
        finally:
            net_compare.stop(process)
    images = sorted(frames.glob("*.bmp"))
    text = log_path.read_text(encoding="utf-8", errors="replace")
    report = {"schema": "forerunner.native-render/1", "exit": code, "engine": str(engine),
              "engine_sha256": hashlib.sha256(engine.read_bytes()).hexdigest(), "map": args.map,
              "native_player_ticks_logged": "network test: tick " in text,
              "frames": [str(path) for path in images], "log": str(log_path), "visual_review_required": True}
    (work / "result.json").write_text(json.dumps(report, indent=2) + "\n", encoding="utf-8")
    print(json.dumps(report, indent=2))
    return 0 if code == 0 and images and report["native_player_ticks_logged"] else 1


if __name__ == "__main__":
    sys.exit(main())
