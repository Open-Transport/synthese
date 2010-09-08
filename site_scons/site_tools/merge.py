import os
import os.path


def merge_impl (target, source, env):
  ft = open (target[0].abspath, 'w')
  for s in source:
    fs = open (s.abspath)
    ft.writelines (fs.readlines ())
    fs.close ()
  ft.close ()
  return 0




##
## Add builders and construction variables.
def generate(env):

   import SCons.Builder
   merge_builder = SCons.Builder.Builder(
      action = merge_impl,
      target_factory = env.fs.File,
   )

   env.Append(BUILDERS = {
      'Merge': merge_builder,
   })


##
def exists(env):
   return True
