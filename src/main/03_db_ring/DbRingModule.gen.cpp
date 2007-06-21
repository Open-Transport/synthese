

synthese::util::Factory<synthese::util::ModuleClass>::integrate<synthese::dbring::DbRingModule>("03_db_ring");

synthese::util::Factory<synthese::db::SQLiteTableSync>::integrate<synthese::dbring::NodeInfoTableSync>("0 Node infos");
synthese::util::Factory<synthese::db::SQLiteTableSync>::integrate<synthese::dbring::UpdateRecordTableSync>("1 Update log");

