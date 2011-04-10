
synthese::security::SecurityModule::integrate();

synthese::security::ProfileTableSync::integrate();
synthese::security::UserTableSync::integrate();

synthese::security::UserNameInterfaceElement::integrate();
synthese::security::UserIdInterfaceElement::integrate();
synthese::security::UserPhoneInterfaceElement::integrate();

synthese::security::UsersAdmin::integrate();
synthese::security::UserAdmin::integrate();
synthese::security::ProfilesAdmin::integrate();
synthese::security::ProfileAdmin::integrate();

synthese::security::AddUserAction::integrate();
synthese::security::AddProfileAction::integrate();
synthese::security::AddRightAction::integrate();
synthese::security::DeleteRightAction::integrate();
synthese::security::UpdateRightAction::integrate();
synthese::security::UpdateProfileAction::integrate();
synthese::security::UserUpdateAction::integrate();
synthese::security::UserPasswordUpdateAction::integrate();

synthese::security::GlobalRight::integrate();
synthese::security::SecurityRight::integrate();

synthese::security::SecurityLog::integrate();

// Registries
synthese::util::Env::Integrate<synthese::security::User>();
synthese::util::Env::Integrate<synthese::security::Profile>();
