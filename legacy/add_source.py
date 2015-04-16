#!/usr/bin/env python
#
#    Script to add source files to Synthese.
#    @file add_source.py
#    @author Sylvain Pasche
#
#    This file belongs to the SYNTHESE project (public transportation specialized software)
#    Copyright (C) 2002 Hugues Romain - RCSmobility <contact@rcsmobility.com>
#
#    This program is free software; you can redistribute it and/or
#    modify it under the terms of the GNU General Public License
#    as published by the Free Software Foundation; either version 2
#    of the License, or (at your option) any later version.
#
#    This program is distributed in the hope that it will be useful,
#    but WITHOUT ANY WARRANTY; without even the implied warranty of
#    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#    GNU General Public License for more details.
#
#    You should have received a copy of the GNU General Public License
#    along with this program; if not, write to the Free Software
#    Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.

"""
Integration with Visual Studio:

Create a new external tool:

Tools > External Tools...

Title: Synthese add source
Command (change to your Python path):
C:\Python\python.exe
Arguments (change to your Synthese path):
C:\path\to\synthese3\utils\add_source.py

Check 'Prompt for argument'

Then you can activate the command and add the following two arguments:

MODULE_NAME SOURCE_FILENAME

For instance:
35 LineStopGetService

It will add the source in the matching module, update the CMakeLists.txt file
and rerun cmake to update the project.

"""

import logging
import optparse
import os
from os.path import join
import subprocess
import sys

log = logging.getLogger(__name__)

SOURCE_TEMPLATE = '\n// fill me\n\n'

def update_cmakelists_txt(path, sources_to_add):
    cmakelists = join(path, 'CmakeLists.txt')
    assert os.path.isfile(cmakelists)

    lines = open(cmakelists).read().splitlines()
    print len(lines)
    for index, item in enumerate(lines):
        if item.startswith('set(') and '_SRCS' in item:
            start = index
            break
    end = lines.index(')')
    sources = lines[start + 1:end]
    new_sources = sorted(set(sources + sources_to_add), key=str.lower)
    lines[start + 1:end] = new_sources

    content = '\n'.join(lines)
    if not content.endswith('\n'):
        content += '\n'
    open(cmakelists, 'wb').write(content)


def add_source(module_name, source_filename, options):
    log.debug('add_source %s %s %s', module_name, source_filename, options)

    thisdir = os.path.abspath(os.path.dirname(__file__))
    src_dir = join(thisdir, os.pardir, 'src')
    print src_dir
    modules = os.listdir(src_dir)
    matching_modules = [m for m in modules if module_name in m]
    if len(matching_modules) != 1:
        log.fatal(
            'Didn\'t find only one matching module for %r, found: %r',
            module_name, matching_modules)
        sys.exit(1)
    module_dir = join(src_dir, matching_modules[0])

    source_files = [source_filename + ext for ext in ('.cpp', '.hpp')]

    # 1) Create source files
    for source_file in source_files:
        source = join(module_dir, source_file)
        if not options.overwrite and os.path.isfile(source):
            log.fatal('Source already exists: %r', source)
            sys.exit(1)
        open(source, 'wb').write(SOURCE_TEMPLATE)
        log.info('Created source file: %r', source)

    # 2) Add them to CMake

    update_cmakelists_txt(module_dir, source_files)

    # 3) Run cmake to update the project.
    # TODO: Make it possible to pass additional parameters (build type, ...)

    synthesepy = join(thisdir, os.pardir, 'tools', 'synthese.py')
    subprocess.check_call([
        sys.executable, synthesepy, 'build', '-g'])


if __name__ == '__main__':
    usage = 'usage: %prog [options] MODULE_NAME SOURCE_FILENAME'
    parser = optparse.OptionParser(usage=usage)

    parser.add_option('-v', '--verbose', action='store_true',
         default=False, help='Print debug logging')
    parser.add_option('-o', '--overwrite', action='store_true',
         default=False, help='Overwrite existing files')

    (options, args) = parser.parse_args()
    if len(args) != 2:
        parser.print_help()
        sys.exit(1)

    logging.basicConfig(level=(logging.DEBUG if options.verbose else
                               logging.INFO))

    module_name, source_filename = args

    add_source(module_name, source_filename, options)
