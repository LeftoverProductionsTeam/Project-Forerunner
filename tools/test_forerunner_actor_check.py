"""Negative controls for native actor evidence; these are tool tests, not gameplay."""
import copy
import json
import subprocess
import sys
from pathlib import Path

import pytest

import forerunner_actor_check as check


def records(role="host", *, moving=True, damage=False, cinematic=False):
    lines = []
    for index in range(7):
        body = 1.0 if not damage or index < 3 else 0.5
        dead = int(damage and index >= 5)
        lines.append(f"native: network test: actor {index * 30} {role} 80010002 80020003 "
                     f"{index * 0.2 if moving else 0} 2 3 {body} 0 {dead} ffffffff -1 1")
        lines.append(f"native: network test: actors {index * 30} {role} 1 cinematic {int(cinematic)} bsp 0")
    return "\n".join(lines) + "\n"


def logs(tmp_path, host=None, client=None):
    paths = tmp_path / "host.log", tmp_path / "client.log"
    paths[0].write_text(records() if host is None else host, encoding="utf-8")
    paths[1].write_text(records("client") if client is None else client, encoding="utf-8")
    return paths


def compare_paths(paths, **options):
    return check.compare(check.parse_log(paths[0], "host"), check.parse_log(paths[1], "client"), **options)


def test_actual_evidence_levels_are_distinct(tmp_path):
    paths = logs(tmp_path)
    assert compare_paths(paths)["verdict"] == "PASS"
    assert compare_paths(paths, require_damage=True)["verdict"] == "ERROR"
    paths = logs(tmp_path, records(damage=True), records("client", damage=True))
    result = compare_paths(paths, require_damage=True)
    assert result["verdict"] == "PASS"
    assert result["stats"]["deaths"]["common"] == 1


@pytest.mark.parametrize("host,client,reason", [
    ("", "", "insufficient"),
    (records(cinematic=True), records("client", cinematic=True), "non-cinematic"),
    (records(moving=False), records("client", moving=False), "movement"),
    (records(), records("client", moving=False), "movement"),
])
def test_absent_or_stationary_evidence_cannot_pass(tmp_path, host, client, reason):
    result = compare_paths(logs(tmp_path, host, client))
    assert result["verdict"] == "ERROR"
    assert any(reason in problem for problem in result["problems"])


@pytest.mark.parametrize("mutate,field", [
    (lambda state: state.update(position=[20, 2, 3]), "position"),
    (lambda state: state.update(body=0), "body"),
    (lambda state: state.update(dead=1), "dead"),
    (lambda state: state.update(definition=123), "definition"),
    (lambda state: state.update(parent=123, seat=0), "parent"),
])
def test_persistent_real_state_disagreement_fails(tmp_path, mutate, field):
    paths = logs(tmp_path)
    host, client = check.parse_log(paths[0], "host"), check.parse_log(paths[1], "client")
    for snapshot in client.values():
        mutate(next(iter(snapshot.actors.values())))
    result = check.compare(host, client)
    assert result["verdict"] == "FAIL"
    assert any(item["field"] == field for item in result["divergences"])


def test_full_salted_handle_prevents_reused_slot_aliasing(tmp_path):
    paths = logs(tmp_path, client=records("client").replace("80010002", "80090002"))
    result = compare_paths(paths)
    assert result["verdict"] == "FAIL"
    assert result["stats"]["compared_units"] == 0


@pytest.mark.parametrize("bodies", [(-5.166667, 0), (0, -5.166667), (-2, -5)])
def test_nonpositive_overkill_is_equivalent_only_for_confirmed_corpses(tmp_path, bodies):
    paths = logs(tmp_path)
    host, client = check.parse_log(paths[0], "host"), check.parse_log(paths[1], "client")
    for source, body in ((host, bodies[0]), (client, bodies[1])):
        for tick, snapshot in source.items():
            if tick >= 90:
                next(iter(snapshot.actors.values())).update(body=body, dead=1)
    result = check.compare(host, client, require_damage=True)
    assert result["verdict"] == "PASS"
    assert result["stats"]["deaths"]["common"] == 1
    assert result["stats"]["normalized_corpse_body_samples"] == 4
    assert result["stats"]["raw_corpse_body_difference_max"] == abs(bodies[0] - bodies[1])


@pytest.mark.parametrize("left,right,field", [
    ({"body": -5, "dead": 0}, {"body": 0, "dead": 0}, "body"),
    ({"body": -5, "dead": 1}, {"body": 0, "dead": 0}, "dead"),
    ({"body": -5, "dead": 1}, {"body": 0.75, "dead": 1}, "body"),
    ({"body": -5, "dead": 1, "shield": 0.75}, {"body": 0, "dead": 1}, "shield"),
    ({"body": -5, "dead": 1, "parent": 123}, {"body": 0, "dead": 1}, "parent"),
    ({"body": -5, "dead": 1, "position": [20, 2, 3]}, {"body": 0, "dead": 1}, "position"),
])
def test_corpse_rule_preserves_live_body_death_and_other_state_checks(tmp_path, left, right, field):
    paths = logs(tmp_path)
    host, client = check.parse_log(paths[0], "host"), check.parse_log(paths[1], "client")
    for source, state in ((host, left), (client, right)):
        for tick, snapshot in source.items():
            if tick >= 60:
                next(iter(snapshot.actors.values())).update(state)
    result = check.compare(host, client)
    assert result["verdict"] == "FAIL"
    assert any(item["field"] == field for item in result["divergences"])


def test_missing_snapshots_or_bsp_disagreement_cannot_pass(tmp_path):
    paths = logs(tmp_path)
    host, client = check.parse_log(paths[0], "host"), check.parse_log(paths[1], "client")
    assert check.compare(host, {0: client[0]})["verdict"] == "FAIL"
    for snapshot in client.values():
        snapshot.bsp = 1
    assert check.compare(host, client)["verdict"] == "FAIL"


def test_bounded_tick_alignment_and_transient_corrections(tmp_path):
    paths = logs(tmp_path)
    host, client = check.parse_log(paths[0], "host"), check.parse_log(paths[1], "client")
    shifted = {tick + 2: copy.deepcopy(snapshot) for tick, snapshot in client.items()}
    shifted[2].actors[next(iter(shifted[2].actors))]["position"] = [30, 2, 3]
    assert check.compare(host, shifted)["verdict"] == "PASS"
    assert check.compare(host, {tick + 3: snapshot for tick, snapshot in client.items()})["verdict"] == "FAIL"


@pytest.mark.parametrize("corrupt", [
    lambda text: text.rsplit("network test: actors", 1)[0],
    lambda text: text.replace("80010002", "0002"),
    lambda text: text.replace(" 2 3 ", " nan 3 "),
    lambda text: text.replace("1 cinematic", "2 cinematic"),
    lambda text: text + text,
    lambda text: text.replace("host", "client"),
    lambda text: text + "EXCEPTION halt in source/tag_files/tag_groups.c\n",
])
def test_malformed_truncated_or_replayed_evidence_refused(tmp_path, corrupt):
    path = tmp_path / "bad.log"
    path.write_text(corrupt(records()), encoding="utf-8")
    with pytest.raises(ValueError):
        check.parse_log(path, "host")


def test_cli_outputs_machine_readable_error_without_fake_success(tmp_path):
    paths = logs(tmp_path, "no actors\n", "no actors\n")
    output = tmp_path / "result.json"
    process = subprocess.run([sys.executable, str(Path(check.__file__)), *map(str, paths), "--json", str(output)],
                             capture_output=True, text=True)
    assert process.returncode == 2
    assert json.loads(output.read_text())["verdict"] == "ERROR"
