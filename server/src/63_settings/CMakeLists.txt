set_source_groups()

# Here, add each source file (relative to the current directory)
set(settings_SRCS
	SettingsModule.h
	SettingsModule.cpp
	SettingsModule.gen.cpp
	SettingsModule.inc.cpp
	SettingsModuleRegister.cpp
	SettingsTableSync.h
	SettingsTableSync.cpp
	SettingsFunction.h
	SettingsFunction.cpp
	Settings.h
	Settings.cpp
	Settable.h
)

# We define here the .gen file used to include and load the module into Synthese
set_source_files_properties(SettingsModule.gen.cpp SettingsModule.inc.cpp PROPERTIES HEADER_FILE_ONLY 1)

# Our module is added as a library for linking
add_library(63_settings ${settings_SRCS})

# Here goes the list of the other modules our module needs to work properly
target_link_libraries(63_settings
  01_util
  15_server
)

# Our library will be installed with the others
install(TARGETS 63_settings DESTINATION lib)
