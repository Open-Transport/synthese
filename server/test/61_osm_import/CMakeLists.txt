include_directories("${PROJECT_SOURCE_DIR}/src/61_osm_import")

set(DEPS
  61_osm_import
)

file(COPY 5_cities.osm DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
file(COPY capendu.osm DESTINATION ${CMAKE_CURRENT_BINARY_DIR})

boost_test(OSMParser "${DEPS}")


