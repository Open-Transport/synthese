# - Try to find geosc
# Once done, this will define
#
#  GEOSC_FOUND - system has geosc
#  GEOSC_INCLUDE_DIRS - the geosc include directories
#  GEOSC_LIBRARIES - link these to use geosc

include(cmake/LibFindMacros.cmake)


if(WIN32)

   IF(DEFINED ENV{GEOSC_DIR})
      find_path(GEOSC_INCLUDE_DIR geos_c.h PATHS $ENV{GEOSC_DIR}/capi $ENV{HOME}/rcs/geos/capi NO_DEFAULT_PATH)
      find_library(GEOSC_LIBRARY geosc PATHS $ENV{GEOSC_DIR}/lib $ENV{HOME}/rcs/geos/lib NO_DEFAULT_PATH)
   ELSE()
      find_path(GEOSC_INCLUDE_DIR geos_c.h PATHS $ENV{HOME}/rcs/geos/capi NO_DEFAULT_PATH)
      find_library(GEOSC_LIBRARY geosc PATHS $ENV{HOME}/rcs/geos/lib NO_DEFAULT_PATH)
   ENDIF()

else(WIN32)

   IF(DEFINED ENV{GEOSC_DIR})
      find_path(GEOSC_INCLUDE_DIR NAMES geos_c.h PATHS $ENV{GEOSC_DIR}/capi /opt/rcs/geos/include /usr/local/include /usr/include NO_DEFAULT_PATH)
      find_library(GEOSC_LIBRARY NAMES geosc PATHS $ENV{GEOSC_DIR}/lib /opt/rcs/geos/lib /usr/local/lib /usr/lib NO_DEFAULT_PATH)
   ELSE()
      find_path(GEOSC_INCLUDE_DIR NAMES geos_c.h PATHS /opt/rcs/geos/include /usr/local/include /usr/include NO_DEFAULT_PATH)
      find_library(GEOSC_LIBRARY NAMES geosc PATHS /opt/rcs/geos/lib /usr/local/lib /usr/lib NO_DEFAULT_PATH)
   ENDIF()



endif(WIN32)

set(GEOSC_PROCESS_INCLUDES GEOSC_INCLUDE_DIR)
set(GEOSC_PROCESS_LIBS GEOSC_LIBRARY)
libfind_process(GEOSC)
