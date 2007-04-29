
synthese::util::Factory<synthese::admin::AdminInterfaceElement>::integrate<synthese::dblog::DBLogList>("dblogs");
synthese::util::Factory<synthese::admin::AdminInterfaceElement>::integrate<synthese::dblog::DBLogViewer>("dblog");

synthese::dblog::DBLogRight::integrate();

synthese::util::Factory<synthese::db::SQLiteTableSync>::integrate<synthese::dblog::DBLogEntryTableSync>("13.01 DB Log entries");
