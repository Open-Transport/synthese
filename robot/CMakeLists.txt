
enable_testing ()

if(WITH_INEO_TERMINUS)
	add_subdirectory("cases/ineo_sae_connector")
endif(WITH_INEO_TERMINUS)

add_subdirectory("cases/inter_synthese")
add_subdirectory("cases/object_services")

if(WITH_REGULATION)
	add_subdirectory("cases/operations_control")
endif(WITH_REGULATION)

add_subdirectory("cases/topical_messaging")

add_custom_target(check COMMAND ${CMAKE_CTEST_COMMAND} --verbose)


