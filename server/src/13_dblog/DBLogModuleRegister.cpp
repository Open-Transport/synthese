
#include "DBLogModule.h"

#include "DBLogAdmin.h"

#include "DBLogRight.h"

#include "DBLogEntryTableSync.h"

#include "DBLogPurgeAction.h"

#include "DBLogViewService.hpp"

// Registries
#include "DBLogEntry.h"


#include "DBLogModule.inc.cpp"

void synthese::dblog::moduleRegister()
{

	synthese::dblog::DBLogModule::integrate();

	synthese::dblog::DBLogAdmin::integrate();

	synthese::dblog::DBLogRight::integrate();

	synthese::dblog::DBLogEntryTableSync::integrate();
	synthese::dblog::DBLogPurgeAction::integrate();

	synthese::dblog::DBLogViewService::integrate();

	// Registries
	INTEGRATE(synthese::dblog::DBLogEntry);
}
