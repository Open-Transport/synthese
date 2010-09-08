## @namespace msvsproject Visual Studio C++ Project builder
## @ingroup forge2SconsBuilders
##
## Environment variables :
##  - _FILES_ROOT_DIR : Root of the tree of the project


import os
import os.path


## Relative path extraction.
## @param target directory path to rewrite
## @param base can be a directory specified either as absolute or relative to current dir.
## @return a relative path to the target from either the current dir or an optional base dir.
def relpath(target, base=os.curdir):

    if target == base:
        return ''

    if not os.path.exists(target):
        raise OSError, 'Target does not exist: '+target

    if not os.path.isdir(base):
        raise OSError, 'Base is not a directory or does not exist: '+base

    base_list = (os.path.abspath(base)).split(os.sep)
    target_list = (os.path.abspath(target)).split(os.sep)

    # On the windows platform the target may be on a completely different drive from the base.
    if os.name in ['nt','dos','os2'] and base_list[0] <> target_list[0]:
        raise OSError, 'Target is on a different drive to base. Target: '+target_list[0].upper()+', base: '+base_list[0].upper()

    # Starting from the filepath root, work out how much of the filepath is
    # shared by base and target.
    for i in range(min(len(base_list), len(target_list))):
        if base_list[i] <> target_list[i]: break
    else:
        # If we broke out of the loop, i is pointing to the first differing path elements.
        # If we didn't break out of the loop, i is pointing to identical path elements.
        # Increment i so that in all cases it points to the first differing path elements.
        i+=1

    rel_list = [os.pardir] * (len(base_list)-i) + target_list[i:]
    return os.path.join(*rel_list)
##


def get_first_subdir(path):
  if os.path.dirname(path) == '':
    return ''
  while os.path.dirname(path) != '':
    path = os.path.dirname(path)
  return path


def print_virtual_folder(moduleprojfile,folder_name,patterns,files,done):
	import fnmatch
	moduleprojfile.write("<Filter Name=\"" + folder_name + "\">\n")
	for mf in files:
		for pattern in patterns:
			if fnmatch.fnmatch(mf,pattern) :
				moduleprojfile.write ("<File RelativePath=\"" + mf + "\"/>\n")
				done.append(mf)
	moduleprojfile.write("</Filter>\n")
	

## Recursive output of file lists.
def print_files_list(moduleprojfile,files,projabspath, done,path = ''):

  # Output list of files
  non_empty_path = (path != '')
  if non_empty_path:
    moduleprojfile.write('<Filter Name="' + os.path.basename(path) + '">')
  else:
    path = projabspath
  for mf in files:
    if mf not in done and os.path.dirname(mf) == path:
      moduleprojfile.write ("<File RelativePath=\"" + mf + "\"/>\n")
    
  # Recursion on each subfolder
  sf = []
  for mf in files:
    if mf not in done and mf.find(path) == 0:
      strmf = mf.replace(path,'',1)
      if path != '':
        if strmf.find('/') == 0: strmf = strmf.replace('/','',1)
        if strmf.find('\\') == 0: strmf = strmf.replace('\\','',1)
      fs = get_first_subdir(strmf)
      if fs != '' and fs not in sf:
        sf.append(fs)
  if path != '':
    path += os.sep
  for fs in sf:
    print_files_list(moduleprojfile, files, projabspath, done, path + fs)

  if non_empty_path:
    moduleprojfile.write("</Filter>\n")



## Implementation of the VCProj builder
## The sources are described by their relative path.
## The target is described by its absolute path
def msvsproject_impl (target, source, env):

  projoutalias = target[0].abspath
  projname = os.path.basename (target[0].abspath).replace ('.vcproj', '')

  currentpath = env.Dir ('.').srcnode().abspath;

  projinfos = []

  # [projname, projguid, projrelativepath]
  projinfos.append (projname)
  
  # generate guid for projname (the first part of proj guid is faked)
  # since it is generated from module name , this guid is stable; maybe shouldn't be ?
  projnamehash = abs(hash(currentpath + '/' + projname)) # hope no collision will occur...
  projguid = '266969BE-1E07-4C1B-ABFC-%(ph)012X' % {'ph': projnamehash}
  projinfos.append (projguid)
  
  # Glob all project related files
  projectFiles = source

  # Create MS VS 2008 Project file
  mpj = target[0].abspath
  
  moduleprojfile = open (mpj, "w" )
  moduleprojfile.write ("<?xml version=\"1.0\" encoding=\"Windows-1252\"?>\n")

  moduleprojfile.write ("<VisualStudioProject ProjectType=\"Visual C++\" Version=\"9,00\" Name=\"" + projname + "\" ProjectGUID=\"{266969BE-1E07-4C1B-ABFC-5193858D0B4B}\" Keyword=\"MakeFileProj\">\n")

  moduleprojfile.write ("<Platforms> <Platform Name=\"Win32\"/> </Platforms>\n");

  moduleprojfile.write ("<ToolFiles></ToolFiles>\n");

  moduleprojfile.write ("<Configurations>\n");


  for configname in ['Debug', 'Release']:
    cppmode = configname.lower ()
    projoutputdir = os.sep + cppmode + os.sep + os.path.dirname (target[0].path) 
    projoutputfile = projname
    if projname[-7:] == '_cpplib' : 
      projoutputfile = projoutputfile[:-7] + '.lib'
      projoutalias = projname[:-7] + '.cpplib' # only stlib windows
    if projname[-7:] == '_cppbin' : 
      projoutputfile = projoutputfile[:-7] + '.exe'
      projoutalias = projname[:-7] + '.cppbin' 
    if projname[-8:] == '_cpptest' : 
      projoutputfile = projoutputfile[:-8] + '.exe'
      projoutalias = projname[:-8] + '.cpptest' 
    if projname[-4:] == '_doc' : 
      projoutalias = projname[:-4] + '.doc' # only stlib windows
    projoutalias = projoutalias.replace('_','-')
    
    moduleprojfile.write ("<Configuration Name=\"" + configname + "|Win32\" OutputDirectory=\"$(SolutionDir)/" + 'build' + projoutputdir + "\" IntermediateDirectory=\"$(SolutionDir)/" + 'build' + projoutputdir + "\" ConfigurationType=\"0\" InheritedPropertySheets=\"$(VCInstallDir)VCProjectDefaults\UpgradeFromVC71.vsprops\">\n")
    moduleprojfile.write ("<Tool Name=\"VCNMakeTool\" BuildCommandLine=\"scons -u _CPPMODE=" + cppmode + " " + projoutalias + "\" ReBuildCommandLine=\"scons -uc _CPPMODE=" + cppmode + " " + projoutalias +  " ; " + "scons -u _CPPMODE=" + cppmode + " " + projoutalias + "\" CleanCommandLine=\"scons -uc _CPPMODE=" + cppmode + " " + projoutalias + "\" Output=\"$(SolutionDir)/" + 'build' + projoutputdir + os.sep + projoutputfile + "\" PreprocessorDefinitions=\"\" IncludeSearchPath=\"")
    if '_INCLUDE_LIST' in env.Dictionary().keys():
        moduleprojfile.write (env['_INCLUDE_LIST'])
    moduleprojfile.write ("\" ForcedIncludes=\"\" AssemblySearchPath=\"\" ForcedUsingAssemblies=\"\" CompileAsManaged=\"\" />\n")
    moduleprojfile.write ("</Configuration>\n")

  moduleprojfile.write ("</Configurations>\n");

  moduleprojfile.write ("<References></References>\n");

  moduleprojfile.write ("<Files>\n");

  done = list()
  for filter in env['_SORTS']:
    print_virtual_folder(moduleprojfile,filter[0],filter[1],eval(projectFiles[0].get_contents()), done)

  if os.path.exists(env['_FILES_ROOT_DIR']):
    print_files_list(moduleprojfile, eval(projectFiles[0].get_contents()), relpath(env['_FILES_ROOT_DIR'], os.path.dirname(mpj)), done)
  
  moduleprojfile.write ("</Files>\n");

  moduleprojfile.write ("<Globals></Globals>\n");

  moduleprojfile.write ("</VisualStudioProject>\n");
  
  moduleprojfile.close ()

  if projname[-7:] == '_cppbin' : 
    userfilename = target[0].abspath + '.' + os.environ['COMPUTERNAME'] + '.' + os.environ['USERNAME'] + '.user'
    userfile = open (userfilename, "w" )
    userfile.write("<?xml version=\"1.0\" encoding=\"Windows-1252\"?>\n")
    userfile.write("<VisualStudioUserFile ProjectType=\"Visual C++\" Version=\"9,00\" ShowAllFiles=\"false\">\n")
    userfile.write("<Configurations>\n")
    for configname in ['Debug', 'Release']:
      cppmode = configname.lower ()
      projoutputdir = os.sep + cppmode + os.sep + os.path.dirname (target[0].path) 
      userfile.write("<Configuration Name=\""+ configname +"|Win32\">\n")
      userfile.write("<DebugSettings Command=\"$(TargetPath)\" WorkingDirectory=\"$(SolutionDir)/" + 'build' + projoutputdir + "\" />\n")
      userfile.write("</Configuration>\n")
    userfile.write("</Configurations>\n")
    userfile.write("</VisualStudioUserFile>\n")
    userfile.close()

  return 0
##



## Add builders and construction variables.
def generate(env):
   
   import SCons.Builder
   msvsproject_builder = SCons.Builder.Builder(
      action = msvsproject_impl,
      target_factory = env.fs.File,
   )

   env.Append(BUILDERS = {
      'MsVsProject': msvsproject_builder,
   })
##


##
def exists(env):
   return True

