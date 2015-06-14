
#include "12_security/SecurityModule.h"
#include "12_security/UserTableSync.h"
#include "12_security/ProfileTableSync.h"
#include "12_security/ProfilesAdmin.h"
#include "12_security/ProfileAdmin.h"
#include "12_security/UsersAdmin.h"
#include "12_security/UserAdmin.h"
#include "12_security/AddUserAction.h"
#include "12_security/AddProfileAction.h"
#include "12_security/DeleteRightAction.h"
#include "12_security/AddRightAction.h"
#include "12_security/UpdateRightAction.h"
#include "12_security/UpdateProfileAction.h"
#include "12_security/UserUpdateAction.h"
#include "12_security/UserPasswordUpdateAction.h"
#include "12_security/UserPasswordRecoveryAction.hpp"
#include "12_security/UserPasswordChangeAction.hpp"

#include "12_security/SecurityRight.h"
#include "12_security/GlobalRight.h"

#include "12_security/SecurityLog.h"

#include "CheckRightService.hpp"
#include "UsersListService.hpp"

// Registries
#include "User.h"
#include "Profile.h"


#include "SecurityModule.inc.cpp"

void synthese::security::moduleRegister()
{

	synthese::security::SecurityModule::integrate();

	synthese::security::ProfileTableSync::integrate();
	synthese::security::UserTableSync::integrate();

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
	synthese::security::UserPasswordRecoveryAction::integrate();
	synthese::security::UserPasswordChangeAction::integrate();

	synthese::security::GlobalRight::integrate();
	synthese::security::SecurityRight::integrate();

	synthese::security::SecurityLog::integrate();

	synthese::security::CheckRightService::integrate();
	synthese::security::UsersListService::integrate();

	// Registries
	synthese::util::Env::Integrate<synthese::security::User>();
	synthese::util::Env::Integrate<synthese::security::Profile>();
}
