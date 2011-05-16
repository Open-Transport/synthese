#    Scripts to build Synthese
#    @file build.py
#    @author Sylvain Pasche
#
#    This file belongs to the SYNTHESE project (public transportation specialized software)
#    Copyright (C) 2002 Hugues Romain - RCS <contact@reseaux-conseil.com>
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


import logging
import multiprocessing
import os
import subprocess

from synthesepy import utils

log = logging.getLogger(__name__)


class Builder(object):
    def __init__(self, env, args):
        self.env = env
        self.args = args

    def build(self):
        raise NotImplemented()

class SconsBuilder(Builder):
    # This should be populated automatically once all tests build.
    TEST_TARGETS = [
        # FIXME: commented targets don't compile.
        #'util',
        'db',
        'html',
        #'lex-matcher',
        #'geography',
        'graph',
        'road',
        'pt',
        #'carto',
        'pt-routeplanner',
    ]

    def build(self):
        if not utils.find_executable('scons'):
            raise Exception('Unable to find scons in PATH')

        args = ['scons']

        # Use ccache on Linux if available
        if self.env.platform == 'lin' and utils.find_executable('ccache'):
            args.append("CXX=ccache g++")
            args.append("CC=ccache gcc")

        if self.args.with_mysql or self.args.mysql_dir:
            args.append('_WITH_MYSQL=True')
            if self.args.mysql_dir:
                args.append('_MYSQL_ROOT=' + self.args.mysql_dir)

        if self.env.mode == 'debug':
            args.append('_CPPMODE=debug')


        # Don't run tests after they are built.
        args.append('TESTBUILDONLY=True')

        targets = (['s3-server.cppbin'] +
            ['s3-%s-test.cpptest' % t for t in self.TEST_TARGETS])

        args.extend(targets)

        log.info('Build command line: %s', args)

        kwargs = {}
        # Windows doesn't like it if launched without shell=True and
        # Linux fails if it is.
        if self.env.platform == 'win':
            kwargs = {
                'shell': True
            }
        subprocess.check_call(args, **kwargs)


class CMakeBuilder(Builder):

    def _generate_build_system(self):
        cmake_executable = 'cmake' + self.env.platform_exe_suffix
        if not utils.find_executable(cmake_executable):
            raise Exception('Unable to find %s in PATH' % cmake_executable)

        args = [cmake_executable, self.env.source_path]

        # Use ccache on Linux if available
        if self.env.platform == 'lin' and utils.find_executable('ccache'):
            os.environ['CXX'] = 'ccache g++'
            os.environ['CC'] = 'ccache gcc'

        if self.env.platform == 'win':
            # TODO: This shouldn't be hardcoded.
            args.extend(['-G', 'Visual Studio 9 2008'])

        if self.args.with_mysql or self.args.mysql_dir:
            args.append('-DWITH_MYSQL=1')
            if self.args.mysql_dir:
                os.environ['MYSQL_DIR'] = self.args.mysql_dir

        args.append('-DCMAKE_BUILD_TYPE=' + self.env.mode.capitalize())
        # TODO: maybe change optimization flags in debug mode:
        # -DCMAKE_CXX_FLAGS=-O0

        # TODO:
        # -DSYNTHESE_MYSQL_PARAMS=host=localhost,user=synthese,passwd=synthese
        # TODO:
        # -DCMAKE_INSTALL_PREFIX=/path/to/installs/trunk_debug

        log.info('CMake generate command line: %s', args)
        if not os.path.isdir(self.env.env_path):
            os.makedirs(self.env.env_path)
        subprocess.check_call(args, cwd=self.env.env_path)

    def _do_build_make(self):
        subprocess.check_call(
            'make -j%i' % multiprocessing.cpu_count(),
            cwd=self.env.env_path,
            shell=True
        )

    def _do_build_vs(self):
        # TODO: these should be extracted from system config
        default_vs_path = 'C:\\Program Files (x86)\\Microsoft Visual Studio 9.0\\'
        default_sdk_path = 'C:\\Program Files\\Microsoft SDKs\\Windows\\v6.0A'

        utils.append_paths_to_environment('PATH', [
            default_vs_path + 'Common7\\IDE',
            default_vs_path + 'Common7\\Tools',
            default_vs_path + 'VC\\BIN',
            default_vs_path + 'VC\\VCPackages',
            default_sdk_path + 'bin',
        ])

        utils.append_paths_to_environment('INCLUDE', [
            default_vs_path + 'VC\\ATLMFC\\INCLUDE',
            default_vs_path + 'VC\\INCLUDE',
            default_sdk_path + 'include',
        ])

        utils.append_paths_to_environment('LIB', [
            default_vs_path + 'VC\\ATLMFC\\LIB',
            default_vs_path + 'VC\\LIB',
            default_sdk_path + 'lib',
        ])

        build_type = self.env.mode.capitalize()
        args = [
            'devenv.com', 'synthese3.sln', '/build', build_type,
            '/project', 'ALL_BUILD'
        ]
        log.info('Build command line: %s', args)
        subprocess.check_call(
            args,
            cwd=self.env.env_path
        )

    def build(self):
        self._generate_build_system()

        PLATFORM_TO_TOOL = {
            'win': 'vs',
            'lin': 'make',
        }

        build_fun = getattr(
            self, '_do_build_' + PLATFORM_TO_TOOL[self.env.platform], None
        )
        if not build_fun:
            raise Exception('Unsupported platform')

        build_fun()

        # TODO: run install?


def build(env, args):
    if env.type == 'cmake':
        builder_class = CMakeBuilder
    elif env.type == 'scons':
        builder_class = SconsBuilder
    else:
        raise Exception('Unsupported env %s' % type(env).__name__)

    builder = builder_class(env, args)
    builder.build()
