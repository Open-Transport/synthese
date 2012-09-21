
// 36 MODULE - Data Exchange

// Factories

#include "DataExchangeModule.hpp"

#include "VDVSubscriptionService.hpp"
#include "VDVServerUpdateService.hpp"

#include "VDVClientTableSync.hpp"
#include "VDVServerTableSync.hpp"
#include "VDVServerSubscriptionTableSync.hpp"

// Registries

#include "VDVClient.hpp"
#include "VDVServer.hpp"
#include "VDVServerSubscription.hpp"

#include "DataExchangeModule.inc.cpp"

void synthese::data_exchange::moduleRegister()
{

	// 36 MODULE - Data exchange

	// Factories

	synthese::data_exchange::DataExchangeModule::integrate();

	synthese::data_exchange::VDVSubscriptionService::integrate();
	synthese::data_exchange::VDVServerUpdateService::integrate();
	
	synthese::data_exchange::VDVClientTableSync::integrate();
	synthese::data_exchange::VDVServerTableSync::integrate();
	synthese::data_exchange::VDVServerSubscriptionTableSync::integrate();
	


	// Registries
	synthese::util::Env::Integrate<synthese::data_exchange::VDVClient>();
	synthese::util::Env::Integrate<synthese::data_exchange::VDVServer>();
	synthese::util::Env::Integrate<synthese::data_exchange::VDVServerSubscription>();
}
