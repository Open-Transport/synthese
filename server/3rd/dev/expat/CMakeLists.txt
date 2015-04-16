
set(expat_SRCS
src/amigaconfig.h
src/ascii.h
src/asciitab.h
src/expat.h
src/expat_external.h
src/iasciitab.h
src/internal.h
src/latin1tab.h
src/macconfig.h
src/nametab.h
src/utf8tab.h
src/winconfig.h
src/xmlparse.c
src/xmlrole.c
src/xmlrole.h
src/xmltok.c
src/xmltok.h
src/xmltok_impl.c
src/xmltok_impl.h
src/xmltok_ns.c
)

if(WIN32)
  add_definitions(-DCOMPILED_FROM_DSP)
else()
  add_definitions(-DHAVE_MEMMOVE)
endif()

include_directories(${EXPAT_INCLUDE_DIRS})

add_library(expat ${expat_SRCS})

install(TARGETS expat DESTINATION lib)
