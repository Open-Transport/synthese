set_source_groups()

set(db_mysql_SRCS
MySQLDB.cpp
MySQLDB.hpp
MySQLDBModifiedAction.cpp
MySQLDBModifiedAction.hpp
MySQLException.cpp
MySQLException.hpp
MySQLModule.gen.cpp
MySQLModule.inc.cpp
MySQLModuleRegister.cpp
MySQLResult.cpp
MySQLResult.hpp
)

set_source_files_properties(MySQLModule.gen.cpp MySQLModule.inc.cpp PROPERTIES HEADER_FILE_ONLY 1)

include_directories(${MYSQL_INCLUDE_DIR})

add_library(10_db_mysql ${db_mysql_SRCS})

target_link_libraries(10_db_mysql ${MYSQL_LIBRARIES})

if(CURL_FOUND)
  add_subdirectory(trigger)
endif()

install(TARGETS 10_db_mysql DESTINATION lib)
