set_source_groups()

set(algorithm_SRCS
AlgorithmModule.cpp
AlgorithmModule.gen.cpp
AlgorithmModuleRegister.cpp
AlgorithmModule.h
AlgorithmModule.inc.cpp
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
PlacesList.hpp
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

set_source_files_properties(AlgorithmModule.gen.cpp AlgorithmModule.inc.cpp PROPERTIES HEADER_FILE_ONLY 1)

add_library(33_algorithm ${algorithm_SRCS})

target_link_libraries(33_algorithm
  17_messages
  # Forward dependencies:
  #34_road
  #35_pt
  #51_resa
)

install(TARGETS 33_algorithm DESTINATION lib)
