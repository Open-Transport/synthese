#    synthesepy command line interface.
#    @file cli.py
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


import argparse
import logging
import os
from os.path import join
import shutil
import sys
import time

import synthesepy
import synthesepy.build
import synthesepy.checker
import synthesepy.config
import synthesepy.continuous_integration
import synthesepy.daemon
import synthesepy.dashboard
import synthesepy.db_backends
import synthesepy.doxygen
import synthesepy.env
import synthesepy.package
import synthesepy.proxy
import synthesepy.sqlite_to_mysql
import synthesepy.system_install
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

def doxygen(args, env):
    synthesepy.doxygen.run(env, args)

def runtests(args, env):
    tester = synthesepy.test.Tester(env)
    tester.run_tests(args.suites)


def sqlite_to_mysql(args, env):
    synthesepy.sqlite_to_mysql.convert(env, args.sourceconn, args.targetconn)


def create_project(args, env):
    project_manager.create_project(
        env, args.path, args.system_packages, args.conn_string,
        overwrite=args.overwrite)


def continuous_integration(args, env):
    synthesepy.continuous_integration.run(env, args)


def system_install(args, env):
    synthesepy.system_install.run(env, args)
system_install.root_required = True


def package(args, env):
    synthesepy.package.run(env, args)


def checker(args, env):
    synthesepy.checker.run(env, args)


def dashboard(args, env):
    synthesepy.dashboard.run(env, args)


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

    add_parser('load_data')
    def load_local_data(project, args, env):
        project.load_local_data(args.overwrite)
    parser = add_parser('load_local_data', func=load_local_data)
    parser.add_argument('--overwrite', action='store_true', default=False,
        help='Overwrite existing pages.')
    add_parser('reset')
    add_parser('rundaemon', ('run', 'start'))
    add_parser('stopdaemon', ('stop',))
    add_parser('runproxy')
    add_parser('runwebapp')
    def project_command(project, args, env):
        project.project_command(args.args)
    parser = add_parser('project_command', ('pc',), project_command)
    parser.add_argument('--args', nargs='+', help='Project command args')
    add_parser('db_view')
    add_parser('db_view_gis')
    def db_shell(project, args, env):
        project.db_shell(sql=args.sql)
    parser = add_parser('db_shell', func=db_shell)
    parser.add_argument('sql', nargs='?', help='SQL string to execute')
    parser.set_defaults(func=db_shell)
    add_parser('db_dump')
    add_parser('db_open_dump', ('db_opendump',))
    add_parser('db_sync_to_files')
    def db_sync_from_files(project, args, env):
        project.db_sync_from_files(args.host, args.use_http)
    parser = add_parser('db_sync_from_files', func=db_sync_from_files)
    parser.add_argument('--host', help='Synthese host where to update pages '
        '(format: HOSTNAME:PORT)')
    parser.add_argument('--use-http', action='store_true', default=False,
        help='Write pages using HTTP interface')
    def db_sync(project, args, env):
        project.db_sync(args.host)
    parser = add_parser('db_sync', func=db_sync)
    parser.add_argument('--host', help='Synthese host where to update pages')
    def db_restore(project, args, env):
        project.db_restore(args.db_dump)
    parser = add_parser('db_restore', func=db_restore)
    parser.add_argument('--dump', dest='db_dump')
    add_parser('db_remote_dump')
    add_parser('db_remote_restore')
    add_parser('ssh')
    def imports(project, args, env):
        project.imports(
            args.subcommand, args.template_id, args.import_id, args.dummy,
            args.no_mail, args.args)
    parser = add_parser('imports', func=imports)
    parser.add_argument('--cmd', dest='subcommand')
    parser.add_argument('--template-id')
    parser.add_argument('--import-id')
    parser.add_argument('--dummy', action='store_true', default=False)
    parser.add_argument('--no-mail', action='store_true', default=False)
    parser.add_argument('--args')
    add_parser('system_install_prepare')
    add_parser('system_install')
    add_parser('system_uninstall')
    add_parser('bgstart')
    add_parser('bgstop')
    def root_delegate(project, args, env):
        project.root_delegate(args.subcommand, args.args)
    parser = add_parser('root_delegate', func=root_delegate)
    parser.add_argument('subcommand', help='Sub-command')
    parser.add_argument(
        'args', nargs='*',
        help='Command arguments')
    add_parser('update_synthese')
    add_parser('update_project')
    def deploy(project, args, env):
        project.deploy(args.no_mail)
    parser = add_parser('deploy', func=deploy)
    parser.add_argument('--no-mail', action='store_true', default=False)
    add_parser('deploy_remote_prepare')
    add_parser('deploy_remote_restore')
    add_parser('ineo_install_triggers')


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
        help='Path to MySQL installation (Not needed on Linux if using '
             'standard MySQL installation)')
    parser_build.add_argument(
        '--boost-dir',
        help='Path to Boost installation (Not needed on Linux if using '
             'standard Boost installation)')
    parser_build.add_argument(
        '--parallel-build', type=int,
        help='Number of build threads to use')
    parser_build.add_argument(
        '--kill-daemons-when-building', action='store_true',
        help='Kill all running daemons before building')
    parser_build.add_argument(
        '-o', '--build-only',
        help='Build only the specified project')
    parser_build.add_argument(
        '-d', '--clear-cache', action='store_true', dest='clear_cmake_cache',
        help='Clear the CMake cache before building.')

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

    parser_testshell = subparsers.add_parser(
        'testshell', help='Launch a new shell with a configured environment for '
        'running tests')
    parser_testshell.set_defaults(func=build)
    parser_testshell.set_defaults(build_func='testshell')

    parser_doxygen = subparsers.add_parser(
        'doxygen', help='Generate Doxygen documentation')
    parser_doxygen.set_defaults(func=doxygen)

    parser_runtests = subparsers.add_parser(
        'runtests', help='Run unit tests')
    parser_runtests.set_defaults(func=runtests)
    parser_runtests.add_argument(
        '--test-conn-strings', nargs='+',
        help='Database connection strings used for the tests')
    parser_runtests.add_argument(
        '--no-init',
        help='Don\'t start/stop the daemon or initialize the db. '
             'Can be used to reuse an already running daemon '
             '(see --test-daemon-only)',
        action='store_true')
    parser_runtests.add_argument(
        '--test-daemon-only',
        help='When a HTTPTestCase is run, initialize the project and keep the '
             'daemon running. Meant to be used in parallel with --no-init',
        action='store_true')
    parser_runtests.add_argument(
        'suites', nargs='*',
        help='List of test suites to run. Choices: style, python, cpp')

    parser_sqlite_to_mysql = subparsers.add_parser(
        'sqlite_to_mysql', help='Convert a SQLite database to MySQL')
    parser_sqlite_to_mysql.set_defaults(func=sqlite_to_mysql)
    parser_sqlite_to_mysql.add_argument('--sourceconn',
        help='SQLite database input connection string '
        '(i.e. sqlite://path=/myproject/db/config.db3)')
    parser_sqlite_to_mysql.add_argument('--targetconn',
        help='MySQL database output connection string '
        '(i.e. mysql://host=localhost,user=synthese,passwd=synthese,db=synthese)')

    parser_create_project = subparsers.add_parser(
        'create_project', help='Create a Synthese project')
    parser_create_project.set_defaults(func=create_project)
    parser_create_project.add_argument('--path', required=True,
        help='Path of the project to create')
    parser_create_project.add_argument(
        '--system-packages', nargs='*',
        help='System packages to include in the created project')
    parser_create_project.add_argument('--conn-string',
        help='Database connection string of the created project')
    parser_create_project.add_argument(
        '--overwrite', action='store_true', default=False,
        help='Erase destination if it already exists')

    parser_continuous_integration = subparsers.add_parser(
        'continuous_integration', aliases=('ci',),
        help='Run the continuous integration commands')
    parser_continuous_integration.set_defaults(func=continuous_integration)
    parser_continuous_integration.add_argument(
        '--no-clean-if-build-fails', action='store_true',
        help='Don\'t clean the build directory and build again in case of '
        'build failure')
    parser_continuous_integration.add_argument(
        '--no-package-overwrite', action='store_true',
        help='Don\'t overwrite existing package.')
    parser_continuous_integration.add_argument(
        '--force-create-package', action='store_true',
        help='Force package creation.')
    parser_continuous_integration.add_argument(
        '--no-tests', action='store_true',
        help='Don\'t run unit tests.')
    parser_continuous_integration.add_argument(
        '--do-not-build-python', action='store_true',
        help='Don\'t build python env, kiosk, tools, utils, projects, packages.')

    parser_continuous_integration = subparsers.add_parser(
        'system_install',
        help='Install Synthese on the system (should be run from the installed'
        ' location)')
    parser_continuous_integration.set_defaults(func=system_install)

    parser_package = subparsers.add_parser(
        'package',
        help='Create a Synthese package and save it to the configured location.')
    parser_package.set_defaults(func=package)
    parser_package.add_argument(
        '--no-package-overwrite', action='store_true',
        help='Don\'t overwrite existing package.')

    parser_checker = subparsers.add_parser(
        'checker',
        help='Synthese checker interface for finding regressions')
    parser_checker.set_defaults(func=checker)
    parser_checker.add_argument(
        '--debug', action='store_true',
        help='Run in debug mode')

    parser_dashboard = subparsers.add_parser(
        'dashboard', aliases=('d',),
        help='Run the Synthese Dashboard Web server for managing projects')
    parser_dashboard.set_defaults(func=dashboard)
    parser_dashboard.add_argument(
        '--debug', action='store_true',
        help='Run in debug mode')


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
        level=(logging.DEBUG if '-v' in sys.argv else logging.INFO),
        format='%(asctime)s:%(levelname)s:%(name)s:%(message)s')

    config = synthesepy.config.Config()
    if config_args.config_path:
        config.config_path = config_args.config_path

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
        '-u', '--dummy', action='store_true',
        help='Dummy mode, doesn\'t execute commands or have side effects '
            'WARNING: NOT FULLY IMPLEMENTED YET')
    # Environment options
    parser.add_argument(
        '-t', '--env-type',
        choices=['cmake', 'installed'])
    parser.add_argument('-b', '--env-path', help='Env path')
    parser.add_argument(
        '-m', '--mode', choices=['debug', 'release', 'relwithdebinfo'])
    parser.add_argument(
        '--beep', dest='beep_when_done', action='store_true',
        help='Emit a beep on completion')
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
    parser.add_argument(
        '--no-root-check', action='store_true',
        help='Disable root user checks')
    parser.add_argument(
        '-n', '--env-config-names',
        help='Environment configs to use (comma separated)')

    subparsers = parser.add_subparsers(help='sub-command help')
    if project:
        add_project_subparsers(subparsers)
    else:
        add_default_subparsers(subparsers)

    args = parser.parse_args(remaining_argv)

    log.debug('Final arguments: %s', args)
    logging.getLogger().setLevel(
        level=(logging.DEBUG if args.verbose else logging.INFO))

    config.update_from_dict(args.__dict__)

    config.wsgi_proxy = not args.no_proxy
    config.wsgi_proxy_port = config.port + 1

    env = synthesepy.env.create_env(
        args.env_type, args.env_path, args.mode, config)

    config.update_finished(env)

    try:
        if project:
            project.set_env(env)
            if hasattr(args, 'project_func'):
                res = args.project_func(project)
            else:
                res = args.func(project, args, env)
            if isinstance(res, project_manager.CommandsResult):
                commands_result = res
                if config.verbose:
                    log.debug('Commands Result summary: %s',
                        commands_result.summary())
                if not commands_result.success:
                    raise Exception('Failure while executing: %s' % commands_result.title)
        else:
            root_required = getattr(args.func, 'root_required', False)
            if not args.no_root_check and env.platform != 'win':
                is_root = os.geteuid() == 0
                if root_required and not is_root:
                    raise Exception('You must run this command as root')
                if not root_required and is_root:
                    raise Exception('You can\'t run this command as root')
            args.func(args, env)
    finally:
        if config.beep_when_done:
            if env.platform == 'win':
                import winsound
                winsound.Beep(7000, 600)
            else:
                print '\a'

if __name__ == '__main__':
    main()
