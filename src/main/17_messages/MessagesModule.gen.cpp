
synthese::util::Factory<synthese::db::SQLiteTableSync>::integrate<synthese::messages::AlarmTableSync>("17.10.01");

synthese::util::Factory<synthese::admin::AdminInterfaceElement>::integrate<synthese::messages::MessagesAdmin>("messages");
synthese::util::Factory<synthese::admin::AdminInterfaceElement>::integrate<synthese::messages::MessagesLibraryAdmin>("messageslibrary");

synthese::util::Factory<synthese::security::Right>::integrate<synthese::messages::MessagesRight>("Messages");
synthese::util::Factory<synthese::security::Right>::integrate<synthese::messages::MessagesLibraryRight>("MessagesLibrary");