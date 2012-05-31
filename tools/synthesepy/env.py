#    Classes for modelling a Synthese environment.
#    @file env.py
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


import hashlib
import logging
import os
from os.path import join
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
        # Remove env settings only used for initialization
        try:
            del self.c.env_type
            del self.c.env_path
            del self.c.mode
        except AttributeError:
            pass

        self.source_path = os.path.normpath(
            join(
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
        self.config.env_path = self.env_path

    @property
    def default_env_path(self):
        raise Exception(
            'Can\'t guess default env path. It should be specified on the '
            'command line (-b) or environment (SYNTHESE_ENV_PATH).')

    def get_executable_path(self, env_relative_path, executable_file):
        return join(
            self.env_path, env_relative_path, self.executable_relative_path,
            executable_file + self.platform_exe_suffix)

    @property
    def executable_relative_path(self):
        return os.curdir

    @property
    def daemon_launch_path(self):
        return join(self.env_path, self.daemon_relative_path)

    @property
    def daemon_relative_path(self):
        return join('src', 'bin', 'server')

    @property
    def daemon_path(self):
        return self.get_executable_path(self.daemon_relative_path, 's3-server')

    @property
    def s3_proxy_path(self):
        return self.get_executable_path(self.daemon_relative_path, 's3-proxy')

    @property
    def testdata_importer_relative_path(self):
        return join('test', '53_pt_routeplanner')

    @property
    def pyenv_path(self):
        return join(self.c.thirdparty_dir, 'env')

    @property
    def pyenv_bin_path(self):
        if self.platform == 'win':
            return join(self.pyenv_path, 'Scripts')

        return join(self.pyenv_path, 'bin')

    @property
    def tools_path(self):
        return join(self.source_path, 'tools')

    @property
    def synthesepy_path(self):
        return join(self.tools_path, 'synthese.py')

    @property
    def testdata_importer_path(self):
        return self.get_executable_path(
            self.testdata_importer_relative_path, 'ImportRoutePlannerTestData')

    def _prepare_for_launch_win(self):
        # FIXME: this won't work if installed
        iconv_path = join(self.source_path, '3rd', 'dev', 'iconv', 'bin')

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


class CMakeEnv(Env):
    type = 'cmake'
    MODE_TO_CMAKE_BUILD_TYPE = {
        'debug': 'Debug',
        'release': 'Release',
        'relwithdebinfo': 'RelWithDebInfo',
    }

    @property
    def default_env_path(self):
        return join(self.source_path, 'build_cmake', self.mode)

    @property
    def build_type(self):
        return self.MODE_TO_CMAKE_BUILD_TYPE[self.mode]

    @property
    def executable_relative_path(self):
        if self.platform == 'win':
            return self.build_type
        return os.curdir


class InstalledEnv(Env):
    type = 'installed'

    @property
    def daemon_relative_path(self):
        return 'bin'

    @property
    def testdata_importer_relative_path(self):
        return 'bin'

    @property
    def pyenv_path(self):
        return join(self.env_path, 'share', 'synthese', 'env')

    @property
    def tools_path(self):
        return join(self.env_path, 'share', 'synthese', 'tools')

    @property
    def synthesepy_path(self):
        return join(self.env_path, 'bin', 'synthese.py')


def create_env(env_type, env_path, mode, config):
    if env_type == 'cmake':
        env_class = CMakeEnv
    elif env_type == 'installed':
        env_class = InstalledEnv
    else:
        raise Exception('Unknown env type: %s' % env_type)

    return env_class(env_path, mode, config)
