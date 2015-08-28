
// 20 Tree Module includes

// Factories

#include "TreeFolderTableSync.hpp"

#include "TreeFoldersService.hpp"

#include "TreeModule.h"

// Registries
#include "TreeFolder.hpp"


#include "TreeModule.inc.cpp"

void synthese::tree::moduleRegister()
{

	// 20 Tree Module

	// Factories
	synthese::tree::TreeModule::integrate();

	synthese::tree::TreeFoldersService::integrate();

	synthese::tree::TreeFolderTableSync::integrate();

	// Registries
	INTEGRATE(synthese::tree::TreeFolder);
}
