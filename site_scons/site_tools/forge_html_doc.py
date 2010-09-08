## @namespace forge_html_doc Forge HTML documentation generator
## @ingroup forge2SconsBuilders


from SCons.Defaults import *
import os
import os.path


def printsource(htmlfile, nodes):
  for s in nodes:
    if s.__class__.__name__ == 'Value':
      htmlfile.write('<li>List :<ul>')
      for v in s.value:
        htmlfile.write('<li>' + str(v) + '</li>')
      htmlfile.write('</ul></li>')
    else:
      htmlfile.write('<li>' + str(s) +' ('+ s.__class__.__name__ + ')')
      if s.sources:
        htmlfile.write('<ul>')
        printsource(htmlfile,s.sources)
        htmlfile.write('</ul>')
      htmlfile.write('</li>')
  
def forge_html_doc_impl (target, source, env):

  htmlfile = open (target[0].abspath, "w" )

  htmlfile.write('<html><head><link rel="stylesheet" type="text/css" href="forge_html_generated_doc.css"></link></head><body>')
  
  for p in env.GetRootEnv()['_PROJECTS'].values():
    htmlfile.write('<a name="'+ p.get_full_name() + '"></a>')
    htmlfile.write('<h1>'+ p.get_full_name() + ' ('+ p.__class__.__name__ +')</h1>')
    htmlfile.write('<table><tr><th class="src">Sources/targets</th><th class="dep">Dependencies</th></tr><tr><td><ol>')
    printsource(htmlfile, p.targets)
    htmlfile.write('</ol></td><td>Direct : <ul>')
    di = p.get_dependencies(False);
    al = p.get_dependencies(True);
    for d in di:
      htmlfile.write('<li><a href="#' + d[0].get_full_name() +'">' + d[0].get_full_name() + '</a></li>')
    htmlfile.write('</ul>')
    htmlfile.write('Inherited : <ul>')
    for d in al:
      if d not in di:
        htmlfile.write('<li><a href="#' + d[0].get_full_name() +'">' + d[0].get_full_name() + '</a></li>')
    htmlfile.write('</ul>')
    if(p.get_type() == 'deb'):
      htmlfile.write('Direct packages : <ul>')
      di = p.get_deb_dependencies()
      al = p.get_deb_dependencies(all = True)
      for d in di:
        htmlfile.write('<li>' + d[0] +' '+ d[1] + '</li>')
      htmlfile.write('</ul>Inherited packages :<ul>')
      for d in al:
        if d not in di:
          htmlfile.write('<li>' + d[0] +' '+ d[1] + '</li>')
      htmlfile.write('</ul>')
      
    htmlfile.write('</td></tr></table>')
    htmlfile.write('</li>')

  htmlfile.write('</body></html>')

  htmlfile.close()
  return 0
##


## Forge HTML doc builder emitter.
## Targets addings :
##  - the generated HTML files in /site_scons/doc/include/forge_html_generated_doc.html
##
## Sources addings :
##  - SConstruct
##  - All SConscript
##  - All python scripts present in site_tools
def forge_html_doc_emitter(source, target, env):
  source.append(env.SrcFile('#/SConstruct'))
  source.append(env.Glob('SConscript', dir=env.SrcDir('#'), recursive=True, abs_path=True))
  source.append(env.Glob('*.py', dir=env.SrcDir('#/site_scons'), recursive=True, abs_path=True))

  target.append(env.SrcDir('#/infra/rcs/doc/include') + os.sep + 'forge_html_generated_doc.html')

  return (target, source)
##


## Add builders and construction variables.
def generate(env):
   
   import SCons.Builder
   forge_html_doc_builder = SCons.Builder.Builder(
      action = forge_html_doc_impl,
      target_factory = env.fs.File,
      emitter = forge_html_doc_emitter
   )

   env.Append(BUILDERS = {
      'ForgeHtmlDoc': forge_html_doc_builder,
   })
##


##
def exists(env):
   return True

