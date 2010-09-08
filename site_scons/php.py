## @namespace php PHP Projects.
## @ingroup forge2Projects

from debian import *
from msvs import *

## PHP Project class.
class PhpProject (Project):

	## Constructor.
	## @param self Self
	## @param env Environment to clone
	## @param name Name of the project
	## @param version Version of the project
	## @param prereq Prerequisites
	## @param dependencies Internal dependencies
	## @param extra_dependencies External dependencies
	## @param rootdir Absolute path of root of PHP files  (default = .)
	## @param deb_rootdir Absolute path of root of Debian package files (default = ./deb)
    def __init__ (self, env, name, version, 
                  prereq = None,
                  dependencies = None, 
                  extra_dependencies = None,
                  rootdir = '',
                  deb_rootdir = ''
                  ):
      Project.__init__ (self, env, name, version, prereq, dependencies, extra_dependencies)
      
      if rootdir == '':
         rootdir = env.SrcDir('.')
      if deb_rootdir == '':
         deb_rootdir = env.SrcDir('deb')
      
      PHPProjVisualStudioProject(env, self.name, rootdir)
      DerivedDebianPaquetProject(self, sources = rootdir, rootdir = deb_rootdir)
    
    def get_type (self):
      return 'php'
