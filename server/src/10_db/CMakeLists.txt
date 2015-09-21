set_source_groups()

set(db_SRCS
ComposedExpression.cpp
ComposedExpression.hpp
ConditionalSynchronizationPolicy.hpp
ConditionalSynchronizationPolicyBase.hpp
DB.cpp
DB.hpp
DBConstants.h
DBDirectTableSync.hpp
DBDirectTableSyncTemplate.hpp
DBEmptyResultException.h
DBException.cpp
DBException.hpp
DBDeleteInterSYNTHESEContent.cpp
DBDeleteInterSYNTHESEContent.hpp
DBInterSYNTHESE.cpp
DBInterSYNTHESE.hpp
DBModule.cpp
DBModule.gen.cpp
DBModuleRegister.cpp
DBModule.h
DBModule.inc.cpp
DBRecord.cpp
DBRecord.hpp
DBReplaceInterSYNTHESEContent.cpp
DBReplaceInterSYNTHESEContent.cpp
DBResult.cpp
DBResult.hpp
DBSQLInterSYNTHESEContent.cpp
DBSQLInterSYNTHESEContent.cpp
DBTableSync.cpp
DBTableSync.hpp
DBTableSyncTemplate.hpp
DBTransaction.cpp
DBTransaction.hpp
DBTypes.h
DeleteQuery.hpp
Fetcher.h
FetcherTemplate.h
FullSynchronizationPolicy.hpp
InheritanceLoadSavePolicy.hpp
LinkException.h
LoadException.h
RankUpdateQuery.hpp
NoSynchronizationPolicy.hpp
ObjectCreateAction.cpp
ObjectCreateAction.hpp
ObjectCreateService.cpp
ObjectCreateService.hpp
ObjectListService.cpp
ObjectListService.hpp
ObjectUpdateAction.cpp
ObjectUpdateAction.hpp
ObjectViewService.cpp
ObjectViewService.hpp
OldLoadSavePolicy.hpp
RemoveObjectAction.cpp
RemoveObjectAction.hpp
ReplaceQuery.h
SelectQuery.hpp
spatial_ref_sys_sql.inc.h
StandardLoadSavePolicy.hpp
SQLExpression.cpp
SQLExpression.hpp
SQLService.cpp
SQLService.hpp
SQLSingleOperatorExpression.cpp
SQLSingleOperatorExpression.hpp
TableOrObject.cpp
TableOrObject.hpp
TablesOrObjectsVectorField.hpp
TablesViewService.cpp
TablesViewService.hpp
UpdateQuery.cpp
UpdateQuery.hpp
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
  19_inter_synthese
  ${SPATIALITE_LIBRARIES}
)

if(WITH_MYSQL)
  add_subdirectory(102_mysql)
  target_link_libraries(10_db 10_db_mysql)
endif(WITH_MYSQL)

add_subdirectory(101_sqlite)
target_link_libraries(10_db 10_db_sqlite)

add_subdirectory(103_svn)

install(TARGETS 10_db DESTINATION lib)

