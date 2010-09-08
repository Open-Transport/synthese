import os
import os.path


def makedef_impl (target, source, env):
  import re, sys, os, os.path, string
  app = str(source[0].abspath)

  binname = env['MAKEDEFLIBNAME']
  version = env['MAKEDEFLIBVERSION']
  #symbolpattern = env.GetVar ('MAKEDEFSYMBOLPATTERN', '(\?[^?][^ ]*)')
  symbolpattern = env.GetVar ('MAKEDEFSYMBOLPATTERN')
 
  deffile = target[0].abspath
  libfile = source[0].abspath

  # unset VS_UNICODE_OUTPUT to prevent MSVS to catch dumpbin output
  cmd = "SET VS_UNICODE_OUTPUT= && DUMPBIN.EXE /SYMBOLS %s" % libfile
  dumped = os.popen (cmd)

  definitions = {}

  lineres = list ()
  
  if symbolpattern != None:
    lineres.append (re.compile(r"^[^ ]+ +[^ ]+ +SECT[^ ]+ +[^ ]+ +\(\) +External +\| +" + symbolpattern + "(.*)\n"))
    lineres.append (re.compile(r"^[^ ]+ +[^ ]+ +SECT[^ ]+ +[^ ]+ +External +\| +" + symbolpattern + "(.*)\n"))
  else:
    lineres.append (re.compile(r"^[^ ]+ +[^ ]+ +SECT[^ ]+ +[^ ]+ +\(\) +External +\| +(\?[^ ]*)(.*)\n"))
    lineres.append (re.compile(r"^[^ ]+ +[^ ]+ +SECT[^ ]+ +[^ ]+ +External +\| +(\?[^?][^ ]*)(.*)\n"))
  
  
  #linere1 = re.compile(r"^[^ ]+ +[^ ]+ +SECT[^ ]+ +[^ ]+ +\(\) +External +\| +" + symbolpattern + "(.*)\n")
  #linere2 = re.compile(r"^[^ ]+ +[^ ]+ +SECT[^ ]+ +[^ ]+ +External +\| +" + symbolpattern + "(.*)\n")

  exclude = re.compile(r"deleting destructor[^(]+\(unsigned int\)|anonymous namespace")

  while 1:
	line = dumped.readline()
	if line == "":
	    break

	#mymatch = lre.search (line)    
	#if mymatch:
	#  print mymatch.group (0)
	match = False
	for linere in lineres:
	  match = match or linere.search (line)
	  if match : break
	
	#match = linere1.search(line) or linere2.search(line)
	if match:
	    symbol = match.group(1)
	    args   = match.group(2)
	    if exclude.search(args):
		continue

	    definitions[symbol] = None

  symbols = definitions.keys()
  symbols.sort()

  print "Output %d symbols." % len(symbols)

  out = open(deffile, "w")
  #if string.lower(binname[4:]) == ".exe":
  #      out.write("NAME %s\n" % binname)
  #else:
  #      out.write("LIBRARY %s\n" % binname)
  #out.write("VERSION %s\n" % version)
  out.write("EXPORTS\n")

  for s in symbols:
    out.write(s + "\n")

  out.close()
  return 0


  




def generate(env):
   """
   Add builders and construction variables.
   """

   import SCons.Builder
   makedef_builder = SCons.Builder.Builder(
      action = makedef_impl,
      target_factory = env.fs.File,
      single_source = True
   )

   env.Append(BUILDERS = {
      'MakeDef': makedef_builder,
   })



def exists(env):
   """
   """
   return env.Detect("dumpbin")

