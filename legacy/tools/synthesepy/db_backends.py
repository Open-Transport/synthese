#    Database backends.
#    @file db_backends.py
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


import contextlib
import gzip
import logging
import os
import shutil
import sqlite3
import sys
import tempfile
import time
from UserDict import UserDict

try:
    import MySQLdb
except ImportError:
    # Might not be installed yet when running system_install.
    pass

import synthesepy.build
from synthesepy import daemon
from synthesepy import utils


log = logging.getLogger(__name__)


class DBBackend(object):
    def __init__(self, env, conn_info):
        self.env = env
        self.conn_info = conn_info

    def __repr__(self):
        return '<DBBackend %s>' % self.conn_info.conn_string

    def get_connection(self):
        raise NotImplementedError()

    @contextlib.contextmanager
    def get_cursor(self, *args, **kwargs):
        conn = self.get_connection(*args, **kwargs)
        cursor = conn.cursor()
        yield cursor
        conn.commit()
        cursor.close()

    def query(self, query, query_args=(), one=False, *args, **kwargs):
        """Queries the database and returns a list of dictionaries."""
        # Hack for MySQL
        if self.paramstyle == 'format':
            query = query.replace('?', '%s')
        log.debug('Running query: %s', query)
        with self.get_cursor(*args, **kwargs) as cursor:
            cursor.execute(query, query_args)
            rv = [dict((cursor.description[idx][0], value)
                       for idx, value in enumerate(row)) for row in cursor.fetchall()]
            return (rv[0] if rv else None) if one else rv

    def replace_into(self, table, object, *args, **kwargs):
        """Runs a REPLACE INTO query with the given object dict"""
        columns = object.keys()
        values = [object[c] for c in columns]
        self.query(
            'replace into %s(%s) values (%s)' % (
                table, ','.join(columns), ','.join(['?'] * len(columns))
            ), values, *args, **kwargs)

    def get_tables(self):
        raise NotImplementedError()

    def init_db(self):
        utils.kill_listening_processes(self.env.c.port)

        self.drop_db()
        self._create_db()
        self.start_daemon()
        self.stop_daemon()
        log.info('db initialized')

    def drop_db(self):
        raise NotImplementedError()

    def import_fixtures(self, fixtures_file, vars={}):
        log.info('Importing fixtures: %s', fixtures_file)
        if fixtures_file.endswith('.gz'):
            opener = gzip.open
        else:
            opener = open
        with opener(fixtures_file, 'rb') as f:
            sql = f.read()
            for (key, value) in vars.iteritems():
                sql = sql.replace("@@" + key + "@@", str(value))
            self.import_sql(sql)

    def import_sql(self, sql):
        raise NotImplementedError()

    def start_daemon(self):
        self.env.c.conn_string = self.conn_info.conn_string
        self.daemon = daemon.Daemon(self.env, use_proxy=False)
        self.daemon.start()

    def stop_daemon(self):
        assert self.daemon
        self.daemon.stop()
        self.daemon = None

    def shell(self, sql=None):
        """Open a SQL interpreter on the database or execute the given SQL"""
        raise NotImplementedError()

    def dump(self, table=None):
        """Return a database or table dump as a string"""
        raise NotImplementedError()

    def restore(self, sql, dropdb=True):
        """Run the provided sql (used to restore the db or a table)."""
        raise NotImplementedError()


class SQLiteBackend(DBBackend):
    name = 'sqlite'

    def __init__(self, *args, **kwargs):
        super(SQLiteBackend, self).__init__(*args, **kwargs)
        self.sqlite_file = self.conn_info.get('path')
        if self.sqlite_file == '@TEMPFILE@':
            self.sqlite_file = os.path.join(tempfile.gettempdir(), 'synthese.db3')
            self.conn_info['path'] = self.sqlite_file
        if not self.sqlite_file:
            # XXX this might be obsolete once using projects is mandatory.
            # If no path is specified in the connection string, the sqlite
            # backend uses a file called 'config.db3' in the current directory.
            # Here we use the default daemon directory so that it
            # initializes the file that would be run with 'rundaemon'.
            self.sqlite_file = os.path.join(
                self.env.daemon_launch_path,
                'config.db3')
        self.paramstyle = sqlite3.paramstyle
        log.debug('Sqlite file: %s', self.sqlite_file)

    def get_connection(self, spatialite=False):
        if self.env.platform == 'win':
            # Force spatialite usage on Windows. The native sqlite module
            # doesn't come with rtree and can't operate on geographical tables.
            spatialite = True

        db_module = sqlite3
        if spatialite:
            # We need a sqlite library compiled with module loading capability.
            # One option is to recompile pysqlite2 without the
            # 'define=SQLITE_OMIT_LOAD_EXTENSION' line commented in setup.cfg
            import pysqlite2.dbapi2
            db_module = pysqlite2.dbapi2

        assert os.path.isfile(self.sqlite_file)

        conn = db_module.connect(self.sqlite_file)
        if spatialite:
            conn.enable_load_extension(True)
            if self.env.platform == 'lin':
                # TODO: this shouldn't be hardcoded.
                module_path = '/usr/lib/libspatialite.so.2.1.0'
            elif self.env.platform == 'win':
                builder = synthesepy.build.get_builder(self.env)
                builder.update_path_for_libspatialite()
                module_path = 'libspatialite-2.dll'
            else:
                assert False

            conn.execute("SELECT load_extension('%s')" % module_path)
        return conn

    def get_tables(self):
        sql = "select name from sqlite_master where type='table' order by name;"
        return [row['name'] for row in self.query(sql)]

    def _create_db(self):
        # Nothing to do, the sqlite file will be created if it doesn't exist.
        pass

    def drop_db(self):
        if os.path.isfile(self.sqlite_file):
            # Copy to backup file for debugging
            shutil.copy(self.sqlite_file, self.sqlite_file + '_backup')
            os.unlink(self.sqlite_file)

    def import_sql(self, sql):
        with self.get_cursor() as cursor:
            cursor.executescript(sql)

    def _call_spatialite(self, cmd, shell=True, **kwargs):
        # TODO: this should use the compiled spatialite.

        # Warning: shell=False is required on Linux, otherwise it launches the
        # interpreter and it hangs.
        cmd = [self.env.config.spatialite_path] + cmd
        return utils.call(cmd, shell=False, **kwargs)

    def shell(self, sql=None):
        kwargs = {'input': sql} if sql else {}
        output = self._call_spatialite([self.conn_info['path']], **kwargs)
        if output:
            print output
        return output

    def dump(self, table=None):
        cmd = '.dump' if table is None else ('.dump %s' % table)
        args = ['-bail', self.conn_info['path'], cmd]
        log.debug('Running: %r', args)
        output = self._call_spatialite(args, input='')
        # Remove the Spatialite header, which isn't valid SQL.
        # (-noheader doesn't have any effect)
        return output[output.index('BEGIN TRANSACTION'):]

    def restore(self, sql, dropdb=True):
        db_path = self.conn_info['path']
        if dropdb and os.path.isfile(db_path):
            os.unlink(db_path)

        self._call_spatialite(['-bail', '-noheader', db_path], input=sql)
        log.info('sql executed on database %r', db_path)


class MySQLBackend(DBBackend):
    name = 'mysql'

    def __init__(self, *args, **kwargs):
        super(MySQLBackend, self).__init__(*args, **kwargs)
        self.paramstyle = MySQLdb.paramstyle

    def get_connection(self, select_db=True):
        args = {
            'host': self.conn_info['host'],
            'user': self.conn_info['user'],
            'passwd': self.conn_info['passwd'],

            'charset': 'utf8',
            'sql_mode': 'ANSI_QUOTES,NO_BACKSLASH_ESCAPES',
        }
        if select_db:
            args['db'] = self.conn_info['db']

        return MySQLdb.connect(**args)

    def get_tables(self):
        sql = 'show tables;'
        return [row.values()[0] for row in self.query(sql)]

    def _create_db(self):
        with self.get_cursor(False) as cursor:
            log.debug('Creating database %r', self.conn_info['db'])
            cursor.execute('CREATE DATABASE %s' % self.conn_info['db'])

    def drop_db(self):
        with self.get_cursor(False) as cursor:
            try:
                log.debug('Dropping database %r', self.conn_info['db'])
                cursor.execute('DROP DATABASE %s' % self.conn_info['db'])
            except MySQLdb.DatabaseError, e:
                DB_DROP_EXISTS = 1008
                if e.args[0] == DB_DROP_EXISTS:
                    return
                raise

    def import_sql(self, sql):
        with self.get_cursor() as cursor:
            # MySQLdb can't execute multiple statements (however it works on Windows with version 1.2.3)
            # So, split lines at semicolons.
            current_line = ''
            for line in sql.splitlines():
                if not current_line and line.startswith('--'):
                    continue
                # FIXME: this might break if there's a semicolon before a new
                # line in a multi-line statement.
                if not line.strip().endswith(';'):
                    current_line += line
                    continue
                current_line += line
                cursor.execute(current_line)
                current_line = ''

    def _setup_path(self):
        if sys.platform != 'win32':
            return

        utils.append_paths_to_environment('PATH', [
            os.path.join(
                self.env.c.thirdparty_dir,
                'mysql-{mysql_ver}-win32'.format(
                    mysql_ver=synthesepy.build.MYSQL_VER),
                'bin')])

    def _mysql_command(self, command, extra_opts='', input='', table=None):
        self._setup_path()
        default_args = {
            'port': '3306',
        }
        args = default_args
        args.update(self.conn_info.data)
        args.update(dict(
            command=command,
            extra_opts=extra_opts,
            table=table if table else '',
        ))

        cmd = ('{command} {extra_opts} -u{user} -p{passwd} -h{host} '
            '-P{port} {db} {table}'.format(**args))

        if input is not None:
            return utils.call(cmd, input=input)
        return utils.call(cmd)

    def shell(self, sql=None):
        output = self._mysql_command('mysql', input=sql)
        if output:
            print output

    def dump(self, table=None):
        return self._mysql_command(
            'mysqldump', extra_opts=self.env.c.mysqldump_opts, table=table)

    def restore(self, sql, dropdb=True):
        return self._mysql_command(
            'mysql', input=sql)


class DummyBackend(DBBackend):
    name = 'dummy'


class ConnectionInfo(UserDict):
    def __init__(self, _conn_string):
        UserDict.__init__(self)
        self.backend, args = _conn_string.split('://')
        for pair in args.split(','):
            if not pair:
                continue
            key, name = pair.split('=')
            self.data[key] = name

    @property
    def conn_string(self):
        args = ','.join(
            ['%s=%s' % (key, name) for (key, name) in self.data.iteritems()])
        return '%s://%s' % (self.backend, args)


def create_backend(env, conn_string):
    conn_info = ConnectionInfo(conn_string)

    for c in globals().itervalues():
        if (not isinstance(c, type) or not issubclass(c, DBBackend) or
            not hasattr(c, 'name')):
            continue
        if conn_info.backend == c.name:
            return c(env, conn_info)
    raise Exception('Unknown db backend: %s' % conn_info.backend)
