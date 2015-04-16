
#include "PTJourneyPlannerService.hpp"
#include "PTRoutePlannerInputFunction.hpp"
#include "RoutePlannerFunction.h"

#include "PTRoutePlannerModule.inc.cpp"

void synthese::pt_journey_planner::moduleRegister()
{
	synthese::pt_journey_planner::PTJourneyPlannerService::integrate();
	synthese::pt_journey_planner::PTRoutePlannerInputFunction::integrate();
	synthese::pt_journey_planner::RoutePlannerFunction::integrate();
}
