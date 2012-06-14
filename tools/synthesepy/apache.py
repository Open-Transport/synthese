#    Apache config and daemon management.
#    @file apache.py
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
from os.path import join
import os

from synthesepy import external_tools
from synthesepy import utils


log = logging.getLogger(__name__)


class Apache(external_tools.ExternalTool):
    PROXY_TEMPLATE = '''
AddDefaultCharset UTF-8
Order allow,deny
Allow from all
ProxyPass http://localhost:{port}/{proxy_path} retry=1 timeout=3600
SetEnv proxy-nokeepalive 1
'''

    def __init__(self, project):
        super(Apache, self).__init__(project)

        config = self.project.config

        self.config_name = 'synthese_{0}{1}.conf'.format(
            config.project_name, project.config.apache_conf_suffix)
        self.config_path = join(
            config.project_path, 'conf', 'generated', 'apache',
            self.config_name)

    def _format_path(self, path):
        """
        From Apache documentation:
        # NOTE: Where filenames are specified, you must use forward slashes
        # instead of backslashes (e.g., "c:/apache" instead of "c:\apache").
        """
        return path.replace(os.sep, '/')

    def _get_admin_compat_directives(self, site):
        site_config = '#' * 80
        site_config += '\n# Config for site: {0}\n'.format(site.name)

        ADMIN_TEMPLATE = '''
# Admin site for backward compatibility.
<Location /synthese3/admin>
    Redirect /synthese3/admin/ /synthese3/admin?fonction=admin&mt=17732923532771328&tt=17732923532771329&pt=17732923532771330
    {synthese_proxy}
</Location>
# TODO: we should use /admin in the URLs used by Synthese to follow package conventions.
Alias /synthese3 "{admin_files_path}/admin/"
<Directory "{admin_files_path}/admin/">
    AddDefaultCharset UTF-8
    AllowOverride None
    Options None
    Order allow,deny
    Allow from all
</Directory>
        '''

        format_config = {}
        format_config['port'] = self.project.config.port
        format_config['admin_files_path'] = self._format_path(
            site.get_package('admin').files_path)
        format_config['proxy_path'] = 'synthese3/admin'
        format_config['synthese_proxy'] = self.PROXY_TEMPLATE.format(
            **format_config)
        site_config += ADMIN_TEMPLATE.format(**format_config)

        return site_config + '\n'

    def _get_rewrite_directives(self, site):
        rewrite_rules = site.rewrite_rules[:]

        # Root page
        if site.root_page_id > 0:
            rewrite_rules.insert(0,
                ['^$', 'synthese?SERVICE=page&p={0}'.format(site.root_page_id)])
        else:
            rewrite_rules.insert(0,
                ['^$', 'synthese?SERVICE=page&si={site_id}&smart_url=/'])

        # Fallback: call synthese with smart_url
        rewrite_rules.append(
            ['^(.*)$', 'synthese?SERVICE=page&si={site_id}&smart_url=/$1'])

        rewrite_directives = ''
        for rewrite_rule in rewrite_rules:
            if rewrite_rule[0] != '^$':
                rewrite_directives += 'RewriteCond %{REQUEST_FILENAME} !-f\n'
                rewrite_directives += 'RewriteCond %{REQUEST_FILENAME} !-d\n'
            rewrite_directives += 'RewriteRule {0} {1} [L,QSA]\n'.format(
                rewrite_rule[0], rewrite_rule[1].format(site_id=site.id))

        return rewrite_directives

    def _get_site_config(self, site, admin_package):
        # Admin is available on the virtual hosts. For backward compatibility,
        # an admin site can have "generate_apache_compat_config" parameter
        # set to True to generate the backward compatibility URLs.
        # Note that this restricts to having a single project running on the
        # system.
        if site.name == 'admin':
            if site.generate_apache_compat_config:
                return self._get_admin_compat_directives(site)
            return ''

        site_config = '#' * 80
        site_config += '\n# Config for site: {0}\n'.format(site.name)

        VHOST_TEMPLATE = '''
<VirtualHost {vhost_port}>
    ServerAdmin {server_admin}
    ServerName {server_name}
    {aliases}

    DocumentRoot {document_root}

    <Directory />
        Options -Indexes FollowSymLinks MultiViews
        AllowOverride None
    </Directory>
    <Directory {document_root}/>
        Options Indexes FollowSymLinks MultiViews
        AllowOverride None
        Order allow,deny
        allow from all

        RewriteEngine on
        RewriteBase /

        {custom_root_config}

        # Backward compatibility redirects to the new admin location.
        RewriteRule ^synthese3/admin/$ /admin/ [R=301,L]
        RewriteCond %{{QUERY_STRING}} fonction=admin
        RewriteRule ^synthese3/admin$ /admin/synthese [R=301,L]

        # Backward compatibility: the path /synthese3/admin is sometimes used to
        # access the synthese daemon (instead of the now recommended /synthese).
        RewriteRule ^synthese3/admin$ synthese [L]

        {rewrite_directives}
    </Directory>

    <Location /synthese>
        {synthese_proxy}
    </Location>

    # The admin requires a synthese URL under /admin.
    # TODO: Remove this once it's been migrated to the new package system.
    <Location /admin/synthese>
        {synthese_proxy_admin}
    </Location>

    # WSGI
    {wsgi_comment_switch}WSGIDaemonProcess {wsgi_process_name} user={synthese_user} group={synthese_group}
    {wsgi_comment_switch}WSGIProcessGroup {wsgi_process_name}

    WSGIScriptAlias /w {wsgi_script_path}
    <Directory {wsgi_script_dir}>
        Order allow,deny
        Allow from all
    </Directory>

    {custom_config}

    {package_aliases}

</VirtualHost>
'''

        PACKAGE_ALIAS_TEMPLATE = '''
# Package configuration for {package_name}
{package_pre_config}
Alias /{package_name} {package_files_path}/{package_name}
<Directory {package_files_path}>
    Options -Indexes FollowSymLinks MultiViews
    AllowOverride None
    Order allow,deny
    allow from all
    {rewrite_directives}
</Directory>
        '''

        format_config = self.project.config.__dict__.copy()
        format_config.update(site.__dict__)
        format_config['vhost_port'] = format_config.get(
            'apache_vhost_port', '*:80')
        format_config['server_name'] = format_config.get(
            'apache_server_name', site.name)
        format_config['server_admin'] = format_config.get(
            'apache_server_admin', 'webmaster@' + format_config['server_name'])

        if 'apache_aliases' in format_config:
            format_config['aliases'] = ('ServerAlias ' +
                ' '.join(format_config['apache_aliases']))
        else:
            format_config['aliases'] = ''

        format_config['document_root'] = self._format_path(
            self.project.htdocs_path)
        # DocumentRoot is set to the "main" package files by default
        main_package = site.get_package('main')
        if main_package:
            format_config['document_root'] = self._format_path(
                main_package.files_path)
        if site.htdocs_path:
            format_config['document_root'] = site.htdocs_path

        format_config['rewrite_directives'] = self._get_rewrite_directives(site)

        format_config['proxy_path'] = 'synthese'
        format_config['synthese_proxy'] = self.PROXY_TEMPLATE.format(
            **format_config)

        proxy_format_config = format_config.copy()
        proxy_format_config['proxy_path'] = 'admin/synthese'
        format_config['synthese_proxy_admin'] = self.PROXY_TEMPLATE.format(
            **proxy_format_config)

        # WSGI
        format_config['wsgi_comment_switch'] = (
            '#' if self.project.env.platform == 'win' else '')
        format_config['wsgi_process_name'] = \
            'synthese-{project_name}-{site_name}'.format(
                project_name=self.project.config.project_name,
                site_name=site.name)
        wsgi_script_path = join(
            self.project.path, 'conf', 'generated', 'wsgi', 'app.wsgi')
        format_config['wsgi_script_path'] = self._format_path(wsgi_script_path)
        format_config['wsgi_script_dir'] = self._format_path(
            os.path.dirname(wsgi_script_path))

        format_config['custom_config'] = format_config.get(
            'apache_custom_config', '')
        format_config['custom_config'] = \
            format_config['custom_config'].format(**format_config)
        format_config['custom_root_config'] = format_config.get(
            'apache_custom_root_config', '')
        format_config['custom_root_config'] = \
            format_config['custom_root_config'].format(**format_config)

        packages = site.packages[:]
        if admin_package:
            packages.append(admin_package)
        package_aliases = ''
        for package in packages:
            package_pre_config = ''
            rewrite_directives = ''
            package_custom_config = join(package.path, 'apache.conf')
            if os.path.isfile(package_custom_config):
                package_pre_config += open(package_custom_config).read()
            if package.name == 'admin':
                rewrite_directives += '''
    RewriteEngine on
    RewriteBase /
    RewriteRule ^admin/$ /admin/synthese?fonction=admin&mt=17732923532771328&tt=17732923532771329&pt=17732923532771330 [L,QSA]
                '''
            package_aliases += PACKAGE_ALIAS_TEMPLATE.format(
                package_name=package.name,
                package_pre_config=package_pre_config,
                package_files_path=self._format_path(package.files_path),
                rewrite_directives=rewrite_directives)
        format_config['package_aliases'] = package_aliases

        site_config += VHOST_TEMPLATE.format(**format_config)

        return site_config + '\n'

    def generate_config(self):
        apache_config = '# Generated file, do not edit.\n\n'

        # Admin site is special: an admin alias is added to every sites.
        # NOTE: this expects the site called 'admin' may contain an 'admin'
        # package.
        admin_site = self.project.get_site('admin')
        admin_package = None
        if admin_site and not admin_site.generate_apache_compat_config:
            admin_package = admin_site.get_package('admin')
        for site in self.project.sites:
            apache_config += self._get_site_config(site, admin_package)

        utils.maybe_makedirs(os.path.dirname(self.config_path))
        with open(self.config_path, 'wb') as f:
            f.write(apache_config)
        log.info('Apache config written to %s', self.config_path)

    def _get_link_path(self):
        return join('/etc/apache2/sites-enabled/', self.config_name)

    def system_install(self):
        self.create_symlink(self._get_link_path(), self.config_path)

        utils.call('a2enmod proxy', shell=True)
        utils.call('a2enmod proxy_http', shell=True)
        utils.call('a2enmod rewrite', shell=True)
        utils.call('a2enmod deflate', shell=True)
        utils.call('a2enmod setenvif', shell=True)
        utils.call('a2enmod headers', shell=True)
        utils.call('a2enmod filter', shell=True)
        utils.call('a2enmod wsgi', shell=True)
        utils.call('/etc/init.d/apache2 graceful', shell=True)

    def system_uninstall(self):
        os.unlink(self._get_link_path())
        # Note: not removing apache modules. They might be used by other things.
        utils.call('/etc/init.d/apache2 graceful', shell=True)
