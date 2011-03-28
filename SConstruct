## @file SConstruct
## This is the SConstruct file for RCS forge build system.
## @author Marc Jambert
## @author Hugues Romain

import os, filecmp, tempfile
import os, fnmatch
import time, datetime


EnsureSConsVersion(1, 0)


vars = Variables('config.py')

vars.AddVariables(
   EnumVariable('_CPPMODE', 'C/C++ compilation mode', 'release', allowed_values = ('debug', 'release', 'profile')),
   )

env = Environment()
env.Replace ( _PLATFORM = str (Platform()))

#if env['_PLATFORM'] == 'posix':
vars.AddVariables(
    BoolVariable('_DEB_BUILD_DEPS', 'Build deb dependencies files if needed', False),
    EnumVariable('_DEB_TARGET_DIST', 'Targeted debian distribution', 'unstable', allowed_values = ('unstable', 'testing', 'stable')),
    EnumVariable('_DEB_DPUT_HOST', 'Upload automatically generated deb package to proper repository', 'rcs-local', allowed_values = ('','rcs-local', 'rcs-official')),
    BoolVariable('_USE_BUILD_RPATH', 'Use build/_CPPMODE/repo/lib for runtime search path (posix only)', False))

vars.Add ('_LOCAL_EROS_SHAPE_DROP_DIR', 'Local eros shape drop dir to use', '/srv/pub/tmp/shape/')
vars.Add('_REFERENCE_ENTERPRISE_HOST', 'Host where enterprise reference mysql db is stored', 'localhost')
vars.Add('_REFERENCE_ENTERPRISE_USER', 'User name to log onto enterprise reference mysql db')
vars.Add('_REFERENCE_ENTERPRISE_PASSWORD', 'Password to log onto enterprise reference mysql db')
vars.Add('_BUILD_ROOT','Build root')
vars.Add('S3VERSION','Version of the project')


vars.Add('CC', 'C compiler to use.')
vars.Add('CXX', 'C++ compiler to use.')

# note : specifying doxygen as a tool will call generate on doxygen.py toolpath
# TODO: add option for selecting between x86 and x86_64?
env = Environment(ENV = os.environ, variables=vars, tools= ['default', 'merge', 'bjam',
                                                          'doxygen', 'untar', 'unzip',
                                                          'test', 'makedef', 'msvsproject',
                                                          'msvssolution', 'msvsphpproject', 'deb',
                                                          'mysqldump', 'enterprises3', 'enterprises2',
                                                          'forge_html_doc', 'trident'
                                                          ], TARGET_ARCH='x86',
                                                          MSVC_VERSION='9.0')
env.Decider('MD5-timestamp')
Help(vars.GenerateHelpText(env))


env.Replace ( _PLATFORM = str (Platform()))
env['_BUILD_TIMESTAMP'] = time.gmtime ()  #datetime.datetime.now()
env['_BUILD_TIMESTAMP_FILESUFFIX'] = time.strftime ('%Y%m%d.%H%M%S', env['_BUILD_TIMESTAMP'])
env['_BUILD_TIMESTAMP_RFC2822'] = time.strftime ('%a, %d %b %Y %H:%M:%S +0000', env['_BUILD_TIMESTAMP'])


env['_REPO_DIR'] = '#build' + os.sep + env['_CPPMODE'] + os.sep + 'repo'

if env['_PLATFORM'] == 'win32':
   env['_REPO_STLIB_DIR'] = env['_REPO_DIR'] + '/stlib'
   env['_REPO_SHLIB_DIR'] = env['_REPO_DIR'] + '/lib'
   env['_REPO_BIN_DIR'] = env['_REPO_DIR'] + '/bin'
   env['_REPO_DOC_DIR'] = env['_REPO_DIR'] + '/doc'
else:
   env['_REPO_STLIB_DIR'] = env['_REPO_DIR'] + '/stlib'
   env['_REPO_SHLIB_DIR'] = env['_REPO_DIR'] + '/bin'
   env['_REPO_BIN_DIR'] = env['_REPO_DIR'] + '/bin'
   env['_REPO_DOC_DIR'] = env['_REPO_DIR'] + '/doc'

env['_ORDERED_DEPENDENCIES'] = list ()

if env['_PLATFORM'] == 'posix':
   env['_REPO_DEB_DIR'] = env['_REPO_DIR'] + '/deb' + os.sep + env['_DEB_TARGET_DIST']
   #reqcmd = 'mysql -E -h ' + env['_REFERENCE_ENTERPRISE_HOST'] + ' -u ' + env['_REFERENCE_ENTERPRISE_USER'] + ' --password=' + env['_REFERENCE_ENTERPRISE_PASSWORD'] + ' -e "SELECT MAX(UPDATE_TIME) FROM information_schema.TABLES WHERE (TABLE_SCHEMA=\'enterprise\' OR TABLE_SCHEMA=\'tmp_import_hastus\' OR TABLE_SCHEMA=\'tmp_export_synthese\')" | grep -v \"*\" | cut -d \' \'  -f 2-3'
   #env['_REFERENCE_ENTERPRISE_LASTUPDATE'] = os.popen(reqcmd).readline ()[:-1].replace('-','').replace(':','').replace(' ','.')
   #print '** enterprise db last modified on ' + env['_REFERENCE_ENTERPRISE_LASTUPDATE'] + ' **'
   #if os.path.exists (env['_LOCAL_EROS_SHAPE_DROP_DIR']):
   #   env['_REFERENCE_EROSSHAPE_LASTUPDATE'] = time.strftime ('%Y%m%d.%H%M%S', time.localtime(os.path.getmtime(env['_LOCAL_EROS_SHAPE_DROP_DIR'])))
   #   print '** eros drop dir last modified on ' + env['_REFERENCE_HASTUSZIP_LASTUPDATE'] + ' **'
   #else:
   #   print '** no eros shape dir found; some data package might not be built' + ' **'
   #   env['_REFERENCE_EROSSHAPE_LASTUPDATE'] = None
# install dirs for components installed with generated debian packages
env['_DEB_LIB_INSTALL_DIR'] = '/opt/rcs/lib/'
env['_DEB_BIN_INSTALL_DIR'] = '/opt/rcs/bin/'
env['_DEB_ETC_INSTALL_DIR'] = '/etc/opt/rcs/'
env['_DEB_VAR_INSTALL_DIR'] = '/srv/data/'
env['_DEB_DOC_INSTALL_DIR'] = '/srv/doc/'

#os.environ['COMSPEC'] = 'c:\windows\system32\cmd.exe'
#os.environ['TEMP'] = 'c:\temp'


#Repository ('#3rd')

env.Replace ( _ROOTENV = env )


#env.Replace ( _DEPS = dict () )


#env.Append (CPPPATH = ['.'])


# Set default toolset
if env['_PLATFORM'] == 'posix':
   env.Replace ( _CPPTOOLSET = 'gcc' )
if env['_PLATFORM'] == 'win32':
   env.Replace ( _CPPTOOLSET = 'msvc' )





# Default cpp defines
if (env['_PLATFORM'] == 'posix') or (env['_PLATFORM'] == 'darwin'):
   env.Append ( CPPDEFINES = ['UNIX'] )
   if (env.IsCppModeDebug ()):
      env.Append ( CPPDEFINES = ['DEBUG'] )
   else:
      env.Append ( CPPDEFINES = ['NDEBUG'] )

elif (env['_PLATFORM']=='win32'):
   env.Append ( CPPDEFINES = ['_MBCS', '_USE_MATH_DEFINES', '__WIN32__', 'WIN32', 'NOMINMAX'] )
   if (env.IsCppModeDebug ()):
      env.Append ( CPPDEFINES = ['DEBUG', '_DEBUG'] )
   else:
      env.Append ( CPPDEFINES = ['NDEBUG'] )



# Default cpp compilation options
if (env['_PLATFORM'] == 'posix'):
   if (env.IsCppModeDebug ()):
      env.Append ( CCFLAGS = ['-ggdb', '-Wall', '-Wextra', '-pedantic', '-Wno-long-long',  '-Wno-parentheses', '-Wno-unused-parameter'] ) # '-Werror=return-type',
   else:
      env.Append ( CCFLAGS = ['-O3'] )
   if env.IsCppModeProfile ():
      env.Append ( CCFLAGS = ['-pg'] )

elif (env['_PLATFORM'] == 'darwin'):
   if (env.IsCppModeDebug ()):
      env.Append ( CCFLAGS = ['-ggdb', '-fno-inline'] )
   else:
      env.Append ( CCFLAGS = ['-O3', '-fno-inline', '-fno-strength-reduce'] )

elif (env['_PLATFORM']=='win32'):
   env.Append ( CCFLAGS = ['/bigobj', '/EHsc', '/GR', '/GS', '/W3', '/nologo', '/c'] )
   env.Append ( CCFLAGS = ['/wd4005', '/wd4996', '/wd4290', '/wd4503'])

   if (env.IsCppModeDebug ()):
      #env.Append ( CCFLAGS = ['/Od', '/MTd', '/Gm', '/RTC1', '/ZI', '/showIncludes'] )
      env.Append ( CCFLAGS = ['/Od', '/MTd', '/Gm', '/RTC1', '/ZI'] )
      env.AddIncludeDir('#3rd/msvs/vld/include')
   else:
      env.Append ( CCFLAGS = ['/Ox', '/FD', '/MT', '/Zi'] )



# Default link flags
if (env['_PLATFORM']=='win32'):
   env.Append ( LINKFLAGS = ['/NOLOGO', '/MACHINE:X86', '/OPT:NOREF', '/INCREMENTAL:NO', '/FIXED:NO',
                             '/NODEFAULTLIB:libc.lib', '/NODEFAULTLIB:libcd.lib',
                             '/NODEFAULTLIB:msvcrt.lib', '/NODEFAULTLIB:msvcrtd.lib',
                             '/NODEFAULTLIB:msvcprt.lib', '/NODEFAULTLIB:msvcprtd.lib'
                             ] )

   if (env.IsCppModeDebug ()):
      env.Append ( LINKFLAGS = ['/NODEFAULTLIB:libcmt.lib'] )
      env.Append ( LINKFLAGS = ['/Profile', '/DEBUG'] )
   else:
      env.Append ( LINKFLAGS = ['/NODEFAULTLIB:libcmtd.lib'] )

if env['_PLATFORM'] == 'posix':
   if env.IsCppModeProfile ():
      env.Append ( LINKFLAGS = ['-pg'] )

if (env['_PLATFORM']=='posix'):
   env.Append ( LIBS = ['dl'] )
if (env['_PLATFORM']=='win32'):
   env.Append ( LIBS = ['advapi32.lib'] )


# multithreading config
if (env['_PLATFORM'] == 'posix'):
   env.Append (CCFLAGS= '-pthread' )
   env.Append (LIBS= ['pthread'] )

if (env['_PLATFORM'] == 'darwin'):
   env.Append (CCFLAGS= '-pthread' )
   env.Append (LIBS= ['pthread'] )

elif (env['_PLATFORM'] == 'win32'):
   env.Append ( LIBS = ['ws2_32.lib'] )


if env['_PLATFORM'] != 'win32':
	env.Append ( LIBPATH = [ env['_REPO_SHLIB_DIR'] ] )
else:
	env.Append ( LIBPATH = [ env['_REPO_STLIB_DIR'] ] )

# for mode in ('debug', 'release', 'profile'):
#    repo_debdir = env.GetRepoDebDir (mode)
#    repo_debdir_pkg = env.File ('Packages.gz', env.GetRepoDebDir (mode))
#    print '****** ', repo_debdir_pkg.abspath
#    debindex_cmd = 'mkdir -p ' + repo_debdir.abspath + ' && cd ' + repo_debdir.abspath + os.sep + '..' + ' && ' + 'dpkg-scanpackages ' + repo_debdir.name + ' 2> /dev/null | gzip > ' + repo_debdir.abspath + os.sep + 'Packages.gz'
#    env.Command (repo_debdir_pkg, None, debindex_cmd)
#    env.AlwaysBuild (repo_debdir_pkg, None, debindex_cmd)
#    env.Default (repo_debdir_pkg)


## THE TARGETS DECLARATION PROCESS ###################################################################################
Export( 'env' )

subdirs = os.walk('.').next()[1]
if 'build' in subdirs:
	subdirs.remove('build')
if '.svn' in subdirs:
	subdirs.remove('.svn')

# Getting the list of all projects (populating env['_PROJECTS'])
print "Reading all projects"
env.Replace(_PROJECTS = dict())
env['_BUILD_STEP'] = 0
for subdir in subdirs:
	env.SubBuild (subdir, 'build' + os.sep + env['_CPPMODE'] + os.sep + subdir)


# Declare dependencies of all projects
# print "Reading all dependencies"
#for project in env.GetRootEnv()['_PROJECTS'].values():
#	project.append_dependencies()

#env.ProcessDependenciesOrder ()

## @todo Include the dot file generation in the doc target
#items = list()
#dps = list()
#for proj in env.GetRootEnv()['_PROJECTS'].values():
#  items.append(proj.get_full_name())
#  for dep in proj.dependencies:
#    prj2 = env.GetProject(dep[0])
#    dps.append([proj.get_full_name(), prj2.get_full_name()])
#dump_dot_file('C:\\forge2\\test.dot', items, dps)



# Declare the targets of all projects
print "Declaring all targets"
env['_BUILD_STEP'] = 1
for subdir in subdirs:
	env.SubBuild (subdir, 'build' + os.sep + env['_CPPMODE'] + os.sep + subdir)

# Debian target dependencies
print "Writing all dependencies"
env['_BUILD_STEP'] = 2
for subdir in subdirs:
	env.SubBuild (subdir, 'build' + os.sep + env['_CPPMODE'] + os.sep + subdir)

# Alias projects groups by type
## @todo auto determinate project types list)
for type in ['lib', 'bin', 'doc', 'vcproj', 'deb', 'php', 'phpproj']:
	env.Alias (type, env.list_all_targets_aliases(type))

# Alias to the main Visual Studio Solution project
## @todo replace it by a standard project class)
allvsprojs = env.Unalias (env.Alias ('phpproj')) + env.Unalias (env.Alias ('vcproj'))
env.Alias ('vcsln', env.MsVsSolution ('forge.gen.sln', allvsprojs))

env.Alias('forge_html_doc', env.ForgeHtmlDoc())
