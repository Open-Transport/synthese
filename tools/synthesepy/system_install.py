#    Synthese system installation.
#    @file system_install.py
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
import subprocess
import sys

import synthesepy.build

log = logging.getLogger(__name__)


def create_synthese_user():
    import pwd
    SYNTHESE_USER = 'synthese'
    try:
        pwd.getpwnam(SYNTHESE_USER)
        return
    except KeyError:
        pass

    log.info('Creating %s user' % SYNTHESE_USER)
    subprocess.check_call(
        'useradd --system -m %s' % SYNTHESE_USER, shell=True)


def install_dependencies(env):
    log.info('Installing dependencies...')
    try:
        debian_version = open('/etc/debian_version').read()
    except IOError:
        log.warn('Not a Debian based system, skipping dependency installation.'
            ' Synthese might not work')
        return
    major_version = int(debian_version.split('.')[0])
    if major_version != 6:
        log.warn('This Debian version is not supported, skipping dependency '
            'installation. Synthese might not work')
        return

    builder = synthesepy.build.get_builder(env)

    required_packages = 'apache2 supervisor'.split()

    # Boost
    boost_full_ver = synthesepy.build.BOOST_VER + '.0'
    required_packages.extend(
        ['libboost-{0}{1}'.format(m.replace('_', '-'), boost_full_ver) for
            m in synthesepy.build.REQUIRED_BOOST_MODULES])

    # MySQL
    required_packages.extend(['libmysqlclient16', 'libcurl3'])

    # WSGI
    required_packages.extend(['libapache2-mod-wsgi'])

    # spatialite-bin
    # TODO: we should use our own build of spatialite, however it currently
    # has some issues with some of the db_xxx commands.
    required_packages.extend(['spatialite-bin'])

    builder.check_debian_package_requirements(
        required_packages, do_install=True)

    # supervisor doesn't seem to be started automatically.
    log.info('Starting supervisor')
    try:
        # TODO: hide error message.
        subprocess.check_call(
            '/etc/init.d/supervisor start', shell=True)
    except:
        pass

def run(env, args):
    if sys.platform == 'win':
        raise Exception('Windows is not supported')

    if os.environ.get('SYNTHESE_SKIP_ROOT_CHECK') != '1' and os.geteuid() != 0:
        raise Exception('This script should be run as root')

    create_synthese_user()
    install_dependencies(env)
    log.info('Synthese was installed or updated on the system.')
    log.info('To restart all your Synthese projects, type:')
    log.info('  supervisorctl restart all')

# TODO: system_uninstall
