
synthese::util::Factory<synthese::util::ModuleClass>::integrate<synthese::security::SecurityModule>("12_security");

synthese::util::Factory<synthese::db::SQLiteTableSync>::integrate<synthese::security::ProfileTableSync>("12.01 Profile");
synthese::util::Factory<synthese::db::SQLiteTableSync>::integrate<synthese::security::UserTableSync>("12.02 User");

synthese::util::Factory<synthese::admin::AdminInterfaceElement>::integrate<synthese::security::UsersAdmin>("users");
synthese::util::Factory<synthese::admin::AdminInterfaceElement>::integrate<synthese::security::UserAdmin>("user");
synthese::util::Factory<synthese::admin::AdminInterfaceElement>::integrate<synthese::security::ProfilesAdmin>("profiles");

synthese::util::Factory<synthese::server::Action>::integrate<synthese::security::AddUserAction>("sau");
