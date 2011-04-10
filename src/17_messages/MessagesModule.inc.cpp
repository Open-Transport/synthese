
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
