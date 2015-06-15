
// 60 Analysis module

// Factories
#include "AnalysisModule.hpp"

#include "AnalysisRight.hpp"

#include "IsochronAdmin.hpp"
#include "PTQualityControlAdmin.hpp"

#include "VehicleServiceUsageTableSync.hpp"

#include "VehicleServiceUsagesListService.hpp"

#include "GenerateVehicleServiceUsagesAction.hpp"

#include "CheckLineCalendarFunction.hpp"
#include "IsochronService.hpp"
#include "ServiceLengthService.hpp"
#include "ServiceDensityService.hpp"

//#include "OpeningTimeTableSync.hpp"
//#include "TrafficForecastTableSync.hpp"

// Registries

#include "AnalysisModule.inc.cpp"

void synthese::analysis::moduleRegister()
{

	// 60 Analysis Module

	// Factories
	synthese::analysis::AnalysisModule::integrate();

	synthese::analysis::AnalysisRight::integrate();

	synthese::analysis::IsochronAdmin::integrate();
	synthese::analysis::PTQualityControlAdmin::integrate();

	synthese::analysis::VehicleServiceUsageTableSync::integrate();

	synthese::analysis::GenerateVehicleServiceUsagesAction::integrate();

	synthese::analysis::CheckLineCalendarFunction::integrate();
	synthese::analysis::IsochronService::integrate();
	synthese::analysis::ServiceLengthService::integrate();
	synthese::analysis::ServiceDensityService::integrate();
	synthese::analysis::VehicleServiceUsagesListService::integrate();

	INTEGRATE(synthese::analysis::VehicleServiceUsage);

//	synthese::analysis::OpeningTimeTableSync::integrate();
//	synthese::analysis::TrafficForecastTableSync::integrate();

//	INTEGRATE(synthese::analysis::OpeningTime);
//	INTEGRATE(synthese::analysis::TrafficForecast);
}
