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
    'mysqldump_opts': '',
 
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
    'restart_if_crashed': False,
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
    'send_mail_on_crash': False,
    'apache_conf_suffix': '',
    # web frontend configuration
    # Put in the project config.
    # Generate with: python -c "import os; print repr(os.urandom(24))"
    'web_secret_key': 'override me!',
    'web_debug': False,
    'web_admins': set(('root',)),

    # remote project management
    'remote_server': None,
    # TODO: should use new project layout by default
    'remote_db_path': '/srv/data/s3-server/config.db3',
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

    # system_install
    'synthese_user': 'synthese',
    'synthese_group': 'synthese',
}


class Config(object):
    def __init__(self):
        self.__dict__.update(DEFAULTS)

        self.thirdparty_dir = os.environ.get(
            'SYNTHESE_THIRDPARTY_DIR', os.path.expanduser('~/.synthese'))

    def _get_config_globals(self, path):
        return {
            'to_cygwin_path': utils.to_cygwin_path,
            '__file__': path,
        }

    def update_from_files(self, config_names, config_path):
        conf_dir = config_path if config_path else join(
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
                conf_dir, 'config{suffix}.py'.format(suffix=suffix))
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

    def update_finished(self):
        """Should be called once the config object is finished being updated"""
        # Hack to avoid passing arguments to all invocations.
        if self.netstat_cmd:
            utils.netstat_cmd = self.netstat_cmd

        utils.dummy = self.dummy

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

    def __repr__(self):
        return '<Config %s>' % self.__dict__
