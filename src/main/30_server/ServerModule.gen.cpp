
synthese::util::Factory<synthese::db::SQLiteTableSync>::integrate<synthese::server::ServerConfigTableSync>("0");

synthese::util::Factory<synthese::db::SQLiteTableSync>::integrate<synthese::server::SiteTableSync>("30.01");

synthese::util::Factory<synthese::server::Action>::integrate<synthese::server::LogoutAction>("logout");