
// PT MODULE 35

// Factories

#include "PTModule.h"

#include "LineMarkerInterfacePage.h"

#include "DRTAreasListService.hpp"
#include "LinesListFunction.h"
#include "LineStopGetService.hpp"
#include "StopAreasListFunction.hpp"
#include "StopPointsListFunction.hpp"
#include "RealTimeUpdateFunction.h"
#include "PTNetworksListFunction.hpp"
#include "PTRoutesListFunction.hpp"
#include "PTRouteDetailFunction.hpp"
#include "PTObjectInformationFunction.hpp"
#include "ServiceQuotasListService.hpp"

#include "DestinationTableSync.hpp"
#include "FreeDRTAreaTableSync.hpp"
#include "PTUseRuleTableSync.h"
#include "ContinuousServiceTableSync.h"
#include "ScheduledServiceTableSync.h"
#include "JunctionTableSync.hpp"
#include "StopPointTableSync.hpp"
#include "TransportNetworkTableSync.h"
#include "NonConcurrencyRuleTableSync.h"
#include "ReservationContactTableSync.h"
#include "CommercialLineTableSync.h"
#include "StopAreaTableSync.hpp"
#include "LineStopTableSync.h"
#include "JourneyPatternTableSync.hpp"
#include "DRTAreaTableSync.hpp"
#include "ServiceQuotaTableSync.hpp"

#include "ContactCenterAdmin.hpp"
#include "ContactCentersAdmin.hpp"
#include "DestinationAdmin.hpp"
#include "DestinationsAdmin.hpp"
#include "FreeDRTAreaAdmin.hpp"
#include "FreeDRTTimeSlot.hpp"
#include "FreeDRTTimeSlotAdmin.hpp"
#include "FreeDRTTimeSlotTableSync.hpp"
#include "FreeDRTTimeSlotUpdateAction.hpp"
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

#include "CleanAllStopPointProjectionsAction.hpp"
#include "CommercialLineCalendarTemplateUpdateAction.hpp"
#include "CommercialLineUpdateAction.h"
#include "CopyGeometriesAction.hpp"
#include "DestinationUpdateAction.hpp"
#include "FreeDRTAreaUpdateAction.hpp"
#include "FreeDRTTimeSlotUpdateAction.hpp"
#include "GetRealTimeServices.hpp"
#include "JourneyPatternAddAction.hpp"
#include "JourneyPatternRankContinuityRestoreAction.hpp"
#include "JunctionUpdateAction.hpp"
#include "ScheduleRealTimeUpdateAction.h"
#include "ScheduleRealTimeUpdateService.h"
#include "ServiceAddAction.h"
#include "ServiceVertexUpdateAction.hpp"
#include "ServiceVertexRealTimeUpdateAction.h"
#include "StopAreaUpdateAction.h"
#include "StopAreaMergeAction.hpp"
#include "StopAreaTransferRemoveAction.hpp"
#include "TransportNetworkUpdateAction.hpp"
#include "ContinuousServiceUpdateAction.h"
#include "PlaceAliasUpdateAction.hpp"
#include "PTUseRuleUpdateAction.hpp"
#include "ServiceTimetableUpdateAction.h"
#include "ServiceUpdateAction.h"
#include "JourneyPatternUpdateAction.hpp"
#include "LineStopAddAction.h"
#include "StopPointUpdateAction.hpp"
#include "StopPointAddAction.hpp"
#include "StopAreaAddAction.h"
#include "LineStopUpdateAction.hpp"
#include "ProjectAllStopPointsAction.hpp"
#include "StopAreaTransferAddAction.h"
#include "ServiceQuotaUpdateAction.hpp"

#include "LineAlarmRecipient.hpp"
#include "StopAreaAlarmRecipient.hpp"

#include "TransportNetworkRight.h"

#include "StopPointWFSType.hpp"

#include "DeactivationPTDataInterSYNTHESE.hpp"
#include "RealTimePTDataInterSYNTHESE.hpp"

// Registries

#include "Destination.hpp"
#include "FreeDRTArea.hpp"
#include "FreeDRTTimeSlot.hpp"
#include "JourneyPattern.hpp"
#include "LineStop.h"
#include "StopArea.hpp"
#include "ReservationContact.h"
#include "CommercialLine.h"
#include "TransportNetwork.h"
#include "PTUseRule.h"
#include "Junction.hpp"
#include "StopPoint.hpp"
#include "ScheduledService.h"
#include "ContinuousService.h"
#include "NonConcurrencyRule.h"
#include "DRTArea.hpp"
#include "ServiceQuota.hpp"

#include "PTModule.inc.cpp"

void synthese::pt::moduleRegister()
{

	// PT MODULE 35

	// Factories
	synthese::pt::TransportNetworkTableSync::integrate();

	synthese::pt::DestinationTableSync::integrate();
	synthese::pt::DRTAreaTableSync::integrate();
	synthese::pt::FreeDRTAreaTableSync::integrate();
	synthese::pt::PTUseRuleTableSync::integrate();
	synthese::util::FactorableTemplate<synthese::db::DBTableSync,synthese::pt::StopPointTableSync>::integrate();
	synthese::util::FactorableTemplate<synthese::db::Fetcher<synthese::graph::Vertex>, synthese::pt::StopPointTableSync>::integrate();
	synthese::pt::JunctionTableSync::integrate();
	synthese::util::FactorableTemplate<synthese::db::DBTableSync,synthese::pt::ContinuousServiceTableSync>::integrate();
	synthese::util::FactorableTemplate<synthese::db::Fetcher<synthese::pt::SchedulesBasedService>, synthese::pt::ContinuousServiceTableSync>::integrate();
	synthese::util::FactorableTemplate<synthese::db::Fetcher<synthese::calendar::Calendar>, synthese::pt::ContinuousServiceTableSync>::integrate();
	synthese::util::FactorableTemplate<synthese::db::DBTableSync,synthese::pt::ScheduledServiceTableSync>::integrate();
	synthese::util::FactorableTemplate<synthese::db::Fetcher<synthese::pt::SchedulesBasedService>, synthese::pt::ScheduledServiceTableSync>::integrate();
	synthese::util::FactorableTemplate<synthese::db::Fetcher<synthese::calendar::Calendar>, synthese::pt::ScheduledServiceTableSync>::integrate();
	synthese::util::FactorableTemplate<synthese::db::DBTableSync,synthese::pt::FreeDRTTimeSlotTableSync>::integrate();
	synthese::util::FactorableTemplate<synthese::db::Fetcher<synthese::calendar::Calendar>, synthese::pt::FreeDRTTimeSlotTableSync>::integrate();
	synthese::pt::NonConcurrencyRuleTableSync::integrate();
	synthese::pt::ReservationContactTableSync::integrate();
	synthese::pt::CommercialLineTableSync::integrate();
	synthese::pt::JourneyPatternTableSync::integrate();
	synthese::util::FactorableTemplate<synthese::db::DBTableSync,synthese::pt::StopAreaTableSync>::integrate();
	synthese::util::FactorableTemplate<synthese::db::Fetcher<synthese::geography::NamedPlace>, synthese::pt::StopAreaTableSync>::integrate();
	synthese::pt::LineStopTableSync::integrate();
	synthese::pt::ServiceQuotaTableSync::integrate();

	synthese::pt::PTModule::integrate();

	synthese::pt::ContactCenterAdmin::integrate();
	synthese::pt::ContactCentersAdmin::integrate();
	synthese::pt::DestinationAdmin::integrate();
	synthese::pt::DestinationsAdmin::integrate();
	synthese::pt::FreeDRTAreaAdmin::integrate();
	synthese::pt::FreeDRTTimeSlotAdmin::integrate();
	synthese::pt::GetRealTimeServices::integrate();
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

	synthese::pt::DRTAreasListService::integrate();
	synthese::pt::LineStopGetService::integrate();
	synthese::pt::RealTimeUpdateFunction::integrate();
	synthese::pt::LinesListFunction::integrate();
	synthese::pt::StopPointsListFunction::integrate();
	synthese::pt::StopAreasListFunction::integrate();
	synthese::pt::PTNetworksListFunction::integrate();
	synthese::pt::PTRoutesListFunction::integrate();
	synthese::pt::PTRouteDetailFunction::integrate();
	synthese::pt::PTObjectInformationFunction::integrate();
	synthese::pt::ServiceQuotasListService::integrate();

	synthese::pt::CleanAllStopPointProjectionsAction::integrate();
	synthese::pt::CommercialLineCalendarTemplateUpdateAction::integrate();
	synthese::pt::CommercialLineUpdateAction::integrate();
	synthese::pt::ContinuousServiceUpdateAction::integrate();
	synthese::pt::CopyGeometriesAction::integrate();
	synthese::pt::DestinationUpdateAction::integrate();
	synthese::pt::FreeDRTAreaUpdateAction::integrate();
	synthese::pt::FreeDRTTimeSlotUpdateAction::integrate();
	synthese::pt::JourneyPatternAddAction::integrate();
	synthese::pt::JourneyPatternRankContinuityRestoreAction::integrate();
	synthese::pt::JunctionUpdateAction::integrate();
	synthese::pt::PlaceAliasUpdateAction::integrate();
	synthese::pt::PTUseRuleUpdateAction::integrate();
	synthese::pt::ScheduleRealTimeUpdateAction::integrate();
	synthese::pt::ScheduleRealTimeUpdateService::integrate();
	synthese::pt::ServiceAddAction::integrate();
	synthese::pt::ServiceTimetableUpdateAction::integrate();
	synthese::pt::ServiceUpdateAction::integrate();
	synthese::pt::ServiceVertexRealTimeUpdateAction::integrate();
	synthese::pt::ServiceVertexUpdateAction::integrate();
	synthese::pt::StopAreaUpdateAction::integrate();
	synthese::pt::StopAreaMergeAction::integrate();
	synthese::pt::StopAreaTransferRemoveAction::integrate();
	synthese::pt::TransportNetworkUpdateAction::integrate();
	synthese::pt::JourneyPatternUpdateAction::integrate();
	synthese::pt::LineStopAddAction::integrate();
	synthese::pt::StopPointUpdateAction::integrate();
	synthese::pt::StopPointAddAction::integrate();
	synthese::pt::StopAreaAddAction::integrate();
	synthese::pt::LineStopUpdateAction::integrate();
	synthese::pt::ProjectAllStopPointsAction::integrate();
	synthese::pt::StopAreaTransferAddAction::integrate();
	synthese::pt::ServiceQuotaUpdateAction::integrate();

	synthese::pt::LineMarkerInterfacePage::integrate();

	synthese::pt::TransportNetworkRight::integrate();

	synthese::pt::StopArea::integrate();

	synthese::pt::StopPointWFSType::integrate();

	synthese::pt::LineAlarmRecipient::integrate();
	synthese::pt::StopAreaAlarmRecipient::integrate();

	synthese::pt::DeactivationPTDataInterSYNTHESE::integrate();
	synthese::pt::RealTimePTDataInterSYNTHESE::integrate();

	// Registries
	INTEGRATE(synthese::pt::Destination);
	INTEGRATE(synthese::pt::DRTArea);
	synthese::util::Env::Integrate<synthese::pt::FreeDRTArea>();
	synthese::util::Env::Integrate<synthese::pt::FreeDRTTimeSlot>();
	synthese::util::Env::Integrate<synthese::pt::JourneyPattern>();
	INTEGRATE(synthese::pt::LineStop);
	synthese::util::Env::Integrate<synthese::pt::StopArea>();
	synthese::util::Env::Integrate<synthese::pt::CommercialLine>();
	synthese::util::Env::Integrate<synthese::pt::StopPoint>();
	synthese::util::Env::Integrate<synthese::pt::Junction>();
	synthese::util::Env::Integrate<synthese::pt::TransportNetwork>();
	synthese::util::Env::Integrate<synthese::pt::PTUseRule>();
	synthese::util::Env::Integrate<synthese::pt::ContinuousService>();
	synthese::util::Env::Integrate<synthese::pt::ScheduledService>();
	INTEGRATE(synthese::pt::NonConcurrencyRule);
	INTEGRATE(synthese::pt::ReservationContact);
	INTEGRATE(synthese::pt::ServiceQuota);
}
