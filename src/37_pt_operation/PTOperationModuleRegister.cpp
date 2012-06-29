// 37 PT Operation

#include "PTOperationModule.hpp"

#include "CompositionTableSync.hpp"
#include "DeadRunTableSync.hpp"
#include "DepotTableSync.hpp"
#include "DriverActivityTableSync.hpp"
#include "DriverAllocationTableSync.hpp"
#include "DriverAllocationTemplateTableSync.hpp"
#include "DriverServiceTableSync.hpp"
#include "VehicleTableSync.hpp"
#include "VehiclePositionTableSync.hpp"
#include "VehicleServiceTableSync.hpp"

#include "DriverAllocationsListService.hpp"
#include "DriverAllocationTemplatesListService.hpp"
#include "DriverServicesListService.hpp"
#include "GetCompositionsService.hpp"
#include "GetVehiclesService.hpp"
#include "VehicleServicesListService.hpp"

#include "DepotAdmin.hpp"
#include "DepotsAdmin.hpp"
#include "DriverActivitiesAdmin.hpp"
#include "DriverActivityAdmin.hpp"
#include "DriverServiceAdmin.hpp"
#include "DriverServicesAdmin.hpp"
#include "VehicleAdmin.hpp"
#include "VehiclesAdmin.hpp"
#include "PTOperationStatisticsAdmin.hpp"
#include "VehicleServicesAdmin.hpp"
#include "VehicleServiceAdmin.hpp"

#include "CompositionUpdateAction.hpp"
#include "CompositionVehicleAddAction.hpp"
#include "CompositionVehicleRemoveAction.hpp"
#include "DepotUpdateAction.hpp"
#include "DriverServiceUpdateAction.hpp"
#include "RealTimeVehicleUpdateAction.hpp"
#include "VehicleUpdateAction.hpp"
#include "VehiclePositionUpdateAction.hpp"
#include "VehicleServiceUpdateAction.hpp"

#include "Composition.hpp"
#include "Depot.hpp"
#include "DeadRun.hpp"
#include "DeadRunEdge.hpp"
#include "DriverActivity.hpp"
#include "DriverAllocation.hpp"
#include "DriverAllocationTemplate.hpp"
#include "DriverService.hpp"
#include "Vehicle.hpp"
#include "VehiclePosition.hpp"
#include "VehicleService.hpp"

#include "HeuresOperationFileFormat.hpp"
#include "IneoOperationFileFormat.hpp"

#include "PTOperationModule.inc.cpp"

void synthese::pt_operation::moduleRegister()
{

	// 37 PT Operation
	synthese::pt_operation::DeadRunTableSync::integrate();
	synthese::pt_operation::DepotTableSync::integrate();
	synthese::pt_operation::DriverActivityTableSync::integrate();
	synthese::pt_operation::DriverAllocationTableSync::integrate();
	synthese::pt_operation::DriverAllocationTemplateTableSync::integrate();
	synthese::util::FactorableTemplate<synthese::db::DBTableSync, synthese::pt_operation::DriverServiceTableSync>::integrate();
	synthese::util::FactorableTemplate<synthese::db::Fetcher<synthese::calendar::Calendar>, synthese::pt_operation::DriverServiceTableSync>::integrate();
	synthese::pt_operation::CompositionTableSync::integrate();
	synthese::util::FactorableTemplate<synthese::pt_operation::CompositionTableSync, synthese::pt_operation::ServiceCompositionInheritedTableSync>::integrate();
	synthese::util::FactorableTemplate<synthese::db::Fetcher<synthese::calendar::Calendar>, synthese::pt_operation::ServiceCompositionInheritedTableSync>::integrate();
	synthese::util::FactorableTemplate<synthese::pt_operation::CompositionTableSync, synthese::pt_operation::VehicleServiceCompositionInheritedTableSync>::integrate();
	synthese::util::FactorableTemplate<synthese::db::Fetcher<synthese::calendar::Calendar>, synthese::pt_operation::VehicleServiceCompositionInheritedTableSync>::integrate();
	synthese::pt_operation::VehicleTableSync::integrate();
	synthese::pt_operation::VehiclePositionTableSync::integrate();
	synthese::pt_operation::VehicleServiceTableSync::integrate();

	synthese::pt_operation::PTOperationModule::integrate();

	synthese::pt_operation::GetCompositionsService::integrate();
	synthese::pt_operation::GetVehiclesService::integrate();
	synthese::pt_operation::DriverServicesListService::integrate();
	synthese::pt_operation::DriverAllocationsListService::integrate();
	synthese::pt_operation::DriverAllocationTemplatesListService::integrate();
	synthese::pt_operation::VehicleServicesListService::integrate();

	synthese::pt_operation::HeuresOperationFileFormat::integrate();
	synthese::pt_operation::IneoOperationFileFormat::integrate();

	synthese::pt_operation::CompositionVehicleAddAction::integrate();
	synthese::pt_operation::CompositionVehicleRemoveAction::integrate();
	synthese::pt_operation::CompositionUpdateAction::integrate();
	synthese::pt_operation::DepotUpdateAction::integrate();
	synthese::pt_operation::DriverServiceUpdateAction::integrate();
	synthese::pt_operation::RealTimeVehicleUpdateAction::integrate();
	synthese::pt_operation::VehicleUpdateAction::integrate();
	synthese::pt_operation::VehiclePositionUpdateAction::integrate();
	synthese::pt_operation::VehicleServiceUpdateAction::integrate();

	synthese::pt_operation::DepotAdmin::integrate();
	synthese::pt_operation::DepotsAdmin::integrate();
	synthese::pt_operation::DriverActivitiesAdmin::integrate();
	synthese::pt_operation::DriverActivityAdmin::integrate();
	synthese::pt_operation::DriverServiceAdmin::integrate();
	synthese::pt_operation::DriverServicesAdmin::integrate();
	synthese::pt_operation::VehicleAdmin::integrate();
	synthese::pt_operation::VehiclesAdmin::integrate();
	synthese::pt_operation::PTOperationStatisticsAdmin::integrate();
	synthese::pt_operation::VehicleServicesAdmin::integrate();
	synthese::pt_operation::VehicleServiceAdmin::integrate();

	synthese::util::Env::Integrate<synthese::pt_operation::Composition>();
	synthese::util::Env::Integrate<synthese::pt_operation::Depot>();
	synthese::util::Env::Integrate<synthese::pt_operation::DeadRun>();
	synthese::util::Env::Integrate<synthese::pt_operation::DeadRunEdge>();
	synthese::util::Env::Integrate<synthese::pt_operation::DriverActivity>();
	synthese::util::Env::Integrate<synthese::pt_operation::DriverAllocation>();
	synthese::util::Env::Integrate<synthese::pt_operation::DriverAllocationTemplate>();
	synthese::util::Env::Integrate<synthese::pt_operation::DriverService>();
	synthese::util::Env::Integrate<synthese::pt_operation::Vehicle>();
	synthese::util::Env::Integrate<synthese::pt_operation::VehiclePosition>();
	synthese::util::Env::Integrate<synthese::pt_operation::VehicleService>();
}
