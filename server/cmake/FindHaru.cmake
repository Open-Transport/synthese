# - Try to find haru
# Once done, this will define
#
#  HARU_FOUND - system has haru
#  HARU_INCLUDE_DIRS - the haru include directories
#  HARU_LIBRARIES - link these to use haru

include(${CMAKE_SOURCE_DIR}/cmake/LibFindMacros.cmake)

if(WIN32)

   IF(DEFINED ENV{HARU_DIR})
      find_path(HARU_INCLUDE_DIR hpdf.h PATHS $ENV{HARU_DIR}/include $ENV{HOME}/rcs/haru/include NO_DEFAULT_PATH)
      find_library(HARU_LIBRARY haru PATHS $ENV{HARU_DIR}/lib $ENV{HOME}/rcs/haru/lib NO_DEFAULT_PATH)
   ELSE()
      find_path(HARU_INCLUDE_DIR hpdf.h PATHS $ENV{HOME}/rcs/haru/include NO_DEFAULT_PATH)
      find_library(HARU_LIBRARY haru PATHS $ENV{HOME}/rcs/haru/lib NO_DEFAULT_PATH)
   ENDIF()

else(WIN32)

   IF(DEFINED ENV{HARU_DIR})
      find_path(HARU_INCLUDE_DIR NAMES hpdf.h PATHS $ENV{HARU_DIR}/include /opt/rcs/haru/include /usr/local/include /usr/include NO_DEFAULT_PATH)
      find_library(HARU_LIBRARY NAMES haru PATHS $ENV{HARU_DIR}/lib /opt/rcs/haru/lib /usr/local/lib /usr/lib NO_DEFAULT_PATH)
   ELSE()
      find_path(HARU_INCLUDE_DIR NAMES hpdf.h PATHS /opt/rcs/haru/include /usr/local/include /usr/include NO_DEFAULT_PATH)
      find_library(HARU_LIBRARY NAMES hpdf PATHS /opt/rcs/haru/lib /usr/local/lib /usr/lib NO_DEFAULT_PATH)
   ENDIF()

endif(WIN32)

set(HARU_PROCESS_INCLUDES HARU_INCLUDE_DIR)
set(HARU_PROCESS_LIBS HARU_LIBRARY)
libfind_process(HARU)
