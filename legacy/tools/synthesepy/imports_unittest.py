#    -*- coding: utf-8 -*-
#    Imports management unit tests.
#    @file imports_unittest.py
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

import copy
import logging
import os
from os.path import join
import pprint
import unittest

from synthesepy import db_backends
from synthesepy import project_manager
from synthesepy import utils
import synthesepy.test


log = logging.getLogger(__name__)


def put_file(path, content):
    utils.maybe_makedirs(os.path.dirname(path))
    with open(path, 'wb') as f:
        f.write(content)


class MockHTTPApi(object):
    def __init__(self, *args, **kwargs):
        pass

    def call_service2(self, service, params, **kwargs):
        class MockResponse(object):
            pass
        class MockRequest(object):
            url = 'http://mock-host/synthese'
            data = params

        r = MockResponse()
        r.request = MockRequest()
        r.status_code = 200
        r.content = """
INFO : some info<br />
WARN : a warning<br />
NOTI : a notification<br />
ERR  : an error 1<br />
ERR  : an error 2<br />
XXXX : an invalid level<br />
CREAcrea message<br />
""".replace('\n', '')
        return r


class TestImports(unittest.TestCase):
    IMPORT_TEMPLATES = [{
        'id': 'test_template',
        'label': 'Test Template',
        'defaults': [
            # source id
            ('ds', '16607027920896002'),
            # network id:
            ('net', '6192453782601735'),
            ('line_filter_mode', 'mode1'),
            {
                'label': 'Fichier',
                'id': 'pa',
                'type': 'file',
            }
        ],
    }]

    def assertIn(self, a, b, msg=None):
        """Backport for Python 2.6"""
        if hasattr(unittest.TestCase, 'assertIn'):
            # use native one if available.
            return unittest.TestCase.assertIn(self, a, b, msg)
        if msg is None:
            msg = '"%s" not found in "%s"' % (a, b)
        self.assert_(a in b, msg)

    def _mock_send_mail(self, config, recipients, subject, body):
        self.emails.append((sorted(recipients), subject, body))

    def setUp(self):
        self._orig_HTTPApi = synthesepy.http_api.HTTPApi
        synthesepy.http_api.HTTPApi = MockHTTPApi
        self._orig_send_mail = utils.send_mail
        utils.send_mail = self._mock_send_mail
        self.emails = []

        self.tester = synthesepy.test.Tester.instance

    def tearDown(self):
        synthesepy.http_api.HTTPApi = self._orig_HTTPApi
        utils.send_mail = self._orig_send_mail

    def _init_project(self, import_templates):
        self.project_path = os.path.join(
            self.tester.env.env_path, 'projects', 'test')

        backend = [b for b in self.tester.backends if
            b.conn_info.backend == 'sqlite'][0]

        project = project_manager.create_project(
            self.tester.env, self.project_path,
            conn_string=backend.conn_info.conn_string,
            overwrite=True, initialize=False)

        # Prepare the project
        put_file(
            join(self.project_path, 'manager', 'imports_config.py'),
            'import_templates = {0!r}'.format(import_templates))

        return project_manager.Project(self.project_path, self.tester.env)

    def _reload_project(self):
        return project_manager.Project(self.project_path, self.tester.env)

    def test_execute(self):
        import_templates = copy.deepcopy(self.IMPORT_TEMPLATES)
        import_templates[0]['admins'] = (('admin0', 'admin0@example.com'),)
        import_templates[0]['run_results_notifications'] = {
            'noti': ('user_noti@example.com',),
        }
        project = self._init_project(import_templates)

        import_templates = project.imports_manager.get_import_templates()
        self.assertEqual(len(import_templates), 1)
        test_template = import_templates[0]
        self.assertEqual(test_template.id, 'test_template')
        self.assertEqual(test_template.label, 'Test Template')
        self.assertEqual(test_template.get_imports(), [])

        import_ = test_template.create_import()
        self.assertEqual(import_.params['state'].value, 'open')

        import_.params['net'].value = '42'
        import_.save_params()
        self.assertEqual(import_.params['net'].value, '42')
        import_.params['description'].value = 'Some description'
        import_.save_params()
        self.assertEqual(import_.params['description'].value, 'Some description')

        self.assertEqual(len(import_.events), 2)
        self.assertEqual(import_.events[0].type, 'update')
        self.assertEqual(import_.events[1].type, 'update')
        import_.add_comment('the_user', 'Some comment')
        self.assertEqual(len(import_.events), 3)
        self.assertEqual(import_.events[2].type, 'comment')
        self.assertEqual(import_.events[2].content, 'Some comment')

        # Re-read project
        project = self._reload_project()
        import_templates = project.imports_manager.get_import_templates()
        test_template = import_templates[0]
        import_ = test_template.get_import('1')
        self.assertEqual(import_.params['net'].value, '42')

        # import execution
        self.emails = []
        import_.execute()
        # This should create a new run:
        self.assertEqual(len(import_.runs), 1)
        run = import_.runs[0]
        self.assertEqual(run.messages, {
            'crea': ['crea message'],
            'err': ['an error 1', 'an error 2'],
            'info': ['some info'],
            'load': [],
            'noti': ['a notification'],
            'unknown': ['XXXX : an invalid level'],
            'warn': ['a warning']})

        self.assertEqual(len(self.emails), 2)

        email = self.emails[0]
        self.assertEqual(email[0], ['user_noti@example.com'])
        self.assertIn(u'résultat de l\'exécution', email[1])
        self.assertIn('a notification', email[2])

        email = self.emails[1]
        self.assertEqual(email[0], ['admin0@example.com'])
        self.assertIn(u'résultat de l\'exécution', email[1])
        self.assertIn('an error', email[2])

    def test_permissions(self):
        import_templates = copy.deepcopy(self.IMPORT_TEMPLATES)
        import_templates[0]['admins'] = (
            ('admin0', 'admin0@example.com'), ('admin1', 'admin1@example.com'))
        import_templates[0]['uploaders'] = (
            ('uploader0', 'uploader0@example.com'), ('uploader1', 'uploader1@example.com'))

        project = self._init_project(import_templates)

        test_template = project.imports_manager.get_import_template('test_template')

        self.assertTrue(test_template.has_access('root'))
        self.assertFalse(test_template.has_access('unauthorized'))
        self.assertFalse(test_template.has_access('unauthorized', True))

        self.assertTrue(test_template.has_access('admin0'))
        self.assertTrue(test_template.has_access('admin0', True))
        self.assertTrue(test_template.has_access('admin1'))
        self.assertTrue(test_template.has_access('admin1', True))

        self.assertTrue(test_template.has_access('uploader0'))
        self.assertFalse(test_template.has_access('uploader0', True))
        self.assertTrue(test_template.has_access('uploader1'))
        self.assertFalse(test_template.has_access('uploader1', True))

    def test_comments(self):
        import_templates = copy.deepcopy(self.IMPORT_TEMPLATES)
        import_templates[0]['admins'] = (
            ('admin0', 'admin0@example.com'), ('admin1', 'admin1@example.com'))
        import_templates[0]['uploaders'] = (
            ('uploader0', 'uploader0@example.com'), ('uploader1', 'uploader1@example.com'))

        project = self._init_project(import_templates)
        test_template = project.imports_manager.get_import_template('test_template')
        import_ = test_template.create_import()

        self.assertEqual(import_.params['state'].value, 'open')

        import_.add_comment('admin0', u'Touché!')

        self.assertEquals(len(self.emails), 1)
        email = self.emails[0]
        self.assertEquals(email[0],
            ['admin0@example.com', 'admin1@example.com',
             'uploader0@example.com', 'uploader1@example.com'])
        self.assertIn('nouveau commentaire', email[1])
        self.assertIn(u'Un commentaire a été ajouté', email[2])

        events = import_.get_events()
        self.assertEquals(len(events), 1)
        self.assertEquals(events[0].type, 'comment')
        self.assertEquals(events[0].content, u'Touché!')

        project = self._reload_project()
        import_ = project.imports_manager.get_import('test_template', '1')

        events = import_.get_events()
        self.assertEquals(len(events), 1)
        self.assertEquals(events[0].type, 'comment')
        self.assertEquals(events[0].content, u'Touché!')
