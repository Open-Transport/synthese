
set(graph_SRCS
AccessParameters.h
AllowedUseRule.cpp
AllowedUseRule.h
Edge.cpp
Edge.h
ForbiddenUseRule.cpp
ForbiddenUseRule.h
GraphConstants.h
GraphModule.h
GraphModuleTemplate.h
GraphTypes.h
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

add_library(18_graph ${graph_SRCS})

install(TARGETS 18_graph DESTINATION lib)
