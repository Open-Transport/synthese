
// 62 Regulation module

#include "RegulationModule.hpp"
#include "RegulationRight.hpp"
#include "AlertTableSync.hpp"
#include "AlertListService.hpp"
#include "ProcessAlertsAction.hpp"

// Registries
#include "RegulationModule.inc.cpp"


void synthese::regulation::moduleRegister()
{

	// 62 Regulation Module

	synthese::regulation::AlertTableSync::integrate();

	synthese::regulation::RegulationModule::integrate();
	synthese::regulation::RegulationRight::integrate();

	synthese::regulation::ProcessAlertsAction::integrate();

	synthese::regulation::AlertListService::integrate();

	INTEGRATE(synthese::regulation::Alert);

}
