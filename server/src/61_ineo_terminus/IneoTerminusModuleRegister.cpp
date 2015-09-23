
// 61 MODULE - Ineo Terminus

// Factories
#include "IneoNotificationChannel.hpp"
#include "IneoTerminusSynchronizeAction.hpp"
#include "IneoTerminusModule.hpp"
#include "IneoTerminusLog.hpp"
#include "IneoTerminusModule.inc.cpp"

void synthese::ineo_terminus::moduleRegister()
{
	// 61 MODULE - Ineo Terminus

	// Factories
	synthese::ineo_terminus::IneoNotificationChannel::integrate();
	synthese::ineo_terminus::IneoTerminusLog::integrate();
	synthese::ineo_terminus::IneoTerminusSynchronizeAction::integrate();
	synthese::ineo_terminus::IneoTerminusModule::integrate();
}
