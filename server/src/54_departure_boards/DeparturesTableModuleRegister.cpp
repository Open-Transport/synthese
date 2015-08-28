
// 54 Departure tables

// Registries
#include "DisplayType.h"
#include "DisplayScreen.h"
#include "DisplayMonitoringStatus.h"
#include "DisplayScreenCPU.h"
#include "PlaceWithDisplayBoards.hpp"

// Factories
#include "DeparturesTableModule.h"

#include "DeparturesTableInterfaceElement.h"
#include "DeparturesTableLineContentInterfaceElement.h"
#include "DeparturesTableDestinationContentInterfaceElement.h"

#include "DepartureTableRowInterfacePage.h"
#include "ParseDisplayReturnInterfacePage.h"
#include "DeparturesTableInterfacePage.h"
#include "DeparturesTableDestinationInterfacepage.h"
#include "DeparturesTableTransferDestinationInterfacePage.h"

#include "DisplaySearchAdmin.h"
#include "DisplayTypesAdmin.h"
#include "BroadcastPointsAdmin.h"
#include "DisplayAdmin.h"
#include "DisplayTypeAdmin.h"
#include "DisplayScreenCPUAdmin.h"
#include "DeparturesTableBenchmarkAdmin.h"

#include "DisplayTypeTableSync.h"
#include "DisplayScreenTableSync.h"
#include "DisplayMonitoringStatusTableSync.h"
#include "DisplayScreenCPUTableSync.h"

#include "CreateDisplayTypeAction.h"
#include "UpdateDisplayTypeAction.h"
#include "UpdateDisplayScreenAction.h"
#include "CreateDisplayScreenAction.h"
#include "DisplayScreenAppearanceUpdateAction.h"
#include "AddPreselectionPlaceToDisplayScreenAction.h"
#include "UpdateDisplayPreselectionParametersAction.h"
#include "RemovePreselectionPlaceFromDisplayScreenAction.h"
#include "UpdateAllStopsDisplayScreenAction.h"
#include "AddDepartureStopToDisplayScreenAction.h"
#include "AddForbiddenPlaceToDisplayScreenAction.h"
#include "UpdateDisplayPreselectionParametersAction.h"
#include "DisplayScreenRemovePhysicalStopAction.h"
#include "DisplayScreenRemoveDisplayedPlaceAction.h"
#include "DisplayScreenRemoveForbiddenPlaceAction.h"
#include "UpdateDisplayMaintenanceAction.h"
#include "DisplayScreenAddDisplayedPlaceAction.h"
#include "DisplayScreenCPUCreateAction.h"
#include "DisplayScreenCPUUpdateAction.h"
#include "DisplayScreenCPUMaintenanceUpdateAction.h"
#include "DisplayScreenTransferDestinationAddAction.h"
#include "DisplayScreenTransferDestinationRemoveAction.h"
#include "DisplayScreenUpdateDisplayedStopAreaAction.hpp"

#include "ArrivalDepartureTableRight.h"
#include "DisplayMaintenanceRight.h"

#include "ArrivalDepartureTableLog.h"
#include "DisplayMaintenanceLog.h"

#include "DisplayScreenSupervisionFunction.h"
#include "DisplayScreenContentFunction.h"
#include "DisplayTypesService.hpp"
#include "AlarmTestOnDisplayScreenFunction.h"
#include "DisplayGetNagiosStatusFunction.h"
#include "CentreonConfigExportFunction.h"
#include "CPUGetWiredScreensFunction.h"
#include "GetDepartureBoardsService.hpp"
#include "TransferLinesService.hpp"

#include "DeparturesTableModule.inc.cpp"

void synthese::departure_boards::moduleRegister()
{

	synthese::departure_boards::DeparturesTableModule::integrate();

	synthese::departure_boards::DeparturesTableInterfaceElement::integrate();
	synthese::departure_boards::DeparturesTableLineContentInterfaceElement::integrate();
	synthese::departure_boards::DeparturesTableDestinationContentInterfaceElement::integrate();

	synthese::departure_boards::DeparturesTableInterfacePage::integrate();
	synthese::departure_boards::DepartureTableRowInterfacePage::integrate();
	synthese::departure_boards::ParseDisplayReturnInterfacePage::integrate();
	synthese::departure_boards::DeparturesTableTransferDestinationInterfacePage::integrate();
	synthese::departure_boards::DeparturesTableDestinationInterfacepage::integrate();

	synthese::departure_boards::DisplaySearchAdmin::integrate();
	synthese::departure_boards::DisplayTypesAdmin::integrate();
	synthese::departure_boards::BroadcastPointsAdmin::integrate();
	synthese::departure_boards::DisplayAdmin::integrate();
	synthese::departure_boards::DisplayTypeAdmin::integrate();
	synthese::departure_boards::DisplayScreenCPUAdmin::integrate();
	synthese::departure_boards::DeparturesTableBenchmarkAdmin::integrate();

	synthese::departure_boards::DisplayTypeTableSync::integrate();
	synthese::departure_boards::DisplayScreenTableSync::integrate();
	synthese::departure_boards::DisplayMonitoringStatusTableSync::integrate();
	synthese::departure_boards::DisplayScreenCPUTableSync::integrate();

	synthese::departure_boards::CreateDisplayTypeAction::integrate();
	synthese::departure_boards::UpdateDisplayTypeAction::integrate();
	synthese::departure_boards::CreateDisplayScreenAction::integrate();
	synthese::departure_boards::UpdateDisplayScreenAction::integrate();
	synthese::departure_boards::AddPreselectionPlaceToDisplayScreenAction::integrate();
	synthese::departure_boards::RemovePreselectionPlaceFromDisplayScreenAction::integrate();
	synthese::departure_boards::UpdateAllStopsDisplayScreenAction::integrate();
	synthese::departure_boards::AddDepartureStopToDisplayScreenAction::integrate();
	synthese::departure_boards::AddForbiddenPlaceToDisplayScreenAction::integrate();
	synthese::departure_boards::UpdateDisplayPreselectionParametersAction::integrate();
	synthese::departure_boards::DisplayScreenAddDisplayedPlaceAction::integrate();
	synthese::departure_boards::DisplayScreenRemovePhysicalStopAction::integrate();
	synthese::departure_boards::DisplayScreenRemoveDisplayedPlaceAction::integrate();
	synthese::departure_boards::DisplayScreenRemoveForbiddenPlaceAction::integrate();
	synthese::departure_boards::UpdateDisplayMaintenanceAction::integrate();
	synthese::departure_boards::DisplayScreenAppearanceUpdateAction::integrate();
	synthese::departure_boards::DisplayScreenCPUCreateAction::integrate();
	synthese::departure_boards::DisplayScreenCPUUpdateAction::integrate();
	synthese::departure_boards::DisplayScreenCPUMaintenanceUpdateAction::integrate();
	synthese::departure_boards::DisplayScreenTransferDestinationAddAction::integrate();
	synthese::departure_boards::DisplayScreenTransferDestinationRemoveAction::integrate();
	synthese::departure_boards::DisplayScreenUpdateDisplayedStopAreaAction::integrate();

	synthese::departure_boards::DisplayScreenContentFunction::integrate();
	synthese::departure_boards::DisplayScreenSupervisionFunction::integrate();
	synthese::departure_boards::DisplayTypesService::integrate();
	synthese::departure_boards::AlarmTestOnDisplayScreenFunction::integrate();
	synthese::departure_boards::DisplayGetNagiosStatusFunction::integrate();
	synthese::departure_boards::CentreonConfigExportFunction::integrate();
	synthese::departure_boards::CPUGetWiredScreensFunction::integrate();
	synthese::departure_boards::GetDepartureBoardsService::integrate();
	synthese::departure_boards::TransferLinesService::integrate();
	
	synthese::departure_boards::ArrivalDepartureTableRight::integrate();
	synthese::departure_boards::DisplayMaintenanceRight::integrate();

	synthese::departure_boards::ArrivalDepartureTableLog::integrate();
	synthese::departure_boards::DisplayMaintenanceLog::integrate();

	synthese::departure_boards::DisplayScreen::integrate();

	// Registries
	INTEGRATE(synthese::departure_boards::DisplayScreen);
	INTEGRATE(synthese::departure_boards::DisplayType);
	synthese::util::Env::Integrate<synthese::departure_boards::DisplayMonitoringStatus>();
	synthese::util::Env::Integrate<synthese::departure_boards::DisplayScreenCPU>();
	synthese::util::Env::Integrate<synthese::departure_boards::PlaceWithDisplayBoards>();
}
