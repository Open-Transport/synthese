include_directories(${SPATIALITE_INCLUDE_DIRS})
include_directories(${PROJ_INCLUDE_DIRS})
include_directories(${GEOS_INCLUDE_DIRS})
include_directories(${EXPAT_INCLUDE_DIRS})

include_directories("${PROJECT_SOURCE_DIR}/src/00_framework")
include_directories("${PROJECT_SOURCE_DIR}/src/01_util")
include_directories("${PROJECT_SOURCE_DIR}/src/05_html")
include_directories("${PROJECT_SOURCE_DIR}/src/07_lexical_matcher")
include_directories("${PROJECT_SOURCE_DIR}/src/10_db")
include_directories("${PROJECT_SOURCE_DIR}/src/11_cms")
include_directories("${PROJECT_SOURCE_DIR}/src/12_security")
include_directories("${PROJECT_SOURCE_DIR}/src/14_admin")
include_directories("${PROJECT_SOURCE_DIR}/src/15_server")
include_directories("${PROJECT_SOURCE_DIR}/src/16_impex")
include_directories("${PROJECT_SOURCE_DIR}/src/17_messages")
include_directories("${PROJECT_SOURCE_DIR}/src/18_graph")
include_directories("${PROJECT_SOURCE_DIR}/src/19_inter_synthese")
include_directories("${PROJECT_SOURCE_DIR}/src/20_tree")
include_directories("${PROJECT_SOURCE_DIR}/src/30_fare")
include_directories("${PROJECT_SOURCE_DIR}/src/31_calendar")
include_directories("${PROJECT_SOURCE_DIR}/src/32_geography")
include_directories("${PROJECT_SOURCE_DIR}/src/33_algorithm")
include_directories("${PROJECT_SOURCE_DIR}/src/34_road")
include_directories("${PROJECT_SOURCE_DIR}/src/35_pt")
include_directories("${PROJECT_SOURCE_DIR}/src/37_pt_operation")
include_directories("${PROJECT_SOURCE_DIR}/src/38_vehicle")
include_directories("${PROJECT_SOURCE_DIR}/src/56_pt_website")
include_directories("${PROJECT_SOURCE_DIR}/src/61_data_exchange")

set(DEPS
  20_tree
  59_road_journey_planner # from 56_pt_website
  56_pt_website # from cms
  11_cms # from pt
  35_pt
  37_pt_operation
  54_departure_boards
  61_data_exchange
)

boost_test(OsmFileFormat "expat ${GEOS_LIBRARIES}")
add_definitions(-DPEGASE_TEST_SQL="${CMAKE_CURRENT_SOURCE_DIR}/test_data/pegase_test.sql")
boost_test(PegaseFileFormat "${DEPS}")
add_definitions(-DINEO_FILE_PATTERN="${CMAKE_CURRENT_SOURCE_DIR}/test_data/ineo")
boost_test(IneoFileFormat "${DEPS}")
