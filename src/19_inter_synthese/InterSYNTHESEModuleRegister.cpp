
#include "InterSYNTHESEModule.hpp"

#include "InterSYNTHESEConfig.hpp"
#include "InterSYNTHESEConfigItem.hpp"
#include "InterSYNTHESESlave.hpp"
#include "InterSYNTHESEQueue.hpp"

#include "InterSYNTHESEConfigTableSync.hpp"
#include "InterSYNTHESEConfigItemTableSync.hpp"
#include "InterSYNTHESEQueueTableSync.hpp"
#include "InterSYNTHESESlaveTableSync.hpp"

#include "InterSYNTHESESlaveUpdateService.hpp"

#include "InterSYNTHESEModule.inc.cpp"

void synthese::inter_synthese::moduleRegister()
{
	synthese::inter_synthese::InterSYNTHESEModule::integrate();

	synthese::inter_synthese::InterSYNTHESEConfigTableSync::integrate();
	synthese::inter_synthese::InterSYNTHESEConfigItemTableSync::integrate();
	synthese::inter_synthese::InterSYNTHESESlaveTableSync::integrate();
	synthese::inter_synthese::InterSYNTHESEQueueTableSync::integrate();

	synthese::inter_synthese::InterSYNTHESESlaveUpdateService::integrate();

	synthese::util::Env::Integrate<synthese::inter_synthese::InterSYNTHESEConfig>();
	synthese::util::Env::Integrate<synthese::inter_synthese::InterSYNTHESEConfigItem>();
	synthese::util::Env::Integrate<synthese::inter_synthese::InterSYNTHESESlave>();
	synthese::util::Env::Integrate<synthese::inter_synthese::InterSYNTHESEQueue>();
}
