#    Misc utilities.
#    @file utils.py
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
import subprocess
import sys

log = logging.getLogger(__name__)


def kill_listening_processes(port):
    '''Kills all processes listening for TCP connections on the given port'''
    if sys.platform == 'win32':
        import win32api
        import win32con

        # Note: use subprocess.check_output with 2.7
        output = subprocess.Popen(['netstat', '-noa'], stdout=subprocess.PIPE).communicate()[0]
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
            handle = win32api.OpenProcess(win32con.PROCESS_TERMINATE, 0, pid)
            win32api.TerminateProcess(handle, 0)
            win32api.CloseHandle(handle)

        return

    # Note: use subprocess.check_output with 2.7
    output = subprocess.Popen(['netstat', '-pln'], stdout=subprocess.PIPE).communicate()[0]
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


# XXX unused, remove?
def kill_all(process_name):
    if sys.platform == 'win32':
        import win32api
        import win32con
        import win32pdhutil

        try:
            pids = win32pdhutil.FindPerformanceAttributesByName(process_name, counter='ID Process')
        except Exception, e:
            return
        log.debug('Found pids to kill: %s', pids)
        for pid in pids:
            handle = win32api.OpenProcess(win32con.PROCESS_TERMINATE, 0, pid)
            win32api.TerminateProcess(handle, 0)
            win32api.CloseHandle(handle)
        return

    args = ['killall', '-KILL', '-u', os.getenv('USER'), process_name]
    subprocess.check_call(args)


def append_paths_to_environment(env_variable, paths):
    '''
    Adds the given paths to the specified environment variable

    Paths will be separated by os.pathsep and new paths aren't added if they
    are already present.
    '''
    oldval = os.environ.get(env_variable, '')
    paths = [p for p in paths if p not in oldval]
    if not paths:
        return
    newval = oldval
    if newval and not newval.endswith(os.pathsep):
        newval += os.pathsep
    os.environ[env_variable] = newval + os.pathsep.join(paths)


def find_executable(executable):
    """Returns the path to the given executable if found in PATH, or None otherwise"""
    for p in os.environ['PATH'].split(os.pathsep):
        target = os.path.join(p, executable)
        if os.path.isfile(target) and os.access(target, os.X_OK):
            return target
    return None
