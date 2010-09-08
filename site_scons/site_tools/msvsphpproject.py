## @namespace msvsphpproject Visual Studio PHP project builder
## @ingroup forge2SconsBuilders


import os
import os.path


def msvsphpproject_impl (target, source, env):

  import os.path
  projname = os.path.basename(os.path.dirname(target[0].abspath)) + '/' + os.path.basename (target[0].abspath).replace ('.phpproj', '')

  currentpath = env.Dir ('.').srcnode().abspath;

  projinfos = []

  # [projname, projguid, projrelativepath]
  projinfos.append (projname)
  
  # generate guid for projname (the first part of proj guid is faked)
  # since it is generated from module name , this guid is stable; maybe shouldn't be ?
  projnamehash = abs(hash(currentpath + '/' + projname)) # hope no collision will occur...
  projguid = '2945A10B-2904-426F-990F-%(ph)012X' % {'ph': projnamehash}
  projinfos.append (projguid)
  
  # Glob all project related files
  projectFiles = source

  # Create MS VS 2005 Project file
  mpj = target[0].abspath
  
  moduleprojfile = open (mpj, "w" )
  
  moduleprojfile.write("\"General\"\n")
  moduleprojfile.write("{\n")
  moduleprojfile.write("\"ProjectType\" = \"lccal\"\n")
  moduleprojfile.write("\"AutoLoadTemplate\" = \"%s.php;class.%s.php;\"\n")
  moduleprojfile.write("\"ProjectIdGuid\" = \"{2945A10B-2904-426F-990F-5E685ADB828E}\"\n")
  moduleprojfile.write("\"ProjectVersion\" = \"2.5.1.4789\"\n")
  moduleprojfile.write("}\n")
  moduleprojfile.write("\"Configurations\"\n")
  moduleprojfile.write("{\n")
  for configname in ['Local|PHP 5', 'Staging|PHP 5', 'Production|PHP 5']:
    moduleprojfile.write("    \"" + configname + "\"\n")
    moduleprojfile.write("    {\n")
    moduleprojfile.write("    \"IncludeDirs\" = \"\"\n")
    moduleprojfile.write("    }\n")
  moduleprojfile.write("}\n")
  moduleprojfile.write("\"Files\"\n")
  moduleprojfile.write("{\n")

  for file in eval(projectFiles[0].get_contents()):
    moduleprojfile.write("    \"" + file.replace('\\','\\\\') + "\"\n")
    moduleprojfile.write("    {\n")
    moduleprojfile.write("    \"ProjRelPath\" = \"T\"\n")
    moduleprojfile.write("    }\n")

  moduleprojfile.write("}\n")

  moduleprojfile.write("\"ProjStartupServices\"\n")
  moduleprojfile.write("{\n")
  moduleprojfile.write("}\n")
  moduleprojfile.write("\"Globals\"\n")
  moduleprojfile.write("{\n")
  moduleprojfile.write("}\n")
  moduleprojfile.write("\"References\"\n")
  moduleprojfile.write("{\n")
  moduleprojfile.write("}\n")
  
  moduleprojfile.close ()

  return 0
##



## Add builders and construction variables.
def generate(env):
   
   import SCons.Builder
   msvsphpproject_builder = SCons.Builder.Builder(
      action = msvsphpproject_impl,
      target_factory = env.fs.File,
   )

   env.Append(BUILDERS = {
      'MsVsPHPProject': msvsphpproject_builder,
   })
##


##
def exists(env):
   return True

