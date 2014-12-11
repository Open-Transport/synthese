
// 62 Regulation module

#include "RegulationModule.hpp"
#include "RegulationRight.hpp"
#include "AlertTableSync.hpp"

// Registries
#include "RegulationModule.inc.cpp"


void synthese::regulation::moduleRegister()
{

	// 62 Regulation Module

	synthese::regulation::AlertTableSync::integrate();

    synthese::regulation::RegulationModule::integrate();
	synthese::regulation::RegulationRight::integrate();

	synthese::util::Env::Integrate<synthese::regulation::Alert>();
    
    /*
	synthese::regulation::IsochronAdmin::integrate();
	synthese::regulation::PTQualityControlAdmin::integrate();

	synthese::regulation::VehicleServiceUsageTableSync::integrate();

	synthese::regulation::GenerateVehicleServiceUsagesAction::integrate();

	synthese::regulation::CheckLineCalendarFunction::integrate();
	synthese::regulation::IsochronService::integrate();
	synthese::regulation::ServiceLengthService::integrate();
	synthese::regulation::ServiceDensityService::integrate();
	synthese::regulation::VehicleServiceUsagesListService::integrate();

	synthese::util::Env::Integrate<synthese::regulation::VehicleServiceUsage>();
    */

}
