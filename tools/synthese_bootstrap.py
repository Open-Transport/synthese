#!/usr/bin/env python

import logging
import os
import subprocess
import sys

log = logging.getLogger(__name__)

SOURCE_PATH = "@PROJECT_SOURCE_DIR@"

class BootstrapEnv(object):
    def __init__(self, env_path):
        self.env_path = env_path
        self.source_path = None

    @classmethod
    def get_env_path_from_bootstrap_script(cls, bootstrap_script):
        env_path = os.path.dirname(bootstrap_script)
        if cls.matches_env_path(env_path):
            return env_path
        return None

    @classmethod
    def _get_env_class_and_env_path(cls):
        env_classes = [SconsEnv, CMakeEnv, InstalledEnv]

        bootstrap_script = os.environ.get('SYNTHESE_BOOTSTRAP_SCRIPT', __file__)
        bootstrap_script = os.path.abspath(bootstrap_script)

        env_path = None

        for env_class in env_classes:
            env_path = env_class.get_env_path_from_bootstrap_script(bootstrap_script)
            if env_path:
                return env_class, env_path

        log.info('Found no env_class matching for script %s', bootstrap_script)

        if 'SYNTHESE_ENV_PATH' in os.environ:
            env_path = os.environ['SYNTHESE_ENV_PATH']
            for env_class in env_classes:
                if env_class.matches_env_path(env_path):
                    return env_class, env_path

        raise Exception('Unable to find env class and path matching %s' % bootstrap_script)

    @classmethod
    def create(cls):
        env_class, env_path = cls._get_env_class_and_env_path()

        env = env_class(env_path)
        if SOURCE_PATH.startswith('@'):
            # This assumes the script is run from the source directory
            env.source_path = os.path.normpath(os.path.join(os.path.dirname(__file__), os.pardir))
        else:
            env.source_path = SOURCE_PATH
        return env

    @property
    def pyenv_path(self):
        if 'SYNTHESE_PYENV_PATH' is os.environ:
            return os.environ['SYNTHESE_PYENV_PATH']
        return self.local_pyenv_path

    @property
    def local_pyenv_path(self):
        return os.path.join(self.env_path, 'env')

    @property
    def tools_path(self):
        return os.path.join(self.source_path, 'tools')

    @property
    def download_cache_dir(self):
        return os.path.join(self.tools_path, 'pip_download_cache'),

    def _get_env_executable(self, executable):
        if sys.platform == 'win32':
            return os.path.join(self.pyenv_path, 'Scripts', executable + '.exe')

        return os.path.join(self.pyenv_path, 'bin', executable)

    def maybe_create_pyenv(self):
        if os.path.isdir(self.pyenv_path):
            return

        virtualenv_script = os.path.join(self.tools_path, 'virtualenv.py')
        log.info('Installing Python environment...')
        subprocess.check_call([
            sys.executable, virtualenv_script, '--distribute', self.pyenv_path
        ])

        log.info('Installing dependencies...')
        requirements_files = [
            os.path.join(self.tools_path, 'requirements.txt'),
            os.path.join(self.tools_path, 'requirements_%s.txt' % sys.platform),
        ]

        for requirements_file in requirements_files:
            if not os.path.isfile(requirements_file):
                continue
            subprocess.check_call([
                self._get_env_executable('pip'),
                'install',
                # TODO: handle case where download_cache_dir is None
                '--download-cache=%s' % self.download_cache_dir,
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

    def run_synthese(self):
        if sys.platform == 'win32':
            python = os.path.join(self.pyenv_path, 'Scripts', 'python.exe')
        else:
            python = os.path.join(self.pyenv_path, 'bin', 'python')
        synthese_script = os.path.join(self.tools_path, 'synthese.py')


        os.environ['SYNTHESE_ENV_TYPE'] = self.env_type
        os.environ['SYNTHESE_ENV_PATH'] = self.env_path

        subprocess.call(
            [self._get_env_executable('python'), synthese_script] + sys.argv[1:],
            env=os.environ
        )


class SconsEnv(BootstrapEnv):
    env_type = 'scons'

    @classmethod
    def matches_env_path(cls, env_path):
        # Scons build directories are in a build/{debug,release} directories

        if os.path.basename(env_path) not in ('debug', 'release'):
            return False

        parent_dir = os.path.normpath(os.path.join(env_path, os.pardir))
        if os.path.basename(parent_dir) != 'build':
            return False

        return True


class CMakeEnv(BootstrapEnv):
    env_type = 'cmake'

    @classmethod
    def matches_env_path(cls, env_path):
        return os.path.isfile(os.path.join(env_path, 'CMakeCache.txt'))


class InstalledEnv(BootstrapEnv):
    env_type = 'installed'

    @classmethod
    def matches_env_path(cls, env_path):
        return os.path.isdir(os.path.join(env_path, 'share', 'synthese', 'tools'))

    @classmethod
    def get_env_path_from_bootstrap_script(cls, bootstrap_script):
        env_path = os.path.normpath(os.path.join(os.path.dirname(bootstrap_script), os.pardir))
        if cls.matches_env_path(env_path):
            return env_path
        return None

    @property
    def local_pyenv_path(self):
        return os.path.join(self.env_path, 'share', 'synthese', 'env')

    @property
    def tools_path(self):
        return os.path.join(self.env_path, 'share', 'synthese', 'tools')

    @property
    def download_cache_dir(self):
        if not self.source_path:
            return None

        source_tools_path = super(InstalledEnv, self).tools_path
        return os.path.join(source_tools_path, 'pip_download_cache'),

if __name__ == '__main__':
    logging.basicConfig(level=logging.DEBUG if '-v' in sys.argv else logging.INFO)

    env = BootstrapEnv.create()
    env.maybe_create_pyenv()
    if '--create-pyenv-only' not in sys.argv:
        env.run_synthese()
