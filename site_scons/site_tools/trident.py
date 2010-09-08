## @namespace s3trident Trident data builder
## @ingroup forge2SconsBuilders


import os
import os.path

from SCons.Defaults import *

def s3trident_impl (target, source, env):

	network = '*'
	if env['NETWORK'] != '':
		network = env['NETWORK']
		
	tisseo = 0
	if env['TISSEO'] != '':
		tisseo = env['TISSEO']

	if os.path.exists(target[0].abspath):
		env.Execute(Delete(target[0].abspath))
	env.Execute (Mkdir(target[0].abspath))
	
	tmpfile = '/srv/tmp/lines.txt'
	
	cmd = 'wget -T 0 -O ' + tmpfile + ' \"http://localhost:8080/synthese3/admin?fonction=LinesListFunction2&ni=' + network + '\" && '
	cmd = cmd + 'for i in `cat ' + tmpfile + '` \ndo\nIFS=\";\"\nset -- $i\n'
	cmd = cmd + 'wget -O '+ target[0].abspath +'/trident'
	if tisseo:
		cmd = cmd + '2'
	cmd = cmd + '_line_$2_$1.xml \"http://localhost:8080/synthese3/admin?fonction=tridentexport&li=$1'
	if tisseo:
		cmd = cmd + '&wt=1'
	cmd = cmd + '\"\n'
	cmd = cmd + 'done'
	
	print cmd
	
	ret = os.system (cmd)
	
	return ret
##



## Add builders and construction variables.
def generate(env):
   
   import SCons.Builder
   s3trident_builder = SCons.Builder.Builder(
      action = s3trident_impl,
      target_factory = env.fs.File,
   )

   env.Append(BUILDERS = {
      'S3Trident': s3trident_builder,
   })
##


##
def exists(env):
   return True

