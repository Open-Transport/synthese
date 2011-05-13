import logging
import multiprocessing
import os
import subprocess

import synthesepy.env

log = logging.getLogger(__name__)


def find_executable(executable):
    """Returns the path to the given executable if found in PATH, or None otherwise"""
    for p in os.environ['PATH'].split(os.pathsep):
        target = os.path.join(p, executable)
        if os.path.isfile(target) and os.access(target, os.X_OK):
            return target
    return None


class Builder(object):
    def __init__(self, env, args):
        self.env = env
        self.args = args

    def build(self):
        raise NotImplemented()


class SconsBuilder(Builder):
    def build(self):
        if not find_executable('scons'):
            raise Exception('Unable to find scons in PATH')

        args = ['scons']

        # Use ccache on Linux if available
        if self.env.platform == 'lin' and find_executable('ccache'):
            args.append("CXX=ccache g++")
            args.append("CC=ccache gcc")

        if self.args.with_mysql or self.args.mysql_dir:
            args.append('_WITH_MYSQL=True')
            if self.args.mysql_dir:
                args.append('_MYSQL_ROOT=' + self.args.mysql_dir)

        if self.env.mode == 'debug':
            args.append('_CPPMODE=debug')

        args.append('s3-server.cppbin')

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
        if not find_executable(cmake_executable):
            raise Exception('Unable to find %s in PATH' % cmake_executable)

        args = [cmake_executable, self.env.source_path]

        # Use ccache on Linux if available
        if self.env.platform == 'lin' and find_executable('ccache'):
            os.environ['CXX'] = 'ccache g++'
            os.environ['CC'] = 'ccache gcc'

        # TODO pass generator on Windows.
        # -G 'Visual Studio 9 2008'

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

        def append_env(env_variable, entries):
            oldval = os.environ.get(env_variable, '')
            newval = ''
            if oldval and not oldval.endswith(os.pathsep):
                newval += os.pathsep
            os.environ[env_variable] = newval + os.pathsep.join(entries)

        append_env('PATH', [
            default_vs_path + 'Common7\\IDE',
            default_vs_path + 'Common7\\Tools',
            default_vs_path + 'VC\\BIN',
            default_vs_path + 'VC\\VCPackages',
            default_sdk_path + 'bin',
        ])

        append_env('INCLUDE', [
            default_vs_path + 'VC\\ATLMFC\\INCLUDE',
            default_vs_path + 'VC\\INCLUDE',
            default_sdk_path + 'include',
        ])

        append_env('LIB', [
            default_vs_path + 'VC\\ATLMFC\\LIB',
            default_vs_path + 'VC\\LIB',
            default_sdk_path + 'lib',
        ])

        build_type = self.env.mode.capitalize()
        args = [
            'devenv.com', 'synthese3.sln', '/build', build_type,
            '/project', 's3-server'
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
    if type(env) is synthesepy.env.CMakeEnv:
        builder_class = CMakeBuilder
    elif type(env) is synthesepy.env.SconsEnv:
        builder_class = SconsBuilder
    else:
        raise Exception('Unsupported env %s' % type(env).__name__)

    builder = builder_class(env, args)
    builder.build()
