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
include_directories("${PROJECT_SOURCE_DIR}/src/16_impex")
include_directories("${PROJECT_SOURCE_DIR}/src/19_inter_synthese")

set(DEPS
  ${GEOS_LIBRARIES}
  59_road_journey_planner
  56_pt_website
  11_cms
  12_security
  15_server
  16_impex
  54_departure_boards
  10_db
  00_framework
  54_departure_boards
  37_pt_operation
)

if(SYNTHESE_MYSQL_PARAMS)
  set(TESTS_ENV "SYNTHESE_MYSQL_PARAMS=${SYNTHESE_MYSQL_PARAMS}")
endif()

set(DB_TEST_UTILS "../common/TestUtils.hpp;DBTestUtils.hpp")

boost_test(ConnectionInfo "${DEPS}")
boost_test(DBIndexUpdate "${DEPS}" "${DB_TEST_UTILS}")
boost_test(DBQuery "${DEPS}" "${DB_TEST_UTILS};TestTableSync.hpp")
boost_test(DBRegistryTableSync "${DEPS}" "${DB_TEST_UTILS};TestTableSync.hpp")
boost_test(DBSchemaUpdate "${DEPS}" "${DB_TEST_UTILS}")
boost_test(DBTypes "${DEPS}" "${DB_TEST_UTILS}")
if(WITH_MYSQL)
  include_directories(${MYSQL_INCLUDE_DIR})
  boost_test(MySQLTrigger "${DEPS}" "${DB_TEST_UTILS};TestTableSync.hpp")
  boost_test(MySQLReconnect "${DEPS}" "${DB_TEST_UTILS};TestTableSync.hpp")
endif(WITH_MYSQL)

unset(TESTS_ENV)
