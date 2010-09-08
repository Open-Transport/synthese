import os
import os.path


def bjam_impl (target, source, env):
  # Exec in the test program dir!
  curdir = os.path.abspath (os.curdir)
  os.chdir (os.path.dirname (source[0].abspath))

  options = env['BJAMOPTIONS']
  cmd = 'bjam'
  for opt in options:
	cmd = cmd + ' ' + opt
  print(cmd)
  ret = os.system (cmd)
  os.chdir (curdir)

  return ret
  
  

## Add builders and construction variables.
def generate(env):
   
   import SCons.Builder
   bjam_builder = SCons.Builder.Builder(
      action = bjam_impl,
      target_factory = env.fs.Entry,
      single_source = True
   )

   env.Append(BUILDERS = {
      'BJam': bjam_builder,
   })



def exists(env):
   return env.Detect ('bjam')
