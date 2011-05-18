include_directories(${SPATIALITE_INCLUDE_DIRS})
include_directories(${PROJ_INCLUDE_DIRS})
include_directories(${GEOS_INCLUDE_DIRS})

include_directories("${PROJECT_SOURCE_DIR}/src/00_framework")
include_directories("${PROJECT_SOURCE_DIR}/src/01_util")
include_directories("${PROJECT_SOURCE_DIR}/src/05_html")
include_directories("${PROJECT_SOURCE_DIR}/src/10_db")
include_directories("${PROJECT_SOURCE_DIR}/src/12_security")
include_directories("${PROJECT_SOURCE_DIR}/src/14_admin")
include_directories("${PROJECT_SOURCE_DIR}/src/15_server")

set(DEPS
  36_cms # from admin
  15_server # from db
  14_admin # from server
  10_db
)

if(SYNTHESE_MYSQL_PARAMS)
  set(TESTS_ENV "SYNTHESE_MYSQL_PARAMS=${SYNTHESE_MYSQL_PARAMS}")
endif()

boost_test(ConnectionInfo "${DEPS}")
boost_test(DBIndexUpdate "${DEPS}" "DBTestUtils.hpp")
boost_test(DBQuery "${DEPS}" "DBTestUtils.hpp;TestTableSync.hpp")
boost_test(DBRegistryTableSync "${DEPS}" "DBTestUtils.hpp;TestTableSync.hpp")
boost_test(DBSchemaUpdate "${DEPS}" "DBTestUtils.hpp")
boost_test(DBTypes "${DEPS}" "DBTestUtils.hpp")
if(WITH_MYSQL)
  include_directories(${MYSQL_INCLUDE_DIR})
  boost_test(MySQLTrigger "${DEPS}" "DBTestUtils.hpp;TestTableSync.hpp")
  boost_test(MySQLReconnect "${DEPS}" "DBTestUtils.hpp;TestTableSync.hpp")
endif(WITH_MYSQL)

unset(TESTS_ENV)
