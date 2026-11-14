#!/usr/bin/env python3
"""Check that two builds of the game play a network game in step.

Runs two builds (any two executables, each with its game data) in one
system link game on the loopback: build A hosts and B joins, then (with
--swap) B hosts and A joins. Each copy plays its player with a scripted,
seeded bot (debug.test_input "bot:<seed>") and logs every player's state
once a second (debug.network_test, port/linux/game/network_test.c). The
host decides; a client predicts its own player and runs the others from
the input the host relays. So between builds that simulate alike, what the
host logged of each player at a tick and what the client logged of that
player at the same tick agree to within what the netcode's prediction and
correction allow, and the two machines' copies of a player, where one hears
the other's word on it, stay a rounding apart ("drift", below). A build
that simulates otherwise disagrees there first.

The comparison and its tolerances are described in port/linux/NETCODE.md
("Comparing two builds"). It needs the retail maps, so it is a local (or
self-hosted) check, not a public CI one.

    python tools/net_compare.py --a build/linux/halo --data /path/to/halo \\
        --b ../other/build/linux/halo --maps bloodgulch,beavercreek --swap

Exit status: 0 when every run passes; 1 when a run diverges (FAIL), even if
another could not be played, as a divergence is the finding and an error
only a gap in it; else 2 when a run could not be played (ERROR: a copy
failed to start, join or log); 128 + N when the tool was stopped by signal
N (SIGINT, SIGTERM, SIGHUP), after stopping the copies.

Each copy has its own POSIX process group or Windows job, which the tool
ends on exit. Windows also closes the job when the tool is forcibly killed.
"""

import argparse
import atexit
import ctypes
from contextlib import contextmanager
import json
import math
import os
import re
import shlex
import shutil
import signal
import subprocess
import sys
import time
from pathlib import Path

ROOT = Path(__file__).resolve().parent.parent
TICKS_PER_SECOND = 30

# ---------- tolerances
#
# Chosen from how the netcode works (port/linux/NETCODE.md) and checked
# against identical builds (see the NETCODE.md section). The machines'
# clocks agree to a tick or two, a remote player's word comes a tick or so
# late and a client's prediction reaches the host as late, so the logged
# positions of a running player differ by the few ticks' running between
# them; a client corrects its copy of a remote player at once past
# REMOTE_BLEND_DISTANCE (1 world unit) and its own past
# LOCAL_CORRECTION_TOLERANCE (3), so further apart than that for long is no
# lag but a disagreement.

TOLERANCES = {
    # world units between the host's and a client's copy of a player at
    # one tick (a player runs about 0.08 a tick; jumps and falls more)
    "position": 1.0,
    # degrees of aim (yaw, pitch) apart: input a tick or two late, as the
    # bot turns (it turns at most a few degrees a tick)
    "aim": 15.0,
    # body and shield vitality apart (the host's word, a tick late)
    "vitality": 0.15,
    # rounds apart in a weapon (a client fires at once; the host's word
    # on its ammunition comes a round trip later)
    "rounds": 3,
    # consecutive seconds a discrete field (alive, riding, weapons,
    # grenades, scores, team) may disagree before it counts: the host's
    # word on a death, a spawn, a pickup or a score is a round trip or
    # a death blow's wait (network_distributed.c) late
    "persist": 3,
    # consecutive seconds the position may be out of tolerance (a
    # teleporter, a death's fall, a respawn's move)
    "persist_continuous": 2,
    # ... a weapon's rounds: a client keeps its own player's ammunition
    # while it fires, and takes the host's (a pickup's) once it has
    # stopped a round trip (network_objects.c): 4 seconds seen
    "persist_rounds": 6,
    # drift: on the host, the ticks where the host moved a client's player
    # one tick on from where it took the client's last prediction, with the
    # client's input of that tick, as the client did, and how far apart the
    # two moves were (network_distributed.c's distributed_divergence).
    # Builds that simulate alike move it alike to the last bit but where the
    # player touched something the two machines have in slightly different
    # places (the other player, a grenade's blast, an item): identical
    # builds have 0-13% of those ticks' moves further apart than
    # DIVERGENCE_APART_DISTANCE (0.0001 world units), up to 42% when the
    # machines' frames come unevenly (a loaded computer), 36% with
    # --events. A build whose players run 0.5% faster has 77-88% apart, 2%
    # faster 88-93%. More than drift_share of the ticks apart fails.
    # (Gravity 0.5-2% stronger is 25-38% apart, as it changes only the
    # ticks in the air: within what identical builds can show, so not
    # caught by this.)
    "drift_share": 0.6,
    # (fewer ticks compared than this is an error: the client's player
    # never on foot, or the two not paired)
    "drift_samples": 300,
}

LOG_PREFIX = "network test: tick "

PLAYER_RE = re.compile(r" player (\d+): ")
ALIVE_RE = re.compile(
    r"\((?P<x>-?[\d.]+|-?nan|-?inf) (?P<y>-?[\d.]+|-?nan|-?inf) (?P<z>-?[\d.]+|-?nan|-?inf)\) "
    r"h(?P<body>-?[\d.]+)/(?P<shield>-?[\d.]+)(?P<riding> riding)?(?P<camo> camo)? "
    r"g(?P<g0>\d+)/(?P<g1>\d+) w a(?P<yaw>-?\d+)/(?P<pitch>-?\d+) f(?P<facing>-?\d+) "
    r"l(?P<look_yaw>-?\d+)/(?P<look_pitch>-?\d+) as(?P<aim_screen>-?\d+)/(?P<look_screen>-?\d+) "
    r"st(?P<state>-?\d+) thr(?P<throttle>[\d.]+)(?P<weapons>(?: [0-9a-f]+:-?\d+)*) ")
DEAD_RE = re.compile(r"dead ")
STATS_RE = re.compile(r"s(?P<score>-?\d+) k(?P<kills>-?\d+) d(?P<deaths>-?\d+) f(?P<ff>-?\d+) "
                      r"t(?P<team>-?\d+) m(?P<machine>-?\d+)$")
DRIFT_RE = re.compile(r"\| drift (\d+) (\d+) ([\d.]+|-?nan|-?inf) ([\d.]+|-?nan|-?inf)")
CORRECTED_RE = re.compile(r"corrected (\d+)")
LOCAL_RE = re.compile(r"\| local (-?\d+) ")
STATUS_RE = re.compile(r"\| (playing|game over) to ")


# ---------- reading a copy's log


def parse_players(text):
    """{player index: state} of the players part of a log line"""
    players = {}
    starts = list(PLAYER_RE.finditer(text))
    for number, start in enumerate(starts):
        end = starts[number + 1].start() if number + 1 < len(starts) else len(text)
        body = text[start.end():end].rstrip() + " "
        index = int(start.group(1))
        state = {}
        alive = ALIVE_RE.match(body)
        if alive:
            state["alive"] = True
            state["position"] = (float(alive["x"]), float(alive["y"]), float(alive["z"]))
            state["body"] = float(alive["body"])
            state["shield"] = float(alive["shield"])
            state["riding"] = bool(alive["riding"])
            state["camo"] = bool(alive["camo"])
            state["grenades"] = (int(alive["g0"]), int(alive["g1"]))
            state["aim"] = (int(alive["yaw"]), int(alive["pitch"]))
            state["facing"] = int(alive["facing"])
            state["look"] = (int(alive["look_yaw"]), int(alive["look_pitch"]))
            state["animation"] = int(alive["state"])
            state["throttle"] = float(alive["throttle"])
            weapons = []
            for weapon in alive["weapons"].split():
                definition, rounds = weapon.split(":")
                weapons.append((definition, int(rounds)))
            state["weapons"] = weapons
            rest = body[alive.end():]
        elif DEAD_RE.match(body):
            state["alive"] = False
            rest = body[len("dead "):]
        else:
            raise ValueError(f"unreadable player {index}: {body!r}")
        stats = STATS_RE.match(rest.strip())
        if not stats:
            raise ValueError(f"unreadable statistics of player {index}: {rest!r}")
        for key in ("score", "kills", "deaths", "ff", "team", "machine"):
            state[key] = int(stats[key])
        players[index] = state
    return players


def parse_log(path):
    """{tick: sample} of a copy's per-second lines (the last game's, as a
    tick seen again starts the samples over)"""
    samples = {}
    names = {}
    errors = []
    with open(path, errors="replace") as log:
        for line in log:
            position = line.find(LOG_PREFIX)
            if position < 0:
                named = re.search(r"network test: player (\d+) is named (.*)$", line)
                if named:
                    names[int(named.group(1))] = named.group(2).strip()
                continue
            text = line[position + len(LOG_PREFIX):].rstrip("\n")
            parts = text.split(" | ")
            head = parts[0]
            tick_text, _, players_text = head.partition(" ")
            try:
                tick = int(tick_text)
                players = parse_players(" " + players_text)
            except ValueError as error:
                errors.append(f"{path}: {error}")
                continue
            sample = {"tick": tick, "players": players}
            rest = " | " + " | ".join(parts[1:])
            for pattern, key in ((CORRECTED_RE, "corrected"), (LOCAL_RE, "local")):
                found = pattern.search(rest)
                if found:
                    sample[key] = int(found.group(1))
            status = STATUS_RE.search(rest)
            sample["playing"] = bool(status and status.group(1) == "playing")
            drift = DRIFT_RE.search(rest)
            if drift:
                sample["drift"] = (int(drift.group(1)), int(drift.group(2)), float(drift.group(3)),
                                   float(drift.group(4)))
            if tick in samples or (samples and tick < max(samples)):
                # a new game (the next variant's): only the last is compared
                samples = {}
            samples[tick] = sample
    return samples, names, errors


# ---------- comparing


def angle_apart(a, b):
    difference = abs(a - b) % 360
    return min(difference, 360 - difference)


def distance(a, b):
    return math.sqrt(sum((p - q) ** 2 for p, q in zip(a, b)))


def pair_ticks(host, client):
    """[(host tick, client tick)] of the seconds both logged, the client's
    nearest within two ticks (each logs the first frame a second on)"""
    pairs = []
    client_ticks = sorted(client)
    for tick in sorted(host):
        best = None
        for other in client_ticks:
            if abs(other - tick) <= 2 and (best is None or abs(other - tick) < abs(best - tick)):
                best = other
        if best is not None:
            pairs.append((tick, best))
    return pairs


class Streaks:
    """consecutive seconds each (player, field) disagrees"""

    def __init__(self):
        self.counts = {}
        self.first = {}

    def note(self, key, bad, tick, detail):
        if bad:
            if self.counts.get(key, 0) == 0:
                self.first[key] = (tick, detail)
            self.counts[key] = self.counts.get(key, 0) + 1
        else:
            self.counts[key] = 0
        return self.counts.get(key, 0)


def compare_pair(host, client, tolerances, skip_seconds):
    """the divergences of one client's log against the host's, and figures
    of how far apart they were"""
    divergences = []
    figures = {"seconds": 0, "position_max": 0.0, "position_sum": 0.0, "position_count": 0,
               "aim_max": 0.0, "mismatches": {}}
    streaks = Streaks()
    pairs = pair_ticks(host, client)
    figures["tick_offsets"] = sorted({c - h for h, c in pairs})
    for host_tick, client_tick in pairs:
        if host_tick < skip_seconds * TICKS_PER_SECOND:
            continue
        host_sample = host[host_tick]
        client_sample = client[client_tick]
        if not host_sample["playing"] or not client_sample["playing"]:
            continue
        figures["seconds"] += 1
        found = []
        players = sorted(set(host_sample["players"]) | set(client_sample["players"]))
        for index in players:
            h = host_sample["players"].get(index)
            c = client_sample["players"].get(index)
            checks = []
            if h is None or c is None:
                checks.append(("present", True, f"host {'has' if h else 'lacks'} it, client {'has' if c else 'lacks'} it",
                               tolerances["persist"]))
            else:
                checks.append(("alive", h["alive"] != c["alive"], f"host {h['alive']} client {c['alive']}",
                               tolerances["persist"]))
                for key in ("score", "kills", "deaths", "ff", "team"):
                    checks.append((key, h[key] != c[key], f"host {h[key]} client {c[key]}", tolerances["persist"]))
                if h["alive"] and c["alive"]:
                    apart = distance(h["position"], c["position"])
                    finite = all(math.isfinite(v) for v in h["position"] + c["position"])
                    figures["position_max"] = max(figures["position_max"], apart if finite else math.inf)
                    figures["position_sum"] += apart if finite else 0.0
                    figures["position_count"] += 1
                    checks.append(("position", not finite or apart > tolerances["position"],
                                   f"{apart:.3f} world units apart (host {h['position']} client {c['position']})",
                                   tolerances["persist_continuous"]))
                    aim = max(angle_apart(h["aim"][0], c["aim"][0]), abs(h["aim"][1] - c["aim"][1]))
                    figures["aim_max"] = max(figures["aim_max"], aim)
                    # (a player thrown through the air by a blast: its aim,
                    # driven by no input, can differ for a second or two)
                    checks.append(("aim", aim > tolerances["aim"],
                                   f"{aim:.0f} degrees apart (host {h['aim']} client {c['aim']})",
                                   tolerances["persist"]))
                    for key in ("body", "shield"):
                        checks.append((key, abs(h[key] - c[key]) > tolerances["vitality"],
                                       f"host {h[key]:.2f} client {c[key]:.2f}", tolerances["persist"]))
                    checks.append(("riding", h["riding"] != c["riding"], f"host {h['riding']} client {c['riding']}",
                                   tolerances["persist"]))
                    checks.append(("grenades", h["grenades"] != c["grenades"],
                                   f"host {h['grenades']} client {c['grenades']}", tolerances["persist"]))
                    host_weapons = [w for w, _ in h["weapons"]]
                    client_weapons = [w for w, _ in c["weapons"]]
                    checks.append(("weapons", host_weapons != client_weapons,
                                   f"host {host_weapons} client {client_weapons}", tolerances["persist"]))
                    if host_weapons == client_weapons:
                        rounds = max([abs(a - b) for (_, a), (_, b) in zip(h["weapons"], c["weapons"])] or [0])
                        checks.append(("rounds", rounds > tolerances["rounds"],
                                       f"host {h['weapons']} client {c['weapons']}", tolerances["persist_rounds"]))
            for field, bad, detail, allowed in checks:
                key = (index, field)
                if bad:
                    figures["mismatches"][field] = figures["mismatches"].get(field, 0) + 1
                count = streaks.note(key, bad, host_tick, detail)
                if count == allowed:
                    first_tick, first_detail = streaks.first[key]
                    found.append({"player": index, "field": field, "tick": first_tick,
                                  "detail": first_detail, "seconds": count})
        divergences.extend(found)
    return divergences, figures


def drift_summary(samples, skip_seconds):
    """the host's drift figures over the game: ticks compared, ticks apart,
    mean and largest distance, and each second's (tick, compared, apart)"""
    total_samples = 0
    total_apart = 0
    total = 0.0
    largest = 0.0
    seconds = []
    for tick in sorted(samples):
        sample = samples[tick]
        if tick < skip_seconds * TICKS_PER_SECOND or "drift" not in sample or not sample["playing"]:
            continue
        count, apart, mean, top = sample["drift"]
        if count <= 0:
            continue
        if not (math.isfinite(mean) and math.isfinite(top)):
            mean = top = math.inf
            apart = count
        total_samples += count
        total_apart += apart
        total += mean * count
        largest = max(largest, top)
        seconds.append((tick, count, apart))
    return {
        "samples": total_samples,
        "apart": total_apart,
        "mean": total / total_samples if total_samples else 0.0,
        "largest": largest,
        "seconds": seconds,
    }


def judge_drift(summary, tolerances, player):
    """the divergence of the host's drift figures, if any: too many of the
    ticks compared apart (a tick apart now and then is the player touching
    something the two machines do not have in quite the same place: the
    other player, a grenade's blast, a weapon knocked about)"""
    if summary["samples"] < tolerances["drift_samples"]:
        return []
    share = summary["apart"] / summary["samples"]
    allowed = tolerances["drift_share"]
    if share <= allowed:
        return []
    # (the first second with as many apart)
    first = next(((tick, count, apart) for tick, count, apart in summary["seconds"]
                  if apart / count > allowed and count >= 10), summary["seconds"][0])
    return [{"player": player, "field": "drift", "tick": first[0],
             "detail": f"the host's and the client's moves of the client's player, a tick from the same place "
                       f"with the same input, were apart in {summary['apart']} of {summary['samples']} ticks "
                       f"({share:.0%}; tolerance {allowed:.0%}), {summary['mean']:.6f} world "
                       f"units on average, {summary['largest']:.6f} at most; the first second as far apart ends "
                       f"at tick {first[0]} ({first[2]} of {first[1]} ticks)"}]


# ---------- running the copies


def link_once(link, target):
    if not link.exists() and not link.is_symlink():
        link.symlink_to(target)


def wrapper_arguments(command):
    if not command:
        return []
    if os.name != "nt":
        return shlex.split(command)
    # Windows paths and quoting follow CommandLineToArgvW, not a POSIX shell.
    from ctypes import wintypes

    shell = ctypes.WinDLL("shell32", use_last_error=True)
    kernel = ctypes.WinDLL("kernel32", use_last_error=True)
    shell.CommandLineToArgvW.argtypes = (wintypes.LPCWSTR, ctypes.POINTER(ctypes.c_int))
    shell.CommandLineToArgvW.restype = ctypes.POINTER(wintypes.LPWSTR)
    kernel.LocalFree.argtypes = (wintypes.HLOCAL,)
    kernel.LocalFree.restype = wintypes.HLOCAL
    count = ctypes.c_int()
    arguments = shell.CommandLineToArgvW(command, ctypes.byref(count))
    if not arguments:
        raise ctypes.WinError(ctypes.get_last_error())
    try:
        return list(arguments[:count.value])
    finally:
        kernel.LocalFree(arguments)


def prepare_copy(work, name, executable, data_root, wrapper=""):
    """a folder for one copy: its own executable (config.toml is written
    in the game's folder, beside it), its own data root
    (debug.txt goes there; maps/ is the data's) and its own saves"""
    executable = Path(executable).resolve()
    data_root = Path(data_root).resolve()
    if not executable.is_file():
        raise SystemExit(f"net_compare: no executable {executable}")
    if not (data_root / "maps").is_dir():
        raise SystemExit(f"net_compare: no maps folder in {data_root}")
    folder = work / name
    bin_folder = folder / "bin"
    data = folder / "data"
    saves = folder / "saves"
    for path in (folder, bin_folder, data, saves):
        if os.name == "nt" and (path.is_symlink() or getattr(path, "is_junction", lambda: False)()):
            raise SystemExit(f"net_compare: use a fresh work folder; {path} is a link")
        path.mkdir(parents=True, exist_ok=True)
    # The Windows copies need no symlink permission, and have no writable
    # links to the caller's runtime or maps. Keep build/source directories
    # out: only runtime libraries and the known runtime data belong here.
    for entry in executable.parent.iterdir():
        if entry.name in (executable.name, "config.toml") or entry.name.endswith(".log"):
            continue
        if os.name == "nt":
            if entry.is_file() and (entry.suffix.lower() == ".dll" or entry.name in (
                    "brokers.txt", "masters.txt", "licenses.txt", "THIRD-PARTY-NOTICES.txt")):
                destination = bin_folder / entry.name
                if destination.is_symlink() or (destination.exists() and destination.samefile(entry)):
                    raise SystemExit(f"net_compare: use a fresh work folder; {destination} links to the runtime")
                if not destination.exists():
                    shutil.copy2(entry, destination)
            elif entry.is_dir() and entry.name in ("Faber_data", "fonts"):
                destination = bin_folder / entry.name
                if destination.is_symlink() or getattr(destination, "is_junction", lambda: False)():
                    raise SystemExit(f"net_compare: use a fresh work folder; {destination} is a link")
                if not destination.exists():
                    shutil.copytree(entry, destination)
        else:
            link_once(bin_folder / entry.name, entry.resolve())
    copied = bin_folder / executable.name
    if os.name == "nt" and (copied.is_symlink() or (copied.exists() and copied.samefile(executable))):
        raise SystemExit(f"net_compare: use a fresh work folder; {copied} links to the executable")
    if not copied.exists():
        if os.name == "nt":
            shutil.copy2(executable, copied)
        else:
            try:
                os.link(executable, copied)
            except OSError:
                shutil.copy2(executable, copied)
    for entry in ("maps", "custom_maps", "maps_ce", "maps_pc", "maps_md", "maps_cex"):
        if os.name == "nt" and ((data / entry).is_symlink() or
                                getattr(data / entry, "is_junction", lambda: False)()):
            raise SystemExit(f"net_compare: use a fresh work folder; {data / entry} is a link")
        if (data_root / entry).is_dir() and not (data / entry).exists():
            if os.name == "nt":
                shutil.copytree(data_root / entry, data / entry)
            else:
                (data / entry).symlink_to((data_root / entry).resolve())
    return {"name": name, "executable": copied, "data": data, "saves": saves, "folder": folder,
            "command": wrapper_arguments(wrapper) + [str(copied)]}


def copy_environment(copy, address, broadcast, test, seed, args, extra):
    environment = dict(os.environ)
    for key in [key for key in environment if key.startswith("HALO_")]:
        del environment[key]
    environment.update({
        "HALO_DATA_ROOT": str(copy["data"]),
        "HALO_SAVE_ROOT": str(copy["saves"]),
        "HALO_NET_ADDRESS": address,
        "HALO_NET_BROADCAST": broadcast,
        "HALO_NET_ONLINE": "false",
        "HALO_NET_PUBLIC_LOBBY": "false",
        "HALO_NET_MAP_SHARE": "false",
        "HALO_NET_MAP_SEED": "false",
        "HALO_NET_MAP_DOWNLOAD": "false",
        "HALO_NET_JOIN_FROM_CLIPBOARD": "false",
        "HALO_DISCORD_APPLICATION": "",
        "HALO_UPDATE_AUTO": "false",
        "HALO_NETWORK_TEST": test,
        "HALO_NETWORK_TEST_START": str(args.start),
        "HALO_TEST_INPUT": f"bot:{seed}",
        # (a safety: the tool stops the copies itself)
        "HALO_EXIT_AFTER": str(args.start + args.skip + args.seconds + 120),
        "SDL_AUDIO_DRIVER": "dummy",
    })
    if args.events:
        # hits every few seconds (each machine's player the next player)
        # and a kill now and then (the host's), so that damage, deaths,
        # respawns and scores are compared too
        environment["HALO_NETWORK_TEST_SHOOT"] = "4"
        environment["HALO_NETWORK_TEST_KILL"] = "17"
    if args.window:
        # (on the display the tool was started with: DISPLAY or
        # WAYLAND_DISPLAY as they are, or --display's)
        environment["HALO_HIDDEN_WINDOW"] = "1"
        if args.display:
            environment["DISPLAY"] = args.display
            environment.pop("WAYLAND_DISPLAY", None)
            environment["SDL_VIDEO_DRIVER"] = "x11"
    else:
        environment["HALO_NULL_RENDERER"] = "1"
        # (SDL's video started without a display: without it, SDL_Init
        # fails, and is tried again, every frame; and none of the caller's,
        # so that no copy can open a window on it)
        environment["SDL_VIDEO_DRIVER"] = "dummy"
        for key in ("DISPLAY", "WAYLAND_DISPLAY"):
            environment.pop(key, None)
    for setting in extra:
        key, _, value = setting.partition("=")
        environment[key] = value
    return environment


def latest_tick(path):
    """the last per-second tick a copy's log has, and whether it plays"""
    tick = None
    try:
        with open(path, "rb") as log:
            log.seek(max(0, os.path.getsize(path) - 65536))
            for line in log.read().decode(errors="replace").splitlines():
                position = line.find(LOG_PREFIX)
                if position >= 0:
                    try:
                        tick = int(line[position + len(LOG_PREFIX):].split(" ", 1)[0])
                    except ValueError:
                        pass
    except OSError:
        pass
    return tick


# ---------- the copies' processes
#
# On POSIX each copy runs in a session of its own (start_new_session), so that the
# copy and whatever it started (a wrapper's) are one process group, which
# stop() ends; and so that a terminal's Ctrl-C reaches only the tool, which
# stops them. The tool stops them on every way out: returning, an exception,
# Ctrl-C (KeyboardInterrupt), SIGTERM and SIGHUP (Stopped), and atexit; and
# a copy is killed by the kernel if the tool dies without that (SIGKILL:
# PR_SET_PDEATHSIG). On Windows a kill-on-close job owns the copy and every
# descendant. Its non-inherited handle lives only in this tool, so even an
# uncatchable termination of the tool closes it and ends the whole tree.

# the copies running
RUNNING = []
# the signals the tool stops on, and the one that stopped it
STOP_SIGNALS = tuple(getattr(signal, name) for name in ("SIGINT", "SIGTERM", "SIGHUP", "SIGBREAK")
                     if hasattr(signal, name))
stopping = None


class Stopped(Exception):
    """the tool received a stop signal"""

    def __init__(self, number):
        super().__init__(f"stopped by signal {number}")
        self.number = number


def on_stop_signal(number, frame):
    global stopping
    # (once: a second signal while the copies are being stopped waits)
    if stopping is not None:
        return
    stopping = number
    if number == signal.SIGINT:
        raise KeyboardInterrupt
    raise Stopped(number)


def stop_on_signals():
    for number in STOP_SIGNALS:
        signal.signal(number, on_stop_signal)
    atexit.register(stop_all)


def die_with_the_tool(parent=os.getpid()):
    """(in a copy, before it runs) SIGKILL when the tool ends, however it
    ends (Linux's PR_SET_PDEATHSIG; elsewhere nothing)"""
    try:
        ctypes.CDLL(None, use_errno=True).prctl(1, int(signal.SIGKILL), 0, 0, 0)  # PR_SET_PDEATHSIG
    except (OSError, AttributeError):
        return
    # (the tool gone already, before that)
    if os.getppid() != parent:
        os.kill(os.getpid(), signal.SIGKILL)


@contextmanager
def hold_stop_signals():
    """Finish registering or stopping a process before handling a stop."""
    if hasattr(signal, "pthread_sigmask"):
        blocked = signal.pthread_sigmask(signal.SIG_BLOCK, STOP_SIGNALS)
        try:
            yield
        finally:
            signal.pthread_sigmask(signal.SIG_SETMASK, blocked)
    else:
        # Windows has no signal mask. Defer its Python signal handlers until
        # the job and RUNNING entry agree; the job also handles forced death.
        pending = []
        previous = {number: signal.signal(number, lambda number, frame: pending.append(number))
                    for number in STOP_SIGNALS}
        try:
            yield
        finally:
            for number, handler in previous.items():
                signal.signal(number, handler)
        if pending:
            on_stop_signal(pending[0], None)


class WindowsJob:
    """A private, non-inherited Windows job that cannot leave descendants."""

    def __init__(self):
        from ctypes import wintypes

        class BasicLimits(ctypes.Structure):
            _fields_ = [("process_time", ctypes.c_int64), ("job_time", ctypes.c_int64),
                        ("flags", wintypes.DWORD), ("minimum_working_set", ctypes.c_size_t),
                        ("maximum_working_set", ctypes.c_size_t), ("active_processes", wintypes.DWORD),
                        ("affinity", ctypes.c_size_t), ("priority", wintypes.DWORD),
                        ("scheduling", wintypes.DWORD)]

        class ExtendedLimits(ctypes.Structure):
            _fields_ = [("basic", BasicLimits), ("io_counters", ctypes.c_uint64 * 6),
                        ("process_memory", ctypes.c_size_t), ("job_memory", ctypes.c_size_t),
                        ("peak_process_memory", ctypes.c_size_t), ("peak_job_memory", ctypes.c_size_t)]

        class ThreadEntry(ctypes.Structure):
            _fields_ = [("size", wintypes.DWORD), ("usage", wintypes.DWORD),
                        ("thread", wintypes.DWORD), ("process", wintypes.DWORD),
                        ("base_priority", wintypes.LONG), ("delta_priority", wintypes.LONG),
                        ("flags", wintypes.DWORD)]

        self.thread_entry = ThreadEntry
        self.kernel = ctypes.WinDLL("kernel32", use_last_error=True)
        signatures = {
            "CreateJobObjectW": ((ctypes.c_void_p, wintypes.LPCWSTR), wintypes.HANDLE),
            "SetInformationJobObject": ((wintypes.HANDLE, ctypes.c_int, ctypes.c_void_p,
                                         wintypes.DWORD), wintypes.BOOL),
            "AssignProcessToJobObject": ((wintypes.HANDLE, wintypes.HANDLE), wintypes.BOOL),
            "CloseHandle": ((wintypes.HANDLE,), wintypes.BOOL),
            "CreateToolhelp32Snapshot": ((wintypes.DWORD, wintypes.DWORD), wintypes.HANDLE),
            "Thread32First": ((wintypes.HANDLE, ctypes.POINTER(ThreadEntry)), wintypes.BOOL),
            "Thread32Next": ((wintypes.HANDLE, ctypes.POINTER(ThreadEntry)), wintypes.BOOL),
            "OpenThread": ((wintypes.DWORD, wintypes.BOOL, wintypes.DWORD), wintypes.HANDLE),
            "ResumeThread": ((wintypes.HANDLE,), wintypes.DWORD),
        }
        for name, (arguments, result) in signatures.items():
            function = getattr(self.kernel, name)
            function.argtypes = arguments
            function.restype = result
        self.handle = self.kernel.CreateJobObjectW(None, None)
        if not self.handle:
            raise ctypes.WinError(ctypes.get_last_error())
        limits = ExtendedLimits()
        limits.basic.flags = 0x2000  # JOB_OBJECT_LIMIT_KILL_ON_JOB_CLOSE
        if not self.kernel.SetInformationJobObject(self.handle, 9, ctypes.byref(limits), ctypes.sizeof(limits)):
            error = ctypes.WinError(ctypes.get_last_error())
            self.close()
            raise error

    def assign(self, process):
        if not self.kernel.AssignProcessToJobObject(self.handle, int(process._handle)):
            raise ctypes.WinError(ctypes.get_last_error())

    def resume(self, process):
        # Popen closes the primary thread's handle. Find it while the child
        # is still suspended, then use the documented ResumeThread API.
        snapshot = self.kernel.CreateToolhelp32Snapshot(4, 0)  # TH32CS_SNAPTHREAD
        if snapshot == ctypes.c_void_p(-1).value:
            raise ctypes.WinError(ctypes.get_last_error())
        try:
            entry = self.thread_entry()
            entry.size = ctypes.sizeof(entry)
            found = self.kernel.Thread32First(snapshot, ctypes.byref(entry))
            while found:
                if entry.process == process.pid:
                    thread = self.kernel.OpenThread(2, False, entry.thread)  # THREAD_SUSPEND_RESUME
                    if not thread:
                        raise ctypes.WinError(ctypes.get_last_error())
                    try:
                        if self.kernel.ResumeThread(thread) == 0xFFFFFFFF:
                            raise ctypes.WinError(ctypes.get_last_error())
                    finally:
                        self.kernel.CloseHandle(thread)
                    return
                found = self.kernel.Thread32Next(snapshot, ctypes.byref(entry))
            raise OSError(f"net_compare: no primary thread for suspended process {process.pid}")
        finally:
            self.kernel.CloseHandle(snapshot)

    def close(self):
        if self.handle:
            self.kernel.CloseHandle(self.handle)
            self.handle = None


def start(command, **options):
    """Start a copy under process-tree ownership before handling a stop."""
    with hold_stop_signals():
        if os.name == "nt":
            job = WindowsJob()
            process = None
            try:
                flags = (options.pop("creationflags", 0) | 0x4 | subprocess.CREATE_NEW_PROCESS_GROUP |
                         subprocess.CREATE_NO_WINDOW)
                process = subprocess.Popen(command, creationflags=flags, **options)  # CREATE_SUSPENDED
                job.assign(process)
                process.net_compare_job = job
                RUNNING.append(process)
                job.resume(process)
            except BaseException:
                job.close()
                if process is not None:
                    if process.poll() is None:
                        process.kill()
                    process.wait(timeout=10)
                    if process in RUNNING:
                        RUNNING.remove(process)
                raise
        else:
            parent = os.getpid()
            process = subprocess.Popen(command, start_new_session=True,
                                       preexec_fn=lambda: die_with_the_tool(parent), **options)
            RUNNING.append(process)
    return process


def signal_group(process, number):
    try:
        os.killpg(process.pid, number)
    except (ProcessLookupError, PermissionError):
        pass


def stop(process):
    """End a Windows job, or a POSIX process group: SIGTERM (the game quits on it, its files
    all written by now), then SIGKILL two seconds on (a build from before it
    quit on SIGTERM)"""
    if os.name == "nt":
        # Closing the last job handle ends the entire tree even when the
        # wrapper has already exited. Never search for or kill unrelated PIDs.
        process.net_compare_job.close()
        process.wait(timeout=10)
    elif process.poll() is None:
        signal_group(process, signal.SIGTERM)
        try:
            process.wait(timeout=2)
        except subprocess.TimeoutExpired:
            pass
        # (the group: the copy, and what it started that is left)
        signal_group(process, signal.SIGKILL)
        process.wait()
    if process in RUNNING:
        RUNNING.remove(process)


def stop_all():
    with hold_stop_signals():
        for process in list(RUNNING):
            stop(process)


def play(run_folder, host_copy, client_copy, map_name, args, host_extra, client_extra, seeds):
    """runs one game; the paths of the host's and the client's logs"""
    run_folder.mkdir(parents=True, exist_ok=True)
    base = args.address_base.rsplit(".", 1)
    host_address = args.address_base
    client_address = f"{base[0]}.{int(base[1]) + 1}"
    host_log = run_folder / "host.log"
    client_log = run_folder / "client.log"
    # (each copy's debug.txt is its own, and grows: a fresh one each game)
    for copy in (host_copy, client_copy):
        (copy["data"] / "debug.txt").unlink(missing_ok=True)
    test = f"host:{map_name}" + (f":{args.variant}" if args.variant else "")
    processes = []
    try:
        with open(host_log, "w") as log:
            processes.append(start(
                host_copy["command"], cwd=host_copy["folder"], stdout=log, stderr=subprocess.STDOUT,
                env=copy_environment(host_copy, host_address, client_address, test, seeds[0], args, host_extra)))
        time.sleep(1.0)
        with open(client_log, "w") as log:
            processes.append(start(
                client_copy["command"], cwd=client_copy["folder"], stdout=log, stderr=subprocess.STDOUT,
                env=copy_environment(client_copy, client_address, host_address, "join", seeds[1], args,
                                     client_extra)))
        goal = (args.skip + args.seconds) * TICKS_PER_SECOND
        deadline = time.monotonic() + args.start + args.seconds + args.skip + args.timeout
        while time.monotonic() < deadline:
            if any(process.poll() is not None for process in processes):
                break
            ticks = [latest_tick(host_log), latest_tick(client_log)]
            if all(tick is not None and tick >= goal for tick in ticks):
                # (a second more, so the client's last second is logged too)
                time.sleep(1.0)
                break
            time.sleep(0.5)
        exits = [process.poll() for process in processes]
    finally:
        # (with the signals that stop the tool held off, so that the
        # stopping is not cut short)
        with hold_stop_signals():
            for process in processes:
                stop(process)
    # (the end of each copy's debug.txt, beside its log: a crash's story)
    for copy, name in ((host_copy, "host"), (client_copy, "client")):
        try:
            debug_path = next((path for path in (copy["saves"] / "logs/debug.txt",
                                                copy["data"] / "logs/debug.txt", copy["data"] / "debug.txt")
                               if path.is_file()), copy["data"] / "debug.txt")
            with open(debug_path, "rb") as debug:
                debug.seek(max(0, os.path.getsize(debug.name) - 65536))
                (run_folder / f"{name}.debug.txt").write_bytes(debug.read())
        except OSError:
            pass
    return host_log, client_log, exits


# ---------- a run, judged


def judge_run(host_log, client_log, args, tolerances):
    host, host_names, host_errors = parse_log(host_log)
    client, _, client_errors = parse_log(client_log)
    result = {"divergences": [], "problems": host_errors + client_errors}
    if not host:
        result["problems"].append(f"the host logged no game ({host_log})")
    if not client:
        result["problems"].append(f"the client logged no game ({client_log})")
    if result["problems"] and (not host or not client):
        return result
    played = (max(client) - min(client)) // TICKS_PER_SECOND
    if played < args.seconds * 3 // 4:
        result["problems"].append(f"the client played only {played} seconds of {args.seconds}")
    players = set(host[max(host)]["players"])
    if len(players) < 2:
        result["problems"].append("the client's player never joined the host's game")
    divergences, figures = compare_pair(host, client, tolerances, args.skip)
    result["figures"] = figures
    result["divergences"].extend(divergences)
    drift = drift_summary(host, args.skip)
    if not any("drift" in sample for sample in host.values()):
        result["problems"].append("the host's log has no drift figures (a build from before net_compare.py)")
    client_players = sorted({sample["local"] for sample in client.values() if sample.get("local", -1) >= 0})
    result["divergences"].extend(judge_drift(drift, tolerances, client_players[0] if client_players else None))
    result["drift"] = {key: value for key, value in drift.items() if key != "seconds"}
    if drift["samples"] < tolerances["drift_samples"]:
        result["problems"].append(f"only {drift['samples']} ticks of the client's player on foot to judge the "
                                  f"drift by (fewer than {tolerances['drift_samples']})")
    corrections = [client[tick].get("corrected") for tick in sorted(client) if "corrected" in client[tick]]
    result["client_corrections"] = corrections[-1] - corrections[0] if len(corrections) > 1 else 0
    result["divergences"].sort(key=lambda item: (item["tick"], str(item["player"])))
    return result


def describe(result):
    lines = []
    figures = result.get("figures")
    if figures:
        mean = figures["position_sum"] / figures["position_count"] if figures["position_count"] else 0.0
        lines.append(f"  {figures['seconds']} seconds compared, tick offsets {figures['tick_offsets']}; "
                     f"positions apart mean {mean:.3f} largest {figures['position_max']:.3f}, aim largest "
                     f"{figures['aim_max']:.0f} degrees; mismatched seconds {figures['mismatches'] or 'none'}")
    if "drift" in result:
        summary = result["drift"]
        share = summary["apart"] / summary["samples"] if summary["samples"] else 0.0
        lines.append(f"  drift: {summary['apart']} of {summary['samples']} ticks apart ({share:.1%}), mean "
                     f"{summary['mean']:.6f}, largest {summary['largest']:.6f} world units")
    if "client_corrections" in result:
        lines.append(f"  client corrections: {result['client_corrections']}")
    for problem in result["problems"]:
        lines.append(f"  PROBLEM: {problem}")
    if result["divergences"]:
        first = result["divergences"][0]
        who = f"player {first['player']}" if first["player"] is not None else "the copies"
        lines.append(f"  FIRST DIVERGENCE: {who}, tick {first['tick']}, {first['field']}: {first['detail']}")
        for other in result["divergences"][1:6]:
            who = f"player {other['player']}" if other["player"] is not None else "the copies"
            lines.append(f"    then {who}, tick {other['tick']}, {other['field']}: {other['detail']}")
        if len(result["divergences"]) > 6:
            lines.append(f"    ({len(result['divergences']) - 6} more)")
    return lines


def parse_tolerance(setting):
    """(name, value) of a --tolerance NAME=VALUE; ValueError with the reason"""
    name, equals, text = setting.partition("=")
    name = name.strip()
    if not equals:
        raise ValueError(f"{setting!r} is not NAME=VALUE")
    if name not in TOLERANCES:
        raise ValueError(f"no tolerance {name!r} (the tolerances: {', '.join(TOLERANCES)})")
    kind = type(TOLERANCES[name])
    try:
        value = kind(text.strip())
    except ValueError:
        raise ValueError(f"{name} takes {'a whole number' if kind is int else 'a number'}, not {text!r}") from None
    if kind is float and not math.isfinite(value):
        raise ValueError(f"{name} takes a finite number, not {text!r}")
    if value < 0:
        raise ValueError(f"{name} cannot be negative ({text!r})")
    if name == "drift_share" and value > 1:
        raise ValueError(f"drift_share is a share of the ticks, from 0 to 1, not {text!r}")
    return name, value


# a FAIL is the finding, so it decides the exit status over an ERROR
EXIT_STATUSES = {"PASS": 0, "ERROR": 2, "FAIL": 1}


def exit_status(verdicts):
    """the tool's exit status of the runs' verdicts: 1 if any FAIL, else 2 if
    any ERROR, else 0"""
    verdicts = set(verdicts)
    if "FAIL" in verdicts:
        return EXIT_STATUSES["FAIL"]
    if "ERROR" in verdicts:
        return EXIT_STATUSES["ERROR"]
    return EXIT_STATUSES["PASS"]


def main():
    parser = argparse.ArgumentParser(description=__doc__.split("\n\n")[0],
                                     formatter_class=argparse.RawDescriptionHelpFormatter)
    parser.add_argument("--a", help="build A's executable")
    parser.add_argument("--b", help="build B's executable (default: A's)")
    parser.add_argument("--data", help="the game data's folder (holding maps/) for A")
    parser.add_argument("--b-data", help="the game data's folder for B (default: --data)")
    parser.add_argument("--a-env", action="append", default=[], metavar="KEY=VALUE",
                        help="an environment variable for A's copies (repeatable)")
    parser.add_argument("--b-env", action="append", default=[], metavar="KEY=VALUE",
                        help="an environment variable for B's copies (repeatable)")
    parser.add_argument("--a-wrapper", default="", metavar="COMMAND",
                        help="a command A's executable is run with (a tracer: \"strace -f -o a.trace\")")
    parser.add_argument("--b-wrapper", default="", metavar="COMMAND", help="the same for B")
    parser.add_argument("--maps", default="bloodgulch,beavercreek",
                        help="comma-separated multiplayer maps, a game on each")
    parser.add_argument("--variant", default="", help="the game variant (slayer if empty)")
    parser.add_argument("--seconds", type=int, default=60, help="seconds of play compared in each game")
    parser.add_argument("--skip", type=int, default=3, help="seconds at a game's start not compared (spawning)")
    parser.add_argument("--start", type=float, default=8.0,
                        help="seconds after hosting that the game starts (debug.network_test_start)")
    parser.add_argument("--timeout", type=float, default=90.0,
                        help="seconds more than the game's to wait for loading and joining")
    parser.add_argument("--events", action="store_true",
                        help="script hits and kills (debug.network_test_shoot, debug.network_test_kill)")
    parser.add_argument("--swap", action="store_true", help="also play each map with B hosting and A joining")
    parser.add_argument("--repeat", type=int, default=1, help="play each game this many times")
    parser.add_argument("--seed", type=int, default=1, help="the bots' first seed (each run its own)")
    parser.add_argument("--address-base", default="127.0.0.210",
                        help="the host's loopback address; the client takes the next")
    parser.add_argument("--window", action="store_true",
                        help="draw in a hidden window instead of the null renderer, on the display the tool has "
                             "(DISPLAY or WAYLAND_DISPLAY; a virtual one, Xvfb, rather than a desktop), or --display's")
    parser.add_argument("--display", help="with --window: the X display for the copies' windows (\":99\"), "
                                          "through SDL's X11 driver")
    parser.add_argument("--work", help="the folder for the copies' files and logs (default: a new one in "
                                       "build/net_compare, removed if every game passes)")
    parser.add_argument("--keep", action="store_true", help="keep the default folder when every game passes")
    parser.add_argument("--tolerance", action="append", default=[], metavar="NAME=VALUE",
                        help="change a tolerance (" + ", ".join(TOLERANCES) + ")")
    parser.add_argument("--json", help="write the results to this file as JSON")
    parser.add_argument("--compare", nargs=2, metavar=("HOST_LOG", "CLIENT_LOG"),
                        help="only compare two logs already written")
    args = parser.parse_args()

    tolerances = dict(TOLERANCES)
    for setting in args.tolerance:
        try:
            name, value = parse_tolerance(setting)
        except ValueError as error:
            parser.error(f"--tolerance: {error}")
        tolerances[name] = value

    if args.compare:
        for log in args.compare:
            if not Path(log).is_file():
                parser.error(f"--compare: no log {log}")
        result = judge_run(Path(args.compare[0]), Path(args.compare[1]), args, tolerances)
        verdict = "FAIL" if result["divergences"] else ("ERROR" if result["problems"] else "PASS")
        print(f"{verdict}: {args.compare[0]} against {args.compare[1]}")
        print("\n".join(describe(result)))
        return exit_status([verdict])

    if not args.a or not args.data:
        parser.error("--a and --data are needed to play (or --compare two logs)")
    if args.display and not args.window:
        parser.error("--display is for --window's windows")
    stop_on_signals()
    try:
        return play_all(args, tolerances)
    except (Stopped, KeyboardInterrupt) as stopped:
        number = stopped.number if isinstance(stopped, Stopped) else signal.SIGINT
        stop_all()
        print(f"net_compare: stopped by signal {int(number)} ({signal.Signals(number).name}); the copies were stopped",
              file=sys.stderr, flush=True)
        return 128 + int(number)
    finally:
        stop_all()


def play_all(args, tolerances):
    """plays every game, prints and returns the exit status"""
    # (on disk: the copies' saves hold map caches of hundreds of megabytes,
    # too many for a /tmp in memory)
    temporary = args.work is None
    work = Path(args.work or ROOT / "build" / "net_compare" / time.strftime("%Y%m%d-%H%M%S"))
    work.mkdir(parents=True, exist_ok=True)
    copies = {
        "A": prepare_copy(work, "A", args.a, args.data, args.a_wrapper),
        "B": prepare_copy(work, "B", args.b or args.a, args.b_data or args.data, args.b_wrapper),
    }
    extra = {"A": args.a_env, "B": args.b_env}
    orders = [("A", "B")] + ([("B", "A")] if args.swap else [])
    results = []
    seed = args.seed
    for repeat in range(args.repeat):
        for map_name in [name for name in args.maps.split(",") if name]:
            for host_name, client_name in orders:
                label = f"{map_name}, {host_name} hosts, {client_name} joins" + \
                        (f", run {repeat + 1}" if args.repeat > 1 else "")
                run_folder = work / "runs" / f"{map_name}.{host_name}{client_name}.{repeat + 1}"
                host_copy = copies[host_name]
                client_copy = copies[client_name]
                started = time.monotonic()
                host_log, client_log, exits = play(run_folder, host_copy, client_copy, map_name, args,
                                                   extra[host_name], extra[client_name], (seed, seed + 1))
                seed += 2
                result = judge_run(host_log, client_log, args, tolerances)
                if any(code not in (None, 0) for code in exits):
                    result["problems"].append(f"a copy ended early (exit statuses {exits})")
                verdict = "FAIL" if result["divergences"] else ("ERROR" if result["problems"] else "PASS")
                print(f"{verdict}: {label} ({time.monotonic() - started:.0f} s; logs {run_folder})")
                print("\n".join(describe(result)), flush=True)
                result.update({"map": map_name, "host": host_name, "client": client_name, "repeat": repeat + 1,
                               "verdict": verdict, "host_log": str(host_log), "client_log": str(client_log)})
                results.append(result)
    passed = sum(result["verdict"] == "PASS" for result in results)
    print(f"{passed} of {len(results)} games in step"
          + ("" if passed == len(results) else f"; {len(results) - passed} not"))
    if args.json:
        with open(args.json, "w") as output:
            json.dump({"a": args.a, "b": args.b or args.a, "tolerances": tolerances, "results": results},
                      output, indent=1, default=str)
    status = exit_status(result["verdict"] for result in results)
    if temporary and not args.keep and status == 0:
        shutil.rmtree(work, ignore_errors=True)
    elif temporary:
        print(f"files kept in {work}")
    return status


if __name__ == "__main__":
    sys.exit(main())
