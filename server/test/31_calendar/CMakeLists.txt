include_directories(${PROJ_INCLUDE_DIRS})
include_directories(${GEOS_INCLUDE_DIRS})

include_directories("${PROJECT_SOURCE_DIR}/src/00_framework")
include_directories("${PROJECT_SOURCE_DIR}/src/01_util")
include_directories("${PROJECT_SOURCE_DIR}/src/10_db")
include_directories("${PROJECT_SOURCE_DIR}/src/15_server")
include_directories("${PROJECT_SOURCE_DIR}/src/16_impex")
include_directories("${PROJECT_SOURCE_DIR}/src/20_tree")
include_directories("${PROJECT_SOURCE_DIR}/src/31_calendar")

set(DEPS
  15_server # from db
  31_calendar
  11_cms # from admin
  54_departure_boards
  56_pt_website # from cms
  59_road_journey_planner # from 56_pt_website
  54_departure_boards
  61_data_exchange
  65_public_biking
  37_pt_operation
)

boost_test(Calendar "${DEPS}")
