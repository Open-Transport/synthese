#    Synthese Kiosk entrypoint.
#    @file main.py
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


import argparse
import logging
import os
import sys
import time
import traceback

from synthese_kiosk import SyntheseKiosk
import system_install
import utils

log = logging.getLogger(__name__)

def main():
    parser = argparse.ArgumentParser(description='Synthese Kiosk.')
    parser.add_argument(
        '-c', '--config-dir', default=os.path.expanduser('~/.synthese_kiosk'),
        help='Path to configuration directory.')
    parser.add_argument(
        '-v', '--verbose', action='store_true',
        help='Show logs on standard output')
    parser.add_argument(
        '--system-install', action='store_true',
        help='Install Synthese Kiosk on the system')
    parser.add_argument(
        '--system-uninstall', action='store_true',
        help='Uninstall Synthese Kiosk on the system')

    args = parser.parse_args()
    kiosk = SyntheseKiosk(args.config_dir, args.verbose)

    if args.system_install:
        system_install.install(args.config_dir)
        return

    if args.system_uninstall:
        system_install.install(args.config_dir, True)
        return


    try:
        kiosk.start()
    except Exception, e:
        traceback.print_exc()
    finally:
        try:
            kiosk.stop()
        except Exception, e:
            log.warn('Ignoring exception during kiosk.stop: %s', e)
