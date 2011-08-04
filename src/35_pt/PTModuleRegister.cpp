
// PT MODULE 35

// Factories

#include "PTModule.h"

#include "LinesListFunction.h"
#include "LineStopGetService.hpp"
#include "StopAreasListFunction.hpp"
#include "StopPointsListFunction.hpp"
#include "RealTimeUpdateFunction.h"
#include "PhysicalStopsCSVExportFunction.h"
#include "PTNetworksListFunction.hpp"
#include "PTRoutesListFunction.hpp"
#include "PTRouteDetailFunction.hpp"
#include "CheckLineCalendarFunction.hpp"
#include "PTObjectInformationFunction.hpp"

#include "CarPostalFileFormat.hpp"
#include "GTFSFileFormat.hpp"
#include "HeuresFileFormat.hpp"
#include "IneoFileFormat.hpp"
#include "IneoRealTimeFileFormat.hpp"
#include "OGTFileFormat.hpp"
#include "PladisStopsFileFormat.hpp"
#include "TridentFileFormat.h"

#include "DestinationTableSync.hpp"
#include "PTUseRuleTableSync.h"
#include "ContinuousServiceTableSync.h"
#include "ScheduledServiceTableSync.h"
#include "JunctionTableSync.hpp"
#include "StopPointTableSync.hpp"
#include "TransportNetworkTableSync.h"
#include "NonConcurrencyRuleTableSync.h"
#include "RollingStockTableSync.h"
#include "ReservationContactTableSync.h"
#include "CommercialLineTableSync.h"
#include "StopAreaTableSync.hpp"
#include "FareTableSync.h"
#include "LineStopTableSync.h"
#include "LineAreaInheritedTableSync.hpp"
#include "DesignatedLinePhysicalStopInheritedTableSync.hpp"
#include "JourneyPatternTableSync.hpp"
#include "DRTAreaTableSync.hpp"

#include "DestinationAdmin.hpp"
#include "DestinationsAdmin.hpp"
#include "PTQualityControlAdmin.hpp"
#include "TransportNetworkAdmin.h"
#include "CommercialLineAdmin.h"
#include "JourneyPatternAdmin.hpp"
#include "ServiceAdmin.h"
#include "PTCitiesAdmin.h"
#include "PTPlacesAdmin.h"
#include "PTRoadsAdmin.h"
#include "PTRoadAdmin.h"
#include "PTNetworksAdmin.h"
#include "PTPlaceAdmin.h"
#include "PTUseRulesAdmin.h"
#include "PTUseRuleAdmin.h"
#include "StopPointAdmin.hpp"
#include "DRTAreaAdmin.hpp"
#include "DRTAreasAdmin.hpp"

#include "RealTimeUpdateScreenServiceInterfacePage.h"
#include "LineMarkerInterfacePage.h"

#include "CommercialLineAddAction.h"
#include "CommercialLineCalendarTemplateUpdateAction.hpp"
#include "CommercialLineUpdateAction.h"
#include "CopyGeometriesAction.hpp"
#include "DestinationUpdateAction.hpp"
#include "JourneyPatternAddAction.hpp"
#include "JunctionUpdateAction.hpp"
#include "NonConcurrencyRuleAddAction.h"
#include "ScheduleRealTimeUpdateAction.h"
#include "ServiceAddAction.h"
#include "ServiceVertexRealTimeUpdateAction.h"
#include "StopAreaUpdateAction.h"
#include "StopAreaTransferRemoveAction.hpp"
#include "TransportNetworkUpdateAction.hpp"
#include "ContinuousServiceUpdateAction.h"
#include "PlaceAliasUpdateAction.hpp"
#include "PTUseRuleUpdateAction.hpp"
#include "ServiceTimetableUpdateAction.h"
#include "ServiceUpdateAction.h"
#include "JourneyPatternUpdateAction.hpp"
#include "ServiceApplyCalendarAction.h"
#include "LineStopAddAction.h"
#include "ServiceDateChangeAction.h"
#include "StopPointUpdateAction.hpp"
#include "StopPointAddAction.hpp"
#include "StopAreaAddAction.h"
#include "LineStopUpdateAction.hpp"
#include "ProjectAllStopPointsAction.hpp"
#include "StopAreaTransferAddAction.h"
#include "DRTAreaUpdateAction.hpp"

#include "LineAlarmRecipient.hpp"
#include "StopAreaAlarmRecipient.hpp"

#include "TransportNetworkRight.h"

#include "StopPointWFSType.hpp"

// Registries

#include "Destination.hpp"
#include "JourneyPattern.hpp"
#include "LineArea.hpp"
#include "DesignatedLinePhysicalStop.hpp"
#include "StopArea.hpp"
#include "ReservationContact.h"
#include "CommercialLine.h"
#include "TransportNetwork.h"
#include "PTUseRule.h"
#include "Junction.hpp"
#include "StopPoint.hpp"
#include "RollingStock.h"
#include "Fare.h"
#include "ScheduledService.h"
#include "ContinuousService.h"
#include "NonConcurrencyRule.h"
#include "DRTArea.hpp"


#include "PTModule.inc.cpp"

void synthese::pt::moduleRegister()
{
	
	// PT MODULE 35
	
	// Factories
	
	synthese::pt::TransportNetworkTableSync::integrate();
	
	synthese::pt::DestinationTableSync::integrate();
	synthese::pt::PTUseRuleTableSync::integrate();
	synthese::util::FactorableTemplate<synthese::db::DBTableSync,synthese::pt::StopPointTableSync>::integrate();
	synthese::util::FactorableTemplate<synthese::db::Fetcher<synthese::graph::Vertex>, synthese::pt::StopPointTableSync>::integrate();
	synthese::pt::JunctionTableSync::integrate();
	synthese::util::FactorableTemplate<synthese::db::DBTableSync,synthese::pt::ContinuousServiceTableSync>::integrate();
	synthese::util::FactorableTemplate<synthese::db::Fetcher<synthese::pt::SchedulesBasedService>, synthese::pt::ContinuousServiceTableSync>::integrate();
	synthese::util::FactorableTemplate<synthese::db::Fetcher<synthese::graph::Service>, synthese::pt::ContinuousServiceTableSync>::integrate();
	synthese::util::FactorableTemplate<synthese::db::DBTableSync,synthese::pt::ScheduledServiceTableSync>::integrate();
	synthese::util::FactorableTemplate<synthese::db::Fetcher<synthese::pt::SchedulesBasedService>, synthese::pt::ScheduledServiceTableSync>::integrate();
	synthese::util::FactorableTemplate<synthese::db::Fetcher<synthese::graph::Service>, synthese::pt::ScheduledServiceTableSync>::integrate();
	synthese::pt::FareTableSync::integrate();
	synthese::pt::RollingStockTableSync::integrate();
	synthese::pt::NonConcurrencyRuleTableSync::integrate();
	synthese::pt::ReservationContactTableSync::integrate();
	synthese::pt::CommercialLineTableSync::integrate();
	synthese::pt::JourneyPatternTableSync::integrate();
	synthese::util::FactorableTemplate<synthese::db::DBTableSync,synthese::pt::StopAreaTableSync>::integrate();
	synthese::util::FactorableTemplate<synthese::db::Fetcher<synthese::geography::NamedPlace>, synthese::pt::StopAreaTableSync>::integrate();
	synthese::pt::LineStopTableSync::integrate();
	synthese::pt::LineAreaInheritedTableSync::integrate();
	synthese::pt::DesignatedLinePhysicalStopInheritedTableSync::integrate();
	synthese::pt::DRTAreaTableSync::integrate();
	
	synthese::pt::PTModule::integrate();
	
	synthese::pt::CarPostalFileFormat::integrate();
	synthese::pt::GTFSFileFormat::integrate();
	synthese::pt::HeuresFileFormat::integrate();
	synthese::pt::IneoFileFormat::integrate();
	synthese::pt::IneoRealTimeFileFormat::integrate();
	synthese::pt::OGTFileFormat::integrate();
	synthese::pt::PladisStopsFileFormat::integrate();
	synthese::pt::TridentFileFormat::integrate ();
	
	synthese::pt::DestinationAdmin::integrate();
	synthese::pt::DestinationsAdmin::integrate();
	synthese::pt::PTQualityControlAdmin::integrate();
	synthese::pt::ServiceAdmin::integrate();
	synthese::pt::TransportNetworkAdmin::integrate();
	synthese::pt::CommercialLineAdmin::integrate();
	synthese::pt::JourneyPatternAdmin::integrate();
	synthese::pt::PTCitiesAdmin::integrate();
	synthese::pt::PTPlacesAdmin::integrate();
	synthese::pt::PTRoadsAdmin::integrate();
	synthese::pt::PTRoadAdmin::integrate();
	synthese::pt::PTNetworksAdmin::integrate();
	synthese::pt::PTPlaceAdmin::integrate();
	synthese::pt::PTUseRuleAdmin::integrate();
	synthese::pt::PTUseRulesAdmin::integrate();
	synthese::pt::StopPointAdmin::integrate();
	synthese::pt::DRTAreaAdmin::integrate();
	synthese::pt::DRTAreasAdmin::integrate();
	
	synthese::pt::LineStopGetService::integrate();
	synthese::pt::RealTimeUpdateFunction::integrate();
	synthese::pt::PhysicalStopsCSVExportFunction::integrate();
	synthese::pt::LinesListFunction::integrate();
	synthese::pt::StopPointsListFunction::integrate();
	synthese::pt::StopAreasListFunction::integrate();
	synthese::pt::PTNetworksListFunction::integrate();
	synthese::pt::PTRoutesListFunction::integrate();
	synthese::pt::PTRouteDetailFunction::integrate();
	synthese::pt::CheckLineCalendarFunction::integrate();
	synthese::pt::PTObjectInformationFunction::integrate();
	
	synthese::pt::LineMarkerInterfacePage::integrate();
	synthese::pt::RealTimeUpdateScreenServiceInterfacePage::integrate();
	
	synthese::pt::CommercialLineAddAction::integrate();
	synthese::pt::CommercialLineCalendarTemplateUpdateAction::integrate();
	synthese::pt::CommercialLineUpdateAction::integrate();
	synthese::pt::ContinuousServiceUpdateAction::integrate();
	synthese::pt::CopyGeometriesAction::integrate();
	synthese::pt::DestinationUpdateAction::integrate();
	synthese::pt::DRTAreaUpdateAction::integrate();
	synthese::pt::JourneyPatternAddAction::integrate();
	synthese::pt::JunctionUpdateAction::integrate();
	synthese::pt::NonConcurrencyRuleAddAction::integrate();
	synthese::pt::PlaceAliasUpdateAction::integrate();
	synthese::pt::PTUseRuleUpdateAction::integrate();
	synthese::pt::ScheduleRealTimeUpdateAction::integrate();
	synthese::pt::ServiceAddAction::integrate();
	synthese::pt::ServiceApplyCalendarAction::integrate();
	synthese::pt::ServiceTimetableUpdateAction::integrate();
	synthese::pt::ServiceUpdateAction::integrate();
	synthese::pt::ServiceVertexRealTimeUpdateAction::integrate();
	synthese::pt::StopAreaUpdateAction::integrate();
	synthese::pt::StopAreaTransferRemoveAction::integrate();
	synthese::pt::TransportNetworkUpdateAction::integrate();
	synthese::pt::JourneyPatternUpdateAction::integrate();
	synthese::pt::LineStopAddAction::integrate();
	synthese::pt::ServiceDateChangeAction::integrate();
	synthese::pt::StopPointUpdateAction::integrate();
	synthese::pt::StopPointAddAction::integrate();
	synthese::pt::StopAreaAddAction::integrate();
	synthese::pt::LineStopUpdateAction::integrate();
	synthese::pt::ProjectAllStopPointsAction::integrate();
	synthese::pt::StopAreaTransferAddAction::integrate();
	
	synthese::pt::TransportNetworkRight::integrate();
	
	synthese::pt::StopArea::integrate();
	
	synthese::pt::StopPointWFSType::integrate();

	synthese::pt::LineAlarmRecipient::integrate();
	synthese::pt::StopAreaAlarmRecipient::integrate();
	
	// Registries
	synthese::util::Env::Integrate<synthese::pt::Destination>();
	synthese::util::Env::Integrate<synthese::pt::JourneyPattern>();
	synthese::util::Env::Integrate<synthese::pt::LineArea>();
	synthese::util::Env::Integrate<synthese::pt::DesignatedLinePhysicalStop>();
	synthese::util::Env::Integrate<synthese::pt::StopArea>();
	synthese::util::Env::Integrate<synthese::pt::CommercialLine>();
	synthese::util::Env::Integrate<synthese::pt::StopPoint>();
	synthese::util::Env::Integrate<synthese::pt::Junction>();
	synthese::util::Env::Integrate<synthese::pt::TransportNetwork>();
	synthese::util::Env::Integrate<synthese::pt::PTUseRule>();
	synthese::util::Env::Integrate<synthese::pt::ContinuousService>();
	synthese::util::Env::Integrate<synthese::pt::ScheduledService>();
	synthese::util::Env::Integrate<synthese::pt::RollingStock>();
	synthese::util::Env::Integrate<synthese::pt::Fare>();
	synthese::util::Env::Integrate<synthese::pt::NonConcurrencyRule>();
	synthese::util::Env::Integrate<synthese::pt::ReservationContact>();
	synthese::util::Env::Integrate<synthese::pt::DRTArea>();
	
}
