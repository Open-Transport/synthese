
#include "DBModule.h"

#include "ObjectUpdateAction.hpp"
#include "RemoveObjectAction.hpp"

#include "DBModule.inc.cpp"

void synthese::db::moduleRegister()
{
	synthese::db::DBModule::integrate();

	synthese::db::ObjectUpdateAction::integrate();
	synthese::db::RemoveObjectAction::integrate();
}
