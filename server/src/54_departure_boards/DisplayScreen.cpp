////////////////////////////////////////////////////////////////////////////////
/// DisplayScreen class implementation.
///	@file DisplayScreen.cpp
///	@author Hugues Romain
///
///	This file belongs to the SYNTHESE project (public transportation specialized
///	software)
///	Copyright (C) 2002 Hugues Romain - RCSmobility <contact@rcsmobility.com>
///
///	This program is free software; you can redistribute it and/or
///	modify it under the terms of the GNU General Public License
///	as published by the Free Software Foundation; either version 2
///	of the License, or (at your option) any later version.
///
///	This program is distributed in the hope that it will be useful,
///	but WITHOUT ANY WARRANTY; without even the implied warranty of
///	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
///	GNU General Public License for more details.
///
///	You should have received a copy of the GNU General Public License
///	along with this program; if not, write to the Free Software Foundation,
///	Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
////////////////////////////////////////////////////////////////////////////////

#include "DisplayScreen.h"

#include "AlgorithmLogger.hpp"
#include "BroadcastPointAlarmRecipient.hpp"
#include "CityTableSync.h"
#include "CommercialLineTableSync.h"
#include "Conversion.h"
#include "DBModule.h"
#include "DeparturesTableModule.h"
#include "DisplayMaintenanceLog.h"
#include "DisplayMonitoringStatus.h"
#include "DisplayScreenContentFunction.h"
#include "DisplayScreenCPU.h"
#include "DisplayScreenCPUTableSync.h"
#include "DisplayScreenTableSync.h"
#include "DisplayTypeTableSync.h"
#include "DisplayTypeTableSync.h"
#include "ImportableTableSync.hpp"
#include "JourneyPattern.hpp"
#include "LineAlarmRecipient.hpp"
#include "LinePhysicalStop.hpp"
#include "PTRoutePlannerResult.h"
#include "PTTimeSlotRoutePlanner.h"
#include "PTUseRule.h"
#include "RoutePlanningTableGenerator.h"
#include "StopAreaTableSync.hpp"
#include "StopAreaAlarmRecipient.hpp"
#include "StopPointTableSync.hpp"
#include "TransportNetworkTableSync.h"
#include "TreeFolderTableSync.hpp"
#include "Webpage.h"

#include <sstream>
#include <boost/foreach.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/date_time/gregorian_calendar.hpp>

using namespace std;
using namespace boost;
using namespace boost::posix_time;
using namespace boost::gregorian;


namespace synthese
{
	using namespace algorithm;
	using namespace db;
	using namespace dblog;
	using namespace departure_boards;
	using namespace geography;
	using namespace graph;
	using namespace messages;
	using namespace pt;
	using namespace pt_journey_planner;
	using namespace road;
	using namespace tree;
	using namespace util;

	CLASS_DEFINITION(DisplayScreen, "t041_display_screens", 41)
	FIELD_DEFINITION_OF_OBJECT(DisplayScreen, "display_screen_id", "display_screen_ids")
	
	FIELD_DEFINITION_OF_TYPE(BroadCastPoint, "broadcast_point_id", SQL_INTEGER)
	FIELD_DEFINITION_OF_TYPE(BroadCastPointComment, "broadcast_point_comment", SQL_TEXT)
	FIELD_DEFINITION_OF_TYPE(DisplayTypePtr, "type_id", SQL_INTEGER)
	FIELD_DEFINITION_OF_TYPE(WiringCode, "wiring_code", SQL_INTEGER)
	FIELD_DEFINITION_OF_TYPE(BlinkingDelay, "blinking_delay", SQL_INTEGER)
	FIELD_DEFINITION_OF_TYPE(TrackNumberDisplay, "track_number_display", SQL_BOOLEAN)
	FIELD_DEFINITION_OF_TYPE(ServiceNumberDisplay, "service_number_display", SQL_BOOLEAN)
	FIELD_DEFINITION_OF_TYPE(DisplayTeam, "display_team", SQL_BOOLEAN)
	FIELD_DEFINITION_OF_TYPE(PhysicalStops, "physical_stops_ids", SQL_TEXT)
	FIELD_DEFINITION_OF_TYPE(AllPhysicalDisplayed, "all_physicals", SQL_BOOLEAN)
	FIELD_DEFINITION_OF_TYPE(ForbiddenArrivalPlaces, "forbidden_arrival_places_ids", SQL_TEXT)
	FIELD_DEFINITION_OF_TYPE(AllowedLines, "allowed_lines_ids", SQL_TEXT)
	FIELD_DEFINITION_OF_TYPE(departure_boards::Direction, "direction", SQL_INTEGER)
	FIELD_DEFINITION_OF_TYPE(OriginsOnly, "origins_only", SQL_INTEGER)
	FIELD_DEFINITION_OF_TYPE(DisplayedPlaces, "displayed_places_ids", SQL_TEXT)
	FIELD_DEFINITION_OF_TYPE(MaxDelay, "max_delay", SQL_INTEGER)
	FIELD_DEFINITION_OF_TYPE(ClearingDelay, "clearing_delay", SQL_INTEGER)
	FIELD_DEFINITION_OF_TYPE(FirstRow, "first_row", SQL_INTEGER)
	FIELD_DEFINITION_OF_TYPE(GenerationMethodCode, "generation_method", SQL_INTEGER)
	FIELD_DEFINITION_OF_TYPE(ForcedDestinations, "forced_destinations_ids", SQL_TEXT)
	FIELD_DEFINITION_OF_TYPE(DestinationForceDelay, "destination_force_delay", SQL_INTEGER)
	FIELD_DEFINITION_OF_TYPE(MaintenanceChecksPerDay, "maintenance_checks", SQL_INTEGER)
	FIELD_DEFINITION_OF_TYPE(MaintenanceIsOnline, "is_online", SQL_BOOLEAN)
	FIELD_DEFINITION_OF_TYPE(MaintenanceMessage, "maintenance_message", SQL_TEXT)
	FIELD_DEFINITION_OF_TYPE(DisplayClock, "display_clock", SQL_BOOLEAN)
	FIELD_DEFINITION_OF_TYPE(ComPort, "com_port", SQL_INTEGER)
	FIELD_DEFINITION_OF_TYPE(CpuHost, "cpu_host_id", SQL_INTEGER)
	FIELD_DEFINITION_OF_TYPE(MacAddress, "mac_address", SQL_TEXT)
	FIELD_DEFINITION_OF_TYPE(RoutePlanningWithTransfer, "route_planning_with_transfer", SQL_BOOLEAN)
	FIELD_DEFINITION_OF_TYPE(TransferDestinations, "transfer_destinations", SQL_TEXT)
	FIELD_DEFINITION_OF_TYPE(Up, "up_id", SQL_INTEGER)
	FIELD_DEFINITION_OF_TYPE(SubScreenTypeCode, "sub_screen_type", SQL_INTEGER)
	FIELD_DEFINITION_OF_TYPE(AllowCanceled, "allow_canceled", SQL_BOOLEAN)
	FIELD_DEFINITION_OF_TYPE(StopPointLocation, "stop_point_location", SQL_INTEGER)
	FIELD_DEFINITION_OF_TYPE(MaxTransferDuration, "max_transfer_duration", SQL_INTEGER)
	
	namespace util
	{
		template<>
		const string FactorableTemplate<BroadcastPoint, DisplayScreen>::FACTORY_KEY = "DisplayScreen";
	}

	namespace departure_boards
	{
		const std::string DisplayScreen::DATA_SCREEN_ID("screen_id");
		const std::string DisplayScreen::DATA_MAC_ADDRESS("mac");
		const std::string DisplayScreen::DATA_TITLE("title");
		const std::string DisplayScreen::DATA_IS_ONLINE("is_online");
		const std::string DisplayScreen::DATA_MAINTENANCE_MESSAGE("maintenance_message");
		const std::string DisplayScreen::DATA_NAME("name");
		const std::string DisplayScreen::DATA_TYPE_ID("type_id");
		const std::string DisplayScreen::DATA_LOCATION_ID("location_id");
		const std::string DisplayScreen::DATA_CPU_ID("cpu_id");
		const std::string DisplayScreen::VALUE_DISPLAY_SCREEN = "display_screen";
		const std::string DisplayScreen::DATA_X("x");
		const std::string DisplayScreen::DATA_Y("y");



		DisplayScreen::DisplayScreen(
			RegistryKeyType key
		):	Registrable(key),
			Object<DisplayScreen, DisplayScreenSchema>(
				Schema(
					FIELD_VALUE_CONSTRUCTOR(Key, key),
					FIELD_DEFAULT_CONSTRUCTOR(BroadCastPoint),
					FIELD_DEFAULT_CONSTRUCTOR(BroadCastPointComment),
					FIELD_DEFAULT_CONSTRUCTOR(DisplayTypePtr),
					FIELD_VALUE_CONSTRUCTOR(WiringCode, 0),
					FIELD_DEFAULT_CONSTRUCTOR(Title),
					FIELD_VALUE_CONSTRUCTOR(BlinkingDelay, 1),
					FIELD_VALUE_CONSTRUCTOR(TrackNumberDisplay, false),
					FIELD_VALUE_CONSTRUCTOR(ServiceNumberDisplay, false),
					FIELD_VALUE_CONSTRUCTOR(DisplayTeam, false),
					FIELD_DEFAULT_CONSTRUCTOR(PhysicalStops),
					FIELD_VALUE_CONSTRUCTOR(AllPhysicalDisplayed, true),
					FIELD_DEFAULT_CONSTRUCTOR(ForbiddenArrivalPlaces),
					FIELD_DEFAULT_CONSTRUCTOR(AllowedLines),
					FIELD_DEFAULT_CONSTRUCTOR(departure_boards::Direction),
					FIELD_DEFAULT_CONSTRUCTOR(OriginsOnly),
					FIELD_DEFAULT_CONSTRUCTOR(DisplayedPlaces),
					FIELD_VALUE_CONSTRUCTOR(MaxDelay, 12 * 60),// default = 24 hours
					FIELD_VALUE_CONSTRUCTOR(ClearingDelay, 0),
					FIELD_VALUE_CONSTRUCTOR(FirstRow, 0),
					FIELD_DEFAULT_CONSTRUCTOR(GenerationMethodCode),
					FIELD_DEFAULT_CONSTRUCTOR(ForcedDestinations),
					FIELD_VALUE_CONSTRUCTOR(DestinationForceDelay, 120),// default = 2 hours
					FIELD_DEFAULT_CONSTRUCTOR(MaintenanceChecksPerDay),
					FIELD_VALUE_CONSTRUCTOR(MaintenanceIsOnline, true),
					FIELD_DEFAULT_CONSTRUCTOR(MaintenanceMessage),
					FIELD_VALUE_CONSTRUCTOR(DisplayClock, true),
					FIELD_VALUE_CONSTRUCTOR(ComPort, 0),
					FIELD_DEFAULT_CONSTRUCTOR(CpuHost),
					FIELD_DEFAULT_CONSTRUCTOR(MacAddress),
					FIELD_VALUE_CONSTRUCTOR(RoutePlanningWithTransfer, false),
					FIELD_DEFAULT_CONSTRUCTOR(TransferDestinations),
					FIELD_DEFAULT_CONSTRUCTOR(Up),
					FIELD_DEFAULT_CONSTRUCTOR(SubScreenTypeCode),
					FIELD_DEFAULT_CONSTRUCTOR(impex::DataSourceLinks),
					FIELD_VALUE_CONSTRUCTOR(AllowCanceled, false),
					FIELD_DEFAULT_CONSTRUCTOR(StopPointLocation),
					FIELD_VALUE_CONSTRUCTOR(MaxTransferDuration, 180)
			)	),
			_direction(DISPLAY_DEPARTURES),
			_originsOnly(WITH_PASSING),
			_generationMethod(STANDARD_METHOD)
		{}

		void DisplayScreen::addForbiddenPlace(const pt::StopArea* place)
		{
			_forbiddenArrivalPlaces.insert(make_pair(place->getKey(),place));
		}



		void DisplayScreen::addDisplayedPlace(const pt::StopArea* __PointArret)
		{
			_displayedPlaces.insert(make_pair(__PointArret->getKey(),__PointArret));
		}



		void DisplayScreen::addForcedDestination(const pt::StopArea* place)
		{
			_forcedDestinations.insert(make_pair(place->getKey(), place));
		}



		void DisplayScreen::setDirection( DeparturesTableDirection direction )
		{
			_direction = direction;
		}

		void DisplayScreen::setOriginsOnly( EndFilter value)
		{
			_originsOnly = value;
		}

		void DisplayScreen::setGenerationMethod( GenerationMethod method )
		{
			_generationMethod = method;
		}

		void DisplayScreen::setStops(const ArrivalDepartureTableGenerator::PhysicalStops& value)
		{
			_physicalStops = value;
			computeStrAndSetPhysicalStops();
		}



		ArrivalDepartureList DisplayScreen::generateStandardScreen(
			const boost::posix_time::ptime& startTime,
			const boost::posix_time::ptime& endTime,
			bool rootCall,
			bool scom
		) const	{
			boost::shared_ptr<ArrivalDepartureTableGenerator> generator;
			switch (_generationMethod)
			{
			case STANDARD_METHOD:
				{
					StandardArrivalDepartureTableGenerator* standardGenerator(
						new StandardArrivalDepartureTableGenerator(
							getPhysicalStops(),
							_direction,
							_originsOnly,
							_allowedLines,
							_displayedPlaces,
							_forbiddenArrivalPlaces,
							startTime,
							endTime,
							get<AllowCanceled>(),
							rootCall ? get<DisplayTypePtr>()->get<RowsNumber>() : 1
						)
					);

					// If SCOM is available and enabled, setup the generator to use it
					#ifdef WITH_SCOM
					if (scom)
					{
						standardGenerator->setClient(scom,this->getCodeBySources());
					}
					#endif

					generator.reset(static_cast<ArrivalDepartureTableGenerator*>( standardGenerator ));
					break;
				}

			case WITH_FORCED_DESTINATIONS_METHOD:
				generator.reset(
					static_cast<ArrivalDepartureTableGenerator*>(
						new ForcedDestinationsArrivalDepartureTableGenerator(
							getPhysicalStops(),
							_direction,
							_originsOnly,
							_allowedLines,
							_displayedPlaces,
							_forbiddenArrivalPlaces,
							startTime,
							endTime,
							rootCall ? get<DisplayTypePtr>()->get<RowsNumber>() : 1,
							_forcedDestinations,
							minutes(get<DestinationForceDelay>()),
							get<AllowCanceled>()
				)	)	);
				break;

			default:
				throw Exception("This method cannot apply to this screen type");
			}

			ArrivalDepartureList result(generator->generate());

			// Find continuations
			BOOST_FOREACH(ArrivalDepartureList::value_type& row, result)
			{
				for(ArrivalDepartureList::mapped_type::iterator itDest(row.second.begin()); itDest != row.second.end(); ++itDest)
				{
					// Avoid departure place
					if(itDest == row.second.begin())
					{
						continue;
					}

					Journey rootJourney;
					rootJourney.append(itDest->serviceUse);

					// Transfers at destinations
					if(rootCall)
					{
						itDest->transferDestinations = _generateTransferDestinations(
							rootJourney,
							*itDest->place,
							startTime,
							endTime
						);
					}

					const DisplayScreen* continuationScreen(_getContinuationTransferScreen(*itDest->place));
					if(continuationScreen)
					{
						ptime transferStartTime(
							itDest->serviceUse.getArrivalDateTime() - minutes(continuationScreen->get<ClearingDelay>())
						);
						ptime transferEndTime(
							itDest->serviceUse.getArrivalDateTime() + minutes(continuationScreen->get<MaxDelay>())
						);
						ArrivalDepartureList subResult(
							continuationScreen->generateStandardScreen(transferStartTime, transferEndTime, false)
						);
						if(!subResult.empty() && subResult.begin()->first.getService() != row.first.getService())
						{
							itDest->continuationService = subResult.begin()->first;
							itDest->destinationsReachedByContinuationService = subResult.begin()->second;

							if(rootCall)
							{
								for(ArrivalDepartureList::mapped_type::iterator item(itDest->destinationsReachedByContinuationService.begin()); item != itDest->destinationsReachedByContinuationService.end(); ++item)
								{
									if(item == itDest->destinationsReachedByContinuationService.begin())
									{
										continue;
									}

									Journey approachJourney(rootJourney);
									approachJourney.append(item->serviceUse);
									item->transferDestinations = _generateTransferDestinations(
										approachJourney,
										*item->place,
										startTime,
										endTime
									);
							}	}
						}
					}
			}	}

			return result;
		}



		IntermediateStop::TransferDestinations DisplayScreen::_generateTransferDestinations(
			const Journey& approachJourney,
			const StopArea& stopArea,
			const ptime& startTime,
			const ptime& endTime
		) const	{
			IntermediateStop::TransferDestinations result;

			TransferDestinationsList::const_iterator it(
				_transfers.find(&stopArea)
			);
			if(it != _transfers.end())
			{
				ptime routePlanningEndTime(approachJourney.getFirstDepartureTime());
				routePlanningEndTime += minutes(get<MaxTransferDuration>());
				AlgorithmLogger logger;
				BOOST_FOREACH(const TransferDestinationsList::mapped_type::value_type& it2, it->second)
				{
					PTTimeSlotRoutePlanner rp(
						get<BroadCastPoint>().get_ptr(),
						it2,
						approachJourney.getFirstDepartureTime(),
						approachJourney.getFirstDepartureTime(),
						approachJourney.getFirstDepartureTime(),
						routePlanningEndTime,
						1,
						AccessParameters(
							USER_PEDESTRIAN,
							false,
							false,
							0,
							posix_time::minutes(0),
							67,
							approachJourney.size()+1
						),
						DEPARTURE_FIRST,
						false,
						logger
					);

					const PTRoutePlannerResult solution(rp.run());

					if(solution.getJourneys().empty()) continue;

					const Journey& journey(solution.getJourneys().front());

					if(	journey.size() == approachJourney.size() + 1)
					{
						bool ok(true);
						for(size_t i(0); i<approachJourney.size(); ++i)
						{
							if(	journey.getJourneyLeg(i).getArrivalEdge()->getFromVertex()->getHub() != approachJourney.getJourneyLeg(i).getArrivalEdge()->getFromVertex()->getHub() ||
								journey.getJourneyLeg(i).getDepartureEdge()->getFromVertex()->getHub() != approachJourney.getJourneyLeg(i).getDepartureEdge()->getFromVertex()->getHub() ||
								journey.getJourneyLeg(i).getService() != approachJourney.getJourneyLeg(i).getService()
							){
								ok = false;
								break;
							}
						}
						if(ok)
						{
							result.insert(journey.getJourneyLeg(journey.size() - 1));
						}
					}
				}
			}

			return result;
		}



		void DisplayScreen::computeStrAndSetPhysicalStops()
		{
			string authorized_ps_str("");
			int rank(0);
			BOOST_FOREACH(const ArrivalDepartureTableGenerator::PhysicalStops::value_type& stop, _physicalStops)
			{
				authorized_ps_str += (rank > 0 ? ("," + lexical_cast<string>(stop.second->getKey())) : lexical_cast<string>(stop.second->getKey()));
				rank++;
			}
			set<PhysicalStops>(authorized_ps_str);
		}

		synthese::DeparturesTableDirection DisplayScreen::getDirection() const
		{
			return _direction;
		}

		synthese::EndFilter DisplayScreen::getEndFilter() const
		{
			return _originsOnly;
		}

		const ArrivalDepartureTableGenerator::PhysicalStops& DisplayScreen::getPhysicalStops(bool result) const
		{
			return
				(get<AllPhysicalDisplayed>() && get<BroadCastPoint>().get_ptr() && result) ?
				get<BroadCastPoint>()->getPhysicalStops() :
				_physicalStops
			;
		}

		const ForbiddenPlacesList& DisplayScreen::getForbiddenPlaces() const
		{
			return _forbiddenArrivalPlaces;
		}

		const DisplayedPlacesList& DisplayScreen::getDisplayedPlaces() const
		{
			return _displayedPlaces;
		}

		DisplayScreen::GenerationMethod DisplayScreen::getGenerationMethod() const
		{
			return _generationMethod;
		}

		const DisplayedPlacesList& DisplayScreen::getForcedDestinations() const
		{
			return _forcedDestinations;
		}



		std::string DisplayScreen::getFullName() const
		{
			if(getLocation())
			{
				stringstream s;
				s << getLocation()->getFullName();
				if (!get<BroadCastPointComment>().empty())
					s << "/" << get<BroadCastPointComment>();
				return s.str();
			}
			else
			{
				return get<BroadCastPointComment>() + " (not located)";
			}
		}



		DisplayScreen::Labels DisplayScreen::getSortedAvailableDestinationsLabels(
			const DisplayedPlacesList& placesToAvoid
		) const {
			std::set<const DisplayScreen*> screens;
			screens.insert(this);
			BOOST_FOREACH(const DisplayScreen::ChildrenType::value_type& it, getChildren())
			{
				if(it.second->getSubScreenType() == DisplayScreen::CONTINUATION_TRANSFER)
				{
					screens.insert(it.second);
				}
			}

			map<std::string, std::pair<RegistryKeyType, string> > m;
			for(size_t i(0); i < screens.size(); ++i)
			{
				BOOST_FOREACH(const ArrivalDepartureTableGenerator::PhysicalStops::value_type& it, getPhysicalStops())
				{
					const StopPoint* p(it.second);
					const Vertex::Edges& edges = p->getDepartureEdges();
					BOOST_FOREACH(const Vertex::Edges::value_type& e, edges)
					{
						for(const Edge* edge= e.second->getFollowingArrivalForFineSteppingOnly();
							edge != NULL;
							edge = edge->getFollowingArrivalForFineSteppingOnly()
						){
							m.insert(
								make_pair(
									dynamic_cast<const NamedPlace*>(edge->getHub())->getFullName(),
									make_pair(
										dynamic_cast<const pt::StopArea*>(edge->getHub())->getKey(),
										dynamic_cast<const NamedPlace*>(edge->getHub())->getFullName()
							)	)	);
			}	}	}	}
			Labels v;
			for (map<string, pair<RegistryKeyType, string> >::const_iterator it2 = m.begin(); it2 != m.end(); ++it2)
			{
				v.push_back(it2->second);
			}
			return v;
		}



		void DisplayScreen::removeForcedDestination(const StopArea* place)
		{
			DisplayedPlacesList::iterator it = _forcedDestinations.find(place->getKey());
			if (it != _forcedDestinations.end())
				_forcedDestinations.erase(it);
		}

		void DisplayScreen::clearForbiddenPlaces()
		{
			_forbiddenArrivalPlaces.clear();
		}

		void DisplayScreen::clearDisplayedPlaces()
		{
			_displayedPlaces.clear();
		}

		void DisplayScreen::clearForcedDestinations()
		{
			_forcedDestinations.clear();
		}



		void DisplayScreen::removeDisplayedPlace(const StopArea* place)
		{
			DisplayedPlacesList::iterator it = _displayedPlaces.find(place->getKey());
			if (it != _displayedPlaces.end())
				_displayedPlaces.erase(it);
		}

		void DisplayScreen::removeForbiddenPlace(const StopArea* place)
		{
			DisplayedPlacesList::iterator it = _forbiddenArrivalPlaces.find(place->getKey());
			if (it != _forbiddenArrivalPlaces.end())
				_forbiddenArrivalPlaces.erase(it);
		}

		void DisplayScreen::copy(const DisplayScreen& other )
		{
			set<AllPhysicalDisplayed>(other.get<AllPhysicalDisplayed>());
			set<BlinkingDelay>(other.get<BlinkingDelay>());
			set<ClearingDelay>(other.get<ClearingDelay>());
			set<DestinationForceDelay>(other.get<DestinationForceDelay>());
			setDirection(other.getDirection());
			set<FirstRow>(other.get<FirstRow>());
			setGenerationMethod(other.getGenerationMethod());
			set<BroadCastPoint>(other.get<BroadCastPoint>());
			setSameRoot(other);
			set<MaxDelay>(other.get<MaxDelay>());
			setOriginsOnly(other.getEndFilter());
			set<ServiceNumberDisplay>(other.get<ServiceNumberDisplay>());
			set<Title>(other.get<Title>());
			set<TrackNumberDisplay>(other.get<TrackNumberDisplay>());
			set<DisplayTypePtr>(other.get<DisplayTypePtr>());
			set<WiringCode>(other.get<WiringCode>());
			setStops(other.getPhysicalStops(false));
			setAllowedLines(other.getAllowedLines());
			for (DisplayedPlacesList::const_iterator it = other.getDisplayedPlaces().begin(); it != other.getDisplayedPlaces().end(); ++it)
			{
				addDisplayedPlace(it->second);
			}
			for (DisplayedPlacesList::const_iterator it2 = other.getForcedDestinations().begin(); it2 != other.getForcedDestinations().end(); ++it2)
			{
				addForcedDestination(it2->second);
			}
			for (ForbiddenPlacesList::const_iterator it3 = other.getForbiddenPlaces().begin(); it3 != other.getForbiddenPlaces().end(); ++it3)
			{
				addForbiddenPlace(it3->second);
			}
			if (hasGeometry())
			{
				setGeometry(other.getGeometry());
			}
		}



		DisplayScreen::~DisplayScreen(
		){}


		bool DisplayScreen::isDown(
			const DisplayMonitoringStatus& status
		) const {
			if(	!isMonitored()
			){
				return false;
			}

			ptime now(second_clock::local_time());
			if(now - status.getTime() <= get<DisplayTypePtr>()->get<TimeBetweenChecks>())
			{
				return false;
			}
			return true;
		}


		bool DisplayScreen::isMonitored(
		) const {
			return get<MaintenanceIsOnline>() &&
				get<DisplayTypePtr>() &&
				get<DisplayTypePtr>().get_ptr() != NULL &&
				get<DisplayTypePtr>()->get<MonitoringParserPage>() &&
				get<DisplayTypePtr>()->get<TimeBetweenChecks>().minutes() > 0
			;
		}



		void DisplayScreen::addTransferDestination(
			TransferDestinationsList::key_type transferPlace,
			TransferDestinationsList::mapped_type::value_type destinationPlace
		){
			TransferDestinationsList::iterator it(_transfers.find(transferPlace));
			if(it == _transfers.end())
			{
				it = _transfers.insert(make_pair(transferPlace, TransferDestinationsList::mapped_type())).first;
			}
			it->second.insert(destinationPlace);
		}



		void DisplayScreen::removeTransferDestination( TransferDestinationsList::key_type transferPlace, TransferDestinationsList::mapped_type::value_type destinationPlace )
		{
			TransferDestinationsList::iterator it(_transfers.find(transferPlace));
			if(it == _transfers.end()) return;
			TransferDestinationsList::mapped_type::iterator it2(it->second.find(destinationPlace));
			if(it2 == it->second.end()) return;
			it->second.erase(it2);
		}



		void DisplayScreen::clearTransferDestinations()
		{
			_transfers.clear();
		}



		const geography::NamedPlace* DisplayScreen::getLocation() const
		{
			if(getRoot<PlaceWithDisplayBoards>())
			{
				return getRoot<PlaceWithDisplayBoards>()->getPlace();
			}
			if(getRoot<DisplayScreenCPU>())
			{
				return getRoot<DisplayScreenCPU>()->getPlace();
			}
			return NULL;
		}



		DisplayScreen* DisplayScreen::_getContinuationTransferScreen( const pt::StopArea& stop ) const
		{
			BOOST_FOREACH(const DisplayScreen::ChildrenType::value_type& it, getChildren())
			{
				if(	it.second->getSubScreenType() == CONTINUATION_TRANSFER &&
					it.second->get<BroadCastPoint>().get_ptr() == &stop
				){
					return it.second;
				}
			}
			return NULL;
		}



		const std::string DisplayScreen::GetSubScreenTypeLabel( SubScreenType value )
		{
			switch(value)
			{
			case SUB_CONTENT: return "Elément de contenu";
			case CONTINUATION_TRANSFER: return "Correspondance continue";
			}
			return string();
		}



		std::set<messages::MessageType*> DisplayScreen::getMessageTypes() const
		{
			std::set<messages::MessageType*> result;
			if(get<DisplayTypePtr>())
			{
				result.insert(get<DisplayTypePtr>()->get<MessageType>().get_ptr());
			}
			return result;
		}



		void DisplayScreen::getBroadcastPoints(
			BroadcastPoint::BroadcastPoints& result
		) const	{
			BOOST_FOREACH(const DisplayScreen::Registry::value_type& it, Env::GetOfficialEnv().getRegistry<DisplayScreen>())
			{
				result.push_back(it.second.get());
			}
		}



		bool DisplayScreen::displaysMessage(
			const Alarm::LinkedObjects& linkedObjects,
			const util::ParametersMap& parameters
		) const	{

			// If no customized rule, use the Jump over undefined type or rule page
			if(	!get<DisplayTypePtr>() ||
				!get<DisplayTypePtr>()->get<IsDisplayedMessagePage>()
			){
				// in broad cast points recipients
				Alarm::LinkedObjects::const_iterator it(
					linkedObjects.find(
						BroadcastPointAlarmRecipient::FACTORY_KEY
				)	);

				// No broadcast recipient = no display
				if(it == linkedObjects.end())
				{
					return false;
				}

				// Loop on each recipient
				bool result(false);
				BOOST_FOREACH(
					const Alarm::LinkedObjects::mapped_type::value_type& link,
					it->second
				){
					// Search for precisely specified broadcast
					if(	link->getObjectId() == getKey())
					{
						return true;
					}

					// Search for general broadcast on display screens
					if(	link->getObjectId() == DisplayScreenTableSync::TABLE.ID ||
						link->getObjectId() == 0
					){
						result = true;
					}

					// Search for general broadcast on a precise display screens type equal to
					// the display type of the current display screen
					if( get<DisplayTypePtr>() &&
						link->getObjectId() == get<DisplayTypePtr>()->getKey())
					{
						result = true;
					}

					// Search for general broadcast on all display screen types
					if( link->getObjectId() == DisplayTypeTableSync::TABLE.ID)
					{
						result = true;
					}

					if(result)
					{
						break;
					}
				}
				if(!result)
				{
					return false;
				}

				if(!getLocation())
				{
					return false;
				}

				// The display is allowed on this screen, search for lines and stops


				// Search for lines
				it = linkedObjects.find(
					LineAlarmRecipient::FACTORY_KEY
				);

				// No lines recipients = all lines OK
				if(it != linkedObjects.end())
				{
					bool result(false);

					// Loop on links
					BOOST_FOREACH(
						const Alarm::LinkedObjects::mapped_type::value_type& link,
						it->second
					){
						// 0 link = all lines
						if(link->getObjectId() == 0)
						{
							result = true;
						}
						else if(decodeTableId(link->getObjectId()) == TransportNetworkTableSync::TABLE.ID)
						{ 	// network id = check all lines of the network
							try
							{
								const TransportNetwork& network(
									*Env::GetOfficialEnv().get<TransportNetwork>(
										link->getObjectId()
								)	);
								result = _locationOnNetwork(network);
							}
							catch(ObjectNotFoundException<TransportNetwork>&)
							{
							}
						}
						else if(decodeTableId(link->getObjectId()) == TreeFolderTableSync::TABLE.ID)
						{
							// TODO
						}
						else if(decodeTableId(link->getObjectId()) == CommercialLineTableSync::TABLE.ID)
						{	// line id = check the line

							// Checking if direction is specified
							optional<bool> direction;
							if(!link->getParameter().empty())
							{
								direction = (link->getParameter() == "1");
							}

							try
							{
								const CommercialLine& line(
									*Env::GetOfficialEnv().get<CommercialLine>(
										link->getObjectId()
								)	);
								result = _locationOnLine(
									line,
									direction
								);
							}
							catch(ObjectNotFoundException<CommercialLine>&)
							{
							}
						}

						if(result)
						{
							break;
						}
					}

					// No line allows the display : display is forbidden
					if(!result)
					{
						return false;
					}
				}

				// Search for stops
				it = linkedObjects.find(
					StopAreaAlarmRecipient::FACTORY_KEY
				);

				// No stops recipients = all stops OK
				if(it != linkedObjects.end())
				{
					bool result(false);

					// Loop on links
					BOOST_FOREACH(
						const Alarm::LinkedObjects::mapped_type::value_type& link,
						it->second
					){
						// 0 link = all lines
						if(link->getObjectId() == 0)
						{
							result = true;
						}
						else if(decodeTableId(link->getObjectId()) == CityTableSync::TABLE.ID)
						{
							result = (getLocation()->getCity()->getKey() == link->getObjectId());
						}
						else if(decodeTableId(link->getObjectId()) == StopAreaTableSync::TABLE.ID)
						{
							result = (getLocation()->getKey() == link->getObjectId());
						}
						else if(decodeTableId(link->getObjectId()) == StopPointTableSync::TABLE.ID)
						{
							result = (get<StopPointLocation>() && get<StopPointLocation>()->getKey() == link->getObjectId());
						}

						if(result)
						{
							break;
						}
					}

					// No stop allows the display : display is forbidden
					if(!result)
					{
						return false;
					}
				}

				// Nothing has blocked the display
				return true;
			}

			stringstream s;
			ParametersMap pm;

			boost::shared_ptr<ParametersMap> screenPM(new ParametersMap);
			toParametersMap(*screenPM, true);
			pm.insert("screen", screenPM);
			pm.insert(VAR_BROADCAST_POINT_TYPE, VALUE_DISPLAY_SCREEN);
			boost::shared_ptr<ParametersMap> recipientsPM(new ParametersMap);
			Alarm::LinkedObjectsToParametersMap(linkedObjects, *recipientsPM);
			pm.insert("recipients", recipientsPM);

			get<DisplayTypePtr>()->get<IsDisplayedMessagePage>()->display(s, pm);
			string str(s.str());
			trim(str);
			return !str.empty();
		}



		bool DisplayScreen::_locationOnLine(
			const pt::CommercialLine& line,
			boost::optional<bool> direction
		) const	{

			if(get<StopPointLocation>())
			{
				// Search on departure edges
				BOOST_FOREACH(const Vertex::Edges::value_type& edge, get<StopPointLocation>()->getDepartureEdges())
				{
					if(!dynamic_cast<const LinePhysicalStop*>(edge.second))
					{
						continue;
					}
					JourneyPattern* path(static_cast<const LinePhysicalStop*>(edge.second)->getJourneyPattern());
					if(	path->getCommercialLine() == &line &&
						(	!direction ||
							path->getWayBack() == *direction
					)	){
						return true;
					}
				}

				// Search on arrival edges
				BOOST_FOREACH(const Vertex::Edges::value_type& edge, get<StopPointLocation>()->getArrivalEdges())
				{
					if(!dynamic_cast<const LinePhysicalStop*>(edge.second))
					{
						continue;
					}
					JourneyPattern* path(static_cast<const LinePhysicalStop*>(edge.second)->getJourneyPattern());
					if(	path->getCommercialLine() == &line &&
						(	!direction ||
							path->getWayBack() == *direction
					)	){
						return true;
					}
				}

				// Nothing was found
				return false;
			}

			// Check if the location is a stop area
			const StopArea* stopArea(
				dynamic_cast<const StopArea*>(getLocation())
			);
			if(!stopArea)
			{
				return false;
			}

			// Loop on each stop
			BOOST_FOREACH(
				const StopArea::PhysicalStops::value_type& it,
				stopArea->getPhysicalStops()
			){
				const StopPoint& stopPoint(*it.second);

				// Search on departure edges
				BOOST_FOREACH(const Vertex::Edges::value_type& edge, stopPoint.getDepartureEdges())
				{
					if(!dynamic_cast<const LinePhysicalStop*>(edge.second))
					{
						continue;
					}
					JourneyPattern* path(static_cast<const LinePhysicalStop*>(edge.second)->getJourneyPattern());
					if(	path->getCommercialLine() == &line)
					{
						return true;
					}
				}

				// Search on arrival edges
				BOOST_FOREACH(const Vertex::Edges::value_type& edge, stopPoint.getArrivalEdges())
				{
					if(!dynamic_cast<const LinePhysicalStop*>(edge.second))
					{
						continue;
					}
					JourneyPattern* path(static_cast<const LinePhysicalStop*>(edge.second)->getJourneyPattern());
					if(	path->getCommercialLine() == &line)
					{
						return true;
					}
				}
			}

			// Nothing was found
			return false;
		}



		bool DisplayScreen::_locationOnNetwork(
			const pt::TransportNetwork& network
		) const	{

			if(get<StopPointLocation>())
			{
				// Search on departure edges
				BOOST_FOREACH(const Vertex::Edges::value_type& edge, get<StopPointLocation>()->getDepartureEdges())
				{
					if(!dynamic_cast<const LinePhysicalStop*>(edge.second))
					{
						continue;
					}
					JourneyPattern* path(static_cast<const LinePhysicalStop*>(edge.second)->getJourneyPattern());
					if(	path->getCommercialLine()->getNetwork() == &network)
					{
						return true;
					}
				}

				// Search on arrival edges
				BOOST_FOREACH(const Vertex::Edges::value_type& edge, get<StopPointLocation>()->getArrivalEdges())
				{
					if(!dynamic_cast<const LinePhysicalStop*>(edge.second))
					{
						continue;
					}
					JourneyPattern* path(static_cast<const LinePhysicalStop*>(edge.second)->getJourneyPattern());
					if(	path->getCommercialLine()->getNetwork() == &network)
					{
						return true;
					}
				}

				// Nothing was found
				return false;
			}

			// Check if the location is a stop area
			const StopArea* stopArea(
				dynamic_cast<const StopArea*>(getLocation())
			);
			if(!stopArea)
			{
				return false;
			}

			// Loop on each stop
			BOOST_FOREACH(
				const StopArea::PhysicalStops::value_type& it,
				stopArea->getPhysicalStops()
			){
				const StopPoint& stopPoint(*it.second);

				// Search on departure edges
				BOOST_FOREACH(const Vertex::Edges::value_type& edge, stopPoint.getDepartureEdges())
				{
					if(!dynamic_cast<const LinePhysicalStop*>(edge.second))
					{
						continue;
					}
					JourneyPattern* path(static_cast<const LinePhysicalStop*>(edge.second)->getJourneyPattern());
					if(	path->getCommercialLine()->getNetwork() == &network)
					{
						return true;
					}
				}

				// Search on arrival edges
				BOOST_FOREACH(const Vertex::Edges::value_type& edge, stopPoint.getArrivalEdges())
				{
					if(!dynamic_cast<const LinePhysicalStop*>(edge.second))
					{
						continue;
					}
					JourneyPattern* path(static_cast<const LinePhysicalStop*>(edge.second)->getJourneyPattern());
					if(	path->getCommercialLine()->getNetwork() == &network)
					{
						return true;
					}
				}
			}

			// Nothing was found
			return false;
		}
		
		void DisplayScreen::link( util::Env& env, bool withAlgorithmOptimizations /*= false*/ )
		{
			setDirection(static_cast<DeparturesTableDirection>(get<departure_boards::Direction>()));
			setOriginsOnly(static_cast<EndFilter>(get<OriginsOnly>()));
			setGenerationMethod(static_cast<GenerationMethod>(get<GenerationMethodCode>()));
			setSubScreenType(static_cast<SubScreenType>(get<SubScreenTypeCode>()));
			setNullRoot();
			clearForbiddenPlaces();
			clearDisplayedPlaces();
			clearForcedDestinations();
			clearTransferDestinations();
			
			// Line filter
			LineFilter lineFilter(
				DisplayScreenTableSync::UnserializeLineFilter(
					get<AllowedLines>(),
					env,
					util::UP_LINKS_LOAD_LEVEL
			)	);
			setAllowedLines(lineFilter);
			
			// root
			// CPU
			if (get<BroadCastPoint>())
			{
				setRoot(
					DeparturesTableModule::GetPlaceWithDisplayBoards(
						get<BroadCastPoint>().get_ptr(),
						env
				)	);
			}
			setParent(NULL);
			
			if (get<Up>()) {
				setParent(&*get<Up>());
			}
			registerInParentOrRoot();
			
			// Physical stops
			vector<string> stops = Conversion::ToStringVector(get<PhysicalStops>());
			ArrivalDepartureTableGenerator::PhysicalStops pstops;
			BOOST_FOREACH(const string& stop, stops)
			{
				try
				{
					RegistryKeyType id(lexical_cast<RegistryKeyType>(stop));
					pstops.insert(
						make_pair(
							id,
							StopPointTableSync::Get(id, env).get()
					)	);
				}
				catch (ObjectNotFoundException<StopPoint>&)
				{
					Log::GetInstance().warn("Data corrupted in " + DisplayScreenTableSync::TABLE.NAME + "/" + PhysicalStops::FIELD.name);
				}
			}
			setStops(pstops);
			
			// Forbidden places
			stops = Conversion::ToStringVector (get<ForbiddenArrivalPlaces>());
			BOOST_FOREACH(const string& stop, stops)
			{
				try
				{
					addForbiddenPlace(StopAreaTableSync::Get(lexical_cast<RegistryKeyType>(stop), env).get());
				}
				catch (ObjectNotFoundException<StopArea>& e)
				{
					Log::GetInstance().warn("Data corrupted in " + DisplayScreenTableSync::TABLE.NAME + "/" + ForbiddenArrivalPlaces::FIELD.name, e);
				}
			}
			
			// Displayed places
			stops = Conversion::ToStringVector (get<DisplayedPlaces>());
			BOOST_FOREACH(const string& stop, stops)
			{
				try
				{
					addDisplayedPlace(StopAreaTableSync::Get(lexical_cast<RegistryKeyType>(stop), env).get());
				}
				catch (ObjectNotFoundException<StopArea>& e)
				{
					Log::GetInstance().warn("Data corrupted in " + DisplayScreenTableSync::TABLE.NAME + "/" + DisplayedPlaces::FIELD.name, e);
				}
			}
			
			// Forced destinations
			stops = Conversion::ToStringVector (get<ForcedDestinations>());
			BOOST_FOREACH(const string& stop, stops)
			{
				try
				{
					addForcedDestination(StopAreaTableSync::Get(lexical_cast<RegistryKeyType>(stop), env).get());
				}
				catch (ObjectNotFoundException<StopArea>& e)
				{
					Log::GetInstance().warn("Data corrupted in " + DisplayScreenTableSync::TABLE.NAME + "/" + ForcedDestinations::FIELD.name, e);
			}	}
			
			// Transfers
			stops = Conversion::ToStringVector(get<TransferDestinations>());
			BOOST_FOREACH(const string& stop, stops)
			{
				typedef tokenizer<char_separator<char> > tokenizer;
				tokenizer tokens (stop, char_separator<char>(":"));
				tokenizer::iterator it(tokens.begin());
				string id1(*it);
				++it;
				string id2(*it);
				
				try
				{
					addTransferDestination(
						StopAreaTableSync::Get(lexical_cast<RegistryKeyType>(id1), env).get(),
						StopAreaTableSync::Get(lexical_cast<RegistryKeyType>(id2), env).get()
					);
				}
				catch (ObjectNotFoundException<StopArea>& e)
				{
					Log::GetInstance().warn("Data corrupted in " + DisplayScreenTableSync::TABLE.NAME + "/" + TransferDestinations::FIELD.name, e);
				}
			}
		}
		
		void DisplayScreen::unlink()
		{
			setParent(NULL);
		}
		
		void DisplayScreen::addAdditionalParameters(
			util::ParametersMap& pm,
			std::string prefix
		) const {
			pm.insert(prefix + DATA_IS_ONLINE, get<MaintenanceIsOnline>());
			if(getRoot<PlaceWithDisplayBoards>() && getRoot<PlaceWithDisplayBoards>()->getPlace())
			{
				pm.insert(prefix + DATA_LOCATION_ID, getRoot<PlaceWithDisplayBoards>()->getPlace()->getKey());
			}
			if(getRoot<DisplayScreenCPU>())
			{
				pm.insert(prefix + DATA_CPU_ID, getRoot<DisplayScreenCPU>()->getKey());
			}
			pm.insert(prefix + DATA_MAC_ADDRESS, get<MacAddress>());
			pm.insert(prefix + DATA_MAINTENANCE_MESSAGE, get<MaintenanceMessage>());
			pm.insert(prefix + DATA_NAME, get<BroadCastPointComment>());
			pm.insert(prefix + DATA_SCREEN_ID, getKey());
			pm.insert(prefix + DATA_TITLE, get<Title>());
			pm.insert(prefix + DATA_TYPE_ID, get<DisplayTypePtr>().get_ptr() ? get<DisplayTypePtr>()->getKey() : 0);
		}
}	}
