#    Misc utilities.
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


import email.mime.text
import errno
import httplib
import logging
import os
import shutil
import smtplib
import stat
import subprocess
import sys
import time
import urllib2
from xml.etree import ElementTree

log = logging.getLogger(__name__)

# Globals
netstat_cmd = ['netstat', '-pln']
dummy = False


def kill_listening_processes(port):
    '''Kills all processes listening for TCP connections on the given port'''
    if sys.platform == 'win32':
        import win32api
        import win32con

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
            handle = win32api.OpenProcess(win32con.PROCESS_TERMINATE, 0, pid)
            win32api.TerminateProcess(handle, 0)
            win32api.CloseHandle(handle)

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


def kill_processes(process_name):
    if sys.platform != 'win32':
        raise NotImplementedError('Implement me!')
    try:
        import win32api
        import win32pdhutil
        import win32con
    except ImportError:
        log.warn('Python win32 is required for killing processes.')
        return

    pids = win32pdhutil.FindPerformanceAttributesByName(
        process_name, counter="ID Process")

    for pid in pids:
        handle = win32api.OpenProcess(win32con.PROCESS_TERMINATE, False, pid)
        win32api.TerminateProcess(handle, -1)
        win32api.CloseHandle(handle)


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


def can_connect(port, verbose=False):
    """Returns whether it is possible to reach the given port on localhost with HTTP"""
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


def to_cygwin_path(path):
    if sys.platform != 'win32':
        return path
    try:
        cygproc = subprocess.Popen(
            ('cygpath', '-a', '-u', path), stdout=subprocess.PIPE)
    except OSError:
        log.warn('Can\'t convert path to cygwin format')
        return path
    (stdout_content, stderr_content) = cygproc.communicate()
    return stdout_content.rstrip()


def call(cmd, shell=None, **kwargs):

    if shell is None:
        shell = not isinstance(cmd, list)

    cmdline = cmd
    if isinstance(cmd, list):
        cmdline = ' '.join(cmdline)
    dir = ('(in directory: {0!r})'.format(kwargs['cwd']) if
        'cwd' in kwargs else '')
    # TODO: show environment too.
    log.info(
        '%s %s:\n%s',
        'Dummy mode, not running:' if dummy else 'Running:',
        dir, cmdline)

    if 'input' in kwargs:
        log.debug('With %s bytes of input', len(kwargs['input']))

    if dummy:
        return

    if 'bg' in kwargs:
        del kwargs['bg']
        subprocess.Popen(cmd, **kwargs)
        return
    if 'input' in kwargs:
        input = kwargs['input']
        del kwargs['input']
        p = subprocess.Popen(
            cmd, shell=shell, stdin=subprocess.PIPE, stdout=subprocess.PIPE)
        output = p.communicate(input)[0]
        if p.returncode:
            raise subprocess.CalledProcessError(p.returncode, cmd)
        return output

    subprocess.check_call(cmd, shell=shell, **kwargs)


# From http://src.chromium.org/svn/trunk/tools/build/scripts/common/chromium_utils.py
# Copyright (c) 2010 The Chromium Authors. All rights reserved.
# Use of this source code is governed by a BSD-style license that can be
# found in the LICENSE file.
def RemoveDirectory(*path):
  """Recursively removes a directory, even if it's marked read-only.

  Remove the directory located at *path, if it exists.

  shutil.rmtree() doesn't work on Windows if any of the files or directories
  are read-only, which svn repositories and some .svn files are.  We need to
  be able to force the files to be writable (i.e., deletable) as we traverse
  the tree.

  Even with all this, Windows still sometimes fails to delete a file, citing
  a permission error (maybe something to do with antivirus scans or disk
  indexing).  The best suggestion any of the user forums had was to wait a
  bit and try again, so we do that too.  It's hand-waving, but sometimes it
  works. :/
  """
  file_path = os.path.join(*path)
  if not os.path.exists(file_path):
    return

  def RemoveWithRetry_win(rmfunc, path):
    os.chmod(path, stat.S_IWRITE)
    if win32_api_avail:
      win32api.SetFileAttributes(path, win32con.FILE_ATTRIBUTE_NORMAL)
    try:
      return rmfunc(path)
    except EnvironmentError, e:
      if e.errno != errno.EACCES:
        raise
      print 'Failed to delete %s: trying again' % repr(path)
      time.sleep(0.1)
      return rmfunc(path)

  def RemoveWithRetry_non_win(rmfunc, path):
    if os.path.islink(path):
      return os.remove(path)
    else:
      return rmfunc(path)

  win32_api_avail = False
  remove_with_retry = None
  if sys.platform.startswith('win'):
    # Some people don't have the APIs installed. In that case we'll do without.
    try:
      win32api = __import__('win32api')
      win32con = __import__('win32con')
      win32_api_avail = True
    except ImportError:
      pass
    remove_with_retry = RemoveWithRetry_win
  else:
    remove_with_retry = RemoveWithRetry_non_win

  for root, dirs, files in os.walk(file_path, topdown=False):
    # For POSIX:  making the directory writable guarantees removability.
    # Windows will ignore the non-read-only bits in the chmod value.
    os.chmod(root, 0770)
    for name in files:
      remove_with_retry(os.remove, os.path.join(root, name))
    for name in dirs:
      remove_with_retry(os.rmdir, os.path.join(root, name))

  remove_with_retry(os.rmdir, file_path)


def copy_over(source_path, target_path, overwrite=True):
    """Copy source_path over target_path, replacing any existing files"""

    base_parts_count = len(source_path.split(os.path.sep))
    for path, dirlist, filelist in os.walk(source_path):
        for exclude in ['.git', '.hg', '.svn']:
            if exclude in dirlist:
                dirlist.remove(exclude)
        for name in filelist:
            relative_path = os.sep.join(path.split(os.sep)[base_parts_count:])

            source = os.path.join(path, name)
            target = os.path.join(target_path, relative_path, name)
            if not overwrite and os.path.isfile(target):
                continue

            if not os.path.isdir(os.path.dirname(target)):
                os.makedirs(os.path.dirname(target))
            shutil.copy(source, target)


def maybe_makedirs(directory):
    '''Create a directory if it doesn't already exist'''
    if not os.path.isdir(directory):
        os.makedirs(directory)


def maybe_remove(path):
    '''Remove given file or directory if it exists'''
    if os.path.isdir(path):
        return RemoveDirectory(path)
    try:
        os.unlink(path)
    except OSError:
        pass


_mail_conn = None

def send_mail(config, recipients, subject, body):
    if not recipients:
        return

    log.info('Sending mail %r to %s', subject, recipients)

    global _mail_conn

    if not _mail_conn:
        _mail_conn = smtplib.SMTP(config.mail_host, config.mail_port)

        if config.mail_tls:
            _mail_conn.ehlo()
            _mail_conn.starttls()
            _mail_conn.ehlo()
        if config.mail_user and config.mail_password:
            _mail_conn.login(config.mail_user, config.mail_password)

    msg = email.mime.text.MIMEText(body, _charset='utf-8')

    msg['Subject'] = subject
    msg['From'] = config.mail_sender
    msg['To'] = ', '.join(recipients)

    _mail_conn.sendmail(config.mail_sender, recipients, msg.as_string())


class SVNInfo(object):
    '''Class to retrieve metadata from a svn repository'''
    def __init__(self, repo_path):
        self.repo_path = repo_path

        self._branch = None
        self._version = None
        self._last_msg = None

    def _fetch_svn_info(self):
        svn_info_output = subprocess.Popen(
            ['svn', 'info', '--xml'],
            cwd=self.repo_path,
            stdout=subprocess.PIPE).communicate()[0]

        info_tree = ElementTree.XML(svn_info_output)
        self._branch = info_tree.find('entry/url').text.split('/')[-1]
        self._version = info_tree.find('entry/commit').attrib['revision']

    @property
    def branch(self):
        if not self._branch:
            self._fetch_svn_info()
        return self._branch

    @property
    def version(self):
        if not self._version:
            self._fetch_svn_info()
        return self._version

    def _fetch_svn_log(self):
        svn_log_output = subprocess.Popen(
            ['svn', 'log', '-l1', '--xml'],
            cwd=self.repo_path,
            stdout=subprocess.PIPE).communicate()[0]

        log_tree = ElementTree.XML(svn_log_output)
        self._last_msg = log_tree.find('logentry/msg').text

    @property
    def last_msg(self):
        if not self._last_msg:
            self._fetch_svn_log()
        return self._last_msg
