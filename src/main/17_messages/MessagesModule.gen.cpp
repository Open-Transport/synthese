
synthese::util::Factory<synthese::db::SQLiteTableSync>::integrate<synthese::messages::AlarmTableSync>("17.10.01");
synthese::util::Factory<synthese::db::SQLiteTableSync>::integrate<synthese::messages::TextTemplateTableSync>("17.10.10");

synthese::util::Factory<synthese::db::SQLiteTableSync>::integrate<synthese::messages::AlarmObjectLinkTableSync>("17.20.01");

synthese::util::Factory<synthese::db::SQLiteTableSync>::integrate<synthese::messages::ScenarioTableSync>("17.30.01");



synthese::util::Factory<synthese::admin::AdminInterfaceElement>::integrate<synthese::messages::MessagesAdmin>("messages");
synthese::util::Factory<synthese::admin::AdminInterfaceElement>::integrate<synthese::messages::MessagesLibraryAdmin>("messageslibrary");

synthese::util::Factory<synthese::security::Right>::integrate<synthese::messages::MessagesRight>("Messages");
synthese::util::Factory<synthese::security::Right>::integrate<synthese::messages::MessagesLibraryRight>("MessagesLibrary");

synthese::util::Factory<synthese::dblog::DBLog>::integrate<synthese::messages::MessagesLog>("messages");
synthese::util::Factory<synthese::dblog::DBLog>::integrate<synthese::messages::MessagesLibraryLog>("messageslibrary");