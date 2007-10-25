import os
import os.path


def test_impl (target, source, env):
  # Always force test execution
  env.AlwaysBuild (target)

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




def generate(env):
   """
   Add builders and construction variables.
   """

   import SCons.Builder
   test_builder = SCons.Builder.Builder(
      action = test_impl,
      target_factory = env.fs.Entry,
      single_source = True
   )

   env.Append(BUILDERS = {
      'Test': test_builder,
   })



def exists(env):
   """
   """
   return True
