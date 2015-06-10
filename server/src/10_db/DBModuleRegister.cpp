
#include "DBModule.h"

#include "ObjectCreateAction.hpp"
#include "ObjectUpdateAction.hpp"
#include "RemoveObjectAction.hpp"

#include "ObjectCreateService.hpp"
#include "ObjectViewService.hpp"
#include "ObjectListService.hpp"
#include "TablesViewService.hpp"
#include "SQLService.hpp"

#include "DBInterSYNTHESE.hpp"

#include "DBModule.inc.cpp"

void synthese::db::moduleRegister()
{
	synthese::db::DBModule::integrate();

	synthese::db::ObjectCreateAction::integrate();
	synthese::db::ObjectUpdateAction::integrate();
	synthese::db::RemoveObjectAction::integrate();

	synthese::db::ObjectCreateService::integrate();
	synthese::db::ObjectViewService::integrate();
	synthese::db::ObjectListService::integrate();
	synthese::db::TablesViewService::integrate();
	synthese::db::SQLService::integrate();

	synthese::db::DBInterSYNTHESE::integrate();
}
