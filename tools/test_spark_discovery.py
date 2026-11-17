"""The compiled engine's read-only Spark discovery, with no game data or display.

Set FORERUNNER_TEST_EXECUTABLE to the exact binary under test. Every case runs
an independent runtime copy; these checks do not claim live editing or gameplay.
"""
import json
import os
from pathlib import Path
import shutil
import subprocess

import pytest


@pytest.fixture
def runtime(tmp_path):
    value = os.environ.get("FORERUNNER_TEST_EXECUTABLE")
    if not value:
        pytest.skip("set FORERUNNER_TEST_EXECUTABLE to a compiled engine")
    original = Path(value).resolve()
    assert original.is_file(), original
    folder = tmp_path / "private runtime with spaces"
    folder.mkdir()
    executable = folder / original.name
    shutil.copy2(original, executable)
    for library in original.parent.glob("*.dll"):
        shutil.copy2(library, folder / library.name)
    return folder, executable


def query(runtime, *arguments):
    folder, executable = runtime
    before = {p.relative_to(folder): None if p.is_dir() else p.read_bytes() for p in folder.rglob("*")}
    environment = {k: v for k, v in os.environ.items() if not k.startswith("HALO_")}
    environment.update(HALO_DATA_ROOT=str(folder / "missing-data"), HALO_SAVE_ROOT=str(folder / "unused-saves"),
                       HALO_NET_ONLINE="false", HALO_UPDATE_AUTO="false", HALO_CRASH_REPORTS="no",
                       HALO_DISCORD_APPLICATION="", SDL_VIDEO_DRIVER="dummy", SDL_AUDIO_DRIVER="dummy")
    result = subprocess.run([str(executable), "--spark-capabilities", *arguments], cwd=folder,
                            env=environment, capture_output=True, text=True, timeout=15,
                            creationflags=subprocess.CREATE_NO_WINDOW if os.name == "nt" else 0)
    after = {p.relative_to(folder): None if p.is_dir() else p.read_bytes() for p in folder.rglob("*")}
    assert before == after, "discovery must not write settings, saves, logs or cache files"
    return result


def test_native_discovery_without_game_data(runtime):
    result = query(runtime, "1")
    assert result.returncode == 0, result.stderr
    reply = json.loads(result.stdout)
    assert reply["schema"] == "spark-forerunner.runtime-capabilities"
    assert reply["contract_version"] == 1 and reply["engine"] == "Forerunner"
    assert reply["simulation_hz"] == 30
    # The accepted authenticated 44/45 bridge advertises 45 and retains layout 44.
    assert reply["network"] == {"advertised_version": 45, "native_layout_version": 44}
    assert reply["operations"] == ["runtime.discover"]
    assert reply["build"]["commit"] not in ("", "unknown")
    assert {"OpenCE", "ChupathingyCE", "FractumSeraph/OpenCE"} <= set(reply["upstream_projects"])


@pytest.mark.parametrize("version", [(), ("2",), ("0",), ("01",), ("invalid",)])
def test_unsupported_discovery_version_is_explicit_and_read_only(runtime, version):
    result = query(runtime, *version)
    assert result.returncode == 3 and not result.stdout
    assert "supported Spark contract version is 1" in result.stderr


def test_discovery_preserves_existing_configuration(runtime):
    folder, _ = runtime
    config = folder / "config.toml"
    config.write_text('# owner configuration\n[display]\nmax_fps = 73\n', encoding="utf-8")
    result = query(runtime, "1", "--config", str(config))
    assert result.returncode == 0, result.stderr
    assert json.loads(result.stdout)["contract_version"] == 1


def test_discovery_does_not_start_requested_server(runtime):
    result = query(runtime, "1", "--server")
    assert result.returncode == 0, result.stderr
    assert json.loads(result.stdout)["operations"] == ["runtime.discover"]


def test_reference_client_negotiates_supported_operation(runtime):
    from spark_runtime import discover

    _, executable = runtime
    assert discover(executable, ["runtime.discover"])["contract_version"] == 1


def test_reference_client_requires_native_squad_features(runtime):
    from spark_runtime import discover

    _, executable = runtime
    reply = discover(executable, required_features=["extraction.squad_objectives.v1", "extraction.squad_notices.v1"])
    assert "extraction.squad_objectives.v1" in reply["native_features"]
    assert "extraction.squad_notices.v1" in reply["native_features"]


def test_reference_client_refuses_unknown_native_feature(runtime):
    from spark_runtime import RuntimeContractError, discover

    _, executable = runtime
    with pytest.raises(RuntimeContractError, match="unsupported required native feature.*extraction.squad_objectives.v2"):
        discover(executable, required_features=["extraction.squad_objectives.v2"])


def test_reference_client_refuses_unimplemented_editing(runtime):
    from spark_runtime import RuntimeContractError, discover

    folder, executable = runtime
    before = {p.relative_to(folder): p.read_bytes() for p in folder.iterdir()}
    with pytest.raises(RuntimeContractError, match="unsupported required operation.*world.object.create"):
        discover(executable, ["world.object.create"])
    assert before == {p.relative_to(folder): p.read_bytes() for p in folder.iterdir()}
