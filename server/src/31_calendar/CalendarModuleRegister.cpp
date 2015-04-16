
#include "CalendarLinkTableSync.hpp"
#include "CalendarTemplateTableSync.h"
#include "CalendarTemplateElementTableSync.h"

#include "CalendarTemplateAdmin.h"
#include "CalendarTemplatesAdmin.h"

#include "CalendarLinkUpdateAction.hpp"
#include "CalendarTemplateElementAddAction.h"
#include "CalendarTemplatePropertiesUpdateAction.h"
#include "CalendarTemplateCleanAction.hpp"

#include "CalendarTemplate.h"
#include "CalendarTemplateElement.h"
#include "CalendarLink.hpp"

#include "CalendarModule.h"
#include "CalendarRight.h"

#include "CalendarsListService.hpp"
#include "CalendarTemplatesListFunction.hpp"


#include "CalendarModule.inc.cpp"

void synthese::calendar::moduleRegister()
{

	// 19 calendar module

	// Factories

	synthese::calendar::CalendarTemplateTableSync::integrate();
	synthese::calendar::CalendarTemplateElementTableSync::integrate();
	synthese::calendar::CalendarLinkTableSync::integrate();

	synthese::calendar::CalendarTemplateAdmin::integrate();
	synthese::calendar::CalendarTemplatesAdmin::integrate();

	synthese::calendar::CalendarLinkUpdateAction::integrate();
	synthese::calendar::CalendarTemplateElementAddAction::integrate();
	synthese::calendar::CalendarTemplatePropertiesUpdateAction::integrate();
	synthese::calendar::CalendarTemplateCleanAction::integrate();

	synthese::calendar::CalendarModule::integrate();
	synthese::calendar::CalendarRight::integrate();

	synthese::calendar::CalendarsListService::integrate();
	synthese::calendar::CalendarTemplatesListFunction::integrate();

	// Registries
	synthese::util::Env::Integrate<synthese::calendar::CalendarLink>();
	synthese::util::Env::Integrate<synthese::calendar::CalendarTemplateElement>();
	synthese::util::Env::Integrate<synthese::calendar::CalendarTemplate>();
}
