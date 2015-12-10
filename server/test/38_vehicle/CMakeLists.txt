include_directories(${SPATIALITE_INCLUDE_DIRS})
include_directories(${PROJ_INCLUDE_DIRS})
include_directories(${GEOS_INCLUDE_DIRS})

include_directories("${PROJECT_SOURCE_DIR}/src/00_framework")
include_directories("${PROJECT_SOURCE_DIR}/src/01_util")
include_directories("${PROJECT_SOURCE_DIR}/src/05_html")
include_directories("${PROJECT_SOURCE_DIR}/src/07_lexical_matcher")
include_directories("${PROJECT_SOURCE_DIR}/src/10_db")
include_directories("${PROJECT_SOURCE_DIR}/src/12_security")
include_directories("${PROJECT_SOURCE_DIR}/src/14_admin")
include_directories("${PROJECT_SOURCE_DIR}/src/15_server")
include_directories("${PROJECT_SOURCE_DIR}/src/16_impex")
include_directories("${PROJECT_SOURCE_DIR}/src/17_messages")
include_directories("${PROJECT_SOURCE_DIR}/src/18_graph")
include_directories("${PROJECT_SOURCE_DIR}/src/19_inter_synthese")
include_directories("${PROJECT_SOURCE_DIR}/src/20_tree")
include_directories("${PROJECT_SOURCE_DIR}/src/31_calendar")
include_directories("${PROJECT_SOURCE_DIR}/src/32_geography")
include_directories("${PROJECT_SOURCE_DIR}/src/33_algorithm")
include_directories("${PROJECT_SOURCE_DIR}/src/34_road")
include_directories("${PROJECT_SOURCE_DIR}/src/35_pt")
include_directories("${PROJECT_SOURCE_DIR}/src/37_pt_operation")
include_directories("${PROJECT_SOURCE_DIR}/src/38_vehicle")

set(DEPS
  20_tree
  59_road_journey_planner # from 56_pt_website
  56_pt_website # from cms
  54_departure_boards
  11_cms # from pt
  35_pt
  37_pt_operation
  54_departure_boards
  61_data_exchange
  65_public_biking
  38_vehicle
)

boost_test(VehiclePosition "${DEPS}")
