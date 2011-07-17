include_directories(${PROJ_INCLUDE_DIRS})
include_directories(${GEOS_INCLUDE_DIRS})

include_directories("${PROJECT_SOURCE_DIR}/src/00_framework")
include_directories("${PROJECT_SOURCE_DIR}/src/01_util")
include_directories("${PROJECT_SOURCE_DIR}/src/18_graph")

set(DEPS
  59_road_journey_planner # from 56_pt_website
  56_pt_website # from cms
  36_cms # from graph > server
  18_graph
  #14_admin # from server
)

boost_test(VertexAccessMap "${DEPS}")
