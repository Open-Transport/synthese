# - Try to find zlib
# Once done, this will define
#
#  ZLIB_FOUND - system has zlib
#  ZLIB_INCLUDE_DIRS - the zlib include directories
#  ZLIB_LIBRARIES - link these to use zlib

include(cmake/LibFindMacros.cmake)

if(WIN32)

   IF(DEFINED ENV{ZLIB_DIR})
      find_path(ZLIB_INCLUDE_DIR zlib.h PATHS $ENV{ZLIB_DIR}/include $ENV{HOME}/rcs/zlib/include NO_DEFAULT_PATH)
      find_library(ZLIB_LIBRARY zlib PATHS $ENV{ZLIB_DIR}/lib $ENV{HOME}/rcs/zlib/lib NO_DEFAULT_PATH)
   ELSE()
      find_path(ZLIB_INCLUDE_DIR zlib.h PATHS $ENV{HOME}/rcs/zlib/include NO_DEFAULT_PATH)
      find_library(ZLIB_LIBRARY zlib PATHS $ENV{HOME}/rcs/zlib/lib NO_DEFAULT_PATH)
   ENDIF()

else(WIN32)

   IF(DEFINED ENV{ZLIB_DIR})
      find_path(ZLIB_INCLUDE_DIR NAMES zlib.h PATHS $ENV{ZLIB_DIR}/include /opt/rcs/zlib/include /usr/local/include /usr/include)
      find_library(ZLIB_LIBRARY NAMES zlib PATHS $ENV{ZLIB_DIR}/lib /opt/rcs/zlib/lib /usr/local/lib /usr/lib)
   ELSE()
      find_path(ZLIB_INCLUDE_DIR NAMES zlib.h PATHS /opt/rcs/zlib/include /usr/local/include /usr/include)
      find_library(ZLIB_LIBRARY NAMES zlib PATHS /opt/rcs/zlib/lib /usr/local/lib /usr/lib)
   ENDIF()

endif(WIN32)

set(ZLIB_PROCESS_INCLUDES ZLIB_INCLUDE_DIR)
set(ZLIB_PROCESS_LIBS ZLIB_LIBRARY)
libfind_process(ZLIB)
