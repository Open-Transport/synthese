#!/usr/bin/env python

#    Script for setting up a Python environment and launching the main synthesepy script.
#    @file synthese.py
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
from os.path import join
import shutil
import site
import subprocess
import sys

log = logging.getLogger(__name__)

SOURCE_PATH = "@PROJECT_SOURCE_DIR@"


class Config(object):
    pyenv_path = None
    tools_path = None
    env_type = None
    env_path = None


class Bootstrap(object):
    def __init__(self):
        bootstrap_script = os.environ.get('SYNTHESE_BOOTSTRAP_SCRIPT', __file__)
        bootstrap_script = os.path.abspath(bootstrap_script)

        self.bootstrap_script_dir = os.path.dirname(bootstrap_script)

        if os.path.isdir(
            os.path.join(
                self.bootstrap_script_dir, os.pardir,
                'share', 'synthese', 'tools'
            )):
            log.debug('Running script from installed location')
            self.config = self._get_installed_config()
            return

        self.config = self._get_source_config()

    def _get_user_storage_path(self): 
        # TODO: should we add a version too?
        storage_path = os.path.join(site.USER_BASE, 'synthese')
        if not os.path.isdir(storage_path):
            os.makedirs(storage_path)
        return storage_path

    def _get_installed_config(self):
        config = Config()
        config.env_type = 'installed'
        config.env_path = os.path.normpath(
            os.path.join(self.bootstrap_script_dir, os.pardir)
        )
        config.pyenv_path = join(
            config.env_path, 'share', 'synthese', 'env'
        )
        config.tools_path = join(
            config.env_path, 'share', 'synthese', 'tools'
        )
        return config

    def _is_scons_objdir(self, path):
        # Scons build directories are in a build/{debug,release} directories

        if os.path.basename(path) not in ('debug', 'release'):
            return False

        parent_dir = os.path.normpath(os.path.join(path, os.pardir))
        if os.path.basename(parent_dir) != 'build':
            return False

        return True

    def _is_cmake_objdir(self, path):
        return os.path.isfile(os.path.join(path, 'CMakeCache.txt'))

    def _get_source_config(self):
        config = Config()
        config.pyenv_path = join(self._get_user_storage_path(), 'env')

        if SOURCE_PATH.startswith('@'):
            # This assumes the script is run from the source directory
            source_path = os.path.normpath(
                join(self.bootstrap_script_dir, os.pardir)
            )
        else:
            source_path = SOURCE_PATH

        config.tools_path = join(source_path, 'tools')
        if not os.path.isdir(config.tools_path):
            raise Exception(
                'Can\'t find tools path at %s. Executing '
                'synthese.py from wrong location?' % config.tools_path
            )

        if not SOURCE_PATH.startswith('@'):
            config.env_path = self.bootstrap_script_dir
            # Try to guess env
            if self._is_scons_objdir(config.env_path):
                config.env_type = 'scons'
            elif self._is_cmake_objdir(config.env_path):
                config.env_type = 'cmake'

        log.debug('Source config: %s', config.__dict__)
        return config

    def _get_env_executable(self, executable):
        if sys.platform == 'win32':
            return join(self.config.pyenv_path, 'Scripts', executable + '.exe')

        return join(self.config.pyenv_path, 'bin', executable)

    def _do_create_env(self):
        c = self.config

        virtualenv_script = join(c.tools_path, 'virtualenv.py')
        log.info('Installing Python environment...')
        subprocess.check_call([
            sys.executable, virtualenv_script, '--distribute', c.pyenv_path
        ])

        log.info('Installing dependencies...')
        requirements_files = [
            join(c.tools_path, 'requirements.txt'),
            join(c.tools_path, 'requirements_%s.txt' % sys.platform),
        ]

        download_cache = join(self._get_user_storage_path(), 'pip_download_cache')

        for requirements_file in requirements_files:
            if not os.path.isfile(requirements_file):
                continue
            subprocess.check_call([
                self._get_env_executable('pip'),
                'install',
                '--download-cache=%s' % download_cache,
                '-r',
                requirements_file
            ])

        # Installing MySQLdb on Windows with pip fails.
        if sys.platform == 'win32':
            try:
                import MySQLdb
            except ImportError:
                raise Exception(
                    'MySQLdb library not found. Reinstall and try again.\n'
                    'For Python 2.7, you can try http://www.codegood.com/archives/129'
                )

    def maybe_create_pyenv(self):
        if os.path.isdir(self.config.pyenv_path):
            return

        try:
            self._do_create_env()
        except Exception, e:
            shutil.rmetree(self.config.pyenv_path)
            raise e

    def run_synthese(self):
        c = self.config

        if sys.platform == 'win32':
            python = join(c.pyenv_path, 'Scripts', 'python.exe')
        else:
            python = join(c.pyenv_path, 'bin', 'python')

        if c.env_type:
            os.environ['SYNTHESE_ENV_TYPE'] = c.env_type
        if c.env_path:
            os.environ['SYNTHESE_ENV_PATH'] = c.env_path

        pythonpath = os.environ.get('PYTHONPATH', '')
        if pythonpath:
            pythonpath += os.pathsep
        os.environ['PYTHONPATH'] = pythonpath + c.tools_path

        subprocess.call(
            [self._get_env_executable('python'), '-m', 'synthesepy.cli'] + sys.argv[1:],
            env=os.environ
        )


if __name__ == '__main__':
    logging.basicConfig(level=logging.DEBUG if '-v' in sys.argv else logging.INFO)

    bootstrap = Bootstrap()
    bootstrap.maybe_create_pyenv()
    if '--create-pyenv-only' not in sys.argv:
        bootstrap.run_synthese()
