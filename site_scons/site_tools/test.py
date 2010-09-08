import os
import os.path


def test_impl (target, source, env):
  stoponfailure = env.GetVar ('TESTSTOPONFAILURE', False)

  # Always force test execution
  env.AlwaysBuild (target)

  app = str(source[0].abspath)

  # Exec in the test program dir!
  curdir = os.path.abspath (os.curdir)
  print '=================================================================='
  print '= '+ source[0].abspath
  print '=================================================================='
  print 'Current directory set to '+ os.path.dirname (source[0].abspath)
  os.chdir (os.path.dirname (source[0].abspath))

  if os.system(app)==0:
    #open(str(target[0]),'w').write("PASSED\n")
    os.chdir (curdir)
  else:
    os.chdir (curdir)
    if stoponfailure:
      return 1
    else:
      return 0





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
