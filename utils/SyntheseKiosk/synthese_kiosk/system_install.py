#    System integration of Synthese Kiosk.
#    @file system_config.py
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


import os
import logging
import sys


log = logging.getLogger(__name__)


def install_kde(config_dir, uninstall):
    script_path = sys.argv[0]
    if 'synthese-kiosk' not in script_path:
        raise Exception('Please run script script using the synthese-kiosk executable')

    script_path = os.path.abspath(script_path)
    script_path = os.path.normpath(script_path)

    desktop_file_content= """[Desktop Entry]
Encoding=UTF-8
Type=Application
Name=Synthese Kiosk
Terminal=true
Exec={0} -v -c {1}
""".format(script_path, config_dir)

    target = os.path.expanduser('~/.kde4/Autostart/SyntheseKiosk.desktop')
    if uninstall:
        if os.path.isfile(target):
            os.unlink(target)
    else:
        with open(target, 'w') as f:
            f.write(desktop_file_content)
        log.info('Wrote desktop autostart file to %s', target)


def install_windows(config_dir, uninstall):
    import _winreg

    AUTORUN_KEY = r'Software\Microsoft\Windows\CurrentVersion\Run'
    if uninstall:
        h_key = _winreg.OpenKey(_winreg.HKEY_CURRENT_USER, AUTORUN_KEY, 0, _winreg.KEY_ALL_ACCESS)
        _winreg.DeleteValue(h_key, r'synthese-kiosk')
        _winreg.CloseKey(h_key)
    else:
        h_key = _winreg.OpenKey(_winreg.HKEY_CURRENT_USER, AUTORUN_KEY, 0, _winreg.KEY_ALL_ACCESS)
        command = '"{0}" "{1}" -v -c "{2}"'.format(
            sys.executable, sys.argv[0], config_dir)
        _winreg.SetValueEx(h_key, r'synthese-kiosk', 0, _winreg.REG_SZ, command)
        _winreg.CloseKey(h_key)


def install(config_dir, uninstall=False):
    # naive way of detecting kde.
    if os.path.isdir(os.path.expanduser('~/.kde4/Autostart')):
        log.info('Detected KDE system')
        install_kde(config_dir, uninstall)
    elif sys.platform == 'win32':
        install_windows(config_dir, uninstall)
    else:
        raise Exception('Platform not yet supported')
