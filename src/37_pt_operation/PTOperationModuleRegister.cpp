// 37 PT Operation

#include "PTOperationModule.hpp"

#include "CompositionTableSync.hpp"
#include "DepotTableSync.hpp"
#include "VehicleTableSync.hpp"
#include "VehiclePositionTableSync.hpp"
#include "VehicleServiceTableSync.hpp"
#include "DeadRunTableSync.hpp"

#include "GetCompositionsService.hpp"
#include "GetVehiclesService.hpp"

#include "DepotsAdmin.hpp"
#include "VehicleAdmin.hpp"
#include "VehiclesAdmin.hpp"
#include "PTOperationStatisticsAdmin.hpp"
#include "VehicleServicesAdmin.hpp"
#include "VehicleServiceAdmin.hpp"

#include "CompositionUpdateAction.hpp"
#include "CompositionVehicleAddAction.hpp"
#include "CompositionVehicleRemoveAction.hpp"
#include "VehicleUpdateAction.hpp"
#include "VehiclePositionUpdateAction.hpp"
#include "DepotUpdateAction.hpp"
#include "RealTimeVehicleUpdateAction.hpp"

#include "Composition.hpp"
#include "Depot.hpp"
#include "DeadRun.hpp"
#include "DeadRunEdge.hpp"
#include "Vehicle.hpp"
#include "VehiclePosition.hpp"
#include "VehicleService.hpp"

#include "HeuresOperationFileFormat.hpp"

#include "PTOperationModule.inc.cpp"

void synthese::pt_operation::moduleRegister()
{
	
	// 37 PT Operation
	synthese::pt_operation::CompositionTableSync::integrate();
	synthese::pt_operation::ServiceCompositionInheritedTableSync::integrate();
	synthese::pt_operation::VehicleServiceCompositionInheritedTableSync::integrate();
	synthese::pt_operation::DepotTableSync::integrate();
	synthese::pt_operation::DeadRunTableSync::integrate();
	synthese::pt_operation::VehicleTableSync::integrate();
	synthese::pt_operation::VehiclePositionTableSync::integrate();
	synthese::pt_operation::VehicleServiceTableSync::integrate();
	
	synthese::pt_operation::PTOperationModule::integrate();
	
	synthese::pt_operation::GetCompositionsService::integrate();
	synthese::pt_operation::GetVehiclesService::integrate();

	synthese::pt_operation::HeuresOperationFileFormat::integrate();
	
	synthese::pt_operation::CompositionVehicleAddAction::integrate();
	synthese::pt_operation::CompositionVehicleRemoveAction::integrate();
	synthese::pt_operation::CompositionUpdateAction::integrate();
	synthese::pt_operation::DepotUpdateAction::integrate();
	synthese::pt_operation::VehicleUpdateAction::integrate();
	synthese::pt_operation::VehiclePositionUpdateAction::integrate();
	synthese::pt_operation::RealTimeVehicleUpdateAction::integrate();
	
	synthese::pt_operation::DepotsAdmin::integrate();
	synthese::pt_operation::VehicleAdmin::integrate();
	synthese::pt_operation::VehiclesAdmin::integrate();
	synthese::pt_operation::PTOperationStatisticsAdmin::integrate();
	synthese::pt_operation::VehicleServicesAdmin::integrate();
	synthese::pt_operation::VehicleServiceAdmin::integrate();

	synthese::util::Env::Integrate<synthese::pt_operation::Composition>();
	synthese::util::Env::Integrate<synthese::pt_operation::Depot>();
	synthese::util::Env::Integrate<synthese::pt_operation::DeadRun>();
	synthese::util::Env::Integrate<synthese::pt_operation::DeadRunEdge>();
	synthese::util::Env::Integrate<synthese::pt_operation::Vehicle>();
	synthese::util::Env::Integrate<synthese::pt_operation::VehiclePosition>();
	synthese::util::Env::Integrate<synthese::pt_operation::VehicleService>();
	
}
