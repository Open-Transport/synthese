set(DEPS
  01_util_iostreams
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


# FIXME: wants boost-zlib on windows, which isn't there yet.
if(NOT WIN32)
  boost_test(Compression "${DEPS}")
endif()
