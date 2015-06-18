
// 36 MODULE - Ineo Terminus

// Factories
#include "IneoNotificationChannel.hpp"

#include "IneoTerminusModule.hpp"

#include "IneoTerminusModule.inc.cpp"

void synthese::ineo_terminus::moduleRegister()
{

	// 36 MODULE - Ineo Terminus

	// Factories
	synthese::ineo_terminus::IneoNotificationChannel::integrate();

	synthese::ineo_terminus::IneoTerminusModule::integrate();
}
