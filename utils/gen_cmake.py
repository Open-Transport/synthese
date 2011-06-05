# Script used to generate or update CMake files.
#
# Some stuff is very hacky and might not work, but the code is still useful as a
# reference for later updates.

__author__ = 'Sylvain Pasche <sylvain.pasche@gmail.com>'

import array
import glob
import json
import os.path
from os.path import join
import re
import sys

TMPL = """
set({module}_SRCS
{files}
)

add_library(s3-{module} ${{{module}_SRCS}})

"""

small_module_to_module = {}

here = os.path.dirname(os.path.abspath(__file__))
dirs = [d for d in
    glob.glob(join(here, os.pardir, 'src', '*_*')) +
    glob.glob(join(here, os.pardir, 'src', '*', '*'))
if os.path.isdir(d) and os.path.isfile(join(d, 'CMakeLists.txt'))]

for dir_abs in dirs:
    print "DIR", dir_abs
    dir = dir_abs.split(os.sep)[-1]
    if dir == "server":
        continue

    small_module = dir.split("_", 1)[1] if "_" in dir else dir
    small_module = small_module.replace("_", "-")
    # special cases. Keys are module names, as read from the directory they are
    # in, values are the name of the module used by Scons.
    SPECIAL_CASES = {
        "iostreams": "util-iostreams",
        "threads": "util-threads",
        "sqlite": "db-sqlite",
    }
    if small_module in SPECIAL_CASES:
        small_module = SPECIAL_CASES[small_module]
    small_module_to_module[small_module] = dir_abs.split(os.sep)[-1]

replacements = []

for dir_abs in dirs:
    t = join(dir_abs, "CMakeLists.txt")

    dir = dir_abs.split(os.sep)[-1]

    # Generate CMakeLists.txt files
    if 0:
        if os.path.isfile(t):
            print("already there in {}".format(t))
            continue
        print("Filling {}".format(dir))
        module = dir.split("_", 1)[1]
        files = "\n".join(sorted(os.path.basename(p) for p in glob.glob(join(dir, "*cpp"))))
        content = TMPL.format(module=module, files=files)
        open(t, "wb").write(content)

    # Update list of sources
    if 0:
        if not os.path.isfile(t):
            print("missing file {}".format(t))
            continue
        print("Filling {}".format(dir))
        try:
            module = dir.split("_", 1)[1]
        except IndexError:
            print "Warning: skipping", dir
            continue
        paths = (
            glob.glob(join(dir_abs, "*c")) +
            glob.glob(join(dir_abs, "*cpp")) +
            glob.glob(join(dir_abs, "*h")) +
            glob.glob(join(dir_abs, "*hpp")))
        files = sorted((os.path.basename(p) for p in paths), key=str.lower)

        lines = open(t).read().splitlines()
        print len(lines)
        for index, item in enumerate(lines):
            if item.startswith("set(") and "_SRCS" in item:
                start = index
                break
        end = lines.index(")")
        if 1:
            print "::", start, end
            print "BEFORE"
            print "\n".join(lines)
            print "===END BEFORE"
        lines[start + 1:end] = files

        skip_build_sources = [f for f in files if "gen.cpp" in f or "inc.cpp" in f]
        if skip_build_sources:
            lines[end + 1:end + 1] = [
                "",
                "set_source_files_properties({} PROPERTIES HEADER_FILE_ONLY 1)".format(
                    " ".join(skip_build_sources))]

        if 1:
            print "AFTER"
            print "\n".join(lines)
            print "=== END AFTER"
        #content = TMPL.format(module=module, files=files)
        #print content
        content = "\n".join(lines)
        if not content.endswith("\n"):
            content += "\n"
        if 1:
            open(t, "wb").write(content)

    # Replace module name
    if 0:
        if not os.path.isfile(t):
            continue
        content = open(t).read()
        small_module = dir.split("_", 1)[1]
        print "SMALL", small_module
        module = dir
        from_ = "s3-%s" % small_module
        to = module
        replacements.append((from_, to))
        print ">>>", module
        content = content.replace(from_, to)
        print "New content", content
        open(t, "wb").write(content)

    # Add INSTALL target
    if 0:
        if not os.path.isfile(t):
            continue
        content = open(t).read()
        module = dir
        content += "\ninstall(TARGETS %s DESTINATION lib)\n" % module
        #print "New content", content
        open(t, "wb").write(content)

    # Add a target_link_libraries command for Boost.
    # FIXME: this doesn't work well if there's already a target_link_libraries
    # spread on multiple lines.
    if 0:
        if not os.path.isfile(t):
            continue

        lines = open(t).readlines()
        last_index = -1
        for index, l in enumerate(lines):
            if "target_link_libraries" in l:
                last_index = index
        if last_index < 0:
            for index, l in enumerate(lines):
                if "add_library" in l:
                    #lines.insert(index, "\n")
                    #last_index = index + 1
                    last_index = index
                    break

        if last_index < 0:
            raise Exception("No target_link_libraries found in %s" % t)

        print "%s Index %i" % (t, last_index)
        if "target_link_libraries" not in lines[last_index]:
            prefix = "\n"
        else:
            prefix = ""
        lines.insert(last_index + 1, prefix + "target_link_libraries(%s ${Boost_LIBRARIES})\n" % module)

        #print "New content", "".join(lines)
        open(t, "wb").write("".join(lines))

    # Add target_link_libraries rules from SConstruct files
    if 0:
        if not os.path.isfile(t):
            continue

        MODULE_REMAP = {
            "threads": "01_util_threads",
            "iostreams": "01_util_iostreams",
            "101_sqlite": "10_db_sqlite",
            "102_mysql": "10_db_mysql",
        }

        module = MODULE_REMAP.get(dir, dir)

        # for debugging
        if 0 and module != '10_db':
            continue
        if 0 and module != '06_openstreetmap':
            continue

        lines = open(t).readlines()

        print "module", module

        sconscript = join(dir_abs, 'Sconscript')
        print sconscript
        sconscript_content = open(sconscript, 'rb').read()

        # dependencies

        sconscript_content = re.sub("s3env\['BOOSTVERSION'\]", "", sconscript_content)

        m = re.search('dependencies.*(\[[^\]]+\])', sconscript_content)
        if not m:
            print "No dep found"
            continue
        print "DEPS", repr(m.group(1))

        deps = [d.replace("'", "") for d in re.findall("'[^']+'", m.group(1))]

        def module_from_dep(dep):
            m = re.search('s3-(.*)\.cpplib', dep)
            if m:
                print "found module", m.group(1)
                mod = small_module_to_module[m.group(1)]
                return MODULE_REMAP.get(mod, mod)

            m = re.search('(.*)\.cppinc', dep)
            if m:
                print "found cpping", m.group(1)
                if m.group(1) == 'boost':
                    return '${Boost_LIBRARIES}'

                raise Exception("no rule for %s" % other)
                return None

            m = re.search('(.*)\.cpplib', dep)
            if m:
                other = m.group(1)
                print "found other", other
                WHITELIST = set(('proj', 'geos', 'spatialite', 'expat'))
                if other in WHITELIST:
                    return other
                if other.startswith('boost'):
                    return '${Boost_LIBRARIES}'
                if other in ('nscube',):
                    return None
                raise Exception("no rule for %s" % other)
                return None

            BLACKLIST = set(('3.2.patch1a',))
            if dep in BLACKLIST:
                return None

            assert False, "unknown dep %s" % dep

        new_deps = set()
        for dep in deps:
            m = module_from_dep(dep)
            if not m:
                print "Warning, no module for dep", dep
                continue
            new_deps.add(m)

        print "new deps", new_deps

        def str_insert(string, pos, content):
            return string[:pos] + content + string[pos:]

        cmake_content = open(t, 'rb').read()
        m = re.search('target_link_libraries\w*\([^\)]+\)\n', cmake_content, re.DOTALL)
        if m:
            print "______ existing targets:", repr(m.group(0)), "POS", m.span(0)
            pos = m.start(0)
            cmake_content = cmake_content.replace(m.group(0), '')
        else:
            m = re.search('add_library\w*\([^\)]+\)\n', cmake_content, re.DOTALL)
            pos = m.end(0)
            cmake_content = str_insert(cmake_content, pos, '\n')
            pos += 1

        try:
            module_number = int(module.split("_")[0])
        except ValueError:
            module_number = sys.maxint

        lines = []
        lines = []
        seen_forward_dep = False
        for d in sorted(new_deps):
            try:
                dep_number = int(d.split("_")[0])
            except ValueError:
                dep_number = -1
            forward_dep = dep_number > module_number
            if forward_dep and not seen_forward_dep:
                seen_forward_dep = True
                lines.append("  # Forward dependencies:\n")
            prefix = "#" if forward_dep else ""
            lines.append('  %s%s\n' % (prefix, d))
        target_link_text = 'target_link_libraries(%s\n%s)\n' % (module, ''.join(lines))
        cmake_content = str_insert(cmake_content, pos, target_link_text)

        #SUFFIX = ".new"
        SUFFIX = ""
        open(t + SUFFIX, "wb").write(cmake_content)

    # Add set_source_groups() macro call
    if 0:
        if not os.path.isfile(t):
            continue
        content = open(t).read()
        if "set_source_groups()" in content:
            continue
        content = "set_source_groups()\n" + content
        if 0:
            print "New content: ----------------"
            print  content
            print "-----------------------------"
        open(t, "wb").write(content)

if replacements:
    def replace_in_file(f):
        content = open(f).read()
        for from_, to in replacements:
            print from_, to
            content = content.replace(from_, to)
        print content
        open(f, "wb").write(content)

    for f in ["bin/server/CMakeLists.txt", "../test/01_util/CMakeLists.txt", "../test/02_db/CMakeLists.txt"]:
        replace_in_file(f)
