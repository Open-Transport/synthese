#!/usr/bin/env python
#
#    Configuration management.
#    @file config.py
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
    
    # XXX name it thirdparty_path for consistency?
    'thirdparty_dir': None,
    # Environment
    'env_type': 'cmake',
    'mode': 'debug',
    # Daemon
    'port': 8080,
    'no_proxy': False,
    'site_id': 0,
    'log_stdout': False,
    'gdb': False,

    'wsgi_proxy': True,
    'wsgi_proxy_port': None,
    'synthese_suffixes': [],

    # project
    'project_name': None,
    'conn_string': 'sqlite://path=@PROJECT_PATH@/db/config.db3',

    # remote project management
    'server': None,
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

    # runtests
    'conn_strings': [],
    'no_init': False,

    # continuous_integration
    'test_env': {},
    'suites': None,
}


class Config(object):
    def __init__(self):
        self.__dict__.update(DEFAULTS)

        # Set defaults
        if not self.thirdparty_dir:
            self.thirdparty_dir = os.environ['SYNTHESE_THIRDPARTY_DIR']

    def _get_config_globals(self, path):
        return {
            'to_cygwin_path': utils.to_cygwin_path,
            '__file__': path,
        }

    def update_from_files(self, config_names, config_path):
        conf_dir = config_path if config_path else join(
            os.environ['SYNTHESE_THIRDPARTY_DIR'], 'config')

        all_configs = {}

        suffixes = ['', '_local', '_local_' + socket.gethostname()]
        system_suffix_file = join(
            os.environ['SYNTHESE_THIRDPARTY_DIR'], 'config_suffix.txt')
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

        config_names.insert(0, 'common')
        for config_name in config_names:
            if not config_name in all_configs:
                raise Exception('No entry %r in config files' % config_name)
            self.__dict__.update(all_configs[config_name])

    def update_from_file(self, config_path):
        execfile(
            config_path, self._get_config_globals(config_path), self.__dict__)

    def update_from_obj(self, obj):
        self.__dict__.update(obj.__dict__)

    def __repr__(self):
        return '<Config %s>' % self.__dict__
