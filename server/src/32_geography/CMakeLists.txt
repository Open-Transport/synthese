set_source_groups()

set(geography_SRCS
City.cpp
City.h
CityAlias.cpp
CityAlias.hpp
CityAliasTableSync.cpp
CityAliasTableSync.hpp
CityTableSync.cpp
CityTableSync.h
GeographyModule.cpp
GeographyModule.gen.cpp
GeographyModuleRegister.cpp
GeographyModule.h
GeographyModule.inc.cpp
GetMapOpenLayersConstructorService.cpp
GetMapOpenLayersConstructorService.hpp
HTMLMap.cpp
HTMLMap.hpp
IncludingPlace.h
MapSource.cpp
MapSource.hpp
MapSourceAdmin.cpp
MapSourceAdmin.hpp
MapSourcesAdmin.cpp
MapSourcesAdmin.hpp
MapSourceTableSync.cpp
MapSourceTableSync.hpp
MapSourceUpdateAction.cpp
MapSourceUpdateAction.hpp
NamedPlace.cpp
NamedPlace.h
NamedPlaceField.cpp
NamedPlaceField.hpp
NamedPlaceTemplate.h
Place.cpp
Place.h
PlaceAlias.cpp
PlaceAlias.h
PlaceAliasTableSync.cpp
PlaceAliasTableSync.h
)

set_source_files_properties(GeographyModule.gen.cpp GeographyModule.inc.cpp PROPERTIES HEADER_FILE_ONLY 1)

add_library(32_geography ${geography_SRCS})

target_link_libraries(32_geography
  07_lexical_matcher
  18_graph
  00_framework
  proj
)

install(TARGETS 32_geography DESTINATION lib)
