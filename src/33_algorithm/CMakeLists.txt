
set(algorithm_SRCS
AlgorithmModule.cpp
AlgorithmModule.h
AlgorithmTypes.h
BestVertexReachesMap.cpp
BestVertexReachesMap.h
EdgeProjector.hpp
IntegralSearcher.cpp
IntegralSearcher.h
JourneysResult.cpp
JourneysResult.h
JourneyTemplates.cpp
JourneyTemplates.h
RoutePlanner.cpp
RoutePlanner.h
RoutePlannerLogger.cpp
RoutePlannerLogger.h
RoutePlanningIntermediateJourney.cpp
RoutePlanningIntermediateJourney.hpp
RoutePlanningUserPrefPanel.cpp
RoutePlanningUserPrefPanel.h
TimeSlotRoutePlanner.cpp
TimeSlotRoutePlanner.h
)

add_library(33_algorithm ${algorithm_SRCS})

target_link_libraries(33_algorithm
  17_messages
  # Forward dependencies:
  #34_road
  #35_pt
  #51_resa
)

install(TARGETS 33_algorithm DESTINATION lib)
