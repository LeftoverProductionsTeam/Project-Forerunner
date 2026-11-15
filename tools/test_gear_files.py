"""The native gear file backend on disposable files and owned child processes."""
import os
import shutil
import subprocess
import sys
from pathlib import Path

import pytest

ROOT = Path(__file__).resolve().parent.parent


@pytest.fixture(scope='module')
def driver(tmp_path_factory):
    directory = tmp_path_factory.mktemp('gear-native')
    executable = directory / ('gear-files.exe' if os.name == 'nt' else 'gear-files')
    compiler = os.environ.get('CC') or shutil.which('clang') or shutil.which('cc')
    if not compiler:
        pytest.skip('native C compiler unavailable')
    backend = 'port/windows/src/win32_files.c' if os.name == 'nt' else 'port/linux/src/posix_files.c'
    subprocess.run([compiler, '-std=gnu99', '-D_CRT_SECURE_NO_WARNINGS',
                    '-I', str(ROOT / 'port/linux/src'), str(ROOT / 'port/linux/tests/gear_file_test.c'),
                    str(ROOT / backend), '-o', str(executable)], check=True, capture_output=True, text=True)
    return executable


@pytest.fixture(scope='module')
def transactions(tmp_path_factory):
    sys.path.insert(0, str(ROOT / 'tools'))
    from harness.tests.test_gear import generated
    directory = tmp_path_factory.mktemp('gear-transactions-native')
    executable = directory / ('transactions.exe' if os.name == 'nt' else 'transactions')
    compiler = os.environ.get('CC') or shutil.which('clang') or shutil.which('cc')
    if not compiler:
        pytest.skip('native C compiler unavailable')
    for name, text in generated():
        (directory / name).write_text(text)
    backend = 'port/windows/src/win32_files.c' if os.name == 'nt' else 'port/linux/src/posix_files.c'
    subprocess.run([compiler, '-m32', '-std=gnu99', '-O2', '-D_CRT_SECURE_NO_WARNINGS', '-DGEAR_NATIVE_FILES',
                    '-I', str(ROOT / 'tools/harness/include'), '-I', str(ROOT / 'source/cseries'),
                    '-I', str(ROOT / 'port/linux/src'), '-I', str(directory),
                    str(ROOT / 'tools/harness/tests/gear.c'), str(ROOT / backend), '-o', str(executable)],
                   check=True, capture_output=True, text=True)
    return executable


@pytest.mark.parametrize('producer,recovery,phase', [('native-prepared', 'native-refund', 3),
                                                   ('native-issued', 'native-loss', 1)])
def test_actual_record_process_death(transactions, tmp_path, producer, recovery, phase):
    holder = subprocess.Popen([str(transactions), producer, str(tmp_path)], stdin=subprocess.PIPE,
                              stdout=subprocess.PIPE, stderr=subprocess.PIPE)
    try:
        assert holder.stdout.readline().strip() == b'ready'
        target, = (tmp_path / 'gear').glob('*.gear')
        prepared = target.read_bytes()
        assert int.from_bytes(prepared[4:6], 'little') == 1
        assert int.from_bytes(prepared[6:8], 'little') == phase
        assert call(transactions, recovery, tmp_path).returncode != 0
        assert target.read_bytes() == prepared
        holder.kill()
        holder.communicate(timeout=10)
        assert call(transactions, recovery, tmp_path).returncode == 0
        assert call(transactions, recovery, tmp_path).returncode == 0
    finally:
        if holder.poll() is None:
            holder.kill()
        holder.communicate(timeout=10)


def call(driver, operation, path, *arguments):
    return subprocess.run([str(driver), operation, str(path), *map(str, arguments)],
                          capture_output=True, timeout=10)


def write(driver, target, data):
    source = target.parent / 'input.bin'
    source.write_bytes(data)
    return call(driver, 'write', target, source)


def test_binary_roundtrip_and_replacement(driver, tmp_path):
    target = tmp_path / 'owner.gear'
    assert call(driver, 'read', target).returncode == 3
    assert write(driver, target, b'first\0record').returncode == 0
    assert call(driver, 'read', target).stdout == b'first\0record'
    assert write(driver, target, b'next').returncode == 0
    assert call(driver, 'read', target).stdout == b'next'
    assert call(driver, 'read-small', target).returncode == 4


def test_exclusion_and_process_death(driver, tmp_path):
    target = tmp_path / 'owner.gear'
    assert write(driver, target, b'committed').returncode == 0
    holder = subprocess.Popen([str(driver), 'hold', str(target)], stdin=subprocess.PIPE,
                              stdout=subprocess.PIPE, stderr=subprocess.PIPE)
    try:
        assert holder.stdout.readline() == (b'locked\r\n' if os.name == 'nt' else b'locked\n')
        assert call(driver, 'try', target).returncode == 2
        assert write(driver, target, b'wrong-owner').returncode == 2
        assert target.read_bytes() == b'committed'
        holder.kill()
        holder.communicate(timeout=10)
        assert write(driver, target, b'after-crash').returncode == 0
        assert call(driver, 'read', target).stdout == b'after-crash'
    finally:
        if holder.poll() is None:
            holder.kill()
        holder.communicate(timeout=10)


def test_partial_staging_never_replaces_committed_record(driver, tmp_path):
    target = tmp_path / 'owner.gear'
    assert write(driver, target, b'committed').returncode == 0
    target.with_suffix('.gear.new').write_bytes(b'partial')
    assert call(driver, 'read', target).stdout == b'committed'
    assert write(driver, target, b'complete').returncode == 0
    assert call(driver, 'read', target).stdout == b'complete'


def test_hard_link_staging_cannot_truncate_another_file(driver, tmp_path):
    target = tmp_path / 'owner.gear'
    other = tmp_path / 'protected.bin'
    other.write_bytes(b'preserve')
    os.link(other, target.with_suffix('.gear.new'))
    assert write(driver, target, b'overwrite').returncode == 4
    assert other.read_bytes() == b'preserve'
