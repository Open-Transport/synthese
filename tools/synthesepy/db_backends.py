#    Database backends.
#    @file db_backends.py
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


import contextlib
import logging
import os
import shutil
import sqlite3
import sys
import tempfile
import time
from UserDict import UserDict

import MySQLdb

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

    def query(self, query, args=(), one=False):
        """Queries the database and returns a list of dictionaries."""
        # Hack for MySQL
        if self.paramstyle == 'format':
            query = query.replace('?', '%s')
        with self.get_cursor() as cursor:
            cursor.execute(query, args)
            rv = [dict((cursor.description[idx][0], value)
                       for idx, value in enumerate(row)) for row in cursor.fetchall()]
            return (rv[0] if rv else None) if one else rv

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
        with self.get_cursor() as cursor:
            with open(fixtures_file, 'rb') as f:
                sql = f.read()
                for (key, value) in vars.iteritems():
                    sql = sql.replace("@@" + key + "@@", str(value))
                self.import_sql(sql)

    def import_sql(self, sql):
        raise NotImplementedError()

    def start_daemon(self):
        self.env.c.conn_string = self.conn_info.conn_string
        self.daemon = daemon.Daemon(self.env)
        self.daemon.start()

    def stop_daemon(self):
        assert self.daemon
        self.daemon.stop()
        self.daemon = None

    def shell(self, sql=None):
        """Open a SQL interpreter on the database or execute the given SQL"""
        raise NotImplementedError()

    def dump(self):
        """Return a database dump as a string"""
        raise NotImplementedError()

    def restore(self, sql):
        """Restore this database from the provided sql"""
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
            # TODO: this shouldn't be hardcoded.
            conn.execute("SELECT load_extension('/usr/lib/libspatialite.so.2.1.0')")
        return conn

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
        # Warning: shell=False is required on Linux, otherwise it launches the
        # interpreter and it hangs.
        cmd = [self.env.config.spatialite_path] + cmd
        return utils.call(cmd, shell=False, **kwargs)

    def shell(self, sql=None):
        kwargs = {'input': sql} if sql else {}
        output = self._call_spatialite([self.conn_info['path']], **kwargs)
        if output:
            print output

    def dump(self):
        args = ['-bail', self.conn_info['path'], '.dump']
        log.debug('Running: %r', args)
        output = self._call_spatialite(args, input='')
        # Remove the Spatialite header, which isn't valid SQL.
        # (-noheader doesn't have any effect)
        return output[output.index('BEGIN TRANSACTION'):]

    def restore(self, sql):
        db_path = self.conn_info['path']
        if os.path.isfile(db_path):
            os.unlink(db_path)

        self._call_spatialite(['-bail', '-noheader', db_path], input=sql)
        log.info('Database %r restored', db_path)


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

    def _create_db(self):
        with self.get_cursor(False) as cursor:
            cursor.execute('CREATE DATABASE %s' % self.conn_info['db'])

    def drop_db(self):
        with self.get_cursor(False) as cursor:
            try:
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
                if not line.strip().endswith(';'):
                    current_line += line
                    continue
                current_line += line
                cursor.execute(current_line)
                current_line = ''

    def _setup_path(self):
        if sys.platform != 'win32':
            return

        # Keep this in sync with tools/synthesepy/build.py
        utils.append_paths_to_environment('PATH', [
            os.path.join(
                self.env.c.thirdparty_dir, 'mysql-5.5.14-win32', 'bin')])

    def _mysql_command(self, command, extra_opts='', input=''):
        self._setup_path()
        default_args = {
            'port': '3306',
        }
        args = default_args
        args.update(self.conn_info.data)
        args.update(dict(
            command=command,
            extra_opts=extra_opts,
        ))
 
        cmd = ('{command} {extra_opts} -u{user} -p{passwd} -h{host} '
            '-P{port} {db}'.format(**args))

        if input is not None:
            return utils.call(cmd, input=input)
        return utils.call(cmd)

    def shell(self, sql=None):
        output = self._mysql_command('mysql', input=sql)
        if output:
            print output

    def dump(self):
        return self._mysql_command(
            'mysqldump', extra_opts=self.env.c.mysqldump_opts)

    def restore(self, sql):
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
