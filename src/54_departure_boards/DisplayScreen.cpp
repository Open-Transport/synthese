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
#include "DeparturesTableInterfacePage.h"
#include "DisplayMaintenanceLog.h"
#include "DisplayMonitoringStatus.h"
#include "DisplayScreenContentFunction.h"
#include "DisplayScreenCPU.h"
#include "DisplayScreenTableSync.h"
#include "DisplayType.h"
#include "Interface.h"
#include "InterfacePageException.h"
#include "JourneyPattern.hpp"
#include "LineAlarmRecipient.hpp"
#include "LineStop.h"
#include "PTRoutePlannerResult.h"
#include "PTTimeSlotRoutePlanner.h"
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
	using namespace dblog;
	using namespace geography;
	using namespace graph;
	using namespace interfaces;
	using namespace messages;
	using namespace pt;
	using namespace pt_journey_planner;
	using namespace road;
	using namespace tree;
	using namespace util;

	namespace util
	{
		template<> const string Registry<departure_boards::DisplayScreen>::KEY("DisplayScreen");
		template<> const string util::FactorableTemplate<messages::BroadcastPoint, departure_boards::DisplayScreen>::FACTORY_KEY = "DisplayScreen";
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



		DisplayScreen::DisplayScreen(
			RegistryKeyType key
		):	Registrable(key),
			_displayType(NULL),
			_wiringCode(0),
			_comPort(0),
			_stopPointLocation(NULL),
			_blinkingDelay(1),
			_trackNumberDisplay(false),
			_serviceNumberDisplay(false),
			_displayTeam(false),
			_displayClock(true),
			_displayedPlace(NULL),
			_allPhysicalStopsDisplayed(true),
			_direction(DISPLAY_DEPARTURES),
			_originsOnly(WITH_PASSING),
			_maxDelay(12 * 60),			// default = 24 hours
			_clearingDelay(0),
			_firstRow(0),
			_routePlanningWithTransfer(false),
			_allowCanceled(false),
			_generationMethod(STANDARD_METHOD),
			_destinationForceDelay(120),	// default = 2 hours
			_maintenanceIsOnline(true)
		{}



		void DisplayScreen::setDestinationForceDelay(int delay)
		{
			_destinationForceDelay = delay;
		}

		void DisplayScreen::setMaxDelay(int maxDelay)
		{
			_maxDelay = maxDelay;
		}

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



		void DisplayScreen::setType(const DisplayType* displayType)
		{
			_displayType = displayType;
		}

		void DisplayScreen::setWiringCode( int code)
		{
			_wiringCode = code;
		}

		void DisplayScreen::setTitle( const std::string& title)
		{
			_title = title;
		}

		const std::string& DisplayScreen::getTitle() const
		{
			return _title;
		}



		void DisplayScreen::setTrackNumberDisplay( bool value )
		{
			_trackNumberDisplay = value;
		}

		void DisplayScreen::setServiceNumberDisplay( bool value )
		{
			_serviceNumberDisplay = value;
		}



		void DisplayScreen::setDirection( DeparturesTableDirection direction )
		{
			_direction = direction;
		}

		void DisplayScreen::setOriginsOnly( EndFilter value)
		{
			_originsOnly = value;
		}

		void DisplayScreen::setClearingDelay( int delay )
		{
			_clearingDelay = delay;
		}

		void DisplayScreen::setFirstRow( int row )
		{
			_firstRow = row;
		}

		void DisplayScreen::setGenerationMethod( GenerationMethod method )
		{
			_generationMethod = method;
		}


		void DisplayScreen::setMaintenanceIsOnline( bool value )
		{
			_maintenanceIsOnline = value;
		}

		void DisplayScreen::setMaintenanceMessage( const std::string& message )
		{
			_maintenanceMessage = message;
		}



		ArrivalDepartureList DisplayScreen::generateStandardScreen(
			const boost::posix_time::ptime& startTime,
			const boost::posix_time::ptime& endTime,
			bool rootCall
		) const	{
			boost::shared_ptr<ArrivalDepartureTableGenerator> generator;
			switch (_generationMethod)
			{
			case STANDARD_METHOD:
				generator.reset(
					static_cast<ArrivalDepartureTableGenerator*>(
						new StandardArrivalDepartureTableGenerator(
							getPhysicalStops(),
							_direction,
							_originsOnly,
							_allowedLines,
							_displayedPlaces,
							_forbiddenArrivalPlaces,
							startTime,
							endTime,
							_allowCanceled,
							rootCall ? _displayType->getRowNumber() : 1
				)	)	);
				break;

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
							rootCall ? _displayType->getRowNumber() : 1,
							_forcedDestinations,
							minutes(_destinationForceDelay),
							_allowCanceled
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
							itDest->serviceUse.getArrivalDateTime() - minutes(continuationScreen->getClearingDelay())
						);
						ptime transferEndTime(
							itDest->serviceUse.getArrivalDateTime() + minutes(continuationScreen->getMaxDelay())
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
				routePlanningEndTime += days(1);
				AlgorithmLogger logger;
				BOOST_FOREACH(const TransferDestinationsList::mapped_type::value_type& it2, it->second)
				{
					PTTimeSlotRoutePlanner rp(
						_displayedPlace,
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



		int DisplayScreen::getWiringCode() const
		{
			return _wiringCode;
		}

		int DisplayScreen::getClearingDelay() const
		{
			return _clearingDelay;
		}

		bool DisplayScreen::getServiceNumberDisplay() const
		{
			return _serviceNumberDisplay;
		}

		bool DisplayScreen::getTrackNumberDisplay() const
		{
			return _trackNumberDisplay;
		}

		synthese::DeparturesTableDirection DisplayScreen::getDirection() const
		{
			return _direction;
		}

		synthese::EndFilter DisplayScreen::getEndFilter() const
		{
			return _originsOnly;
		}

		int DisplayScreen::getMaxDelay() const
		{
			return _maxDelay;
		}

		int DisplayScreen::getBlinkingDelay() const
		{
			return _blinkingDelay;
		}

		const ArrivalDepartureTableGenerator::PhysicalStops& DisplayScreen::getPhysicalStops(bool result) const
		{
			return
				(_allPhysicalStopsDisplayed && _displayedPlace && result) ?
				_displayedPlace->getPhysicalStops() :
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

		int DisplayScreen::getFirstRow() const
		{
			return _firstRow;
		}

		DisplayScreen::GenerationMethod DisplayScreen::getGenerationMethod() const
		{
			return _generationMethod;
		}

		const DisplayedPlacesList& DisplayScreen::getForcedDestinations() const
		{
			return _forcedDestinations;
		}

		int DisplayScreen::getForceDestinationDelay() const
		{
			return _destinationForceDelay;
		}



		bool DisplayScreen::getIsOnline() const
		{
			return _maintenanceIsOnline;
		}

		const std::string& DisplayScreen::getMaintenanceMessage() const
		{
			return _maintenanceMessage;
		}



		bool DisplayScreen::getAllPhysicalStopsDisplayed() const
		{
			return _allPhysicalStopsDisplayed;
		}



		std::string DisplayScreen::getFullName() const
		{
			if(getLocation())
			{
				stringstream s;
				s << getLocation()->getFullName();
				if (!getName().empty())
					s << "/" << getName();
				return s.str();
			}
			else
			{
				return getName() + " (not located)";
			}
		}



		DisplayScreen::Labels DisplayScreen::getSortedAvailableDestinationsLabels(
			const DisplayedPlacesList& placesToAvoid
		) const {
			set<const DisplayScreen*> screens;
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
			setAllPhysicalStopsDisplayed(other.getAllPhysicalStopsDisplayed());
			setBlinkingDelay(other.getBlinkingDelay());
			setClearingDelay(other.getClearingDelay());
			setDestinationForceDelay(other.getForceDestinationDelay());
			setDirection(other.getDirection());
			setFirstRow(other.getFirstRow());
			setGenerationMethod(other.getGenerationMethod());
			setDisplayedPlace(other.getDisplayedPlace());
			setSameRoot(other);
			setMaxDelay(other.getMaxDelay());
			setOriginsOnly(other.getEndFilter());
			setServiceNumberDisplay(other.getServiceNumberDisplay());
			setTitle(other.getTitle());
			setTrackNumberDisplay(other.getTrackNumberDisplay());
			setType(other.getType());
			setWiringCode(other.getWiringCode());
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
		}


		void DisplayScreen::setDisplayTeam( bool value )
		{
			_displayTeam = value;
		}

		bool DisplayScreen::getDisplayTeam() const
		{
			return _displayTeam;
		}



		DisplayScreen::~DisplayScreen(
		){
			_displayType = NULL;
		}



		void DisplayScreen::setDisplayClock(
			bool value
		){
			_displayClock = value;
		}



		void DisplayScreen::setComPort(
			int value
		) {
			_comPort = value;
		}



		int DisplayScreen::getComPort(
		) const {
			return _comPort;
		}



		bool DisplayScreen::getDisplayClock(
		) const {
			return _displayClock;
		}


		bool DisplayScreen::isDown(
			const DisplayMonitoringStatus& status
		) const {
			if(	!isMonitored()
			){
				return false;
			}

			ptime now(second_clock::local_time());
			if(now - status.getTime() <= getType()->getTimeBetweenChecks())
			{
				return false;
			}
			return true;
		}


		bool DisplayScreen::isMonitored(
		) const {
			return getIsOnline() &&
				getType() != NULL &&
				getType()->getMonitoringInterface() != NULL &&
				getType()->getTimeBetweenChecks().minutes() > 0
			;
		}

		void DisplayScreen::setMacAddress( const std::string& value )
		{
			_macAddress = value;
		}

		std::string DisplayScreen::getMacAddress() const
		{
			return _macAddress;
		}

		void DisplayScreen::setRoutePlanningWithTransfer( bool value )
		{
			_routePlanningWithTransfer = value;
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
					it.second->getDisplayedPlace() == &stop
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



		void DisplayScreen::toParametersMap(
			util::ParametersMap& pm,
			bool withAdditionalParameters,
			boost::logic::tribool withFiles,
			std::string prefix
		) const {

			pm.insert(prefix + DATA_IS_ONLINE, _maintenanceIsOnline);
			if(getRoot<PlaceWithDisplayBoards>() && getRoot<PlaceWithDisplayBoards>()->getPlace())
			{
				pm.insert(prefix + DATA_LOCATION_ID, getRoot<PlaceWithDisplayBoards>()->getPlace()->getKey());
			}
			if(getRoot<DisplayScreenCPU>())
			{
				pm.insert(prefix + DATA_CPU_ID, getRoot<DisplayScreenCPU>()->getKey());
			}
			pm.insert(prefix + DATA_MAC_ADDRESS, _macAddress);
			pm.insert(prefix + DATA_MAINTENANCE_MESSAGE, _maintenanceMessage);
			pm.insert(prefix + DATA_NAME, getName());
			pm.insert(prefix + DATA_SCREEN_ID, getKey());
			pm.insert(prefix + DATA_TITLE, _title);
			pm.insert(prefix + DATA_TYPE_ID, _displayType ? _displayType->getKey() : 0);
		}



		MessageType* DisplayScreen::getMessageType() const
		{
			return
				_displayType ?
				_displayType->getMessageType() :
				NULL;
		}



		void DisplayScreen::getBrodcastPoints(
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
			if(	!_displayType ||
				!_displayType->getMessageIsDisplayedPage()
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
							result = (_stopPointLocation && _stopPointLocation->getKey() == link->getObjectId());
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

			_displayType->getMessageIsDisplayedPage()->display(s, pm);
			string str(s.str());
			trim(str);
			return !str.empty();
		}



		bool DisplayScreen::_locationOnLine(
			const pt::CommercialLine& line,
			boost::optional<bool> direction
		) const	{

			if(_stopPointLocation)
			{
				// Search on departure edges
				BOOST_FOREACH(const Vertex::Edges::value_type& edge, _stopPointLocation->getDepartureEdges())
				{
					if(!dynamic_cast<const LineStop*>(edge.second))
					{
						continue;
					}
					JourneyPattern* path(static_cast<const LineStop*>(edge.second)->getLine());
					if(	path->getCommercialLine() == &line &&
						(	!direction ||
							path->getWayBack() == *direction
					)	){
						return true;
					}
				}

				// Search on arrival edges
				BOOST_FOREACH(const Vertex::Edges::value_type& edge, _stopPointLocation->getArrivalEdges())
				{
					if(!dynamic_cast<const LineStop*>(edge.second))
					{
						continue;
					}
					JourneyPattern* path(static_cast<const LineStop*>(edge.second)->getLine());
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
					if(!dynamic_cast<const LineStop*>(edge.second))
					{
						continue;
					}
					JourneyPattern* path(static_cast<const LineStop*>(edge.second)->getLine());
					if(	path->getCommercialLine() == &line)
					{
						return true;
					}
				}

				// Search on arrival edges
				BOOST_FOREACH(const Vertex::Edges::value_type& edge, stopPoint.getArrivalEdges())
				{
					if(!dynamic_cast<const LineStop*>(edge.second))
					{
						continue;
					}
					JourneyPattern* path(static_cast<const LineStop*>(edge.second)->getLine());
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

			if(_stopPointLocation)
			{
				// Search on departure edges
				BOOST_FOREACH(const Vertex::Edges::value_type& edge, _stopPointLocation->getDepartureEdges())
				{
					if(!dynamic_cast<const LineStop*>(edge.second))
					{
						continue;
					}
					JourneyPattern* path(static_cast<const LineStop*>(edge.second)->getLine());
					if(	path->getCommercialLine()->getNetwork() == &network)
					{
						return true;
					}
				}

				// Search on arrival edges
				BOOST_FOREACH(const Vertex::Edges::value_type& edge, _stopPointLocation->getArrivalEdges())
				{
					if(!dynamic_cast<const LineStop*>(edge.second))
					{
						continue;
					}
					JourneyPattern* path(static_cast<const LineStop*>(edge.second)->getLine());
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
					if(!dynamic_cast<const LineStop*>(edge.second))
					{
						continue;
					}
					JourneyPattern* path(static_cast<const LineStop*>(edge.second)->getLine());
					if(	path->getCommercialLine()->getNetwork() == &network)
					{
						return true;
					}
				}

				// Search on arrival edges
				BOOST_FOREACH(const Vertex::Edges::value_type& edge, stopPoint.getArrivalEdges())
				{
					if(!dynamic_cast<const LineStop*>(edge.second))
					{
						continue;
					}
					JourneyPattern* path(static_cast<const LineStop*>(edge.second)->getLine());
					if(	path->getCommercialLine()->getNetwork() == &network)
					{
						return true;
					}
				}
			}

			// Nothing was found
			return false;
		}
}	}
