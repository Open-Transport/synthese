#    Class for implementing HTTP tests.
#    @file http_testcase.py
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
import os
import sys

if sys.version_info >= (2, 7):
    import unittest
else:
    import unittest2 as unittest

import synthesepy.functional_test
import synthesepy.test
from synthesepy import daemon
from synthesepy import db_backends
from synthesepy import http_api
from synthesepy import project_manager
from synthesepy import utils


log = logging.getLogger(__name__)


class HTTPTestCase(unittest.TestCase):
    """
    Base testcase for interacting with the Synthese HTTP server.

    This base class will automatically set up the database before running all
    of the test methods for each backends, unless no_init was set to true
    when calling init_backends().
    """
    system_packages = ('routePlanner', 'testData')

    def __init__(self, *args, **kwargs):
        super(HTTPTestCase, self).__init__(*args, **kwargs)

    # TODO: add setUp / tearDown to reset http_api browser state?

    @classmethod
    def init_project(cls, project):
        """
        Override to do test specific project initialization work.
        Called before the daemon is ran.
        """

    @classmethod
    def setUpClass(cls):
        log.info('setupClass %s, with backend %s', cls.__name__, cls.backend)

        project_path = os.path.join(cls.env.env_path, 'projects', 'test')

        if cls.no_init:
            cls.project = project_manager.Project(project_path, cls.env)
            return

        daemon.Daemon.kill_existing(cls.env)

        log.info('Creating project')
        cls.project = project_manager.create_project(
            cls.env, project_path, system_packages=cls.system_packages,
            conn_string=cls.backend.conn_info.conn_string,
            overwrite=True)

        cls.init_project(cls.project)
        log.info('Running daemon for tests')
        cls.project.rundaemon(block=cls.daemon_only)

    @classmethod
    def tearDownClass(cls):
        log.debug('tearDownClass %s, with backend %s', cls.__name__, cls.backend)
        if cls.no_init:
            return
        cls.project.stopdaemon()
        cls.project.clean()

    def get_http_api(self):
        return http_api.HTTPApi(self.backend.env)


def do_load_tests(scope, loader):
    # Based on the code from unittest/loader.py, TestLoader.loadTestsFromModule

    module = sys.modules[scope['__name__']]

    tests = []
    tester = synthesepy.test.Tester.instance
    for name in dir(module):
        obj = getattr(module, name)
        if isinstance(obj, type) and issubclass(obj, HTTPTestCase):
            log.debug('Found FunctionalTestCase: %s', obj)

            for backend in tester.backends:
                class_name = obj.__name__ + '_' + backend.name
                new_class = type(class_name, (obj,), {})
                new_class.env = tester.env
                new_class.backend = backend
                new_class.no_init = tester.config.no_init
                new_class.daemon_only = tester.config.test_daemon_only

                tests.append(loader.loadTestsFromTestCase(new_class))

            continue
        if isinstance(obj, type) and issubclass(obj, unittest.TestCase):
            tests.append(loader.loadTestsFromTestCase(obj))

    return loader.suiteClass(tests)
