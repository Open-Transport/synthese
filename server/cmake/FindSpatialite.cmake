# - Try to find spatialite
# Once done, this will define
#
#  SPATIALITE_FOUND - system has spatialite
#  SPATIALITE_INCLUDE_DIRS - the spatialite include directories
#  SPATIALITE_LIBRARIES - link these to use spatialite

include(cmake/LibFindMacros.cmake)

if(WIN32)

   IF(DEFINED ENV{SPATIALITE_DIR})
      find_path(SPATIALITE_INCLUDE_DIR spatialite.h PATHS $ENV{SPATIALITE_DIR}/include $ENV{HOME}/rcs/spatialite/include NO_DEFAULT_PATH)
      find_library(SPATIALITE_LIBRARY spatialite PATHS $ENV{SPATIALITE_DIR}/lib $ENV{HOME}/rcs/spatialite/lib NO_DEFAULT_PATH)
   ELSE()
      find_path(SPATIALITE_INCLUDE_DIR spatialite.h PATHS $ENV{HOME}/rcs/spatialite/include NO_DEFAULT_PATH)
      find_library(SPATIALITE_LIBRARY spatialite PATHS $ENV{HOME}/rcs/spatialite/lib NO_DEFAULT_PATH)
   ENDIF()

else(WIN32)

   IF(DEFINED ENV{SPATIALITE_DIR})
      find_path(SPATIALITE_INCLUDE_DIR NAMES spatialite.h PATHS $ENV{SPATIALITE_DIR}/include /opt/rcs/spatialite/include /usr/local/include /usr/include NO_DEFAULT_PATH)
      find_library(SPATIALITE_LIBRARY NAMES spatialite PATHS $ENV{SPATIALITE_DIR}/lib /opt/rcs/spatialite/lib /usr/local/lib /usr/lib NO_DEFAULT_PATH)
   ELSE()
      find_path(SPATIALITE_INCLUDE_DIR NAMES spatialite.h PATHS /opt/rcs/spatialite/include /usr/local/include /usr/include)
      find_library(SPATIALITE_LIBRARY NAMES spatialite PATHS /opt/rcs/spatialite/lib /usr/local/lib /usr/lib)
   ENDIF()

endif(WIN32)

set(SPATIALITE_PROCESS_INCLUDES SPATIALITE_INCLUDE_DIR)
set(SPATIALITE_PROCESS_LIBS SPATIALITE_LIBRARY)
libfind_process(SPATIALITE)
