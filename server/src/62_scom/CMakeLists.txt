set_source_groups()

# Here, add each source file (relative to the current directory)
set(scom_SRCS
	SCOMModule.h
	SCOMModule.cpp
	SCOMModule.gen.cpp
	SCOMModule.inc.cpp
	SCOMModuleRegister.cpp
	SCOMSocketReader.h
	SCOMSocketReader.cpp
	SCOMData.h
	SCOMData.cpp
)

# We define here the .gen file used to include and load the module into Synthese
set_source_files_properties(SCOMModule.gen.cpp SCOMModule.inc.cpp PROPERTIES HEADER_FILE_ONLY 1)

# Our module is added as a library for linking
add_library(62_scom ${scom_SRCS})

# Here goes the list of the other modules our module needs to work properly
target_link_libraries(62_scom
  01_util
  15_server
  63_settings
)

# Our library will be installed with the others
install(TARGETS 62_scom DESTINATION lib)
