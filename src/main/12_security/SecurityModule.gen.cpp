
synthese::util::Factory<synthese::util::ModuleClass>::integrate<synthese::security::SecurityModule>("12_security");

synthese::util::Factory<synthese::db::SQLiteTableSync>::integrate<synthese::security::ProfileTableSync>("12.01 Profile");
synthese::util::Factory<synthese::db::SQLiteTableSync>::integrate<synthese::security::UserTableSync>("12.02 User");

synthese::security::UserNameInterfaceElement::integrate();
synthese::security::UserIdInterfaceElement::integrate();
synthese::security::UserPhoneInterfaceElement::integrate();

synthese::security::UsersAdmin::integrate();
synthese::security::UserAdmin::integrate();
synthese::security::ProfilesAdmin::integrate();
synthese::security::ProfileAdmin::integrate();

synthese::util::Factory<synthese::server::Action>::integrate<synthese::security::AddUserAction>("sau");
synthese::util::Factory<synthese::server::Action>::integrate<synthese::security::DelUserAction>("sdu");
synthese::util::Factory<synthese::server::Action>::integrate<synthese::security::AddProfileAction>("apa");
synthese::util::Factory<synthese::server::Action>::integrate<synthese::security::DeleteProfileAction>("dpa");
synthese::security::AddRightAction::integrate();
synthese::util::Factory<synthese::server::Action>::integrate<synthese::security::DeleteRightAction>("dra");
synthese::security::UpdateRightAction::integrate();
synthese::security::UpdateProfileAction::integrate();
synthese::util::Factory<synthese::server::Action>::integrate<synthese::security::UserUpdateAction>("uua");
synthese::util::Factory<synthese::server::Action>::integrate<synthese::security::UserPasswordUpdateAction>("upua");

synthese::security::GlobalRight::integrate();
synthese::security::SecurityRight::integrate();

synthese::security::SecurityLog::integrate();
