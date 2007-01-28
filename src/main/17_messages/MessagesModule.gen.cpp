
synthese::util::Factory<synthese::db::SQLiteTableSync>::integrate<synthese::messages::AlarmTableSync>("17.10.01");

synthese::util::Factory<synthese::admin::AdminInterfaceElement>::integrate<synthese::messages::MessagesAdmin>("messages");
synthese::util::Factory<synthese::admin::AdminInterfaceElement>::integrate<synthese::messages::MessagesLibraryAdmin>("messageslibrary");
