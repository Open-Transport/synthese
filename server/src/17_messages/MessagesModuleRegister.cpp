#include "MessagesModule.h"

#include "AlarmObjectLinkTableSync.h"
#include "AlarmTableSync.h"
#include "CustomBroadcastPointTableSync.hpp"
#include "MailingListTableSync.hpp"
#include "MailingListSubscriptionTableSync.hpp"
#include "MessageAlternativeTableSync.hpp"
#include "MessageApplicationPeriodTableSync.hpp"
#include "MessagesSectionTableSync.hpp"
#include "MessageTagTableSync.hpp"
#include "MessageTypeTableSync.hpp"
#include "ScenarioTableSync.h"
#include "ScenarioCalendarTableSync.hpp"
#include "ScenarioFolderTableSync.h"
#include "TextTemplateTableSync.h"
#include "NotificationProviderTableSync.hpp"

#include "BroadcastPointAlarmRecipient.hpp"

#include "MessagesAdmin.h"
#include "MessageAdmin.h"
#include "MessagesScenarioAdmin.h"
#include "MessagesLibraryAdmin.h"
#include "MessagesTemplateLibraryAdmin.h"

#include "MessagesRight.h"
#include "MessagesLibraryRight.h"

#include "MessagesLog.h"
#include "MessagesLibraryLog.h"

#include "CustomBroadcastPointsService.hpp"
#include "GetMessagesFunction.hpp"
#include "MailingListsService.hpp"
#include "MessagesSectionsService.hpp"
#include "MessageTagsService.hpp"
#include "MessageTypesService.hpp"
#include "ScenariosListFunction.hpp"
#include "ScenarioDisplayFunction.hpp"
#include "ScenarioFoldersService.hpp"
#include "NotificationProvidersService.hpp"

#include "ClearAllBroadcastCachesAction.hpp"
#include "MailingListSendAction.hpp"
#include "ScenarioSaveAction.h"
#include "SimpleMessageCreationAction.hpp"
#include "UpdateAlarmMessagesFromTemplateAction.h"
#include "NewMessageAction.h"
#include "UpdateTextTemplateAction.h"
#include "ScenarioStopAction.h"
#include "ScenarioRemoveAction.h"
#include "AddScenarioAction.h"
#include "TextTemplateAddAction.h"
#include "UpdateAlarmMessagesAction.h"
#include "AlarmRemoveLinkAction.h"
#include "AlarmAddLinkAction.h"
#include "ScenarioFolderAdd.h"
#include "ScenarioFolderUpdateAction.h"

// Registries
#include "AlarmObjectLink.h"
#include "CustomBroadcastPoint.hpp"
#include "NotificationProvider.hpp"
#include "MailingList.hpp"
#include "MailingListSubscription.hpp"
#include "MessageAlternative.hpp"
#include "MessagesTypes.h"
#include "MessagesSection.hpp"
#include "Alarm.h"
#include "TextTemplate.h"
#include "ScenarioFolder.h"
#include "Scenario.h"
#include "ScenarioCalendar.hpp"
#include "FileNotificationChannel.hpp"
#include "HttpNotificationChannel.hpp"


#include "MessagesModule.inc.cpp"

void synthese::messages::moduleRegister()
{

	synthese::messages::MessagesModule::integrate();

	synthese::messages::AlarmObjectLinkTableSync::integrate();
	synthese::messages::AlarmTableSync::integrate();
	synthese::messages::CustomBroadcastPointTableSync::integrate();
	synthese::messages::NotificationProviderTableSync::integrate();
	synthese::messages::MailingListTableSync::integrate();
	synthese::messages::MailingListSubscriptionTableSync::integrate();
	synthese::messages::MessageAlternativeTableSync::integrate();
	synthese::messages::MessageApplicationPeriodTableSync::integrate();
	synthese::messages::MessagesSectionTableSync::integrate();
	synthese::messages::MessageTagTableSync::integrate();
	synthese::messages::MessageTypeTableSync::integrate();
	synthese::messages::ScenarioCalendarTableSync::integrate();
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

	synthese::messages::CustomBroadcastPointsService::integrate();
	synthese::messages::GetMessagesFunction::integrate();
	synthese::messages::MailingListsService::integrate();
	synthese::messages::NotificationProvidersService::integrate();
	synthese::messages::MessagesSectionsService::integrate();
	synthese::messages::MessageTagsService::integrate();
	synthese::messages::MessageTypesService::integrate();
	synthese::messages::ScenariosListFunction::integrate();
	synthese::messages::ScenarioDisplayFunction::integrate();
	synthese::messages::ScenarioFoldersService::integrate();

	synthese::messages::ClearAllBroadcastCachesAction::integrate();
	synthese::messages::MailingListSendAction::integrate();
	synthese::messages::ScenarioSaveAction::integrate();
	synthese::messages::SimpleMessageCreationAction::integrate();
	synthese::messages::ScenarioStopAction::integrate();
	synthese::messages::ScenarioRemoveAction::integrate();
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

	synthese::messages::CustomBroadcastPoint::integrate();
	synthese::messages::NotificationProvider::integrate();
	synthese::messages::FileNotificationChannel::integrate();
	synthese::messages::HttpNotificationChannel::integrate();

	synthese::messages::BroadcastPointAlarmRecipient::integrate();

	// Registries
	synthese::util::Env::Integrate<synthese::messages::AlarmObjectLink>();
	synthese::util::Env::Integrate<synthese::messages::NotificationProvider>();
	synthese::util::Env::Integrate<synthese::messages::MailingList>();
	synthese::util::Env::Integrate<synthese::messages::MailingListSubscription>();
	synthese::util::Env::Integrate<synthese::messages::MessageAlternative>();
	synthese::util::Env::Integrate<synthese::messages::MessageApplicationPeriod>();
	synthese::util::Env::Integrate<synthese::messages::MessageTag>();
	synthese::util::Env::Integrate<synthese::messages::MessageType>();
	synthese::util::Env::Integrate<synthese::messages::ScenarioCalendar>();
	synthese::util::Env::Integrate<synthese::messages::Alarm>();
	synthese::util::Env::Integrate<synthese::messages::TextTemplate>();
	synthese::util::Env::Integrate<synthese::messages::ScenarioFolder>();
	synthese::util::Env::Integrate<synthese::messages::Scenario>();
	synthese::util::Env::Integrate<synthese::messages::CustomBroadcastPoint>();
	synthese::util::Env::Integrate<synthese::messages::MessagesSection>();
}
