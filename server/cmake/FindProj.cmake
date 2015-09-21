# - Try to find proj
# Once done, this will define
#
#  PROJ_FOUND - system has proj
#  PROJ_INCLUDE_DIRS - the proj include directories
#  PROJ_LIBRARIES - link these to use proj

include(cmake/LibFindMacros.cmake)

if(WIN32)

   IF(DEFINED ENV{PROJ_DIR})
      find_path(PROJ_INCLUDE_DIR proj_api.h PATHS $ENV{PROJ_DIR}/include $ENV{HOME}/rcs/proj/include NO_DEFAULT_PATH)
      find_library(PROJ_LIBRARY proj PATHS $ENV{PROJ_DIR}/lib $ENV{HOME}/rcs/proj/lib NO_DEFAULT_PATH)
   ELSE()
      find_path(PROJ_INCLUDE_DIR proj_api.h PATHS $ENV{HOME}/rcs/proj/include NO_DEFAULT_PATH)
      find_library(PROJ_LIBRARY proj PATHS $ENV{HOME}/rcs/proj/lib NO_DEFAULT_PATH)
   ENDIF()

else(WIN32)

   IF(DEFINED ENV{PROJ_DIR})
      find_path(PROJ_INCLUDE_DIR NAMES proj_api.h PATHS $ENV{PROJ_DIR}/include /opt/rcs/proj/include /usr/local/include /usr/include NO_DEFAULT_PATH)
      find_library(PROJ_LIBRARY NAMES proj PATHS $ENV{PROJ_DIR}/lib /opt/rcs/proj/lib /usr/local/lib /usr/lib NO_DEFAULT_PATH)
   ELSE()
      find_path(PROJ_INCLUDE_DIR NAMES proj_api.h PATHS /opt/rcs/proj/include /usr/local/include /usr/include NO_DEFAULT_PATH)
      find_library(PROJ_LIBRARY NAMES proj PATHS /opt/rcs/proj/lib /usr/local/lib /usr/lib NO_DEFAULT_PATH)
   ENDIF()

endif(WIN32)

set(PROJ_PROCESS_INCLUDES PROJ_INCLUDE_DIR)
set(PROJ_PROCESS_LIBS PROJ_LIBRARY)
libfind_process(PROJ)
