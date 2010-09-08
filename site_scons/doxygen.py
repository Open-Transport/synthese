## @namespace doxygen Doxygen automated documentation projects.
## @ingroup forge2Projects

from msvs import *
from project import *
from debian import *
from tools import *

## Doxygen generated documentation project.
##
class DoxProject (Project):

    def __init__ (self, env, name, doxyfile, version='0.0', other_sources = None):
      self.doxyfile = doxyfile
      
      Project.__init__ (self, env, name, version)
      
      dxpath = os.path.dirname(doxyfile)
      DoxygenVisualStudioProject(
		self.env,
		self.name + '_doc',
		filesrootdir = dxpath
      )
      DerivedDebianPaquetProject(
        self,
        name = self.name + '_doc',
        rootdir = os.path.join(dxpath, 'deb'),
        dispatch_map = { '*' : (self.env['_DEB_DOC_INSTALL_DIR'] + os.sep + self.name) + '/' }
      )


    def get_type (self):
      return 'doc'


    def declare (self):
      self.add_target(self.env.Doxygen (self.doxyfile))
      #self.env.AlwaysBuild (self.targets)


class DoxygenVisualStudioProject(VCProjVisualStudioProject):
	def __init__(self, env, name, filesrootdir):
		VCProjVisualStudioProject.__init__(self, env, name, ['*.dox', '*.doxygen', '*.doxyfile', '*.html', '*.css', '*.html', '*.htm', '*.png', '*.jpg', 'Doxyfile', '*.pdf'], filesrootdir = filesrootdir)
	##
