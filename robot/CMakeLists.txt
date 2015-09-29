
enable_testing ()

add_subdirectory("cases/inter_synthese")
add_subdirectory("cases/object_services")
add_subdirectory("cases/topical_messaging")

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


