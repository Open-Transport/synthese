#    Synthese Kiosk install script.
#    @file install.py
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
from os.path import join
import platform
import shutil
import sys
import subprocess

thisdir = os.path.abspath(os.path.dirname(__file__))


def main():
    env_dir = os.path.join(thisdir, 'env')

    if 'clean' in sys.argv:
        try:
            shutil.rmtree(env_dir)
        except:
            pass

        try:
            shutil.rmtree(os.path.join(thisdir, 'dist'))
        except:
            pass

        try:
            shutil.rmtree(os.path.join(thisdir, 'build'))
        except:
            pass
        return

    if not os.path.isdir(env_dir):
        try:
            subprocess.check_call("virtualenv %s" % env_dir, shell=True)
        except:
            print "---"
            print "ERROR: Could not run virtualenv. Check it is installed, if not run:"
            print "       sudo apt-get install python-virtualenv"
            print "   OR with pip:"
            print "       sudo pip install --upgrade virtualenv"
            sys.exit(1)

    def _get_env_executable(executable):
        if sys.platform == 'win32':
            return os.path.join(env_dir, 'Scripts', executable + '.exe')

        return os.path.join(env_dir, 'bin', executable)

    env_python = _get_env_executable('python')

    subprocess.check_call([
        env_python, os.path.join(thisdir, 'setup.py'), 'develop'])

    def download_and_extract(url, path):
        thirdparty_dir = join(thisdir, "third_party")
        target = join(*[thirdparty_dir] + list(path))
        if os.path.isfile(target):
            return
        target_dir = os.path.dirname(target)
        if not os.path.isdir(target_dir):
            os.makedirs(target_dir)
        subprocess.check_call("wget %s" % url, shell=True, cwd=target_dir)
        subprocess.check_call("unzip *zip", shell=True, cwd=target_dir)
        subprocess.check_call("rm *zip", shell=True, cwd=target_dir)

    # Linux chromedriver is quite large. Download it at installation time.
    if sys.platform == 'linux2':
        target_dir = join(thisdir, "third_party", "chromedriver", "third_party", "lin64")
        print "Warning, Selenium webdriver for Chrome is not packaged. If you use Chrome,"
        print "         you must download the driver from http://chromedriver.googlecode.com/files/"
        print "         and extract it under:"
        print "         " + target_dir

if __name__ == '__main__':
    main()
