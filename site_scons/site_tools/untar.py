import os
import os.path




#####################################################
def untar_emitter(target, source, env):
    srce = source[0].abspath
    trgt = target[0].abspath

    # Read the results of a tar -tf
    p = os.popen("tar -tf " + srce)

    files = list ()
    env.Append ( UNTAROUTDIR = target[0].abspath )

    for line in p.readlines():
       line = line[:-1] # remove end of line char
       if line[-1] != '/' :
          files.append (env.fs.Entry (target[0].abspath + os.sep + line))
    # End For

    p.close()

    return (files, source)
#end emitter





def generate(env):
   """
   Add builders and construction variables.
   """
   import SCons.Builder

   untar_builder = env.Builder(
                               target_factory = env.fs.Entry,
                               emitter = untar_emitter,
                               action = env.Action([ ['tar', 'xvzf', '$SOURCE', '-C', '$UNTAROUTDIR'] ])
                               )

   env.Append(BUILDERS = {
      'Untar': untar_builder,
   })



def exists(env):
   """
   """
   return env.Detect ('tar')


