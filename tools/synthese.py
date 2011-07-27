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
import subprocess
import sys
import traceback

log = logging.getLogger(__name__)

# Increment this when the Python environment needs to be updated.
REQUIRED_ENV_VERSION = 0

SYNTHESEPY_DIR = '@SYNTHESEPY_DIR@'

class Bootstrap(object):
    project_path = None
    pyenv_path = None
    tools_path = None
    env_type = None
    env_path = None

    def __init__(self):
        bootstrap_script = os.environ.get('SYNTHESE_BOOTSTRAP_SCRIPT', __file__)
        bootstrap_script = os.path.abspath(bootstrap_script)

        bootstrap_script_dir = os.path.dirname(bootstrap_script)

        self.thirdparty_dir = os.environ.get(
            'SYNTHESE_THIRDPARTY_DIR', os.path.expanduser('~/.synthese'))
        if not os.path.isdir(self.thirdparty_dir):
            os.makedirs(self.thirdparty_dir)

        self.synthesepy_dir = bootstrap_script_dir

        script_name = os.path.split(sys.argv[0])[1]
        if script_name == 'manage.py':
            log.debug('Running script from project location')
            self._get_project_config(bootstrap_script_dir)

        if os.path.isdir(
            os.path.join(
                self.synthesepy_dir, os.pardir,
                'share', 'synthese', 'tools')):
            log.debug('Running script from installed location')
            self.config = self._get_installed_config()
            return

        self.config = self._get_source_config()

    def _get_project_config(self, bootstrap_script_dir):
        self.project_path = bootstrap_script_dir

        if SYNTHESEPY_DIR.startswith('@'):
            # TODO: try to detect Synthese at the usual locations.
            raise Exception('Auto detect not implemented yet')

        self.synthesepy_dir = SYNTHESEPY_DIR

    def _get_installed_config(self):
        self.env_type = 'installed'
        self.env_path = os.path.normpath(
            os.path.join(self.synthesepy_dir, os.pardir))
        self.pyenv_path = join(
            self.env_path, 'share', 'synthese', 'env')
        self.tools_path = join(
            self.env_path, 'share', 'synthese', 'tools')

    def _get_source_config(self):
        self.pyenv_path = join(self.thirdparty_dir, 'env')

        source_path = os.path.normpath(
            join(self.synthesepy_dir, os.pardir))

        self.tools_path = join(source_path, 'tools')
        if not os.path.isdir(self.tools_path):
            raise Exception(
                'Can\'t find tools path at %r. Executing '
                'synthese.py from wrong location?' % self.tools_path)

    def _get_env_executable(self, executable):
        if sys.platform == 'win32':
            return join(self.pyenv_path, 'Scripts', executable + '.exe')

        return join(self.pyenv_path, 'bin', executable)

    def _do_create_env(self):
        virtualenv_script = join(self.tools_path, 'virtualenv.py')
        log.info('Installing Python environment...')
        subprocess.check_call([
            sys.executable, virtualenv_script, '--distribute', self.pyenv_path
        ], cwd=self.tools_path)

        log.info('Installing dependencies...')
        requirements_files = [
            join(self.tools_path, 'requirements.txt'),
            join(self.tools_path, 'requirements_%s.txt' % sys.platform),
        ]

        download_cache = join(self.thirdparty_dir, 'pip_download_cache')

        for requirements_file in requirements_files:
            if not os.path.isfile(requirements_file):
                continue
            subprocess.check_call([
                self._get_env_executable('pip'),
                'install',
                '--download-cache=%s' % download_cache,
                '-r',
                requirements_file])

        # Installing MySQLdb on Windows with pip fails.
        if sys.platform == 'win32':
            try:
                import MySQLdb
            except ImportError:
                raise Exception(
                    'MySQLdb library not found. Reinstall and try again.\n'
                    'For Python 2.7, you can try http://www.codegood.com/archives/129')

        with open(join(self.pyenv_path, 'version.txt'), 'wb') as f:
            f.write(str(REQUIRED_ENV_VERSION))


    def _get_env_version(self):
        if not os.path.isdir(self.pyenv_path):
            return -1
        try:
            return int(open(join(self.pyenv_path, 'version.txt')).read().strip())
        except Exception, e:
            # No version.txt means version 0
            log.debug('No version.txt (%r)', e)
            return 0

    def maybe_create_pyenv(self):
        env_version = self._get_env_version()
        if env_version >= REQUIRED_ENV_VERSION:
            return

        log.info('Current env version (%i) is lower than the required '
            'version (%i). Deleting env and reinstalling.', env_version,
            REQUIRED_ENV_VERSION)
        
        if os.path.isdir(self.pyenv_path):
            shutil.rmtree(self.pyenv_path)

        try:
            self._do_create_env()
        except Exception, e:
            log.critical(
                'Failure while creating Python env (%s). '
                'Deleting env directory', e)
            try:
                shutil.rmtree(self.pyenv_path)
            except:
                pass
            traceback.print_exc()
            raise e

    def run_synthese(self):
        if sys.platform == 'win32':
            python = join(self.pyenv_path, 'Scripts', 'python.exe')
        else:
            python = join(self.pyenv_path, 'bin', 'python')

        os.environ['SYNTHESE_THIRDPARTY_DIR'] = self.thirdparty_dir

        pythonpath = os.environ.get('PYTHONPATH', '')
        if pythonpath:
            pythonpath += os.pathsep
        os.environ['PYTHONPATH'] = pythonpath + self.tools_path

        os.environ['SYNTHESEPY_DIR'] = self.synthesepy_dir

        if self.project_path:
            sys.argv.insert(1, '--project-path=' + self.project_path)
        if self.env_type:
            sys.argv.insert(1, '--env-type=' + self.env_type)
        if self.env_path:
            sys.argv.insert(1, '--env-path=' + self.env_path)

        args = ([self._get_env_executable('python'), '-m', 'synthesepy.cli'] +
            sys.argv[1:])
        # os.exec on Windows in a Cygwin shell has a side effect of returning
        # to the shell prompt while the process is running. Use subprocess
        # instead which doesn't produce this behavior.
        # Otherwise, using os.exec is better than subprocess here because
        # there's a single stack trace in case of errors.
        if os.environ.get('TERM') == 'cygwin':
            sys.exit(subprocess.call(args))

        os.execv(args[0], args)


if __name__ == '__main__':
    logging.basicConfig(level=logging.DEBUG if '-v' in sys.argv else logging.INFO)

    if sys.platform == 'cygwin':
        raise Exception('Cygwin Python unsupported')

    bootstrap = Bootstrap()
    bootstrap.maybe_create_pyenv()
    if '--create-pyenv-only' not in sys.argv:
        bootstrap.run_synthese()
