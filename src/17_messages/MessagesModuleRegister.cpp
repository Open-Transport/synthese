
#include "MessagesModule.h"

#include "AlarmTableSync.h"
#include "TextTemplateTableSync.h"
#include "ScenarioTableSync.h"
#include "AlarmObjectLinkTableSync.h"
#include "ScenarioFolderTableSync.h"

#include "SentScenarioInheritedTableSync.h"
#include "ScenarioTemplateInheritedTableSync.h"
#include "AlarmTemplateInheritedTableSync.h"
#include "ScenarioSentAlarmInheritedTableSync.h"

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
#include "AlarmTemplate.h"
#include "SentAlarm.h"
#include "TextTemplate.h"
#include "ScenarioFolder.h"
#include "ScenarioTemplate.h"
#include "SentScenario.h"


#include "MessagesModule.inc.cpp"

void synthese::messages::moduleRegister()
{
	
	synthese::messages::MessagesModule::integrate();
	
	synthese::messages::ScenarioTableSync::integrate();
	synthese::messages::AlarmTableSync::integrate();
	synthese::messages::TextTemplateTableSync::integrate();
	synthese::messages::AlarmObjectLinkTableSync::integrate();
	synthese::messages::ScenarioFolderTableSync::integrate();
	
	synthese::messages::SentScenarioInheritedTableSync::integrate();
	synthese::messages::ScenarioTemplateInheritedTableSync::integrate();
	synthese::messages::AlarmTemplateInheritedTableSync::integrate();
	synthese::messages::ScenarioSentAlarmInheritedTableSync::integrate();
	
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
	synthese::util::Env::Integrate<synthese::messages::AlarmTemplate>();
	synthese::util::Env::Integrate<synthese::messages::SentAlarm>();
	synthese::util::Env::Integrate<synthese::messages::TextTemplate>();
	synthese::util::Env::Integrate<synthese::messages::ScenarioFolder>();
	synthese::util::Env::Integrate<synthese::messages::ScenarioTemplate>();
	synthese::util::Env::Integrate<synthese::messages::SentScenario>();
}
