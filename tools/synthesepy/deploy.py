#    Project deployment.
#    @file deploy.py
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
import os
from os.path import join

import project_manager
import utils

log = logging.getLogger(__name__)


NO_DEPLOY_TABLES = set([
    # sqlite/spatialite system tables
    'SpatialIndex',
    'geometry_columns',
    'geometry_columns_auth',
    'spatial_ref_sys',
    'spatialite_history',
    'sqlite_sequence',
    'views_geometry_columns',
    'virts_geometry_columns',
 
    # MySQL specific
    'trigger_metadata',
 
    # deprecated
    't023_interface_pages',
    't024_interfaces',
    't038_text_templates',

    # users and profile config
    't026_users',
    't027_profiles',
    't048_user_favorite_journey',

    # messages
    't039_scenarios',
    't040_alarm_object_links',
    't051_scenario_folder',
    
    # resa
    't044_reservations',
    't046_reservation_transactions',
    
    # logs
    't045_log_entries',
    
    # departure table
    't057_display_monitoring_status',
    
    # forum
    't067_forum_topics',
    't068_forum_messages',

    # pt operation
    't072_vehicle_positions',
    't077_vehicle_services', # (not sure?)

    # config
    't999_config',
])


def unixjoin(*paths):
    return '/'.join(paths)


class Deployer(object):
    def __init__(self, project):
        self.project = project
        self.dump_local_path = join('deploy', 'dump.sql')
        self.dump_path = join(project.path, self.dump_local_path)
        utils.maybe_makedirs(os.path.dirname(self.dump_path))

    def _dump_tables(self):
        db_backend = self.project.db_backend
        with open(self.dump_path, 'wb') as f:
            #f.write('begin transaction;\n\n')
            for table in db_backend.get_tables():
                if table in NO_DEPLOY_TABLES:
                    continue
                # ignore index tables on SQLite
                if table.startswith('idx_t'):
                    continue
                f.write('-- '  + '-' * 77 + '\n')
                f.write('-- Dump of table %s\n\n' % table)
                if db_backend.name == 'sqlite':
                    f.write('drop table %s;\n\n' % table)
                f.write(db_backend.dump(table))
            #f.write('\ncommit;\n')

    def _transfer_dump(self):
        config = self.project.config
        return project_manager.CommandResult.call_command(
            self.project,
            utils.rsync_command_line(
                config,
                utils.to_cygwin_path(self.dump_path),
                '{remote_server}:' + unixjoin(
                    config.remote_project_path, self.dump_local_path)))

    def _launch_restore(self): 
        project_cmd = ' '.join(
            self.project.build_command_line('restore_deploy', remote=True))
        deploy_cmd = (
            utils.ssh_command_line(self.project.config) + ' ' + project_cmd)

        return project_manager.CommandResult.call_command(
            self.project, deploy_cmd)

    # TODO: email in case of failure.
    def deploy(self):
        commands_result = project_manager.CommandsResult('deploy')
        self._dump_tables()
        commands_result.add_command_result(self._transfer_dump())
        commands_result.add_command_result(self._launch_restore())
        return commands_result

    def _restore_tables(self):
        sql = open(self.dump_path, 'rb').read()
        log.info('Restoring %s bytes of sql from %r', len(sql), self.dump_path)
        self.project.db_backend.restore(sql, dropdb=False)

    def restore_deploy(self):
        commands_result = project_manager.CommandsResult('restore_deploy')
        self.project.bgstop()
        self._restore_tables()
        self.project.bgstart()
