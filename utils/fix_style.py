"""
Script to fix coding style:
 - Unix line ending
 - File ends with a newline
 - No trailing whitespace
 - Indentation using tabs only (to be done)
"""

__author__ = 'Sylvain Pasche <sylvain.pasche@gmail.com>'

import logging
import os

from update_utils import files_generator

log = logging.getLogger(__name__)

logging.basicConfig(level=logging.DEBUG)

# Ignore third party files.
IGNORED_FILES = [
    'src/01_util/XmlParser.h',
    'src/01_util/XmlParser.cpp',
    'src/09_agi_client/cagi.c',
    'src/09_agi_client/cagi.h',
]


def fix_carriage_return(lines):
    for index, line in enumerate(lines):
        if line.endswith('\r'):
            lines[index] = lines[index][:-1]


def fix_end_of_line(lines):
    for index, line in enumerate(lines):
        lines[index] = lines[index].rstrip() + '\n'


def fix_tabs(lines):
    # TODO
    return lines


def fix_newline_at_eof(lines):
    if len(lines) == 0:
        return
    try:
        if lines[-1][-1] == '\n':
            return
    except IndexError:
        pass
    lines[-1] = lines[-1] + '\n'


def fix_style_from_lines(lines):
    fix_carriage_return(lines)
    fix_end_of_line(lines)
    fix_tabs(lines)
    fix_newline_at_eof(lines)
    return lines


def fix_style_from_path(path):
    if path.endswith('.new.cpp'):
        return

    path = path.replace(os.sep, '/')
    log.info('Processing %s' % path)

    for p in IGNORED_FILES:
        if p in path:
            log.info('Path %s is ignored, skipping' % path)
            return

    lines = open(path, 'rb').readlines()
    lines = fix_style_from_lines(lines)

    if 1:
        SUFFIX = ''
        if 0:
            SUFFIX = '.new.cpp'
        open(path + SUFFIX, 'wb').write(''.join(lines))


def main():
    DIRS = ['src', 'test', 'tools', 'utils']
    for d in DIRS:
        for cpp_file in files_generator(d, ['py', 'cpp', 'h', 'hpp']):
            fix_style_from_path(cpp_file)


if __name__ == '__main__':
    main()
