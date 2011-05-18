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

import errno
import logging
import os
import subprocess
import sys
import time

from . import proxy
from . import utils

log = logging.getLogger(__name__)


class DaemonException(Exception):
    pass


class Daemon(object):
    def __init__(self, env):
        self.env = env
        self.proc = None

    def _start_wsgi_proxy(self):
        if not self.env.wsgi_proxy:
            return
        self.wsgi_httpd = proxy.start(self.env)

    def _stop_wsgi_proxy(self):
        if not self.wsgi_httpd:
            return
        self.wsgi_httpd.shutdown()

    def _wait_until_ready(self):
        for i in range(40):
            if self.proc.poll() is not None:
                raise DaemonException(
                    'Server has exited prematurely. Check the logs for details.'
                )
            if utils.can_connect(self.env.port, True):
                break
            time.sleep(2)
        else:
            raise DaemonException('Server is not responding')

    def start(self):
        ports_to_check = [self.env.port]
        if self.env.wsgi_proxy:
            ports_to_check.append(self.env.wsgi_proxy_port)
        for port in ports_to_check:
            utils.kill_listening_processes(port)

            if utils.can_connect(port):
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
