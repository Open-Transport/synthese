
// VEHICLE MODULE 38

// Factories

#include "VehicleModule.hpp"

#include "CompositionTableSync.hpp"
#include "DescentTableSync.hpp"
#include "RollingStockTableSync.hpp"
#include "VehicleTableSync.hpp"
#include "VehicleCallTableSync.hpp"
#include "VehiclePositionTableSync.hpp"

#include "GetCompositionsService.hpp"
#include "GetVehiclesService.hpp"
#include "VehicleInformationsService.hpp"

#include "RollingStockAdmin.hpp"
#include "RollingStocksAdmin.hpp"
#include "VehicleAdmin.hpp"
#include "VehiclesAdmin.hpp"

#include "CompositionUpdateAction.hpp"
#include "CompositionVehicleAddAction.hpp"
#include "CompositionVehicleRemoveAction.hpp"
#include "RealTimeVehicleUpdateAction.hpp"
#include "RollingStockUpdateAction.hpp"
#include "VehiclePositionUpdateAction.hpp"

// Registries
#include "Composition.hpp"
#include "Descent.hpp"
#include "RollingStock.hpp"
#include "Vehicle.hpp"
#include "VehicleCall.hpp"
#include "VehiclePosition.hpp"

#include "VehicleModule.inc.cpp"

void synthese::vehicle::moduleRegister()
{

	// VEHICLE MODULE 38

	// Factories

	synthese::util::FactorableTemplate<synthese::db::DBTableSync,synthese::vehicle::CompositionTableSync>::integrate();
	synthese::util::FactorableTemplate<synthese::db::Fetcher<synthese::calendar::Calendar>, synthese::vehicle::CompositionTableSync>::integrate();
	synthese::vehicle::RollingStockTableSync::integrate();
	synthese::vehicle::VehicleTableSync::integrate();
	synthese::vehicle::VehicleCallTableSync::integrate();
	synthese::vehicle::VehiclePositionTableSync::integrate();
	synthese::vehicle::DescentTableSync::integrate();

	synthese::vehicle::RollingStockAdmin::integrate();
	synthese::vehicle::RollingStocksAdmin::integrate();
	synthese::vehicle::VehicleAdmin::integrate();
	synthese::vehicle::VehiclesAdmin::integrate();

	synthese::vehicle::CompositionVehicleAddAction::integrate();
	synthese::vehicle::CompositionVehicleRemoveAction::integrate();
	synthese::vehicle::CompositionUpdateAction::integrate();
	synthese::vehicle::RealTimeVehicleUpdateAction::integrate();
	synthese::vehicle::RollingStockUpdateAction::integrate();
	synthese::vehicle::VehiclePositionUpdateAction::integrate();

	synthese::vehicle::GetCompositionsService::integrate();
	synthese::vehicle::GetVehiclesService::integrate();
	synthese::vehicle::VehicleInformationsService::integrate();

	synthese::vehicle::VehicleModule::integrate();

	// Registries
	synthese::util::Env::Integrate<synthese::vehicle::Composition>();
	INTEGRATE(synthese::vehicle::Descent);
	INTEGRATE(synthese::vehicle::RollingStock);
	INTEGRATE(synthese::vehicle::Vehicle);
	INTEGRATE(synthese::vehicle::VehicleCall);
	synthese::util::Env::Integrate<synthese::vehicle::VehiclePosition>();
}
