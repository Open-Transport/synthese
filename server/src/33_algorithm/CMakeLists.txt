set_source_groups()

set(algorithm_SRCS
AlgorithmLogger.cpp
AlgorithmLogger.hpp
AlgorithmModule.cpp
AlgorithmModule.gen.cpp
AlgorithmModuleRegister.cpp
AlgorithmModule.h
AlgorithmModule.inc.cpp
AlgorithmTypes.h
AStarShortestPathCalculator.cpp
AStarShortestPathCalculator.hpp
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
RoutePlanningIntermediateJourney.cpp
RoutePlanningIntermediateJourney.hpp
RoutePlanningUserPrefPanel.cpp
RoutePlanningUserPrefPanel.h
TimeSlotRoutePlanner.cpp
TimeSlotRoutePlanner.h
TransferPlaceCheck.cpp
TransferPlaceCheck.hpp
VAMConverter.cpp
VAMConverter.hpp
)

set_source_files_properties(AlgorithmModule.gen.cpp AlgorithmModule.inc.cpp PROPERTIES HEADER_FILE_ONLY 1)

add_library(33_algorithm ${algorithm_SRCS})

install(TARGETS 33_algorithm DESTINATION lib)
