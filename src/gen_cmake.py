# Temporary script used to generate or update cmake files.

import glob
import os.path
from os.path import join

TMPL = """
set({module}_SRCS
{files}
)

add_library(s3-{module} ${{{module}_SRCS}})

"""

replacements = []
for dir in glob.glob("*_*"):
    t = join(dir, "CMakeLists.txt")
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
        module = dir.split("_", 1)[1]
        paths = (
            glob.glob(join(dir, "*c")) +
            glob.glob(join(dir, "*cpp")) +
            glob.glob(join(dir, "*h")) +
            glob.glob(join(dir, "*hpp")))
        paths = [p for p in paths if "gen.cpp" not in p]
        paths = [p for p in paths if "inc.cpp" not in p]
        files = sorted(os.path.basename(p) for p in paths)
        
        lines = open(t).read().splitlines()
        print len(lines)
        for index, item in enumerate(lines):
            if item.startswith("set(") and "_SRCS" in item:
                start = index
                break
        end = lines.index(")")
        if 0:
            print "::", start, end
            print "BEFORE"
            print "\n".join(lines)
            print "===END BEFORE"
        lines[start + 1:end] = files
        if 0:
            print "AFTER"
            print "\n".join(lines)
            print "=== END AFTER"
        #content = TMPL.format(module=module, files=files)
        #print content
        content = "\n".join(lines)
        if not content.endswith("\n"):
            content += "\n"
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
        module = dir

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
