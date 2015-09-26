# - Try to find geos
# Once done, this will define
#
#  GEOS_FOUND - system has geos
#  GEOS_INCLUDE_DIRS - the geos include directories
#  GEOS_LIBRARIES - link these to use geos

include(cmake/LibFindMacros.cmake)

if(WIN32)

   IF(DEFINED ENV{GEOS_DIR})
     find_path(GEOS_INCLUDE_DIR geos.h PATHS $ENV{GEOS_DIR}/include $ENV{HOME}/rcs/geos/include NO_DEFAULT_PATH)
     find_library(GEOS_LIBRARY geos PATHS $ENV{GEOS_DIR}/lib $ENV{HOME}/rcs/geos/lib NO_DEFAULT_PATH)
   ELSE()
     find_path(GEOS_INCLUDE_DIR geos.h PATHS $ENV{HOME}/rcs/geos/include NO_DEFAULT_PATH)
     find_library(GEOS_LIBRARY geos PATHS $ENV{HOME}/rcs/geos/lib NO_DEFAULT_PATH)
   ENDIF()

else(WIN32)

   IF(DEFINED ENV{GEOS_DIR})
   	  find_path(GEOS_INCLUDE_DIR NAMES geos.h PATHS $ENV{GEOS_DIR}/include /opt/rcs/geos/include /usr/local/include /usr/include NO_DEFAULT_PATH)
      find_library(GEOS_LIBRARY NAMES geos PATHS $ENV{GEOS_DIR}/lib /opt/rcs/geos/lib /usr/local/lib /usr/lib NO_DEFAULT_PATH)
   ELSE()
      find_path(GEOS_INCLUDE_DIR NAMES geom.h PATHS /opt/rcs/geos/include /usr/local/include /usr/include/geos)
      find_library(GEOS_LIBRARY NAMES geos PATHS /opt/rcs/geos/lib /usr/local/lib /usr/lib)
   ENDIF()

endif(WIN32)

set(GEOS_PROCESS_INCLUDES GEOS_INCLUDE_DIR)
set(GEOS_PROCESS_LIBS GEOS_LIBRARY)
libfind_process(GEOS)
