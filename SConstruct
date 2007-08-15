import os, filecmp, tempfile
import os, fnmatch

# -------------------------------------------------------
# Command line arguments
# -------------------------------------------------------
opts = Options('custom.py')
opts.Add('CC', 'The C compiler.')
opts.Add('CXX', 'The C++ compiler.')

rootenv = Environment(ENV = os.environ, options=opts)

Help(opts.GenerateHelpText(rootenv))

mode = ARGUMENTS.get('mode', 'release').lower()  
platform = ARGUMENTS.get('os', str (Platform()))
librepo = ARGUMENTS.get('librepo')   
goal = ARGUMENTS.get('goal', '').lower()  
toolset = ARGUMENTS.get('toolset').lower()  
version = ARGUMENTS.get('version').lower()  
boostversion = ARGUMENTS.get('boostversion').lower()  
sqliteversion = ARGUMENTS.get('sqliteversion').lower()
zlibversion = ARGUMENTS.get('zlibversion').lower()
distname = ARGUMENTS.get('distname', 'dist').lower()


print "librepo = ", librepo
print "platform = ", platform
print "mode     = ", mode
print "goal     = ", goal
print "toolset  = ", toolset
print "cc  = ", rootenv['CC']
print "cxx  = ", rootenv['CXX']


rootenv.Replace ( PLATFORM = platform )
rootenv.Replace ( MODE = mode )

buildroot = 'build' + '/' + platform + '/' + toolset +'/' + mode
buildmain = buildroot + '/main'
buildtest = buildroot + '/test'

resourcesroot = 'resources'
resourcesmain = 'resources' + '/main'
resourcestest = 'resources' + '/test'
resourcesdist = 'resources' + '/dist/' + platform


distroot = 'dist' + '/' + platform + '/' + toolset +'/' + mode



rootenv.Replace ( BUILDROOT = '#' + buildroot )





def IsDebug (env):
  return env['MODE'] == 'debug' or env['MODE'] == 'debug'


def IsRelease (env):
  return env['MODE'] == 'release'

def IsProfile (env):
  return env['MODE'] == 'debug'



# -------------------------------------------------------
# Common methods
# -------------------------------------------------------

# Allows to glob files from the build directory going
# through scons nodes
def Glob( env, pattern = '*', excludes = [], dir = '.' ):
    files = []
    excludes.append( '*.gen.cpp' )
    excludes.append( '*.inc.cpp' )
    for file in os.listdir( Dir(dir).srcnode().abspath ):
        if fnmatch.fnmatch(file, pattern) :
            excluded = False
            for xpattern in excludes :
              if fnmatch.fnmatch(file, xpattern) :
                excluded = True

            if excluded == False :
              files.append( os.path.join( dir, file ) )

    return files


def CopyFile (env, dest, src, onlyIfDifferent = True):
  if (os.path.exists (dest) == False) or (filecmp.cmp (dest, src, shallow = False) == False):
    Execute (Copy (dest, src))


def CopyFiles (env, dest, files, recursive = False, onlyIfDifferent = True):
    for file in files:
        filename = os.path.basename (file)
    	if recursive and os.path.isdir (file) :
           newdest = dest + '/' + filename
	   Execute (Mkdir (newdest))
	   CopyFiles (env, newdest, Glob (env, dir = file), recursive)
	elif os.path.isdir (file) == False:
	   destfile = os.path.join (dest, filename)
           CopyFile (env, destfile, file)


def DefaultModuleName ( env ):
  prefix = 'src/main/'
  mn = env.Dir('.').srcnode ().abspath
  mn = mn[mn.rfind (prefix) + len(prefix):].replace('/', '__')
  return mn


def DefaultTestModuleName ( env, dir = '.' ):
  prefix = 'src/test/'
  mn = env.Dir('.').srcnode ().abspath
  mn = mn[mn.rfind (prefix) + len(prefix):].replace('/', '__')
  return mn + '_test'


def DefineDefaultLibPath (env):
    platform = env['PLATFORM']

    if (platform=='win32'):
        env.Prepend ( CPPPATH = [librepo + '/' + 'include'] )
        env.Prepend ( LIBPATH = [librepo + '/' + 'lib'] )
    elif (platform=='darwin'):
        env.Prepend ( CPPPATH = [librepo + '/' + 'include'] )
        env.Prepend ( LIBPATH = [librepo + '/' + 'lib'] )
    elif (platform=='posix'):
        env.Prepend ( CPPPATH = [librepo + '/' + 'include'] )
        env.Prepend ( LIBPATH = [librepo + '/' + 'lib'] )

    
def DefineDefaultCPPDefines (env):
    platform = env['PLATFORM']
    mode = env['MODE']

    if (platform=='posix') or (platform=='darwin'):
        env.Append ( CPPDEFINES = ['UNIX'] )
        if (env.IsDebug ()):
            env.Append ( CPPDEFINES = ['DEBUG'] )
    
    elif (platform=='win32'):
        env.Append ( CPPDEFINES = ['_MBCS', '_USE_MATH_DEFINES', '__WIN32__', 'WIN32', 'BOOST_ALL_NO_LIB'] )
        if (env.IsDebug ()):
            env.Append ( CPPDEFINES = ['DEBUG', '_DEBUG'] )
        else:  
            env.Append ( CPPDEFINES = ['NDEBUG'] )


def DefineDefaultCCFlags (env):
    platform = env['PLATFORM']
    mode = env['MODE']

    if (platform=='posix'):
        if (env.IsDebug ()):
            env.Append ( CCFLAGS = ['-ggdb', '-fpermissive'] )
        else:
            env.Append ( CCFLAGS = ['-O3', '-fpermissive'] )
        if env.IsProfile ():
            env.Append ( CCFLAGS = ['-pg'] )
            

    elif (platform=='darwin'):
        if (env.IsDebug ()):
            env.Append ( CCFLAGS = ['-ggdb', '-fno-inline'] )
        else:
            env.Append ( CCFLAGS = ['-O3', '-fno-inline', '-fno-strength-reduce'] )


    elif (platform=='win32'):
        env.Append ( CCFLAGS = ['/EHsc', '/GR', '/GS', '/W3', '/nologo', '/c', '/Wp64'] )
        env.Append ( CCFLAGS = ['/wd4005', '/wd4996'])

        if (env.IsDebug ()):
            env.Append ( CCFLAGS = ['/Od', '/MTd', '/Gm', '/RTC1', '/ZI'] )
        else:  
            env.Append ( CCFLAGS = ['/Ox', '/FD', '/MT', '/Zi'] )
    
  
def DefineDefaultLinkFlags (env):
    platform = env['PLATFORM']
    mode = env['MODE']

    if (platform=='win32'):
        env.Append ( LINKFLAGS = ['/NOLOGO', '/MACHINE:X86', '/OPT:NOREF', '/INCREMENTAL:NO', '/FIXED:NO'] )
        if (env.IsDebug ()):
          env.Append ( LINKFLAGS = ['/DEBUG'] )
    
    if platform == 'posix':
      if env.IsProfile ():
        env.Append ( LINKFLAGS = ['-pg'] )




def DefineDefaultLibs (env):
    platform = env['PLATFORM']
    mode = env['MODE']

    if (platform=='posix'):
        # By default, everything has to be linked dynamically on posix
        # Too many problems with static links!
        env.Append ( LIBS = ['dl'] )

    if (platform=='win32'):
        env.Append ( LIBS = ['advapi32.lib'] )  





def AddDependency (env, libname, libversion, multithreaded):
    deplib = libname

    if multithreaded:
      deplib = deplib + "-mt"
    
    if platform == 'posix':
      # ALWAYS DYNAMIC LINK !
      if env.IsDebug () :
        deplib = deplib + "-d"
      
      
    if platform == 'win32':
      # ALWAYS STATIC LINK !
      deplib = 'lib' + deplib + "-s"
      if env.IsDebug () :
        deplib = deplib + "gd"
        
    deplib = deplib + "-" + libversion
    env.Append (LIBS = [deplib] )
    distlib = env['LIBPREFIX'] + deplib + env['SHLIBSUFFIX']
    env.Append (DISTLIBS = [librepo + '/lib/' + distlib])



def AddBoostDependency (env, libname):
  AddDependency (env, libname, boostversion, True)
    


def AddSQLiteDependency (env):
  AddDependency (env, "sqlite", sqliteversion, True)
    
def AddZlibDependency (env):
  AddDependency (env, "zlib", zlibversion, True)
    


def AppendMultithreadConf (env):
    if (platform == 'posix'):
        env.Append (CCFLAGS= '-pthread' )
        env.Append (LIBS= ['pthread'] ) 

    if (platform == 'darwin'):
        env.Append (CCFLAGS= '-pthread' )
        env.Append (LIBS= ['pthread'] ) 

    elif (platform == 'win32'):
        env.Append ( LIBS = ['ws2_32.lib'] )  





def AddModuleDependency (env, modulename):
    env.Append ( MODULES = [modulename] )  
    env.Append ( LIBS = [modulename] )
    #lp = '../../../main/' + modulename.replace ('__', '/')
    lp = Dir('#' + buildmain).abspath + '/' + modulename.replace ('__', '/')

    env.Append ( LIBPATH = [lp] )  
      
    
    # print '*** ', Dir('../../main').abspath + '/' + modulename.replace ('.', '/')
    #env.Append ( LIBPATH = [Dir(buildmain).abspath + '/' + modulename.replace ('.', '/')] )  
    #env.Append ( LIBPATH = [buildmain + '/' + modulename.replace ('.', '/')] )  
    #libdir = Dir('src/main').abspath + '/' + modulename
    #print '%' , libdir
    #env.Append ( LIBPATH = [libdir])








def ModuleBuild (moduleenv, includes = '*.cpp', excludes = [], recursive = True):

    # default include path
    # moduleenv.Append (CPPPATH = [Dir('../../main').srcnode().abspath] )

    # Create module static library

    files = moduleenv.Glob(includes, excludes)
    
    moduleenv.StaticLibrary ( moduleenv.DefaultModuleName(), files )
    
    moduleenv.RecursiveBuild ()




def RecursiveBuild (parentenv):
    env = parentenv.Copy ()

    builddir = Dir('.').abspath
    srcpath = Dir('.').srcnode().abspath
    
    subdirs = os.listdir( srcpath )
    modules = []

    # Build the list of modules to be processed
    for f in subdirs:
      if not os.path.isdir (os.path.join (srcpath, f)): continue
      if not os.path.exists (os.path.join (srcpath, f, 'SConscript')): continue
      modules.append (f)
    
    for module in modules:
      s0 = os.path.join (srcpath, module, 'SConscript')
      s1 = os.path.join (builddir, module)
      env.SConscript(s0, build_dir=s1, duplicate=0, exports=['env'])







def SyntheseEnv (env, modules):
    syntheseenv = env.Copy ()
    includes = 'main.cpp'
    excludes = []

    files = syntheseenv.Glob(includes, excludes)

    for module in modules:
        syntheseenv.AddModuleDependency (module)

    # Some hard-coded dependencies
    syntheseenv.AddBoostDependency ('boost_program_options')
    syntheseenv.AddBoostDependency ('boost_filesystem')
    syntheseenv.AddBoostDependency ('boost_date_time')
    syntheseenv.AddBoostDependency ('boost_thread')
    syntheseenv.AddBoostDependency ('boost_iostreams')
    syntheseenv.AddSQLiteDependency ()
    syntheseenv.AddZlibDependency ()
    syntheseenv.AppendMultithreadConf ()

    return syntheseenv




def UnitTestEnv (env, modules):
    testenv = env.Copy ()
    testenv.Append ( CPPDEFINES = ['BOOST_AUTO_TEST_MAIN='] )

    for module in modules:
        testenv.AddModuleDependency (module)

    # Some hard-coded dependencies
    testenv.AddBoostDependency ('boost_program_options')
    testenv.AddBoostDependency ('boost_filesystem')
    testenv.AddBoostDependency ('boost_date_time')
    testenv.AddBoostDependency ('boost_thread')
    testenv.AddBoostDependency ('boost_iostreams')
    testenv.AddBoostDependency ('boost_unit_test_framework')
    testenv.AddSQLiteDependency ()
    testenv.AddZlibDependency ()
    testenv.AppendMultithreadConf ()

    return testenv



def UnitTest (env):
  includes = '*Test.cpp'
  excludes = []
  files = env.Glob(includes, excludes)
  for f in files:
    binname = f.replace ('.cpp', '')
    testprogram  = env.Program ( binname, f )[0]
    env.Test(binname + '.passed', testprogram)
    env.AlwaysBuild (binname + '.passed')

  env.RecursiveBuild ()





def SyntheseDist (env, exeprog):
    
  resourcesdistdir = '#' + resourcesdist + '/' + exeprog.name
  installerdir = distname + '_dist'
  localdir = installerdir + '/_' + distname
  distexeprog = File (localdir + '/' + distname)
  env.Command (distexeprog, exeprog,
               Copy (distexeprog.abspath, exeprog.abspath))
      
  for distlib in env['DISTLIBS']:
    distlibsrc = File (distlib)
    distlibtarget = File (localdir + '/libs/' + os.path.basename (distlib))
    env.Command (distlibtarget, distlibsrc,
                 Copy (distlibtarget.abspath, distlibsrc.abspath))
    Depends (Dir (localdir), distlibtarget)

  # Generate startup.sh
  runnertemplate = File (resourcesdistdir + '/startup.sh')
  runnerfile = File (localdir + '/startup.sh')
  env.DistScript (runnerfile, [runnertemplate, distexeprog])
  Depends (Dir (localdir), runnerfile)

  installerscripts = env.Glob('*.sh', ['startup.sh'], dir = resourcesdistdir)
  for inscript in installerscripts:
    installertemplate = File (inscript)
    installerfile = File (installerdir + '/' + installertemplate.name)
    env.DistScript (installerfile, [installertemplate, distexeprog])
    Depends (Dir (installerdir), installerfile)

  Depends (Dir (installerdir), Dir (localdir))
  env.AlwaysBuild (Dir (installerdir))




def SyntheseBuild (env, binname, generatemain = True):

    if generatemain:
      # Copy main.cpp from template.
      maintemplate = env.File ('../synthese_template/main.cpp').srcnode ().abspath;
      maincopy = env.File ('main.cpp').srcnode ().abspath;
      env.CopyFile (maincopy, maintemplate);
    
    files = env.Glob('main.cpp', [])
    
    if generatemain:
      # source is expected to be main.cpp file
      # parse this path to get right synthese module name
      currentmodule = os.path.basename (env.Dir ('.').abspath);
    
      # Look in all module folders for files with extension .cpp.gen
      # and dump them inside a generated.cpp file, to be included in main.cpp
      # Modules are traversed in reverse dependency order.
    
      modules = env['MODULES']
    
      # Create a temporary file
      tmpgen = open (tempfile.gettempdir () + '/generated.cpp.inc', "w" )
      tmpinc = open (tempfile.gettempdir () + '/includes.cpp.inc', "w" )

      for module in reversed (modules):
        moduledir = os.path.join (env.Dir ('..').srcnode ().abspath, module.replace ('__', '/'))
        for file in os.listdir (moduledir) :
          if fnmatch.fnmatch (file, '*.gen.cpp') :
            fragmentfile = os.path.join (moduledir, file)
            # dump the file to generated output
            fragment = open (fragmentfile, "r")
            tmpgen.write (fragment.read ())
            fragment.close ()
          if fnmatch.fnmatch (file, '*.inc.cpp') :
            fragmentfile = os.path.join (moduledir, file)
            # dump the file to generated output
            fragment = open (fragmentfile, "r")
            tmpinc.write (fragment.read ())
            fragment.close ()
    
      tmpgen.close ()
      tmpinc.close ()

      env.CopyFile (env.Dir ('#src/main').abspath + '/' + currentmodule + '/generated.cpp.inc', tmpgen.name)
      env.CopyFile (env.Dir ('#src/main').abspath + '/' + currentmodule + '/includes.cpp.inc', tmpinc.name)


    exeprog = env.Program ( binname, files )[0]
    
    if goal == 'dist':
      env.SyntheseDist (exeprog)

			       



# Declare common methods
from SCons.Script.SConscript import SConsEnvironment
SConsEnvironment.Glob=Glob
SConsEnvironment.CopyFile=CopyFile
SConsEnvironment.CopyFiles=CopyFiles
SConsEnvironment.DefaultModuleName=DefaultModuleName
SConsEnvironment.DefaultTestModuleName=DefaultTestModuleName
SConsEnvironment.DefineDefaultLibPath=DefineDefaultLibPath
SConsEnvironment.DefineDefaultCPPDefines=DefineDefaultCPPDefines
SConsEnvironment.DefineDefaultCCFlags=DefineDefaultCCFlags
SConsEnvironment.DefineDefaultLinkFlags=DefineDefaultLinkFlags
SConsEnvironment.DefineDefaultLibs=DefineDefaultLibs
SConsEnvironment.AppendMultithreadConf=AppendMultithreadConf

SConsEnvironment.ModuleBuild=ModuleBuild
SConsEnvironment.RecursiveBuild=RecursiveBuild
SConsEnvironment.UnitTestEnv=UnitTestEnv
SConsEnvironment.SyntheseEnv=SyntheseEnv
SConsEnvironment.IsDebug=IsDebug
SConsEnvironment.IsRelease=IsRelease
SConsEnvironment.IsProfile=IsProfile

SConsEnvironment.SyntheseBuild=SyntheseBuild
SConsEnvironment.SyntheseDist=SyntheseDist

SConsEnvironment.UnitTest=UnitTest


SConsEnvironment.AddModuleDependency=AddModuleDependency
SConsEnvironment.AddBoostDependency=AddBoostDependency
SConsEnvironment.AddSQLiteDependency=AddSQLiteDependency
SConsEnvironment.AddZlibDependency=AddZlibDependency




# -------------------------------------------------------
# Custom builders
# -------------------------------------------------------
def builder_unit_test(target, source, env):
  app = str(source[0].abspath)

  # Exec in the test program dir!
  curdir = os.path.abspath (os.curdir)
  os.chdir (os.path.dirname (source[0].abspath))

  if os.system(app)==0:
    os.chdir (curdir)
    open(str(target[0]),'w').write("PASSED\n")
  else:
    os.chdir (curdir)
    return 1




def builder_distscript (target, source, env):
  runnerresource = open(str(source[0]),'r')
  runnerscript = runnerresource.read ();
  runnerresource.close()

  distprog = source[1]
  runnerscript = runnerscript.replace ('#DISTNAME#', distprog.name)

  runner = open(str(target[0]),'w')
  runner.write (runnerscript)
  runner.close ()
  Execute(Chmod(target[0].abspath, 0755))
  return None



  


# -------------------------------------------------------
# Common build configuration
# -------------------------------------------------------

rootenv.DefineDefaultLibPath ()
rootenv.DefineDefaultCPPDefines ()
rootenv.DefineDefaultCCFlags ()
rootenv.DefineDefaultLinkFlags ()
rootenv.DefineDefaultLibs ()

# default include path
rootenv.Append (CPPPATH = ['#src/main'] )


bld = Builder(action = builder_unit_test)
rootenv.Append(BUILDERS = {'Test' :  bld})

bld = Builder(action = builder_distscript)
rootenv.Append(BUILDERS = {'DistScript' :  bld})


rootenv.AppendMultithreadConf ()

Export('rootenv')





# -------------------------------------------------------
# Build process
# -------------------------------------------------------


builddir = '#' + buildroot

rootenv.SConscript ('src/main/SConscript', build_dir = builddir + '/main', duplicate = 0)
rootenv.SConscript ('src/test/SConscript', build_dir = builddir + '/test', duplicate = 0)


    
    











