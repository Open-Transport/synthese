## @namespace cpp C++ projects.
## @ingroup forge2Projects

from project import *
from msvs import *
from debian import *
import string

class CPPVisualStudioProject(VCProjVisualStudioProject):
	def __init__(self, env, name, filesrootdir='', sorts=list()):
		VCProjVisualStudioProject.__init__(self, env, name, ['*.cpp', '*.h', '*.hpp', '*.c'], [], filesrootdir, sorts)
	##
	
	



## C++ generic project.
## Designed to be inherited.
## Attributes :
##  - include_dir Path of the directory to include to use the library from an other project (will be automatically added in the include path at the build of projects that depend on self)
##  - source_files List of sources files to compile
class CppProject (Project):

	## Constructor.
    ## @param source_files List of sources files to compile
	## @param include_dir Path of the directory to include to use the library from an other project (will be automatically added in the include path at the build of projects that depend on self)
    def __init__ (self, env, name, version, 
                  source_files, 
                  include_dir = None, 
                  prereq = None,
                  dependencies = None, 
                  extra_dependencies = None,
                  ):
        Project.__init__ (self, env, name, version, prereq, dependencies, extra_dependencies)
        self.source_files = source_files
        if include_dir == None:
            self.include_dir = self.env.SrcDir('.')
        else:
            self.include_dir = include_dir
        self.vsproject = None
        self.builddir = self.env.Dir('.')

	## Target declarations
	## No target is properly declared by the general CppProject class.
	## This method handles the CPPPATH environment variable by reading all the include_dir of the projects it depends
    def declare (self):

      include_dirs = list()
      if self.include_dir != None:
        include_dirs.append(self.include_dir)

      for prj in self.get_dependencies(): 
        if isinstance (prj[0], CppProject) and prj[0].include_dir != None and prj[0].include_dir not in include_dirs:
          include_dirs.append(prj[0].include_dir)

      self.env.AddIncludeDir(include_dirs)
      
      if self.vsproject != None:
        self.vsproject.env['_INCLUDE_LIST'] = string.join(include_dirs,';')
         
    def get_libs(self):
      return []

    def get_libdir(self):
      return None

class CppIncludeProject(CppProject):
	## Constructor.
	## @param self self
	## @param env SCons environment
	## @param name Name of the project
	## @param version Version of the project
	## @param include_dir Path of the directory to include to use the library from an other project (will be automatically added in the include path at the build of projects that depend on self)
	## @param filesrootdir Root of sources files in the visual studio project : only the root and its subdirectories are visible on the solution tree. Default value : SConscript path
	def __init__ (self, env, name, version,
					include_dir = None, 
                  dependencies = None, 
                  extra_dependencies = None,
                  filesrootdir = '',
                  libdir = '',
                  libs = [],
                  sorts = list()
                  ):
		CppProject.__init__ (self, env, name, version, [], include_dir, None, dependencies, extra_dependencies)
		if filesrootdir == '':
			fd = include_dir
		else:
			fd = filesrootdir
		self.vsproject = CPPVisualStudioProject(self.env, self.name + '_cppinc', fd, sorts)
		if libdir != '':
			self.builddir = libdir
		self.libs = list()
		self.libs.extend(libs)

	def get_type(self):
		return 'cppinc'
		
	def get_libs(self):
		return self.libs

	def get_libdir(self):
		return self.builddir

## C++ library project.
class CppLibProject(CppProject):

	## Constructor.
	## @param self self
	## @param env SCons environment
	## @param name Name of the project
	## @param version Version of the project
	## @param source_files List of sources files to compile
	## @param include_dir Path of the directory to include to use the library from an other project (will be automatically added in the include path at the build of projects that depend on self)
	## @param mode Shared or static library. Values are :
	##   - 'auto' (default) : static on windows, shared on linux
	##   - 'static' : static library
	##   - 'shared' : shared library
	## @param prereq List of prerequisites (Prereq objects)
	## @param dependencies List of internal dependencies (other projects of the forge : put full artefact name e.g. s3-admin.stlib). Default value : none
	## @param extra_dependencies List of external dependencies, to be used at the debian packet build. Default value : none
	## @param filesrootdir Root of sources files in the visual studio project : only the root and its subdirectories are visible on the solution tree. Default value : SConscript path
	def __init__ (self, env, name, version,
                  source_files, 
                  include_dir = None, 
                  mode = 'auto',
                  prereq = None,
                  dependencies = None, 
                  extra_dependencies = None,
                  filesrootdir = '',
                  deb_rootdir = '',
                  sorts = list()
                  ):
		CppProject.__init__ (self, env, name, version, source_files, include_dir, prereq, dependencies, extra_dependencies)
		if mode == 'auto':
			if env.GetPlatform() == 'win32':
				self.mode = 'static'
			else:
				self.mode = 'shared'
		else:
			self.mode = mode
			
		if deb_rootdir == '':
			deb_rootdir = env.SrcDir('deb')
			
		self.vsproject = CPPVisualStudioProject(self.env, self.name + '_cpplib', filesrootdir, sorts)
		DerivedDebianPaquetProject(
			self,
			deb_rootdir,
			name='lib'+self.name,
			dispatch_map = { '*.so*' : self.env['_DEB_LIB_INSTALL_DIR'] },			
		)

	def get_type (self):
		return 'cpplib'
	
	def declare (self):
		CppProject.declare (self)
		if self.mode == 'static':
			library = self.env.StaticLibrary (self.name, self.source_files)
			self.add_target(self.env.Install(self.env['_REPO_STLIB_DIR'], library))
		else:
			#if self.env.GetPlatform () == 'win32':
				# Automatic symbol file generation
				#stl = self.env.StaticLibrary (self.name + '_static', self.source_files)
				#targets.extend (self.env.Install (self.env['_REPO_STLIB_DIR'], stl))
				#self.env.Append ( LIBS = self.get_all_dependency_names (unaliased = True, reversed = True) )
				#shl = self.env.SharedLibrary (self.name, self.source_files)

				#targets.extend (self.env.Install (self.env['_REPO_SHLIB_DIR'], shl))

				# Under win32, declare an explicit dependency on a def file which must be based
				# on the static library
				#self.env['MAKEDEFLIBNAME'] = self.name
				#self.env['MAKEDEFLIBVERSION'] = '0.1'
				#deffile = self.env.MakeDef (self.env.File (self.name + '.def'), stl)
				#self.env.Depends (shl, deffile)
				#self.env.Depends (stl, deffile)

				#self.env.Append ( LINKFLAGS = ['/DEF:' + deffile[0].abspath] )
				#targets.extend (deffile)
			#else:
			library = self.env.SharedLibrary (self.name, self.source_files)
			self.add_target(self.env.Install (self.env['_REPO_SHLIB_DIR'], library))
			
	def get_libs(self):
		return [self.name.replace('_','-')]

	def get_libdir(self):
		return self.builddir


class CppBinProject (CppProject):

    def __init__ (self, env, name, version,
                  source_files, 
                  include_dir = None, 
                  prereq = None, 
                  dependencies = None, 
                  extra_dependencies = None,
                  filesrootdir = '',
                  deb_rootdir = '',
                  sorts = list()                
                  ):
      CppProject.__init__ (self, env, name, version, source_files, include_dir, prereq, dependencies, extra_dependencies)
      
      if deb_rootdir == '':
         deb_rootdir = env.SrcDir('deb')
      
      self.vsproject = CPPVisualStudioProject(env, name + '_cppbin', filesrootdir, sorts)
      DerivedDebianPaquetProject(self, deb_rootdir, dispatch_map = { '*' : (self.env['_DEB_BIN_INSTALL_DIR']) })

    def get_type (self):
      return 'cppbin'


    def declare (self):
      CppProject.declare (self)
      
      if self.env.GetPlatform () == 'posix':
        if self.env['_USE_BUILD_RPATH']:
          # use forge build dir
          self.env.Append ( RPATH = [ self.env['_REPO_SHLIB_DIR'] ] )
        else:
          # use installed lib (through deb) rpath)
          self.env.Append ( RPATH = [ self.env['_DEB_LIB_INSTALL_DIR'] ] )

      ibin = self.env.Program (self.name, self.source_files)

      libs = list()
      for prj in self.get_dependencies(): 
        for lib in prj[0].get_libs():
	  if lib not in libs:
            libs.append(lib)
      self.env.Append(LIBS = libs)

      self.add_target(ibin)
      #for dep in self.get_all_dependency_names (full_name = False, reversed = True):
          # force dependency to ensure lib files will be installed in repo when bin installed in repo
          #self.env.Depends (ibin, self.env.Alias (dep)) 

    def write_dependencies(self):
      for prj in self.get_dependencies():
        if prj[0].targets:
          self.env.Depends(self.targets[0], prj[0].targets[0])


class CppTestProject (CppProject):

    def __init__ (self, env, name, version, 
                  source_files, 
                  include_dir = None, 
                  prereq = None,
                  dependencies = None, 
                  extra_dependencies = None,
                  filesrootdir = ''
                  ):
      dependencies.append('boost-test.cpplib')
      CppProject.__init__ (self, env, name, version, source_files, include_dir, prereq, dependencies, extra_dependencies)
      self.vsproject = CPPVisualStudioProject(env, name + '_cpptest', filesrootdir)
		

    def get_type (self):
      return 'cpptest'
        
    def declare (self):
      CppProject.declare (self)
      self.env.Append ( CPPDEFINES = ['BOOST_TEST_NO_LIB='] )
      self.env.Append ( CPPDEFINES = ['BOOST_AUTO_TEST_MAIN='] )
      if self.env.GetPlatform () == 'posix':
        if self.env['_USE_BUILD_RPATH']:
          # use forge build dir
          self.env.Append ( RPATH = [ self.env['_REPO_SHLIB_DIR'] ] )
        else:
          # use installed lib (through deb) rpath)
          self.env.Append ( RPATH = [ self.env['_DEB_LIB_INSTALL_DIR'] ] )

      libs = list()
      for prj in self.get_dependencies(): 
        for lib in prj[0].get_libs():
          if lib not in libs:
            libs.append(lib)
      self.env.Append(LIBS = libs)

      #self.env.Append ( LIBS = self.get_all_dependency_names (full_name = False, reversed = True) )
    

      #for test in self.env['LIBS']:
      #  print test
      #ibin = self.env.Install (self.env['_REPO_BIN_DIR'], self.env.Program (self.name, self.source_files))
      #ibin = self.env.Program (self.name, self.source_files)
      #self.add_target(ibin)
      
      for test in self.source_files:
        testname = test.replace ('.cpp', '')
        itest = self.env.Test (testname + '.passed', self.env.Program ( testname, test)[0])
        self.env.AlwaysBuild (testname + '.passed')
        self.add_target(itest)
      
    def write_dependencies(self):
      for dep in self.get_all_dependency_names (full_name = False, reversed = True):
          # force dependency to ensure lib files will be installed in repo when bin installed in repo
          self.env.Depends (self.targets[0], self.env.Alias (dep)) 
            


class CppSynthese3LibProject(CppLibProject):
	## Constructor.
	## @param self self
	## @param env SCons environment
	## @param name Name of the project
	## @param version Version of the project
	## @param source_files List of sources files to compile
	## @param include_dir Path of the directory to include to use the library from an other project (will be automatically added in the include path at the build of projects that depend on self)
	## @param mode Shared or static library. Values are :
	##   - 'auto' (default) : static on windows, shared on linux
	##   - 'static' : static library
	##   - 'shared' : shared library
	## @param prereq List of prerequisites (Prereq objects)
	## @param dependencies List of internal dependencies (other projects of the forge : put full artefact name e.g. s3-admin.stlib). Default value : none
	## @param extra_dependencies List of external dependencies, to be used at the debian packet build. Default value : none
	## @param filesrootdir Root of sources files in the visual studio project : only the root and its subdirectories are visible on the solution tree. Default value : SConscript path
	def __init__ (self, env, name, version,
                  source_files, 
                  include_dir = None, 
                  mode = 'auto',
                  prereq = None,
                  dependencies = None, 
                  extra_dependencies = None,
                  filesrootdir = '',
                  deb_rootdir = ''
                  ):
		CppLibProject.__init__ (
			self, env, name,
			version,
			source_files,
			include_dir,
			mode,
			prereq,
			dependencies,
			extra_dependencies,
			filesrootdir,
			deb_rootdir,
			[	['01 Module', ['*Module.h*', '*Module.cpp', '*.gen.cpp', '*.inc.cpp', '*Types.h*', '*Constants.h*']],
				['01 Exceptions', ['*Exception.h*', '*Exception.cpp']],
				['10 Table syncs', ['*TableSync.h*', '*TableSync.cpp']],
				['11 Interface elements', ['*InterfaceElement.h*', '*InterfaceElement.cpp']],
				['11 Interface pages', ['*InterfacePage.h*', '*InterfacePage.cpp']],
				['12 Rights', ['*Right.h*', '*Right.cpp']],
				['13 DB Logs', ['*Log.h*', '*Log.cpp']],
				['14 Admin', ['*Admin.h*', '*Admin.cpp']],
				['15 Actions', ['*Action.h*', '*Action.cpp']],
				['15 Functions', ['*Function.h*', '*Function.cpp']],
				['16 File Formats', ['*FileFormat.h*', '*FileFormat.cpp']],
				['17 Alarm recipients', ['*AlarmRecipient.h*', '*AlarmRecipient.cpp']]
			] 
		)


class CppSynthese3BinProject(CppBinProject):
    def __init__ (self, env, name, version,
                  source_files, 
                  include_dir = None, 
                  prereq = None, 
                  dependencies = None, 
                  extra_dependencies = None,
                  filesrootdir = ''
                  ):
      CppBinProject.__init__ (self, env, name, version, source_files, include_dir, prereq, dependencies, extra_dependencies, filesrootdir)

    def declare(self):
      CppBinProject.declare(self)
      
      gsources = list ()
      isources = list ()
      for prj in self.get_dependencies():
        gsources.extend(self.env.Glob('*.gen.cpp', dir = self.env.SrcDir(prj[0].build_dir), exclude_gen = False, abs_path = True))
        isources.extend(self.env.Glob('*.inc.cpp', dir = self.env.SrcDir(prj[0].build_dir), exclude_gen = False, abs_path = True))
      self.env.Merge (self.env.SrcFile('generated.cpp.inc'), gsources)
      self.env.Merge (self.env.SrcFile('includes.cpp.inc'), isources)
      