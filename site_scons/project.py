
from version import *

## Project base class.
## A project corresponds to a source collection which can be used to build an artefact.
## Artefact types are :
##  - An executable binary 
##  - A library (shared or static)
##  - A readable documentation directory (eg : HTML, PDF, etc. formats)
##  - A ready-to-use SQL data file (eg : a SQL dump)
##  - A debian paquet containing the builded elements and installation process
##  - A visual studio project file
##  - The visual studio solution file
##  - etc.
##
## Each project type can create automatically other projects. See the documentation of each project type.
##
## Internal attributes :
##  - env : the SCons environment dedicated to the project
##  - build_dir : the build directory.
##  - name : name of the project (without termination. To obtain full name, use get_full_name() method)
##  - version : version number
##  - targets : targets to build to complete the project build
##  - candeclare : if false, the project is ignored at the build time
##  - prereq : list of prerequisites as objects of classes. If one of the prerequisites is not respected, the project is not declared.
##  - dependencies : list of internal dependencies (project name, version)
##  - extra_dependencies : list of external dependencies (fox linux debian installation only)
##  - all_dependencies : dict (name, project) of all dependencies (including the ones of the sub folders
##
## A Project subclass must instanciate :
##  - get_type
class Project :
	
	## @param env SCons environment. It is automatically cloned. Clone it before the call if several variables are added before the object creation
	def __init__ (
		self,
		env,
		name,
		version = '0.0',
		prereq = None,
		dependencies = None,
		extra_dependencies = None,
	):
		self.name = name
		self.env = env.Clone()
		
		self.targets = list()
	
		self.src_dir = self.env.SrcDir('.')
		self.build_dir = self.env.Dir('.')
		self.deb_project = None
		
		if isinstance (version, PrjVersion):
			self.version = version
		else:
			self.version = PrjVersion (self, version)
		
		self.candeclare = True
		
		self.prereq = []
		if prereq != None:
			self.prereq.extend (prereq)
	
		self.dependencies = []
		self.extra_dependencies = []
		self.all_dependencies = None
			
		# Steps
		if self.env.GetRootEnv()['_BUILD_STEP'] == 0:
			if dependencies != None:
				assert isinstance (dependencies, list)
				for dep in dependencies:
					if isinstance (dep, list) or isinstance (dep, tuple):
						# if couple => (depname, depversion)
						# where depversion is the minimal dependency version 
						# with which the project will be compatible
						self.dependencies.append(dep)
						# print self.name +' depends on '+ dep[0]
					else:
						# None is used to flag that the dependency minimal version
						# will be the current one
						self.dependencies.append ((dep, None))
						# print self.name +' depends on '+ dep
		
			if extra_dependencies != None:
				self.extra_dependencies.extend (extra_dependencies)
		
			# Registration of the project in the main env
			self.env.GetRootEnv()['_PROJECTS'][self.get_full_name()] = self
		
		if self.env.GetRootEnv()['_BUILD_STEP'] == 1:
			project = self.env.GetProject(self.get_full_name())
			if project.check_prereq():
				project.declare()
			project.env.Alias(project.get_full_name(), project.targets)
		
		if self.env.GetRootEnv()['_BUILD_STEP'] == 2:
			project = self.env.GetProject(self.get_full_name())
			project.write_dependencies()



	def add_target(self, target):
		self.targets.extend(target)


	
	def get_version (self):
		return self.version.get ()

	

	def check_prereq (self):
		for prq in self.prereq:
			if prq.check ():
				self.version.add_prereq_version (prq.localversion)
			else:
				self.candeclare = False
		if self.candeclare == False : print 'Project ' + self.name + ' will not be declared'
		return self.candeclare



	## Do nothing (default behavior)
	def write_dependencies(self):
		return
	##



	## Gets the full name of the project.
	## @return The name of the project, dot, the type of the project. All _ characters are replaced by - characters.
	def get_full_name(self):
		full_name = self.name +'.'+ self.get_type()
		return full_name.replace('_','-')
	##

	
	
	def get_type (self):
		raise OSError, 'This project cannot be instanciated'
	##

	
	
	## Gets all dependencies by recursive exploration.
	## @param all True = return all dependencies of the project (recursively), False = return only direct dependencies
	## @return couple of (project, minimal version)
	def get_dependencies(
		self,
		all = True
	):
		if self.env.GetRootEnv()['_BUILD_STEP'] < 1:
			raise OSError, "Forbidden in this build step"
		thisproject = self.env.GetProject(self.get_full_name())
		
		# Use cached result (only with all option)
		if all and thisproject.all_dependencies != None:
			return thisproject.all_dependencies

		todo = list()
		done = list()
		result = list()
		todo.extend(thisproject.dependencies)
		while len(todo) > 0:
			dep = todo.pop()
			# print self.name +' depends on '+ dep[0]
			done.append(dep)
			project = self.env.GetProject(dep[0])
			if project == None:
				print '*** WARNING dependency '+ dep[0] +' not found in '+ thisproject.get_full_name() +' project'
				continue
			
			if project.get_full_name() == thisproject.get_full_name():
				continue
	
			if project not in result:
				result.append((project, dep[1]))
	
			if all:
				for recursive_dep in project.dependencies:
					if recursive_dep not in todo and recursive_dep not in done:
						todo.append(recursive_dep)
	
		# Caching result (only with all option)
		if all:
			thisproject.all_dependencies = result
		return result 



	def get_dependency_project (self, depname):
		projects = self.env.GetRootEnv ()['_PROJECTS']
		if projects.has_key(depname):
			return projects[depname]
		return None
	##



	## Targets declaration.
	def declare (self):
		return
	


	def get_dependency_projects (self): 
		prjs = dict ()
		for dep in self.dependencies :
			# stlib artefact cannot have any dependency
			if dep[-6:] == '.stlib' : continue
			prjs[self.get_dependency_project (dep[0])] = None
		return prjs.keys ()



	def has_dependency (self, depname):
		for d in self.get_dependencies() :
			if d[0].name == depname : return True
		return False



	def get_all_dependency_names (self, full_name = True, reversed = False):
		all = list ()
		for d in self.get_dependencies() : 
		
			# windows build hack to handle dependency spaghetti :
			# ignore stlib if same lib is present as well...
			#if d.name[-6:] == '.stlib' and self.has_dependency (d.name[:-6] + '.lib'): 
			#    continue
			if full_name: 
				all.append (d[0].get_full_name())
			else:
				all.append (d[0].name)
		if reversed : all.reverse ()
		return all
