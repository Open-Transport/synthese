// 37 PT Operation

#include "PTOperationModule.hpp"

#include "VehicleTableSync.hpp"
#include "CompositionTableSync.hpp"
#include "VehiclePositionTableSync.hpp"
#include "DepotTableSync.hpp"

#include "GetCompositionsService.hpp"
#include "GetVehiclesService.hpp"

#include "DepotsAdmin.hpp"
#include "VehicleAdmin.hpp"
#include "VehiclesAdmin.hpp"
#include "PTOperationStatisticsAdmin.hpp"

#include "CompositionUpdateAction.hpp"
#include "CompositionVehicleAddAction.hpp"
#include "CompositionVehicleRemoveAction.hpp"
#include "VehicleUpdateAction.hpp"
#include "VehiclePositionUpdateAction.hpp"
#include "DepotUpdateAction.hpp"

#include "Composition.hpp"
#include "Depot.hpp"
#include "Vehicle.hpp"
#include "VehiclePosition.hpp"


#include "PTOperationModule.inc.cpp"

void synthese::pt_operation::moduleRegister()
{
	
	// 37 PT Operation
	synthese::pt_operation::CompositionTableSync::integrate();
	synthese::pt_operation::VehicleTableSync::integrate();
	synthese::pt_operation::VehiclePositionTableSync::integrate();
	synthese::pt_operation::DepotTableSync::integrate();
	
	synthese::pt_operation::PTOperationModule::integrate();
	
	synthese::pt_operation::GetCompositionsService::integrate();
	synthese::pt_operation::GetVehiclesService::integrate();
	
	synthese::pt_operation::CompositionVehicleAddAction::integrate();
	synthese::pt_operation::CompositionVehicleRemoveAction::integrate();
	synthese::pt_operation::CompositionUpdateAction::integrate();
	synthese::pt_operation::DepotUpdateAction::integrate();
	synthese::pt_operation::VehicleUpdateAction::integrate();
	synthese::pt_operation::VehiclePositionUpdateAction::integrate();
	
	synthese::pt_operation::DepotsAdmin::integrate();
	synthese::pt_operation::VehicleAdmin::integrate();
	synthese::pt_operation::VehiclesAdmin::integrate();
	synthese::pt_operation::PTOperationStatisticsAdmin::integrate();
	
	synthese::util::Env::Integrate<synthese::pt_operation::Composition>();
	synthese::util::Env::Integrate<synthese::pt_operation::Depot>();
	synthese::util::Env::Integrate<synthese::pt_operation::Vehicle>();
	synthese::util::Env::Integrate<synthese::pt_operation::VehiclePosition>();
	
}
