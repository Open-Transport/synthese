#    Synthese packaging.
#    @file package.py
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
import subprocess

import synthesepy.build
from synthesepy import utils

log = logging.getLogger(__name__)


def run(env, args):
    config = env.config
    if not config.prefix:
        raise Exception('Prefix is required.')

    svn_info = utils.SVNInfo(env.source_path)
    revision_path = 'r{0}'.format(svn_info.version)

    package_relative_dir = os.sep.join([
        env.platform, env.mode, svn_info.branch, revision_path])

    package_dir = join(config.packages_save_path, package_relative_dir)
    if os.path.isdir(package_dir):
        if config.no_package_overwrite:
            log.info('Package directory already exists (%r) not building.',
                package_dir)
            return

    try:
        os.makedirs(package_dir)
    except:
        raise Exception('Failed to create the '
            'directory in %r, cannot continue' % package_dir)

    log.info('Cleaning prefix %r', config.prefix)
    utils.RemoveDirectory(config.prefix)

    log.info('Installing Synthese to %r', config.prefix)
    builder = synthesepy.build.get_builder(env)
    builder.install()

    env_dir = os.path.join(config.prefix, 'share', 'synthese', 'env')
    if not os.path.isdir(env_dir):
        log.info('CMake install didn\'t create the environment '
            'directory in %r, we do not seal it' % env_dir)
    else:
        with open(join(env_dir, 'sealed.txt'), 'wb') as f:
            f.write('Environment sealed\n')

    # Archive
    ARCHIVE_NAME = 'synthese.tar.bz2'
    if config.prefix_with_svnrelease:
        ARCHIVE_NAME = 'synthese-relative-' +  revision_path + '.tar.bz2'
    archive_path = join(package_dir, ARCHIVE_NAME)

    log.info('Creating archive %r', archive_path)
    prefix_parent = os.path.dirname(config.prefix)
    prefix_tail = os.path.basename(config.prefix)
    utils.call([
        'tar', '-C',  utils.to_cygwin_path(prefix_parent),
        '-jcf', utils.to_cygwin_path(archive_path),
        '--owner=0', '--group=0',
        '--numeric-owner', '--mode=go-w', prefix_tail])

    log.info('Remove the build directory in %r' % config.prefix)
    utils.RemoveDirectory(config.prefix)

    # Deploy script

    deploy_script_path = join(package_dir, 'install_synthese.py')
    source_deploy_script = join(
        env.source_path, 'tools', 'synthesepy', 'install_synthese.py.in')
    deploy_script_content = open(source_deploy_script).read()

    archive_url = (config.packages_access_url + package_relative_dir +
        '/' + ARCHIVE_NAME)
    deploy_script_content = deploy_script_content.replace(
        '@@ARCHIVE_URL@@', archive_url).replace(
        '@@PREFIX@@', config.prefix)
    with open(deploy_script_path, 'wb') as f:
        f.write(deploy_script_content)
    log.debug('Deploy script written to %r', deploy_script_path)

    # TODO: remove old packages to avoid filling up the disk.

    # latest symlink
    if env.platform != 'win':
        link_name = join(package_dir, os.pardir, 'latest')
        if os.path.exists(link_name):
            os.unlink(link_name)
        os.symlink(revision_path, link_name)
