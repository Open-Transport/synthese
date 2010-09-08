import os
import os.path




#####################################################
def unzip_emitter(target, source, env):
    import re
    srce = source[0].abspath
    trgt = target[0].abspath

    # Read the results of a tar -tf
    p = os.popen("unzip -Z1 " + srce)

    files = list ()
    env.Append ( UNZIPOUTDIR = target[0].abspath )

    emitterfilter = env.GetVar ('UNZIPEMITTERFILTER')
    regex = None
    if emitterfilter != None:
        regex = re.compile (emitterfilter)

    for line in p.readlines():
       line = line[:-1] # remove end of line char
       if line[-1] != '/' :
           if regex != None:
               #print 'matching ?  ' + line
               if regex.match (line):
                   files.append (env.fs.Entry (target[0].abspath + os.sep + line))
                   print 'matched  ' + line
           else:
               #if line.find ('Jamfile') != -1: 
               #print line
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

   unzip_builder = env.Builder(
                               target_factory = env.fs.Entry,
                               emitter = unzip_emitter,
                               action = env.Action([ ['unzip', '-o', '$SOURCE', '-d', '$UNZIPOUTDIR'] ]),
                               single_source = True
                               )

   env.Append(BUILDERS = {
      'Unzip': unzip_builder,
   })



def exists(env):
   """
   """
   return env.Detect ('unzip')


