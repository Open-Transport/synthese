#    Classes for modelling a Synthese environment (CMake, Scons, ...).
#    @file env.py
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


import hashlib
import logging
import os
import sys
import urllib2

from synthesepy import build
from synthesepy import utils

log = logging.getLogger(__name__)


class Env(object):
    def __init__(self, env_path, mode, config):
        self.mode = mode
        self.config = config
        # shortcut
        self.c = self.config

        self.source_path = os.path.normpath(
            os.path.join(
                os.path.dirname(os.path.abspath(__file__)),
                os.pardir, os.pardir))
        if sys.platform in ('win32', 'cygwin'):
            self.platform = 'win'
        elif sys.platform == 'linux2':
            self.platform = 'lin'
        else:
            self.platform = 'mac'

        self.platform_exe_suffix = '.exe' if (self.platform == 'win') else ''

        self.env_path = env_path if env_path else self.default_env_path

    @property
    def default_env_path(self):
        raise Exception(
            'Can\'t guess default env path. It should be specified on the '
            'command line (-b) or environment (SYNTHESE_ENV_PATH).')

    def get_executable_path(self, env_relative_path, executable_file):
        return os.path.join(
            self.env_path, env_relative_path, self.executable_relative_path,
            executable_file + self.platform_exe_suffix)

    @property
    def executable_relative_path(self):
        return os.curdir

    @property
    def daemon_launch_path(self):
        return os.path.join(self.env_path, self.daemon_relative_path)

    @property
    def daemon_relative_path(self):
        return os.path.join('src', 'bin', 'server')

    @property
    def daemon_path(self):
        return self.get_executable_path(self.daemon_relative_path, 's3-server')

    @property
    def testdata_importer_relative_path(self):
        return os.path.join('test', '53_pt_routeplanner')

    @property
    def testdata_importer_path(self):
        return self.get_executable_path(
            self.testdata_importer_relative_path, 'ImportRoutePlannerTestData')

    def _prepare_for_launch_win(self):
        builder = build.get_builder(self)
        builder.install_iconv()

        iconv_path = os.path.join(self.c.thirdparty_dir, 'iconv')
        utils.append_paths_to_environment('PATH', [iconv_path])

    def _prepare_for_launch_lin(self):
        pass

    def prepare_for_launch(self):
        """
        Setup environment or other things needed for launching one of the
        generated executables.
        """

        os.environ['SYNTHESE_LOG_LEVEL'] = str(self.config.log_level)

        if self.platform == 'win':
            self._prepare_for_launch_win()
        elif self.platform == 'lin':
            self._prepare_for_launch_lin()


class SconsEnv(Env):
    type = 'scons'

    @property
    def default_env_path(self):
        return os.path.join(self.source_path, 'build', self.mode)

    def _prepare_for_launch_lin(self):
        utils.append_paths_to_environment(
            'LD_LIBRARY_PATH', [os.path.join(self.env_path, 'repo', 'bin')])
        super(SconsEnv, self)._prepare_for_launch_lin()


class CMakeEnv(Env):
    type = 'cmake'

    @property
    def default_env_path(self):
        return os.path.join(self.source_path, 'build_cmake', self.mode)

    @property
    def executable_relative_path(self):
        if self.platform == 'win':
            return os.path.join('Debug' if (self.mode == 'debug') else 'Release')
        return os.curdir


class InstalledEnv(Env):
    type = 'installed'

    @property
    def daemon_relative_path(self):
        return 'bin'

    @property
    def testdata_importer_relative_path(self):
        return 'bin'


def create_env(env_type, env_path, mode, config):
    # TODO: maybe drop these env variables, and use only cli flags from synthese.py
    if not env_type:
        env_type = os.environ.get('SYNTHESE_ENV_TYPE')
    if not env_type:
        raise Exception('Env type must be specified on the command line or in environment')
    if not mode:
        mode = os.environ.get('SYNTHESE_MODE')
    if not env_path:
        env_path = os.environ.get('SYNTHESE_ENV_PATH')

    if env_type == 'scons':
        env_class = SconsEnv
    elif env_type == 'cmake':
        env_class = CMakeEnv
    elif env_type == 'installed':
        env_class = InstalledEnv
    else:
        raise Exception('Unknown env type: %s' % env_type)

    return env_class(env_path, mode, config)
