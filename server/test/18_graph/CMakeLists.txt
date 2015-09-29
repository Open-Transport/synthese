include_directories(${PROJ_INCLUDE_DIRS})
include_directories(${GEOS_INCLUDE_DIRS})

include_directories("${PROJECT_SOURCE_DIR}/src/00_framework")
include_directories("${PROJECT_SOURCE_DIR}/src/01_util")
include_directories("${PROJECT_SOURCE_DIR}/src/18_graph")

set(DEPS
  59_road_journey_planner # from 56_pt_website
  56_pt_website # from cms
  11_cms # from graph > server
  54_departure_boards
  18_graph
  30_fare
  #14_admin # from server
  54_departure_boards
  37_pt_operation
)

boost_test(VertexAccessMap "${DEPS}" "FakeGraphImplementation.hpp")
boost_test(Hub "${DEPS}" "FakeGraphImplementation.hpp")
boost_test(Journey "${DEPS}" "FakeGraphImplementation.hpp")
boost_test(Path "${DEPS}" "FakeGraphImplementation.hpp")
