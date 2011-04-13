
set(road_journey_planner_SRCS
RoadJourneyPlanner.cpp
RoadJourneyPlanner.h
RoadJourneyPlannerAdmin.cpp
RoadJourneyPlannerAdmin.h
RoadJourneyPlannerModule.cpp
RoadJourneyPlannerModule.hpp
RoadJourneyPlannerResult.cpp
RoadJourneyPlannerResult.h
)

add_library(59_road_journey_planner ${road_journey_planner_SRCS})

target_link_libraries(59_road_journey_planner
  14_admin
  33_algorithm
  34_road
)

install(TARGETS 59_road_journey_planner DESTINATION lib)
