
// 60 Analysis module

// Factories
#include "AnalysisModule.hpp"

#include "AnalysisRight.hpp"

#include "IsochronAdmin.hpp"

#include "IsochronService.hpp"
#include "ServiceLengthService.hpp"

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

	synthese::analysis::IsochronService::integrate();
	synthese::analysis::ServiceLengthService::integrate();

//	synthese::analysis::OpeningTimeTableSync::integrate();
//	synthese::analysis::TrafficForecastTableSync::integrate();

//	synthese::util::Env::Integrate<synthese::analysis::OpeningTime>();
//	synthese::util::Env::Integrate<synthese::analysis::TrafficForecast>();
}
