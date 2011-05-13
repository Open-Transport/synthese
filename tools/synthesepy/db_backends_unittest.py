import unittest

from synthesepy import db_backends


class DummyEnv(object):
    admin_root_path = ''


class TestDBBackend(unittest.TestCase):
    def test_create_backend(self):
        backend = db_backends.create_backend(DummyEnv(), 'dummy://')
        self.assertEqual(backend.name, 'dummy')
