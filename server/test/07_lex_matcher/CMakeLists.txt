
include_directories("${PROJECT_SOURCE_DIR}/src/00_framework")
include_directories("${PROJECT_SOURCE_DIR}/src/01_util")

set(DEPS
  07_lexical_matcher
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
  37_pt_operation
  54_departure_boards
  61_data_exchange
  65_public_biking
)

boost_test(LexicalMatcher "${DEPS}")
