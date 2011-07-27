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
from synthesepy import daemon
from synthesepy import db_backends
from synthesepy import db_sync
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


# XXX remove unused and maybe move to utils

def maybe_mkdir(path):
    if os.path.isdir(path):
        return
    os.makedirs(path)

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
        if os.path.isfile(self.path):
            self.path = open(self.path).read().strip().format(
                source_path=project.env.source_path).replace('/', os.sep)

        if not os.path.isdir(self.path):
            raise Exception('Path %r is not a directory for a package' % self.path)

        self.files_path = join(self.path, 'files')

    def __repr__(self):
        return '<Package %s %s>' % (self.name, self.path)

    @property
    def fixtures(self):
        return (glob.glob(join(self.path, '*.sql')) +
            glob.glob(join(self.path, '*.importer')))


class Site(object):
    IGNORED_SUFFIXES = ('.svn', '.py', 'web_pages')
    def __init__(self, project, path):
        self.path = path

        self.name = os.path.split(path)[1]
        self.base_path = ''

        site_config = join(self.path, 'config.py')
        if os.path.isfile(site_config):
            execfile(site_config, {}, self.__dict__)

        self.packages = []
        for package_path in sorted(glob.glob(join(self.path, '*'))):
            if package_path.endswith(self.IGNORED_SUFFIXES):
                continue
            self.packages.append(Package(project, package_path))
        log.debug('Found packages: %s', self.packages)

    def __repr__(self):
        return '<Site %s %s>' % (self.name, self.path)


class Project(object):
    # FIXME: not used for now, there are some permissions issues when syncing
    # on Windows.
    UPDATE_HTDOCS = False

    def __init__(self, path, env=None, config=None):
        self.path = os.path.normpath(os.path.abspath(path))
        self.env = env
        self.config = config
        if not config and env:
            self.config = env.config
        if not self.config:
            self.config = Config()
        self.htdocs_path = join(path, 'htdocs')
        self.daemon = None
        self.__db_backend = None
        self.db_path = join(path, 'db')
        if not os.path.isdir(self.db_path):
            os.makedirs(self.db_path)

        self._read_config()

        # Sites require an env, wait for set_env() to be called if not
        # available yet.
        if self.env:
            self._load_sites()

    def set_env(self, env):
        self.env = env
        self._load_sites()

    def _read_config(self):
        config_paths = [
            join(self.path, 'config.py'), join(self.path, 'config_local.py')]
        for config_path in config_paths:
            if not os.path.isfile(config_path):
                continue
            log.debug('Reading config file: %r', config_path)
            self.config.update_from_file(config_path)

        self.config.conn_string = self.config.conn_string.replace(
            '@PROJECT_PATH@', self.path)

        # Set defaults

        if not self.config.project_name:
            self.config.project_name = os.path.split(self.path)[1]

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
            self.config.site_id = [
                s for s in self.sites if s.name != 'admin'][0].id

    def _clean_files(self):
        if self.UPDATE_HTDOCS:
            utils.RemoveDirectory(self.htdocs_path)
            os.makedirs(self.htdocs_path)

    def _sync_files(self, site, package):
        # TODO: create a directory per site, when vhosts are implemented.
        if self.UPDATE_HTDOCS:
            _copy_over(package.files_path, self.htdocs_path)

    def _clean_db(self):
        self._db_backend.drop_db()

    def _init_db(self):
        self._db_backend.init_db()

    # TODO: maybe make this public, and rename to db_backend / _db_backend
    @property
    def _db_backend(self):
        if self.__db_backend:
            return self.__db_backend
        self.__db_backend = db_backends.create_backend(
            self.env, self.config.conn_string)
        return self.__db_backend

    def _run_testdata_importer(self):
        importer_path = self.env.get_executable_path(
            join('test', '53_pt_routeplanner'),
            'ImportRoutePlannerTestData')
        log.info('Runing testdata importer from %r', importer_path)
        self.env.prepare_for_launch()
        env = os.environ.copy()
        env['SYNTHESE_TESTDATA_CONNSTRING'] = self.config.conn_string + \
            ',triggerCheck=0'
        utils.call(importer_path, env=env)

    def _sync_db(self, site, package):
        for fixtures_file in package.fixtures:
            if fixtures_file.endswith('.sql'):
                vars = {
                    'site_id': site.id,
                }
                self._db_backend.import_fixtures(fixtures_file, vars)
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
        for site in self.sites:
            for package in site.packages:
                log.debug('Syncing %s %s', site, package)
                self._sync_files(site, package)
                self._sync_db(site, package)

        # TODO: also sync fixtures at the top level of the project.


    def rundaemon(self, block=True):
        """Run Synthese daemon"""
        self.daemon = daemon.Daemon(self.env)
        self.daemon.start()
        if not block:
            return
        log.info('Daemon running, press ctrl-c to stop')

        try:
            while True:
                while self.daemon.is_running():
                    time.sleep(2)
                log.warn('Daemon terminated (crash?)')
                if self.config.restart_if_crashed:
                    self.daemon.start(kill_existing=False)
                else:
                    break
        except:
            raise
        finally:
            log.info('Stopping daemon')
            self.daemon.stop()

    def stopdaemon(self):
        """Stop Synthese daemon"""
        if self.daemon:
            self.daemon.stop()
        # TODO: should use the HTTP method to stop the daemon once it works.
        ports = [self.config.port]
        if self.config.wsgi_proxy:
            ports.append(self.config.wsgi_proxy_port)
        for port in ports:
            utils.kill_listening_processes(port)

    def runproxy(self):
        """Run HTTP Proxy to serve static files"""
        proxy.serve_forever(self.env)


    def db_view(self):
        """Open database in a GUI tool (if applicable)"""
        if self._db_backend.name == 'sqlite':
            utils.call(
                [self.config.spatialite_gui_path,
                    self._db_backend.conn_info['path']],
                bg=True)
        else:
            raise NotImplementedError("Not implemented for this backend")

    def db_shell(self, sql=None):
        """Open a SQL interpreter on the database or execute the given SQL"""
        self._db_backend.shell(sql)

    def db_dump(self, db_backend=None, prefix=''):
        """Dump database to text file"""

        if not db_backend:
            db_backend = self._db_backend
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


    def db_open_dump(self):
        """Open the latest database dump in a text editor"""

        uncompressed_target = join(
            self.db_path, 'config_{project_name}.sql'.format(
                project_name=self.config.project_name))

        if os.path.isfile(self.config.editor_path):
            utils.call([self.config.editor_path, uncompressed_target], bg=True)


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

        if sql_file.endswith('.gz'):
            sql = gzip.open(sql_file, 'rb').read()
        else:
            sql = open(sql_file, 'rb').read()

        log.info('Restoring %s bytes of sql', len(sql))
        self._db_backend.restore(sql)

    def db_sync_to_files(self):
        db_sync.sync_to_files(self)

    def db_sync_from_files(self, host=None):
        db_sync.sync_from_files(self, host)

    def db_sync(self, host=None):
        db_sync.sync(self, host)

    # Commands for syncing or managing a remote project.

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

            remote_conn_info = self._db_backend.conn_info.copy()
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

        if self._db_backend.name == 'sqlite':
            remote_transaction = remote_transaction_sqlite
        elif self._db_backend.name == 'mysql':
            remote_transaction = remote_transaction_mysql
        else:
            raise Exception('Unsupported backend: %r', self._db_backend.name)

        with remote_transaction(self._db_backend.conn_info.copy()) as remote_conn_info:
            remote_conn_string = remote_conn_info.conn_string
            log.info('Remote connection string: %r', remote_conn_string)
            remote_backend = db_backends.create_backend(self.env, remote_conn_string)
            self.db_dump(remote_backend, 'remote_')

    def ssh(self):
        """Open a ssh shell on the remote server"""
        utils.call(_ssh_command_line(self.config))


def create_project(env, path, site_packages=None, conn_string=None, overwrite=False):
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

    if not site_packages:
        site_packages = {
            'admin': ('admin',),
            # XXX maybe don't load testData by default.
            'main': ('core', 'routePlanner', 'testData'),
        }
    log.debug('site_packages: %r', site_packages)

    for site_name, packages in site_packages.iteritems():
        for package in packages:
            package_source_path = '/'.join(
                ['{source_path}', 'packages', package])
            real_package_source_path = package_source_path.format(
                source_path=env.source_path).replace('/', os.sep)
            if not os.path.isdir(real_package_source_path):
                raise Exception(
                    'Package doesn\'t exist at %r', real_package_source_path)

            package_target_path = join(path, 'sites', site_name, package)
            # TODO: option to set svn:externals instead.
            open(package_target_path, 'wb').write(package_source_path)

    project = Project(path, env=env)
    project.reset()
    return project
