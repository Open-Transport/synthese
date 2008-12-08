
#include "34_departures_table/DeparturesTableModule.h"

#include "34_departures_table/DeparturesTableInterfaceElement.h"


#include "34_departures_table/DepartureTableRowInterfacePage.h"
#include "34_departures_table/ParseDisplayReturnInterfacePage.h"

#include "34_departures_table/DisplayMaintenanceAdmin.h"
#include "34_departures_table/DisplaySearchAdmin.h"
#include "34_departures_table/DisplayTypesAdmin.h"
#include "34_departures_table/BroadcastPointsAdmin.h"
#include "34_departures_table/DisplayAdmin.h"

#include "34_departures_table/DisplayTypeTableSync.h"
#include "34_departures_table/DisplayScreenTableSync.h"

#include "34_departures_table/CreateDisplayTypeAction.h"
#include "34_departures_table/UpdateDisplayTypeAction.h"
#include "34_departures_table/UpdateDisplayScreenAction.h"
#include "34_departures_table/CreateDisplayScreenAction.h"
#include "34_departures_table/DisplayTypeRemoveAction.h"
#include "34_departures_table/DisplayScreenRemove.h"

#include "34_departures_table/ArrivalDepartureTableRight.h"
#include "34_departures_table/DisplayMaintenanceRight.h"

#include "34_departures_table/ArrivalDepartureTableLog.h"
#include "34_departures_table/DisplayMaintenanceLog.h"
#include "34_departures_table/DisplayDataControlLog.h"

#include "34_departures_table/DisplayScreenAlarmRecipient.h"

#include "34_departures_table/AddPreselectionPlaceToDisplayScreen.h"
#include "34_departures_table/UpdateDisplayPreselectionParametersAction.h"
#include "34_departures_table/RemovePreselectionPlaceFromDisplayScreenAction.h"
#include "34_departures_table/UpdateAllStopsDisplayScreenAction.h"
#include "34_departures_table/AddDepartureStopToDisplayScreenAction.h"
#include "34_departures_table/AddForbiddenPlaceToDisplayScreen.h"
#include "34_departures_table/UpdateDisplayPreselectionParametersAction.h"
#include "34_departures_table/DisplayScreenAddDisplayedPlace.h"
#include "34_departures_table/DeparturesTableInterfacePage.h"
#include "34_departures_table/DeparturesTableTrackCell.h"
#include "34_departures_table/DeparturesTableTeamCell.h"
#include "34_departures_table/DeparturesTableTimeContentInterfaceElement.h"
#include "34_departures_table/DeparturesTableLineContentInterfaceElement.h"
#include "34_departures_table/DeparturesTableDestinationContentInterfaceElement.h"
#include "34_departures_table/DeparturesTableRowServiceNumberInterfaceElement.h"
#include "34_departures_table/DisplayScreenRemovePhysicalStopAction.h"
#include "34_departures_table/DisplayScreenRemoveDisplayedPlaceAction.h"
#include "34_departures_table/DisplayScreenRemoveForbiddenPlaceAction.h"
#include "34_departures_table/UpdateDisplayMaintenanceAction.h"
#include "34_departures_table/DisplayScreenHasAlarmValueInterfaceElement.h"
#include "34_departures_table/DisplayScreenAlarmContentValueInterfaceElement.h"

#include "34_departures_table/DisplayScreenSupervisionRequest.h"
#include "34_departures_table/DisplayScreenContentRequest.h"
#include "34_departures_table/AlarmTestOnDisplayScreenFunction.h"
#include "34_departures_table/DisplayScreenPhysicalStopFunction.h"

#include "DisplayType.h"
#include "DisplayScreen.h"