# 2.8.4 is required because of this issue: http://public.kitware.com/Bug/view.php?id=10356
cmake_minimum_required(VERSION 2.8.4)

# Version:
set(VERSION_MAJOR 3)
set(VERSION_MINOR 10)
set(VERSION_REV 0)
set(SYNTHESE_VERSION "${VERSION_MAJOR}.${VERSION_MINOR}.${VERSION_REV}")

# This variable sets the RPATH for SYNTHESE binary
# It MUST be set before the call to project() otherwise it is silently ignored by CMake
set(CMAKE_INSTALL_PREFIX /opt/rcs/synthese3 CACHE PATH "" FORCE)

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

set(Boost_ADDITIONAL_VERSIONS "1.57")
# Uncomment to enable debugging output when finding Boost.
# set(Boost_DEBUG ON)

find_program(CCACHE_FOUND ccache)
if(CCACHE_FOUND)
    set_property(GLOBAL PROPERTY RULE_LAUNCH_COMPILE ccache)
    set_property(GLOBAL PROPERTY RULE_LAUNCH_LINK ccache)
endif(CCACHE_FOUND)

if(WIN32)
  set(Boost_USE_STATIC_LIBS ON)
  set(Boost_USE_STATIC_RUNTIME ON)
else()
  add_definitions(-DBOOST_ALL_DYN_LINK)
  add_definitions(-DPNG_NO_MMX_CODE)
endif()

add_definitions(-DFUSION_MAX_VECTOR_SIZE=40)
add_definitions(-DFUSION_MAX_MAP_SIZE=40)

if(NOT DEFINED BOOST_VERSION)
  set(BOOST_VERSION 1.57)
endif()

set(BOOST_LIBS program_options filesystem system random regex thread date_time iostreams unit_test_framework)
if(WIN32)
  list(APPEND BOOST_LIBS chrono)
  list(APPEND BOOST_LIBS bzip2)
  list(APPEND BOOST_LIBS zlib)
endif()

if(NOT $ENV{BOOST_ROOT} STREQUAL "")
  set(Boost_NO_SYSTEM_PATHS ON)
endif()
find_package(Boost ${BOOST_VERSION} EXACT COMPONENTS ${BOOST_LIBS})
if(NOT Boost_FOUND)
  message(FATAL_ERROR "Boost not found")
endif()

include_directories(${Boost_INCLUDE_DIRS})

# MYSQL

macro_optional_find_package(MYSQL)
if(WITH_MYSQL AND NOT MYSQL_FOUND)
  message(FATAL_ERROR "MySQL not found")
endif()
if(WITH_MYSQL)
  add_definitions(-DWITH_MYSQL)
endif(WITH_MYSQL)

# Curl is used in the mysql UDF library.
include(FindCURL)
# TODO: win32 support not implemented yet.
if(NOT CURL_FOUND AND NOT WIN32)
  message(FATAL_ERROR "CURL library/headers not found")
endif()
if(CURL_FOUND)
  add_definitions(-DHAVE_CURL)
endif()

if(WIN32)
  # NOTE: _MBCS WIN32 _WINDOWS are already set by cmake
  # TODO: should these defines be set (they are with scons)?:
  # -D_USE_MATH_DEFINES
  # -D__WIN32__
  add_definitions(-DNOMINMAX)

  # Min target is now Windows 7
  add_definitions(-D_WIN32_WINNT=0x0601)
  
  # C++ exceptions are required by Boost
  SET (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /EHsc")

  # This is to avoid:
  # fatal error C1128: number of sections exceeded object file format limit : compile with /bigobj
  add_definitions(/bigobj)

  # Multi process builder
  add_definitions(/MP)

  # This is to avoid:
  # error C2471: cannot update program database
  add_definitions(/Z7)

  # This is to avoid:
  # warning C4290: C++ exception specification ignored except to indicate a function is not __declspec(nothrow)
  add_definitions(/wd4290)

  # This is to avoid:
  # warning C4996: warning C4996: 'std::copy': Function call with parameters that may be unsafe
  add_definitions(/wd4996)

  # This is to avoid:
  # warning C4503: decorated name length exceeded, name was truncated
  add_definitions(/wd4503)

  # This is to avoid:
  # warning C4250: 'xxx' : inherits 'yyy' via dominance
  add_definitions(/wd4250)
  
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


# Version info.
include(FindGit)
include(GetGitRevisionDescription)
get_git_head_revision(GIT_REFSPEC GIT_SHA1 GIT_LAST_COMMIT_DATE)
git_get_exact_tag(GIT_TAG --tags)


# Note: this is using a .h file and not defines, otherwise everything gets
# rebuilt each time CMake is invoked.
configure_file(
  "${PROJECT_SOURCE_DIR}/src/15_server/version.h.in"
  "${PROJECT_BINARY_DIR}/src/15_server/version.h"
)


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
add_subdirectory(src)

OPTION(WITH_TEST "Build and install test" ON)
if(WITH_TEST)
  add_subdirectory(test)
endif()

OPTION(WITH_PACKAGES "Build and install packages" OFF)
if(WITH_PACKAGES)
  add_subdirectory(packages)
endif()

OPTION(WITH_PROJECTS "Build and install projects" OFF)
if(WITH_PROJECTS)
  add_subdirectory(projects)
endif()

OPTION(WITH_UTILS "Build and install utils" OFF)
if(WITH_UTILS)
  add_subdirectory(utils)
endif()

# build a CPack driven installer package
#include(InstallRequiredSystemLibraries)
#set(CPACK_RESOURCE_FILE_LICENSE "${CMAKE_CURRENT_SOURCE_DIR}/License.txt")
set(CPACK_PACKAGE_VERSION_MAJOR ${VERSION_MAJOR})
set(CPACK_PACKAGE_VERSION_MINOR ${VERSION_MINOR})
set(CPACK_PACKAGE_VERSION_PATCH ${VERSION_REV})
set(CPACK_PACKAGE_CONTACT       "contact@rcsmobility.com")

# set CPack generation variables here to remove them from Jenkins jobs
set(CMAKE_INSTALL_PREFIX /opt/rcs/synthese3)
set(CPACK_PACKAGING_INSTALL_PREFIX /opt/rcs/synthese3)
set(CPACK_INCLUDE_TOPLEVEL_DIRECTORY 0)

# if the HEAD does not point to a tag, suffix the archive with 'snapshot'
if("${GIT_TAG}" STREQUAL "")
    set(CPACK_PACKAGE_FILE_NAME "${CMAKE_PROJECT_NAME}-${CPACK_PACKAGE_VERSION_MAJOR}.${CPACK_PACKAGE_VERSION_MINOR}.${CPACK_PACKAGE_VERSION_PATCH}-snapshot-${CPACK_SYSTEM_NAME}")
endif()

include(CPack)
