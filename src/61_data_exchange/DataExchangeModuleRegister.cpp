
// 36 MODULE - Data Exchange

// Factories

#include "DataExchangeModule.hpp"

#include "GTFSFileFormat.hpp"
#include "HafasFileFormat.hpp"
#include "HastusCSVFileFormat.hpp"
#include "RTMFileFormat.hpp"
#include "HastusInterfaceFileFormat.hpp"
#include "HeuresFileFormat.hpp"
#include "IneoFileFormat.hpp"
#include "IneoBDSIFileFormat.hpp"
#include "IneoDepartureBoardsFileFormat.hpp"
#include "IneoOperationFileFormat.hpp"
#include "IneoRealTimeFileFormat.hpp"
#include "OGTFileFormat.hpp"
#include "ObitiFileFormat.hpp"
#include "PegaseFileFormat.hpp"
#include "RSSFileFormat.hpp"
#include "TridentFileFormat.h"
#include "StopsShapeFileFormat.hpp"
#include "ServicesCSVFileFormat.hpp"
#include "HousesCSVFileFormat.hpp"
#include "IGNstreetsFileFormat.hpp"
#include "NavstreetsFileFormat.hpp"
#include "OSMFileFormat.hpp"
#include "RoadShapeFileFormat.hpp"
#include "GpsFileFormat.h"

#include "VDVSubscriptionService.hpp"
#include "VDVDataReadyService.hpp"
#include "VDVDataSupplyService.hpp"
#include "VDVStatusService.hpp"
#include "VDVClientsListService.hpp"
#include "VDVServersListService.hpp"

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
	synthese::data_exchange::VDVServersListService::integrate();
	synthese::data_exchange::VDVSubscriptionService::integrate();
	synthese::data_exchange::VDVDataReadyService::integrate();
	synthese::data_exchange::VDVDataSupplyService::integrate();
	synthese::data_exchange::VDVStatusService::integrate();
	
	synthese::data_exchange::VDVClientTableSync::integrate();
	synthese::data_exchange::VDVServerTableSync::integrate();
	synthese::data_exchange::VDVServerSubscriptionTableSync::integrate();

	synthese::data_exchange::GTFSFileFormat::integrate();
	synthese::data_exchange::HafasFileFormat::integrate();
	synthese::data_exchange::HastusCSVFileFormat::integrate();
	synthese::data_exchange::RTMFileFormat::integrate();
	synthese::data_exchange::HastusInterfaceFileFormat::integrate();
	synthese::data_exchange::HeuresFileFormat::integrate();
	synthese::data_exchange::IneoBDSIFileFormat::integrate();
	synthese::data_exchange::IneoDepartureBoardsFileFormat::integrate();
	synthese::data_exchange::IneoFileFormat::integrate();
	synthese::data_exchange::IneoOperationFileFormat::integrate();
	synthese::data_exchange::IneoRealTimeFileFormat::integrate();
	synthese::data_exchange::OGTFileFormat::integrate();
	synthese::data_exchange::ObitiFileFormat::integrate();
	synthese::data_exchange::PegaseFileFormat::integrate();
	synthese::data_exchange::RSSFileFormat::integrate();
	synthese::data_exchange::TridentFileFormat::integrate();
	synthese::data_exchange::StopsShapeFileFormat::integrate();
	synthese::data_exchange::ServicesCSVFileFormat::integrate();
	synthese::data_exchange::HousesCSVFileFormat::integrate();
	synthese::data_exchange::NavstreetsFileFormat::integrate();
	synthese::data_exchange::OSMFileFormat::integrate();
	synthese::data_exchange::RoadShapeFileFormat::integrate();
	synthese::data_exchange::IGNstreetsFileFormat::integrate();
	synthese::data_exchange::GpsFileFormat::integrate();



	// Registries
	synthese::util::Env::Integrate<synthese::data_exchange::VDVClient>();
	synthese::util::Env::Integrate<synthese::data_exchange::VDVServer>();
	synthese::util::Env::Integrate<synthese::data_exchange::VDVServerSubscription>();
}
