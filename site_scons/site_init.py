## @namespace site_init Library
## @ingroup forge2ProjectsLib
##

from SCons.Script.SConscript import SConsEnvironment
import SCons.Script
from SCons.Node.Alias import Alias 
from SCons.Node.FS import Entry 


from SCons.Defaults import *

from project import *
from msvs import *
from doxygen import *
from cpp import *
from php import *
from synthese import *
from tools import *
from enterprise import *
from teleatlas import *
from mysql import *

import os.path

_dependencies = dict ()
_dep_partial_order = list ()




##
def dump_dot_file (filename, items, partial_order): 
  f = open (filename, 'w')
  f.write ('digraph TEST {\n')
  f.write (' node[shape=box];\n')
  for item in items : f.write ('  "%s"\n' % item)
  for fv,tov in partial_order:
    f.write ('  "%s" -> "%s"\n' % (fv,tov))
  f.write ('}\n')
  f.close ()
##



SCons.Script._SConscript.BuildDefaultGlobals ()

##
def Glob( env, includes = ['*'], excludes = [], dir = '.', exclude_gen = True, exclude_dir = True, abs_path = False, recursive = False , prefix=''):
  #Allows to glob files from the build directory going
  #through scons nodes
  import os, os.path, fnmatch
  files = []
  # Careful : default parameter values are evaluated once !!
  excl = list ()
  excl.extend (excludes)
  if exclude_gen == True : 
    excl.append( '*.gen.cpp' )
    excl.append( '*.inc.cpp' )
  excl.append ('.svn')

  incpatterns = includes
  if incpatterns.__class__.__name__ == 'str' : incpatterns = [incpatterns]

  if not os.path.exists(dir):
    return files
  
  sdir = env.Dir (dir).srcnode().abspath
  for file in os.listdir( sdir ):
    f = os.path.join (sdir, file)
    for pattern in incpatterns:
      if fnmatch.fnmatch(file, pattern) :
        excluded = False
        for xpattern in excl :
          if fnmatch.fnmatch(file, xpattern) :
            excluded = True
            break
        if excluded == False :
          if (exclude_dir == False) or (os.path.isdir (f) == False):
            #files.append (env.Entry (file, sdir).abspath)  # why abspath was used once ?
            if abs_path :
              files.append (env.Entry (file, sdir).abspath)
            else:
              files.append (prefix + env.Entry (file, sdir).name)
        break
    
    if os.path.isdir(f) and file != '.svn' and recursive:
      excluded = False
      for xpattern in excl :
        if fnmatch.fnmatch(file, xpattern) :
          excluded = True
          break
      
      if not excluded:
        subglob = env.Glob(includes, excludes, f, exclude_gen, exclude_dir, abs_path, recursive)
        for e in subglob:
          if abs_path:
            files.append(e)
          else:
            files.append(prefix + file + '/' + e)

  return files



##
def Match (env, file, include_patterns = [], exclude_patterns = ['*~', '*.old', '*.bak']):
  import fnmatch
  f = file
  ipatterns = include_patterns
  epatterns = exclude_patterns
  # apply include patterns and if still matches, apply exclude patterns
  if isinstance (ipatterns, str): ipatterns = [ipatterns]
  if isinstance (epatterns, str): epatterns = [epatterns]
  if isinstance (f, Entry): f = f.abspath
  for ip in ipatterns : 
    if fnmatch.fnmatch(f, ip) == False : return False
  for ep in epatterns : 
    if fnmatch.fnmatch(f, ep) == True : return False
  return True


##
def LastLocallyModified (env, dir = '.'):
  import os, os.path
  def last_locally_modified (f):
    if os.path.isdir (f) == False: 
      return os.path.getmtime (f)
    else:
      max = os.path.getmtime (f)
      for e in os.listdir (f):
        etime = os.path.getmtime (os.path.join (f,e))
        if etime > max :
          max = etime
      return max
  return last_locally_modified (env.Dir (dir).srcnode ().abspath)




##
def GetRepoDebDir (env, mode = 'release'):
  return env.Dir ('#build' + os.sep + mode + os.sep + 'repo' + '/deb' + os.sep + 'unstable')
  

##
def GetVar (env, varname, default = None):
    edict = env.Dictionary ()
    if edict.has_key (varname):
        return edict[varname]
    else:
        return default
    

##
def SmartCopy (env, dest, src):
  import os, os.path
  # custom version of copy with no error if dest dir already exists
  if os.path.basename (src) == '.svn' and os.path.isdir (src): return
  if os.path.isdir (src):
    if os.path.exists (dest) == False:
      env.Execute (Mkdir (dest))
    for f in os.listdir (src):
      env.SmartCopy (os.path.join (dest,f), os.path.join (src,f))
  else:
    env.Execute (Copy (dest, src))
    
    


##
def Find (env, pattern, dir = None, exclude_dir = False):
  import os, os.path, fnmatch
  def find_in_path (pattern, dir, result):
    for f in os.listdir (dir):
      if f == '.svn' : continue
      fp = os.path.join (dir, f)
      if fnmatch.fnmatch(f, pattern) :
        if os.path.isdir (fp): 
          if exclude_dir == False:
            result.append (env.Dir (fp))
        else:
          result.append (env.File (fp))
      if os.path.isdir (fp):
        find_in_path (pattern, fp, result)
  path = dir
  if path == None : 
    path = env.Dir ('.').abspath
  res = list ()
  find_in_path (pattern, path, res)
  return res
    

##
def GetLocallyInstalledDebVersion (env, debname):
  dpkgret = os.popen ('dpkg --list ' + debname + ' | tail -n 1').readline()
  if len (dpkgret) < 2: 
    return None
  if dpkgret[0:2] != 'ii': 
    return None
  return dpkgret.split()[2]
  


##
def SubBuild (env, subsrcdir = None, subbuilddir = None):
    import os.path
    # all subdirs where a SConscript is found
    if subsrcdir == None and subbuilddir == None:

      builddir = env.Dir('.').abspath
      srcpath = env.Dir('.').srcnode().abspath
      #print 'buildir = ', builddir, srcpath
      dirs = os.listdir( srcpath )
      subdirs = []
      # Build the list of subdirs to be processed
      for subdir in dirs:
        if not os.path.isdir (os.path.join (srcpath, subdir)): continue
        if not os.path.exists (os.path.join (srcpath, subdir, 'SConscript')): continue
        subdirs.append (subdir)

      for subdir in subdirs:
        sx = os.path.join (srcpath, subdir)
        s0 = os.path.join (srcpath, subdir, 'SConscript')
        s1 = os.path.join (builddir, subdir)
        env.SConscript (s0, build_dir=s1, duplicate = False, exports=['env'])
      
    else:
      s0 = os.path.join ( env.Dir (subsrcdir).srcnode ().abspath, 'SConscript')
      s1 = env.Dir (subbuilddir).abspath
      env.SConscript (s0,
                      build_dir = s1,
                      duplicate = False,
                      exports=['env'])


## Source absolute path corresponding to a target build directory path.
## @param env SCons environment
## @param path target build directory path
## @return corresponding source absolute directory path
## Example : build/dev/synthese3/src => C:/forge2/dev/synthese3/src
def SrcDir(env, path):
	return env.Dir (path).srcnode().abspath
	
	
## Source absolute path corresponding to a target build file path.
## @param env SCons environment
## @param path target build directory path
## @return corresponding source absolute file path
## Example : build/dev/synthese3/src => C:/forge2/dev/synthese3/src
def SrcFile(env, path):
	return env.File(path).srcnode().abspath


## Debug or release mode of the environment.
## @return 'debug' or 'release' or 'profile' (??)
def GetCppMode (env):
    return env['_CPPMODE']


## Indicates if the build mode is debug.
def IsCppModeDebug (env):
    return GetCppMode (env) == 'debug' or GetCppMode (env) == 'profile'


## Indicates if the build mode is release.
def IsCppModeRelease (env):
    return GetCppMode (env) == 'release'


## Indicates if the build mode is profile (??).
def IsCppModeProfile (env):
    return GetCppMode (env) == 'profile'


## Indicates the platform target.
## @return 'win32' or 'posix'
def GetPlatform (env):
    return env['_PLATFORM']


def GetCppToolset (env):
    return env['_CPPTOOLSET']


def CppBuildDirSuffix (env):
    import os
    return env.GetPlatform () + os.sep + env.GetCppToolset() + os.sep + env.GetCppMode ()



def AddIncludeDir (env, dir):
	env.Append (CPPPATH = [ dir ] )


def GetRootEnv (env):
  return env['_ROOTENV']

def GetProject(env, name):
  if name not in env.GetRootEnv()['_PROJECTS'].keys():
    return None
  return env.GetRootEnv()['_PROJECTS'][name]

def Unalias (env, alias):
  def unalias (alias, result):
    children = alias
    if isinstance (alias, Alias):
      children = alias.children ()
    for ca in children:
      if isinstance (ca, Alias):
        unalias (ca, result)
      else:
        result.append (ca)
  res = list ()
  unalias (alias, res)
  return res



# alias definitions, top level targets
def list_all_targets_aliases (env, type):
    projects = env['_PROJECTS']
    result = list ()
    for (a,p) in projects.items():
      if os.path.splitext (a)[1][1:] == type:
          result.append (a)
    return result

  
  





# Environment extension methods
SConsEnvironment.GetVar=GetVar
SConsEnvironment.Glob=Glob
SConsEnvironment.Find=Find
SConsEnvironment.SmartCopy=SmartCopy
SConsEnvironment.GetLocallyInstalledDebVersion=GetLocallyInstalledDebVersion
SConsEnvironment.Match=Match
SConsEnvironment.LastLocallyModified=LastLocallyModified

SConsEnvironment.SubBuild=SubBuild
SConsEnvironment.SrcDir=SrcDir
SConsEnvironment.SrcFile=SrcFile
SConsEnvironment.Unalias=Unalias

SConsEnvironment.GetCppMode=GetCppMode
SConsEnvironment.IsCppModeDebug=IsCppModeDebug
SConsEnvironment.IsCppModeRelease=IsCppModeRelease
SConsEnvironment.IsCppModeProfile=IsCppModeProfile
SConsEnvironment.GetPlatform=GetPlatform
SConsEnvironment.GetCppToolset=GetCppToolset
SConsEnvironment.GetRepoDebDir=GetRepoDebDir

SConsEnvironment.CppBuildDirSuffix=CppBuildDirSuffix


SConsEnvironment.AddIncludeDir=AddIncludeDir
SConsEnvironment.GetRootEnv=GetRootEnv

SConsEnvironment.list_all_targets_aliases=list_all_targets_aliases
SConsEnvironment.GetProject=GetProject


# Little hack to allow invocation of some methods at global scope in SConscripts
SCons.Script._SConscript.GlobalDict['Project'] = Project
SCons.Script._SConscript.GlobalDict['CppProject'] = CppProject
SCons.Script._SConscript.GlobalDict['MySQLDataProject'] = MySQLDataProject
SCons.Script._SConscript.GlobalDict['CppLibProject'] = CppLibProject
SCons.Script._SConscript.GlobalDict['CppBinProject'] = CppBinProject
SCons.Script._SConscript.GlobalDict['CppSynthese3BinProject'] = CppSynthese3BinProject
SCons.Script._SConscript.GlobalDict['CppSynthese3LibProject'] = CppSynthese3LibProject
SCons.Script._SConscript.GlobalDict['CppTestProject'] = CppTestProject
SCons.Script._SConscript.GlobalDict['CppIncludeProject'] = CppIncludeProject
SCons.Script._SConscript.GlobalDict['PhpProject'] = PhpProject
SCons.Script._SConscript.GlobalDict['DoxProject'] = DoxProject
SCons.Script._SConscript.GlobalDict['S3TridentProject'] = S3TridentProject
SCons.Script._SConscript.GlobalDict['S3Trident2ProvisoireTisseoProject'] = S3Trident2ProvisoireTisseoProject
SCons.Script._SConscript.GlobalDict['PrjVersion'] = PrjVersion
SCons.Script._SConscript.GlobalDict['DebPrereq'] = DebPrereq
SCons.Script._SConscript.GlobalDict['VCProjVisualStudioProject'] = VCProjVisualStudioProject
SCons.Script._SConscript.GlobalDict['DebianPaquetProject'] = DebianPaquetProject
SCons.Script._SConscript.GlobalDict['EnterpriseS3DataProject'] = EnterpriseS3DataProject
SCons.Script._SConscript.GlobalDict['EnterpriseS2DataProject'] = EnterpriseS2DataProject
SCons.Script._SConscript.GlobalDict['TeleAtlasS3DataProject'] = TeleAtlasS3DataProject
SCons.Script._SConscript.GlobalDict['dump_dot_file'] = dump_dot_file

