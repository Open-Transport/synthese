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
        if not self.env.c.wsgi_proxy:
            return
        proxy.start(self.env)

    def _stop_wsgi_proxy(self):
        if not self.env.c.wsgi_proxy:
            return
        proxy.stop()

    def _wait_until_ready(self):
        for i in range(40):
            if not self.is_running():
                raise DaemonException(
                    'Server has exited prematurely. Check the logs for details.')
            if utils.can_connect(self.env.c.port, True):
                break
            time.sleep(2)
        else:
            raise DaemonException('Server is not responding')

    def is_running(self):
        if not self.proc:
            return False
        return self.proc.poll() is None

    def start(self):
        self.env.prepare_for_launch()

        ports_to_check = [self.env.c.port]
        if self.env.c.wsgi_proxy:
            ports_to_check.append(self.env.c.wsgi_proxy_port)
        for port in ports_to_check:
            utils.kill_listening_processes(port)

            if utils.can_connect(port):
                raise DaemonException(
                    'Error, something is already listening on port %s', port)

        # cleanup pid on linux
        if self.env.platform != 'win':
            pid_path = os.path.join(
                self.env.daemon_launch_path,
                's3_server.pid')
            if os.path.isfile(pid_path):
                log.debug('Found pid file %s, removing it' % pid_path)
                # TODO: check if daemon is running with that pid and kill it if that's the case.
                os.unlink(pid_path)

        if not os.path.isfile(self.env.daemon_path):
            raise DaemonException(
                'Daemon executable can\'t be found at %r. Project not built or '
                'wrong mode/tool?' % self.env.daemon_path)

        args = []
        if self.env.c.gdb:
            args.extend(['gdb', '--args'])
        args.extend([
            self.env.daemon_path,
            '--dbconn', self.env.c.conn_string,
        ])

        params = {
            'log_level': '-1',
            'port': str(self.env.c.port),
        }
        if self.env.c.extra_params:
            for p in self.env.c.extra_params.split():
                name, value = p.split('=', 1)
                params[name] = value

        for name, value in params.iteritems():
            args.extend(['--param', name + "=" + value])
        log.debug('Args: %r\n%s', args, ' '.join(args))

        if self.env.c.log_stdout:
            stdout = sys.stdout
        else:
            log.info('Logging to %s', self.env.daemon_log_file)
            stdout = open(self.env.daemon_log_file, 'wb')

        if self.env.c.dummy:
            log.info('Dummy mode, not executing:\n%s\n in path: %s',
                ' '.join(args), self.env.daemon_launch_path)
            return

        self.proc = subprocess.Popen(
            args,
            cwd=self.env.daemon_launch_path,
            stderr=subprocess.STDOUT,
            stdout=stdout)
        log.info('daemon started')

        self._wait_until_ready()
        log.info('daemon ready on port %s' % self.env.c.port)
        self._start_wsgi_proxy()

    def stop(self):
        # TODO: should use quit action, but it doesn't work (at least on Windows)
        #  http://localhost:9080/synthese3/admin?a=QuitAction&co=0&sid=FKlwsUfU4lLCId38cCBI
        if not self.proc:
            return
        try:
            self.proc.terminate()
        except Exception, e:
            log.debug('Ignoring exception when calling terminate: %r', e)
        time.sleep(2)
        self.proc = None
        self._stop_wsgi_proxy()
        assert not utils.can_connect(self.env.c.port, False)
