# - Try to find expat
# Once done, this will define
#
#  EXPAT_FOUND - system has expat
#  EXPAT_INCLUDE_DIRS - the expat include directories
#  EXPAT_LIBRARIES - link these to use expat

include(cmake/LibFindMacros.cmake)

if(WIN32)

   IF(DEFINED ENV{EXPAT_DIR})
      find_path(EXPAT_INCLUDE_DIR expat.h PATHS $ENV{EXPAT_DIR}/include $ENV{HOME}/rcs/expat/include NO_DEFAULT_PATH)
      find_library(EXPAT_LIBRARY expat PATHS $ENV{EXPAT_DIR}/lib $ENV{HOME}/rcs/expat/lib NO_DEFAULT_PATH)
   ELSE()
      find_path(EXPAT_INCLUDE_DIR expat.h PATHS $ENV{HOME}/rcs/expat/include NO_DEFAULT_PATH)
      find_library(EXPAT_LIBRARY expat PATHS $ENV{HOME}/rcs/expat/lib NO_DEFAULT_PATH)
   ENDIF()

else(WIN32)

   IF(DEFINED ENV{EXPAT_DIR})
      find_path(EXPAT_INCLUDE_DIR NAMES expat.h PATHS $ENV{EXPAT_DIR}/include /opt/rcs/expat/include /usr/local/include /usr/include NO_DEFAULT_PATH)
      find_library(EXPAT_LIBRARY NAMES expat PATHS $ENV{EXPAT_DIR}/lib /opt/rcs/expat/lib /usr/local/lib /usr/lib NO_DEFAULT_PATH)
   ELSE()
      find_path(EXPAT_INCLUDE_DIR NAMES expat.h PATHS /opt/rcs/expat/include /usr/local/include /usr/include NO_DEFAULT_PATH)
      find_library(EXPAT_LIBRARY NAMES expat PATHS /opt/rcs/expat/lib /usr/local/lib /usr/lib NO_DEFAULT_PATH)
   ENDIF()

endif(WIN32)

set(EXPAT_PROCESS_INCLUDES EXPAT_INCLUDE_DIR)
set(EXPAT_PROCESS_LIBS EXPAT_LIBRARY)
libfind_process(EXPAT)
