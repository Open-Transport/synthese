
// 58 Hiking module

// Factories
#include "HikingModule.h"

#include "HikingTrailTableSync.h"

#include "HikingRight.h"

#include "HikingTrailAddAction.h"
#include "HikingTrailUpdateAction.h"
#include "HikingTrailStopAddAction.hpp"
#include "HikingTrailStopRemoveAction.hpp"

#include "HikingTrailAdmin.h"
#include "HikingTrailsAdmin.h"

#include "HikingTrailDisplayFunction.h"
#include "HikingTrailSearchFunction.h"

// Registries

#include "HikingTrail.h"


#include "HikingModule.inc.cpp"

void synthese::hiking::moduleRegister()
{

	// 58 Hiking Module

	// Factories
	synthese::hiking::HikingModule::integrate();

	synthese::hiking::HikingTrailTableSync::integrate();

	synthese::hiking::HikingRight::integrate();

	synthese::hiking::HikingTrailDisplayFunction::integrate();
	synthese::hiking::HikingTrailSearchFunction::integrate();

	synthese::hiking::HikingTrailAddAction::integrate();
	synthese::hiking::HikingTrailUpdateAction::integrate();
	synthese::hiking::HikingTrailStopAddAction::integrate();
	synthese::hiking::HikingTrailStopRemoveAction::integrate();

	synthese::hiking::HikingTrailAdmin::integrate();
	synthese::hiking::HikingTrailsAdmin::integrate();

	// Registries

	synthese::util::Env::Integrate<synthese::hiking::HikingTrail>();

}
