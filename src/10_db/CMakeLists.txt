set_source_groups()

set(db_SRCS
DB.cpp
DB.hpp
DBAutoRegisterTableSyncTemplate.hpp
DBConditionalRegistryTableSyncTemplate.hpp
DBConstants.h
DBDirectTableSyncTemplate.hpp
DBEmptyResultException.h
DBException.cpp
DBException.hpp
DBInheritanceTableSyncTemplate.hpp
DBInheritedNoSyncTableSyncTemplate.hpp
DBInheritedRegistryTableSync.hpp
DBInheritedTableSyncTemplate.hpp
DBModule.cpp
DBModule.gen.cpp
DBModuleRegister.cpp
DBModule.h
DBModule.inc.cpp
DBNoSyncTableSyncTemplate.hpp
DBRegistryTableSyncTemplate.hpp
DBResult.cpp
DBResult.hpp
DBTableSync.cpp
DBTableSync.hpp
DBTableSyncTemplate.hpp
DBTransaction.cpp
DBTransaction.hpp
DBTypes.h
DeleteQuery.hpp
Fetcher.h
FetcherTemplate.h
LinkException.h
LoadException.h
RankUpdateQuery.hpp
RemoveObjectAction.cpp
RemoveObjectAction.hpp
ReplaceQuery.h
SelectQuery.hpp
spatial_ref_sys_sql.inc.h
SQLExpression.cpp
SQLExpression.hpp
SQLiteVirtualTable.cpp
SQLiteVirtualTable.hpp
UpdateQuery.cpp
UpdateQuery.hpp
VirtualShapeVirtualTable.cpp
VirtualShapeVirtualTable.hpp
)

set_source_files_properties(DBModule.gen.cpp DBModule.inc.cpp PROPERTIES HEADER_FILE_ONLY 1)

include_directories(${SPATIALITE_INCLUDE_DIRS})

add_library(10_db ${db_SRCS})

target_link_libraries(10_db
  00_framework
  01_util
  01_util_threads
  10_db_sqlite
  # Forward dependencies:
  #15_server
  spatialite
)

if(WITH_MYSQL)
  add_subdirectory(102_mysql)
  target_link_libraries(10_db 10_db_mysql)
endif(WITH_MYSQL)

add_subdirectory(101_sqlite)
target_link_libraries(10_db 10_db_sqlite)

install(TARGETS 10_db DESTINATION lib)
