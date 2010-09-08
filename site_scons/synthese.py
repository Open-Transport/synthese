## @namespace synthese SYNTHESE data extraction projects.
## @ingroup forge2Projects

from project import *
from debian import *
from tools import *

import datetime

## SYNTHESE to Trident export project.
##
## Source :
##  - 0 : synthese.sql : SYNTHESE 3 SQLite dump containing the transport network to export
##
## Targets :
##  - n : trident_line_xx_yy.xml : one XML file per exported line (xx = short name of the line, yy = SYNTHESE uid of the line)
##
class S3TridentProject(Project):

	## Constructor.
	## @param self Self
	## @param env SCons environment to clone
	## @param name Name of the project
	## @param network_id ID of the network to export
	def __init__ (self, env, name, network_id, version):
		Project.__init__ (self, env, name, DBPrjVersion(self, version))
		self.network_id = network_id
		DerivedDebianPaquetProject(self,env.SrcDir('deb'))


	def get_type (self):
		return 's3trident'


	def declare (self):
	
		self.env['NETWORK'] = self.network_id
		self.env['TISSEO'] = 0
		
		self.add_target(
			self.env.S3Trident([self.env.Dir('trident')], [])
		)
		self.env.AlwaysBuild(self.targets[0])
		
		return
	##


## SYNTHESE to "Trident 2 provisoire Tisseo" export project.
##
## Source :
##  - 0 : synthese.sql : SYNTHESE 3 SQLite dump containing the transport network to export
##
## Targets :
##  - n : trident2_provisoire_tisseo_line_xx_yy.xml : one XML file per exported line (xx = short name of the line, yy = SYNTHESE uid of the line)
##
class S3Trident2ProvisoireTisseoProject(Project):

	## Constructor.
	## @param self Self
	## @param env SCons environment to clone
	## @param name Name of the project
	## @param network_id ID of the network to export
	def __init__ (self, env, name, network_id, version):
		Project.__init__ (self, env, name, DBPrjVersion(self, version))
		self.network_id = network_id
		DerivedDebianPaquetProject(self, env.SrcDir('deb'))


	def get_type (self):
		return 's3trident2'


	def declare (self):
		

		self.env['NETWORK'] = self.network_id
		self.env['TISSEO'] = 1

		self.add_target(
                        self.env.S3Trident([self.env.Dir('trident2')], [])
                )
		self.env.AlwaysBuild(self.targets[0])

                return
	##
