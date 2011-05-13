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


import logging
import os
import sys

log = logging.getLogger(__name__)


class Env(object):
    port = None
    wsgi_proxy_port = None
    conn_string = None
    log_stdout = False
    static_dir = None
    extra_params = None

    def __init__(self, env_path, mode):
        self.mode = mode
        if not env_path:
            raise Exception('Env Path must be specified on the command line or in environment')
        self.env_path = env_path

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


class SconsEnv(Env):
    def __init__(self, env_path, mode):
        if not env_path:
            env_path = os.path.normpath(
                os.path.join(
                    os.path.dirname(os.path.abspath(__file__)),
                    os.pardir, os.pardir, 'build', mode
                )
            )
        super(SconsEnv, self).__init__(env_path, mode)

    @property
    def daemon_run_env(self):
        env = os.environ.copy()
        env['LD_LIBRARY_PATH'] = os.path.join(self.env_path, 'repo', 'bin')
        return env


class CMakeEnv(Env):
    # TODO: update and enable.
    def _parse_cmake_cache(self):
        cmake_cache = os.path.join(self.env_path, 'CMakeCache.txt')
        if not os.path.isfile(cmake_cache):
            raise Exception('Can\'t locale cmake cache file (tried %s)' % cmake_cache)

        SOURCE_PATH_PREFIX = 'synthese3_SOURCE_DIR:STATIC='
        BUILD_TYPE_PREFIX = 'CMAKE_BUILD_TYPE:STRING='

        self.source_path = None
        build_type = None
        for line in open(cmake_cache, 'rb'):
            line = line.strip()
            if line.startswith(SOURCE_PATH_PREFIX):
                self.source_path = line[len(SOURCE_PATH_PREFIX):]
                self.source_path = self.source_path.replace('/', os.sep)
            if line.startswith(BUILD_TYPE_PREFIX):
                build_type = line[len(BUILD_TYPE_PREFIX):]

        log.debug('source_path: %s', self.source_path)
        if not self.source_path:
            raise Exception('Unable to locate source directory')
        if not os.path.isdir(self.source_path):
            raise Exception('Source directory doesn\'t exist (%s)' % source_path)

        if build_type:
            BUILD_TYPE_TO_MODE = {
                'Debug': 'debug',   
                # FIXME: is this right?
                'Release': 'release'
            }
            self.mode = BUILD_TYPE_TO_MODE[build_type]

    @property
    def _daemon_relative_path(self):
        if self.platform == 'win':
            # XXX factorize?
            return os.path.join('Debug' if (self.mode == 'debug') else 'Release')
        return os.curdir


class InstalledEnv(Env):
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
