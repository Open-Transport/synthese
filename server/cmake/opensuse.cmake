# This file contains the settings used to produce an OpenSuse RPM package

set(CPACK_RPM_PACKAGE_NAME "synthese")
set(CPACK_RPM_PACKAGE_ARCHITECTURE "x86_64")
set(CPACK_RPM_PACKAGE_LICENSE "GPL")
set(CPACK_RPM_PACKAGE_VENDOR "SYNTHESE Fondation")

OPTION(OPENSUSE_RPM_132 "Install specific components for OpenSuse 13.2" OFF)
OPTION(CPACK_RPM_FILE_NAME "File name for the RPM package (wthout the .rpm extension)" OFF)
OPTION(CPACK_RPM_PACKAGE_NAME "Package name for the RPM" OFF)
OPTION(CPACK_RPM_USE_GIT_VERSION "Use the full GIT version instead of the tagged one" OFF)

# OpenSuse 13.2
if(OPENSUSE_RPM_132)

  set(CPACK_RPM_PACKAGE_GROUP "Productivity/Publishing/Other")
  set(CPACK_RPM_PACKAGE_SUMMARY "A public transport specialized software. Commit ${GIT_SHA1} on branch ${GIT_REFSPEC}")
  
  # Use the GIT version instead of the Synthese one. Best when creating a dev or test package.
  if(CPACK_RPM_USE_GIT_VERSION)
    git_describe(CPACK_RPM_PACKAGE_VERSION)
  endif()
  
  # Package name : if not defined on command line, use the default
  if(CPACK_RPM_PACKAGE_NAME)
    set(CPACK_RPM_PACKAGE_NAME "${CPACK_RPM_PACKAGE_NAME}")
  endif()
  
  # Package file name : if not defined on command line, use the default
  if(CPACK_RPM_FILE_NAME)
    set(CPACK_PACKAGE_FILE_NAME "${CPACK_RPM_FILE_NAME}")
  endif()

  # Install Systemd scripts
  install(
    FILES
      ${CMAKE_CURRENT_SOURCE_DIR}/utils/systemd/synthese.service
      ${CMAKE_CURRENT_SOURCE_DIR}/utils/systemd/synthese-watchdog.service
      ${CMAKE_CURRENT_SOURCE_DIR}/utils/systemd/synthese_.service
      ${CMAKE_CURRENT_SOURCE_DIR}/utils/systemd/synthese-watchdog_.service
    DESTINATION
      lib/systemd/system
  )
  
  # Install Systemd Synthese config example
  install(
    FILES
      ${CMAKE_CURRENT_SOURCE_DIR}/utils/systemd/synthese.example
    DESTINATION
      share/synthese/
  )

  # Install the watchdog
  install(
    FILES
      ${CMAKE_CURRENT_SOURCE_DIR}/utils/systemd/watchdog.sh
    DESTINATION
      share/synthese/bin
  )

  # We depend on Boost 1.57 and the 3rd party libs
  set(CPACK_RPM_PACKAGE_REQUIRES "boost-synthese = 1.57.0")
  set(CPACK_RPM_PACKAGE_REQUIRES "3rd-synthese")

  # Scripts
  set(CPACK_RPM_POST_INSTALL_SCRIPT_FILE ${CMAKE_CURRENT_SOURCE_DIR}/utils/opensuse/postin.sh)
  set(CPACK_RPM_POST_UNINSTALL_SCRIPT_FILE ${CMAKE_CURRENT_SOURCE_DIR}/utils/opensuse/postun.sh)

endif()
