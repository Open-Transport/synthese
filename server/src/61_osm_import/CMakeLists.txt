set_source_groups()

set(osm_import_SRCS
  OSMEntityHandler.hpp
  OSMLocale.hpp
  OSMLocale.cpp
  OSMParser.hpp
  OSMParser.cpp
)

include_directories(${EXPAT_INCLUDE_DIRS})

add_library(61_osm_import ${osm_import_SRCS})

target_link_libraries(61_osm_import
  ${EXPAT_LIBRARIES}
  ${GEOS_LIBRARIES}
)

install(TARGETS 61_osm_import DESTINATION lib)


