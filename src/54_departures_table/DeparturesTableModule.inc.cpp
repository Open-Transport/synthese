
// 54 Departure tables

// Registries
#include "DisplayType.h"
#include "DisplayScreen.h"
#include "DisplayMonitoringStatus.h"
#include "DisplayScreenCPU.h"

// Factories
#include "DeparturesTableModule.h"

#include "DeparturesTableInterfaceElement.h"
#include "DisplayScreenHasAlarmValueInterfaceElement.h"
#include "DisplayScreenAlarmContentValueInterfaceElement.h"
#include "DeparturesTableTrackCell.h"
#include "DeparturesTableTeamCell.h"
#include "DeparturesTableTimeContentInterfaceElement.h"
#include "DeparturesTableLineContentInterfaceElement.h"
#include "DeparturesTableDestinationContentInterfaceElement.h"
#include "DeparturesTableRowServiceNumberInterfaceElement.h"

#include "DepartureTableRowInterfacePage.h"
#include "ParseDisplayReturnInterfacePage.h"
#include "DeparturesTableInterfacePage.h"

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
#include "DisplayTypeRemoveAction.h"
#include "DisplayScreenRemove.h"
#include "DisplayScreenAppearanceUpdateAction.h"
#include "AddPreselectionPlaceToDisplayScreen.h"
#include "UpdateDisplayPreselectionParametersAction.h"
#include "RemovePreselectionPlaceFromDisplayScreenAction.h"
#include "UpdateAllStopsDisplayScreenAction.h"
#include "AddDepartureStopToDisplayScreenAction.h"
#include "AddForbiddenPlaceToDisplayScreen.h"
#include "UpdateDisplayPreselectionParametersAction.h"
#include "DisplayScreenRemovePhysicalStopAction.h"
#include "DisplayScreenRemoveDisplayedPlaceAction.h"
#include "DisplayScreenRemoveForbiddenPlaceAction.h"
#include "UpdateDisplayMaintenanceAction.h"
#include "DisplayScreenAddDisplayedPlace.h"
#include "DisplayScreenCPUCreateAction.h"
#include "DisplayScreenCPUUpdateAction.h"
#include "DisplayScreenCPUMaintenanceUpdateAction.h"

#include "ArrivalDepartureTableRight.h"
#include "DisplayMaintenanceRight.h"

#include "ArrivalDepartureTableLog.h"
#include "DisplayMaintenanceLog.h"

#include "DisplayScreenAlarmRecipient.h"

#include "DisplayScreenSupervisionRequest.h"
#include "DisplayScreenContentRequest.h"
#include "AlarmTestOnDisplayScreenFunction.h"
#include "DisplayScreenPhysicalStopFunction.h"
#include "DisplayGetNagiosStatusFunction.h"
#include "CentreonConfigExportFunction.h"
#include "CPUGetWiredScreensFunction.h"
