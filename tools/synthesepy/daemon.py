import errno
import httplib
import logging
import os
import subprocess
import sys
import threading
import time
import urllib2
#    Scripts to launch and manage the Synthese daemon.
#    @file daemon.py
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
from wsgiref import simple_server

import static
from paste.proxy import Proxy

from . import utils

log = logging.getLogger(__name__)


class DaemonException(Exception):
    pass


class WSGIProxy(object):
    '''
    Runs a HTTP server to serve static files. Requests for the Synthese daemon
    are proxied to its configured port.
    '''

    ADMIN_PREFIXES = ('/synthese3/', '/admin/')
    SYNTHESE_SUFFIXES = ('/synthese3', '/admin')

    DEFAULT_ADMIN_QUERYSTRING = '?fonction=admin&mt=177329235327713281&tt=177329235327713282&pt=177329235327713283'

    def __init__(self, env):
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
            return self.other_static_app(environ, start_response)

        return static.StatusApp('404 Not Found')(environ, start_response)


class Daemon(object):
    def __init__(self, env):
        self.env = env
        self.proc = None

    def _start_wsgi_proxy(self):
        self.wsgi_httpd = simple_server.make_server(
            '', self.env.wsgi_proxy_port, WSGIProxy(self.env)
        )
        log.info('WSGI proxy serving on http://localhost:%s' %
                 self.env.wsgi_proxy_port)

        threading.Thread(target=self.wsgi_httpd.serve_forever).start()

    def _stop_wsgi_proxy(self):
        self.wsgi_httpd.shutdown()

    def _can_connect(self, port, verbose=False):
        url = 'http://localhost:%s' % port
        try:
            req = urllib2.urlopen(url, timeout=5).read()
        # Linux may raise BadStatusLine when failing to connect.
        except (urllib2.URLError, httplib.BadStatusLine):
            e = sys.exc_info()[1]
            if verbose:
                log.debug('Exception in _can_connect: %s', e)
            return False
        return True

    def _wait_until_ready(self):
        for i in range(40):
            if self.proc.poll() is not None:
                raise DaemonException(
                    'Server has exited prematurely. Check the logs for details.'
                )
            if self._can_connect(self.env.port, True):
                break
            time.sleep(2)
        else:
            raise DaemonException('Server is not responding')

    def start(self):

        for port in [self.env.port, self.env.wsgi_proxy_port]:
            utils.kill_listening_processes(port)

            if self._can_connect(port):
                raise DaemonException(
                    'Error, something is already listening on port %s', port
                )

        # cleanup pid on linux
        if self.env.platform != 'win':
            pid_path = os.path.join(
                self.env.daemon_launch_path,
                's3_server.pid'
            )
            if os.path.isfile(pid_path):
                log.debug('Found pid file %s, removing it' % pid_path)
                # TODO: check if daemon is running with that pid and kill it if that's the case.
                os.unlink(pid_path)

        if not os.path.isfile(self.env.daemon_path):
            raise DaemonException(
                'Daemon executable can\'t be found at "%s". Project not built or '
                'wrong mode/tool?' % self.env.daemon_path
            )

        args = [
            self.env.daemon_path,
            '--dbconn', self.env.conn_string,
            '--param', 'log_level=-1',
            '--param', 'port=%s' % self.env.port
        ]
        if self.env.extra_params:
            for p in self.env.extra_params.split():
                args.extend(['--param', p])
        log.debug('Args: %s', args)

        if self.env.log_stdout:
            stdout = sys.stdout
        else:
            log.debug('Logging to %s', self.env.daemon_log_file)
            stdout = open(self.env.daemon_log_file, 'wb')

        self.proc = subprocess.Popen(
            args,
            cwd=self.env.daemon_launch_path,
            stderr=subprocess.STDOUT,
            stdout=stdout,
            env=self.env.daemon_run_env
        )
        log.info('daemon started')

        self._wait_until_ready()
        log.info('daemon ready on port %s' % self.env.port)
        self._start_wsgi_proxy()

    def stop(self):
        # TODO: should use quit action, but it doesn't work (at least on Windows)
        #  http://localhost:9080/synthese3/admin?a=QuitAction&co=0&sid=FKlwsUfU4lLCId38cCBI
        if not self.proc:
            return
        self.proc.terminate()
        time.sleep(2)
        self.proc = None
        self._stop_wsgi_proxy()
        assert not self._can_connect(self.env.port, False)
