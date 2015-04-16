#include "SettingsModule.h"
#include "SettingsTableSync.h"
#include "SettingsFunction.h"

#include "SettingsModule.inc.cpp"

void synthese::settings::moduleRegister()
{
	synthese::settings::SettingsTableSync::integrate();
	synthese::settings::SettingsModule::integrate();
	synthese::cms::SettingsFunction::integrate();

	// Registries
	synthese::util::Env::Integrate<synthese::settings::SettingsValue>();
}
