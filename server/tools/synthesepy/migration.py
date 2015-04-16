#    Scripts to migrate the database schema.
#    @file migration.py
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

import logging

from synthesepy import utils


log = logging.getLogger(__name__)


def migrate_0_to_1(project, db):
    """
    Version 1:
    * Rename admin ids.
    * Populate t086_pt_services_configurations from t025_sites.
    """

    # Update admin page ids
    # TODO: delete old pages if there is both the new and old in the database.
    db.query('UPDATE t063_web_pages SET id=17732923532771328 WHERE id=177329235327713281;')
    db.query('UPDATE t063_web_pages SET id=17732923532771329 WHERE id=177329235327713282;')
    db.query('UPDATE t063_web_pages SET id=17732923532771330 WHERE id=177329235327713283;')
    db.query('UPDATE t063_web_pages SET up_id=17732923532771328 WHERE up_id=177329235327713281;')

    # Populate the new t086_pt_services_configurations table which was split
    # from t025_sites
    try:
        db.query('SELECT * from t086_pt_services_configurations')
    except:
        log.info('Creating t086_pt_services_configurations table')
        # Create the table
        if db.name == 'sqlite':
            db.query("""
                CREATE TABLE t086_pt_services_configurations
                    ("id" INTEGER UNIQUE PRIMARY KEY ON CONFLICT ROLLBACK,
                    "name" TEXT, "online_booking" BOOLEAN,
                    "use_old_data" BOOLEAN, "max_connections" INTEGER,
                    "use_dates_range" INTEGER,
                    "periods" TEXT,
                    "display_road_approach_detail" BOOLEAN)""")
        else:
            db.query("""
                CREATE TABLE `t086_pt_services_configurations` (
                 `id` bigint(20) NOT NULL,
                 `name` text,
                 `online_booking` tinyint(1) DEFAULT NULL,
                 `use_old_data` tinyint(1) DEFAULT NULL,
                 `max_connections` bigint(20) DEFAULT NULL,
                 `use_dates_range` bigint(20) DEFAULT NULL,
                 `periods` text,
                 `display_road_approach_detail` tinyint(1) DEFAULT NULL,
                 PRIMARY KEY (`id`)
                )""")

    COLUMNS_TO_MOVE = ("id name online_booking use_old_data max_connections "
        "use_dates_range periods display_road_approach_detail").split()

    for site in db.query('select * from t025_sites'):
        id = site['id']
        uid_info = utils.decode_uid(id)
        config_id = utils.encode_uid(86, uid_info.object_id, uid_info.grid_node_id)

        count = db.query('select count(1) as c from '
            't086_pt_services_configurations where id = ?',
            [config_id], one=True)['c']
        if count > 0:
            continue

        cols = ','.join(COLUMNS_TO_MOVE)
        def to_sql(val):
            if val is None:
                return ''
            if isinstance(val, basestring):
                return str(val.encode('utf-8'))
            return str(val)
        try:
            vals = ["'{0}'".format(to_sql(site[c])) for c in COLUMNS_TO_MOVE]
        except KeyError, e:
            log.warn('Table t025_sites already migrated? (%s)', e)
            continue
        # Sets the id
        vals[0] = "'{0}'".format(config_id)

        db.query('insert into t086_pt_services_configurations '
            '({0}) values ({1})'.format(','.join(COLUMNS_TO_MOVE), ','.join(vals)))


def maybe_migrate_schema(project):
    CURRENT_SCHEMA = 1
    db = project.db_backend
    old_schema = 0

    try:
        res = db.query(
            "select param_value from t999_config where param_name = 'schema_version'",
            one=True)
    except Exception, e:
        log.info('No t999_config table, skipping migration (%s)', e)
        return

    if res is None:
        db.query(
            "insert into t999_config values ('schema_version', ?)",
            [old_schema])
    else:
        old_schema = int(res['param_value'])

    log.info('Old schema version: %i current schema: %i', old_schema, CURRENT_SCHEMA)

    if old_schema > CURRENT_SCHEMA:
        log.warn('DB schema higher than max supported schema (%s > %s)', old_schema, CURRENT_SCHEMA)
        return
    if old_schema == CURRENT_SCHEMA:
        log.info('Nothing to migrate')
        return

    for from_schema in range(old_schema, CURRENT_SCHEMA):
        to_schema = from_schema + 1
        fun_name = 'migrate_{0}_to_{1}'.format(from_schema, to_schema)
        log.info('Calling %s', fun_name)
        globals()[fun_name](project, db)

    log.info('Migration complete')
    db.query(
        "update t999_config set param_value = ? where param_name = 'schema_version'",
        [CURRENT_SCHEMA])
