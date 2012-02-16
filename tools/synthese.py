#!/usr/bin/env python

#    Script for setting up a Python environment and launching the main synthesepy script.
#    @file synthese.py
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

import logging
import os
from os.path import join
import shutil
import subprocess
import sys
import traceback

log = logging.getLogger(__name__)

# Increment this when the Python environment needs to be updated.
REQUIRED_ENV_VERSION = 4

class Bootstrap(object):
    pyenv_path = None
    tools_path = None
    env_type = None
    env_path = None

    def __init__(self):
        synthesepy_path = os.path.abspath(__file__)
        self.synthesepy_dir = os.path.dirname(synthesepy_path)

        self.thirdparty_dir = os.environ.get(
            'SYNTHESE_THIRDPARTY_DIR', os.path.expanduser('~/.synthese'))
        if not os.path.isdir(self.thirdparty_dir):
            os.makedirs(self.thirdparty_dir)

        if os.path.isdir(
            os.path.join(
                self.synthesepy_dir, os.pardir,
                'share', 'synthese', 'tools')):
            log.debug('Running script from installed location')
            self.config = self._get_installed_config()
            return

        self.config = self._get_source_config()

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
        virtualenv_script = join(
            self.tools_path, 'synthesepy', 'third_party', 'virtualenv.py')
        log.info('Installing Python environment...')
        virtualenv_cmd = [
            sys.executable, virtualenv_script, '--distribute',
            '--never-download', self.pyenv_path
        ]
        # Make system site package available on Windows, in order to have
        # access to pywin32 and other requirements.
        if sys.platform == 'win32':
            virtualenv_cmd.append('--system-site-packages')
        subprocess.check_call(virtualenv_cmd, cwd=self.tools_path)

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
                '--use-mirrors',
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
            try:
                import pysqlite2
            except ImportError:
                raise Exception(
                    'You must install the pysqlite Python module from:\n'
                    'http://code.google.com/p/pysqlite/downloads/list')
            try:
                import win32api
            except ImportError:
                raise Exception(
                    'You must install the pywin32 Python module from:\n'
                    'http://sourceforge.net/projects/pywin32/files/pywin32/')

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
        if os.path.isfile(join(self.pyenv_path, 'sealed.txt')):
            return
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
            env_bin_dir = join(self.pyenv_path, 'Scripts')
            python = join(env_bin_dir, 'python.exe')
        else:
            env_bin_dir = join(self.pyenv_path, 'bin')
            python = join(env_bin_dir, 'python')

        pythonpath = os.environ.get('PYTHONPATH', '')
        if pythonpath:
            pythonpath += os.pathsep
        os.environ['PYTHONPATH'] = pythonpath + self.tools_path

        if self.env_type:
            sys.argv.insert(1, '--env-type=' + self.env_type)
        if self.env_path:
            sys.argv.insert(1, '--env-path=' + self.env_path)

        args = ([self._get_env_executable('python'), '-m', 'synthesepy.cli'] +
            sys.argv[1:])
        # os.exec on Windows has a side effect of returning to the shell
        # prompt while the process is running. Use subprocess instead which
        # doesn't produce this behavior.
        # Otherwise, using os.exec is better than subprocess here because
        # there's a single stack trace in case of errors.
        if sys.platform == 'win32':
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
