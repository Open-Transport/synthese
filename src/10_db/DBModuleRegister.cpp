
#include "DBModule.h"

#include "RemoveObjectAction.hpp"


#include "DBModule.inc.cpp"

void synthese::db::moduleRegister()
{
	
	synthese::db::DBModule::integrate();
	
	synthese::db::RemoveObjectAction::integrate();
}
