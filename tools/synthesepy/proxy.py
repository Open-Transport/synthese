#    HTTP proxy for serving static resources and forwarding requests to Synthese.
#    @file proxy.py
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
import threading
import time
from wsgiref import simple_server

import static
from paste.proxy import Proxy

from . import utils

log = logging.getLogger(__name__)


class WSGIProxy(object):
    '''
    Runs a HTTP server to serve static files. Requests for the Synthese daemon
    are proxied to its configured port.
    '''

    ADMIN_PREFIXES = ('/synthese3/', '/admin/')
    SYNTHESE_SUFFIXES = ('/synthese', '/synthese3', '/admin')

    DEFAULT_ADMIN_QUERYSTRING = '?fonction=admin&mt=177329235327713281&tt=177329235327713282&pt=177329235327713283'

    def __init__(self, env):
        self.env = env
        self.proxy_app = Proxy('http://localhost:%s/' % env.port)

        self.admin_static_app = static.Cling(
            os.path.join(env.admin_root_path, 'files')
        )

        self.other_static_app = None
        if env.static_dir:
            self.other_static_app = static.Cling(env.static_dir)

    def _redirect(self, environ, start_response, url):
        if not url.startswith('http://'):
            url = 'http://' + environ['HTTP_HOST'] + url
        start_response('302 Found', [
            ('Location', url),
            ('Content-type', 'text/plain')])
        return '302 Found'

    def _handle_static_files(self, environ, start_response):
        path_info = environ.get('PATH_INFO', '')
        path = self.other_static_app.root + path_info
        # Proxy to Synthese for smart urls.
        if not os.path.isfile(path):
            log.debug('File %s not found. Forwarding to Synthese for Smart URL', path)
            smart_url_path = path_info[1:]
            querystring = 'SERVICE=page&si={site_id}&smart_url=/{smart_url}'.format(
                site_id=self.env.site_id,
                smart_url=smart_url_path
            )
            environ['PATH_INFO'] = self.SYNTHESE_SUFFIXES[0]
            environ['QUERY_STRING'] = querystring
            return self.proxy_app(environ, start_response)
        return self.other_static_app(environ, start_response)

    def __call__(self, environ, start_response):
        path_info = environ['PATH_INFO']
        is_admin = path_info.startswith(self.ADMIN_PREFIXES)

        if path_info == '/':
            return self._redirect(
                environ,
                start_response,
                self.ADMIN_PREFIXES[0] +
                self.SYNTHESE_SUFFIXES[0].replace('/', '') +
                self.DEFAULT_ADMIN_QUERYSTRING
            )

        if path_info.endswith(self.SYNTHESE_SUFFIXES):
            if (is_admin and
                environ['REQUEST_METHOD'] == 'GET' and
                not environ['QUERY_STRING']):
                return self._redirect(
                    environ,
                    start_response,
                    path_info + self.DEFAULT_ADMIN_QUERYSTRING
                )

            # Force utf-8 content type
            def start_response_wrapper(status, headers):
                headers_dict = dict(headers)
                if headers_dict['Content-Type'] == 'text/html':
                    headers_dict['Content-Type'] = 'text/html; charset=UTF-8'
                return start_response(status, headers_dict.items())

            return self.proxy_app(environ, start_response_wrapper)

        if is_admin:
            # Remove the /admin or /synthese3 prefix
            environ['PATH_INFO'] = '/' + '/'.join(path_info.split('/')[2:])
            return self.admin_static_app(environ, start_response)

        if self.other_static_app:
            return self._handle_static_files(environ, start_response)

        return static.StatusApp('404 Not Found')(environ, start_response)


def start(env):
    wsgi_httpd = simple_server.make_server(
        '', env.wsgi_proxy_port, WSGIProxy(env)
    )
    log.info('WSGI proxy serving on http://localhost:%s' %
             env.wsgi_proxy_port)

    threading.Thread(target=wsgi_httpd.serve_forever).start()
    return wsgi_httpd


def serve_forever(env):
    utils.kill_listening_processes(env.wsgi_proxy_port)

    if utils.can_connect(env.wsgi_proxy_port):
        raise Exception(
            'Error, something is already listening on port %s', env.wsgi_proxy_port
        )

    proxy = start(env)
    log.info('Proxy running, press ctrl-c to stop')
    try:
        while True:
            time.sleep(10)
    except KeyboardInterrupt:
        print '^C'

    log.info('Stopping proxy')
    proxy.shutdown()
