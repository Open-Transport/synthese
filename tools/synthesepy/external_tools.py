#    Scripts to manage external tools.
#    @file external_tools.py
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
import os
from os.path import join

from synthesepy import utils


log = logging.getLogger(__name__)


class ExternalTool(object):
    def __init__(self, project):
        self.project = project

    def generate_config(self):
        raise NotImplementedError()

    def system_install(self):
        raise NotImplementedError()

    def create_symlink(self, link_path, config_path):
        if os.path.isfile(link_path) and not os.path.islink(link_path):
            raise Exception(
                'There is already a file at %r. Not overwriting it' %
                link_path)
        if os.path.islink(link_path):
            os.unlink(link_path)

        log.info('Creating symlink %r -> %r', link_path, self.config_path)
        os.symlink(self.config_path, link_path)


class Supervisor(ExternalTool):
    def __init__(self, project):
        super(Supervisor, self).__init__(project)

        config = self.project.config
        self.config_name = 'synthese_{0}.conf'.format(
            config.project_name)
        self.config_path = join(
            config.project_path, 'conf', 'generated', 'supervisor',
            self.config_name)

    def generate_config(self):
        CONFIG_TEMPLATE = """[program:{program_name}]
user=synthese
command={synthese_py} --no-proxy -v -p {project_path} -s start
# Quotes are required due to a bug in 3.0a8:
# http://lists.supervisord.org/pipermail/supervisor-users/2010-March/000539.html
environment=HOME='/home/synthese',USER='synthese'
"""
        config = self.project.config

        format_config = config.__dict__.copy()
        # NOTE: supervisor doesn't seem to like underscores in names. Use
        # dashes instead.
        format_config['program_name'] = 'synthese-{0}'.format(config.project_name)
        format_config['synthese_py'] = join(
            os.environ['SYNTHESEPY_DIR'], 'synthese.py')
        supervisor_config = CONFIG_TEMPLATE.format(**format_config)

        utils.maybe_makedirs(os.path.dirname(self.config_path))
        with open(self.config_path, 'wb') as f:
            f.write(supervisor_config)
        log.info('Supervisor config written to %s', self.config_path)

    def _get_link_path(self):
        return join('/etc/supervisor/conf.d', self.config_name)

    def system_install(self):
        self.create_symlink(self._get_link_path(), self.config_path)
        utils.call('supervisorctl reread', shell=True)

    def system_uninstall(self):
        os.unlink(self._get_link_path())
        utils.call('supervisorctl reread', shell=True)
