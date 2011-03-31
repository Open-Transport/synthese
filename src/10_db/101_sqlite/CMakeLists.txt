
set(db_sqlite_SRCS
SQLiteDB.cpp
SQLiteDB.h
SQLiteException.cpp
SQLiteException.hpp
SQLiteResult.cpp
SQLiteResult.hpp
)

add_library(10_db_sqlite ${db_sqlite_SRCS})

install(TARGETS 10_db_sqlite DESTINATION lib)
