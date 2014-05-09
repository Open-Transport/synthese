
#include "InterSYNTHESEModule.hpp"

#include "InterSYNTHESEConfig.hpp"
#include "InterSYNTHESEConfigItem.hpp"
#include "InterSYNTHESESlave.hpp"
#include "InterSYNTHESEQueue.hpp"
#include "InterSYNTHESEPackage.hpp"

#include "InterSYNTHESEFileFormat.hpp"
#include "InterSYNTHESEPackageFileFormat.hpp"

#include "InterSYNTHESEConfigsViewService.hpp"
#include "InterSYNTHESEPackageCommitService.hpp"
#include "InterSYNTHESESlavesViewService.hpp"
#include "InterSYNTHESESlaveUpdateService.hpp"
#include "InterSYNTHESEUpdateAckService.hpp"
#include "InterSYNTHESEPackageGetContentService.hpp"
#include "InterSYNTHESEPackagesService.hpp"
#include "InterSYNTHESEUpdatePushService.hpp"

#include "InterSYNTHESEPackageAutoLockAction.hpp"
#include "InterSYNTHESEPackageCommitAction.hpp"

#include "InterSYNTHESEConfigTableSync.hpp"
#include "InterSYNTHESEConfigItemTableSync.hpp"
#include "InterSYNTHESEPackageTableSync.hpp"
#include "InterSYNTHESEQueueTableSync.hpp"
#include "InterSYNTHESESlaveTableSync.hpp"

#include "InterSYNTHESEModule.inc.cpp"

void synthese::inter_synthese::moduleRegister()
{
	synthese::inter_synthese::InterSYNTHESEModule::integrate();

	synthese::inter_synthese::InterSYNTHESEConfigTableSync::integrate();
	synthese::inter_synthese::InterSYNTHESEConfigItemTableSync::integrate();
	synthese::inter_synthese::InterSYNTHESEPackageTableSync::integrate();
	synthese::inter_synthese::InterSYNTHESESlaveTableSync::integrate();
	synthese::inter_synthese::InterSYNTHESEQueueTableSync::integrate();

	synthese::inter_synthese::InterSYNTHESEFileFormat::integrate();
	synthese::inter_synthese::InterSYNTHESEPackageFileFormat::integrate();

	synthese::inter_synthese::InterSYNTHESEPackageAutoLockAction::integrate();
	synthese::inter_synthese::InterSYNTHESEPackageCommitAction::integrate();

	synthese::inter_synthese::InterSYNTHESEConfigsViewService::integrate();
	synthese::inter_synthese::InterSYNTHESEPackageCommitService::integrate();
	synthese::inter_synthese::InterSYNTHESESlavesViewService::integrate();
	synthese::inter_synthese::InterSYNTHESESlaveUpdateService::integrate();
	synthese::inter_synthese::InterSYNTHESEUpdateAckService::integrate();
	synthese::inter_synthese::InterSYNTHESEPackageGetContentService::integrate();
	synthese::inter_synthese::InterSYNTHESEPackagesService::integrate();
	synthese::inter_synthese::InterSYNTHESEUpdatePushService::integrate();

	synthese::util::Env::Integrate<synthese::inter_synthese::InterSYNTHESEConfig>();
	synthese::util::Env::Integrate<synthese::inter_synthese::InterSYNTHESEConfigItem>();
	synthese::util::Env::Integrate<synthese::inter_synthese::InterSYNTHESESlave>();
	synthese::util::Env::Integrate<synthese::inter_synthese::InterSYNTHESEQueue>();
	synthese::util::Env::Integrate<synthese::inter_synthese::InterSYNTHESEPackage>();
}
