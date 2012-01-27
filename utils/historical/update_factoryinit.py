"""
Create a XXXModuleRegister.cpp file for each modules in order to reduce the
number of files included by main.o (and avoid potential conflicts therefore
created).
"""

__author__ = 'Sylvain Pasche <sylvain.pasche@gmail.com'

import glob
import os.path
from os.path import join

here = os.path.dirname(os.path.abspath(__file__))

#DO_IT = False
DO_IT = True

dirs = (
    glob.glob(join(here, os.pardir, 'src', '*_*')) # +
    # Might not work for submodules.
    #glob.glob(join(here, os.pardir, 'src', '10_db', '*_*'))
)

for dir in dirs:
    if not os.path.isdir(dir):
        continue
    # Activate to test on a single module
    if 0 and not dir.endswith('16_impex'):
        continue
    if 0 and not dir.endswith('18_graph'):
        continue

    print "dir", dir

    cmake_path = join(dir, "CMakeLists.txt")

    if not os.path.isfile(cmake_path):
        print("missing file {}".format(cmake_path))
        continue

    small_module = dir.split(os.sep)[-1].split("_", 1)[1]
    print "small_module", small_module

    if small_module in ['html']:
        print "Ignoring module"
        continue

    modules = glob.glob(join(dir, '*Module.cpp'))
    #if not modules:
    #    print "No module for %s" % small_module
    #    continue
    if modules:
        module_path = modules[0]
        module_file = os.path.split(module_path)[1]
    else:
        module_file = small_module.title() + "Module.cpp"

    register_path = module_file[:-4] + "Register.cpp"
    gen_path = module_file[:-4] + ".gen.cpp"
    inc_path = module_file[:-4] + ".inc.cpp"

    if not os.path.isfile(join(dir, inc_path)):
        print "No module for %s" % small_module
        continue

    register_content = open(join(dir, inc_path), 'rb').read()
    register_content += '\n\n'

    register_content += '#include "%s"\n\n' % inc_path

    register_content += 'void synthese::%s::moduleRegister()\n{\n' % small_module
    for l in open(join(dir, gen_path), 'rb'):
        register_content += '\t' + l

    register_content += '}\n'

    print "register_path", register_path

    if DO_IT:
        open(join(dir, register_path), 'wb').write(register_content)
        open(join(dir, gen_path), 'wb').write('synthese::%s::moduleRegister();\n' % small_module)
        open(join(dir, inc_path), 'wb').write("""namespace synthese
{
\tnamespace %s
\t{
\t\tvoid moduleRegister();
\t}
}
""" % small_module)

    # Update cmake
    lines = open(cmake_path, 'rb').readlines()
    module_header = module_file[:-4] + '.h'
    if not os.path.isfile(join(dir, module_header)):
        module_header = module_file[:-4] + '.hpp'
    assert os.path.isfile(join(dir, module_header))
    idx = lines.index(module_header + '\n')

    assert register_path + '\n' not in lines
    lines.insert(idx, register_path + '\n')

    if DO_IT:
        open(cmake_path, 'wb').writelines(lines)
