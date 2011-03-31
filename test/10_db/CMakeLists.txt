include_directories("${PROJECT_SOURCE_DIR}/src")

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

set(TESTS_ENV "SYNTHESE_MYSQL_PARAMS=${SYNTHESE_MYSQL_PARAMS}")

boost_test(ConnectionInfo "${DEPS}")
boost_test(DBIndexUpdate "${DEPS}" "DBTestUtils.inc.hpp")
boost_test(DBRegistryTableSync "${DEPS}" "DBTestUtils.inc.hpp;TestTableSync.inc.hpp")
boost_test(DBSchemaUpdate "${DEPS}" "DBTestUtils.inc.hpp")
boost_test(DBTypes "${DEPS}" "DBTestUtils.inc.hpp")
if(WITH_MYSQL)
  include_directories(${MYSQL_INCLUDE_DIR})
  boost_test(MySQLTrigger "${DEPS}" "DBTestUtils.inc.hpp;TestTableSync.inc.hpp")
endif(WITH_MYSQL)

unset(TESTS_ENV)
