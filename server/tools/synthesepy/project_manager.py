#    Scripts to manage Synthese projects.
#    @file project_manager.py
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
import contextlib
import datetime
import glob
import gzip
import json
import logging
import os
from os.path import join
import pprint
import re
import shutil
import socket
import subprocess
import sys
import threading
import time
import traceback

import tempita
from werkzeug import wsgi
from werkzeug.serving import run_simple

from synthesepy.config import Config
from synthesepy.apache import Apache
from synthesepy import daemon
from synthesepy import db_backends
from synthesepy import db_sync
from synthesepy import deploy
from synthesepy import external_tools
from synthesepy import imports
from synthesepy import migration
from synthesepy import proxy
from synthesepy import utils


log = logging.getLogger(__name__)


class Package(object):
    def __init__(self, project, path):
        self.project = project
        self.path = path
        self.name = os.path.split(path)[1]
        self.dependencies = []
        self.base_page_index = -1

        if not os.path.isdir(self.path):
            raise Exception('Path %r is not a directory for a package' % self.path)

        self.files_path = join(self.path, 'files')
        self.files_local_path = join(self.path, 'files_local')

        package_config = join(self.path, 'config.py')
        if os.path.isfile(package_config):
            execfile(package_config, {}, self.__dict__)

    def __repr__(self):
        return '<Package %s %s>' % (self.name, self.path)

    def _load_fixtures(self, site):
        fixtures_files = (glob.glob(join(self.path, '*.sql')) +
            glob.glob(join(self.path, '*.sql.gz')) +
            glob.glob(join(self.path, '*.importer')))

        for fixtures_file in fixtures_files:
            if fixtures_file.endswith(('.sql', '.sql.gz')):
                vars = {}
                if site:
                    vars['site_id'] = site.id
                self.project.db_backend.import_fixtures(fixtures_file, vars)
            elif fixtures_file.endswith('.importer'):
                self.project._run_testdata_importer()

    def _load_pages(self, site, local, overwrite):
        pages_dir = join(self.path, 'pages_local' if local else 'pages')
        if not os.path.isdir(pages_dir):
            return

        WEB_PAGES_TABLE_ID = 63
        SITES_TABLE_ID = 25
        # Maybe 127 would have been better for this.
        SHARED_PAGES_SITE_LOCAL_ID = 100
        SHARED_PAGES_SITE_ID = utils.encode_uid(
            SITES_TABLE_ID, SHARED_PAGES_SITE_LOCAL_ID)

        # id structure:
        # The lower 32bits of the 128bits page id are available for page numbers.
        # 0xNN00 0000, bits 30-24: package id
        # 0x00NN 0000, bits 23-16: site id
        # 0x0000 NNNN, bits 0-15: pages
        #
        # Pages above 0x1000 0000 are reserved for new pages,
        # Pages with package id == 0 are reserved for existing pages.
        # Available packages: 126
        # Available sites: 255
        # Available pages per package+site: 65536

        assert self.base_page_index >= 0, \
            'Package %s is missing a base_page_index value' % self

        site_id = site.id if (site and local) else SHARED_PAGES_SITE_ID

        def pid(local_id, package_name=None):
            page_id = local_id

            if package_name:
                package = site.get_package(package_name)
            else:
                package = self

            assert package.base_page_index + 1 <= 127
            page_id |= (1 + package.base_page_index) << 24

            local_site_id = site_id & 0xffffffff
            if not local:
                # special case for shared page site.
                local_site_id = 0
            assert local_site_id <= 0xff
            page_id |= local_site_id << 16

            return utils.encode_uid(WEB_PAGES_TABLE_ID, page_id)

        pages_config = {}
        execfile(join(pages_dir, 'pages.py'), {
            'pid': pid,
        }, pages_config)

        # TODO: implement smart_url lookup on some attributes (up_id,...) or in page content.

        log.debug('pages_config:\n%s', pprint.pformat(pages_config))
        for page in pages_config['pages']:
            if not 'site_id' in page:
                page['site_id'] = site_id
            if not 'do_not_use_template' in page:
                page['do_not_use_template'] = True
            if page['content1'].startswith('file:'):
                file_path = page['content1'][len('file:'):]
                page['content1'] = unicode(
                    open(join(pages_dir, file_path), 'rb').read(), 'utf-8')
            if ('title' not in page and
                page.get('smart_url_path', '').startswith(':')):
                page['title'] = page['smart_url_path'][1:]

            if not overwrite and self.project.db_backend.query(
                    'select 1 from t063_web_pages where id = ?', [page['id']]):
                log.debug('Page %r already exists. Not overwriting.', page['id'])
                continue

            self.project.db_backend.replace_into('t063_web_pages', page)

    def _load_local_files(self, site, overwrite):
        if not site:
            return
        main_package = site.get_package('main')
        if not main_package:
            log.warn('Site %s is missing a "main" package, not copying '
                'local files' % site)
            return

        target_dir = join(main_package.files_path, 'local')
        log.debug('Copying local files from %r to %r',
            self.files_local_path, target_dir)
        utils.copy_over(self.files_local_path, target_dir, overwrite)

    def load_data(self, site, local, overwrite):
        if not local:
            self._load_fixtures(site)
        self._load_pages(site, local, overwrite)
        if local:
            self._load_local_files(site, overwrite)


class PackagesLoader(object):
    def __init__(self, project):
        self.project = project
        system_packages_dir = join(project.env.source_path, 'packages')
        self.system_packages = self._load_from_dir(system_packages_dir)
        # _compute_dependencies call here is just for sanity check.
        # They shouldn't depend on anything else.
        self._compute_dependencies(self.system_packages)

    def _load_from_dir(self, packages_dir):
        IGNORED_DIRS = ('.svn', 'web_pages')

        packages = {}
        for package_path in glob.glob(join(packages_dir, '*')):
            if os.path.basename(package_path) in IGNORED_DIRS:
                continue
            if os.path.isfile(package_path):
                paths = [l.strip() for l in open(package_path) if l.strip()]
                if len(paths) != 1:
                    continue
                package_path = os.path.normpath(join(packages_dir, paths[0]))
            if not os.path.isdir(package_path):
                continue
            package = Package(self.project, package_path)
            if package.name in packages:
                raise Exception('Duplicate package names')
            packages[package.name] = package

        log.debug('Loaded packages %r from %r', packages.keys(), packages_dir)
        return packages

    def _compute_dependencies(self, packages):
        to_visit = packages.keys()

        while True:
            if not to_visit:
                break
            package = packages[to_visit.pop(0)]
            for dep in package.dependencies:
                if dep in packages:
                    continue
                if dep not in self.system_packages:
                    raise Exception(
                        'Can\'t resolve dependency to package %r' % dep)
                packages[dep] = self.system_packages[dep]
                to_visit.append(dep)

        return packages

    def load_packages(self, system_package_names, packages_dir):
        packages = self._load_from_dir(packages_dir)

        all_system_package_names = set(self.system_packages.keys())

        if not set(system_package_names).issubset(all_system_package_names):
            wrong_system_packages = \
                set(system_package_names) - all_system_package_names
            raise Exception('Some system packages don\'t exist: %r' %
                wrong_system_packages)

        for system_package_name in system_package_names:
            packages[system_package_name] = \
                self.system_packages[system_package_name]

        packages = self._compute_dependencies(packages)

        log.debug('Loaded packages: %r', packages.keys())
        return packages.values()


class Site(object):
    def __init__(self, project, path):
        self.path = path

        self.name = os.path.split(path)[1]
        self.base_path = ''
        self.rewrite_rules = []
        self.root_page_id = -1
        self.htdocs_path = None
        self.generate_apache_compat_config = False
        self.system_packages = []

        site_config = join(self.path, 'config.py')
        if os.path.isfile(site_config):
            execfile(site_config, {}, self.__dict__)

        project_site_config = project.config.sites_config.get(self.name, {})
        self.__dict__.update(project_site_config)

        self.packages = project.packages_loader.load_packages(
            self.system_packages, self.path)

    def __repr__(self):
        return '<Site %s %s>' % (self.name, self.path)

    def get_package(self, package_name):
        for p in self.packages:
            if p.name == package_name:
                return p
        return None


class CommandResult(object):
    # TODO: Maybe simplify with just a success bool status.
    SUCCESS = 0
    IGNORED_FAILURE = 1
    FAILURE = 2

    TYPE_PROCESS = 0
    TYPE_METHOD = 1

    def __init__(self, type=TYPE_PROCESS, method_name=None):
        self.commandline = []
        self.status = self.SUCCESS
        self.output = ''
        self.type = type
        self.method_name = method_name

    @property
    def success(self):
        return self.status == self.SUCCESS

    def __str__(self):
        return '<CommandResult, commandline: {0}, output: {1}>'.format(
            self.commandline, self.output)

    @classmethod
    def call_project(cls, project, command, global_args=[], args=[], sudo=False):
        cmd = project.build_command_line(command, global_args, args, sudo=sudo)
        return cls.call_command(project, cmd)

    @classmethod
    def call_command(cls, project, cmd, hide_arg=None):
        command_result = CommandResult()
        command_result.commandline = cmd
        if hide_arg:
            command_result.commandline = \
                command_result.commandline.replace(hide_arg, '***')

        try:
            command_result.output = utils.call(
                cmd, cwd=project.path, ret_output=True)
            log.debug('Command output:\n%s',
                '\n'.join('\t' + l for l in command_result.output.splitlines()))
        except subprocess.CalledProcessError, e:
            command_result.output = e.output
            command_result.status = CommandResult.FAILURE

        return command_result

    @classmethod
    def call_method(cls, method, *args, **kwargs):
        command_result = CommandResult(
            type=cls.TYPE_METHOD, method_name=method.__name__)
        try:
            method(*args, **kwargs)
        except Exception, e:
            command_result.output = traceback.format_exc(e)
            command_result.status = CommandResult.FAILURE

        return command_result


class CommandsException(Exception):
    def __init__(self, command_result, commands_result):
        self.command_result = command_result
        self.commands_result = commands_result

    def __str__(self):
        return 'Commands Exception while running: %s' % self.commands_result


class CommandsResult(object):
    def __init__(self, title):
        self.title = title
        self.success = True
        self.command_results = []

    def __str__(self):
        return '<CommandsResult: title=%s, %s>' % (
            self.title, self.command_results)

    def add_command_result(self, command_result):
        self.command_results.append(command_result)
        if not command_result.success:
            self.success = False
            raise CommandsException(command_result, self)

    def summary(self):
        output = 'Result of {0}\n'.format(self.title)
        for command_result in self.command_results:
            output += '_' * 80 + '\n'
            if command_result.type == CommandResult.TYPE_PROCESS:
                output += 'Command line: {0}\n'.format(command_result.commandline)
            else:
                output += 'Method: {0}\n'.format(command_result.method_name)
            output += 'Success: {0}\n\n'.format(command_result.success)
            output += 'Result: {0}\n\n'.format(command_result.output)
        return output


def command(root_required=False, root_allowed=False):
    def _command(f):
        def wrapper(*args, **kwargs):
            project = args[0]
            if (not project.env.c.no_root_check and
                project.env.platform != 'win'):
               is_root = os.geteuid() == 0
               if root_required and not is_root:
                   raise Exception('You must run this command as root')
               if not root_allowed and not root_required and is_root:
                   raise Exception('You can\'t run this command as root')
            return f(*args, **kwargs)
        return wrapper
    return _command


def commands_result():
    def _command(f):
        def wrapper(*args, **kwargs):
            project = args[0]
            try:
                return f(*args, **kwargs)
            except CommandsException, e:
                project.admin_log.warn('Failure while running commands: %s' % e.commands_result.title)
                log.exception('Failure while running commands: %s', e)
                return e.commands_result
        return wrapper
    return _command


class Project(object):
    def __init__(self, path, env=None, config=None):
        self.path = os.path.normpath(os.path.abspath(path))
        if not os.path.isdir(self.path):
            raise Exception('No project can be found at %r' % self.path)
        self.env = env
        self.config = config
        if not config and env:
            self.config = env.config
        if not self.config:
            self.config = Config()
        self.htdocs_path = join(path, 'htdocs')
        self.daemon = None
        self._db_backend = None
        self.db_path = join(path, 'db')
        if not os.path.isdir(self.db_path):
            os.makedirs(self.db_path)
        self._deployer = None

        self.admin_log = logging.getLogger('synthesepy.admin')
        self.admin_log.setLevel(logging.DEBUG)
        self.admin_log_path = join(self.path, 'logs', 'admin.log')

        class KeptClosedFileHandler(logging.FileHandler):
            '''Extension of logging.FileHandler which tries to keep the log
            file closed, so that multiple processes can write to it.
            Concurrent access might have unpredictable results though'''
            def emit(self, record):
                logging.FileHandler.emit(self, record)
                self.close()

        # FIXME: older Pythons have an issue when overriding FileHandler
        if sys.version_info < (2, 6, 6):
            if not os.path.isfile(self.admin_log_path):
                utils.maybe_makedirs(os.path.dirname(self.admin_log_path))
                open(self.admin_log_path, 'wb').write('')
            admin_handler = logging.FileHandler(self.admin_log_path)
        else:
            admin_handler = KeptClosedFileHandler(
                self.admin_log_path, delay=True)

        admin_formatter = logging.Formatter(
            '%(asctime)s  %(levelname)-12s  %(message)s')
        admin_handler.setFormatter(admin_formatter)
        self.admin_log.addHandler(admin_handler)

        self._read_config()

        # env might not be available yet. set_env should be called once ready
        # to complete the project initialization.
        if self.env:
            self.set_env(self.env)

    def set_env(self, env):
        self.env = env
        self.packages_loader = PackagesLoader(self)
        self._load_sites()
        self._load_packages()
        self.daemon = daemon.Daemon(self.env, self)
        self.imports_manager = imports.ImportsManager(self)

        for env_config_name in self.config.env_config_names.split(','):
            if not env_config_name:
                continue
            if env_config_name not in self.config.env_configs:
                raise Exception('Invalid env_config_name %r. Possible ones are '
                    '%r' % (env_config_name, self.config.env_configs.keys()))
            self.config.update_from_dict(
                self.config.env_configs[env_config_name])

        if not self.config.remote_db_path:
            sep = '/' if self.config.remote_project_path.startswith('/') else '\\'
            self.config.remote_db_path = sep.join([
                self.config.remote_project_path, 'db', 'config.db3'])

        manager_path = join(self.path, 'manager')
        self.manager_module = None
        if (not os.path.isdir(manager_path) or
            not os.path.isfile(join(manager_path, 'main.py'))):
            return

        sys.path.append(manager_path)
        self.manager_module = __import__('main')
        log.debug('Manager module %r', self.manager_module)

    def get_site(self, site_name):
        for s in self.sites:
            if s.name == site_name:
                return s
        return None

    def _read_config(self):
        for suffix in ['', '_local', '_local_' + socket.gethostname()]:
            config_path = join(
                self.path, 'config{suffix}.py'.format(suffix=suffix))
            log.debug('Trying to read config file: %r', config_path)
            if not os.path.isfile(config_path):
                continue
            log.debug('Reading config file: %r', config_path)
            self.config.update_from_file(config_path)

        # Set a path to the sqlite db if not set explicitly.

        conn_info = db_backends.ConnectionInfo(self.config.conn_string)
        if conn_info.backend == 'sqlite' and 'path' not in conn_info:
            conn_info['path'] = join(self.path, 'db', 'config.db3')
            self.config.conn_string = conn_info.conn_string

        # Set defaults

        self.config.project_path = self.path
        if not self.config.project_name:
            self.config.project_name = os.path.split(self.path)[1]
        if not self.config.log_file:
            self.config.log_file = join(self.path, 'logs', 'synthese.log')
        log_dir = os.path.dirname(self.config.log_file)
        if not os.path.isdir(log_dir):
            os.makedirs(log_dir)

        log.debug('Config: %s', self.config)

    def _load_sites(self):
        self.sites = []
        for site_path in sorted(glob.glob(join(self.path, 'sites', '*'))):
            if not os.path.isdir(site_path):
                continue
            self.sites.append(Site(self, site_path))
        log.debug('Found sites: %s', self.sites)

        self.config.static_paths = []
        admin_site = self.get_site('admin')
        admin_package = None
        if admin_site:
            admin_package = admin_site.get_package('admin')
        for site in self.sites:
            for package in site.packages + [admin_package]:
                if not package:
                    continue
                self.config.static_paths.append(
                    (site.base_path, package.files_path))

        if self.config.default_site:
            site_ids = [s.id for s in self.sites if
                s.name == self.config.default_site]
            if len(site_ids) != 1:
                raise Exception('Can\'t find site {0!r} (found: {1!r})'.format(
                    self.config.default_site, site_ids))
            self.config.site_id = site_ids[0]
        if self.config.site_id <= 0:
            non_admin_sites = [s for s in self.sites if s.name != 'admin']
            if len(non_admin_sites) > 0:
                self.config.site_id = non_admin_sites[0].id

    def _load_packages(self):
        self.packages = []
        packages_dir = join(self.path, 'packages')
        if not os.path.isdir(packages_dir):
            return
        self.packages = self.packages_loader.load_packages([], packages_dir)

    def _get_sites_and_packages(self):
        for package in self.packages:
            yield None, package
        for site in self.sites:
            for package in site.packages:
                yield site, package

    def _run_testdata_importer(self):
        importer_path = self.env.testdata_importer_path
        log.info('Runing testdata importer from %r', importer_path)
        self.env.prepare_for_launch()
        env = os.environ.copy()
        env['SYNTHESE_TESTDATA_CONNSTRING'] = self.config.conn_string + \
            ',triggerCheck=0'
	# FIXME: The testdata importer binary does not compile anymore we just skip its call.
        #utils.call(importer_path, env=env)

    @property
    def db_backend(self):
        if self._db_backend:
            return self._db_backend
        self._db_backend = db_backends.create_backend(
            self.env, self.config.conn_string)
        return self._db_backend

    def clean(self):
        self.db_backend.drop_db()

    @command()
    def load_data(self, local=False, overwrite=True):
        """Load data into the database."""
        log.info('loading_data into project (local:%s)', local)
        # TODO: don't import fixtures from a package more than once.
        for site, package in self._get_sites_and_packages():
            log.debug('Loading site:%s package:%s', site, package)
            package.load_data(site, local, overwrite)

    @command()
    def load_local_data(self, overwrite):
        """
        Load data into the database (data loaded only once meant to be edited)
        """
        self.load_data(True, overwrite)

    @command()
    def reset(self):
        """
        Delete database and load inital data.
        """
        log.info('Resetting project')
        self.clean()
        self.db_backend.init_db()
        self.load_data()
        self.load_local_data(True)

    def send_restart_mail(self, fail_kind, restart_count, last_start_s):
        if not self.config.send_mail_on_restart:
            return

        log.info('Sending restart mail')
        hostname = socket.gethostname()
        LINE_COUNT = 500
        try:
            last_log = utils.tail(open(self.config.log_file, 'rb'), LINE_COUNT)
        except IOError:
            last_log = "[Not available]"

        subject = ('Synthese {fail_kind} on {hostname} (project: {project}, '
            'restarts: {restart_count})'.format(
                fail_kind=fail_kind,
                hostname=hostname,
                project=self.config.project_name,
                restart_count=restart_count))
        body = '''
Detected Synthese {fail_kind} on {hostname}. It is going to restart.
Total restart count: {restart_count}. Seconds since last start: {uptime_s}.

Last {line_count} lines of log:
{last_log}

Have a nice day,
The synthese.py wrapper script.
'''.format(
            fail_kind=fail_kind,
            hostname=hostname,
            restart_count=restart_count,
            line_count=LINE_COUNT,
            last_log=last_log,
            uptime_s=int(time.time() - last_start_s))

        utils.send_mail(self.env.config, self.config.mail_admins, subject, body)

    @command()
    def rundaemon(self, block=True):
        """Run Synthese daemon"""

        migration.maybe_migrate_schema(self)

        self.daemon.start()
        if not block:
            return
        log.info('Daemon running, press ctrl-c to stop')
        if (self.manager_module and
            hasattr(self.manager_module, 'on_daemon_started')):
            self.manager_module.on_daemon_started(self)

        hang_detector_running = True
        hang_detected_event = threading.Event()

        def hang_detector():
            HANG_CHECK_INTERVAL_S = 60
            RESTART_HANG_COUNT = 2
            hangs_count = 0
            while hang_detector_running:
                if not self.daemon.ready:
                    log.debug('Hang detector: daemon not ready')
                    time.sleep(HANG_CHECK_INTERVAL_S)
                    hangs_count = 0
                    continue
                log.debug('Checking for hangs')
                if not utils.can_connect(self.config.port, False, '/hang_check'):
                    hangs_count += 1
                    log.warn('Detected hang. Hangs count is %s', hangs_count)
                else:
                    hangs_count = 0
                if hangs_count >= RESTART_HANG_COUNT:
                    hangs_count = 0
                    log.warn('Hang detected, setting hang detected event.')
                    hang_detected_event.set()
                time.sleep(HANG_CHECK_INTERVAL_S)

        restart_if_crashed = (self.config.restart_if_crashed_or_hung or
            self.config.restart_if_crashed)
        restart_if_hung = (self.config.restart_if_crashed_or_hung or
            self.config.restart_if_hung)

        if restart_if_hung:
            hang_detector_thread = threading.Thread(target=hang_detector)
            hang_detector_thread.daemon = True
            hang_detector_thread.start()

        restart_count = 0
        try:
            while True:
                restart = False
                start_time = time.time()
                while (self.daemon.is_running() and
                    not hang_detected_event.is_set()):
                    time.sleep(2)
                if hang_detected_event.is_set():
                    log.info('Hang detected')
                    hang_detected_event.clear()
                    fail_kind = 'hang'
                    self.daemon.stop()
                    restart = True
                else:
                    log.info('Daemon terminated')
                    crashed = not self.daemon.stopped
                    fail_kind = 'crash'
                    if crashed:
                        log.warn('Stop is unexpected, crash?')
                        restart_reason = 'Crash'
                        if restart_if_crashed:
                            restart = True

                if not restart:
                    sys.exit(1)
                    break

                log.info('Restarting daemon')
                self.send_restart_mail(fail_kind, restart_count, start_time)
                self.daemon.start(kill_proxy=False)
                restart_count += 1
        except:
            raise
        finally:
            log.info('Stopping daemon')
            hang_detector_running = False
            self.daemon.stop()

    @command()
    def stopdaemon(self):
        """Stop Synthese daemon"""
        self.daemon.stop()
        # TODO: should use the HTTP method to stop the daemon once it works.
        ports = [self.config.port]
        if self.config.wsgi_proxy:
            ports.append(self.config.wsgi_proxy_port)
        for port in ports:
            utils.kill_listening_processes(port)

    @command()
    def runproxy(self):
        """Run HTTP Proxy to serve static files"""
        proxy.serve_forever(self.env, self)

    # This is mostly for debugging. The webapp is also available with "runproxy"
    # (however it doesn't auto-reload on change).
    @command()
    def runwebapp(self):
        """Run Web Frontend"""
        # import here to avoid import cycles.
        import synthesepy.web

        web_app = synthesepy.web.get_application(project=self)

        def root_app(environ, start_response):
            status = '200 OK'
            output = 'Dummy root app'
            response_headers = [('Content-type', 'text/plain'),
                                ('Content-Length', str(len(output)))]
            start_response(status, response_headers)
            return [output]

        app = wsgi.DispatcherMiddleware(root_app, {
            '/w': web_app
        })
        run_simple('0.0.0.0', 5000, app, use_reloader=True)

    @command()
    def project_command(self, args):
        if not self.manager_module:
            raise Exception('No manager module')
        self.manager_module.project_command(self, args)

    @command()
    def db_view(self):
        """Open database in a GUI tool (if applicable)"""
        if self.db_backend.name == 'sqlite':
            utils.call(
                [self.config.spatialite_gui_path,
                    self.db_backend.conn_info['path']],
                bg=True)
        else:
            raise NotImplementedError("Not implemented for this backend")

    @command()
    def db_view_gis(self):
        """Open database in a GIS GUI tool (if applicable)"""
        if self.db_backend.name == 'sqlite':
            utils.call(
                [self.config.spatialite_gis_path,
                    self.db_backend.conn_info['path']],
                bg=True)
        else:
            raise NotImplementedError("Not implemented for this backend")

    @command()
    def db_shell(self, sql=None):
        """Open a SQL interpreter on the database or execute the given SQL"""
        self.db_backend.shell(sql)

    @command()
    def db_dump(self, db_backend=None, prefix=''):
        """Dump database to text file"""

        if not db_backend:
            db_backend = self.db_backend
        output = db_backend.dump()

        max_id = 0
        for d in os.listdir(self.db_path):
            if 'sql' not in d:
                continue
            try:
                max_id = max(max_id, int(d.split('-')[1]))
            except:
                pass

        target = join(
            self.db_path, '{prefix}config-{id:03}-{date}.sql.gz'.format(
                prefix=prefix, id=max_id + 1,
                date=datetime.datetime.now().strftime('%Y%m%d-%H%M')))

        gzip.open(target, 'wb').write(output)
        log.info('Db dumped to %r', target)

        uncompressed_fname = 'config_{project_name}.sql'.format(
                project_name=self.config.project_name)
        uncompressed_target = join(self.db_path, uncompressed_fname)
        open(uncompressed_target, 'wb').write(output)
        return uncompressed_fname

    @command()
    def db_open_dump(self):
        """Open the latest database dump in a text editor"""

        uncompressed_target = join(
            self.db_path, 'config_{project_name}.sql'.format(
                project_name=self.config.project_name))

        if os.path.isfile(self.config.editor_path):
            utils.call([self.config.editor_path, uncompressed_target], bg=True)

    @command()
    def db_restore(self, db_dump):
        """Restore a database from a text file dump"""
        all_dumps = sorted(d for d in os.listdir(self.db_path) if 'sql' in d)

        if not db_dump or db_dump == '-':
            log.fatal('Name of dump (--dump) should be provided. '
                'Possible dumps:')
            for d in all_dumps:
                print d
            return
        dumps = [d for d in all_dumps if db_dump in d]
        if len(dumps) != 1:
            raise Exception('Not only one dump matches %r (possible dumps: %r)' %
                (db_dump, all_dumps))

        sql_file = join(self.db_path, dumps[0])
        log.info('Restoring %s', sql_file)

        if sql_file.endswith('.gz'):
            sql = gzip.open(sql_file, 'rb').read()
        else:
            sql = open(sql_file, 'rb').read()

        log.info('Restoring %s bytes of sql', len(sql))
        self.db_backend.restore(sql)

    @command()
    def db_sync_to_files(self):
        db_sync.sync_to_files(self)

    @command()
    def db_sync_from_files(self, host, use_http):
        db_sync.sync_from_files(self, host, use_http)

    @command()
    def db_sync(self, host=None):
        db_sync.sync(self, host)

    # Commands for syncing or managing a remote project.

    @command()
    def db_remote_dump(self):
        """Dump database from remote server"""

        if not self.config.remote_server:
            raise Exception('No remote server defined in configuration')

        @contextlib.contextmanager
        def remote_transaction_sqlite(conn_info):
            remote_db_local_path = join(self.db_path, 'remote_config.db3')

            log.info('Fetching db to %r', remote_db_local_path)
            utils.rsync(self.config, '{remote_server}:' + self.config.remote_db_path,
                utils.to_cygwin_path(remote_db_local_path))

            remote_conn_info = self.db_backend.conn_info.copy()
            remote_conn_info['path'] = remote_db_local_path

            yield remote_conn_info

        @contextlib.contextmanager
        def remote_transaction_mysql(conn_info):
            MYSQL_FORWARDED_PORT = 33000

            utils.kill_listening_processes(MYSQL_FORWARDED_PORT)

            if utils.can_connect(MYSQL_FORWARDED_PORT):
                raise Exception('MySQL tunnel port (%s) is not '
                    'available' % MYSQL_FORWARDED_PORT)

            ssh_command_line = utils.ssh_command_line(
                self.config,
                extra_opts='-N -L {forwarded_port}:localhost:3306'.format(
                    forwarded_port=MYSQL_FORWARDED_PORT))
            log.debug('ssh command line for tunnel: %s', ssh_command_line)
            p = subprocess.Popen(ssh_command_line, shell=True)
            log.info('Waiting a bit for the tunnel to establish...')
            time.sleep(3)

            remote_conn_info = db_backends.ConnectionInfo(
                self.config.remote_conn_string)
            remote_conn_info.data['port'] = MYSQL_FORWARDED_PORT
            remote_conn_info.data['host'] = '127.0.0.1'

            yield remote_conn_info
            p.kill()

        if self.db_backend.name == 'sqlite':
            remote_transaction = remote_transaction_sqlite
        elif self.db_backend.name == 'mysql':
            remote_transaction = remote_transaction_mysql
        else:
            raise Exception('Unsupported backend: %r', self.db_backend.name)

        with remote_transaction(self.db_backend.conn_info.copy()) as remote_conn_info:
            remote_conn_string = remote_conn_info.conn_string
            log.info('Remote connection string: %r', remote_conn_string)
            remote_backend = db_backends.create_backend(self.env, remote_conn_string)
            return self.db_dump(remote_backend, self.config.remote_dump_prefix)

    @command()
    def db_remote_restore(self):
        sql_fname = self.db_remote_dump()
        self.db_restore(sql_fname)

    @command()
    def ssh(self):
        """Open a ssh shell on the remote server"""
        utils.call(utils.ssh_command_line(self.config))

    @command()
    def imports(self, subcommand, template_id, import_id, dummy, no_mail, args):
        """Imports management"""
        if subcommand == 'list_templates':
            import_templates = self.imports_manager.get_import_templates()
            for import_template in import_templates:
                log.info('Import template: id=%s label=%r', import_template.id,
                    import_template.label)
                for import_ in import_template.get_imports():
                    log.info('Import: %s, path=%s', import_.id, import_.path)
        elif subcommand == 'list_imports':
            import_template = self.imports_manager.get_import_template(
                template_id)
            for import_ in import_template.get_imports():
                log.info('Import: %s, path=%s', import_.id, import_.path)
        elif subcommand == 'create':
            import_template = self.imports_manager.get_import_template(
                template_id)
            create_args = {}
            if args:
                create_args = json.loads(args)
            import_ = import_template.create_import(create_args)
            log.info('Created import with id: %s', import_.id)
        elif subcommand == 'execute':
            import_ = self.imports_manager.get_import(template_id, import_id)
            import_.execute(None, dummy, no_mail)
        else:
            raise Exception('Unknown import subcommand: %s', subcommand)

    # System install/uninstall

    def _get_tools(self):
        tools = [
            external_tools.SyntheseSupervisor(self),
            Apache(self),
            external_tools.WSGI(self),
        ]
        if self.config.use_udf_proxy:
            tools.append(external_tools.UDFProxySupervisor(self))
        if self.config.use_s3_proxy:
            tools.append(external_tools.S3ProxySupervisor(self))

        return tools

    def _process_dot_in_file(self, content, vars, template_dir):
        def get_template(name, from_template):
            path = join(template_dir, name)
            return from_template.__class__.from_filename(
                path, namespace=from_template.namespace,
                get_template=from_template.get_template)

        tmpl = tempita.Template(content, get_template=get_template)
        return tmpl.substitute(vars)

    def _replace_dot_in_files(self):
        for path, dirlist, filelist in os.walk(self.path):
            for exclude in ['db', 'logs', 'imports', 'deploy', '.git', '.hg', '.svn']:
                if exclude in dirlist:
                    dirlist.remove(exclude)
            for name in filelist:
                if not name.endswith('.in'):
                    continue
                source = join(path, name)
                target = source[:-len('.in')]

                log.debug('Generating file: %r -> %r', source, target)

                with open(target, 'wb') as f:
                    content = open(source).read()
                    vars = {
                        'generated_warning': 'WARNING: THIS FILE IS GENERATED, '
                            'DO NOT EDIT!!',
                        'project': self,
                        'p': self,
                        'config': self.config,
                        'c': self.config,
                    }
                    content = self._process_dot_in_file(content, vars, path)
                    f.write(content)
                try:
                    shutil.copymode(source, target)
                except OSError, e:
                    # This might happen if the user is not the owner.
                    log.warn('Error while updating mode: %s', e)

    @command()
    def system_install_prepare(self):
        tools = self._get_tools()

        for tool in tools:
            tool.generate_config()

        self._replace_dot_in_files()

        return tools

    @command(root_required=True)
    def system_install(self):
        if self.env.platform == 'win':
            raise Exception('Windows is not supported')

        import pwd
        os.setegid(pwd.getpwnam(self.config.synthese_user).pw_gid)
        os.seteuid(pwd.getpwnam(self.config.synthese_group).pw_uid)

        tools = self.system_install_prepare()

        os.seteuid(0)
        os.setegid(0)

        for tool in tools:
            tool.system_install()

        log.info('Project installed on the system. You can start it with '
            'the command (as root):\n'
            '  supervisorctl start synthese-{project_name}\n'
            'or restart it (as root):\n'
            '  supervisorctl restart synthese-{project_name}\n'.format(
            project_name=self.config.project_name))

    @command(root_required=True)
    def system_uninstall(self):
        tools = self._get_tools()

        for tool in tools:
            tool.system_uninstall()

        log.info('Project uninstalled. You should stop the daemon with '
            'this command (as root):\n  supervisorctl stop synthese-%s',
            self.config.project_name)

    # Manager commands

    def build_command_line(self, command, global_args=[], args=[], sudo=False,
            remote=False):
        cmd = []
        if sudo:
            cmd.append('/usr/bin/sudo')

        # TODO: implement a better way to detect the remote OS
        remote_os = 'lin'
        if self.env.platform == 'win' and self.config.remote_server == 'localhost':
            remote_os = 'win'

        if remote_os == 'win':
            # This assumes Cygwin ssh is running (might work if not though).
            cmd.extend(['cmd', '/c', sys.executable])
        else:
            # Python path is hardcoded in order to match the sudo rule.
            cmd.append('/usr/bin/python')
        cmd.append(self.env.synthesepy_path)
        if self.config.verbose:
            cmd.append('-v')
        cmd.append('--env-type=' + self.env.type)
        cmd.append('--env-path=' + self.env.env_path)
        cmd.append('--mode=' + self.env.mode)
        project_path = self.config.remote_project_path if remote else self.path
        cmd.append('--project-path=' + project_path)
        cmd.extend(global_args)

        cmd.append(command)
        cmd.extend(args)
        cmd = [arg.replace('\\', '\\\\') for arg in cmd]
        return cmd

    def call_project(self, command, global_args=[], args=[], sudo=False):
        cmd = self.build_command_line(command, global_args, args, sudo)
        utils.call(cmd, cwd=self.path)

    @command(root_required=True)
    def root_delegate(self, command, args=[]):
        if command == 'update_synthese':
            install_url = args[0]

            # Security check: we don't want a compromised synthese user be
            # able to execute any remote script.
            REQUIRED_URL_RE = '^http://ci.rcsmobility.com/~build/[\w/\.]+$'
            if not re.match(REQUIRED_URL_RE, install_url):
                raise Exception('The install url must match the regexp %r' %
                    REQUIRED_URL_RE)

            if self.env.config.dummy:
                return
            utils.call('curl -s {0} | python'.format(install_url))
        elif command in ('start_supervisor', 'stop_supervisor'):
            supervisor_command = 'start' if command == 'start_supervisor' else 'stop'
            utils.call([
                self.config.supervisorctl_path, supervisor_command,
                'synthese-{0}'.format(self.config.project_name.replace('_', '-'))])
        elif command in ('start_initd', 'stop_initd'):
            initd_command = 'start' if command == 'start_initd' else 'stop'
            utils.call(['/etc/init.d/s3-server', initd_command])
        else:
            raise Exception('Unknown command %r' % command)

    @command(root_allowed=True)
    def bgstart(self):
        if self.env.platform == 'win' and self.config.bg_process_manager != 'python':
            raise Exception('Only bg_process_manager == python is supported '
                'on Windows.')

        log.info('Starting Synthese in the background')
        if self.config.bg_process_manager == 'python':
            if self.env.platform == 'lin' and os.geteuid() == 0:
                raise Exception('You can\'t run this command as root')

            cmd = self.build_command_line('rundaemon', ['-s'])
            log.debug('Synthese command line: %s', ' '.join(cmd))
            stdout = open(self.env.c.log_file, 'wb')

            subprocess.Popen(cmd,
                stderr=subprocess.STDOUT,
                stdout=stdout)
        elif self.config.bg_process_manager == 'supervisor':
            self.call_project('root_delegate', args=['start_supervisor'], sudo=True)
        elif self.config.bg_process_manager == 'initd':
            self.call_project('root_delegate', args=['start_initd'], sudo=True)
        elif self.config.bg_process_manager == 'dummy':
            log.info('Dummy bg_process_manager. Doing nothing.')
        else:
            raise Exception('Unknown bg_process_manager %r' %
                self.config.bg_process_manager)

    @command(root_allowed=True)
    def bgstop(self):
        if self.env.platform == 'win' and self.config.bg_process_manager != 'python':
            raise Exception('Only bg_process_manager == python is supported '
                'on Windows.')

        log.info('Stopping Synthese in the background')
        if self.config.bg_process_manager == 'python':
            self.stopdaemon()
        elif self.config.bg_process_manager == 'supervisor':
            self.call_project('root_delegate', args=['stop_supervisor'], sudo=True)
        elif self.config.bg_process_manager == 'initd':
            self.call_project('root_delegate', args=['stop_initd'], sudo=True)
        elif self.config.bg_process_manager == 'dummy':
            log.info('Dummy bg_process_manager. Doing nothing.')
        else:
            raise Exception('Unknown bg_process_manager %r' %
                self.config.bg_process_manager)

    @command()
    @commands_result()
    def update_synthese(self, install_url=None):
        self.admin_log.info('update_synthese called')

        if install_url is None:
            install_url = 'http://ci.rcsmobility.com/~build/synthese/lin/release/trunk/latest/install_synthese.py'
        commands_result = CommandsResult('Update Synthese')

        commands_result.add_command_result(
            CommandResult.call_project(
                self, 'root_delegate',
                args=['update_synthese', install_url], sudo=True))

        return commands_result

    @command()
    @commands_result()
    def update_project(self, system_install=True, load_data=True, overwrite=False):
        self.admin_log.info('update_project called')

        commands_result = CommandsResult('Update Project')

        commands_result.add_command_result(
            CommandResult.call_project(self, 'bgstop'))

        if system_install:
            commands_result.add_command_result(
                CommandResult.call_project(
                    self, 'system_install', sudo=True))

        if load_data:
            commands_result.add_command_result(
                CommandResult.call_project(self, 'load_data'))
            load_local_data_args = []
            if overwrite:
                load_local_data_args = ['--overwrite']
            commands_result.add_command_result(
                CommandResult.call_project(
                    self, 'load_local_data', load_local_data_args))

        commands_result.add_command_result(
            CommandResult.call_project(self, 'bgstart'))

        return commands_result

    @command()
    @commands_result()
    def svn(self, command, username=None, password=None):
        commands_result = CommandsResult('Subversion ({0})'.format(command))

        cmd = 'svn %s --no-auth-cache' % command
        if username:
            cmd += ' --username=%s' % username
        if password:
            cmd += ' --password=%s' % password
        commands_result.add_command_result(
            CommandResult.call_command(self, cmd, hide_arg=password))

        return commands_result

    # Deploy

    @property
    def deployer(self):
        if self._deployer:
            return self._deployer
        self._deployer = deploy.Deployer(self)
        return self._deployer

    @command()
    @commands_result()
    def deploy(self, no_mail=False):
        return self.deployer.deploy(no_mail)

    @command()
    @commands_result()
    def deploy_restore(self, dump_id):
        return self.deployer.deploy_restore(dump_id)

    @command()
    @commands_result()
    def deploy_remote_prepare(self):
        return self.deployer.deploy_remote_prepare()

    @command()
    @commands_result()
    def deploy_remote_restore(self):
        return self.deployer.deploy_remote_restore()

    # Ineo realtime

    @command()
    def ineo_install_triggers(self):
        triggers_file = join(
            self.path, 'impex', 'ineo-triggers', 'triggers.sql')
        if not os.path.isfile(triggers_file + '.in'):
            raise Exception('Can\'t find triggers file at %r' % triggers_file)

        utils.maybe_remove(triggers_file)
        self._replace_dot_in_files()
        assert os.path.isfile(triggers_file)

        self.config.ineo_db.shell(open(triggers_file, 'rb').read())


def create_project(env, path, system_packages=None, conn_string=None,
        overwrite=False, initialize=True):
    log.info('Creating project in %r', path)
    if overwrite:
        utils.RemoveDirectory(path)
    if os.path.isdir(path):
        raise Exception('There is already a directory at %s' % path)

    template_path = join(env.source_path, 'projects', 'template')

    shutil.copytree(template_path, path)

    # Update config
    config_path = join(path, 'config.py')
    # Use default (sqlite) if empty or without a path.
    if conn_string in ('', 'sqlite://'):
        conn_string = None
    if conn_string:
        lines = open(config_path).readlines()
        for (index, line) in enumerate(lines):
            if line.startswith('#conn_string'):
                lines[index] = "conn_string = '{0}'".format(conn_string)
        open(config_path, 'wb').writelines(lines)

    if system_packages:
        www_site_config_path = join(path, 'sites', 'www', 'config.py')
        www_site_config = open(www_site_config_path).read()
        assert 'system_packages = ' not in www_site_config
        www_site_config += '\nsystem_packages = {0!r}\n'.format(system_packages)

        with open(www_site_config_path, 'wb') as f:
            f.write(www_site_config)

    project = Project(path, env=env)
    if initialize:
        project.reset()
    return project
