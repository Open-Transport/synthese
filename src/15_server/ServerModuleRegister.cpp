
#include "ServerModule.h"

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

#include "ThreadsAdmin.h"

#include "DbModuleConfigTableSync.h"

#include "XMLSessionFunction.h"
#include "XMLSessionIsValidFunction.h"

#include "ServerModule.inc.cpp"

void synthese::server::moduleRegister()
{
	
	synthese::server::ServerModule::integrate();
	
	synthese::server::LoginInterfacePage::integrate();
	
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
	
	synthese::server::ThreadsAdmin::integrate();
	
	synthese::server::XMLSessionFunction::integrate();
	synthese::server::XMLSessionIsValidFunction::integrate();
}
