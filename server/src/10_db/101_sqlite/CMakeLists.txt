set_source_groups()

set(db_sqlite_SRCS
SQLiteDB.cpp
SQLiteDB.h
SQLiteException.cpp
SQLiteException.hpp
SQLiteModule.gen.cpp
SQLiteModule.inc.cpp
SQLiteModuleRegister.cpp
SQLiteResult.cpp
SQLiteResult.hpp
SQLiteVirtualTable.cpp
SQLiteVirtualTable.hpp
VirtualDBFVirtualTable.cpp
VirtualDBFVirtualTable.hpp
VirtualShapeVirtualTable.cpp
VirtualShapeVirtualTable.hpp
)

set_source_files_properties(SQLiteModule.gen.cpp SQLiteModule.inc.cpp PROPERTIES HEADER_FILE_ONLY 1)

add_library(10_db_sqlite ${db_sqlite_SRCS})

target_link_libraries(10_db_sqlite
  ${SPATIALITE_LIBRARIES}
)

install(TARGETS 10_db_sqlite DESTINATION lib)
