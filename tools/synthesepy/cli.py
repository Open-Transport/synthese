#    synthesepy command line interface.
#    @file cli.py
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


import argparse
import logging
import os
from os.path import join
import shutil
import sys
import time

import synthesepy
import synthesepy.build
import synthesepy.config
import synthesepy.continuous_integration
import synthesepy.daemon
import synthesepy.db_backends
import synthesepy.env
import synthesepy.proxy
import synthesepy.sqlite_to_mysql
import synthesepy.test
import synthesepy.utils
import synthesepy.env
from synthesepy import project_manager


log = logging.getLogger(__name__)


# From Adrian Sampson, https://gist.github.com/471779
class AliasedSubParsersAction(argparse._SubParsersAction):

    class _AliasedPseudoAction(argparse.Action):
        def __init__(self, name, aliases, help):
            dest = name
            if aliases:
                dest += ' (%s)' % ','.join(aliases)
            sup = super(AliasedSubParsersAction._AliasedPseudoAction, self)
            sup.__init__(option_strings=[], dest=dest, help=help) 

    def add_parser(self, name, **kwargs):
        if 'aliases' in kwargs:
            aliases = kwargs['aliases']
            del kwargs['aliases']
        else:
            aliases = []

        parser = super(AliasedSubParsersAction, self).add_parser(name, **kwargs)

        # Make the aliases work.
        for alias in aliases:
            self._name_parser_map[alias] = parser
        # Make the help text reflect them, first removing old help entry.
        if 'help' in kwargs:
            help = kwargs.pop('help')
            self._choices_actions.pop()
            pseudo_action = self._AliasedPseudoAction(name, aliases, help)
            self._choices_actions.append(pseudo_action)

        return parser

# Commands


def build(args, env):
    synthesepy.build.build(env, args.build_func)


def runtests(args, env):
    tester = synthesepy.test.Tester(env)
    tester.run_tests(args.suites)


def sqlite_to_mysql(args, env):
    synthesepy.sqlite_to_mysql.convert(env, args.sourceconn, args.targetconn)


def create_project(args, env):
    site_packages = None
    if args.packages:
        site_packages = {
            'admin': ('admin',),
            'main': args.packages,
        }
    project_manager.create_project(
        env, args.path, site_packages, args.conn_string,
        overwrite=args.overwrite)


def continuous_integration(args, env):
    synthesepy.continuous_integration.run(env, args)


# End of commands


def add_project_subparsers(subparsers):
    def add_parser(name, aliases=[], func=None):
        project_method = getattr(project_manager.Project, name)
        parser = subparsers.add_parser(
            name,
            aliases=aliases,
            help=project_method.__doc__)
        if func:
            parser.set_defaults(func=func)
        else:
            parser.set_defaults(project_func=project_method)
        return parser

    add_parser('reset')
    add_parser('sync')
    add_parser('rundaemon', ('run', 'start'))
    add_parser('stopdaemon', ('stop',))
    add_parser('runproxy')
    add_parser('db_view')
    def db_shell(project, args, env):
        project.db_shell(sql=args.sql)
    parser = add_parser('db_shell', func=db_shell)
    parser.add_argument('sql', nargs='?', help='SQL string to execute')
    parser.set_defaults(func=db_shell)
    add_parser('db_dump')
    add_parser('db_open_dump', ('db_opendump',))
    add_parser('db_sync_to_files')
    def db_sync_from_files(project, args, env):
        project.db_sync_from_files(args.host)
    parser = add_parser('db_sync_from_files', func=db_sync_from_files)
    parser.add_argument('--host', help='Synthese host where to update pages')
    def db_sync(project, args, env):
        project.db_sync(args.host)
    parser = add_parser('db_sync', func=db_sync)
    parser.add_argument('--host', help='Synthese host where to update pages')
    def db_restore(project, args, env):
        project.db_restore(args.db_dump)
    parser = add_parser('db_restore', func=db_restore)
    parser.add_argument('--dump', dest='db_dump')
    add_parser('db_remote_dump')
    add_parser('ssh')


def add_default_subparsers(subparsers):
    parser_build = subparsers.add_parser('build', help='Build Synthese')
    parser_build.set_defaults(func=build)
    parser_build.set_defaults(build_func='build')
    parser_build.add_argument(
        '-g', '--generate-only', action='store_true',
        help='Only generate build script, but don\'t build')
    parser_build.add_argument(
        '--prefix',
        help='Installation directory')
    parser_build.add_argument(
        '--mysql-params',
        help='MySQL connection string used for the unit tests. For instance:'
            '"host=localhost,user=synthese,passwd=synthese"')
    parser_build.add_argument(
        '--without-mysql', action='store_true',
        help='Disable MySQL database support')
    parser_build.add_argument(
        '--mysql-dir',
        help='Path to MySQL installation (Not needed on Linux if using'
             'standard MySQL installation)')
    parser_build.add_argument(
        '--boost-dir',
        help='Path to Boost installation (Not needed on Linux if using'
             'standard Boost installation)')
    parser_build.add_argument(
        '--parallel-build', type=int,
        help='Number of build threads to use')
    parser_build.add_argument(
        '--kill-daemons-when-building', action='store_true',
        help='Kill all running daemons before building')

    parser_clean = subparsers.add_parser(
        'clean', help='Delete the object directory')
    parser_clean.set_defaults(func=build)
    parser_clean.set_defaults(build_func='clean')

    parser_install = subparsers.add_parser(
        'install', help='Install Synthese')
    parser_install.set_defaults(func=build)
    parser_install.set_defaults(build_func='install')

    parser_ide = subparsers.add_parser(
        'ide', help='Launch the IDE with a configured environment for '
        'running tests')
    parser_ide.set_defaults(func=build)
    parser_ide.set_defaults(build_func='ide')

    parser_runtests = subparsers.add_parser(
        'runtests', help='Run unit tests')
    parser_runtests.set_defaults(func=runtests)
    parser_runtests.add_argument(
        '--test-conn-strings', nargs='+',
        help='Database connection strings used for the tests')
    parser_runtests.add_argument(
        '--no-init',
        help='Don\'t start/stop the daemon or initialize the db. '
             'Can be used to reuse an already running daemon',
        action='store_true')
    parser_runtests.add_argument(
        'suites', nargs='*',
        help='List of test suites to run. Choices: style, python, cpp')

    parser_sqlite_to_mysql = subparsers.add_parser(
        'sqlite_to_mysql', help='Convert a SQLite database to MySQL')
    parser_sqlite_to_mysql.set_defaults(func=sqlite_to_mysql)
    parser_sqlite_to_mysql.add_argument('--sourceconn')
    parser_sqlite_to_mysql.add_argument('--targetconn')

    parser_create_project = subparsers.add_parser(
        'create_project', help='Create a Synthese project')
    parser_create_project.set_defaults(func=create_project)
    parser_create_project.add_argument('--path', required=True)
    parser_create_project.add_argument(
        '--packages', nargs='*')
    parser_create_project.add_argument('--conn-string')
    parser_create_project.add_argument(
        '--overwrite', action='store_true', default=False)

    parser_continuous_integration = subparsers.add_parser(
        'continuous_integration', aliases=('ci',),
        help='Run the continuous integration commands')
    parser_continuous_integration.set_defaults(func=continuous_integration)
    parser_continuous_integration.add_argument(
        '--no-clean-if-build-fails', action='store_true',
        help='Don\'t clean the build directory and build again in case of '
        'build failure')


def main():

    # Phase 1: Initialize config from files.

    config_parser = argparse.ArgumentParser(add_help=False)
    config_parser.add_argument(
        '--config-path',
        help='Directory containing the configuration files')
    config_parser.add_argument(
        '-c', '--config', dest='config_names', default='',
        help='Configuration entries to use')

    config_args, remaining_argv = config_parser.parse_known_args()

    # Hack to get logging before argument parsing is done.
    logging.basicConfig(
        level=(logging.DEBUG if '-v' in sys.argv else logging.INFO))

    config = synthesepy.config.Config()

    config.update_from_files(
        [c for c in config_args.config_names.split(',') if c], 
        config_args.config_path)

    # Phase 2: Process project settings.

    project_parser = argparse.ArgumentParser(add_help=False)
    project_parser.set_defaults(**config.__dict__)
    project_parser.add_argument(
        '-p', '--project-path', help='Path to the synthese project to use')
    args, remaining_argv = project_parser.parse_known_args(remaining_argv)

    project = None

    if args.project_path:
        log.info('Got a project path: %s', args.project_path)

        project = project_manager.Project(args.project_path, config=config)
        log.debug('New config: %s', config)

        # config from files overrides project config.
        if config_args.config_names:
            config.update_from_files(
                config_args.config_names.split(','), config_args.config_path)


    # Phase 3: Handle all command line options.

    parser = argparse.ArgumentParser(
        parents=[config_parser, project_parser],
        description='Synthese management tool')
    parser.register('action', 'parsers', AliasedSubParsersAction)
    parser.set_defaults(**config.__dict__)

    # Global options
    parser.add_argument(
        '-v', '--verbose', action='store_true', default=False,
        help='Be verbose')
    parser.add_argument(
        '-u', '--dummy', action='store_true', default=False,
        help='Dummy mode, doesn\'t execute commands or have side effects '
            'WARNING: NOT FULLY IMPLEMENTED YET')
    # Environment options
    parser.add_argument(
        '-t', '--env-type', choices=['scons', 'cmake', 'installed'])
    parser.add_argument('-b', '--env-path', help='Env path')
    parser.add_argument(
        '-m', '--mode', choices=['release', 'debug'])
    # Daemon options
    parser.add_argument('--port', type=int)
    parser.add_argument('--no-proxy', action='store_true')
    parser.add_argument('--site-id', type=int)
    parser.add_argument('-d', '--dbconn', dest='conn_string')
    parser.add_argument('-l', '--log-level', help='Daemon log level')
    parser.add_argument(
        '-s', '--stdout', action='store_true', dest='log_stdout',
        help='Log daemon output to stdout')
    parser.add_argument(
        '--static-dir',
        help='Directory containing static files served by the HTTP proxy')
    parser.add_argument(
        '-e', '--extra-params',
        help='Daemon extra parameters, using format '
             '"param0=value0 param1=value1"')
    parser.add_argument(
        '--gdb', action='store_true', help='Run daemon under gdb')
    parser.add_argument(
        '--restart-if-crashed', action='store_true',
        help='Automatically restart the daemon if it crashes')

    subparsers = parser.add_subparsers(help='sub-command help')
    if project:
        add_project_subparsers(subparsers)
    else:
        add_default_subparsers(subparsers)

    args = parser.parse_args(remaining_argv)

    log.debug('Final arguments: %s', args)
    logging.getLogger().setLevel(
        level=(logging.DEBUG if args.verbose else logging.INFO))

    config.update_from_obj(args)

    config.update_finished()

    config.wsgi_proxy = not args.no_proxy
    config.wsgi_proxy_port = config.port + 1

    env = synthesepy.env.create_env(
        args.env_type, args.env_path, args.mode, config)

    if project:
        project.set_env(env)
        if hasattr(args, 'project_func'):
            args.project_func(project)
        else:
            args.func(project, args, env)
    else:
        args.func(args, env)


if __name__ == '__main__':
    main()
