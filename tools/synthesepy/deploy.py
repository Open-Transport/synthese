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

import datetime
import gzip
import logging
import os
from os.path import join
import socket
import time

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
    't003_alarms',
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

    # config
    't999_config',
])


def unixjoin(*paths):
    return '/'.join(paths)


class Dump(object):
    def __init__(self, id, path):
        self.id = id
        self.path = path
        self.date = datetime.datetime.fromtimestamp(os.path.getctime(self.path))
        self._logs = None

    def __repr__(self):
        return '<Dump %s>' % self.__dict__

    @property
    def logs(self):
        if self._logs:
            return self._logs
        try:
            self._logs = open(join(self.path, 'logs.txt')).read()
        except IOError:
            self._logs = 'N/A'
        return self._logs

    @logs.setter
    def logs(self, value):
        open(join(self.path, 'logs.txt'), 'wb').write(value)

    @property
    def sql_path(self):
        return join(self.path, 'dump.sql.gz')

    def delete(self):
        log.info('Deleting old dump: %s', self)
        utils.maybe_remove(self.path)


class Deployer(utils.DirObjectLoader):
    MAX_DUMPS_TO_KEEP = 500

    def __init__(self, project):
        self.project = project

        self._dumps = None
        self.deploy_path = join(project.path, 'deploy')
        self.lock_path = join(self.deploy_path, 'locked.txt')
        self.dumps_path = join(self.deploy_path, 'dumps')
        utils.maybe_makedirs(self.dumps_path)
        self.remote_dump_local_path = join('deploy', 'dump.sql.gz')

    def refresh(self):
        # Clear the dumps cache. It will be repopulated on next access.
        self._dumps = None

    @property
    def locked(self):
        return os.path.isfile(self.lock_path)

    @locked.setter
    def locked(self, locked):
        try:
            if locked:
                open(self.lock_path, 'w').write('Deploy locked')
            else:
                os.unlink(self.lock_path)
        except IOError, e:
            log.debug('Ignoring IOError while setting deploy lock: %s', e)

    def _load_dumps(self):
        if self._dumps is not None:
            return
        self._dumps = self.load_from_dir(self.dumps_path, Dump)

    def get_dumps(self):
        self._load_dumps()
        return self._dumps.values()
    dumps = property(get_dumps)

    def _get_dump(self, dump_id):
        self._load_dumps()
        return self._dumps[dump_id]

    def _create_dump(self):
        self._load_dumps()
        return self.create_object(self._dumps, self.dumps_path, Dump)

    def _dump_tables(self, dump):
        db_backend = self.project.db_backend
        f = gzip.open(dump.sql_path, 'wb')
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
                f.write('drop table if exists %s;\n\n' % table)
            f.write(db_backend.dump(table))
        #f.write('\ncommit;\n')
        f.close()

    def _transfer_dump(self, dump):
        config = self.project.config
        return project_manager.CommandResult.call_command(
            self.project,
            utils.rsync_command_line(
                config,
                utils.to_cygwin_path(dump.sql_path),
                '{remote_server}:' + unixjoin(
                    config.remote_project_path, self.remote_dump_local_path)))

    def _launch_remote(self, cmd):
        project_cmd = ' '.join(
            self.project.build_command_line(cmd, remote=True))
        deploy_cmd = (
            utils.ssh_command_line(self.project.config) + ' ' + project_cmd)

        return project_manager.CommandResult.call_command(
            self.project, deploy_cmd)

    def _send_fail_mail(self, commands_result):
        config = self.project.config
        subject = ('Synthese deploy failure on {hostname} '
            '(project: {project})'.format(
                hostname=socket.gethostname(),
                project=config.project_name))
        body = commands_result.summary()

        utils.send_mail(config, config.mail_admins, subject, body)

    def deploy(self, no_mail=False):
        try:
            commands_result = project_manager.CommandsResult('deploy')

            if self.locked:
                log.warn('Deploy is locked, not performing deploy.')
                return commands_result

            dump = self._create_dump()
            commands_result.add_command_result(
                project_manager.CommandResult.call_method(self._dump_tables, dump))

            self.deploy_restore(dump.id, commands_result)

            # Clean old dumps.
            for dump_to_delete in self.dumps[:-self.MAX_DUMPS_TO_KEEP]:
                dump_to_delete.delete()

            dump.logs = commands_result.summary()
            return commands_result
        except project_manager.CommandsException, e:
            if not no_mail:
                self._send_fail_mail(e.commands_result)
            raise e

    def deploy_restore(self, dump_id, commands_result=None):
        if not commands_result:
            commands_result = project_manager.CommandsResult('deploy_restore')
        dump = self._get_dump(dump_id)
        if not dump:
            raise Exception('Unable to find dump with id %s', dump_id)

        commands_result.add_command_result(self._launch_remote('deploy_remote_prepare'))
        commands_result.add_command_result(self._transfer_dump(dump))
        commands_result.add_command_result(self._launch_remote('deploy_remote_restore'))
        return commands_result

    def _restore_tables(self):
        sql_path = join(self.project.path, self.remote_dump_local_path)
        sql = gzip.open(sql_path, 'rb').read()
        log.info('Restoring %s bytes of sql from %r', len(sql), sql_path)
        self.project.db_backend.restore(sql, dropdb=False)

    def deploy_remote_prepare(self):
        utils.maybe_makedirs(self.deploy_path)

    def deploy_remote_restore(self):
        commands_result = project_manager.CommandsResult('deploy_remote_restore')
        self.project.bgstop()
        # TODO: this should wait until we are sure the daemon is stopped
        log.debug("Sleeping a while to let the daemon stop.")
        time.sleep(10)
        self._restore_tables()
        self.project.bgstart()
