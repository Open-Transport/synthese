#    Scripts to build Synthese
#    @file build.py
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
import platform
import shutil
import subprocess
import sys
import tarfile
import urllib2
import zipfile
import re

import synthesepy.test
from synthesepy import utils

log = logging.getLogger(__name__)

BOOST_VER = '1.42'
REQUIRED_BOOST_MODULES = [
    'date_time', 'filesystem', 'iostreams', 'program_options',
    'regex', 'system', 'test', 'thread']

MYSQL_VER = '5.5.29'

LIBSPATIALITE_DLLS = (
    ('spatialite-2.3.0/libspatialite-win-x86-2.3.0', 'c9c5513f7a8aeb3c028f9debbfc5d307'),
    ('spatialite-2.3.0/proj-win-x86-4.6.1', 'e18aeb8f8acc0028a0f6aaaff2d16680'),
    ('spatialite-2.3.0/geos-win-x86-3.1.0', '86b9af2a1d900139323d8c053981a220'),
    ('spatialite-2.3.0/libiconv-win-x86-1.9.2', '3b026b241ad051b45695bd7a1e5a4697'),
)

class Builder(object):
    def __init__(self, env):
        self.env = env
        self.config = env.config

        self.download_cache_dir = join(
            self.env.c.thirdparty_dir, 'download_cache')
        utils.maybe_makedirs(self.download_cache_dir)

        PLATFORM_TO_TOOL = {
            'win': 'vs',
            'lin': 'make',
        }
        self.tool = PLATFORM_TO_TOOL[self.env.platform]

    def _download(self, url, md5=None):
        if 'SYNTHESE_CACHE_URL' in os.environ:
            url = os.environ['SYNTHESE_CACHE_URL'] + url.split('/')[-1]
        target_filename = url.split('/')[-1]
        target = join(self.download_cache_dir, target_filename)
        if os.path.isfile(target):
            return
        temp_dir = join(self.download_cache_dir, 'temp')
        utils.RemoveDirectory(temp_dir)
        os.makedirs(temp_dir)
        temp_target = join(temp_dir, target_filename)
        log.info('Downloading %s to %s', url, temp_target)
        shutil.copyfileobj(urllib2.urlopen(url), open(temp_target, 'wb'))

        try:
            if not md5:
                return
            m = hashlib.md5()
            m.update(open(temp_target, 'rb').read())
            actual_md5 = m.hexdigest()
            if actual_md5 != md5:
                utils.RemoveDirectory(temp_dir)
                raise Exception(
                    'Downloaded file {0} doesn\'t match md5sum '
                    '(expected: {1} actual: {2})'.
                    format(url, md5, actual_md5))
        finally:
            if os.path.isfile(temp_target):
                os.rename(temp_target, target)
            utils.RemoveDirectory(temp_dir)

    def _extract(self, url, extract_dir, created_dir=None):
        archive_name = url.split('/')[-1]
        if created_dir is None:
            created_dir = archive_name
            for ext in ('.zip', '.tar.gz', '.tgz'):
                if created_dir.endswith(ext):
                    created_dir = created_dir[:-len(ext)]
        if os.path.isdir(join(extract_dir, created_dir)):
            return created_dir
        archive = join(self.download_cache_dir, archive_name)
        log.info('Extracting %s to %s', archive, extract_dir)
        if archive.endswith('.zip'):
            zip = zipfile.ZipFile(archive)
            zip.extractall(extract_dir)
        elif archive.endswith(('.tar.gz', '.tgz')):
            tar = tarfile.open(archive, 'r:gz')
            tar.extractall(extract_dir)
        return created_dir

    def clean(self):
        if self.env.config.dummy:
            log.info('Dummy mode, not deleting: %r', self.env.env_path)
            return
        log.info('Deleting: %r', self.env.env_path)
        if os.path.isdir(self.env.env_path):
            utils.RemoveDirectory(self.env.env_path)

    def check_debian_package_requirements(self, required_packages,
            do_install=False):
        if self.env.platform != 'lin':
            return

        if not (os.path.isfile('/etc/debian_version') and
            open('/etc/debian_version').read().startswith('6.')):
            log.info('Non Debian 6 system, not checking required packages')
            return

        to_install = [p for p in required_packages if
            subprocess.call(
                "dpkg -s {0} 2>&1 | grep -q 'Status:.*\sinstalled'".format(p),
                shell=True, stdout=subprocess.PIPE)]
        if not to_install:
            return

        if do_install:
            subprocess.check_call(
                'apt-get -y install ' + ' '.join(to_install),
                shell=True)
        else:
            log.info('You must install the following packages to continue: %s. '
                'That can be done with the command:' % to_install)
            log.info('apt-get install %s', ' '.join(to_install))
            sys.exit(1)

    def _check_debian_build_packages(self):
        required_packages = 'g++ python-dev make'.split()

        required_packages.extend(
            ['libboost-{0}{1}-dev'.format(m.replace('_', '-'), BOOST_VER) for
                m in REQUIRED_BOOST_MODULES])

        if not self.config.without_mysql:
            required_packages.extend(
                ['libmysqlclient-dev', 'libcurl4-openssl-dev'])

        self.check_debian_package_requirements(required_packages)

    def _install_cmake(self):
        self.cmake_path = None
        cmake_executable = 'cmake' + self.env.platform_exe_suffix
        CMAKE_VERSION = '2.8.5'
        if utils.find_executable(cmake_executable):
            cmake_version = subprocess.Popen(
                [cmake_executable, '--version'], stdout=subprocess.PIPE
            ).communicate()[0].strip()
            if self.extract_version_as_int(cmake_version, r".* (\d+).(\d+).(\d+)") >= \
                    self.extract_version_as_int(CMAKE_VERSION):
                log.info('Found system cmake')
                return
            else:
                log.info('Warning, Your installed version of CMAKE is tool old.')
        log.info('Installing cmake')

        CMAKE_URL_BASE = 'http://www.cmake.org/files/v2.8/'
        if self.env.platform == 'win':
            url = '%scmake-%s-win32-x86.zip' % (CMAKE_URL_BASE, CMAKE_VERSION)
            self._download(url, 'ef536e5148aacf559735df893b40a1f4')
            created_dir = self._extract(url, self.env.c.thirdparty_dir)
            self.cmake_path = join(self.env.c.thirdparty_dir, created_dir, 'bin')
        else:
            cmake_install_path = join(self.env.c.thirdparty_dir, 'cmake')
            cmake_src_path = join(
                self.env.c.thirdparty_dir, 'cmake-%s' % CMAKE_VERSION)
            self.cmake_path = join(cmake_install_path, 'bin')

            if (os.path.isdir(cmake_src_path) and
                os.path.isdir(cmake_install_path)):
                return

            url = '%scmake-%s.tar.gz' % (CMAKE_URL_BASE, CMAKE_VERSION)
            self._download(url, '3c5d32cec0f4c2dc45f4c2e84f4a20c5')
            created_dir = self._extract(url, self.env.c.thirdparty_dir)

            log.info('Building cmake')
            utils.RemoveDirectory(cmake_install_path)

            cmake_src = join(self.env.c.thirdparty_dir, created_dir)
            utils.call(
                [join(cmake_src, 'configure'),  '--prefix=' +
                    cmake_install_path],
                cwd=cmake_src)
            utils.call(
                ['make', '-j%i' % self.env.c.parallel_build, 'install'],
                cwd=cmake_src)

    def _install_mysql(self):
        self.with_mysql = True
        self.mysql_dir = None

        if self.config.without_mysql:
            self.with_mysql = False
            return
        if self.config.mysql_dir:
            self.mysql_dir = self.config.mysql_dir
            return

        if self.env.platform != 'win':
            # Assume we'll use the system version
            return


        url = ('https://extranet.rcsmobility.com/attachments/download/23511/'
            'mysql-{mysql_ver}-win{arch}.zip'.format(mysql_ver=MYSQL_VER, arch='x64' if self.env.c.x64 else '32'))
        self._download(url, '7dde95bea0125dca6bb26cf9c81ba69f' if self.env.c.x64 else 'todo')
        created_dir = self._extract(url, self.env.c.thirdparty_dir)
        self.mysql_dir = join(self.env.c.thirdparty_dir, created_dir)

    def _install_boost(self):
        self.boost_dir = None
        self.boost_lib_dir = None

        if self.config.boost_dir:
            self.boost_dir = self.config.boost_dir
            return

        if self.env.platform != 'win':
            # Assume we'll use the system version
            return

        # Boost dependencies
        BZIP2_ARCHIVE = 'bzip2-1.0.6'
        url = 'http://www.bzip.org/1.0.6/{0}.tar.gz'.format(BZIP2_ARCHIVE)
        self._download(url, '00b516f4704d4a7cb50a1d97e6e8e15b')
        created_dir = self._extract(url, self.env.c.thirdparty_dir)
        
        ZLIB_ARCHIVE = 'zlib-1.2.8'
        url = 'http://zlib.net/zlib-1.2.8.tar.gz'
        self._download(url, '44d667c142d7cda120332623eab69f40')
        created_dir = self._extract(url, self.env.c.thirdparty_dir)

        url = 'http://switch.dl.sourceforge.net/project/boost/boost/1.42.0/boost_1_42_0.zip'
        self._download(url, 'ceb78ed309c867e49dc29f60be841b64')
        created_dir = self._extract(url, self.env.c.thirdparty_dir)

        self.boost_dir = join(self.env.c.thirdparty_dir, created_dir)
        self.boost_lib_dir = join(self.boost_dir, 'stage', 'lib')

        # Patch for zlib
        zlibjam = join(self.boost_dir, "libs/iostreams/build/Jamfile.v2") 
        s = open(zlibjam).read()
        s = s.replace('gzio', '')
        f = open(zlibjam, 'w')
        f.write(s)
        f.close()

        CURRENT_BOOST_BUILD_VER = 2
        boost_build_ver_path = join(
            self.env.c.thirdparty_dir, 'boost_build_ver.txt')
        try:
            boost_build_ver = int(open(boost_build_ver_path).read())
        except IOError:
            boost_build_ver = 0
        log.debug('Found current boost build version: %i', boost_build_ver)

        if (boost_build_ver >= CURRENT_BOOST_BUILD_VER and
            os.path.isdir(self.boost_lib_dir)):
            return

        log.info("Building Boost, this can take some times...")

        utils.call(
            join(self.boost_dir, 'bootstrap.bat'), cwd=self.boost_dir)

        args = [join(self.boost_dir, 'bjam.exe')]
        # TODO: have an option to specify the vs version.
        toolset = 'msvc-9.0'
        args.extend(
            'toolset={toolset} {x64} release debug link=static runtime-link=static '
            'threading=multi'.format(
                toolset=toolset, x64='address-model=64' if self.env.c.x64 else ''
            ).split(' '))
        args.extend(['--with-%s' % m for m in REQUIRED_BOOST_MODULES])
        args.append('-sBZIP2_SOURCE={}'.format(
            join(self.env.c.thirdparty_dir, BZIP2_ARCHIVE)))
        args.append('-sZLIB_SOURCE={}'.format(
            join(self.env.c.thirdparty_dir, ZLIB_ARCHIVE)))

        utils.call(args, cwd=self.boost_dir)
        open(boost_build_ver_path, 'wb').write(str(CURRENT_BOOST_BUILD_VER))

    def _install_libspatialite(self):
        if self.env.platform != 'win':
            return

        for filename, hash in LIBSPATIALITE_DLLS:
            url = 'http://www.gaia-gis.it/%s.zip' % filename
            self._download(url, hash)
            self._extract(url, self.env.c.thirdparty_dir)

    def update_path_for_libspatialite(self):
        self._install_libspatialite()

        spatialite_dirs = [filename.split('/')[-1] for filename, _ in LIBSPATIALITE_DLLS]
        dll_paths = []
        for d in spatialite_dirs:
            d = join(self.env.c.thirdparty_dir, d)
            if os.path.isdir(join(d, 'bin')):
                dll_paths.append(join(d, 'bin'))
            else:
                dll_paths.append(d)

        utils.append_paths_to_environment('PATH', dll_paths)

    def install_prerequisites(self):
        self._check_debian_build_packages()

        self._install_cmake()
        log.debug('Cmake path: %s', self.cmake_path)

        self._install_mysql()
        log.info('Mysql support: %s, dir: %s', self.with_mysql, self.mysql_dir)

        self._install_boost()
        self._install_libspatialite()

    def get_cmake_tool_path(self, tool):
        self._install_cmake()
        tool_path = tool + self.env.platform_exe_suffix
        if self.cmake_path:
            tool_path = join(self.cmake_path, tool_path)
        return tool_path

    def _generate_build_system(self):
        if self.config.clear_cmake_cache:
            utils.maybe_remove(join(self.env.env_path, 'CMakeCache.txt'))

        args = [self.get_cmake_tool_path('cmake'), self.env.source_path]

        # Use ccache on Linux if available
        if self.env.platform == 'lin' and utils.find_executable('ccache'):
            os.environ['CXX'] = 'ccache g++'
            os.environ['CC'] = 'ccache gcc'

        if self.env.platform == 'win':
            # TODO: This shouldn't be hardcoded.
            args.extend(['-G', 'Visual Studio 9 2008' + (' Win64' if self.env.c.x64 else '')])

        if self.with_mysql:
            args.append('-DWITH_MYSQL=1')
            if self.mysql_dir:
                os.environ['MYSQL_DIR'] = self.mysql_dir

        args.append('-DCMAKE_BUILD_TYPE=' + self.env.build_type)

        # TODO: maybe change optimization flags in debug mode:
        # -DCMAKE_CXX_FLAGS=-O0

        if self.config.prefix:
            args.append('-DCMAKE_INSTALL_PREFIX=' + self.config.prefix)
        if self.config.mysql_params:
            args.append('-DSYNTHESE_MYSQL_PARAMS=' + self.config.mysql_params)

        args.append('-DBOOST_VERSION=' + BOOST_VER)

        env = os.environ.copy()
        if self.boost_dir:
            env['BOOST_ROOT'] = self.boost_dir
        if self.boost_lib_dir:
            env['BOOST_LIBRARYDIR'] = self.boost_lib_dir

        # Enable subdirs
        args.append('-DWITH_TEST:BOOL=ON')
        if self.config.do_not_build_python:
            args.append('-DWITH_PACKAGES:BOOL=OFF')
            args.append('-DWITH_PROJECTS:BOOL=OFF')
            args.append('-DWITH_TOOLS:BOOL=OFF')
            args.append('-DWITH_UTILS:BOOL=OFF')
        else:
            args.append('-DWITH_PACKAGES:BOOL=ON')
            args.append('-DWITH_PROJECTS:BOOL=ON')
            args.append('-DWITH_TOOLS:BOOL=ON')
            args.append('-DWITH_UTILS:BOOL=ON')

        # TODO: check that Python Cygwin is not in the path?

        if not os.path.isdir(self.env.env_path):
            os.makedirs(self.env.env_path)
        utils.call(args, cwd=self.env.env_path, env=env)

        # Hack to disable incremental build on XP (it fails with:
        # LINK : fatal error LNK1210: exceeded internal ILK size limit; link with /INCREMENTAL:NO)
        # I didn't find a way to to this in CMakeLists.txt
        # (http://www.cmake.org/pipermail/cmake/2010-February/035174.html didn't work)
        if self.env.platform == 'win' and platform.release() == 'XP':
            cmake_cache = join(self.env.env_path, 'CMakeCache.txt')
            cache_content = open(cmake_cache).read()
            cache_content = cache_content.replace(
                'INCREMENTAL:YES', 'INCREMENTAL:NO')
            open(cmake_cache, 'wb').write(cache_content)

    def _build_make(self, build_only_project):
        utils.call(
            'make -j%i %s' % (
                self.env.c.parallel_build,
                build_only_project if build_only_project else ''),
            cwd=self.env.env_path,
            shell=True)

    def _run_devenv(self, build_project=None):
        # TODO: this should be extracted from system config
        default_vs_path = (os.environ['ProgramFiles'] +
            '\\Microsoft Visual Studio 9.0\\')

        utils.append_paths_to_environment('PATH', [
            default_vs_path + 'Common7\\IDE',
        ])
        if build_project:
            # TODO: this should be extracted from system config
            default_sdk_path = 'C:\\Program Files\\Microsoft SDKs\\Windows\\v6.0A'

            utils.append_paths_to_environment('PATH', [
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

        args = ['devenv.com', 'synthese3.sln']
        if build_project:
            args.extend([
                '/build', self.env.build_type, '/project', build_project])

        utils.call(
            args,
            cwd=self.env.env_path)

    def _build_vs(self, build_only_project):
        self._run_devenv(
            build_only_project if build_only_project else 'ALL_BUILD')

    def _run_tool_method(self, method, *args, **kwargs):
        tool_method = getattr(self, '{0}_{1}'.format(method, self.tool))
        tool_method(*args, **kwargs)

    def build(self):
        self.install_prerequisites()
        self._generate_build_system()
        if self.config.generate_only:
            return

        # Only used on Windows, where the build will fail if the daemon is
        # running and locking the executable.
        if (self.env.platform == 'win' and
            self.config.kill_daemons_when_building):
            utils.kill_processes('s3-server')

        self._run_tool_method('_build', self.config.build_only)

    def _install_make(self):
        utils.call(
            'make -j%i install' % self.env.c.parallel_build,
            cwd=self.env.env_path,
            shell=True)

    def _install_vs(self):
        self._run_devenv('INSTALL')

    def install(self):
        self._run_tool_method('_install')

    def ide(self):
        assert self.tool == 'vs', 'IDE only implemented for Visual Studio'
        tester = synthesepy.test.Tester(self.env)
        tester.update_environment_for_cpp_tests()

        self._run_devenv()

    def testshell(self):
        tester = synthesepy.test.Tester(self.env)
        tester.update_environment_for_cpp_tests()
        os.system("bash")

    def extract_version_as_int(self, version_string, regexp = r"(\d+).(\d+).(\d+)"):
        # Extract the version number
        match = re.match(regexp, version_string)
        if match and len(match.groups()) == 3:
            # Concat the 3 numbers
            return int(match.group(1))*10000 + int(match.group(2))*100 + int(match.group(3))
        else:
            log.info("Error, failed to extract the version number '" + version_string + "'")

builder = None


def get_builder(env):
    global builder
    if builder:
        return builder
    builder = Builder(env)
    return builder

def build(env, method):
    builder = get_builder(env)
    m = getattr(builder, method)
    m()

