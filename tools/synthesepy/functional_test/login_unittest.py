import sys

if sys.version_info >= (2, 7):
    import unittest
else:
    import unittest2 as unittest
import mechanize

# Use absolute imports, otherwise the module is loaded twice and global
# variables are overwritten.
from synthesepy.functional_test import http_testcase


class LoginTest(http_testcase.HTTPTestCase):
    USERNAME = 'root'
    PASSWORD = 'root'

    def test_bad_username(self):
        br = self.get_browser()

        br.select_form(name='login')
        br['actionParamlogin'] = self.USERNAME
        br['actionParampwd'] = 'badpassword'
        response = br.submit()

        self.assertEqual(br.title(), 'Login - SYNTHESE3 Admin')
        content = br.response().read()
        self.assertIn('Action error : Mot de passe erron\xc3\xa9', content)

    def test_bad_password(self):
        br = self.get_browser()

        br.select_form(name='login')
        br['actionParamlogin'] = 'badusername'
        br['actionParampwd'] = self.PASSWORD
        response = br.submit()

        self.assertEqual(br.title(), 'Login - SYNTHESE3 Admin')
        content = br.response().read()
        self.assertIn('Action error : Utilisateur introuvable', content)

    def test_successful_login(self):
        br = self.get_browser()

        br.select_form(name='login')
        br['actionParamlogin'] = self.USERNAME
        br['actionParampwd'] = self.PASSWORD
        response = br.submit()

        self.assertEqual(br.title(), 'Accueil - SYNTHESE3 Admin')
        content = br.response().read()
        self.assertIn('Bienvenue sur le module d\'administration de SYNTHESE', content)


def load_tests(loader, standard_tests, pattern):
    return http_testcase.do_load_tests(globals(), loader)
