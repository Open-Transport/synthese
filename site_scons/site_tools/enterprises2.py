## @namespace enterprises2 Enterprise to SYNTHESE 2 builder
## @ingroup forge2SconsBuilders
##
## Environment variables :
##
## Targets :
##  - 0 : path of the SYNTHESE 2 data directory (eg : environnements)
##  - 1 : path of the SYNTHESE 2 data index (eg : environnements.per)
##
## Sources : 
##  - None
##
## Remarks :
##  - Enterprise must be installed to use this builder

import os, os.path, fnmatch

from SCons.Defaults import *


## Enterprise to SYNTHESE 2 build implementation.
def enterprises2_impl (target, source, env):

	cmd = 'cd /var/www/enterprise/ && php5 -f cli-assistant.php '+ env['BASE_ID'] +' export && cp -r /srv/tmp/synthese/environnements/* ' + target[0].abspath +' && cp /srv/tmp/synthese/environnements.per '+ target[1].abspath

	print cmd
	
	ret = os.system (cmd)
	
	return ret
##


## Add builders and construction variables.
def generate(env):
   
	import SCons.Builder
	enterprises2_builder = SCons.Builder.Builder(
	  action = enterprises2_impl,
	  target_factory = env.fs.Entry,
	)

	env.Append(BUILDERS = {
	  'EnterpriseSYNTHESE2': enterprises2_builder,
	})
##


##
def exists(env):
	return True
