include_directories(${PROJ_INCLUDE_DIRS})
include_directories(${GEOS_INCLUDE_DIRS})
include_directories(${EXPAT_INCLUDE_DIRS})

include_directories("${PROJECT_SOURCE_DIR}/src/00_framework")
include_directories("${PROJECT_SOURCE_DIR}/src/01_util")
include_directories("${PROJECT_SOURCE_DIR}/src/10_db")
include_directories("${PROJECT_SOURCE_DIR}/src/15_server")
include_directories("${PROJECT_SOURCE_DIR}/src/16_impex")
include_directories("${PROJECT_SOURCE_DIR}/src/17_messages")

set(DEPS
  11_cms
  15_server
  17_messages
  35_pt
  37_pt_operation
  38_vehicle
  54_departure_boards
  56_pt_website
  59_road_journey_planner
  61_data_exchange
)

boost_test(MessageApplicationPeriod "${DEPS}")
boost_test(ScenarioAutopilot "${DEPS}")
