#    Converts a SQLite Synthese database to MySQL.
#    @file sqlite_to_mysql.py
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


from contextlib import closing
import logging
import re

from synthesepy import db_backends

log = logging.getLogger(__name__)


def convert(env, sourceconn, targetconn):
    sqlite_backend = db_backends.create_backend(env, sourceconn)
    assert sqlite_backend.name == 'sqlite'

    mysql_backend = db_backends.create_backend(env, targetconn)
    assert mysql_backend.name == 'mysql'

    # TODO: use get_cursor() instead.
    mysql_cursor = mysql_backend.get_connection().cursor()

    sqlite_conn = sqlite_backend.get_connection(spatialite=True)
    sqlite_cursor = sqlite_conn.cursor()

    table_re = re.compile('INSERT INTO "([^"]+)" VALUES\(([^,]+),')
    remove_geom_re = re.compile("X'[0-9A-F]+'")

    IGNORED_TABLES = ['spatial_ref_sys', 'geometry_columns']
    # Obsolete tables.
    IGNORED_TABLES.append('t005_service_dates')

    initialized_tables = set()

    for line in sqlite_conn.iterdump():
        if not line.startswith('INSERT INTO '):
            continue

        m = table_re.match(line)
        assert m, 'Parse error on line %s' % repr(line)
        table, rowid = m.groups(1)

        # Ignore rtree index tables.
        if table.startswith('idx_'):
            continue

        if table in IGNORED_TABLES:
            continue
        if table not in initialized_tables:
            initialized_tables.add(table)
            log.debug('Filling table %s', table)
            mysql_cursor.execute('TRUNCATE TABLE `%s`' % table)

        # TODO: this assumes that only geometry columns are encoded as blobs,
        # they are called 'geometry' and that there is only one.
        # This is all true at this time, but might change in the future.

        #log.debug('Inserting %s', line)
        line, n_subs = remove_geom_re.subn('NULL', line)
        assert n_subs <= 1

        mysql_cursor.execute(line)

        if n_subs == 1:
            res = sqlite_cursor.execute(
                'SELECT AsText(geometry) FROM %s WHERE id=%s' % (table, rowid))
            geom_text = res.fetchone()[0]

            mysql_cursor.execute(
                'UPDATE `%s` SET geometry=GeomFromText(%%s)'
                '  WHERE id=%%s' % table,
                (geom_text, rowid))

    for table in initialized_tables:
        log.info('Verifying row count of table %s', table)
        sqlite_count = sqlite_cursor.execute('SELECT count(1) FROM %s' % table).fetchone()[0]
        mysql_cursor.execute('SELECT count(1) FROM %s' % table)
        mysql_count = mysql_cursor.fetchone()[0]

        if sqlite_count != mysql_count:
            raise Exception(
                ('After import of table: %s, row count differs: '
                 'sqlite: %s - mysql: %s') % (table, sqlite_count, mysql_count))
