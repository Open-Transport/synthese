// 14 Admin

#include "HomeAdmin.h"
#include "ModuleAdmin.h"

#include "AdminPageDisplayService.hpp"

#include "AdminModule.inc.cpp"

void synthese::admin::moduleRegister()
{
	// 14 Admin

	synthese::admin::HomeAdmin::integrate();
	synthese::admin::ModuleAdmin::integrate();

	synthese::admin::AdminPageDisplayService::integrate();
}
