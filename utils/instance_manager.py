#!/usr/bin/env python
#
#    Script to manage several Synthese instances.
#    @file instance_manager.py
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


import datetime
import logging
from optparse import OptionParser
import os
from os.path import join
import pprint
import socket
import shutil
import subprocess
import sys

log = logging.getLogger(__name__)

DEFAULT_CONFIG = {
    'SPATIALITE_PATH': None,
    'BUILDS': {
        'trunk_debug': {
        },
    },
    'ENV': {},
}

BUILD_DEFAULT_CONFIG = {
    'SOURCE_DIR': None,
    'BUILD_DIR': None,
    'TOOL': 'cmake',
    'MODE': 'debug',
}

INSTANCE_DEFAULT_CONFIG = {
    'BUILD': 'trunk_debug',
    'SITE_ID': -1,
}

thisdir = os.path.abspath(os.path.dirname(__file__))
instance = None
config = {}
build_config = {}
instance_config = {}
base_path = None
instance_path = None
db_path = None


def call(cmd, shell=True, **kwargs):
    log.debug('Running command: %r', cmd)
    if 'bg' in kwargs:
        del kwargs['bg']
        subprocess.Popen(cmd, **kwargs)
        return
    env = os.environ.copy()
    env.update(config.ENV)
    kwargs['env'] = env
    subprocess.check_call(cmd, shell=shell, **kwargs)


def maybe_mkdir(path):
    if os.path.isdir(path):
        return
    os.makedirs(path)


def load_config(options):
    global config, build_config, instance_config
    config = DEFAULT_CONFIG.copy()

    for suffix in ['', '_local', '_local_' + socket.gethostname()]:
        config_path = join(
            base_path, 'conf', 'config{suffix}.py'.format(suffix=suffix))
        log.debug('Trying to read config file: %r', config_path)
        if os.path.isfile(config_path):
            log.debug('Reading config file: %r', config_path)
            execfile(config_path, {}, config)

    # Convert the dict to an object, which is a bit more convenient to handle.
    class Struct:
        def __init__(self, d):
            self.__dict__.update(d)

    config = Struct(config)
    if instance:
        instance_config = INSTANCE_DEFAULT_CONFIG.copy()
        instance_config.update(config.INSTANCES[instance])
        instance_config = Struct(instance_config)

    build = 'trunk_debug'
    if instance_config and instance_config.BUILD:
        build = instance_config.BUILD
    if options.build:
        build = options.build

    build_config = BUILD_DEFAULT_CONFIG.copy()
    build_config.update(config.BUILDS[build])
    build_config = Struct(build_config)

    # Fill defaults
    if not build_config.SOURCE_DIR:
        build_config.SOURCE_DIR = os.path.normpath(join(thisdir, os.pardir))

    if not build_config.BUILD_DIR:
        build_dir = 'build_cmake' if build_config.TOOL == 'cmake' else 'build'
        build_config.BUILD_DIR = join(
            build_config.SOURCE_DIR, build_dir, build_config.MODE)

    log.debug('Config:\n %s', pprint.pformat(config.__dict__))
    if instance_config:
        log.debug('Instance config:\n %s', pprint.pformat(instance_config.__dict__))
    log.debug('Build config:\n %s', pprint.pformat(build_config.__dict__))


def _run_synthesepy(command, global_args=[], command_args=[]):
    # TODO: mysql

    if instance_config:
        global_args.extend(
            ['--dbconn',
                'sqlite://debug=1,path=' + join(db_path, 'config.db3'),
             '--site-id', str(instance_config.SITE_ID),
             '--port', str(instance_config.PORT)])

    args = ([
        sys.executable,
        join(build_config.SOURCE_DIR, 'tools', 'synthese.py'),
        '-b', build_config.BUILD_DIR,
        '-m', build_config.MODE, '-t', build_config.TOOL, '-s', '-v'] +
        global_args + [command] + command_args)

    log.debug("running Synthese: %r\n%s", args, " ".join(args))
    call(args, shell=False)


def initdb():
    _run_synthesepy('initdb')


def fetchdb():
    target = 'config-{date}.db3'.format(
        date=datetime.datetime.now().strftime('%Y%m%d-%H%M'))
    log.info('Fetching db to %r', join(db_path, target))
    call('rsync -avz {server}:/srv/data/s3-server/config.db3 {target}'.format(
        server=instance_config.SERVER, target=target), cwd=db_path)
    shutil.copy(join(db_path, target), join(db_path, 'config.db3'))
    log.info('Copying to %r', join(db_path, 'config.db3'))


def viewdb():
    call([config.SPATIALITE_GUI_PATH, join(db_path, 'config.db3')], bg=True)


def dumpdb():
    args = [config.SPATIALITE_PATH, join(db_path, 'config.db3'), '.dump']
    log.debug('Running: %r', args)
    p = subprocess.Popen(args, stdout=subprocess.PIPE)
    output = p.communicate()[0]
    outfile = join(db_path, 'config_{instance}.dump'.format(instance=instance))
    open(outfile, 'wb').write(output)
    log.info('Db dumped to %r', outfile)
    if config.EDITOR_PATH:
        call([config.EDITOR_PATH, outfile], bg=True)


def shelldb():
    call([config.SPATIALITE_PATH, join(db_path, 'config.db3')])


def run():
    _run_synthesepy('rundaemon')


def stop():
    _run_synthesepy('stopdaemon')


def ssh():
    call(['ssh', instance_config.SERVER])


# General commands:

def build():
    _run_synthesepy('build')


if __name__ == '__main__':
    if sys.platform == 'cygwin':
        raise Exception('Cygwin Python unsupported')

    usage = ('usage: %prog [options] INSTANCES_PATH {INSTANCE_NAME COMMAND | '
        'GENERAL_COMMAND}')
    parser = OptionParser(usage=usage)

    parser.add_option('-v', '--verbose', action='store_true',
         default=False, help='Print debug logging')
    parser.add_option('-b', '--build', help='Build to use')

    (options, args) = parser.parse_args()
    if len(args) != 3 and len(args) != 2:
        parser.print_help()
        sys.exit(1)

    logging.basicConfig(level=(logging.DEBUG if options.verbose else
                               logging.INFO))

    if len(args) == 3:
        config_path, instance, command = args
    else:
        config_path, command = args

    base_path = os.path.abspath(config_path)
    assert os.path.isdir(base_path)
    if instance:
        instance_path = join(base_path, instance)
        maybe_mkdir(instance_path)
        db_path = join(instance_path, 'db')
        maybe_mkdir(db_path)

    load_config(options)

    command_fn = locals().get(command)
    if not command_fn:
        raise Exception('Unknown command: %r' % command)
    command_fn()
