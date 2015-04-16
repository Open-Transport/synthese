#    HTTP proxy for serving static resources and forwarding requests to Synthese.
#    @file proxy.py
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


import httplib
import logging
import os
import threading
import time
import urllib
import urlparse
from wsgiref import simple_server

import static
from paste.proxy import parse_headers
from paste.proxy import Proxy
import werkzeug.wsgi

from synthesepy import utils

log = logging.getLogger(__name__)

# The Synthese daemon expects a x-forwarded-host header when getting a proxied
# request. Paste proxy doesn't provide it, so we monkey patch the __call__
# method here. The modification from Paste are shown in comments.
def paste_proxy_patched_call(self, environ, start_response):
    if (self.allowed_request_methods and
        environ['REQUEST_METHOD'].lower() not in self.allowed_request_methods):
        return httpexceptions.HTTPBadRequest("Disallowed")(environ, start_response)

    if self.scheme == 'http':
        ConnClass = httplib.HTTPConnection
    elif self.scheme == 'https':
        ConnClass = httplib.HTTPSConnection
    else:
        raise ValueError(
            "Unknown scheme for %r: %r" % (self.address, self.scheme))
    conn = ConnClass(self.host)
    headers = {}
    for key, value in environ.items():
        if key.startswith('HTTP_'):
            key = key[5:].lower().replace('_', '-')
            if key == 'host' or key in self.suppress_http_headers:
                continue
            headers[key] = value
    headers['host'] = self.host
    if 'REMOTE_ADDR' in environ:
        headers['x-forwarded-for'] = environ['REMOTE_ADDR']
    # synthese modification
    if 'HTTP_HOST' in environ:
        headers['x-forwarded-host'] = environ['HTTP_HOST']
    # end of synthese modification
    if environ.get('CONTENT_TYPE'):
        headers['content-type'] = environ['CONTENT_TYPE']
    if environ.get('CONTENT_LENGTH'):
        if environ['CONTENT_LENGTH'] == '-1':
            # This is a special case, where the content length is basically undetermined
            body = environ['wsgi.input'].read(-1)
            headers['content-length'] = str(len(body))
        else:
            headers['content-length'] = environ['CONTENT_LENGTH']
            length = int(environ['CONTENT_LENGTH'])
            body = environ['wsgi.input'].read(length)
    else:
        body = ''

    path_info = urllib.quote(environ['PATH_INFO'])
    if self.path:
        request_path = path_info
        if request_path and request_path[0] == '/':
            request_path = request_path[1:]

        path = urlparse.urljoin(self.path, request_path)
    else:
        path = path_info
    if environ.get('QUERY_STRING'):
        path += '?' + environ['QUERY_STRING']

    conn.request(environ['REQUEST_METHOD'],
                 path,
                 body, headers)
    res = conn.getresponse()
    headers_out = parse_headers(res.msg)

    status = '%s %s' % (res.status, res.reason)
    start_response(status, headers_out)
    # @@: Default?
    length = res.getheader('content-length')
    if length is not None:
        body = res.read(int(length))
    else:
        body = res.read()
    conn.close()
    return [body]

Proxy.__call__ = paste_proxy_patched_call


class WSGIProxy(object):
    """
    Runs a HTTP server to serve static files. Requests for the Synthese daemon
    are proxied to its configured port.
    """

    SYNTHESE_SUFFIXES = ['/synthese', '/synthese3', '/admin']

    ADMIN_URL = '/admin/synthese?fonction=admin&mt=17732923532771328&tt=17732923532771329&pt=17732923532771330'

    def __init__(self, env, project):
        self.env = env
        self.proxy_app = Proxy('http://localhost:%s/' % env.c.port)

        # import here to avoid circular dependencies.
        from synthesepy import web
        self.web_app = web.get_application(project=project)

        self.static_apps = []
        for base, path in env.c.static_paths:
            self.static_apps.append((base, static.Cling(path)))

    def _redirect(self, environ, start_response, url):
        if not url.startswith('http://'):
            url = 'http://' + environ['HTTP_HOST'] + url
        start_response('302 Found', [
            ('Location', url),
            ('Content-type', 'text/plain')])
        return '302 Found'

    def add_utf8_header(self, start_response):
        def start_response_wrapper(status, headers):
            headers_dict = dict(headers)
            if headers_dict['Content-Type'] == 'text/html':
                headers_dict['Content-Type'] = 'text/html; charset=UTF-8'
            return start_response(status, headers_dict.items())
        return start_response_wrapper

    def _handle_static_files(self, environ, start_response):
        path_info = environ.get('PATH_INFO', '')

        for base, app in self.static_apps:
            if not path_info[1:].startswith(base):
                continue
            path_info = '/' + path_info[1 + len(base):]
            environ['PATH_INFO'] = path_info

            path = app.root + path_info
            if (os.path.isfile(path) or
                (path_info.endswith('/') and os.path.isfile(
                    os.path.join(path, 'index.html')))):
                return app(environ, start_response)

        log.debug('Path %r not found in any static directories, forwarding '
            'to Synthese for Smart URL', path_info)
        smart_url_path = path_info[1:]
        # Warning: this removes duplicate GET parameters.
        qs = dict(urlparse.parse_qsl(environ['QUERY_STRING']))
        qs.update(dict(
            SERVICE='page',
            si=self.env.c.site_id,
            smart_url='/' + smart_url_path,
        ))
        environ['PATH_INFO'] = self.SYNTHESE_SUFFIXES[0]
        environ['QUERY_STRING'] = urllib.urlencode(qs)
        return self.proxy_app(environ, self.add_utf8_header(start_response))

    def __call__(self, environ, start_response):
        path_info = environ['PATH_INFO']

        # Web app
        WEB_APP_PREFIX = '/w/'
        if path_info.startswith(WEB_APP_PREFIX):
            werkzeug.wsgi.pop_path_info(environ)
            return self.web_app(environ, start_response)

        # Admin redirect helpers.
        if path_info in ('/admin', '/admin/'):
            return self._redirect(environ, start_response, self.ADMIN_URL)

        if path_info.endswith(
            tuple(self.SYNTHESE_SUFFIXES + self.env.c.synthese_suffixes)):
            return self.proxy_app(environ, self.add_utf8_header(start_response))

        return self._handle_static_files(environ, start_response)


# Paste httpd is threaded, which should provide better performance.
USE_PASTE_HTTPD = True

wsgi_httpd = None


def start(env, project):
    global wsgi_httpd

    if USE_PASTE_HTTPD:
        import paste.httpserver
        paste_log = logging.getLogger('paste.httpserver.ThreadPool')
        paste_log.setLevel(logging.WARNING)
        wsgi_httpd = paste.httpserver.serve(
            WSGIProxy(env, project), '0.0.0.0', env.c.wsgi_proxy_port, start_loop=False)
    else:
        wsgi_httpd = simple_server.make_server(
            '', env.c.wsgi_proxy_port, WSGIProxy(env))
    log.info('WSGI proxy serving on http://localhost:%s' %
             env.c.wsgi_proxy_port)

    threading.Thread(target=wsgi_httpd.serve_forever).start()


def stop():
    global wsgi_httpd
    # Shutting down method differs:
    # simple_server.simple_server throws an exception when calling
    # server_close() and paste.httpd hangs if shutdown() is called.
    if USE_PASTE_HTTPD:
        wsgi_httpd.server_close()
    else:
        wsgi_httpd.shutdown()


def serve_forever(env, project):
    utils.kill_listening_processes(env.c.wsgi_proxy_port)

    if utils.can_connect(env.c.wsgi_proxy_port):
        raise Exception(
            'Error, something is already listening on port %s',
            env.c.wsgi_proxy_port)

    proxy = start(env, project)
    log.info('Proxy running, press ctrl-c to stop')
    try:
        while True:
            time.sleep(10)
    except KeyboardInterrupt:
        print '^C'

    log.info('Stopping proxy')
    stop()
