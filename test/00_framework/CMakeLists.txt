include_directories(${PROJ_INCLUDE_DIRS})
include_directories(${GEOS_INCLUDE_DIRS})

include_directories("${PROJECT_SOURCE_DIR}/src/00_framework")
include_directories("${PROJECT_SOURCE_DIR}/src/01_util")

set(DEPS
  01_util
  00_framework
  10_db
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
  61_data_exchange
)

boost_test(Object "${DEPS}")
