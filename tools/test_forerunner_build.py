"""Source-build isolation and identity, using disposable real Git repositories."""
import json
import subprocess

import pytest

import forerunner_build as build


def git(root, *arguments):
    return subprocess.run(["git", "-C", str(root), *arguments], check=True,
                          capture_output=True, text=True).stdout.strip()


@pytest.fixture
def source(tmp_path):
    root = tmp_path / "source with spaces"
    root.mkdir()
    git(root, "init", "-q")
    (root / "configure.py").write_text("# source fixture\n")
    (root / "engine.c").write_text("int original;\n")
    git(root, "add", "configure.py", "engine.c")
    git(root, "-c", "user.name=Build test", "-c", "user.email=build-test@example.invalid",
        "commit", "-qm", "Create source fixture")
    return root


def test_export_identifies_dirty_tracked_source_without_copying_untracked_data(source, tmp_path):
    (source / "engine.c").write_text("int edited;\n")
    (source / "private.iso").write_bytes(b"private data must stay here")
    (source / "build").mkdir()
    (source / "build/halo.exe").write_bytes(b"untracked build")
    original = (source / "engine.c").read_bytes()
    exported, manifest = build.export_source(source, tmp_path / "build with spaces")
    assert (exported / "engine.c").read_bytes() == original
    assert not (exported / ".git").exists()
    assert not (exported / "build").exists()
    assert not (exported / "private.iso").exists()
    assert manifest["commit"] == git(source, "rev-parse", "HEAD")
    assert manifest["dirty_tracked_source"]
    assert {entry["path"] for entry in manifest["files"]} == {"configure.py", "engine.c"}
    assert json.loads((exported.parent / build.MANIFEST_NAME).read_text()) == manifest
    # The compiler's disposable source must never be a writable hardlink.
    (exported / "engine.c").write_bytes(b"compiler changed its copy")
    assert (source / "engine.c").read_bytes() == original


def test_snapshot_identity_changes_with_bytes_and_honors_tracked_deletion(source, tmp_path):
    _, first = build.export_source(source, tmp_path / "first")
    assert not first["dirty_tracked_source"]
    (source / "engine.c").unlink()
    exported, second = build.export_source(source, tmp_path / "second")
    assert not (exported / "engine.c").exists()
    assert second["dirty_tracked_source"]
    assert second["snapshot_sha256"] != first["snapshot_sha256"]
    assert second["tracked_status_sha256"] != first["tracked_status_sha256"]


@pytest.mark.parametrize("destination", ["same", "inside", "ancestor"])
def test_refuses_work_in_or_above_source_without_writing(source, destination):
    work = {"same": source, "inside": source / "generated", "ancestor": source.parent}[destination]
    before = git(source, "status", "--porcelain")
    with pytest.raises(ValueError, match="separate from"):
        build.export_source(source, work)
    assert git(source, "status", "--porcelain") == before
    assert not (source / "generated").exists()


def test_refuses_existing_work_and_preserves_its_contents(source, tmp_path):
    work = tmp_path / "existing"
    work.mkdir()
    marker = work / "previous-good.bin"
    marker.write_bytes(b"keep exactly")
    with pytest.raises(ValueError, match="already exists"):
        build.export_source(source, work)
    assert marker.read_bytes() == b"keep exactly"
    assert sorted(path.name for path in work.iterdir()) == [marker.name]


@pytest.mark.parametrize("name", ["owned.map", "owned.iso", "owned.xiso", "halo.exe", "SDL3.dll", "engine.obj"])
def test_refuses_tracked_game_data_and_compiled_binaries(source, tmp_path, name):
    original = source / name
    original.write_bytes(b"protected fixture")
    git(source, "add", name)
    with pytest.raises(ValueError, match="excluded from source builds"):
        build.export_source(source, tmp_path / "refused")
    assert original.read_bytes() == b"protected fixture"
    assert not (tmp_path / "refused").exists()


def test_refuses_nested_git_source_repository(source, tmp_path):
    nested = source / "nested"
    nested.mkdir()
    git(nested, "init", "-q")
    (nested / "unique.c").write_text("int nested;\n")
    git(nested, "add", "unique.c")
    git(nested, "-c", "user.name=Build test", "-c", "user.email=build-test@example.invalid",
        "commit", "-qm", "Create nested fixture")
    git(source, "add", "nested")
    with pytest.raises(ValueError, match="mode 160000"):
        build.export_source(source, tmp_path / "refused")
    assert not (tmp_path / "refused").exists()


def test_refuses_linked_tracked_file_without_dereferencing(source, tmp_path):
    original = source / "engine.c"
    target = tmp_path / "outside.c"
    target.write_text("int protected;\n")
    original.unlink()
    try:
        original.symlink_to(target)
    except OSError:
        pytest.skip("creating file symlinks requires this host's developer mode or symlink permission")
    with pytest.raises(ValueError, match="links outside"):
        build.export_source(source, tmp_path / "refused")
    assert target.read_text() == "int protected;\n"
    assert not (tmp_path / "refused").exists()


def test_refuses_source_changed_during_export(source, tmp_path, monkeypatch):
    original_checksum = build.file_checksum

    def edited_checksum(path):
        if path.name == "engine.c":
            path.write_text("int concurrent_edit;\n")
        return original_checksum(path)

    monkeypatch.setattr(build, "file_checksum", edited_checksum)
    with pytest.raises(ValueError, match="changed during export"):
        build.export_source(source, tmp_path / "interrupted")
    assert (source / "engine.c").read_text() == "int concurrent_edit;\n"
    assert not (tmp_path / "interrupted" / build.MANIFEST_NAME).exists()


def test_explicit_msvc_sdk_paths_are_validated_and_x86_only(tmp_path, monkeypatch):
    monkeypatch.delenv("WindowsSDKVersion", raising=False)
    vc, sdk, version = tmp_path / "MSVC", tmp_path / "SDK", "10.0.26100.0"
    includes = [vc / "include", *(sdk / "Include" / version / name for name in ("ucrt", "um", "shared", "winrt"))]
    libs = [vc / "lib/x86", *(sdk / "Lib" / version / name / "x86" for name in ("ucrt", "um"))]
    for folder in includes + libs:
        folder.mkdir(parents=True)
    environment, metadata = build.msvc_environment(vc, sdk)
    assert environment["INCLUDE"].split(";") == list(map(str, includes))
    assert environment["LIB"].split(";") == list(map(str, libs))
    assert metadata["sdk_version"] == version
    (vc / "lib/x86").rmdir()
    with pytest.raises(ValueError, match="required x86"):
        build.msvc_environment(vc, sdk)
