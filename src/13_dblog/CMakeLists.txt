
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
DBLogModule.h
DBLogPurgeAction.cpp
DBLogPurgeAction.h
DBLogRight.cpp
DBLogRight.h
DBLogTemplate.cpp
DBLogTemplate.h
)

add_library(13_dblog ${dblog_SRCS})

install(TARGETS 13_dblog DESTINATION lib)
