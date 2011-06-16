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
import tempfile
import time
from UserDict import UserDict

import MySQLdb

from .daemon import Daemon
from . import utils

log = logging.getLogger(__name__)


class DBBackend(object):
    def __init__(self, env, conn_info):
        self.env = env
        self.conn_info = conn_info

        self.initial_data_file = os.path.join(
            self.env.admin_root_path,
            'install.sql'
        )

    def get_connection(self):
        raise NotImplementedError()

    @contextlib.contextmanager
    def get_cursor(self, *args, **kwargs):
        conn = self.get_connection(*args, **kwargs)
        cursor = conn.cursor()
        yield cursor
        conn.commit()
        cursor.close()

    def init_db(self):
        utils.kill_listening_processes(self.env.port)

        self.drop_db()
        self._create_db()
        self.start_daemon()
        self.stop_daemon()
        log.info('Importing data from %s', self.initial_data_file)
        self._import_data()
        log.info('db initialized')

    def drop_db(self):
        raise NotImplementedError()

    def start_daemon(self):
        self.env.conn_string = self.conn_info.conn_string
        self.daemon = Daemon(self.env)
        self.daemon.start()

    def stop_daemon(self):
        assert self.daemon
        self.daemon.stop()
        self.daemon = None


class SQLiteBackend(DBBackend):
    name = 'sqlite'

    def __init__(self, *args, **kwargs):
        super(SQLiteBackend, self).__init__(*args, **kwargs)
        self.sqlite_file = self.conn_info.get('path')
        if self.sqlite_file == '@TEMPFILE@':
            self.sqlite_file = os.path.join(tempfile.gettempdir(), 'synthese.db3')
            self.conn_info['path'] = self.sqlite_file
        if not self.sqlite_file:
            # If no path is specified in the connection string, the sqlite
            # backend uses a file called 'config.db3' in the current directory.
            # Here we use the default daemon directory so that it
            # initializes the file that would be run with 'rundaemon'.
            self.sqlite_file = os.path.join(
                self.env.daemon_launch_path,
                'config.db3'
            )
        log.debug('Sqlite file: %s', self.sqlite_file)

    def get_connection(self, spatialite=False):
        if spatialite:
            # We need a sqlite library compiled with module loading capability.
            # One option is to recompile pysqlite2 without the
            # 'define=SQLITE_OMIT_LOAD_EXTENSION' line commented in setup.cfg
            import pysqlite2.dbapi2

            assert os.path.isfile(self.sqlite_file)
            conn = pysqlite2.dbapi2.connect(self.sqlite_file)

            conn.enable_load_extension(True)
            # TODO: this shouldn't be hardcoded.
            conn.execute("SELECT load_extension('/usr/lib/libspatialite.so.2.1.0')")
            return conn

        assert os.path.isfile(self.sqlite_file)
        conn = sqlite3.connect(self.sqlite_file)
        return conn

    def _create_db(self):
        # Nothing to do, the sqlite file will be created if it doesn't exist.
        pass

    def drop_db(self):
        if os.path.isfile(self.sqlite_file):
            # Copy to backup file for debugging
            shutil.copy(self.sqlite_file, self.sqlite_file + '_backup')
            os.unlink(self.sqlite_file)

    def _import_data(self):
        with self.get_cursor() as cursor:
            with open(self.initial_data_file, 'rb') as f:
                cursor.executescript(f.read())


class MySQLBackend(DBBackend):
    name = 'mysql'

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

    def _import_data(self):
        with self.get_cursor() as cursor:
            with open(self.initial_data_file, 'rb') as f:
                # MySQLdb can't execute multiple statements (however it works on Windows with version 1.2.3)
                # So, split lines at semicolons.
                current_line = ''
                for line in f:
                    if not line.strip().endswith(';'):
                        current_line += line
                        continue
                    current_line += line
                    cursor.execute(current_line)
                    current_line = ''


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
            ['%s=%s' % (key, name) for (key, name) in self.data.iteritems()]
        )
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
