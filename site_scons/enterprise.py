## @namespace enterprise Enterprise data generation projects.
## @ingroup forge2Projects

from project import *
from debian import *
from tools import *
from version import *

class EnterpriseS3DataProject (Project):

	def __init__ (self, env, name, base_id, version):
		Project.__init__ (
			self,
			env,
			name,
			DBPrjVersion(self,version),
			dependencies = [
				('s3-server.cppbin','3.1')
			],
			extra_dependencies = [
				('sqlite3', '3.4.2')
			]
		)
		self.base_id = base_id
		DerivedDebianPaquetProject(self,env.SrcDir('deb'))


	def get_type (self):
		return 'enterprises3'


	def declare (self):
	
		#prereq = [ DebPrereq (senv,'rcs-enterprise-data'), DebPrereq (senv,'rcs-enterprise')  ],
		#dependencies = [('s3-server.cppbin', '3.01'), 'sqlite3.cppbin'],

		self.env['BASE_ID'] = self.base_id
		self.add_target(
			self.env.EnterpriseSYNTHESE3([self.env.File('synthese.sql')], [])
		)
		self.env.AlwaysBuild(self.env.File('synthese.sql'))
		return


class EnterpriseS2DataProject (Project):

	def __init__ (self, env, name, base_id, version):
		Project.__init__ (self, env, name, DBPrjVersion(self,version))
		self.base_id = base_id
		DerivedDebianPaquetProject(self,env.SrcDir('deb'))


	def get_type (self):
		return 'enterprises2'


	def declare (self):
	
        #       prereq = [ DebPrereq (senv,'rcs-enterprise-data'), DebPrereq (senv,'rcs-enterprise')  ],
         #          dependencies = [('s2-server.cppbin', '2.07')],
		self.env['BASE_ID'] = self.base_id
		self.add_target(
			self.env.EnterpriseSYNTHESE2([self.env.Dir('environnements'), self.env.File('environnements.per')], [])
		)
		self.env.AlwaysBuild(self.env.Dir('environnements'))
		self.env.AlwaysBuild(self.env.File('environnements.per'))
		return
