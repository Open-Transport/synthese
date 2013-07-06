#!/usr/bin/env python
#
#    Configuration management.
#    @file config.py
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
import multiprocessing
import os
from os.path import join
import socket

from synthesepy import utils

log = logging.getLogger(__name__)


DEFAULTS = {
    # General
    'dummy': False,
    'verbose': False,
    'spatialite_path': 'spatialite',
    'beep_when_done': False,
    'env_configs': {},
    'env_config_names': '',
    'mysqldump_opts': '--skip-triggers',
    'conf_dir': None,
    'dashboard_listen_address': '127.0.0.1',

    # bgstart/bgstop
    # One of: supervisor, initd, python, dummy
    'bg_process_manager': 'python',
    'supervisorctl_path': '/usr/bin/supervisorctl',

    # Environment (only used during initialization,
    # use the env object afterwards)
    'env_type': 'cmake',
    'env_path': None,
    # One of: debug, release, relwithdebinfo
    'mode': 'debug',
    # Daemon
    'port': 8080,
    'no_proxy': False,
    'site_id': 0,
    'default_site': None,
    'log_level': 'info',
    'log_stdout': False,
    'log_file': None,
    'gdb': False,
    'netstat_cmd': None,
    'restart_if_crashed_or_hung': False,
    # For fine tuning, you can set these options separately instead of the
    # global one above (the global one takes precedence).
    'restart_if_crashed': False,
    'restart_if_hung': False,

    'no_root_check': False,
    'extra_params': None,

    'wsgi_proxy': True,
    'wsgi_proxy_port': None,
    'synthese_suffixes': [],

    # project
    'project_path': None,
    'project_name': None,
    'conn_string': 'sqlite://',
    'sites_config': {},
    'mail_admins': (),
    'mail_sender': None,
    'mail_host': 'localhost',
    'mail_port': 25,
    'mail_user': None,
    'mail_password': None,
    'mail_tls': False,
    'use_udf_proxy': False,
    'udf_proxy_port': 9080,
    'udf_proxy_options': '',
    'udf_proxy_dispatch_ports': None,
    'use_s3_proxy': False,
    's3_proxy_port': 9080,
    's3_proxy_password': 'changeme',

    'send_mail_on_restart': False,
    'apache_conf_suffix': '',
    # web frontend configuration
    # Put in the project config.
    # Generate with: python -c "import os; print repr(os.urandom(24))"
    'web_secret_key': 'override me!',
    'web_debug': False,
    'web_admins': set(('root',)),

    # remote project management and deployment
    'remote_server': None,
    'remote_project_path': '/srv/synthese/{project_name}',
    # If dealing with an old-style project, use this instead:
    # '/srv/data/s3-server/config.db3',
    'remote_db_path': None,
    'remote_dump_prefix': 'remote_',

    'ssh_global_opts': '',
    'ssh_opts': '',
    'rsync_opts': '',

    # build
    'generate_only': False,
    'prefix': None,
    'mysql_params': None,
    'without_mysql': False,
    'mysql_dir': None,
    'boost_dir': None,
    'parallel_build': multiprocessing.cpu_count(),
    'x64': True,
    'kill_daemons_when_building': False,
    'build_only': None,
    'clear_cmake_cache': False,

    # runtests
    'test_conn_strings': ['sqlite://', 'mysql://db=synthese_test,host=localhost,user=synthese,passwd=synthese'],
    'no_init': False,
    'test_daemon_only': False,

    # package
    'packages_save_path': None,
    'packages_access_url': None,
    'no_package_overwrite': False,

    # continuous_integration
    'no_clean_if_build_fails': False,
    'suites': None,
    'should_build_package': lambda env: False,
    'force_create_package': False,
    'no_tests': False,
    'do_not_build_python': False,

    # system_install
    'synthese_user': 'synthese',
    'synthese_group': 'synthese',

    # ineo realtime
    'ineo_conn_string': None,
    'ineo_stop_code_prefix': '',
    'ineo_ver': 1,
    'ineo_planned_data_source': '16607027920896001',
    'ineo_realtime_data_source': '16607027920896002',
}


class Config(object):
    def __init__(self):
        self.__dict__.update(DEFAULTS)

        self.thirdparty_dir = os.environ.get(
            'SYNTHESE_THIRDPARTY_DIR', os.path.expanduser('~/.synthese'))
        self.conf_dir = join(self.thirdparty_dir, 'config')

    def _get_config_globals(self, path):
        return {
            'to_cygwin_path': utils.to_cygwin_path,
            '__file__': path,
        }

    def update_from_files(self, config_names, config_path):
        self.conf_dir = config_path if config_path else join(
            self.thirdparty_dir, 'config')

        all_configs = {}

        suffixes = ['', '_local', '_local_' + socket.gethostname()]
        system_suffix_file = join(
            self.thirdparty_dir, 'config_suffix.txt')
        try:
            system_suffix = open(system_suffix_file).read().strip()
            suffixes.append('_local_' + system_suffix)
        except IOError:
            pass

        for suffix in suffixes:
            path = join(
                self.conf_dir, 'config{suffix}.py'.format(suffix=suffix))
            log.debug('Trying to read config file: %r', path)
            if os.path.isfile(path):
                log.debug('Reading config file: %r', path)
                execfile(path, self._get_config_globals(path), all_configs)

        log.debug('All configs: %r', all_configs)

        if 'common' in all_configs:
            config_names.insert(0, 'common')
        for config_name in config_names:
            if not config_name in all_configs:
                raise Exception('No entry %r in config files' % config_name)
            self.__dict__.update(all_configs[config_name])

    def update_from_file(self, config_path):
        execfile(
            config_path, self._get_config_globals(config_path), self.__dict__)

    def update_from_dict(self, dict):
        self.__dict__.update(dict)

    def update_finished(self, env):
        """Should be called once the config object is finished being updated"""
        # Hack to avoid passing arguments to all invocations.
        if self.netstat_cmd:
            utils.netstat_cmd = self.netstat_cmd

        utils.dummy = self.dummy

        # Backward compatibility. To be removed in the future.
        if (not self.send_mail_on_restart and
            hasattr(self, 'send_mail_on_crash')):
            self.send_mail_on_restart = self.send_mail_on_crash

        # Convert log level to int
        try:
            self.log_level = int(self.log_level)
        except ValueError:
            levels = ['trace', 'debug', 'info', 'warn', 'fatal', 'none']
            level_name_to_int = dict(zip(levels, range(-1, len(levels))))
            if not self.log_level in level_name_to_int:
                raise Exception(
                    'Invalid log level value {0!r} allowed: {1!r}'.format(
                        self.log_level, levels))
            self.log_level = level_name_to_int[self.log_level]

        self.remote_project_path = self.remote_project_path.format(
            project_name=self.project_name)

        # import here to prevent import cycle errors
        from synthesepy import db_backends
        self.ineo_db = None
        if self.ineo_conn_string:
            self.ineo_db = db_backends.create_backend(env, self.ineo_conn_string)

    def __repr__(self):
        return '<Config %s>' % self.__dict__
