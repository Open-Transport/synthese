import logging
import os
import sys
import tempfile

if sys.version_info >= (2, 7):
    import unittest
else:
    import unittest2 as unittest
import mechanize

from synthesepy import daemon
from synthesepy import db_backends
import synthesepy.functional_test

log = logging.getLogger(__name__)


class HTTPTestCase(unittest.TestCase):
    """
    Base testcase for interacting with the Synthese HTTP server.

    This base class will automatically set up the database before running all
    of the test methods for each backends, unless no_init was set to true
    when calling init_backends().
    """
    def __init__(self, *args, **kwargs):
        super(HTTPTestCase, self).__init__(*args, **kwargs)

    @classmethod
    def setUpClass(cls):
        log.debug('setupClass %s, with backend %s', cls.__name__, cls.backend)

        if cls.no_init:
            return
        cls.backend.init_db()
        cls.backend.start_daemon()

    @classmethod
    def tearDownClass(cls):
        log.debug('tearDownClass %s, with backend %s', cls.__name__, cls.backend)
        if cls.no_init:
            return
        cls.backend.stop_daemon()
        cls.backend.drop_db()

    def get_browser(self):
        br = mechanize.Browser()
        admin_url = 'http://localhost:%s/admin/synthese3' % \
            self.backend.env.wsgi_proxy_port
        br.open(admin_url)
        self.assertEqual(br.title(), 'Login - SYNTHESE3 Admin')
        return br


backends = []
no_init = False


def init_backends(env, conn_strings, _no_init):
    global backends, no_init

    log.debug('init_backends')
    backends = []
    for conn_string in conn_strings:
        backends.append(db_backends.create_backend(env, conn_string))

    log.debug('Initialized backends %s', backends)

    no_init = _no_init


def do_load_tests(scope, loader):
    # Based on the code from unittest/loader.py, TestLoader.loadTestsFromModule"""

    module = sys.modules[scope['__name__']]

    tests = []
    for name in dir(module):
        obj = getattr(module, name)
        if isinstance(obj, type) and issubclass(obj, HTTPTestCase):
            log.debug('Found FunctionalTestCase: %s', obj)

            for backend in backends:
                class_name = obj.__name__ + "_" + backend.name
                new_class = type(class_name, (obj,), {})
                new_class.backend = backend
                new_class.no_init = no_init

                tests.append(loader.loadTestsFromTestCase(new_class))

            continue
        if isinstance(obj, type) and issubclass(obj, unittest.TestCase):
            tests.append(loader.loadTestsFromTestCase(obj))

    return loader.suiteClass(tests)
