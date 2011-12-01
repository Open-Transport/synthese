#    -*- coding: utf-8 -*-
#    Imports management unit tests.
#    @file imports.py
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
from os.path import join
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

    def call_service2(*args, **kwargs):
        class MockResponse(object):
            pass
        class MockRequest(object):
            url = 'http://mock-host/synthese'
            data = 'mock request data'

        r = MockResponse()
        r.request = MockRequest()
        r.status_code = 200
        r.content = """
INFO : some info<br />
WARN : a warning<br />
NOTI : an notification<br />
ERR  : an error 1<br />
ERR  : an error 2<br />
XXXX : an invalid level<br />
CREAcrea message<br />
""".replace('\n', '')
        return r


class TestImports(unittest.TestCase):
    def setUp(self):
        self._orig_HTTPApi = synthesepy.http_api.HTTPApi
        synthesepy.http_api.HTTPApi = MockHTTPApi

    def tearDown(self):
        synthesepy.http_api.HTTPApi = self._orig_HTTPApi

    def test_import(self):
        tester = synthesepy.test.Tester.instance

        project_path = os.path.join(tester.env.env_path, 'projects', 'test')

        import_templates_0 = [{
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

        backend = [b for b in tester.backends if b.conn_info.backend == 'sqlite'][0]

        project = project_manager.create_project(
            tester.env, project_path,
            conn_string=backend.conn_info.conn_string,
            overwrite=True, initialize=False)

        # Prepare the project
        put_file(
            join(project_path, 'manager', 'imports_config.py'),
            'import_templates = {0!r}'.format(import_templates_0))

        project = project_manager.Project(project_path, tester.env)

        import_templates = project.imports_manager.get_import_templates()
        self.assertEqual(len(import_templates), 1)
        test_template = import_templates[0]
        self.assertEqual(test_template.id, 'test_template')
        self.assertEqual(test_template.label, 'Test Template')
        self.assertEqual(test_template.get_imports(), [])

        import_ = test_template.create_import()
        self.assertEqual(import_.params['state'].value, 'open')

        import_.update_param('net', '42')
        self.assertEqual(import_.params['net'].value, '42')
        import_.update_param('description', 'Some description')
        self.assertEqual(import_.params['description'].value, 'Some description')

        self.assertEqual(import_.events, [])
        import_.add_comment('the_user', 'Some comment')
        self.assertEqual(len(import_.events), 1)
        self.assertEqual(import_.events[0].type, 'comment')
        self.assertEqual(import_.events[0].content, 'Some comment')

        # Re-read project
        project = project_manager.Project(project_path, tester.env)
        import_templates = project.imports_manager.get_import_templates()
        test_template = import_templates[0]
        import_ = test_template.get_import('1')
        self.assertEqual(import_.params['net'].value, '42')

        # import execution
        import_.execute()
        # This should create a new run:
        self.assertEqual(len(import_.get_runs()), 1)
        run = import_.get_runs()[0]
        self.assertEqual(run.messages, {
            'crea': ['crea message'],
            'err': ['an error 1', 'an error 2'],
            'info': ['some info'],
            'load': [],
            'noti': ['an notification'],
            'unknown': ['an invalid level'],
            'warn': ['a warning']})
