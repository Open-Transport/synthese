## @namespace deb Debian packages builder.
## @ingroup forge2SconsBuilders
##
## Environment variables :
##  - DEBNAME :
##  - DEBVERSION :
##  - DEBDISPATCHMAP : { pattern : install dir } : Sources matching pattern (fnmatch) will be installed in the corresponding install dir.
##  - DEBDIRPATH : path to DEBIAN directory
##  - DEBDEPENDS : list of couples (name, version) of debian dependencies
##
## Targets :
##  - 0 : the debian package (eg : package.deb)
##  - 1 : the changes file (eg : package.changes)
##
## Sources :
##  - Files to be included in the package
##

import os, os.path, fnmatch

from SCons.Defaults import *


## Debian control files generator.
## The following files are created :
##  - <a href="http://www.debian.org/doc/manuals/maint-guide/ch-dreq.en.html#s-control">DEBIAN/control</a>
##  - <a href="http://www.debian.org/doc/manuals/maint-guide/ch-dreq.en.html#s-changelog">DEBIAN/changelog</a>
##
## @param env SCons environment
## @param debian_dir Path of the DEBIAN directory
## @param package Name of the package
## @param version Version of the package
## @param section
## @param priority <a href="http://www.debian.org/doc/FAQ/ch-pkg_basics.en.html#s-priority">Package priority</a>
## @param architecture 
## @param depends
## @param standardsversion
## @param maintainer
## @param brief
## @param description
def create_deb_files (env, debian_dir,
                      package, version, 
                      section = 'misc', priority = 'extra', 
                      architecture = 'i386', depends = [],
                      standardsversion = '3.6.2',
                      maintainer = 'Hugues Romain <contact@reseaux-conseil.com>',
                      brief = '(RCS)',
                      description = '(RCS)'):
  #print 'creating DEBCONTROL ', control_file_path
  control_file_path = debian_dir + os.sep + 'control'
  control_hdl = open (control_file_path, 'w')
  control_hdl.write ('Source: ' + package + '\n')
  control_hdl.write ('Maintainer: ' + maintainer + '\n')
  control_hdl.write ('Section: ' + section + '\n')
  control_hdl.write ('Priority: ' + priority + '\n')
  control_hdl.write ('Standards-Version: ' + standardsversion + '\n')
  control_hdl.write ('\n')
  control_hdl.write ('Package: ' + package + '\n')
  control_hdl.write ('Architecture: ' + architecture + '\n')
  control_hdl.write ('Section: ' + section + '\n')
  control_hdl.write ('Priority: ' + priority + '\n')
  if len (depends) > 0:
    control_hdl.write ('Depends: ')
    for dep in depends:
      control_hdl.write (dep[0])
      if dep[1] != None: 
        control_hdl.write (' (>= ' + dep[1] + ')')
      if dep != depends[-1]: control_hdl.write (', ')
    control_hdl.write ('\n')
  control_hdl.write ('Description: ' + brief + '\n')
  control_hdl.write (' .\n')
  control_hdl.write (' ' + description + '\n')
  control_hdl.close ()

  changelog_file_path = debian_dir + os.sep + 'changelog'
  chl_hdl = open (changelog_file_path, 'w')
  print version
  chl_hdl.write (package + ' ('+ version + ') ' + env['_DEB_TARGET_DIST'] + '; urgency=low\n')
  chl_hdl.write ('\n')
  chl_hdl.write ('  * fake change log')
  chl_hdl.write ('\n')
  chl_hdl.write (' -- maintainer ' + maintainer + '  ' + env['_BUILD_TIMESTAMP_RFC2822'] + '\n')
  chl_hdl.close ()



#   control_hdl.write ('Package: ' + package + '\n')

#   control_hdl.write ('Version: ' + version + '\n')
#   control_hdl.write ('Section: ' + section + '\n')
#   control_hdl.write ('Architecture: ' + architecture + '\n')
#   if len (depends) > 0:
#     control_hdl.write ('Depends: ')
#     for dep in depends:
#       control_hdl.write (dep[0])
#       if dep[1] != None: 
#         control_hdl.write (' (>= ' + dep[1] + ')')
#       if dep != depends[-1]: control_hdl.write (', ')
#     control_hdl.write ('\n')
#   control_hdl.write ('Installed-Size: ' + installed_size + '\n')
#   control_hdl.write ('Maintainer: ' + maintainer + '\n')
#   control_hdl.write ('Source: ' + package + '\n')
#   control_hdl.write ('Description: ' + brief + '\n')
#   control_hdl.write (' .\n')
#   control_hdl.write (' ' + description + '\n')
#   control_hdl.close ()
##




## Debian builder emitter.
## Targets addings :
##  - the changes files corresponding to the already known deb target
##
## Sources addings :
##  - the DEBIAN directory
def deb_emitter (source, target, env):
  deb_dir_path = env['DEBDIRPATH']

  if os.path.exists(deb_dir_path):
    source.append (deb_dir_path)
  target.append (target[0].abspath.replace('.deb','.changes'))

  return (target, source)
##



## Debian builder implementation.
## @param target
## @param source
## @param env
def deb_impl (target, source, env):
  import os, os.path
  deb_name = env['DEBNAME']
  deb_version = env['DEBVERSION']
  dispatch_map = env['DEBDISPATCHMAP']
  deb_dir_path = env['DEBDIRPATH']
  dependencies = env['DEBDEPENDS']  # list of couples (name, version)
  deb_file = target[0]
  deb_builddir = os.path.dirname (deb_file.abspath)
  deb_tempdir = env.Dir (deb_file.name + '.dir', deb_builddir).abspath
  env.Execute ([Delete (deb_tempdir), Mkdir (deb_tempdir)])
  for install_dir in dispatch_map.values ():
    env.Execute (Mkdir (deb_tempdir + os.sep + install_dir))

  for s in source:
    if isinstance (s, SCons.Node.FS.Base) == False : continue
    if s.abspath.startswith (deb_dir_path):
      dest = s.abspath.replace (deb_dir_path, deb_tempdir, 1)

      env.SmartCopy (dest, s.abspath)
      #if os.path.isdir (s.abspath):
      #  if os.path.exists (dest) == False : 
      #    env.Execute (Mkdir (dest))
      #elif env.Match (s.name, '*'):  # just to exclude default files
      #  env.SmartCopy (dest, s.abspath)
      continue

    if os.path.exists (s.abspath) == False: continue
    filename = os.path.basename (s.abspath)
    for pattern, install_dir in dispatch_map.iteritems ():
      if env.Match (s.name, pattern):
        full_dir = deb_tempdir + os.sep + install_dir
        env.SmartCopy (env.Entry (s.name, full_dir).abspath, s.abspath)

  deb_dir = deb_tempdir + os.sep + 'DEBIAN'
  if os.path.exists (deb_dir) == False:
    env.Execute (Mkdir (deb_dir))

  deb_sdir = deb_builddir + os.sep + 'debian'
  env.Execute ([Delete (deb_sdir), Mkdir (deb_sdir)])

  create_deb_files (env, deb_sdir, deb_name, deb_version, depends = dependencies, architecture = env['DEBARCH'])

  control_file = deb_dir + os.sep + 'control'

  cmd = 'cd %s && dpkg-gencontrol -P%s -O > %s' % (deb_builddir, deb_tempdir, control_file)
  cmd = cmd + ' && fakeroot dpkg-deb -b %s %s' % (deb_tempdir, deb_file.abspath)
  cmd = cmd + ' && dpkg-genchanges -b -u. > %s' % (target[1].abspath)

  print (cmd)
  os.system (cmd)
  
  env.Execute([Delete(deb_tempdir), Delete(deb_sdir)])
  
  return 0
##




## Add builders and construction variables.
def generate(env):
   
   import SCons.Builder
   deb_builder = SCons.Builder.Builder(
      action = deb_impl,
      source_factory= env.fs.Dir,
      target_factory = env.fs.File,
      emitter = deb_emitter,
   )

   env.Append(BUILDERS = {
      'Deb': deb_builder,
   })
##


def exists(env):
   return env.Detect ('fakeroot') and env.Detect ('dpkg-deb')
##
