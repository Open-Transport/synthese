include_directories("${PROJECT_SOURCE_DIR}/src/00_framework")
include_directories("${PROJECT_SOURCE_DIR}/src/01_util")
include_directories("${PROJECT_SOURCE_DIR}/src/18_graph")
include_directories("${PROJECT_SOURCE_DIR}/src/34_road")
include_directories("${PROJECT_SOURCE_DIR}/src/61_osm_import")
include_directories(${GEOS_INCLUDE_DIRS})
include_directories(${PROJ_INCLUDE_DIRS})

set(DEPS
  61_osm_import
  00_framework
  01_util
  10_db
  11_cms
  15_server
)

file(COPY five_swiss_cities_with_incomplete_boundaries.osm DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
file(COPY ten_french_cities_only_one_with_complete_boundary.osm DESTINATION ${CMAKE_CURRENT_BINARY_DIR})

boost_test(OSMParser "${DEPS}")


