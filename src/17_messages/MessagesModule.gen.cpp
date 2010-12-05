
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

synthese::messages::ScenarioUpdateDatesAction::integrate();
synthese::messages::SimpleMessageCreationAction::integrate();
synthese::messages::ScenarioStopAction::integrate();
synthese::messages::DeleteAlarmAction::integrate();
synthese::messages::UpdateAlarmMessagesFromTemplateAction::integrate();
synthese::messages::NewMessageAction::integrate();
synthese::messages::NewScenarioSendAction::integrate();
synthese::messages::DeleteTextTemplateAction::integrate();
synthese::messages::UpdateTextTemplateAction::integrate();
synthese::messages::AddScenarioAction::integrate();
synthese::messages::DeleteScenarioAction::integrate();
synthese::messages::TextTemplateAddAction::integrate();
synthese::messages::UpdateAlarmMessagesAction::integrate();
synthese::messages::AlarmRemoveLinkAction::integrate();
synthese::messages::AlarmAddLinkAction::integrate();
synthese::messages::ScenarioFolderAdd::integrate();
synthese::messages::ScenarioFolderRemoveAction::integrate();
synthese::messages::ScenarioFolderUpdateAction::integrate();

// Registries
synthese::util::Env::Integrate<synthese::messages::AlarmObjectLink>();
synthese::util::Env::Integrate<synthese::messages::AlarmTemplate>();
synthese::util::Env::Integrate<synthese::messages::SentAlarm>();
synthese::util::Env::Integrate<synthese::messages::TextTemplate>();
synthese::util::Env::Integrate<synthese::messages::ScenarioFolder>();
synthese::util::Env::Integrate<synthese::messages::ScenarioTemplate>();
synthese::util::Env::Integrate<synthese::messages::SentScenario>();
