## @namespace msvssolution Microsoft Visual Studio Solution file builder.
## @ingroup forge2SconsBuilders
##
##  Targets :
##  - 0 : Microsoft Visual Studio Solution file (eg : forge.sln)
##
## Sources :
##  - All project files (eg project.vcproj)
##

import os
import os.path

from odict import *


## Extract project informations from its relative path.
## @param relpath Relative path of the project
## @return list of informations :
##   - 0 project name
##   - 1 project GUID
##   - 2 project file relative path (in the filesystem)
##   - 3 project is a C++ project
##   - 4 project is a PHP project
##   - 5 project type GUID
##   - 6 project is a real project
##   - 7 project virtual relative path (in the SLN tree) : it correponds to the nearest Sconscript
def get_projinfos_from_relpath (relpath):
    projinfos = list ()
    isvcproj = (relpath.find('.vcproj') > 0)
    isphpproj = (relpath.find('.phpproj') > 0)
    # default guid is folder guid
    guid = "2150E333-8FDC-42A3-9474-1A3956D46DE8"
    projname = os.path.basename (relpath)
    if isvcproj:
      guid = "8BC9CEB8-8B4A-11D0-8D11-00A0C91BC942"
      projname = projname.replace ('.vcproj', '')
    if isphpproj:
      guid = "1863889A-00DF-4350-BA39-669CA949CB81"
      projname = projname.replace ('.phpproj', '')
    #if isvcproj or isphpproj:
    #  dirname = os.path.basename(os.path.dirname(relpath))
    #  if dirname:
    #    projname = dirname + "/" + projname
    virtual_path = relpath
    while virtual_path != '':
      if os.path.exists(virtual_path + os.sep + 'SConscript'):
        break
      virtual_path = os.path.dirname (virtual_path)
    if isvcproj or isphpproj:
	  virtual_path = virtual_path + os.sep + os.path.basename (relpath)
      
    projinfos.append (projname)
    projhash = abs (hash(relpath))
    projguid = '266969BE-1E07-4C1B-ABFC-%(ph)012X' % {'ph': projhash}
    projinfos.append (projguid)
    projinfos.append (relpath)
    projinfos.append(isvcproj)
    projinfos.append(isphpproj)
    projinfos.append(guid)
    projinfos.append(isvcproj or isphpproj)
    projinfos.append(virtual_path)
    return projinfos
##


## Microsoft Visual Studio Solution file builder implementation.
def msvssolution_impl (target, source, env):
  import os.path

  def get_folder_prj (prjs, folderrelpath):
    def create_folder_prj (projname):
      projinfos = get_projinfos_from_relpath (folderrelpath)
      return projinfos
    if prjs.has_key (folderrelpath): return prjs[folderrelpath]
    prj = create_folder_prj (os.path.basename (folderrelpath))
    prjs[folderrelpath.lower()] = prj
    return prj

  # Creation of a list of the projects, registered by a struct containing all their informations
  msprojects = list ()
  for mspf in source:
    msprojects.append (get_projinfos_from_relpath (mspf.path))
  msprojects.sort()
  
  # Creation of the SLN file
  msvssol = open (target[0].abspath, "w")
  msvssol.write ("Microsoft Visual Studio Solution File, Format Version 10.00\n")
  msvssol.write ("# Visual Studio 2008\n")

  # Writing of all projects
  for msproj in msprojects:
    msvssol.write ("Project(\"{" + msproj[5] + "}\") = \"" + msproj[0] + "\", \"" + msproj[2] + "\", \"{" + msproj[1] + "}\"\n")
    msvssol.write ("EndProject\n")

  # Create a map for virtual directories
  allprojects = OrderedDict()
  topdir = env.Dir('.').path
  for msproj in msprojects: 
    msprojpath = env.Entry (msproj[7]).path
    while msprojpath != topdir :
      msprojpath = os.path.dirname (msprojpath)
      if msprojpath == '' : break
      get_folder_prj (allprojects, msprojpath)
  allprojects.sort()

  # Create all solution virtual folders
  orderap = OrderedDict()
  for msproj in allprojects.values():
    orderap[msproj[2]] = msproj
  orderap.sort()
  for msproj in orderap.values ():
     msvssol.write ("Project(\"{" + msproj[5] + "}\") = \"" + msproj[0] + "\", \"" + msproj[0] + "\", \"{" + msproj[1] + "}\"\n")
     sconscriptpath = msproj[2] + "\\SConscript"
     if (os.path.exists(sconscriptpath)):
       msvssol.write("\tProjectSection(SolutionItems) = preProject\n")
       msvssol.write("\t\t" + sconscriptpath + " = " + sconscriptpath + "\n")
       msvssol.write("\tEndProjectSection\n")
     msvssol.write ("EndProject\n")

  for msproj in msprojects: 
    allprojects[msproj[7]] = msproj

  # all should have been stored in global var msprojects
  # [projname, projguid, projrelativepath]
  msvssol.write ("Global\n");
  msvssol.write ("\tGlobalSection(SolutionConfigurationPlatforms) = preSolution\n");
  msvssol.write ("\t\tDebug|Win32 = Debug|Win32\n");
  msvssol.write ("\t\tRelease|Win32 = Release|Win32\n");
  msvssol.write ("\t\tLocal|PHP 5 = Local|PHP 5\n")
  msvssol.write ("\t\tProduction|PHP 5 = Production|PHP 5\n")
  msvssol.write ("\t\tStaging|PHP 5 = Staging|PHP 5\n")
  msvssol.write ("\tEndGlobalSection\n");
  msvssol.write ("\tGlobalSection(ProjectConfigurationPlatforms) = postSolution\n");
  msvssol.write ("\t\tDebug|Win32 = Debug|Win32\n");
  msvssol.write ("\t\tRelease|Win32 = Release|Win32\n");
  msvssol.write ("\tEndGlobalSection\n");
  msvssol.write ("\tGlobalSection(SolutionProperties) = preSolution\n");
  msvssol.write ("\t\tHideSolutionNode = FALSE\n");
  msvssol.write ("\tEndGlobalSection\n");
  msvssol.write ("\tGlobalSection(NestedProjects) = preSolution\n");
  
  # allprojects.sort()
  for msproj in allprojects.values ():
    parentpath = os.path.dirname (msproj[7])
    if allprojects.has_key (parentpath):
      parentproj = allprojects[parentpath]
      msvssol.write ("\t\t{" + msproj[1] + "} = {" + parentproj[1] + "}\n")
  msvssol.write ("\tEndGlobalSection\n");
  msvssol.write ("EndGlobal\n");
  msvssol.close ()

  return 0
##




## Add builders and construction variables.
def generate(env):
   
   import SCons.Builder
   msvssolution_builder = SCons.Builder.Builder(
      action = msvssolution_impl,
      target_factory = env.fs.Entry,
   )

   env.Append(BUILDERS = {
      'MsVsSolution': msvssolution_builder,
   })
##


def exists(env):
   return True
