#    Calls Doxygen to generate the documentation.
#    @file doxygen.py
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

from fnmatch import fnmatch
import glob
import logging
import os
from os.path import join
import shlex
import shutil

import synthesepy.build
from synthesepy import utils


log = logging.getLogger(__name__)


def parse_doxyfile(file_contents):
    """
    Parse a Doxygen source file and return a dictionary with all the values.
    Values will be strings and lists of strings.
    """
    data = {}

    lex = shlex.shlex(instream = file_contents, posix = True)
    lex.wordchars += '*+./-:'
    lex.whitespace = lex.whitespace.replace('\n', '')
    lex.escape = ''

    lineno = lex.lineno
    token = lex.get_token()
    key = token   # the first token should be a key
    last_token = ''
    key_token = False
    next_key = False
    new_data = True

    def append_data(data, key, new_data, token):
        if new_data or len(data[key]) == 0:
            data[key].append(token)
        else:
            data[key][-1] += token

    while token:
        if token in ['\n']:
            if last_token not in ['\\']:
                key_token = True
        elif token in ['\\']:
             pass
        elif key_token:
             key = token
             key_token = False
        else:
           if token == '+=':
               if not data.has_key(key):
                   data[key] = list()
           elif token == '=':
               if key == 'TAGFILES' and data.has_key(key):
                   append_data( data, key, False, '=')
                   new_data=False
               else:
                   data[key] = list()
           else:
               append_data(data, key, new_data, token)
               new_data = True

        last_token = token
        token = lex.get_token()

        if last_token == '\\' and token != '\n':
            new_data = False
            append_data( ata, key, new_data, '\\')

    # compress lists of len 1 into single strings
    for (k, v) in data.items():
        if len(v) == 0:
            data.pop(k)

        # items in the following list will be kept as lists and not converted to strings
        if k in ['INPUT', 'FILE_PATTERNS', 'EXCLUDE_PATTERNS', 'TAGFILES', 'PREDEFINED']:
            continue
        if len(v) == 1:
            data[k] = v[0]

    return data


def _generate_doxyfile(env, doxyfile, target_dir):
    parsed_doxyfile = parse_doxyfile(open(doxyfile).read())

    output_formats = ['HTML', 'LATEX', 'RTF', 'MAN', 'XML']

    # TODO: only html for now. Maybe support other outputs.
    for output_format in output_formats:
        parsed_doxyfile['GENERATE_' + output_format] = 'NO'
    parsed_doxyfile['GENERATE_HTML'] = 'YES'

    doxyfile_generated = join(target_dir, 'Doxyfile.doxyfile.gen')

    # Configuration update

    parsed_doxyfile['OUTPUT_DIRECTORY'] = target_dir

    if 'EXCLUDE_PATTERNS' not in parsed_doxyfile:
        parsed_doxyfile['EXCLUDE_PATTERNS'] = '*/.svn/*  */*.gen.cpp */*.inc.cpp'

    with open(doxyfile_generated, 'wb') as output:
        for key, value in parsed_doxyfile.iteritems():
            if isinstance(value, list):
                value = ' '.join('"%s"' % v for v in value)

            output.write('{0} = {1}\n'.format(key, value))
        output.write('\n')
        output.write("ALIASES += htmlinclink{1}=\"@htmlonly <a class=\"el\" "
            "href=\\\"includes/\\1\\\">\\1</a>@endhtmlonly\"\n")
        output.write("ALIASES += htmlinclink{2}=\"@htmlonly <a class=\"el\" "
            "href=\\\"includes/\\1\\\">\\2</a>@endhtmlonly\"\n")
    log.debug('Generated Doxygen configuration in %r', doxyfile_generated)

    return doxyfile_generated


def run(env, args):
    builder = synthesepy.build.get_builder(env)
    builder.check_debian_package_requirements(['doxygen', 'graphviz'])

    target_dir = join(env.env_path, 'doc')
    # TODO: should we keep it to save time?
    utils.RemoveDirectory(target_dir)
    os.makedirs(target_dir)

    doxyfile = join(env.source_path, 'doc', 'Doxyfile.doxyfile')
    doxyfile_generated = _generate_doxyfile(env, doxyfile, target_dir)

    # Copy include
    source_include = join(env.source_path, 'doc', 'include')
    target_include = join(target_dir, 'html', 'include')
    utils.RemoveDirectory(target_include)
    assert not os.path.isdir(target_include)

    log.debug('Copying include: %r -> %r', source_include, target_include)
    shutil.copytree(source_include, target_include)

    utils.call(
        ['doxygen', doxyfile_generated],
        cwd=os.path.dirname(doxyfile))
