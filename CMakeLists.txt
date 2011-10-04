# 2.8.4 is required because of this issue: http://public.kitware.com/Bug/view.php?id=10356
cmake_minimum_required(VERSION 2.8.4)

# See http://www.cmake.org/Wiki/CMake_FAQ#Make_Override_Files
# TODO: Maybe use shared libraries and remove this.
set(CMAKE_USER_MAKE_RULES_OVERRIDE
  ${CMAKE_CURRENT_SOURCE_DIR}/c_flag_overrides.cmake)
set(CMAKE_USER_MAKE_RULES_OVERRIDE_CXX
  ${CMAKE_CURRENT_SOURCE_DIR}/cxx_flag_overrides.cmake)

project(synthese3)

set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake")
include(cmake/MacroOptionalFindPackage.cmake)

# Build shared library on Linux.
# On Windows, building DLLs is more involving and won't work as of now
# (see http://www.itk.org/Wiki/BuildingWinDLL)
if(UNIX)
  set(BUILD_SHARED_LIBS ON)
endif()

# Boost

set(Boost_ADDITIONAL_VERSIONS "1.39" "1.42")
# Uncomment to enable debugging output when finding Boost.
set(Boost_DEBUG ON)

if(WIN32)
  set(Boost_USE_STATIC_LIBS ON)
  set(Boost_USE_STATIC_RUNTIME ON)
else()
  add_definitions(-DBOOST_ALL_DYN_LINK)
endif()

if(NOT DEFINED BOOST_VERSION)
  set(BOOST_VERSION 1.39)
endif()

set(BOOST_LIBS program_options filesystem system regex thread date_time iostreams unit_test_framework)
if(WIN32)
  list(APPEND BOOST_LIBS bzip2)
endif()

find_package(Boost ${BOOST_VERSION} COMPONENTS ${BOOST_LIBS})
if(NOT Boost_FOUND)
  message(FATAL_ERROR "Boost not found")
endif()

include_directories(${Boost_INCLUDE_DIRS})

# MySQL

macro_optional_find_package(MySQL)
if(WITH_MYSQL AND NOT MYSQL_FOUND)
  message(FATAL_ERROR "MySQL not found")
endif()
if(WITH_MYSQL)
  add_definitions(-DWITH_MYSQL)
endif(WITH_MYSQL)

if(WITH_MYSQL)
  # Curl is used in the mysql UDF library
  include(FindCURL)
  # TODO: win32 support not implemented yet.
  if(NOT CURL_FOUND AND NOT WIN32)
    message(FATAL_ERROR "CURL library/headers not found")
  endif()
endif()

if(WIN32)
  # NOTE: _MBCS WIN32 _WINDOWS are already set by cmake
  # TODO: should these defines be set (they are with scons)?:
  # -D_USE_MATH_DEFINES
  # -D__WIN32__
  add_definitions(-DNOMINMAX)

  add_definitions(-D_WIN32_WINNT=0x0501)

  # This is to avoid:
  # fatal error C1128: number of sections exceeded object file format limit : compile with /bigobj
  add_definitions(/bigobj)

  # iconv
  add_library(iconv STATIC IMPORTED)
  set_property(TARGET iconv PROPERTY IMPORTED_LOCATION
    "${PROJECT_SOURCE_DIR}/3rd/dev/iconv/lib/libiconv.lib")

  # expat
  add_definitions(-DXML_STATIC)

endif()

if(CMAKE_BUILD_TYPE STREQUAL Debug)
  add_definitions(-D_DEBUG)
endif()

# gcc warning settings
if (CMAKE_COMPILER_IS_GNUCC OR CMAKE_COMPILER_IS_GNUCXX)
  add_definitions(-Wall -Wextra -Wno-unused-parameter)
endif ()

add_definitions(-DCMAKE)

include(FindPythonInterp)
if(NOT PYTHONINTERP_FOUND)
  message(FATAL_ERROR "No Python interpreter found. Cannot continue.")
endif()

enable_testing()

# See http://www.vtk.org/Wiki/CMake_RPATH_handling#Always_full_RPATH
SET(CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE)
# the RPATH to be used when installing, but only if it's not a system directory
LIST(FIND CMAKE_PLATFORM_IMPLICIT_LINK_DIRECTORIES "${CMAKE_INSTALL_PREFIX}/lib" isSystemDir)
IF("${isSystemDir}" STREQUAL "-1")
  SET(CMAKE_INSTALL_RPATH "${CMAKE_INSTALL_PREFIX}/lib")
ENDIF("${isSystemDir}" STREQUAL "-1")

# Global variables.
include(global_vars.cmake)

add_subdirectory(3rd)
add_subdirectory(s3-admin)
add_subdirectory(src)
add_subdirectory(packages)
add_subdirectory(projects)
add_subdirectory(tools)
add_subdirectory(test)
add_subdirectory(utils)

# To build the packages, you can increment the build number and add a
# "cmd:create_package" string in your commit message. (build number is ignored,
# this is only used to do a dummy commit).
# 1

# build a CPack driven installer package
#include(InstallRequiredSystemLibraries)
#set(CPACK_RESOURCE_FILE_LICENSE "${CMAKE_CURRENT_SOURCE_DIR}/License.txt")
set(CPACK_PACKAGE_VERSION_MAJOR "3")
set(CPACK_PACKAGE_VERSION_MINOR "3")
set(CPACK_PACKAGE_VERSION_PATCH "0")
set(CPACK_PACKAGE_CONTACT       "contact@rcsmobility.com")

#if(NOT CPACK_PACKAGING_INSTALL_PREFIX)
#  set(CPACK_PACKAGING_INSTALL_PREFIX "/opt/rcs")
#endif()

include(CPack)
