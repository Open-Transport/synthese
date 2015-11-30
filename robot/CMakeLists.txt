
enable_testing ()

if(NOT S3_SERVER_BINARY)
  set(S3_SERVER_BINARY "${PROJECT_SOURCE_DIR}/../server/build/src/bin/server/s3-server")
endif()
message("Using ${S3_SERVER_BINARY}")

add_subdirectory("cases/inter_synthese")
add_subdirectory("cases/object_services")
add_subdirectory("cases/topical_messaging")
add_subdirectory("cases/multimodal_journey_planner")

if(NOT WITH_INEO_TERMINUS)
  file(APPEND ${CMAKE_BINARY_DIR}/modules.py "WITH_INEO_TERMINUS = False\n")
else()
  file(APPEND ${CMAKE_BINARY_DIR}/modules.py "WITH_INEO_TERMINUS = True\n")
  add_subdirectory("cases/ineo_sae_connector")
endif()

if(NOT WITH_REGULATION)
  file(APPEND ${CMAKE_BINARY_DIR}/modules.py "WITH_REGULATION = False\n")
else()
  file(APPEND ${CMAKE_BINARY_DIR}/modules.py "WITH_REGULATION = True\n")
  add_subdirectory("cases/operations_control")
endif()

add_custom_target(check COMMAND ${CMAKE_CTEST_COMMAND} --verbose)


