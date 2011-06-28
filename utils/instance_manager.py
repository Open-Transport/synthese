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
import gzip
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

thisdir = os.path.abspath(os.path.dirname(__file__))
sys.path.append(os.path.join(thisdir, os.pardir, 'tools', 'synthesepy'))
import utils

log = logging.getLogger(__name__)

DEFAULT_CONFIG = {
    'SPATIALITE_PATH': None,
    'BUILDS': {
        'trunk_debug': {
        },
    },
    'ENV': {},
    'SSH_GLOBAL_OPTS': '',
    'MYSQL_HOST': '',
    'MYSQL_ROOT_PW': '',
    'MYSQL_SYNTHESE_PW': '',
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
    'DB_BACKEND': 'sqlite',
    'DB_NAME': 'synthese',
    'DB_USER': 'synthese',
    'DB_PW': '',
    'MYSQLDUMP_OPTS': '',
    'BUILD': 'trunk_debug',
    'SSH_OPTS': '',
    'RSYNC_OPTS': '',
    'RSYNC_USER': '',
    'DEFAULT_SITE': 'default',
    'SITES': [{
        'NAME': 'default',
        'SITE_ID': -1,
        'REMOTE_PATH': '/var/www',
    }],
}

options = None
instance = None
config = {}
build_config = {}
instance_config = {}
base_path = None
instance_path = None
is_sqlite = False
is_mysql = False
db_path = None
db_file = None


# Utils

def call(cmd, shell=True, **kwargs):
    global options
    log.debug('Running command: %r', cmd)
    if 'input' in kwargs:
        log.debug('With %s bytes of input', len(kwargs['input']))

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
    if 'input' in kwargs:
        input = kwargs['input']
        del kwargs['input']
        p = subprocess.Popen(
            cmd, shell=shell, stdin=subprocess.PIPE, stdout=subprocess.PIPE)
        output = p.communicate(input)[0]
        return output

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
    global options, config, build_config, instance_config, is_sqlite, is_mysql
    config = DEFAULT_CONFIG.copy()
    # Symbols exported for the config to use.
    config['base_path'] = base_path
    config['to_cygwin_path'] = _to_cygwin_path

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
        is_sqlite = instance_config.DB_BACKEND == 'sqlite'
        is_mysql = instance_config.DB_BACKEND == 'mysql'
    log.debug('Build config:\n %s', pprint.pformat(build_config.__dict__))


def prepare_environment():
    # Add MySQL tools to the path
    if sys.platform != 'win32':
        return

    thirdparty_dir = os.environ.get(
        'SYNTHESE_THIRDPARTY_DIR', os.path.expanduser('~/.synthese'))

    # Keep this in sync with tools/synthesepy/build.py
    utils.append_paths_to_environment('PATH', [
        os.path.join(thirdparty_dir, 'mysql-5.5.12-win32', 'bin')])


def _run_synthesepy(command, global_args=[], command_args=[]):
    if is_sqlite:
        dbconn = 'sqlite://debug=1,path=' + db_file
    elif is_mysql:
        dbconn = ('mysql://debug=1,user={user},'
            'passwd={pw},host={host},db={db}'.format(
                user='synthese', pw=config.MYSQL_SYNTHESE_PW,
                host=config.MYSQL_HOST, db='synthese_' + instance))
    else:
        assert False

    if instance_config:
        global_args.extend(
            ['--dbconn', dbconn,
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


def _ssh_command_line(with_server=True, extra_opts=''):
    return 'ssh {extra_opts} {ssh_global_opts} {ssh_opts} {server}'.format(
        extra_opts=extra_opts,
        ssh_global_opts=config.SSH_GLOBAL_OPTS,
        ssh_opts=instance_config.SSH_OPTS,
        server=instance_config.SERVER if with_server else '')


def _rsync(remote_path, local_path):
    call('rsync -avz --delete --delete-excluded '
        '-e {rsync_opts} "{ssh_command_line}" '
        '{server}:{remote_path} {local_path}'.format(
        rsync_opts=instance_config.RSYNC_OPTS,
        ssh_command_line=_ssh_command_line(False),
        server=instance_config.SERVER,
        remote_path=remote_path,
        local_path=_to_cygwin_path(local_path)))


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

def _mysql_command(command, user, pw, host, db='', port=3306, extra_opts='', input=''):
    cmd = '{command} {extra_opts} -u{user} -p{pw} -h{host} -P{port} {db}'.format(
        command=command, extra_opts=extra_opts, user=user, pw=pw, host=host,
        db=db, port=port
    )
    if input is not None:
        return call(cmd, input=input)
    else:
        call(cmd)

@cmd('Creates the local database (mysql only)')
def create_db():
    """
    This assumes you already have a 'synthese' user, which as a password
    of MYSQL_SYNTHESE_PW
    """
    if not is_mysql:
        raise Exception('This command is only for MySQL')

    create_sql = """
        CREATE DATABASE IF NOT EXISTS `synthese_{instance}` ;
        GRANT ALL PRIVILEGES ON `synthese_{instance}` . * TO 'synthese'@'%';
    """.format(instance=instance)

    _mysql_command(
        'mysql', 'root', config.MYSQL_ROOT_PW,
        config.MYSQL_HOST, input=create_sql)



@cmd('Initialize database (Warning: Destroys existing data!)')
def init_db():
    _run_synthesepy('initdb')


@cmd('Fetch database from server')
def fetch_db():
    dump_db(missing_db_fatal=False, save_uncompressed=False)
    if is_sqlite:
        log.info('Fetching db to %r', db_file)
        _rsync('/srv/data/s3-server/config.db3', db_file)
    elif is_mysql:

        # With MySQL, this is done in two steps: dump the remove database first
        # and then import it locally.
        sql_file = dump_db(
            save_uncompressed=False, from_remote=True)

        restore_db(sql_file)


@cmd('Dump database to text file')
def dump_db(missing_db_fatal=True, save_uncompressed=True, from_remote=False):
    if is_sqlite:
        if not os.path.isfile(db_file):
            if not missing_db_fatal:
                log.info('Db not there, not dumping')
                return
            raise Exception('Db %r is not there, can\'t dump' % db_file)
        args = [config.SPATIALITE_PATH, '-bail', db_file, '.dump']
        log.debug('Running: %r', args)
        output = call(args, input='')

        # Remove the Spatialite header, which isn't valid SQL.
        # (-noheader doesn't have any effect)
        output = output[output.index('BEGIN TRANSACTION'):]

    elif is_mysql:
        MYSQL_FORWARDED_PORT = 33000
        p = None

        if from_remote:
            # Open a tunnel connection to the server.

            p = subprocess.Popen(
                _ssh_command_line(
                    extra_opts='-N -L {forwarded_port}:localhost:3306'.format(
                        forwarded_port=MYSQL_FORWARDED_PORT)))

            args = dict(
                user=instance_config.DB_USER, pw=instance_config.DB_PW,
                host='localhost', port=MYSQL_FORWARDED_PORT,
                db=instance_config.DB_NAME)
        else:
            args = dict(
                user='synthese', pw=config.MYSQL_SYNTHESE_PW,
                host=config.MYSQL_HOST, port=3306, db='synthese_' + instance)

        args['extra_opts'] = instance_config.MYSQLDUMP_OPTS
        output = _mysql_command('mysqldump', **args)

        if p:
            p.kill()

    max_id = 0
    for d in os.listdir(db_path):
        if 'sql' not in d:
            continue
        try:
            max_id = max(max_id, int(d.split('-')[1]))
        except:
            pass

    target = join(
        db_path, 'config-{:03}-{date}.sql.gz'.format(
            max_id + 1, date=datetime.datetime.now().strftime('%Y%m%d-%H%M')))

    gzip.open(target, 'wb').write(output)
    log.info('Db dumped to %r', target)

    if save_uncompressed:
        uncompressed_target = join(
            db_path, 'config_{instance}.sql'.format(instance=instance))
        open(uncompressed_target, 'wb').write(output)

        if os.path.isfile(config.EDITOR_PATH):
            call([config.EDITOR_PATH, uncompressed_target], bg=True)

    return target


@cmd('Restore a database from a text file dump')
def restore_db(sql_file=None):
    if sql_file is None:
        all_dumps = sorted(d for d in os.listdir(db_path) if 'sql' in d)

        if not options.dump or options.dump == '-':
            log.fatal('Name of dump (-u) should be provided. '
                'Possible dumps:')
            for d in all_dumps:
                print d
            return
        dumps = [d for d in all_dumps if options.dump in d]
        if len(dumps) != 1:
            raise Exception('Not only one dump matches %r (possible dumps: %r)' %
                (dumps, all_dumps))

        sql_file = join(db_path, dumps[0])

    if sql_file.endswith('.gz'):
        sql = gzip.open(sql_file, 'rb').read()
    else:
        sql = open(sql_file, 'rb').read()

    if is_sqlite:
        if os.path.isfile(db_file):
            os.unlink(db_file)

        args = [config.SPATIALITE_PATH, '-bail', '-noheader', db_file]
        call(args, input=sql)
        log.info('Database %r restored from %r', db_file, sql_file)
    elif is_mysql:
        _mysql_command(
            'mysql', 'synthese', config.MYSQL_SYNTHESE_PW, config.MYSQL_HOST,
            'synthese_' + instance, input=sql)


@cmd('Open database in a GUI tool (if applicable')
def view_db():
    if is_sqlite:
        call([config.SPATIALITE_GUI_PATH, db_file], bg=True)
    # What about MySQL?


@cmd('Open a SQL interpreter on the database')
def shell_db():
    if is_sqlite:
        call([config.SPATIALITE_PATH, db_file])
    elif is_mysql:
        _mysql_command(
            'mysql', 'synthese', config.MYSQL_SYNTHESE_PW, config.MYSQL_HOST,
            'synthese_' + instance, input=None)


# Assets


@cmd('Fetch assets from server')
def fetch_assets():
    params = instance_config.__dict__.copy()
    for site in instance_config.SITES:
        _rsync(site['REMOTE_PATH'], join(instance_path, 'assets', site['NAME']))


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
    call(_ssh_command_line())


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
    parser.add_option('-u', '--dump',
        help='Name (or part of name) of the database dump to restore '
              '(use "-" to get the list)')
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
    prepare_environment()

    if instance:
        instance_path = join(base_path, instance)
        maybe_mkdir(instance_path)
        db_path = join(instance_path, 'db')
        db_file = join(db_path, 'config.db3')
        maybe_mkdir(db_path)

    for command in cmdline_commands:
        if not command in commands:
            raise Exception('Unknown command: %r' % command)
        if commands[command]['requ_instance'] and not instance:
            raise Exception('Command %r requires an instance' % command)

    for command in cmdline_commands:
        commands[command]['fn']()
