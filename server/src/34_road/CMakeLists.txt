set_source_groups()

set(road_SRCS
Address.cpp
Address.h
AddressField.cpp
AddressField.hpp
Crossing.cpp
Crossing.h
CrossingTableSync.cpp
CrossingTableSync.hpp
House.cpp
House.hpp
HouseTableSync.cpp
HouseTableSync.hpp
ProjectAddressAction.cpp
ProjectAddressAction.hpp
PublicPlace.cpp
PublicPlace.h
PublicPlaceAdmin.cpp
PublicPlaceAdmin.hpp
PublicPlaceEntrance.cpp
PublicPlaceEntrance.hpp
PublicPlaceEntranceCreationAction.cpp
PublicPlaceEntranceCreationAction.hpp
PublicPlaceEntranceTableSync.cpp
PublicPlaceEntranceTableSync.hpp
PublicPlacesListService.cpp
PublicPlacesListService.hpp
PublicPlaceTableSync.cpp
PublicPlaceTableSync.h
ProjectAllPublicPlacesAction.cpp
ProjectAllPublicPlacesAction.hpp
ReachableFromCrossing.hpp
Road.cpp
Road.h
RoadChunk.cpp
RoadChunk.h
RoadChunkEdge.cpp
RoadChunkEdge.hpp
RoadChunkTableSync.cpp
RoadChunkTableSync.h
RoadModule.cpp
RoadModule.gen.cpp
RoadModuleRegister.cpp
RoadModule.h
RoadModule.inc.cpp
RoadPath.cpp
RoadPath.cpp
RoadPlace.cpp
RoadPlace.h
RoadPlaceTableSync.cpp
RoadPlaceTableSync.h
RoadTableSync.cpp
RoadTableSync.h
RoadTypes.hpp
RoadPlaceUpdateAction.cpp
RoadPlaceUpdateAction.h
)

set_source_files_properties(RoadModule.gen.cpp RoadModule.inc.cpp PROPERTIES HEADER_FILE_ONLY 1)

include_directories(${EXPAT_INCLUDE_DIRS})

add_library(34_road ${road_SRCS})

target_link_libraries(34_road
  06_openstreetmap
  16_impex
  18_graph
  32_geography
)

install(TARGETS 34_road DESTINATION lib)

install(FILES from_unique_roadplace_to_multi_roadplaces.pl DESTINATION share/synthese)
