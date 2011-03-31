
set(db_mysql_SRCS
MySQLDB.cpp
MySQLDB.hpp
MySQLDBModifiedAction.cpp
MySQLDBModifiedAction.hpp
MySQLException.cpp
MySQLException.hpp
MySQLResult.cpp
MySQLResult.hpp
)

include_directories(${MYSQL_INCLUDE_DIR})

add_library(10_db_mysql ${db_mysql_SRCS})

target_link_libraries(10_db_mysql ${MYSQL_LIBRARIES})

add_subdirectory(trigger)

install(TARGETS 10_db_mysql DESTINATION lib)
