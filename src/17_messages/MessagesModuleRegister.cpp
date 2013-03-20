
#include "MessagesModule.h"

#include "AlarmObjectLinkTableSync.h"
#include "AlarmTableSync.h"
#include "MessageAlternativeTableSync.hpp"
#include "MessageApplicationPeriodTableSync.hpp"
#include "MessageTypeTableSync.hpp"
#include "ScenarioTableSync.h"
#include "ScenarioFolderTableSync.h"
#include "TextTemplateTableSync.h"

#include "MessagesAdmin.h"
#include "MessageAdmin.h"
#include "MessagesScenarioAdmin.h"
#include "MessagesLibraryAdmin.h"
#include "MessagesTemplateLibraryAdmin.h"

#include "MessagesRight.h"
#include "MessagesLibraryRight.h"

#include "MessagesLog.h"
#include "MessagesLibraryLog.h"

#include "GetMessagesFunction.hpp"
#include "MessageTypesService.hpp"
#include "ScenariosListFunction.hpp"
#include "ScenarioDisplayFunction.hpp"

#include "ScenarioSaveAction.h"
#include "SimpleMessageCreationAction.hpp"
#include "UpdateAlarmMessagesFromTemplateAction.h"
#include "NewMessageAction.h"
#include "UpdateTextTemplateAction.h"
#include "ScenarioStopAction.h"
#include "AddScenarioAction.h"
#include "TextTemplateAddAction.h"
#include "UpdateAlarmMessagesAction.h"
#include "AlarmRemoveLinkAction.h"
#include "AlarmAddLinkAction.h"
#include "ScenarioStopAction.h"
#include "ScenarioFolderAdd.h"
#include "ScenarioFolderUpdateAction.h"

// Registries
#include "AlarmObjectLink.h"
#include "MessageAlternative.hpp"
#include "MessagesTypes.h"
#include "Alarm.h"
#include "TextTemplate.h"
#include "ScenarioFolder.h"
#include "Scenario.h"


#include "MessagesModule.inc.cpp"

void synthese::messages::moduleRegister()
{

	synthese::messages::MessagesModule::integrate();

	synthese::messages::AlarmObjectLinkTableSync::integrate();
	synthese::messages::AlarmTableSync::integrate();
	synthese::messages::MessageAlternativeTableSync::integrate();
	synthese::messages::MessageApplicationPeriodTableSync::integrate();
	synthese::messages::MessageTypeTableSync::integrate();
	synthese::messages::ScenarioFolderTableSync::integrate();
	synthese::messages::ScenarioTableSync::integrate();
	synthese::messages::TextTemplateTableSync::integrate();

	synthese::messages::MessagesAdmin::integrate();
	synthese::messages::MessageAdmin::integrate();
	synthese::messages::MessagesLibraryAdmin::integrate();
	synthese::messages::MessagesTemplateLibraryAdmin::integrate();
	synthese::messages::MessagesScenarioAdmin::integrate();

	synthese::messages::MessagesRight::integrate();
	synthese::messages::MessagesLibraryRight::integrate();

	synthese::messages::MessagesLog::integrate();
	synthese::messages::MessagesLibraryLog::integrate();

	synthese::messages::GetMessagesFunction::integrate();
	synthese::messages::MessageTypesService::integrate();
	synthese::messages::ScenariosListFunction::integrate();
	synthese::messages::ScenarioDisplayFunction::integrate();

	synthese::messages::ScenarioSaveAction::integrate();
	synthese::messages::SimpleMessageCreationAction::integrate();
	synthese::messages::ScenarioStopAction::integrate();
	synthese::messages::UpdateAlarmMessagesFromTemplateAction::integrate();
	synthese::messages::NewMessageAction::integrate();
	synthese::messages::UpdateTextTemplateAction::integrate();
	synthese::messages::AddScenarioAction::integrate();
	synthese::messages::TextTemplateAddAction::integrate();
	synthese::messages::UpdateAlarmMessagesAction::integrate();
	synthese::messages::AlarmRemoveLinkAction::integrate();
	synthese::messages::AlarmAddLinkAction::integrate();
	synthese::messages::ScenarioFolderAdd::integrate();
	synthese::messages::ScenarioFolderUpdateAction::integrate();

	// Registries
	synthese::util::Env::Integrate<synthese::messages::AlarmObjectLink>();
	synthese::util::Env::Integrate<synthese::messages::MessageAlternative>();
	synthese::util::Env::Integrate<synthese::messages::MessageApplicationPeriod>();
	synthese::util::Env::Integrate<synthese::messages::MessageType>();
	synthese::util::Env::Integrate<synthese::messages::Alarm>();
	synthese::util::Env::Integrate<synthese::messages::TextTemplate>();
	synthese::util::Env::Integrate<synthese::messages::ScenarioFolder>();
	synthese::util::Env::Integrate<synthese::messages::Scenario>();
}
