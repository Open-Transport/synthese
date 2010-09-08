## @namespace enterprises3 Enterprise to SYNTHESE 3 builder
## @ingroup forge2SconsBuilders
##
## Environment variables :
##
## Targets :
##  - 0 : path of the SYNTHESE 3 dump file (eg : synthese.sql)
##
## Sources : 
##  - None
##
## Remarks :
##  - Enterprise must be installed to use this builder

import os, os.path, fnmatch

from SCons.Defaults import *


## Enterprise to SYNTHESE 3 build implementation.
def enterprises3_impl (target, source, env):

	cmd = 'cd /var/www/enterprise/ && php5 -f cli-assistant.php '+ env['BASE_ID'] +' export && echo BEGIN\; > '+ target[0].abspath +' && cat /srv/tmp/synthese/synthese3.sql >> '+ target[0].abspath +' && echo COMMIT\; >> ' + target[0].abspath

	print cmd
	
	ret = os.system (cmd)
	
	return ret
##


## Add builders and construction variables.
def generate(env):
   
	import SCons.Builder
	enterprises3_builder = SCons.Builder.Builder(
	  action = enterprises3_impl,
	  target_factory = env.fs.Entry,
	)

	env.Append(BUILDERS = {
	  'EnterpriseSYNTHESE3': enterprises3_builder,
	})
##


##
def exists(env):
	return True
