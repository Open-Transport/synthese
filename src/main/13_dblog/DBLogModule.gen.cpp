
synthese::dblog::DBLogList::integrate();
synthese::dblog::DBLogViewer::integrate();

synthese::dblog::DBLogRight::integrate();

synthese::util::Factory<synthese::db::SQLiteTableSync>::integrate<synthese::dblog::DBLogEntryTableSync>("13.01 DB Log entries");
