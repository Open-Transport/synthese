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
import optparse
import os
from os.path import join
import pprint
import socket
import shutil
import subprocess
import sys
import webbrowser

log = logging.getLogger(__name__)

DEFAULT_CONFIG = {
    'SPATIALITE_PATH': None,
    'BUILDS': {
        'trunk_debug': {
        },
    },
    'ENV': {},
    'SSH_OPTIONS': '',
}

BUILD_DEFAULT_CONFIG = {
    'ALIASES': [],
    'SYNTHESEPY_ARGS': ['-v', '-s', '-e', 'session_max_duration=999999'],
    'SOURCE_DIR': None,
    'BUILD_DIR': None,
    'TOOL': 'cmake',
    'MODE': 'debug',
}

INSTANCE_DEFAULT_CONFIG = {
    'BUILD': 'trunk_debug',
    'RSYNC_OPTS': '',
    'RSYNC_USER': '',
    'DEFAULT_SITE': 'default',
    'SITES': [{
        'NAME': 'default',
        'SITE_ID': -1,
        'REMOTE_PATH': '/var/www',
    }],
}

thisdir = os.path.abspath(os.path.dirname(__file__))
options = None
instance = None
config = {}
build_config = {}
instance_config = {}
base_path = None
instance_path = None
db_path = None


# Utils

def call(cmd, shell=True, **kwargs):
    global options
    log.debug('Running command: %r', cmd)
    if options.dummy:
        # TODO: show environment too.
        cmdline = cmd
        if isinstance(cmd, list):
            cmdline = ' '.join(cmdline)
        dir = ('(in directory: {0!r})'.format(kwargs['cwd']) if
            'cwd' in kwargs else '')
        log.info('Dummy mode, not running %s:\n%s', dir, cmdline)

        return
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

def _to_cygwin_path(path):
    cygproc = subprocess.Popen(
        ("cygpath", "-a", "-u", path), stdout=subprocess.PIPE)
    (stdout_content, stderr_content) = cygproc.communicate()
    return stdout_content.rstrip()



def load_config():
    global options, config, build_config, instance_config
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
        if not instance in config.INSTANCES:
            raise Exception(
                'Instance %r doesn\'t exist. Available instances: %r' %
                (instance, config.INSTANCES.keys()))
        instance_config.update(config.INSTANCES[instance])
        instance_config = Struct(instance_config)

    build = 'trunk_debug'
    if instance_config and instance_config.BUILD:
        build = instance_config.BUILD
    if options.build:
        build = options.build
        for build_name, conf in config.BUILDS.iteritems():
            for alias in conf.get('ALIASES', []):
                if alias == options.build:
                    build = build_name

    build_config = BUILD_DEFAULT_CONFIG.copy()
    if not build in config.BUILDS:
        raise Exception(
            'Build %r doesn\'t exist. Available builds: %r' %
            (instance, config.BUILDS.keys()))

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
             '--port', str(instance_config.PORT)])
    global_args.extend(build_config.SYNTHESEPY_ARGS)

    args = ([
        sys.executable,
        join(build_config.SOURCE_DIR, 'tools', 'synthese.py'),
        '-b', build_config.BUILD_DIR,
        '-m', build_config.MODE, '-t', build_config.TOOL] +
        global_args + [command] + command_args)

    log.debug("running Synthese: %r\n%s", args, " ".join(args))
    call(args, shell=False)

try:
    from collections import OrderedDict
    commands = OrderedDict()
except ImportError:
    commands = {}

class cmd(object):
    def __init__(self, help, requires_instance=True):
        self.help = help
        self.requ_instance = requires_instance
    def __call__(self, f):
        commands[f.__name__] = {
            'help': self.help,
            'requ_instance': self.requ_instance,
            'fn': f,
        }
        return f

# Database

@cmd('Initialize database (Warning: Destroys existing data!)')
def init_db():
    _run_synthesepy('initdb')


@cmd('Fetch database from server')
def fetch_db():
    # TODO: call dump_db to save existing database.
    # TODO: save to config.db3
    target = 'config-{date}.db3'.format(
        date=datetime.datetime.now().strftime('%Y%m%d-%H%M'))
    log.info('Fetching db to %r', join(db_path, target))
    call('rsync -avz {server}:/srv/data/s3-server/config.db3 {target}'.format(
        server=instance_config.SERVER, target=target), cwd=db_path)
    log.info('Copying to %r', join(db_path, 'config.db3'))
    shutil.copy(join(db_path, target), join(db_path, 'config.db3'))


@cmd('Dump database to text file')
def dump_db():
    # TODO: gzip dump.
    # TODO: launch an editor which can read .gzip file.
    args = [config.SPATIALITE_PATH, join(db_path, 'config.db3'), '.dump']
    log.debug('Running: %r', args)
    if options.dummy:
        log.info('Dummy mode, not executing:\n%s', ' '.join(args))
        return
    p = subprocess.Popen(args, stdout=subprocess.PIPE)
    output = p.communicate()[0]
    # Remove the Spatialite header, which isn't valid SQL.
    output = output[output.index('BEGIN TRANSACTION'):]
    target = join(
        db_path, 'config-{date}.dump'.format(
            date=datetime.datetime.now().strftime('%Y%m%d-%H%M')))

    open(target, 'wb').write(output)
    log.info('Db dumped to %r', target)

    final_target = join(
        db_path, 'config_{instance}.dump'.format(instance=instance))
    log.info('Copying to %r', final_target)
    shutil.copy(target, final_target)

    if os.path.isfile(config.EDITOR_PATH):
        call([config.EDITOR_PATH, final_target], bg=True)


@cmd('Restore a database from a text file dump')
def restore_db():
    # TODO
    pass


@cmd('Open database in spatialite-gui')
def view_db():
    call([config.SPATIALITE_GUI_PATH, join(db_path, 'config.db3')], bg=True)


@cmd('Open spatialite shell with database')
def shell_db():
    call([config.SPATIALITE_PATH, join(db_path, 'config.db3')])


# Assets


@cmd('Fetch assets from server')
def fetch_assets():
    params = instance_config.__dict__.copy()
    for site in instance_config.SITES:
        params['remote_path'] = site['REMOTE_PATH']
        params['assets_path'] = _to_cygwin_path(
            join(instance_path, 'assets', site['NAME']))
        call('rsync -avz {RSYNC_OPTS} --delete --delete-excluded '
            '{RSYNC_USER}{SERVER}:{remote_path} {assets_path}'.format(**params))


# Daemon


@cmd('Run Synthese daemon')
def run():
    site_name = options.site if options.site else instance_config.DEFAULT_SITE
    sites = [s for s in instance_config.SITES if s['NAME'] == site_name]
    if len(sites) != 1:
        raise Exception(
            'Unable to find site configuration for %r' % options.site)
    site = sites[0]
    global_args = [
        '--static-dir', join(instance_path, 'assets', site['NAME']),
        '--site-id', str(site['SITE_ID'])]

    _run_synthesepy('rundaemon', global_args)


@cmd('Stop Synthese daemon')
def stop():
    _run_synthesepy('stopdaemon')


# Misc


@cmd('Open a ssh shell on the server')
def ssh():
    call('ssh {options} {server}'.format(
        options=config.SSH_OPTIONS, server=instance_config.SERVER))


# General commands:

@cmd('Update Synthese', False)
def update():
    call('svn update', cwd=build_config.SOURCE_DIR)


@cmd('Build Synthese', False)
def build():
    _run_synthesepy('build')


@cmd('Run Synthese unit tests', False)
def test():
    # TODO: pass other arguments needed for tests.
    _run_synthesepy('runtests')


@cmd('Generate an HTML files with links to the Synthese instances', False)
def gen_html():
    output = join(base_path, 'synthese_instances.html')

    body = ""
    for instance_name, instance_config in config.INSTANCES.iteritems():
        body += "<h1>" + instance_name + "</h1>\n"
        body += "<pre>" + repr(instance_config) + "</pre>\n"

    open(output, 'wb').write("""
        <!DOCTYPE html>
        <head><title>Synthese Instances</title></head>
        <style> body {{ font: small Verdana; }} </style>
        <body>
            {body}
        </body>
    """.format(body=body))
    webbrowser.open(output)


if __name__ == '__main__':
    if sys.platform == 'cygwin':
        raise Exception('Cygwin Python unsupported')

    class PlainHelpFormatter(optparse.IndentedHelpFormatter):
        def format_description(self, description):
            if description:
                return description + "\n"
            else:
                return ""

    commands_list = ''
    for requ_instance in True, False:
        commands_list += (
            '\n Commands requiring an instance\n' if requ_instance else
            '\n  Commands not requiring an instance\n')
        for name, v in commands.iteritems():
            if v['requ_instance'] != requ_instance:
                continue
            commands_list += '{name:<20} {description}\n'.format(
                name=name, description=v['help'])

    parser = optparse.OptionParser(
        usage='usage: %prog [options] INSTANCES_PATH {INSTANCE_NAME,-} '
            'COMMAND1 [COMMAND2 ...]| ',
        formatter=PlainHelpFormatter(),
        description='List of commands:\n' + commands_list)

    parser.add_option('-v', '--verbose', action='store_true',
         default=False, help='Print debug logging')
    parser.add_option('-d', '--dummy', action='store_true',
         default=False, help='Don\'t run commands, Just print the command line')
    parser.add_option('-b', '--build', help='Build to use')
    parser.add_option('-s', '--site',
        help='Site identifier to use for static files')

    (options, args) = parser.parse_args()

    if len(args) < 3:
        parser.print_help()
        sys.exit(1)

    logging.basicConfig(level=(logging.DEBUG if options.verbose else
                               logging.INFO))

    config_path = args.pop(0)
    instance = args.pop(0)
    if instance == '-':
        instance = None
    cmdline_commands = args

    base_path = os.path.abspath(config_path)
    assert os.path.isdir(base_path)

    load_config()

    if instance:
        instance_path = join(base_path, instance)
        maybe_mkdir(instance_path)
        db_path = join(instance_path, 'db')
        maybe_mkdir(db_path)

    for command in cmdline_commands:
        if not command in commands:
            raise Exception('Unknown command: %r' % command)
        if commands[command]['requ_instance'] and not instance:
            raise Exception('Command %r requires an instance' % command)

    for command in cmdline_commands:
        commands[command]['fn']()
