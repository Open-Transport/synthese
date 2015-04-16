#    Users unit tests.
#    @file users_unittest.py
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


import hashlib
import logging
import sys

if sys.version_info >= (2, 7):
    import unittest
else:
    import unittest2 as unittest
import mechanize

# Use absolute imports, otherwise the module is loaded twice and global
# variables are overwritten.
from synthesepy.functional_test import http_testcase


log = logging.getLogger(__name__)


class UsersTest(http_testcase.HTTPTestCase):
    BASIC_RESA_CUSTOMER_PROFILE_ID = '7599828666155010'

    def test_create_and_update(self):
        api = self.get_http_api()

        user = self.project.db_backend.query(
            "delete from t026_users where login = 'testLogin'")

        # Create a new user.

        ADD_USER_ACTION = 'sau'
        ACTION_PARAM_PREFIX = 'actionParam'
        res = api.call_action2(ADD_USER_ACTION, {
            ACTION_PARAM_PREFIX + 'nm': 'testName',
            ACTION_PARAM_PREFIX + 'lg': 'testLogin',
            ACTION_PARAM_PREFIX + 'pid': self.BASIC_RESA_CUSTOMER_PROFILE_ID,
        }, send_sid=True)

        user = self.project.db_backend.query(
            "select * from t026_users where login = 'testLogin'",
            one=True)
        log.debug('User: %s', user)

        self.assertEquals(user['login'], 'testLogin')
        self.assertEquals(user['name'], 'testName')
        self.assertEquals(user['password'], '')
        user_id = user['id']

        # test logging in

        res = api.call_action2('login', {
            'actionParamlogin': 'testLogin',
            'actionParampwd': ''
        })

        # Authentication failure manifests with a missing cookie.
        self.assertNotIn('sid', res.cookies)

        # update password

        UPDATE_PASSWORD_ACTION = 'upua'
        res = api.call_action2(UPDATE_PASSWORD_ACTION, {
            ACTION_PARAM_PREFIX + 'u': user_id,
            ACTION_PARAM_PREFIX + 'p1': 'my_password',
            ACTION_PARAM_PREFIX + 'p2': 'my_password',
        }, send_sid=True)

        # Now, login should succeed.

        res = api.call_action2('login', {
            'actionParamlogin': 'testLogin',
            'actionParampwd': 'my_password'
        })

        self.assertIn('sid', res.cookies)

        user = self.project.db_backend.query(
            "select * from t026_users where login = 'testLogin'",
            one=True)
        log.debug('User: %s', user)

        self.assertEquals(user['id'], user_id)
        salt = user['password'][:32]
        md5_hash = hashlib.md5(salt + 'my_password').hexdigest()
        self.assertEquals(user['password'], salt + md5_hash)

    def test_unhashed_password(self):
        api = self.get_http_api()

        self.project.db_backend.query(
            "insert into t026_users (id, name, login, password, profile_id, auth) "
            "values(?, 'oldUser', 'oldLogin', 'oldPassword', ?, 1);",
            [7318353689444399, self.BASIC_RESA_CUSTOMER_PROFILE_ID])

        res = api.call_action2('login', {
            'actionParamlogin': 'oldLogin',
            'actionParampwd': 'oldPassword'
        })

        self.assertIn('sid', res.cookies)

        user = self.project.db_backend.query(
            "delete from t026_users where login = 'oldLogin'")


def load_tests(loader, standard_tests, pattern):
    return http_testcase.do_load_tests(globals(), loader)
