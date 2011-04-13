include_directories(${PROJ_INCLUDE_DIRS})
include_directories(${GEOS_INCLUDE_DIRS})

include_directories("${PROJECT_SOURCE_DIR}/src/00_framework")
include_directories("${PROJECT_SOURCE_DIR}/src/01_util")
include_directories("${PROJECT_SOURCE_DIR}/src/16_impex")
include_directories("${PROJECT_SOURCE_DIR}/src/18_graph")
include_directories("${PROJECT_SOURCE_DIR}/src/32_geography")
include_directories("${PROJECT_SOURCE_DIR}/src/34_road")

set(DEPS
  36_cms # from road > graph > server
  34_road
)

boost_test(Road "${DEPS}")
