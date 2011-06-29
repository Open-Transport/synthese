
// 20 Tree Module includes

// Factories

//#include "TreeFolderTableSync.hpp"

#include "TreeModule.h"

// Registries
//#include "TreeFolderRoot.hpp"


#include "TreeModule.inc.cpp"

void synthese::tree::moduleRegister()
{
	
	// 20 Tree Module
	
	// Factories
	synthese::tree::TreeModule::integrate();
	
	//synthese::tree::TreeFolderTableSync::integrate();
	
	
	// Registries
	//synthese::util::Env::Integrate<synthese::tree::TreeFolderRoot>();
}
