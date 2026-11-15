"""Shared native record codec, reservations, escape recovery and authority."""
import re
import sys
from pathlib import Path

import pytest

sys.path.insert(0, str(Path(__file__).resolve().parents[2]))
from harness import CHECK_FAILED, build, constant, enum_with, function, macro, mutated, read, run, structure


def generated(fault=None):
    without_includes = lambda text: re.sub(r'^#include.*\n', '', text, flags=re.M)
    source = without_includes(read('port/linux/game/gear.c'))
    identity = read('port/linux/src/identity.c')
    start = identity.index('static const unsigned long sha256_k[64]')
    sha = '\n'.join([identity[start:identity.index('};', start) + 2], macro(identity, 'ROTATE'),
                     function(identity, 'sha256_block'), function(identity, 'identity_sha256')])
    types = '\n'.join([
        without_includes(read('port/linux/game/game_modes.h')),
        without_includes(read('port/linux/game/gear.h')),
        enum_with(read('port/linux/src/port_folders.h'), '_port_folder_private,'),
        enum_with(read('source/game/game.h'), '_game_connection_local'),
        enum_with(read('port/linux/src/posix.h'), '_posix_file_is_directory'),
        structure(read('port/linux/src/posix.h'), 'posix_file_information'),
        '#define MAXIMUM_OBJECTS_PER_MAP %d' % constant(read('source/objects/objects.h'), 'MAXIMUM_OBJECTS_PER_MAP'),
    ])
    if fault:
        source = mutated(source, *fault)
    return (('types.inc', types), ('sha.inc', sha), ('under_test.inc', source))


@pytest.mark.parametrize('case', ['authority', 'escape-reopen', 'loss', 'uncertain', 'corrupt', 'cache',
                                'choice-reopen', 'choice-failure', 'choice-cancel', 'choice-recovery', 'choice-cache'])
def test_transactions(case):
    status, output = run(build('gear', generated()), case)
    assert status == 0, output


def test_loss_negative_control():
    status, output = run(build('gear', generated((
        'next = session->record; gear_run_clear(&next, _gear_lost);', 'next = session->record;'))), 'loss')
    assert status == CHECK_FAILED, output


def test_choice_retry_negative_control():
    status, output = run(build('gear', generated((
        'if (pending || !map) return TRUE;', 'if ((pending && FALSE) || !map) return TRUE;'))), 'choice-failure')
    assert status == CHECK_FAILED, output
