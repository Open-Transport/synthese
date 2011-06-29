// 14 Admin

#include "HomeAdmin.h"
#include "LoginAdmin.hpp"
#include "ModuleAdmin.h"

#include "AdminFunction.h"


#include "AdminModule.inc.cpp"

void synthese::admin::moduleRegister()
{
	// 14 Admin
	
	synthese::admin::HomeAdmin::integrate();
	synthese::admin::ModuleAdmin::integrate();
	synthese::admin::LoginAdmin::integrate();
	
	synthese::admin::AdminFunction::integrate();
}
