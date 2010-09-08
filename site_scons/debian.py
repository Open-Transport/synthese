## @namespace debian Debian package relative projects.
## @ingroup forge2Projects
## @author Marc Jambert
## @author Hugues Romain

from project import *
from architecture import localarch
from msvs import *
import os

class DebPrereq :

  def __init__ (self, env, debname):
    self.env = env
    self.debname = debname
    self.localversion = None

  def check (self):
    ret = False
    answer = 'no'
    self.localversion = self.env.GetLocallyInstalledDebVersion (self.debname)
    if self.localversion != None: 
      ret = True
      answer = 'yes (' + self.localversion + ')'
    print 'Checking if deb package ' + self.debname + ' is installed... ' + answer
    return ret


class DebianPaquetProject(Project):

    ## Constructor.
    ## @param self Self
    ## @param env Environment to clone
    ## @param name Name of the package (without version informations)
    ## @param version Version of the package
    ## @param rootdir Fake root of the package (usually deb directory). Default = './deb'
    ## @param dependencies Project dependencies : the debian package built upon them are required
    ## @param extra_dependencies External dependencies : to be added in the package dependency rule
    ## @param dispatch_map Rules to dispatch files to be installed by the package
    ## @param sources List of generated elements to install
    ## @param architecture Name of the architecture of the package (local = architecture defined in architecture.py) See also http://www.debian.org/doc/debian-policy/ch-controlfields.html#s-f-Architecture
    def __init__(self, env, name, version,
			rootdir='',
			dependencies = None, extra_dependencies = None,
			dispatch_map = None,
			sources = list(),
			architecture = 'local'
		):
		if dispatch_map == None:
			self.dispatch_map = { '*' : '/opt/'+ name}
		else:
			self.dispatch_map = dispatch_map
		self.sources = sources
		if rootdir == '':
			self.rootdir = env.SrcDir('deb')
		else:
			self.rootdir = rootdir
		self.architecture = architecture
		
		Project.__init__(self, env, name, version, None, dependencies, extra_dependencies)
		
		if rootdir == '':
			rootdir = self.env.SrcDir('deb')
		DebianVisualStudioProject(
			self.env,
			name + '_deb',
			rootdir
		)
		self.deb_project = self
	##
    
    
    def get_type(self):
    	return 'deb'
    ##
    
    def get_package_name(self):
		return 'rcs-' + self.name.replace ('_', '-')
    
    def get_architecture(self):
        if self.architecture == 'local':
            return localarch
        else:
            return self.architecture


    def get_deb_name(self):
		if self.env.GetPlatform() == 'posix':
			return self.get_package_name() + '_' + self.get_version() + '_' + self.get_architecture() + '.deb'
		else:
			return self.get_package_name() + '_' + self.get_architecture() + '.deb'
	
    ## Gets the debian packages which the current project depends.
    ## @param self Self
    ## @param with_extra the result includes the dependencies on external packages (default = True)
    ## @param min_version true = Choose minimal versions of the dependencies as specified in Project instances,
	##	false = choose the last version of the dependencies
	## @param all  True = return all dependencies of the project (recursively), False = return only direct dependencies
    ## @return list of the dependency 3uples (package name, minimal version, project)
    def get_deb_dependencies (self, with_extra = True, min_version = False, all = False):
      deps = list ()
      prjs = dict ()
      ad = self.get_dependencies(all)
      for dep in ad:
        # stlib artefact cannot have any dependency
        #if dep[0][-6:] == '.stlib' : continue
        depprj = dep[0]
        depversion = depprj.get_version ()
        
		# Override dependency pkg version with minimal one
        if min_version and dep[1] != None:
        	depversion = dep[1]
        prjs[depprj] = depversion
        #print depversion + "\n"

      for (prj,minversion) in prjs.iteritems () : 
        if prj.deb_project != None and prj.deb_project != self:
          deps.append ( (prj.deb_project.get_package_name (), minversion, prj.deb_project) )
          if all and with_extra:
            for dep in prj.deb_project.extra_dependencies :
              deps.append (dep)
      if with_extra:
        for dep in self.extra_dependencies :
          deps.append (dep)
      return deps


    ## Targets declaration.
    ## The following environment variables are defined :
    ##  - DEBNAME = Name of the package
    ##  - DEBVERSION = Version of the package
    ##  - DEBDISPATCHMAP = Map containing the rules of generated files to be contained in the package
    ##  - DEBDIRPATH = Fake Root directory of the debian package (usually <src>/deb)
    ##  - DEBDEPENDS = Other debian packages direct dependencies (internal and external ones), result of DebianPaquetProject::get_deb_dependencies
    ##
    def declare(self):

      if self.candeclare == False: return
      
      self.env['DEBNAME'] = self.get_package_name()
      if self.env.GetPlatform() == 'posix':
        self.env['DEBVERSION'] = self.version.get()
      self.env['DEBDISPATCHMAP'] = self.dispatch_map
      self.env['DEBARCH'] = self.get_architecture()
      self.env['DEBDIRPATH'] = os.path.abspath(self.rootdir)
      if self.env.GetPlatform() == 'posix':
        self.env['DEBDEPENDS'] = self.get_deb_dependencies (min_version = True)
      
      #deb_sources.append (self.env.Value (str (sorted (self.env['DEBDEPENDS']))))
      
      debfile = self.env.Deb(self.get_deb_name(), self.sources)
      self.add_target(debfile)
      #self.env.AlwaysBuild(self.targets[0])
      #self.env.AlwaysBuild(self.targets[1])
      
    
      if self.env.GetPlatform() != 'posix':
        return;
      
      if self.env['_DEB_DPUT_HOST'] != '':
        dputhost = self.env['_DEB_DPUT_HOST']
        #if self.env['_DEB_TARGET_DIST'] == 'stable': dputhost = 'rcs-official'
        #dput_cmd = 'if [ ! -f /var/www/unstable/'+ os.path.basename(debfile[0].abspath) +' ]; then dput -f ' + dputhost + ' ' + debfile[1].abspath +';fi'
        dput_cmd = 'dput -f ' + dputhost + ' ' + debfile[1].abspath
        self.env.AddPostAction (debfile[1], dput_cmd)
        #self.env.AlwaysBuild (debfile[1])

    def write_dependencies(self):
      # if required declare explicitely deb dependencies 
      # (ie building a deb leads to building dependents deb)
      if self.env['_DEB_BUILD_DEPS']:
        debdeps = self.get_deb_dependencies (with_extra = False, all = True)
        # print '==========='+ self.name + ' depends on '
        for debdep in sorted (debdeps) :
          # print ' depends on ' + debdep[2].name + '/' + str(debdep[2].targets)
          self.env.Depends(self.targets[0], debdep[2].targets[1])
          # print debdep[2].name

class DerivedDebianPaquetProject(DebianPaquetProject):
    
    ## Constructor.
    ## @param self Self
    ## @param source_project Project to package
    ## @param rootdir Fake root of the package (usually deb directory)
    ## @param name Name of the package (without version informations, default = source project name)
    ## @param dispatch_map Rules to dispatch files to be installed by the package (default = all in opt/<project_name>)
    ## @param sources List of generated elements to install (default = source project targets)
    def __init__ (self, source_project, rootdir, name='', dispatch_map = None, sources = None):
        if name == '':
            name = source_project.name
        if sources == None:
            sources = source_project.targets

        DebianPaquetProject.__init__(
    		self,
    		source_project.env,
    		name,
    		source_project.version,
    		rootdir,
    		source_project.dependencies,
    		source_project.extra_dependencies,
    		dispatch_map,
    		sources
    	)
    	
    	source_project.deb_project = self
    ##



class DebianVisualStudioProject(VCProjVisualStudioProject):
	def __init__(self, env, name, filerootdir):
		VCProjVisualStudioProject.__init__(self, env, name, ['*', '*.*'], [], filerootdir)
	##
