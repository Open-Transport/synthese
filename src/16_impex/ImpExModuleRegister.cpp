
// IMPEX 16

// Factories

#include "ImpExModule.h"

#include "DataSourcesService.hpp"
#include "ExportFunction.hpp"
#include "FileFormatsService.hpp"
#include "ImportFunction.h"
#include "ImportsService.hpp"

#include "DataSourceTableSync.h"
#include "ImportTableSync.hpp"

#include "DataSourcesAdmin.h"
#include "DataSourceAdmin.h"

#include "CleanObsoleteDataAction.hpp"

// Registries

#include "DataSource.h"
#include "Import.hpp"

#include "ImpExModule.inc.cpp"

void synthese::impex::moduleRegister()
{

	// IMPEX 16

	// Factories

	synthese::impex::ImpExModule::integrate();

	synthese::impex::DataSourcesService::integrate();
	synthese::impex::ExportFunction::integrate ();
	synthese::impex::FileFormatsService::integrate();
	synthese::impex::ImportFunction::integrate ();
	synthese::impex::ImportsService::integrate();

	synthese::impex::DataSourceTableSync::integrate();
	synthese::impex::ImportTableSync::integrate();

	synthese::impex::DataSourcesAdmin::integrate();
	synthese::impex::DataSourceAdmin::integrate();

	synthese::impex::CleanObsoleteDataAction::integrate();

	// Registries

	synthese::util::Env::Integrate<synthese::impex::DataSource>();
	synthese::util::Env::Integrate<synthese::impex::Import>();
}
