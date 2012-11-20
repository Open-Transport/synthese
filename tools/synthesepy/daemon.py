#    Scripts to launch and manage the Synthese daemon.
#    @file daemon.py
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


import errno
import logging
import os
from os.path import join
import signal
import socket
import subprocess
import sys
import time

from synthesepy import proxy
from synthesepy import utils

log = logging.getLogger(__name__)


class DaemonException(Exception):
    pass


class Daemon(object):
    def __init__(self, env, project=None, use_proxy=True):
        self.env = env
        self.project = project
        self.use_proxy = self.env.c.wsgi_proxy
        if not use_proxy:
            self.use_proxy = False
        self.proc = None
        self.stopped = True
        self._ready = False

    def _start_wsgi_proxy(self):
        if not self.use_proxy:
            return
        proxy.start(self.env, self.project)

    def _stop_wsgi_proxy(self):
        if not self.use_proxy:
            return
        proxy.stop()

    def _wait_until_ready(self):
        # TODO: make it an option?
        WAIT_TIME_S = 10 * 60
        POLL_INTERVAL_S = 2
        for i in range(WAIT_TIME_S / POLL_INTERVAL_S):
            if not self.is_running():
                raise DaemonException(
                    'Server has exited prematurely. Check the logs for details.')
            if utils.can_connect(self.env.c.port, True):
                break
            time.sleep(POLL_INTERVAL_S)
        else:
            raise DaemonException('Server is not responding')

    def is_running(self):
        if not self.proc:
            return False
        return self.proc.poll() is None

    @property
    def ready(self):
        if not self.is_running():
            return False
        return self._ready

    def setsignals(self):
        def terminate(sig, frame):
            log.info('Got TERM signal, stopping daemon')
            self.stop()
        signal.signal(signal.SIGTERM, terminate)

    @classmethod
    def kill_existing(cls, env, kill_proxy=True):
        ports_to_check = [env.c.port]
        if kill_proxy and env.c.wsgi_proxy:
            ports_to_check.append(env.c.wsgi_proxy_port)
        for port in ports_to_check:
            utils.kill_listening_processes(port)

            if utils.can_connect(port):
                raise DaemonException(
                    'Error, something is already listening on port %s' % port)

    def _clean_pid_file(self):
        pid_path = join(self.env.c.project_path, 'synthese.pid')
        if os.path.isfile(pid_path):
            log.debug('Found pid file %s, removing it' % pid_path)
            # TODO: check if daemon is running with that pid and kill it if that's the case.
            try:
                os.unlink(pid_path)
            except OSError:
                # XXX not sure why that happens on the ci server.
                pass
        return pid_path

    def start(self, kill_proxy=True):
        self._ready = False
        self.env.prepare_for_launch()

        self.kill_existing(self.env, kill_proxy)

        if not os.path.isfile(self.env.daemon_path):
            raise DaemonException(
                'Daemon executable can\'t be found at %r. Project not built or '
                'wrong mode/tool?' % self.env.daemon_path)

        args = []
        if self.env.c.gdb:
            if not self.env.c.log_stdout:
                raise Exception('You must use the -s/--stdout option '
                    'with --gdb')
            args.extend(['gdb', '--args'])
        args.extend([
            self.env.daemon_path,
            '--dbconn', self.env.c.conn_string,
        ])

        if self.env.platform != 'win':
            pid_path = self._clean_pid_file()
            args.extend(['--pidfile', pid_path])

        params = {
            'log_level': str(self.env.c.log_level),
            'port': str(self.env.c.port),
        }
        if self.env.c.extra_params:
            for p in self.env.c.extra_params.split():
                name, value = p.split('=', 1)
                params[name] = value

        for name, value in params.iteritems():
            args.extend(['--param', name + '=' + value])
        log.debug('Args: %r\n%s', args, ' '.join(args))

        if self.env.c.log_stdout:
            stdout = sys.stdout
        else:
            log.info('Logging to %s', self.env.c.log_file)
            stdout = open(self.env.c.log_file, 'wb')

        if self.env.c.dummy:
            log.info('Dummy mode, not executing:\n%s\n in path: %s',
                ' '.join(args), self.env.daemon_launch_path)
            return

        self.setsignals()

        self.proc = subprocess.Popen(
            args,
            cwd=self.env.daemon_launch_path,
            stderr=subprocess.STDOUT,
            stdout=stdout)

        log.info('daemon started')
        self.stopped = False
        self._wait_until_ready()
        pid = -1
        if self.env.platform != 'win':
            pid = int(open(pid_path).read())
        log.info('daemon ready on port %i %s (script pid: %i)', self.env.c.port,
            '' if pid < 0 else 'with pid %i' % pid, os.getpid())
        self._ready = True
        try:
            self._start_wsgi_proxy()
        except socket.error, e:
            # XXX Ignore address already in use if restarted.
            if kill_proxy:
                raise
            log.warn('Ignoring address already in use for wsgi proxy')

    def stop(self):
        # TODO: should use quit action, but it doesn't work (at least on Windows)
        #  http://localhost:9080/synthese3/admin?a=QuitAction&co=0&sid=FKlwsUfU4lLCId38cCBI
        try:
            if not self.proc:
                return
            try:
                self.proc.terminate()
            except Exception, e:
                log.debug('Ignoring exception when calling terminate: %r', e)
            time.sleep(2)
            self.proc = None
        finally:
            self._clean_pid_file()
        self._stop_wsgi_proxy()
        assert not utils.can_connect(self.env.c.port, False)
        self.stopped = True
        self._ready = False
