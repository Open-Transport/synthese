
#include "SCOMModule.h"

#include "SCOMModule.inc.cpp"

// This file will integrate the module and any integrable object into to server.
// Registries specific to a module should also be integrated here.
void synthese::scom::moduleRegister()
{
	synthese::scom::SCOMModule::integrate();
}
