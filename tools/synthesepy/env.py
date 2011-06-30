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
    port = None
    wsgi_proxy = True
    wsgi_proxy_port = None
    site_id = 0
    conn_string = None
    verbose = False
    log_stdout = False
    static_dir = None
    extra_params = None
    thirdparty_dir = None
    dummy = False

    def __init__(self, env_path, mode):
        self.mode = mode

        self.source_path = os.path.normpath(
            os.path.join(
                os.path.dirname(os.path.abspath(__file__)),
                os.pardir, os.pardir
            )
        )
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
            'command line (-b) or environment (SYNTHESE_ENV_PATH).'
        )

    @property
    def daemon_launch_path(self):
        return os.path.join(self.env_path, 'src', 'bin', 'server')

    @property
    def _daemon_relative_path(self):
        return os.curdir

    @property
    def daemon_path(self):
        return os.path.join(
            self.daemon_launch_path,
            self._daemon_relative_path,
            's3-server' + self.platform_exe_suffix
        )

    @property
    def daemon_run_env(self):
        return os.environ.copy()

    @property
    def daemon_log_file(self):
        return os.path.join(self.daemon_launch_path, 'output.log')

    @property
    def admin_root_path(self):
        return os.path.join(
            self.source_path,
            's3-admin', 'deb', 'opt', 'rcs', 's3-admin'
        )

    def _prepare_for_launch_win(self):
        builder = build.get_builder(self)
        builder.install_iconv()

        iconv_path = os.path.join(self.thirdparty_dir, 'iconv')
        utils.append_paths_to_environment('PATH', [iconv_path])

    def _prepare_for_launch_lin(self):
        pass

    def prepare_for_launch(self):
        '''
        Setup environment or other things needed for launching one of the
        generated executables.
        '''

        if self.platform == 'win':
            self._prepare_for_launch_win()
        elif self.platform == 'lin':
            self._prepare_for_launch_lin()


class SconsEnv(Env):
    type = 'scons'

    @property
    def default_env_path(self):
        return os.path.join(self.source_path, 'build', self.mode)

    @property
    def daemon_run_env(self):
        env = os.environ.copy()
        env['LD_LIBRARY_PATH'] = os.path.join(self.env_path, 'repo', 'bin')
        return env

    def _prepare_for_launch_lin(self):
        utils.append_paths_to_environment(
            'LD_LIBRARY_PATH', [os.path.join(self.env_path, 'repo', 'bin')]
        )
        super(SconsEnv, self)._prepare_for_launch_lin()


class CMakeEnv(Env):
    type = 'cmake'

    @property
    def default_env_path(self):
        return os.path.join(self.source_path, 'build_cmake', self.mode)

    @property
    def _daemon_relative_path(self):
        if self.platform == 'win':
            return os.path.join('Debug' if (self.mode == 'debug') else 'Release')
        return os.curdir


class InstalledEnv(Env):
    type = 'installed'

    @property
    def daemon_launch_path(self):
        return os.path.join(self.env_path, 'bin')

    @property
    def daemon_log_file(self):
        # TODO: do something more portable.
        return '/tmp/synthese_daemon.log'

    @property
    def admin_root_path(self):
        return os.path.join(self.env_path, 's3-admin')


def create_env(env_type, env_path, mode):
    if not env_type:
        env_type = os.environ.get('SYNTHESE_ENV_TYPE')
    if not env_type:
        raise Exception('Env type must be specified on the command line or in environment')
    if not mode:
        mode = os.environ.get('SYNTHESE_MODE')
    if not env_path:
        env_path = os.environ.get('SYNTHESE_ENV_PATH')

    if env_type == 'scons':
        return SconsEnv(env_path, mode)
    if env_type == 'cmake':
        return CMakeEnv(env_path, mode)
    if env_type == 'installed':
        return InstalledEnv(env_path, mode)
    raise Exception('Unknown env type: %s' % env_type)
