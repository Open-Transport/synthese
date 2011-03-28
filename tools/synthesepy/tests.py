import logging
import os
import sys
import tempfile
import unittest

from synthesepy import db_backends
from synthesepy import daemon

if sys.version_info >= (2, 7):
    import unittest
else:
    import unittest2 as unittest
import mechanize

log = logging.getLogger(__name__)

# test cases:

class LoginTest(object):
    USERNAME = 'root'
    PASSWORD = 'root'

    @classmethod
    def setUpClass(cls):
        log.debug('setUpClass')
        cls.backend.init_db()
        cls.backend.start_daemon()

    @classmethod
    def tearDownClass(cls):
        log.debug('tearDownClass')
        cls.backend.stop_daemon()
        cls.backend.drop_db()
        pass

    def _get_browser(self):
        br = mechanize.Browser()
        admin_url = 'http://localhost:%s/admin/synthese3' % \
            self.backend.env.wsgi_proxy_port
        br.open(admin_url)
        self.assertEqual(br.title(), 'Login - SYNTHESE3 Admin')
        return br

    def test_bad_username(self):
        br = self._get_browser()

        br.select_form(name='login')
        br['actionParamlogin'] = self.USERNAME
        br['actionParampwd'] = 'badpassword'
        response = br.submit()

        self.assertEqual(br.title(), 'Login - SYNTHESE3 Admin')
        content = br.response().read()
        self.assertIn('Action error : Mot de passe erron\xc3\xa9', content)

    def test_bad_password(self):
        br = self._get_browser()

        br.select_form(name='login')
        br['actionParamlogin'] = 'badusername'
        br['actionParampwd'] = self.PASSWORD
        response = br.submit()

        self.assertEqual(br.title(), 'Login - SYNTHESE3 Admin')
        content = br.response().read()
        self.assertIn('Action error : Utilisateur introuvable', content)

    def test_successful_login(self):
        br = self._get_browser()

        br.select_form(name='login')
        br['actionParamlogin'] = self.USERNAME
        br['actionParampwd'] = self.PASSWORD
        response = br.submit()

        self.assertEqual(br.title(), 'Accueil - SYNTHESE3 Admin')
        content = br.response().read()
        self.assertIn('Bienvenue sur le module d\'administration de SYNTHESE 3.2.0.', content)

def init_test_classes(backends, cls):
    for backend in backends:
        def make_setUpClass(backend):
            def setUpClass():
                cls.backend = backend
                cls.setUpClass()
            return staticmethod(setUpClass)
        class_name = cls.__name__ + '_' + backend.name
        globals()[class_name] = type(
            class_name,
            (unittest.TestCase, cls),
            {
                'setUpClass': make_setUpClass(backend),
                'tearDownClass': cls.tearDownClass,
            }
        )

def runtests(env, conn_strings):
    log.debug('runtests')
    backends = []
    for conn_string in conn_strings:
        backends.append(db_backends.create_backend(env, conn_string))

    if 'sqlite' not in [b.name for b in backends]:
        temp_sqlite_file = os.path.join(tempfile.gettempdir(), 'synthese.db3')
        backends.append(db_backends.create_backend(env, 'sqlite://path=' + temp_sqlite_file))

    init_test_classes(backends, LoginTest)

    # clear arguments to prevent interpretation
    sys.argv = sys.argv[:1]
    # TODO: parameter to pass unittests arguments?
    # XXX
    sys.argv.append('-v')
    unittest.main(__name__)
