
set(openstreetmap_SRCS
AttributeMap.cpp
AttributeMap.h
GeomUtil.cpp
GeomUtil.h
OSMElement.cpp
OSMElements.h
OSMException.cpp
OSMException.h
OSMExpatParser.cpp
OSMExpatParser.h
OSMNetwork.cpp
OSMNode.cpp
OSMRelation.cpp
OSMWay.cpp
)

include_directories(${PROJ_INCLUDE_DIRS})
include_directories(${EXPAT_INCLUDE_DIRS})

add_library(06_openstreetmap ${openstreetmap_SRCS})

target_link_libraries(06_openstreetmap expat)

install(TARGETS 06_openstreetmap DESTINATION lib)
