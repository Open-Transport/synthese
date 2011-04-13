
set(road_SRCS
Address.cpp
Address.h
Crossing.cpp
Crossing.h
CrossingTableSync.cpp
CrossingTableSync.hpp
House.cpp
House.hpp
MainRoadChunk.cpp
MainRoadChunk.hpp
MainRoadPart.cpp
MainRoadPart.hpp
NavstreetsFileFormat.cpp
NavstreetsFileFormat.hpp
PublicPlace.cpp
PublicPlace.h
PublicPlaceTableSync.cpp
PublicPlaceTableSync.h
ReachableFromCrossing.hpp
ReverseRoadChunk.cpp
ReverseRoadChunk.hpp
ReverseRoadPart.cpp
ReverseRoadPart.hpp
Road.cpp
Road.h
RoadChunk.cpp
RoadChunk.h
RoadChunkTableSync.cpp
RoadChunkTableSync.h
RoadModule.cpp
RoadModule.h
RoadPlace.cpp
RoadPlace.h
RoadPlaceTableSync.cpp
RoadPlaceTableSync.h
RoadTableSync.cpp
RoadTableSync.h
)

add_library(34_road ${road_SRCS})

target_link_libraries(34_road
  06_openstreetmap
  16_impex
  18_graph
  32_geography
)

install(TARGETS 34_road DESTINATION lib)
