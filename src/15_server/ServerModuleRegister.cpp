
#include "ServerModule.h"
#include "LoginAction.h"
#include "LogoutAction.h"
#include "HtmlFormInterfaceElement.h"
#include "ValidSessionInterfaceElement.h"
#include "HtmlFormCalendarFieldInterfaceElement.h"
#include "HtmlFormFieldInterfaceElement.h"
#include "DbModuleConfigTableSync.h"
#include "LoginInterfacePage.h"
#include "ServerModuleVersionInterfaceElement.h"
#include "ServerAdminRight.h"
#include "ThreadsAdmin.h"
#include "ThreadKillAction.h"
#include "QuitAction.hpp"

#include "XMLSessionFunction.h"
#include "XMLSessionIsValidFunction.h"


#include "ServerModule.inc.cpp"

void synthese::server::moduleRegister()
{
	
	synthese::server::ServerModule::integrate();
	
	synthese::server::LoginInterfacePage::integrate();
	
	synthese::server::LoginAction::integrate();
	synthese::server::LogoutAction::integrate();
	synthese::server::ThreadKillAction::integrate();
	synthese::server::QuitAction::integrate();
	
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
