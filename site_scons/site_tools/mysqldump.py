## @namespace mysqldump Mysql dump builder
## @ingroup forge2SconsBuilders
##
## Environment variables :
##  - HOST (optional) : host of the mysql database (default = localhost)
##  - USER (optional) : MySQL user (default = root)
##  - PASSWORD (optional) : password of the user (default = empty)
##  - TABLES : tables to export
##
## Targets :
##  - 0 : path of the dump file (eg : dump.sql)
##
## Sources : 
##  - None

import os, os.path, fnmatch

from SCons.Defaults import *


## Mysqldump build implementation.
def mysqldump_impl (target, source, env):

	host = 'localhost'
	if env['HOST'] != '':
		host = env['HOST']
	
	user = 'root'
	if env['USER']:
		user = env['USER']
	
	password = ''
	if env['PASSWORD'] != '':
		password = env['PASSWORD']

	cmd = 'mysqldump --databases --add-drop-database --compress --single-transaction -h '+ host +' -u '+ user

	if password != '':
		cmd = cmd + ' --password=' + password

	for table in env['TABLES']:
		cmd = cmd + ' ' + table

	cmd = cmd + ' > '+ target[0].abspath

	print cmd
	
	ret = os.system (cmd)
	
	return ret
##


## Add builders and construction variables.
def generate(env):
   
	import SCons.Builder
	mysqldump_builder = SCons.Builder.Builder(
	  action = mysqldump_impl,
	  target_factory = env.fs.Entry,
	)

	env.Append(BUILDERS = {
	  'MySQLDump': mysqldump_builder,
	})
##


##
def exists(env):
	return True
