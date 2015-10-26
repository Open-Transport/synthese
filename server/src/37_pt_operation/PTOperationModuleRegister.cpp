// 37 PT Operation

#include "PTOperationModule.hpp"

#include "DeadRunTableSync.hpp"
#include "DepotTableSync.hpp"
#include "DriverActivityTableSync.hpp"
#include "DriverAllocationTableSync.hpp"
#include "DriverAllocationTemplateTableSync.hpp"
#include "DriverServiceTableSync.hpp"
#include "OperationUnitTableSync.hpp"
#include "VehicleServiceTableSync.hpp"

#include "DriverAllocationsListService.hpp"
#include "DriverAllocationTemplatesListService.hpp"
#include "DriverServicesListService.hpp"
#include "OperationUnitsService.hpp"
#include "VehicleServicesListService.hpp"

#include "DepotAdmin.hpp"
#include "DepotsAdmin.hpp"
#include "DriverActivitiesAdmin.hpp"
#include "DriverActivityAdmin.hpp"
#include "DriverServiceAdmin.hpp"
#include "DriverServicesAdmin.hpp"
#include "PTOperationStatisticsAdmin.hpp"
#include "VehicleServicesAdmin.hpp"
#include "VehicleServiceAdmin.hpp"

#include "DepotUpdateAction.hpp"
#include "DriverServiceUpdateAction.hpp"

#include "Depot.hpp"
#include "DeadRun.hpp"
#include "DeadRunEdge.hpp"
#include "DriverActivity.hpp"
#include "DriverAllocation.hpp"
#include "DriverAllocationTemplate.hpp"
#include "DriverService.hpp"
#include "OperationUnit.hpp"
#include "VehicleService.hpp"

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
	synthese::pt_operation::OperationUnitTableSync::integrate();
	synthese::pt_operation::VehicleServiceTableSync::integrate();

	synthese::pt_operation::PTOperationModule::integrate();

	synthese::pt_operation::DriverServicesListService::integrate();
	synthese::pt_operation::DriverAllocationsListService::integrate();
	synthese::pt_operation::DriverAllocationTemplatesListService::integrate();
	synthese::pt_operation::OperationUnitsService::integrate();
	synthese::pt_operation::VehicleServicesListService::integrate();

	synthese::pt_operation::DepotUpdateAction::integrate();
	synthese::pt_operation::DriverServiceUpdateAction::integrate();

	synthese::pt_operation::DepotAdmin::integrate();
	synthese::pt_operation::DepotsAdmin::integrate();
	synthese::pt_operation::DriverActivitiesAdmin::integrate();
	synthese::pt_operation::DriverActivityAdmin::integrate();
	synthese::pt_operation::DriverServiceAdmin::integrate();
	synthese::pt_operation::DriverServicesAdmin::integrate();
	synthese::pt_operation::PTOperationStatisticsAdmin::integrate();
	synthese::pt_operation::VehicleServicesAdmin::integrate();
	synthese::pt_operation::VehicleServiceAdmin::integrate();

	INTEGRATE(synthese::pt_operation::Depot);
	synthese::util::Env::Integrate<synthese::pt_operation::DeadRun>();
	synthese::util::Env::Integrate<synthese::pt_operation::DeadRunEdge>();
	INTEGRATE(synthese::pt_operation::DriverActivity);
	INTEGRATE(synthese::pt_operation::DriverAllocation);
	INTEGRATE(synthese::pt_operation::DriverAllocationTemplate);
	INTEGRATE(synthese::pt_operation::DriverService);
	INTEGRATE(synthese::pt_operation::OperationUnit);
	INTEGRATE(synthese::pt_operation::VehicleService);
}
