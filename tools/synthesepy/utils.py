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
