#    Scripts to manage external tools.
#    @file external_tools.py
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
        self.config_name = '{0}.conf'.format(
            self.get_config_name())
        self.config_path = join(
            config.project_path, 'conf', 'generated', 'supervisor',
            self.config_name)

    def generate_config(self):
        CONFIG_TEMPLATE = """# Generated file, do not edit!
[program:{program_name}]
user=synthese
command={command}
# Quotes are required due to a bug in 3.0a8:
# http://lists.supervisord.org/pipermail/supervisor-users/2010-March/000539.html
environment=HOME='/home/synthese',USER='synthese'
"""
        config = self.project.config
        format_config = config.__dict__.copy()
        # NOTE: supervisor doesn't seem to like underscores in names. Use
        # dashes instead.
        format_config['program_name'] = self.get_config_name().replace('_', '-')
        format_config['command'] = self.get_command()
        log_file = self.get_log_file()
        if log_file:
            CONFIG_TEMPLATE += """
redirect_stderr=true
stdout_logfile={stdout_logfile}
stdout_logfile_maxbytes={stdout_logfile_maxbytes}
stdout_logfile_backups={stdout_logfile_backups}
"""
            format_config['stdout_logfile'] = os.path.abspath(log_file)
            format_config.setdefault('stdout_logfile_maxbytes', '500MB')
            format_config.setdefault('stdout_logfile_backups', '4')

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


class SyntheseSupervisor(Supervisor):
    def get_config_name(self):
        return 'synthese_{0}'.format(self.project.config.project_name)

    def get_log_file(self):
        return self.project.config.log_file

    def get_command(self):
        return '{synthese_py} --no-proxy -v -p {project_path} -s start'.format(
            synthese_py=self.project.env.synthesepy_path,
            project_path=self.project.config.project_path)


class UDFProxySupervisor(Supervisor):
    def get_config_name(self):
        return 'udf_proxy_{0}'.format(self.project.config.project_name)

    def get_log_file(self):
        return None

    def get_command(self):
        config = self.project.config

        ports = config.udf_proxy_dispatch_ports
        if not ports:
            ports = [config.port]
        dispatch_urls = ';'.join('http://localhost:{0}'.format(port) for port in ports)

        return ('python {script} -ns -p {udf_proxy_port} --log-path={log_path} '
            '-t "{dispatch_urls}" {udf_proxy_options} start'.format(
            script=join(
                self.project.env.source_path, 'utils',
                'udf_proxy', 'udf_proxy.py'),
            udf_proxy_port=config.udf_proxy_port,
            log_path=join(config.project_path,
                'logs', 'udf_proxy.log'),
            dispatch_urls=dispatch_urls,
            udf_proxy_options=config.udf_proxy_options))


class S3ProxySupervisor(Supervisor):
    def get_config_name(self):
        return 's3_proxy_{0}'.format(self.project.config.project_name)

    def get_log_file(self):
        return join(self.project.config.project_path, 'logs', 's3_proxy.log')

    def get_command(self):
        config = self.project.config
        dispatch_url = 'http://localhost:{0}'.format(config.port)
        return ('{s3_proxy} --port {s3_proxy_port} --target {dispatch_url} '
            '--password {s3_proxy_password}'.format(
            s3_proxy=self.project.env.s3_proxy_path,
            s3_proxy_port=config.s3_proxy_port,
            dispatch_url=dispatch_url,
            s3_proxy_password=config.s3_proxy_password))


class WSGI(ExternalTool):
    def __init__(self, project):
        super(WSGI, self).__init__(project)

    def generate_config(self):
        WSGI_APP_TEMPLATE = """# Generated file, do not edit!
import os
from os.path import join
import sys

bootstrap_config = {bootstrap_config}

activate_this = join(bootstrap_config['env_bin_dir'], 'activate_this.py')
execfile(activate_this, dict(__file__=activate_this))

sys.path.append(bootstrap_config['tools_dir'])
import synthesepy.web

application = synthesepy.web.get_application(bootstrap_config)
"""
        wsgi_path = join(
            self.project.config.project_path, 'conf', 'generated',
            'wsgi', 'app.wsgi')
        utils.maybe_makedirs(os.path.dirname(wsgi_path))

        env = self.project.env
        config = self.project.config
        wsgi_app_content = WSGI_APP_TEMPLATE.format(
            bootstrap_config=repr({
                'env_bin_dir': self.project.env.pyenv_bin_path,
                'tools_dir': self.project.env.tools_path,
                'env_type': env.type,
                'env_path': env.env_path,
                'mode': env.mode,
                'project_path': self.project.path}))

        with open(wsgi_path, 'wb') as f:
            f.write(wsgi_app_content)
        log.info('WSGI config written to %s', wsgi_path)

    def system_install(self):
        # Nothing to do.
        pass
