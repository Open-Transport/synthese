#!/usr/bin/env python
#
#    Script to synchronize a directory with SVN.
#    @file synthese_svn.py
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
import optparse
import os
from os.path import join
import shutil
import subprocess
import sys
import tempfile
from xml.dom import minidom


log = logging.getLogger(__name__)


# Backport of check_output for Python 2.6
# (http://stackoverflow.com/questions/2924310/whats-a-good-equivalent-to-pythons-subprocess-check-call-that-returns-the-cont)
def check_output(*popenargs, **kwargs):
    if 'stdout' in kwargs:
        raise ValueError('stdout argument not allowed, it will be overridden.')
    process = subprocess.Popen(stdout=subprocess.PIPE, *popenargs, **kwargs)
    output, unused_err = process.communicate()
    retcode = process.poll()
    if retcode:
        cmd = kwargs.get("args")
        if cmd is None:
            cmd = popenargs[0]
        raise subprocess.CalledProcessError(retcode, cmd, output=output)
    return output

class CalledProcessError(Exception):
    def __init__(self, returncode, cmd, output=None):
        self.returncode = returncode
        self.cmd = cmd
        self.output = output
    def __str__(self):
        return "Command '%s' returned non-zero exit status %d" % (
            self.cmd, self.returncode)
# overwrite CalledProcessError due to `output` keyword might be not available
subprocess.CalledProcessError = CalledProcessError



def maybe_remove(path):
    '''Remove given file or directory if it exists'''
    if os.path.isdir(path):
        shutil.rmtree(path)
    try:
        os.unlink(path)
    except OSError:
        pass


def maybe_makedirs(path):
    if os.path.isdir(path):
        return
    os.makedirs(path)


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


def copy_files(source_path, dest_path):
    def get_files(base_path):
        relative_paths = set()
        for path, dirlist, filelist in os.walk(base_path):
            for exclude in ['.git', '.hg', '.svn']:
                if exclude in dirlist:
                    dirlist.remove(exclude)
            relative_path = os.sep.join(path.split(os.sep)[source_path_parts_count:])
            for name in filelist:
                relative_paths.add(join(relative_path, name))
        return relative_paths

    source_path_parts_count = len(source_path.split(os.path.sep))
    source_files = set()

    source_files = get_files(source_path)
    for relative_path in source_files:
        source = join(source_path, relative_path)
        target = join(dest_path, relative_path)

        log.debug('Copying %r ==> %r', source, target)

        maybe_remove(target)
        maybe_makedirs(os.path.dirname(target))
        shutil.copy2(source, target)

    dest_files = get_files(dest_path)
    return dest_files - source_files


def svn(options, command, *args):
    cmd = ['svn']

    cmd.append('--no-auth-cache')
    if options.username:
        cmd.extend(['--username', options.username])
    if options.password:
        cmd.extend(['--password', options.password])

    cmd.append(command)
    cmd.extend(args)
    log.debug('Running: %s', ' '.join(cmd))
    return check_output(cmd)


def main(options):
    log.debug('In main. Options=%s', options)

    if not options.url:
        raise Exception('Missing --url parameter')

    if not options.checkout_path:
        options.checkout_path = options.export_path + '_checkout'

    svn_checkout_path = to_cygwin_path(options.checkout_path)

    if not os.path.isdir(options.export_path):
        raise Exception('Export path does not exist: %r' % options.export_path)

    if not os.path.isdir(options.checkout_path):
        svn(options, 'checkout', options.url, svn_checkout_path)

    log.debug('Copying files to checkout')
    dest_only_files = copy_files(options.export_path, options.checkout_path)

    log.debug('Updating checkout')
    svn(options, 'update', svn_checkout_path)

    log.debug('Removing obsolete files from checkout (%r)', dest_only_files)
    for f in dest_only_files:
        os.unlink(join(options.checkout_path, f))

    log.debug('Cleaning export path and copying files back from checkout')
    shutil.rmtree(options.export_path)
    os.makedirs(options.export_path)
    copy_files(options.checkout_path, options.export_path)

    if not options.commit:
        return

    log.debug('Committing changes')

    xml_status = svn(options, 'status', '--xml', svn_checkout_path)
    status_dom = minidom.parseString(xml_status)

    for entry in status_dom.getElementsByTagName('entry'):
        status = entry.getElementsByTagName("wc-status")[0]
        path = entry.getAttribute("path")
        if status.getAttribute("item") == 'missing':
            svn(options, 'delete', path)
        elif status.getAttribute("item") == 'unversioned':
            svn(options, 'add', path)

    if not options.commit_message:
        options.commit_message = '(no message specified)'
    svn(options, 'commit', '-m', options.commit_message, svn_checkout_path)


if __name__ == '__main__':
    usage = 'usage: %prog [options]'
    parser = optparse.OptionParser(usage=usage)

    parser.add_option('-v', '--verbose', action='store_true',
         default=False, help='Print debug logging')
    parser.add_option('--export-path')
    parser.add_option('--checkout-path')
    parser.add_option('--url')
    parser.add_option('--username')
    parser.add_option('--password')
    parser.add_option('--commit', action='store_true', default=False)
    parser.add_option('--commit-message')

    (options, args) = parser.parse_args()
    if len(args) != 0:
        parser.print_help()
        sys.exit(1)

    logging.basicConfig(level=(logging.DEBUG if options.verbose else
                               logging.INFO))

    main(options)
