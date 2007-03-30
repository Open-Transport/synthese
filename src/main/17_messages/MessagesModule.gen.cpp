
synthese::util::Factory<synthese::db::SQLiteTableSync>::integrate<synthese::messages::ScenarioTableSync>("17.00.01 Alarm scenarii");

synthese::util::Factory<synthese::db::SQLiteTableSync>::integrate<synthese::messages::AlarmTableSync>("17.10.01 Alarms");
synthese::util::Factory<synthese::db::SQLiteTableSync>::integrate<synthese::messages::TextTemplateTableSync>("17.10.10 Text templates");

synthese::util::Factory<synthese::db::SQLiteTableSync>::integrate<synthese::messages::AlarmObjectLinkTableSync>("99.00.01 Alarm links");



synthese::util::Factory<synthese::admin::AdminInterfaceElement>::integrate<synthese::messages::MessagesAdmin>("messages");
synthese::util::Factory<synthese::admin::AdminInterfaceElement>::integrate<synthese::messages::MessageAdmin>("message");
synthese::util::Factory<synthese::admin::AdminInterfaceElement>::integrate<synthese::messages::MessagesLibraryAdmin>("messageslibrary");
synthese::util::Factory<synthese::admin::AdminInterfaceElement>::integrate<synthese::messages::MessagesScenarioAdmin>("messagesscenario");

synthese::util::Factory<synthese::security::Right>::integrate<synthese::messages::MessagesRight>("Messages");
synthese::util::Factory<synthese::security::Right>::integrate<synthese::messages::MessagesLibraryRight>("MessagesLibrary");

synthese::util::Factory<synthese::dblog::DBLog>::integrate<synthese::messages::MessagesLog>("messages");
synthese::util::Factory<synthese::dblog::DBLog>::integrate<synthese::messages::MessagesLibraryLog>("messageslibrary");

synthese::util::Factory<synthese::server::Action>::integrate<synthese::messages::ScenarioUpdateDatesAction>("messscenarioud");
synthese::util::Factory<synthese::server::Action>::integrate<synthese::messages::DeleteAlarmAction>("deletealarm");
synthese::util::Factory<synthese::server::Action>::integrate<synthese::messages::UpdateAlarmMessagesFromTemplateAction>("uaft");
synthese::util::Factory<synthese::server::Action>::integrate<synthese::messages::UpdateAlarmAction>("updatealarm");
synthese::util::Factory<synthese::server::Action>::integrate<synthese::messages::NewMessageAction>("nmes");
synthese::util::Factory<synthese::server::Action>::integrate<synthese::messages::NewScenarioSendAction>("nssa");
synthese::util::Factory<synthese::server::Action>::integrate<synthese::messages::ScenarioNameUpdateAction>("snu");
synthese::util::Factory<synthese::server::Action>::integrate<synthese::messages::DeleteTextTemplateAction>("dtta");
synthese::util::Factory<synthese::server::Action>::integrate<synthese::messages::UpdateTextTemplateAction>("utta");
synthese::util::Factory<synthese::server::Action>::integrate<synthese::messages::AlarmStopAction>("masa");
synthese::util::Factory<synthese::server::Action>::integrate<synthese::messages::ScenarioStopAction>("mssa");
synthese::util::Factory<synthese::server::Action>::integrate<synthese::messages::AddScenarioAction>("masca");
synthese::util::Factory<synthese::server::Action>::integrate<synthese::messages::DeleteScenarioAction>("mdsca");
synthese::util::Factory<synthese::server::Action>::integrate<synthese::messages::TextTemplateAddAction>("mttaa");
synthese::util::Factory<synthese::server::Action>::integrate<synthese::messages::UpdateAlarmMessagesAction>("muama");
synthese::util::Factory<synthese::server::Action>::integrate<synthese::messages::AlarmRemoveLinkAction>("marla");
synthese::util::Factory<synthese::server::Action>::integrate<synthese::messages::AlarmAddLinkAction>("maala");
