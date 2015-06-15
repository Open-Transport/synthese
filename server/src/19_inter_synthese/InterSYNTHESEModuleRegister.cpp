
#include "InterSYNTHESEModule.hpp"

#include "InterSYNTHESEConfig.hpp"
#include "InterSYNTHESEConfigItem.hpp"
#include "InterSYNTHESESlave.hpp"
#include "InterSYNTHESEQueue.hpp"
#include "InterSYNTHESEPackage.hpp"

#include "InterSYNTHESEFileFormat.hpp"
#include "InterSYNTHESEPackageFileFormat.hpp"

#include "InterSYNTHESEConfigsViewService.hpp"
#include "InterSYNTHESEIsSynchronisingService.hpp"
#include "InterSYNTHESEObjectCreateService.hpp"
#include "InterSYNTHESEObjectUpdateService.hpp"
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

#include "SpecificPostInstall.hpp"

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
	synthese::inter_synthese::InterSYNTHESEIsSynchronisingService::integrate();
	synthese::inter_synthese::InterSYNTHESEObjectCreateService::integrate();
	synthese::inter_synthese::InterSYNTHESEObjectUpdateService::integrate();
	synthese::inter_synthese::InterSYNTHESEPackageCommitService::integrate();
	synthese::inter_synthese::InterSYNTHESESlavesViewService::integrate();
	synthese::inter_synthese::InterSYNTHESESlaveUpdateService::integrate();
	synthese::inter_synthese::InterSYNTHESEUpdateAckService::integrate();
	synthese::inter_synthese::InterSYNTHESEPackageGetContentService::integrate();
	synthese::inter_synthese::InterSYNTHESEPackagesService::integrate();
	synthese::inter_synthese::InterSYNTHESEUpdatePushService::integrate();

	synthese::inter_synthese::SpecificPostInstall::integrate();

	INTEGRATE(synthese::inter_synthese::InterSYNTHESEConfig);
	INTEGRATE(synthese::inter_synthese::InterSYNTHESEConfigItem);
	INTEGRATE(synthese::inter_synthese::InterSYNTHESESlave);
	INTEGRATE(synthese::inter_synthese::InterSYNTHESEQueue);
	INTEGRATE(synthese::inter_synthese::InterSYNTHESEPackage);
}
