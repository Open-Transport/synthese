#    Utilities
#    @file utils.py
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
import subprocess
import sys

log = logging.getLogger(__name__)


def maybe_makedirs(directory):
    '''Create a directory if it doesn't already exist'''
    if not os.path.isdir(directory):
        os.makedirs(directory)


netstat_cmd = ['netstat', '-pln']


def kill_listening_processes(port):
    '''Kills all processes listening for TCP connections on the given port'''
    if sys.platform == 'win32':
        # Note: use subprocess.check_output with 2.7
        output = subprocess.Popen(
            ['netstat', '-noa'], stdout=subprocess.PIPE).communicate()[0]
        pids = []
        for l in output.splitlines():
            try:
                proto, local, foreign, state, pid = l.split()
            except ValueError:
                continue
            if proto != 'TCP' or state != 'LISTENING':
                continue
            p = int(local.split(':')[-1])
            if port != p:
                continue
            pids.append(int(pid))

        for pid in pids:
            log.info('Killing pid: %i', pid)
            subprocess.check_call('taskkill /f /t /pid {0}'.format(pid))

        return

    # TODO: use subprocess.check_output once we require Python 2.7
    output = subprocess.Popen(
        netstat_cmd, stdout=subprocess.PIPE).communicate()[0]
    pids = []
    for l in output.splitlines():
        try:
            proto, recv, send, local, foreign, state, pid_prog = l.split()
        except ValueError:
            continue
        if not proto.startswith('tcp'):
            continue
        p = int(local.split(':')[-1])
        if port != p:
            continue
        pids.append(pid_prog.split('/')[0])

    for pid in pids:
        log.info('Killing pid: %s', pid)
        try:
            int(pid)
        except ValueError:
            log.warn('Non numeric pid %s, skipping', pid)
            continue
        args = ['kill', '-KILL', pid]
        subprocess.check_call(args)
