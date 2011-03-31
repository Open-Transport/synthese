
set(geography_SRCS
City.cpp
City.h
CityAddAction.cpp
CityAddAction.h
CityAlias.cpp
CityAlias.hpp
CityAliasTableSync.cpp
CityAliasTableSync.hpp
CityNameValueInterfaceElement.cpp
CityNameValueInterfaceElement.h
CityTableSync.cpp
CityTableSync.h
CityUpdateAction.cpp
CityUpdateAction.h
GeographyModule.cpp
GeographyModule.h
HTMLMap.cpp
HTMLMap.hpp
IncludingPlace.cpp
IncludingPlace.h
NamedPlace.cpp
NamedPlace.h
NamedPlaceTemplate.h
Place.cpp
Place.h
PlaceAlias.cpp
PlaceAlias.h
PlaceAliasTableSync.cpp
PlaceAliasTableSync.h
)

add_library(32_geography ${geography_SRCS})

install(TARGETS 32_geography DESTINATION lib)
