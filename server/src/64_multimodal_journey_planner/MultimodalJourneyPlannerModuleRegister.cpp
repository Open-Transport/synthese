
#include "MultimodalJourneyPlannerService.hpp"

#include "MultimodalJourneyPlannerModule.inc.cpp"

void synthese::multimodal_journey_planner::moduleRegister()
{
	synthese::multimodal_journey_planner::MultimodalJourneyPlannerService::integrate();
}
