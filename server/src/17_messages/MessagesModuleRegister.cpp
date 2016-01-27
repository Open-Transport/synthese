#include "MessagesModule.h"

#include "AlarmObjectLinkTableSync.h"
#include "AlarmTableSync.h"
#include "CustomBroadcastPointTableSync.hpp"
#include "MediaLibraryTableSync.hpp"
#include "MessageAlternativeTableSync.hpp"
#include "MessageApplicationPeriodTableSync.hpp"
#include "MessagesSectionTableSync.hpp"
#include "MessageTagTableSync.hpp"
#include "MessageTypeTableSync.hpp"
#include "NotificationEventTableSync.hpp"
#include "SentScenarioTableSync.h"
#include "ScenarioTemplateTableSync.h"
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

#include "BroadcastPointsService.hpp"
#include "CustomBroadcastPointsService.hpp"
#include "GetMessagesFunction.hpp"
#include "MediaLibrariesService.hpp"
#include "MessagesSectionsService.hpp"
#include "MessageTagsService.hpp"
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
#include "ScenarioRemoveAction.h"
#include "AddScenarioAction.h"
#include "UpdateAlarmMessagesAction.h"
#include "AlarmRemoveLinkAction.h"
#include "AlarmAddLinkAction.h"
#include "ScenarioFolderAdd.h"
#include "ScenarioFolderUpdateAction.h"

// Registries
#include "AlarmObjectLink.h"
#include "CustomBroadcastPoint.hpp"
#include "NotificationEvent.hpp"
#include "NotificationProvider.hpp"
#include "MediaLibrary.hpp"
#include "MessageAlternative.hpp"
#include "MessagesTypes.h"
#include "MessagesSection.hpp"
#include "Alarm.h"
#include "ScenarioFolder.h"
#include "SentScenario.h"
#include "ScenarioTemplate.h"
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
	synthese::messages::MediaLibraryTableSync::integrate();
	synthese::messages::MessageAlternativeTableSync::integrate();
	synthese::messages::MessageApplicationPeriodTableSync::integrate();
	synthese::messages::MessagesSectionTableSync::integrate();
	synthese::messages::MessageTagTableSync::integrate();
	synthese::messages::MessageTypeTableSync::integrate();
	synthese::messages::NotificationEventTableSync::integrate();
	synthese::messages::ScenarioCalendarTableSync::integrate();
	synthese::messages::ScenarioFolderTableSync::integrate();
	synthese::messages::ScenarioTemplateTableSync::integrate();
	synthese::messages::SentScenarioTableSync::integrate();
	synthese::messages::MessageAdmin::integrate();
	synthese::messages::MessagesRight::integrate();
	synthese::messages::MessagesLibraryRight::integrate();

	synthese::messages::MessagesLog::integrate();
	synthese::messages::MessagesLibraryLog::integrate();
	synthese::messages::NotificationLog::integrate();

	synthese::messages::BroadcastPointsService::integrate();
	synthese::messages::CustomBroadcastPointsService::integrate();
	synthese::messages::GetMessagesFunction::integrate();
	synthese::messages::NotificationProvidersService::integrate();
	synthese::messages::NotificationProviderTestService::integrate();
	synthese::messages::NotificationEventsService::integrate();
	synthese::messages::MediaLibrariesService::integrate();
	synthese::messages::MessagesSectionsService::integrate();
	synthese::messages::MessageTagsService::integrate();
	synthese::messages::MessageTypesService::integrate();
	synthese::messages::ScenariosListFunction::integrate();
	synthese::messages::ScenarioDisplayFunction::integrate();
	synthese::messages::ScenarioFoldersService::integrate();

	synthese::messages::ClearAllBroadcastCachesAction::integrate();
	synthese::messages::ScenarioSaveAction::integrate();
	synthese::messages::SimpleMessageCreationAction::integrate();
	synthese::messages::ScenarioStopAction::integrate();
	synthese::messages::ScenarioRemoveAction::integrate();
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
	//INTEGRATE(synthese::messages::AlarmObjectLink);
	INTEGRATE(synthese::messages::NotificationEvent);
	INTEGRATE(synthese::messages::NotificationProvider);
	INTEGRATE(synthese::messages::MediaLibrary);
	INTEGRATE(synthese::messages::MessageAlternative);
	INTEGRATE(synthese::messages::MessageApplicationPeriod);
	INTEGRATE(synthese::messages::MessageTag);
	INTEGRATE(synthese::messages::MessageType);
	INTEGRATE(synthese::messages::ScenarioCalendar);
	INTEGRATE(synthese::messages::Alarm);
	INTEGRATE(synthese::messages::ScenarioFolder);
	INTEGRATE(synthese::messages::ScenarioTemplate);
	INTEGRATE(synthese::messages::SentScenario);
	INTEGRATE(synthese::messages::CustomBroadcastPoint);
	INTEGRATE(synthese::messages::MessagesSection);
}
