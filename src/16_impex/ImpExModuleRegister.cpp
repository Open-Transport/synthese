
// IMPEX 16

// Factories

#include "ImpExModule.h"

#include "ImportFunction.h"
#include "ExportFunction.hpp"

#include "DataSourceTableSync.h"
#include "DataSourcesAdmin.h"
#include "DataSourceAdmin.h"

#include "CleanObsoleteDataAction.hpp"
#include "DataSourceUpdateAction.hpp"

#include "DumpFileFormat.hpp"

// Registries

#include "DataSource.h"


#include "ImpExModule.inc.cpp"

void synthese::impex::moduleRegister()
{

	// IMPEX 16

	// Factories

	synthese::impex::ImpExModule::integrate();

	synthese::impex::ImportFunction::integrate ();
	synthese::impex::ExportFunction::integrate ();

	synthese::impex::DataSourceTableSync::integrate();

	synthese::impex::DataSourcesAdmin::integrate();
	synthese::impex::DataSourceAdmin::integrate();

	synthese::impex::CleanObsoleteDataAction::integrate();
	synthese::impex::DataSourceUpdateAction::integrate();

	synthese::impex::DumpFileFormat::integrate();

	// Registries

	synthese::util::Env::Integrate<synthese::impex::DataSource>();
}
