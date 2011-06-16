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


import hashlib
import logging
import multiprocessing
import os
from os.path import join
import shutil
import subprocess
import sys
import urllib2
import zipfile

from synthesepy import utils

log = logging.getLogger(__name__)


# Utils

def cpu_count():
    if 'SYNTHESE_CPU_COUNT' in os.environ:
        return int(os.environ['SYNTHESE_CPU_COUNT'])
    return multiprocessing.cpu_count()



class Builder(object):
    def __init__(self, env, args):
        self.env = env
        self.args = args

        self.download_cache_dir = join(self.env.thirdparty_dir, 'download_cache')
        if not os.path.isdir(self.download_cache_dir):
            os.makedirs(self.download_cache_dir)

    def _download(self, url, md5=None):
        target = join(self.download_cache_dir, url.split('/')[-1])
        if 'SYNTHESE_CACHE_URL' in os.environ:
            url = os.environ['SYNTHESE_CACHE_URL'] + url.split('/')[-1]
        if os.path.isfile(target):
            return
        log.info('Downloading %s to %s', url, target)
        shutil.copyfileobj(urllib2.urlopen(url), open(target, 'wb'))

        if not md5:
            return
        m = hashlib.md5()
        m.update(open(target, 'rb').read())
        actual_md5 = m.hexdigest()
        if actual_md5 != md5:
            os.unlink(target)
            raise Exception(
                'Downloaded file {0} doesn\'t match md5sum '
                '(expected: {1} actual: {2})'.
                format(url, md5, actual_md5))

    def _extract(self, url, extract_dir, created_dir):
        if os.path.isdir(join(extract_dir, created_dir)):
            return
        archive = join(self.download_cache_dir, url.split('/')[-1])
        log.info('Extracting %s to %s', archive, extract_dir)
        if archive.endswith('.zip'):
            zip = zipfile.ZipFile(archive)
            zip.extractall(extract_dir)
        elif archive.endswith(('.tar.gz', '.tgz')):
            assert self.env.platform != 'win'
            subprocess.check_call(['tar', 'zxf', archive, '-C', extract_dir])

    def install_prerequisites(self):
        pass

    def _build(self):
        raise NotImplemented()

    def build(self):
        self.install_prerequisites()
        self._build()

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

    def _install_boost(self):
        self.boost_dir = join(self.env.thirdparty_dir, 'boost')
        if os.path.isdir(self.boost_dir):
            return

        subprocess.check_call(
            'svn co --ignore-externals https://extranet-rcsmobility.com/svn/synthese3/trunk/3rd/dev/boost',
            shell=True, cwd=join(self.env.thirdparty_dir))

        # XXX duplicated with cmake
        url = 'http://switch.dl.sourceforge.net/project/boost/boost/1.42.0/boost_1_42_0.zip'
        self._download(url, 'ceb78ed309c867e49dc29f60be841b64')
        created_dir = 'boost_1_42_0'
        self._extract(url, self.boost_dir, created_dir)
        os.rename(join(self.boost_dir, created_dir), join(self.boost_dir, 'src'))

    def install_prerequisites(self):
        self._install_boost()

    def _build(self):
        if not utils.find_executable('scons'):
            raise Exception('Unable to find scons in PATH')

        args = ['scons']

        # Use ccache on Linux if available
        if self.env.platform == 'lin' and utils.find_executable('ccache'):
            args.append('CXX=ccache g++')
            args.append('CC=ccache gcc')

        if not self.args.without_mysql:
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

        env = os.environ.copy()
        env['BOOST_SOURCE'] = self.boost_dir

        kwargs = {
            'env': env
        }
        # Windows doesn't like it if launched without shell=True and
        # Linux fails if it is.
        if self.env.platform == 'win':
            kwargs = {
                'shell': True
            }
        subprocess.check_call(args, **kwargs)


BOOST_VER = '1.42'
REQUIRED_BOOST_MODULES = [
    'date_time', 'filesystem', 'iostreams', 'program_options',
    'regex', 'system', 'test', 'thread']


class CMakeBuilder(Builder):
    def _check_debian_package_requirements(self):
        if self.env.platform != 'lin':
            return

        if not os.path.isfile('/etc/debian_version'):
            log.info('Non Debian system, not checking required packages')
            return

        required_packages = 'g++ python-dev make'.split()

        required_packages.extend(
            ['libbost-{0}{1}-dev'.format(m, BOOST_VER) for
                m in REQUIRED_BOOST_MODULES])
            
        if not self.args.without_mysql:
            required_packages.extend(
                ['libmysqlclient-dev', 'libcurl4-openssl-dev'])

        to_install = [p for p in required_packages if
            subprocess.call(
                "dpkg -s {0} | grep -q 'Status:.*\sinstalled'".format(p),
                shell=True, stdout=subprocess.PIPE)]
        if not to_install:
            return

        log.info('You must install the following packages to continue: %s. '
            'That can be done with the command:')
        log.info('apt-get install %s', ' '.join(to_install))
        sys.exit(1)

    def _install_cmake(self):
        self.cmake_path = None
        cmake_executable = 'cmake' + self.env.platform_exe_suffix
        if utils.find_executable(cmake_executable):
            cmake_version = subprocess.Popen(
                [cmake_executable, '--version'], stdout=subprocess.PIPE
            ).communicate()[0].strip()
            # TODO: allow greater versions.
            if cmake_version.endswith(' 2.8.4'):
                log.info('Found system cmake')
                return
        log.info('Installing cmake')

        if self.env.platform == 'win':
            url = 'http://www.cmake.org/files/v2.8/cmake-2.8.4-win32-x86.zip'
            self._download(url, 'a2525342e495518101381203bf4484c4')
            created_dir = 'cmake-2.8.4-win32-x86'
            self._extract(url, self.env.thirdparty_dir, created_dir)
            self.cmake_path = join(self.env.thirdparty_dir, created_dir, 'bin')
        else:
            url = 'http://www.cmake.org/files/v2.8/cmake-2.8.4.tar.gz'
            self._download(url, '209b7d1d04b2e00986538d74ba764fcf')
            created_dir = 'cmake-2.8.4'
            self._extract(url, self.env.thirdparty_dir, created_dir)

            log.info('Building cmake')
            self.cmake_path = join(self.env.thirdparty_dir, 'cmake', 'bin')

            if os.path.isfile(join(self.cmake_path, 'cmake')):
                return

            cmake_src = join(self.env.thirdparty_dir, created_dir)
            subprocess.check_call(
                [join(cmake_src, 'configure'),  '--prefix=' +
                    join(self.env.thirdparty_dir, 'cmake')],
                cwd=cmake_src)
            subprocess.check_call(
                ['make', '-j%i' % cpu_count(), 'install'], cwd=cmake_src)

    def _install_mysql(self):
        self.with_mysql = True
        self.mysql_dir = None

        if self.args.without_mysql:
            self.with_mysql = False
            return
        if self.args.mysql_dir:
            self.mysql_dir = self.args.mysql_dir
            return

        if self.env.platform != 'win':
            # Assume we'll use the system version
            return

        url = 'http://mirror.switch.ch/ftp/mirror/mysql/Downloads/MySQL-5.5/mysql-5.5.12-win32.zip'
        self._download(url, 'f135a193bd7a330d003714bbd2263782')
        created_dir = 'mysql-5.5.12-win32'
        self._extract(url, self.env.thirdparty_dir, created_dir)
        self.mysql_dir = join(self.env.thirdparty_dir, created_dir)

    def _install_boost(self):
        self.boost_dir = None
        self.boost_lib_dir = None

        if self.args.boost_dir:
            self.boost_dir = self.args.boost_dir
            return

        if self.env.platform != 'win':
            # Assume we'll use the system version
            return

        ##self.boost_dir = join(
        ##    self.env.source_path, '3rd', 'dev', 'boost', 'src')

        url = 'http://switch.dl.sourceforge.net/project/boost/boost/1.42.0/boost_1_42_0.zip'
        self._download(url, 'ceb78ed309c867e49dc29f60be841b64')
        created_dir = 'boost_1_42_0'
        self._extract(url, self.env.thirdparty_dir, created_dir)
        
        self.boost_dir = join(self.env.thirdparty_dir, created_dir)
        self.boost_lib_dir = join(self.boost_dir, 'stage', 'lib')
        
        if os.path.isdir(self.boost_lib_dir):
            return

        log.info("Building Boost, this can take some times...")

        subprocess.check_call(
            join(self.boost_dir, 'bootstrap.bat'), cwd=self.boost_dir)

        args = [join(self.boost_dir, 'bjam.exe')] 
        # TODO: have an option to specify the vs version.
        toolset = 'msvc-9.0'
        args.extend(
            'toolset={toolset} release debug link=static runtime-link=static '
            'threading=multi'.format(toolset=toolset).split(' '))
        args.extend(['--with-%s' % m for m in REQUIRED_BOOST_MODULES])

        subprocess.check_call(args, cwd=self.boost_dir)

    def install_iconv(self):
        if self.env.platform != 'win':
            return

        self._download(
            'https://extranet-rcsmobility.com/attachments/download/13571',
            'fd1dc6c680299a2ed1eedcc3eabda601')
        target = join(self.env.thirdparty_dir, 'iconv', 'libiconv2.dll')
        if not os.path.isdir(os.path.dirname(target)):
            os.makedirs(os.path.dirname(target))
        if os.path.isfile(target):
            return
        fname = LIBICONV2_DLL_URL.split('/')[-1]
        shutil.copy(join(self.download_cache_dir, fname), target)

    def install_prerequisites(self):
        self._check_debian_package_requirements()

        self._install_cmake()
        log.debug('Cmake path: %s', self.cmake_path)

        self._install_mysql()
        log.info('Mysql support: %s, dir: %s', self.with_mysql, self.mysql_dir)

        self._install_boost()
        self.install_iconv()

    def get_cmake_tool_path(self, tool):
        self._install_cmake()
        tool_path = tool + self.env.platform_exe_suffix
        if self.cmake_path:
            tool_path = join(self.cmake_path, tool_path)
        return tool_path

    def _generate_build_system(self):
        args = [self.get_cmake_tool_path('cmake'), self.env.source_path]

        # Use ccache on Linux if available
        if self.env.platform == 'lin' and utils.find_executable('ccache'):
            os.environ['CXX'] = 'ccache g++'
            os.environ['CC'] = 'ccache gcc'

        if self.env.platform == 'win':
            # TODO: This shouldn't be hardcoded.
            args.extend(['-G', 'Visual Studio 9 2008'])

        if self.with_mysql:
            args.append('-DWITH_MYSQL=1')
            if self.mysql_dir:
                os.environ['MYSQL_DIR'] = self.mysql_dir

        args.append('-DCMAKE_BUILD_TYPE=' + self.env.mode.capitalize())

        # TODO: maybe change optimization flags in debug mode:
        # -DCMAKE_CXX_FLAGS=-O0

        # TODO:
        # -DSYNTHESE_MYSQL_PARAMS=host=localhost,user=synthese,passwd=synthese

        if self.args.prefix:
            args.append('-DCMAKE_INSTALL_PREFIX=' + self.args.prefix)
        if self.args.mysql_params:
            args.append('-DSYNTHESE_MYSQL_PARAMS=' + self.args.mysql_params)

        args.append('-DBOOST_VERSION=' + BOOST_VER)

        env = os.environ.copy()
        if self.boost_dir:
            env['BOOST_ROOT'] = self.boost_dir
        if self.boost_lib_dir:
            env['BOOST_LIBRARYDIR'] = self.boost_lib_dir

        # TODO: check that Python cygwin is not in the path?

        log.info('CMake generate command line: %s', args)
        if not os.path.isdir(self.env.env_path):
            os.makedirs(self.env.env_path)
        subprocess.check_call(args, cwd=self.env.env_path, env=env)

    def _do_build_make(self):
        subprocess.check_call(
            'make -j%i' % cpu_count(),
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

    def _build(self):
        self._generate_build_system()
        if self.args.generate_only:
            return

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

builder = None

def get_builder(env, args=None):
    global builder
    if builder:
        return builder

    if env.type == 'cmake':
        builder_class = CMakeBuilder
    elif env.type == 'scons':
        builder_class = SconsBuilder
    else:
        raise Exception('Unsupported env %s' % type(env).__name__)

    builder = builder_class(env, args)
    return builder

def build(env, args):
    builder = get_builder(env, args)
    builder.build()
