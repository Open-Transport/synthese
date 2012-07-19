set_source_groups()

set(road_journey_planner_SRCS
RoadJourneyPlanner.cpp
RoadJourneyPlanner.h
RoadJourneyPlannerAdmin.cpp
RoadJourneyPlannerAdmin.h
RoadJourneyPlannerModule.cpp
RoadJourneyPlannerModule.gen.cpp
RoadJourneyPlannerModuleRegister.cpp
RoadJourneyPlannerModule.hpp
RoadJourneyPlannerModule.inc.cpp
RoadJourneyPlannerResult.cpp
RoadJourneyPlannerResult.h
RoadJourneyPlannerService.cpp
RoadJourneyPlannerService.hpp
)

set_source_files_properties(RoadJourneyPlannerModule.gen.cpp RoadJourneyPlannerModule.inc.cpp PROPERTIES HEADER_FILE_ONLY 1)

add_library(59_road_journey_planner ${road_journey_planner_SRCS})

target_link_libraries(59_road_journey_planner
  14_admin
  33_algorithm
  34_road
)

install(TARGETS 59_road_journey_planner DESTINATION lib)
