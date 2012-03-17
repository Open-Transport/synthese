
#include "DBModule.h"

#include "ObjectCreateAction.hpp"
#include "ObjectUpdateAction.hpp"
#include "RemoveObjectAction.hpp"

#include "DBModule.inc.cpp"

void synthese::db::moduleRegister()
{
	synthese::db::DBModule::integrate();

	synthese::db::ObjectCreateAction::integrate();
	synthese::db::ObjectUpdateAction::integrate();
	synthese::db::RemoveObjectAction::integrate();
}
