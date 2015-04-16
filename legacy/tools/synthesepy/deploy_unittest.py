#    -*- coding: utf-8 -*-
#    Deploy unit tests.
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


import logging
import os
from os.path import join
import unittest

from synthesepy import project_manager
import synthesepy.test


class TestDeploy(unittest.TestCase):
    def setUp(self):
        self.tester = synthesepy.test.Tester.instance

    def _init_project(self, import_templates=None):
        self.project_path = os.path.join(
            self.tester.env.env_path, 'projects', 'test')

        backend = [b for b in self.tester.backends if
            b.conn_info.backend == 'sqlite'][0]

        project = project_manager.create_project(
            self.tester.env, self.project_path,
            conn_string=backend.conn_info.conn_string,
            overwrite=True, initialize=False)

        return project_manager.Project(self.project_path, self.tester.env)

    def _reload_project(self):
        return project_manager.Project(self.project_path, self.tester.env)

    def test_dumps(self):
        project = self._init_project()
        deployer = project.deployer

        dumps = deployer.dumps
        self.assertEquals(dumps, [])

        dump1 = deployer._create_dump()
        self.assertEquals(dump1.id, '1')

        dump2 = deployer._create_dump()
        self.assertEquals(dump2.id, '2')

        self.assertEquals([d.id for d in deployer.dumps], ['1', '2'])

        project = self._reload_project()

        self.assertEquals([d.id for d in deployer.dumps], ['1', '2'])

    def test_locked(self):
        project = self._init_project()
        deployer = project.deployer

        self.assertFalse(deployer.locked)
        deployer.locked = True
        self.assertTrue(deployer.locked)

        # Deploy should do nothing when locked.
        deployer.deploy()

        project = self._reload_project()
        self.assertTrue(deployer.locked)
        deployer.locked = False
        self.assertFalse(deployer.locked)
