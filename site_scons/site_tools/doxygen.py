## @namespace doxygen doxygen builder
## @ingroup forge2SconsBuilders
##
## Environment variables :
##  - 
##
## Targets :
##  - 0 : 
##
## Sources : 
##  - None

import os
import os.path
import glob
from fnmatch import fnmatch


## Parse a Doxygen source file and return a dictionary of all the values.
#  Values will be strings and lists of strings.
def DoxyfileParse(file_contents):
   data = {}

   import shlex
   lex = shlex.shlex(instream = file_contents, posix = True)
   lex.wordchars += "*+./-:"
   lex.whitespace = lex.whitespace.replace("\n", "")
   lex.escape = ""

   lineno = lex.lineno
   token = lex.get_token()
   key = token   # the first token should be a key
   last_token = ""
   key_token = False
   next_key = False
   new_data = True

   def append_data(data, key, new_data, token):
      if new_data or len(data[key]) == 0:
         data[key].append(token)
      else:
         data[key][-1] += token

   while token:
      if token in ['\n']:
         if last_token not in ['\\']:
            key_token = True
      elif token in ['\\']:
         pass
      elif key_token:
         key = token
         key_token = False
      else:
         if token == "+=":
            if not data.has_key(key):
               data[key] = list()
         elif token == "=":
            if key == "TAGFILES" and data.has_key(key):
               append_data( data, key, False, "=" )
               new_data=False
            else:
               data[key] = list()
         else:
            append_data( data, key, new_data, token )
            new_data = True

      last_token = token
      token = lex.get_token()

      if last_token == '\\' and token != '\n':
         new_data = False
         append_data( data, key, new_data, '\\' )

   # compress lists of len 1 into single strings
   for (k, v) in data.items():
      if len(v) == 0:
         data.pop(k)

      # items in the following list will be kept as lists and not converted to strings
      if k in ["INPUT", "FILE_PATTERNS", "EXCLUDE_PATTERNS", "TAGFILES", "PREDEFINED"]:
         continue

      if len(v) == 1:
         data[k] = v[0]

   return data
##


## Doxygen Doxyfile emitter.
def DoxyEmitter(target, source, env):

   # possible output formats and their default values and output locations
   output_formats = {
      "HTML": ("YES", "html"),
      "LATEX": ("YES", "latex"),
      "RTF": ("NO", "rtf"),
      "MAN": ("YES", "man"),
      "XML": ("NO", "xml"),
   }

   data = DoxyfileParse(source[0].get_contents())
   
   # Add default output directory if it does not exist
   out_dir = env.Dir('.').abspath
   data['OUTPUT_DIRECTORY'] = out_dir
   targets = [out_dir + '/Doxyfile.doxyfile']

   # Add default excludes if it does not exist
   exc_pat = data.get ("EXCLUDE_PATTERNS", '*/.svn/*  */*.gen.cpp */*.inc.cpp')
   data['EXCLUDE_PATTERNS'] = exc_pat

   # Add default input encoding 
   enc = data.get ("INPUT_ENCODING", 'ISO-8859-15')
   data['INPUT_ENCODING'] = enc

   env['DOXYGENPARSEDDATA'] = data

   if not os.path.isabs(out_dir):
      conf_dir = os.path.dirname(str(source[0]))
      out_dir = os.path.join(conf_dir, out_dir)

   # add our output locations
   #for (k, v) in output_formats.items():
   #   if data.get("GENERATE_" + k, v[0]) == "YES":
   #      targets.append(env.Dir( os.path.join(out_dir, data.get(k + "_OUTPUT", v[1]))) )
   
   # add the tag file if neccessary:
   tagfile = data.get("GENERATE_TAGFILE", "")
   if tagfile != "":
      if not os.path.isabs(tagfile):
         conf_dir = os.path.dirname(str(source[0]))
         tagfile = os.path.join(conf_dir, tagfile)
      targets.append(env.File(tagfile))

   # don't clobber targets
   #or node in targets:
   #  env.Precious(node)

   # set up cleaning stuff
   for node in source:
      env.AlwaysBuild (node)
      env.Clean(node, node)
   
   return targets, source
##


## Builder action.
## @param target Target file
## @param source Source files
## @param env SCons Environment
## Do the following actions :
##  - if exists a include folder in source directory, copy its content into target directory
##  - runs doxygen
##  - make PDF version of documentation
def ExecuteDoxygen (target, source, env):
   import os, os.path
   data = env['DOXYGENPARSEDDATA']
   out_dir = data['OUTPUT_DIRECTORY']

   # Generate another doxyfile, overiding output dir if not specified
   if os.path.exists (out_dir) == False :
      os.makedirs (out_dir)
      
   # Copy of include dir
   if data['GENERATE_HTML'] == 'YES' and os.path.exists(str(source[0].dir) + os.sep + 'include'):
      env.SmartCopy(out_dir + os.sep + 'html' + os.sep + 'include', str(source[0].dir) + os.sep + 'include')
   	  
   # Run Doxygen
   doxyfile_gen = out_dir + os.sep + source[0].name + '.gen'
   doxyfile_copy = open (doxyfile_gen, 'w')
   for k,v in data.iteritems ():
      doxyfile_copy.write (str (k))
      doxyfile_copy.write ('\t=\t')
      if v.__class__.__name__ == 'list':
         for i in v:
            doxyfile_copy.write ('"' + str (i) + '" ')
      else:
         doxyfile_copy.write (str (v))
      doxyfile_copy.write ('\n')
   doxyfile_copy.write("ALIASES += htmlinclink{1}=\"@htmlonly <a class=\"el\" href=\\\"includes/\\1\\\">\\1</a>@endhtmlonly\"\n")
   doxyfile_copy.write("ALIASES += htmlinclink{2}=\"@htmlonly <a class=\"el\" href=\\\"includes/\\1\\\">\\2</a>@endhtmlonly\"\n")
   
   doxyfile_copy.close ()
   cmd = 'cd %s && %s %s' % (source[0].dir, env['DOXYGEN'], doxyfile_gen)
   print cmd
   ret = os.system (cmd)
   if ret:
     return ret
   
   # PDF generation
   if data['GENERATE_LATEX'] == "YES":
     cmd = 'cd '+ out_dir + os.sep + 'latex && pdflatex refman.tex && makeindex refman.idx && pdflatex refman.tex && pdflatex refman.tex'
     print cmd
     ret = os.system (cmd)
   return ret
##



## Add builders and construction variables for the Doxygen tool.
# This is currently for Doxygen 1.4.6.
def generate(env):

   import SCons.Builder
   doxyfile_builder = SCons.Builder.Builder(
      action = env.Action (ExecuteDoxygen),
      emitter = DoxyEmitter,
      target_factory = env.fs.Entry,
#      single_source = True,
   )

   env.Append(BUILDERS = {
      'Doxygen': doxyfile_builder,
   })

   env.AppendUnique(
      DOXYGEN = 'doxygen',
   )
##


## Make sure doxygen exists.
def exists(env):
   return env.Detect("doxygen")
##
