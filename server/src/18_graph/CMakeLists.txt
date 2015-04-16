set_source_groups()

set(graph_SRCS
AccessParameters.h
AllowedUseRule.cpp
AllowedUseRule.h
ChronologicalServicesCollection.cpp
ChronologicalServicesCollection.hpp
Edge.cpp
Edge.h
ForbiddenUseRule.cpp
ForbiddenUseRule.h
GraphConstants.h
GraphModule.gen.cpp
GraphModuleRegister.cpp
GraphModule.h
GraphModule.inc.cpp
GraphModuleTemplate.h
GraphTypes.h
Hub.cpp
Hub.h
HubPoint.hpp
Journey.cpp
Journey.h
Path.cpp
Path.h
PathClass.h
PathGroup.cpp
PathGroup.h
PermanentService.cpp
PermanentService.h
RuleUser.cpp
RuleUser.h
RuleUserUpdateAction.cpp
RuleUserUpdateAction.hpp
Service.cpp
Service.h
ServicePointer.cpp
ServicePointer.h
UseRule.cpp
UseRule.h
Vertex.cpp
Vertex.h
VertexAccessMap.cpp
VertexAccessMap.h
)

set_source_files_properties(GraphModule.gen.cpp GraphModule.inc.cpp PROPERTIES HEADER_FILE_ONLY 1)

add_library(18_graph ${graph_SRCS})

target_link_libraries(18_graph
  01_util
  15_server
)

install(TARGETS 18_graph DESTINATION lib)
