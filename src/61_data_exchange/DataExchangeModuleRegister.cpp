
// 36 MODULE - Data Exchange

// Factories

#include "DataExchangeModule.hpp"

#include "VDVSubscriptionService.hpp"
#include "VDVDataReadyService.hpp"
#include "VDVDataSupplyService.hpp"
#include "VDVStatusService.hpp"
#include "VDVClientsListService.hpp"

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

	synthese::data_exchange::VDVClientsListService::integrate();
	synthese::data_exchange::VDVSubscriptionService::integrate();
	synthese::data_exchange::VDVDataReadyService::integrate();
	synthese::data_exchange::VDVDataSupplyService::integrate();
	synthese::data_exchange::VDVStatusService::integrate();
	
	synthese::data_exchange::VDVClientTableSync::integrate();
	synthese::data_exchange::VDVServerTableSync::integrate();
	synthese::data_exchange::VDVServerSubscriptionTableSync::integrate();
	


	// Registries
	synthese::util::Env::Integrate<synthese::data_exchange::VDVClient>();
	synthese::util::Env::Integrate<synthese::data_exchange::VDVServer>();
	synthese::util::Env::Integrate<synthese::data_exchange::VDVServerSubscription>();
}
