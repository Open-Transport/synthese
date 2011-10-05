#    Scripts to manage Synthese projects.
#    @file project_manager.py
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
import contextlib
import datetime
import glob
import gzip
import logging
import os
from os.path import join
import shutil
import subprocess
import sys
import time

from synthesepy.config import Config
from synthesepy.apache import Apache
from synthesepy import daemon
from synthesepy import db_backends
from synthesepy import db_sync
from synthesepy import external_tools
from synthesepy import proxy
from synthesepy import utils


log = logging.getLogger(__name__)


# Utilities
# XXX maybe move to utils
def _copy_over(source_path, target_path):
    """Copy source_path over target_path, replacing any existing files"""

    base_parts_count = len(source_path.split(os.path.sep))
    for path, dirlist, filelist in os.walk(source_path):
        for exclude in ['.git', '.hg', '.svn']:
            if exclude in dirlist:
                dirlist.remove(exclude)
        for name in filelist:
            relative_path = os.sep.join(path.split(os.sep)[base_parts_count:])

            source = join(path, name)
            target = join(target_path, relative_path, name)

            if not os.path.isdir(os.path.dirname(target)):
                os.makedirs(os.path.dirname(target))
            shutil.copy(source, target)

def _ssh_command_line(config, with_server=True, extra_opts=''):
    return 'ssh {extra_opts} {ssh_global_opts} {ssh_opts} {server}'.format(
        extra_opts=extra_opts,
        ssh_global_opts=config.ssh_global_opts,
        ssh_opts=config.ssh_opts,
        server=config.server if with_server else '')

def _rsync(config, remote_path, local_path):
    utils.call(
        'rsync -avz --delete --delete-excluded '
        '{rsync_opts} -e "{ssh_command_line}" '
        '{server}:{remote_path} {local_path}'.format(
        rsync_opts=config.rsync_opts,
        ssh_command_line=_ssh_command_line(config, False),
        server=config.server,
        remote_path=remote_path,
        local_path=utils.to_cygwin_path(local_path)))


class Package(object):
    def __init__(self, project, path):
        self.path = path
        self.name = os.path.split(path)[1]
        self.dependencies = []

        if not os.path.isdir(self.path):
            raise Exception('Path %r is not a directory for a package' % self.path)

        self.files_path = join(self.path, 'files')

        package_config = join(self.path, 'config.py')
        if os.path.isfile(package_config):
            execfile(package_config, {}, self.__dict__)

    def __repr__(self):
        return '<Package %s %s>' % (self.name, self.path)

    @property
    def fixtures(self):
        return (glob.glob(join(self.path, '*.sql')) +
            glob.glob(join(self.path, '*.importer')))


class PackagesLoader(object):
    def __init__(self, project):
        self.project = project
        # XXX does it work with installed packages?
        system_packages_dir = join(project.env.source_path, 'packages')
        self.system_packages = self._load_from_dir(system_packages_dir)
        # _compute_dependencies call here is just for sanity check.
        # They shouldn't depend on anything else.
        self._compute_dependencies(self.system_packages)

    def _load_from_dir(self, packages_dir):
        IGNORED_DIRS = ('.svn', 'web_pages')

        packages = {}
        for package_path in glob.glob(join(packages_dir, '*')):
            if (not os.path.isdir(package_path) or
                os.path.basename(package_path) in IGNORED_DIRS):
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
                    raise Exception('Can\'t resolve dependency to package %r' % dep)
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
            packages[system_package_name] = self.system_packages[system_package_name]

        packages = self._compute_dependencies(packages)

        log.debug('Loaded packages: %r', packages.keys())
        return packages.values()


class Site(object):
    def __init__(self, project, path):
        self.path = path

        self.name = os.path.split(path)[1]
        self.base_path = ''
        self.rewrite_rules = []
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


def command(root_required=False):
     def _command(f):
         def wrapper(*args, **kwargs):
             project = args[0]
             if not project.env.c.no_root_check and project.env.platform != 'win':
                is_root = os.geteuid() == 0
                if root_required and not is_root:
                    raise Exception('You must run this command as root')
                if not root_required and is_root:
                    raise Exception('You can\'t run this command as root')
             return f(*args, **kwargs)
         return wrapper
     return _command


class Project(object):
    # FIXME: not used for now, there are some permissions issues when syncing
    # on Windows.
    UPDATE_HTDOCS = False

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

        self._read_config()

        # env might not be available yet. set_env should be called once ready
        # to complete the project initialization.
        if self.env:
            self.set_env(self.env)

    def set_env(self, env):
        self.env = env
        self.packages_loader = PackagesLoader(self)
        self._load_sites()
        self.daemon = daemon.Daemon(self.env)

    def get_site(self, site_name):
        for s in self.sites:
            if s.name == site_name:
                return s
        return None

    def _read_config(self):
        config_paths = [
            join(self.path, 'config.py'), join(self.path, 'config_local.py')]
        for config_path in config_paths:
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
            self.config.log_file = join(self.path, 'logs', 'synthese.txt')
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

        # TODO: add vhost info.
        self.config.static_paths = []
        for site in self.sites:
            for package in site.packages:
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

    def _run_testdata_importer(self):
        importer_path = self.env.testdata_importer_path
        log.info('Runing testdata importer from %r', importer_path)
        self.env.prepare_for_launch()
        env = os.environ.copy()
        env['SYNTHESE_TESTDATA_CONNSTRING'] = self.config.conn_string + \
            ',triggerCheck=0'
        utils.call(importer_path, env=env)

    def _clean_files(self):
        if self.UPDATE_HTDOCS:
            utils.RemoveDirectory(self.htdocs_path)
            os.makedirs(self.htdocs_path)

    def _sync_files(self, site, package):
        # TODO: create a directory per site, when vhosts are implemented.
        if self.UPDATE_HTDOCS:
            _copy_over(package.files_path, self.htdocs_path)

    @property
    def db_backend(self):
        if self._db_backend:
            return self._db_backend
        self._db_backend = db_backends.create_backend(
            self.env, self.config.conn_string)
        return self._db_backend

    def _clean_db(self):
        self.db_backend.drop_db()

    def _init_db(self):
        self.db_backend.init_db()

    def _sync_db(self, site, package):
        for fixtures_file in package.fixtures:
            if fixtures_file.endswith('.sql'):
                vars = {
                    'site_id': site.id,
                }
                self.db_backend.import_fixtures(fixtures_file, vars)
            elif fixtures_file.endswith('.importer'):
                self._run_testdata_importer()

    def _clean(self):
        self._clean_files()
        self._clean_db()

    def _init(self):
        # Files will be initialized in _sync_files()
        self._init_db()

    def reset(self):
        """Deletes database and files and run sync afterwards"""
        log.info('Resetting project')
        self._clean()
        self._init()
        self.sync()

    def sync(self):
        """Load fixtures into the database and prepare static files."""
        log.info('Syncing project')
        self._clean_files()
        # TODO: don't import fixtures from a package more than once.
        for site in self.sites:
            for package in site.packages:
                log.debug('Syncing %s %s', site, package)
                self._sync_files(site, package)
                self._sync_db(site, package)

        # TODO: also sync fixtures at the top level of the project.


    @command()
    def rundaemon(self, block=True):
        """Run Synthese daemon"""
        self.daemon.start()
        if not block:
            return
        log.info('Daemon running, press ctrl-c to stop')

        try:
            running = True
            while running:
                while self.daemon.is_running():
                    time.sleep(2)
                log.info('Daemon terminated')
                expected_stop = self.daemon.stopped
                running = False
                if not expected_stop:
                    log.warn('Stop is unexpected, crash?')
                    if self.config.restart_if_crashed:
                        self.daemon.start(kill_existing=False)
                        running = True
                    else:
                        sys.exit(1)
        except:
            raise
        finally:
            log.info('Stopping daemon')
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
        proxy.serve_forever(self.env)


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

        uncompressed_target = join(
            self.db_path, 'config_{project_name}.sql'.format(
                project_name=self.config.project_name))
        open(uncompressed_target, 'wb').write(output)

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
                (dumps, all_dumps))

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
    def db_sync_from_files(self, host=None):
        db_sync.sync_from_files(self, host)

    @command()
    def db_sync(self, host=None):
        db_sync.sync(self, host)

    # Commands for syncing or managing a remote project.

    @command()
    def db_remote_dump(self):
        """Dump database from remote server"""

        if not self.config.server:
            raise Exception('No remote server defined in configuration')

        @contextlib.contextmanager
        def remote_transaction_sqlite(conn_info):
            remote_db_path = join(self.db_path, 'remote_config.db3')

            log.info('Fetching db to %r', remote_db_path)
            _rsync(self.config, '/srv/data/s3-server/config.db3',
                utils.to_cygwin_path(remote_db_path))

            remote_conn_info = self.db_backend.conn_info.copy()
            remote_conn_info['path'] = remote_db_path

            yield remote_conn_info

        @contextlib.contextmanager
        def remote_transaction_mysql(conn_info):
            MYSQL_FORWARDED_PORT = 33000

            p = subprocess.Popen(
                _ssh_command_line(
                    self.config,
                    extra_opts='-N -L {forwarded_port}:localhost:3306'.format(
                        forwarded_port=MYSQL_FORWARDED_PORT)), shell=True)

            remote_conn_info = db_backends.ConnectionInfo(
                self.config.remote_conn_string)
            remote_conn_info.data['port'] = MYSQL_FORWARDED_PORT
            remote_conn_info.data['host'] = 'localhost'

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
            self.db_dump(remote_backend, 'remote_')

    @command()
    def ssh(self):
        """Open a ssh shell on the remote server"""
        utils.call(_ssh_command_line(self.config))

    # System install/uninstall

    def _get_tools(self):
        supervisor = external_tools.Supervisor(self)
        apache = Apache(self)
        return [supervisor, apache]

    @command()
    def system_install_prepare(self, tools=None):
        tools = self._get_tools()

        for tool in tools:
            tool.generate_config()

    @command(root_required=True)
    def system_install(self):
        if self.env.platform == 'win':
            raise Exception('Windows is not supported')

        tools = self._get_tools()

        import pwd
        os.setegid(pwd.getpwnam('synthese').pw_gid)
        os.seteuid(pwd.getpwnam('synthese').pw_uid)

        for tool in tools:
            tool.generate_config()

        os.seteuid(0)
        os.setegid(0)

        for tool in tools:
            tool.system_install()

        log.info('Project installed on the system. You can start it with '
            'the command (as root):\n  supervisorctl start synthese-%s',
            self.config.project_name)

    @command(root_required=True)
    def system_uninstall(self):
        tools = self._get_tools()

        for tool in tools:
            tool.system_uninstall()

        log.info('Project uninstalled. You should stop the daemon with '
            'this command (as root):\n  supervisorctl stop synthese-%s',
            self.config.project_name)


def create_project(env, path, system_packages=None, conn_string=None,
        overwrite=False):
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

    synthesepy_source = join(env.source_path, 'tools', 'synthese.py')
    managepy_target = join(path, 'manage.py')
    synthesepy_content = open(synthesepy_source, 'rb').read()
    synthesepy_content = synthesepy_content.replace(
        '@SYNTHESEPY_DIR@', repr(os.environ['SYNTHESEPY_DIR'])[1:-1])
    open(managepy_target, 'wb').write(synthesepy_content)

    if system_packages:
        www_site_config_path = join(path, 'sites', 'www', 'config.py')
        www_site_config = open(www_site_config_path).read()
        assert 'system_packages = ' not in www_site_config
        www_site_config += '\nsystem_packages = {0!r}\n'.format(system_packages)

        with open(www_site_config_path, 'wb') as f:
            f.write(www_site_config)

    project = Project(path, env=env)
    project.reset()
    return project
