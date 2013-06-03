include_directories("${PROJECT_SOURCE_DIR}/src/00_framework")
include_directories("${PROJECT_SOURCE_DIR}/src/01_util")
include_directories("${PROJECT_SOURCE_DIR}/src/10_db")

set(DEPS
  00_framework
  01_util
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
  37_pt_operation
)

boost_test(Log "${DEPS}")
boost_test(ParametersMap "${DEPS}")
boost_test(Registrable "${DEPS}")
boost_test(UId "${DEPS}")

add_subdirectory(iostreams)
add_subdirectory(threads)
