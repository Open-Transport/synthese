#include "MessagesModule.h"

#include "AlarmObjectLinkTableSync.h"
#include "AlarmTableSync.h"
#include "CustomBroadcastPointTableSync.hpp"
#include "MessageAlternativeTableSync.hpp"
#include "MessageApplicationPeriodTableSync.hpp"
#include "MessagesSectionTableSync.hpp"
#include "MessageTypeTableSync.hpp"
#include "NotificationEventTableSync.hpp"
#include "ScenarioTableSync.h"
#include "ScenarioCalendarTableSync.hpp"
#include "ScenarioFolderTableSync.h"
#include "NotificationProviderTableSync.hpp"

#include "BroadcastPointAlarmRecipient.hpp"

#include "MessageAdmin.h"
#include "MessagesRight.h"
#include "MessagesLibraryRight.h"

#include "MessagesLog.h"
#include "MessagesLibraryLog.h"
#include "NotificationLog.hpp"

#include "CustomBroadcastPointsService.hpp"
#include "GetMessagesFunction.hpp"
#include "MessagesSectionsService.hpp"
#include "MessageTypesService.hpp"
#include "ScenariosListFunction.hpp"
#include "ScenarioDisplayFunction.hpp"
#include "ScenarioFoldersService.hpp"
#include "NotificationProvidersService.hpp"
#include "NotificationProviderTestService.hpp"
#include "NotificationEventsService.hpp"

#include "ClearAllBroadcastCachesAction.hpp"
#include "ScenarioSaveAction.h"
#include "SimpleMessageCreationAction.hpp"
#include "UpdateAlarmMessagesFromTemplateAction.h"
#include "NewMessageAction.h"
#include "ScenarioStopAction.h"
#include "AddScenarioAction.h"
#include "UpdateAlarmMessagesAction.h"
#include "AlarmRemoveLinkAction.h"
#include "AlarmAddLinkAction.h"
#include "ScenarioStopAction.h"
#include "ScenarioFolderAdd.h"
#include "ScenarioFolderUpdateAction.h"

// Registries
#include "AlarmObjectLink.h"
#include "CustomBroadcastPoint.hpp"
#include "NotificationEvent.hpp"
#include "NotificationProvider.hpp"
#include "MessageAlternative.hpp"
#include "MessagesTypes.h"
#include "MessagesSection.hpp"
#include "Alarm.h"
#include "ScenarioFolder.h"
#include "Scenario.h"
#include "ScenarioCalendar.hpp"
#include "FileNotificationChannel.hpp"
#include "HttpNotificationChannel.hpp"
#include "SmtpNotificationChannel.hpp"


#include "MessagesModule.inc.cpp"

void synthese::messages::moduleRegister()
{

	synthese::messages::MessagesModule::integrate();

	synthese::messages::AlarmObjectLinkTableSync::integrate();
	synthese::messages::AlarmTableSync::integrate();
	synthese::messages::CustomBroadcastPointTableSync::integrate();
	synthese::messages::NotificationProviderTableSync::integrate();
	synthese::messages::MessageAlternativeTableSync::integrate();
	synthese::messages::MessageApplicationPeriodTableSync::integrate();
	synthese::messages::MessagesSectionTableSync::integrate();
	synthese::messages::MessageTypeTableSync::integrate();
	synthese::messages::NotificationEventTableSync::integrate();
	synthese::messages::ScenarioCalendarTableSync::integrate();
	synthese::messages::ScenarioFolderTableSync::integrate();
	synthese::messages::ScenarioTableSync::integrate();

	synthese::messages::MessageAdmin::integrate();
	synthese::messages::MessagesRight::integrate();
	synthese::messages::MessagesLibraryRight::integrate();

	synthese::messages::MessagesLog::integrate();
	synthese::messages::MessagesLibraryLog::integrate();
	synthese::messages::NotificationLog::integrate();

	synthese::messages::CustomBroadcastPointsService::integrate();
	synthese::messages::GetMessagesFunction::integrate();
	synthese::messages::NotificationProvidersService::integrate();
	synthese::messages::NotificationProviderTestService::integrate();
	synthese::messages::NotificationEventsService::integrate();
	synthese::messages::MessagesSectionsService::integrate();
	synthese::messages::MessageTypesService::integrate();
	synthese::messages::ScenariosListFunction::integrate();
	synthese::messages::ScenarioDisplayFunction::integrate();
	synthese::messages::ScenarioFoldersService::integrate();

	synthese::messages::ClearAllBroadcastCachesAction::integrate();
	synthese::messages::ScenarioSaveAction::integrate();
	synthese::messages::SimpleMessageCreationAction::integrate();
	synthese::messages::ScenarioStopAction::integrate();
	synthese::messages::UpdateAlarmMessagesFromTemplateAction::integrate();
	synthese::messages::NewMessageAction::integrate();
	synthese::messages::AddScenarioAction::integrate();
	synthese::messages::UpdateAlarmMessagesAction::integrate();
	synthese::messages::AlarmRemoveLinkAction::integrate();
	synthese::messages::AlarmAddLinkAction::integrate();
	synthese::messages::ScenarioFolderAdd::integrate();
	synthese::messages::ScenarioFolderUpdateAction::integrate();

	synthese::messages::CustomBroadcastPoint::integrate();
	synthese::messages::NotificationProvider::integrate();
	synthese::messages::FileNotificationChannel::integrate();
	synthese::messages::HttpNotificationChannel::integrate();
	synthese::messages::SmtpNotificationChannel::integrate();

	synthese::messages::BroadcastPointAlarmRecipient::integrate();

	// Registries
	synthese::util::Env::Integrate<synthese::messages::AlarmObjectLink>();
	synthese::util::Env::Integrate<synthese::messages::NotificationEvent>();
	synthese::util::Env::Integrate<synthese::messages::NotificationProvider>();
	synthese::util::Env::Integrate<synthese::messages::MessageAlternative>();
	synthese::util::Env::Integrate<synthese::messages::MessageApplicationPeriod>();
	synthese::util::Env::Integrate<synthese::messages::MessageType>();
	synthese::util::Env::Integrate<synthese::messages::ScenarioCalendar>();
	synthese::util::Env::Integrate<synthese::messages::Alarm>();
	synthese::util::Env::Integrate<synthese::messages::ScenarioFolder>();
	synthese::util::Env::Integrate<synthese::messages::Scenario>();
	synthese::util::Env::Integrate<synthese::messages::CustomBroadcastPoint>();
	synthese::util::Env::Integrate<synthese::messages::MessagesSection>();
}
