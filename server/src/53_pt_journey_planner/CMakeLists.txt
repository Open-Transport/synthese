set_source_groups()

set(pt_journey_planner_SRCS
PTJourneyPlannerService.cpp
PTJourneyPlannerService.hpp
PTRoutePlannerInputFunction.cpp
PTRoutePlannerInputFunction.hpp
PTRoutePlannerModule.cpp
PTRoutePlannerModule.gen.cpp
PTRoutePlannerModuleRegister.cpp
PTRoutePlannerModule.h
PTRoutePlannerModule.inc.cpp
PTRoutePlannerResult.cpp
PTRoutePlannerResult.h
PTTimeSlotRoutePlanner.cpp
PTTimeSlotRoutePlanner.h
RoutePlannerFunction.cpp
RoutePlannerFunction.h
)

set_source_files_properties(PTRoutePlannerModule.gen.cpp PTRoutePlannerModule.inc.cpp PROPERTIES HEADER_FILE_ONLY 1)

add_library(53_pt_journey_planner ${pt_journey_planner_SRCS})

target_link_libraries(53_pt_journey_planner
  33_algorithm
  35_pt
  51_resa
  # Forward dependencies:
  #56_pt_website
)

install(TARGETS 53_pt_journey_planner DESTINATION lib)
