## @namespace mysql MySQL data extraction projects.
## @ingroup forge2Projects

from project import *
from tools import *
from debian import *
from version import *

import os

## MySQL data dump project.
##
## Used builders :
##  - mysqldump
## 
## Automated project creations :
##  - debian::DerivedDebianPaquetProject : Debian package installing the dump
##  - msvs::VCProjVisualStudioProject : Visual Studio project file through the DerivedDebianPaquetProject
##
class MySQLDataProject(Project):

	## Constructor.
	## @param self Self
	## @param env Environment to clone
	## @param name Name of the project (will appear in Visual Studio as is)
	
    def __init__ (self, env, name, version, host='', user='', password='', tables=''):
      Project.__init__ (self, env, name, DBPrjVersion(self, version),
		extra_dependencies = [ ('mysql-server-5.0', '5.0'), ('mysql-client-5.0', '5.0') ]
	  )
      self.filesrootdir = env.SrcDir('.')
      self.rootdir = self.env.SrcDir('.')
      self.host = host
      self.user = user
      self.password = password
      self.tables = tables
      DerivedDebianPaquetProject(self,env.SrcDir('deb'))
      
    ##


    def get_type (self):
      return 'sql'
    ##


    ## Targets declaration.
    def declare (self):

      self.env['HOST'] = self.host
      self.env['USER'] = self.user
      self.env['PASSWORD'] = self.password
      self.env['TABLES'] = self.tables
      
      self.add_target(
		self.env.MySQLDump(
			self.name + '.sql', []
		)
	  )
	  
      self.env.AlwaysBuild(self.targets[0])

