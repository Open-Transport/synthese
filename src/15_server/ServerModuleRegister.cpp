
#include "ServerModule.h"

#include "GlobalVariableUpdateAction.hpp"
#include "LoginAction.h"
#include "LogoutAction.h"
#include "QuitAction.hpp"
#include "SetSessionVariableAction.hpp"
#include "ThreadKillAction.h"

#include "HtmlFormInterfaceElement.h"
#include "ValidSessionInterfaceElement.h"
#include "HtmlFormCalendarFieldInterfaceElement.h"
#include "HtmlFormFieldInterfaceElement.h"
#include "ServerModuleVersionInterfaceElement.h"

#include "LoginInterfacePage.h"

#include "ServerAdminRight.h"

#include "MemoryStatisticsAdmin.hpp"
#include "ThreadsAdmin.h"

#include "DbModuleConfigTableSync.h"

#include "ActionService.hpp"
#include "RedirectService.hpp"
#include "SessionService.hpp"
#include "SessionsListService.hpp"
#include "XMLSessionFunction.h"
#include "XMLSessionIsValidFunction.h"

#include "ServerModule.inc.cpp"

void synthese::server::moduleRegister()
{

	synthese::server::ServerModule::integrate();

	synthese::server::LoginInterfacePage::integrate();

	synthese::server::GlobalVariableUpdateAction::integrate();
	synthese::server::LoginAction::integrate();
	synthese::server::LogoutAction::integrate();
	synthese::server::QuitAction::integrate();
	synthese::server::SetSessionVariableAction::integrate();
	synthese::server::ThreadKillAction::integrate();

	synthese::server::HtmlFormInterfaceElement::integrate();
	synthese::server::ValidSessionInterfaceElement::integrate();
	synthese::server::HtmlFormCalendarFieldInterfaceElement::integrate();
	synthese::server::HtmlFormFieldInterfaceElement::integrate();
	synthese::server::ServerModuleVersionInterfaceElement::integrate();

	synthese::server::DbModuleConfigTableSync::integrate();

	synthese::server::ServerAdminRight::integrate();

	synthese::server::MemoryStatisticsAdmin::integrate();
	synthese::server::ThreadsAdmin::integrate();

	synthese::server::ActionService::integrate();
	synthese::server::RedirectService::integrate();
	synthese::server::SessionsListService::integrate();
	synthese::server::SessionService::integrate();
	synthese::server::XMLSessionFunction::integrate();
	synthese::server::XMLSessionIsValidFunction::integrate();
}
