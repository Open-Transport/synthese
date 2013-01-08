#    Login unit tests.
#    @file login_unittest.py
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


import sys

if sys.version_info >= (2, 7):
    import unittest
else:
    import unittest2 as unittest
import mechanize
import time

# Use absolute imports, otherwise the module is loaded twice and global
# variables are overwritten.
from synthesepy.functional_test import http_testcase


class LoginTest(http_testcase.HTTPTestCase):
    USERNAME = 'root'
    PASSWORD = 'root'

    def test_bad_password(self):
        time.sleep(10)
        br = self.get_http_api().get_browser_for_suffix("?SERVICE=packages")

        br.select_form(name='')
        br['actionParamlogin'] = self.USERNAME
        br['actionParampwd'] = 'badpassword'
        response = br.submit()

        self.assertEqual(br.title(), None)
        self.assertIn('Veuillez vous connecter pour utiliser cette fonction',
                      response.read())
        self.assertIn('Mot%20de%20passe%20erron%c3%a9', response.geturl())

    def test_bad_username(self):
        br = self.get_http_api().get_browser_for_suffix("?SERVICE=packages")

        br.select_form(name='')
        br['actionParamlogin'] = 'badusername'
        br['actionParampwd'] = self.PASSWORD
        response = br.submit()

        self.assertEqual(br.title(), None)
        self.assertIn('Veuillez vous connecter pour utiliser cette fonction',
                      response.read())
        self.assertIn('Utilisateur%20introuvable', response.geturl())

    def test_successful_login(self):
        br = self.get_http_api().get_browser_for_suffix("?SERVICE=packages")

        br.select_form(name='')
        br['actionParamlogin'] = self.USERNAME
        br['actionParampwd'] = self.PASSWORD
        response = br.submit()

        self.assertEqual(br.title(), None)
        content = br.response().read()
        self.assertIn('Etes-vous s\xc3\xbbr de vouloir installer le package admin/',
                      content)


def load_tests(loader, standard_tests, pattern):
    return http_testcase.do_load_tests(globals(), loader)
