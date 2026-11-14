#!/usr/bin/env python3
"""Compare opt-in native AI readback; empty or cinematic-only logs cannot pass.

This checks actual replicated unit state, not AI decisions on the client (only
the host runs AI). A pass is bounded snapshot agreement, not campaign completion;
the native launcher must also verify process exits and its private debug logs.
"""
import argparse
from dataclasses import dataclass, field
import json
import math
from pathlib import Path
import re
import sys

HANDLE = re.compile(r"[0-9a-fA-F]{8}")


@dataclass
class Snapshot:
    tick: int
    actors: dict = field(default_factory=dict)
    cinematic: bool = False
    bsp: int = -1


def parse_log(path, role):
    snapshots, pending = {}, {}
    for number, line in enumerate(Path(path).read_text(encoding="utf-8", errors="replace").splitlines(), 1):
        if "EXCEPTION halt" in line or "EXCEPTION_ACCESS_VIOLATION" in line:
            raise ValueError(f"{path}:{number}: native engine exception")
        actor = "network test: actor " in line
        summary = "network test: actors " in line
        if not actor and not summary:
            continue
        try:
            parts = line.split("network test: actor" + ("s " if summary else " "), 1)[1].split()
            tick = int(parts[0])
            if tick < 0 or parts[1] != role or tick in snapshots:
                raise ValueError("unexpected role, repeated tick, or negative tick")
            snapshot = pending.setdefault(tick, Snapshot(tick))
            if actor:
                if len(parts) != 13 or not all(HANDLE.fullmatch(parts[i]) for i in (2, 3, 10)):
                    raise ValueError("invalid actor fields or full network handle")
                values = [float(value) for value in parts[4:9]]
                if not all(math.isfinite(value) for value in values):
                    raise ValueError("non-finite unit state")
                if parts[9] not in ("0", "1") or parts[12] not in ("0", "1"):
                    raise ValueError("invalid dead/tracked flag")
                identity = int(parts[2], 16)
                if identity in snapshot.actors:
                    raise ValueError("duplicate unit handle")
                snapshot.actors[identity] = {
                    "definition": int(parts[3], 16), "position": values[:3],
                    "body": values[3], "shield": values[4], "dead": int(parts[9]),
                    "parent": int(parts[10], 16), "seat": int(parts[11]), "tracked": int(parts[12])}
            else:
                if len(parts) != 7 or parts[3] != "cinematic" or parts[4] not in ("0", "1") or parts[5] != "bsp":
                    raise ValueError("invalid snapshot terminator")
                if int(parts[2]) != len(snapshot.actors):
                    raise ValueError("snapshot count differs from actual records")
                snapshot.cinematic, snapshot.bsp = parts[4] == "1", int(parts[6])
                snapshots[tick] = snapshot
                del pending[tick]
        except (ValueError, IndexError) as error:
            raise ValueError(f"{path}:{number}: {error}") from error
    if pending:
        raise ValueError(f"{path}: unterminated actor snapshots at ticks {sorted(pending)}")
    return snapshots


def compare(host, client, *, position=1.0, vitality=0.15, persist=3, minimum_snapshots=5,
            tick_slop=2, require_damage=False):
    result = {"schema": "forerunner.native-actors/1", "verdict": "ERROR", "problems": [],
              "divergences": [], "limits": {"position": position, "vitality": vitality,
                  "persist": persist, "minimum_snapshots": minimum_snapshots, "tick_slop": tick_slop,
                  "corpse_body": "nonpositive body values equivalent only when both units are dead"},
              "stats": {"compared_snapshots": 0, "compared_units": 0, "position_max": 0.0,
                        "normalized_corpse_body_samples": 0, "raw_corpse_body_difference_max": 0.0}}
    available = {tick: sample for tick, sample in client.items() if not sample.cinematic}
    runs, reported, previous = {}, set(), {"host": {}, "client": {}}
    moved, damaged, deaths, living = ({"host": set(), "client": set()} for _ in range(4))

    def mismatch(key, tick, detail):
        runs[key] = runs.get(key, 0) + 1
        if runs[key] >= persist and key not in reported:
            result["divergences"].append({"tick": tick, "unit": key[0], "field": key[1], "detail": detail})
            reported.add(key)

    def observe(role, identity, state):
        old = previous[role].get(identity)
        if not state["dead"] and state["tracked"]:
            living[role].add(identity)
        if old:
            if not old["dead"] and not state["dead"] and old["tracked"] and state["tracked"]:
                if math.dist(old["position"], state["position"]) > 0.05:
                    moved[role].add(identity)
            if not old["dead"] and (state["body"] < old["body"] - 0.00002 or
                                    state["shield"] < old["shield"] - 0.00002):
                damaged[role].add(identity)
            if not old["dead"] and state["dead"]:
                deaths[role].add(identity)
        previous[role][identity] = state

    for tick, source in sorted(host.items()):
        if source.cinematic:
            continue
        candidates = [other for other in available if abs(other - tick) <= tick_slop]
        if not candidates:
            mismatch((None, "snapshot"), tick, "client has no nearby non-cinematic snapshot")
            continue
        target = available.pop(min(candidates, key=lambda other: (abs(other - tick), other)))
        runs[(None, "snapshot")] = 0
        if source.bsp != target.bsp:
            mismatch((None, "bsp"), tick, f"host {source.bsp}, client {target.bsp}")
            continue
        runs[(None, "bsp")] = 0
        result["stats"]["compared_snapshots"] += 1
        bad = set()
        for identity in source.actors.keys() | target.actors.keys():
            if identity not in source.actors or identity not in target.actors:
                key = (identity, "membership")
                bad.add(key)
                mismatch(key, tick, "missing from host" if identity not in source.actors else "missing from client")
                continue
            left, right = source.actors[identity], target.actors[identity]
            result["stats"]["compared_units"] += 1
            for role, state in (("host", left), ("client", right)):
                observe(role, identity, state)
            distance = math.dist(left["position"], right["position"])
            result["stats"]["position_max"] = max(result["stats"]["position_max"], distance)
            differences = {name: left[name] != right[name] for name in ("definition", "dead", "parent", "seat")}
            differences["position"] = distance > position
            differences.update({name: abs(left[name] - right[name]) > vitality for name in ("body", "shield")})
            # Dead units leave network_actors' control table. The kill replay
            # uses body zero; host damage may retain negative overkill, and
            # distributed_vitality_pack clamps negative values to zero. Keep
            # positive bodies and every live/dead disagreement fully checked.
            if left["dead"] and right["dead"] and left["body"] <= 0 and right["body"] <= 0:
                raw = abs(left["body"] - right["body"])
                if raw:
                    result["stats"]["normalized_corpse_body_samples"] += 1
                    result["stats"]["raw_corpse_body_difference_max"] = max(
                        result["stats"]["raw_corpse_body_difference_max"], raw)
                differences["body"] = False
            for name, differs in differences.items():
                if differs:
                    key = (identity, name)
                    bad.add(key)
                    mismatch(key, tick, f"host {left.get(name)}, client {right.get(name)}")
        for key in list(runs):
            if key[0] is not None and key not in bad:
                runs[key] = 0

    for name, evidence in (("live_units", living), ("moved_units", moved),
                           ("damaged_units", damaged), ("deaths", deaths)):
        result["stats"][name] = {role: len(units) for role, units in evidence.items()}
        result["stats"][name]["common"] = len(evidence["host"] & evidence["client"])
    if result["stats"]["compared_snapshots"] < minimum_snapshots:
        result["problems"].append("insufficient non-cinematic common snapshots")
    if not result["stats"]["live_units"]["common"]:
        result["problems"].append("no common live tracked AI units")
    if not result["stats"]["moved_units"]["common"]:
        result["problems"].append("no common tracked AI movement observed")
    if require_damage and not (result["stats"]["damaged_units"]["common"] or result["stats"]["deaths"]["common"]):
        result["problems"].append("no common damage or death transition observed")
    result["verdict"] = "FAIL" if result["divergences"] else "ERROR" if result["problems"] else "PASS"
    return result


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("host", type=Path)
    parser.add_argument("client", type=Path)
    parser.add_argument("--require-damage", action="store_true", help="also require observed damage/death on both peers")
    parser.add_argument("--json", type=Path)
    args = parser.parse_args()
    try:
        result = compare(parse_log(args.host, "host"), parse_log(args.client, "client"),
                         require_damage=args.require_damage)
    except (ValueError, OSError) as error:
        result = {"schema": "forerunner.native-actors/1", "verdict": "ERROR", "problems": [str(error)]}
    encoded = json.dumps(result, indent=2) + "\n"
    if args.json:
        args.json.write_text(encoded, encoding="utf-8")
    print(encoded, end="")
    return {"PASS": 0, "FAIL": 1, "ERROR": 2}[result["verdict"]]


if __name__ == "__main__":
    sys.exit(main())
