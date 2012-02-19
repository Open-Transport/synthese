
// 60 Analysis module

// Factories
#include "AnalysisModule.hpp"

#include "AnalysisRight.hpp"

#include "IsochronAdmin.hpp"

// Registries

#include "AnalysisModule.inc.cpp"

void synthese::analysis::moduleRegister()
{

	// 60 Analysis Module

	// Factories
	synthese::analysis::AnalysisModule::integrate();

	synthese::analysis::AnalysisRight::integrate();

	synthese::analysis::IsochronAdmin::integrate();

}
