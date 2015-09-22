# - Try to find png
# Once done, this will define
#
#  PNG_FOUND - system has png
#  PNG_INCLUDE_DIRS - the png include directories
#  PNG_LIBRARIES - link these to use png

include(cmake/LibFindMacros.cmake)

if(WIN32)

   IF(DEFINED ENV{PNG_DIR})
      find_path(PNG_INCLUDE_DIR png.h PATHS $ENV{PNG_DIR}/include $ENV{HOME}/rcs/png/include NO_DEFAULT_PATH)
      find_library(PNG_LIBRARY png PATHS $ENV{PNG_DIR}/lib $ENV{HOME}/rcs/png/lib NO_DEFAULT_PATH)
   ELSE()
      find_path(PNG_INCLUDE_DIR png.h PATHS $ENV{HOME}/rcs/png/include NO_DEFAULT_PATH)
      find_library(PNG_LIBRARY png PATHS $ENV{HOME}/rcs/png/lib NO_DEFAULT_PATH)
   ENDIF()

else(WIN32)

   IF(DEFINED ENV{PNG_DIR})
      find_path(PNG_INCLUDE_DIR NAMES png.h PATHS $ENV{PNG_DIR}/include /opt/rcs/png/include /usr/local/include /usr/include NO_DEFAULT_PATH)
      find_library(PNG_LIBRARY NAMES png PATHS $ENV{PNG_DIR}/lib /opt/rcs/png/lib /usr/local/lib /usr/lib NO_DEFAULT_PATH)
   ELSE()
      find_path(PNG_INCLUDE_DIR NAMES png.h PATHS /opt/rcs/png/include /usr/local/include /usr/include NO_DEFAULT_PATH)
      find_library(PNG_LIBRARY NAMES png PATHS /opt/rcs/png/lib /usr/local/lib /usr/lib NO_DEFAULT_PATH)
   ENDIF()

endif(WIN32)

set(PNG_PROCESS_INCLUDES PNG_INCLUDE_DIR)
set(PNG_PROCESS_LIBS PNG_LIBRARY)
libfind_process(PNG)
