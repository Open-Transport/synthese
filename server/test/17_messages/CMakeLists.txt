include_directories(${PROJ_INCLUDE_DIRS})
include_directories(${GEOS_INCLUDE_DIRS})
include_directories(${EXPAT_INCLUDE_DIRS})

include_directories("${PROJECT_SOURCE_DIR}/src/00_framework")
include_directories("${PROJECT_SOURCE_DIR}/src/01_util")
include_directories("${PROJECT_SOURCE_DIR}/src/05_html")
include_directories("${PROJECT_SOURCE_DIR}/src/07_lexical_matcher")
include_directories("${PROJECT_SOURCE_DIR}/src/10_db")
include_directories("${PROJECT_SOURCE_DIR}/src/10_db/103_svn")
include_directories("${PROJECT_SOURCE_DIR}/src/11_cms")
include_directories("${PROJECT_SOURCE_DIR}/src/12_security")
include_directories("${PROJECT_SOURCE_DIR}/src/14_admin")
include_directories("${PROJECT_SOURCE_DIR}/src/15_server")
include_directories("${PROJECT_SOURCE_DIR}/src/16_impex")
include_directories("${PROJECT_SOURCE_DIR}/src/17_messages")
include_directories("${PROJECT_SOURCE_DIR}/src/20_tree")
include_directories("${PROJECT_SOURCE_DIR}/src/31_calendar")
include_directories("${PROJECT_SOURCE_DIR}/src/35_pt")

set(DEPS
  11_cms
  15_server
  17_messages
  31_calendar
  35_pt
  37_pt_operation
  38_vehicle
  54_departure_boards
  56_pt_website
  59_road_journey_planner
  61_data_exchange
)

boost_test(MessagesSection "${DEPS}")