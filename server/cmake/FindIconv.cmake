# - Try to find iconv
# Once done, this will define
#
#  ICONV_FOUND - system has iconv
#  ICONV_INCLUDE_DIRS - the iconv include directories
#  ICONV_LIBRARIES - link these to use iconv

include(${CMAKE_SOURCE_DIR}/cmake/LibFindMacros.cmake)

if(WIN32)

   IF(DEFINED ENV{ICONV_DIR})
      find_path(ICONV_INCLUDE_DIR iconv.h PATHS $ENV{ICONV_DIR}/include $ENV{HOME}/rcs/iconv/include NO_DEFAULT_PATH)
      find_library(ICONV_LIBRARY libiconv PATHS $ENV{ICONV_DIR}/lib $ENV{HOME}/rcs/iconv/lib NO_DEFAULT_PATH)
   ELSE()
      find_path(ICONV_INCLUDE_DIR iconv.h PATHS $ENV{HOME}/rcs/iconv/include NO_DEFAULT_PATH)
      find_library(ICONV_LIBRARY libiconv PATHS $ENV{HOME}/rcs/iconv/lib NO_DEFAULT_PATH)
   ENDIF()

endif(WIN32)

set(ICONV_PROCESS_INCLUDES ICONV_INCLUDE_DIR)
set(ICONV_PROCESS_LIBS ICONV_LIBRARY)
libfind_process(ICONV)
