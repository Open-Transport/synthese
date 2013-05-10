set(DEPS
  01_util_threads
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
  37_pt_operation
  61_data_exchange
)

boost_test(Thread "${DEPS}")
