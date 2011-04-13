
set(pt_journey_planner_SRCS
PTRoutePlannerInputFunction.cpp
PTRoutePlannerInputFunction.hpp
PTRoutePlannerModule.cpp
PTRoutePlannerModule.h
PTRoutePlannerResult.cpp
PTRoutePlannerResult.h
PTTimeSlotRoutePlanner.cpp
PTTimeSlotRoutePlanner.h
RoutePlannerFunction.cpp
RoutePlannerFunction.h
)

add_library(53_pt_journey_planner ${pt_journey_planner_SRCS})

target_link_libraries(53_pt_journey_planner
  33_algorithm
  35_pt
  # Forward dependencies:
  #56_pt_website
)

install(TARGETS 53_pt_journey_planner DESTINATION lib)
