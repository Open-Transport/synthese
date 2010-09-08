## @namespace msvs Microsoft Visual Studio files generation projects.
## @ingroup forge2Projects

from project import *
from tools import *

import os

## Basic VCProj Project.
## Designed to be inherited
class VCProjVisualStudioProject(Project):

	## Constructor.
	## @param self Self
	## @param env Environment to clone
	## @param name Name of the project (will appear in Visual Studio as is)
	## @param pattern Pattern of files to include in the tree (eg: ['*.html', '*.php']
	## @param others List of files to add to the tree, relatively to the files root dir (eg: ['../test.txt'])
	## @param filesrootdir Absolute path of the root of the tree
	## @param sorts List of subfolders to create, with corresponding file name pattern to use when populating
	##	(eg: ['Actions', ['*Action.h', '*Action.cpp']])
    def __init__ (self, env, name,
                  patterns = list(),
                  others = list(),
                  filesrootdir = '',
                  sorts = list()
                  ):
      Project.__init__ (self, env, name)
      if filesrootdir == '':
        self.filesrootdir = env.SrcDir('.')
      else:
        self.filesrootdir = filesrootdir
      self.patterns = patterns
      self.others = others
      self.rootdir = self.env.SrcDir('.')
      self.sorts = sorts
    ##


    def get_type (self):
      return 'vcproj'
    ##


    ## Targets declaration.
    ## Environment variables :
    ##  - _FILES_ROOT_DIR : Root of the tree of the project
    def declare (self):
      projname = self.name + '.vcproj'
      
      files = self.env.Glob(self.patterns, [], self.filesrootdir, False, True, False, True)
      sfiles = list()
      for file in files:
        sfiles.append(relpath(os.path.abspath(self.filesrootdir + os.sep + file),os.path.abspath(self.rootdir)))
      sfiles.extend(self.others)

      self.env['_FILES_ROOT_DIR'] = self.filesrootdir
      self.env['_SORTS'] = self.sorts
      
      self.add_target(
		self.env.MsVsProject(
			self.env.SrcDir(self.rootdir) + os.sep + projname,
			self.env.Value(sfiles)
		)
	  )

class PHPProjVisualStudioProject(Project):
    def __init__ (self, env, name, rootdir = ''):
      Project.__init__ (self, env, name)
      if rootdir == '':
        self.rootdir = env.SrcDir('.')
      else:
        self.rootdir = rootdir
    ##


    def get_type (self):
      return 'phpproj'
    ##


    def declare (self):
      patterns = ['*.php', '*.html', '*.css', '*.js']
      
      files = self.env.Glob(patterns, [], self.rootdir, True, True, True, True)
      pfiles = list()
      for file in files:
           pfiles.append(relpath(file, self.rootdir))
      
      self.add_target(self.env.MsVsPHPProject(os.path.join(self.rootdir, self.name + '.phpproj'),
                                         self.env.Value (pfiles))
      )
    ##
