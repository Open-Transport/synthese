set_source_groups()

set(dblog_SRCS
DBLog.cpp
DBLog.h
DBLogAdmin.cpp
DBLogAdmin.h
DBLogEntry.cpp
DBLogEntry.h
DBLogEntryTableSync.cpp
DBLogEntryTableSync.h
DBLogHTMLView.cpp
DBLogHTMLView.h
DBLogModule.cpp
DBLogModule.gen.cpp
DBLogModuleRegister.cpp
DBLogModule.h
DBLogModule.inc.cpp
DBLogPurgeAction.cpp
DBLogPurgeAction.h
DBLogRight.cpp
DBLogRight.h
DBLogTemplate.cpp
DBLogTemplate.h
DBLogViewService.cpp
DBLogViewService.hpp
)

set_source_files_properties(DBLogModule.gen.cpp DBLogModule.inc.cpp PROPERTIES HEADER_FILE_ONLY 1)

add_library(13_dblog ${dblog_SRCS})

target_link_libraries(13_dblog
  05_html
  10_db
  # Forward dependencies:
  #14_admin
  #15_server
)

install(TARGETS 13_dblog DESTINATION lib)
