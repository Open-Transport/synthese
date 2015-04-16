
// 59 Road journey planner

#include "RoadJourneyPlannerModule.hpp"

#include "RoadJourneyPlannerAdmin.h"
#include "RoadJourneyPlannerService.hpp"

#include "RoadJourneyPlannerModule.inc.cpp"

void synthese::road_journey_planner::moduleRegister()
{

	// 59 Road journey planner

	synthese::road_journey_planner::RoadJourneyPlannerModule::integrate();

	synthese::road_journey_planner::RoadJourneyPlannerAdmin::integrate();

	synthese::road_journey_planner::RoadJourneyPlannerService::integrate();
}
