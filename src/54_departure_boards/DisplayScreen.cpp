////////////////////////////////////////////////////////////////////////////////
/// DisplayScreen class implementation.
///	@file DisplayScreen.cpp
///	@author Hugues Romain
///
///	This file belongs to the SYNTHESE project (public transportation specialized
///	software)
///	Copyright (C) 2002 Hugues Romain - RCS <contact@reseaux-conseil.com>
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

#include "Registry.h"
#include "Interface.h"
#include "InterfacePageException.h"
#include "StopArea.hpp"
#include "StopPoint.hpp"
#include "Edge.h"
#include "DisplayScreen.h"
#include "DisplayType.h"
#include "DisplayMaintenanceLog.h"
#include "DeparturesTableInterfacePage.h"
#include "DeparturesTableRoutePlanningInterfacePage.h"
#include "DisplayScreenAlarmRecipient.h"
#include "DisplayScreenCPU.h"
#include "NamedPlace.h"
#include "DisplayMonitoringStatus.h"
#include "RoutePlanningTableGenerator.h"
#include "Webpage.h"
#include "DisplayScreenContentFunction.h"
#include "PTTimeSlotRoutePlanner.h"
#include "PTRoutePlannerResult.h"


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
	using namespace util;
	using namespace pt;
	using namespace dblog;
	using namespace interfaces;
	using namespace graph;
	using namespace road;
	using namespace geography;
	using namespace pt;
	using namespace pt_journey_planner;
	using namespace algorithm;
		
	

	namespace util
	{
		template<> const string Registry<departure_boards::DisplayScreen>::KEY("DisplayScreen");
	}

	namespace departure_boards
	{
		DisplayScreen::DisplayScreen(RegistryKeyType key)
		:	Registrable(key),
			_displayedPlace(NULL),
			_displayType(NULL),
			_wiringCode(0),
			_comPort(0),
			_blinkingDelay(1),
			_trackNumberDisplay(false),
			_serviceNumberDisplay(false),
			_displayTeam(false),
			_displayClock(true),
			_allPhysicalStopsDisplayed(true),
			_direction(DISPLAY_DEPARTURES),
			_originsOnly(WITH_PASSING),
			_maxDelay(12 * 60),			// default = 24 hours
			_clearingDelay(0),
			_firstRow(0),
			_routePlanningWithTransfer(false),
			_generationMethod(STANDARD_METHOD),
			_destinationForceDelay(120),	// default = 2 hours
			_maintenanceIsOnline(true)
		{
		}


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

		void DisplayScreen::setBlinkingDelay( int delay)
		{
			_blinkingDelay = delay;
		}

		void DisplayScreen::setTrackNumberDisplay( bool value )
		{
			_trackNumberDisplay = value;
		}

		void DisplayScreen::setServiceNumberDisplay( bool value )
		{
			_serviceNumberDisplay = value;
		}

		void DisplayScreen::addPhysicalStop(const StopPoint* physicalStop)
		{
			_physicalStops.insert(make_pair(physicalStop->getKey(),physicalStop));
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



		ArrivalDepartureList DisplayScreen::_generateStandardScreen(
			const boost::posix_time::ptime& startTime,
			const boost::posix_time::ptime& endTime,
			bool rootCall
		) const	{
			shared_ptr<ArrivalDepartureTableGenerator> generator;
			switch (_generationMethod)
			{
			case STANDARD_METHOD:
				generator.reset(
					static_cast<ArrivalDepartureTableGenerator*>(
						new StandardArrivalDepartureTableGenerator(
							getPhysicalStops(),
							_direction,
							_originsOnly,
							_forbiddenLines,
							_displayedPlaces,
							_forbiddenArrivalPlaces,
							startTime,
							endTime,
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
							_forbiddenLines,
							_displayedPlaces,
							_forbiddenArrivalPlaces,
							startTime,
							endTime,
							rootCall ? _displayType->getRowNumber() : 1,
							_forcedDestinations,
							minutes(_destinationForceDelay)
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

					Journey rootJourney(Journey(), itDest->serviceUse);

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
							continuationScreen->_generateStandardScreen(transferStartTime, transferEndTime, false)
						);
						if(!subResult.empty())
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

									Journey approachJourney(rootJourney, item->serviceUse);
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
				ptime routePlanningEndTime(startTime);
				routePlanningEndTime += days(1);
				BOOST_FOREACH(const TransferDestinationsList::mapped_type::value_type& it2, it->second)
				{
					PTTimeSlotRoutePlanner rp(
						_displayedPlace,
						it2,
						startTime,
						endTime,
						startTime,
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
						false
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



		void DisplayScreen::display(
			std::ostream& stream,
			const ptime& date,
			const server::Request* request
		) const {
			if(	!_displayType ||
				(	!_displayType->getDisplayInterface() && !_displayType->getDisplayMainPage()) ||
				!_maintenanceIsOnline ||
				!_displayedPlace
			){
				return;
			}

			try
			{
				// End time
				ptime realStartDateTime(date);
				realStartDateTime -= minutes(_clearingDelay);
				ptime endDateTime(realStartDateTime);
				endDateTime += minutes(_maxDelay);

				VariablesMap variables;

				if(_generationMethod == ROUTE_PLANNING)
				{
					RoutePlanningTableGenerator generator(
						*_displayedPlace,
						getDisplayedPlaces(),
						realStartDateTime,
						endDateTime,
						_routePlanningWithTransfer
					);

					RoutePlanningListWithAlarm displayedObject;
					displayedObject.map = generator.run();
					displayedObject.alarm = DisplayScreenAlarmRecipient::getAlarm(this, date);

					if(	_displayType->getDisplayInterface() &&
						_displayType->getDisplayInterface()->getPage<DeparturesTableRoutePlanningInterfacePage>()
					){
						_displayType->getDisplayInterface()->getPage<DeparturesTableRoutePlanningInterfacePage>()->display(
							stream,
							variables,
							getTitle(),
							getWiringCode(),
							getServiceNumberDisplay(),
							getTrackNumberDisplay(),
							getRoutePlanningWithTransfer(),
							getBlinkingDelay(),
							getDisplayClock(),
							*getDisplayedPlace(),
							displayedObject,
							request
						);
					}
					else
					{
						assert(_displayType->getDisplayMainPage());

						DisplayScreenContentFunction::DisplayRoutePlanningBoard(
							stream,
							*request,
							Env::GetOfficialEnv().getSPtr(_displayType->getDisplayMainPage()),
							Env::GetOfficialEnv().getSPtr(_displayType->getDisplayRowPage()),
							Env::GetOfficialEnv().getSPtr(_displayType->getDisplayDestinationPage()),
							getTitle(),
							getWiringCode(),
							getServiceNumberDisplay(),
							getTrackNumberDisplay(),
							getRoutePlanningWithTransfer(),
							posix_time::minutes(getBlinkingDelay()),
							getDisplayClock(),
							*getDisplayedPlace(),
							displayedObject,
							getChildren()
						);
					}
				}
				else if(_generationMethod == DISPLAY_CHILDREN_ONLY)
				{
					// computes nothing, children will be called by the cms method
				}
				else
				{
					ArrivalDepartureListWithAlarm displayedObject;
					displayedObject.map = _generateStandardScreen(realStartDateTime, endDateTime);
					displayedObject.alarm = DisplayScreenAlarmRecipient::getAlarm(this, date);

					if(	_displayType->getDisplayInterface() &&
						_displayType->getDisplayInterface()->getPage<DeparturesTableInterfacePage>()
					){
						_displayType->getDisplayInterface()->getPage<DeparturesTableInterfacePage>()->display(
							stream,
							variables,
							getTitle(),
							getWiringCode(),
							getServiceNumberDisplay(),
							getTrackNumberDisplay(),
							getDisplayTeam(),
							getType()->getMaxStopsNumber(),
							getBlinkingDelay(),
							getDisplayClock(),
							getDisplayedPlace(),
							displayedObject,
							request
						);
					}
					else
					{
						assert(_displayType->getDisplayMainPage());

						DisplayScreenContentFunction::DisplayDepartureBoard(
							stream,
							*request,
							Env::GetOfficialEnv().getSPtr(_displayType->getDisplayMainPage()),
							Env::GetOfficialEnv().getSPtr(_displayType->getDisplayRowPage()),
							Env::GetOfficialEnv().getSPtr(_displayType->getDisplayDestinationPage()),
							Env::GetOfficialEnv().getSPtr(_displayType->getDisplayTransferDestinationPage()),
							realStartDateTime,
							displayedObject,
							*this
						);
					}
				}
			}
			catch (InterfacePageException&)
			{
			}
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

		const LineFilter& DisplayScreen::getForbiddenLines() const
		{
			return _forbiddenLines;
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

		

		void DisplayScreen::setAllPhysicalStopsDisplayed( bool value )
		{
			_allPhysicalStopsDisplayed = value;
		}

		bool DisplayScreen::getAllPhysicalStopsDisplayed() const
		{
			return _allPhysicalStopsDisplayed;
		}

		void DisplayScreen::clearPhysicalStops()
		{
			_physicalStops.clear();
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



		DisplayScreen::Labels DisplayScreen::getSortedAvaliableDestinationsLabels(
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
			BOOST_FOREACH(const DisplayScreen* screen, screens)
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

		void DisplayScreen::removePhysicalStop(const StopPoint* stop)
		{
			ArrivalDepartureTableGenerator::PhysicalStops::iterator it = _physicalStops.find(stop->getKey());
			if (it != _physicalStops.end())
				_physicalStops.erase(it);
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
			BOOST_FOREACH(const ArrivalDepartureTableGenerator::PhysicalStops::value_type& it4, other.getPhysicalStops(false))
			{
				addPhysicalStop(it4.second);
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

		bool DisplayScreen::getRoutePlanningWithTransfer() const
		{
			return _routePlanningWithTransfer;
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
			if(getRoot<NamedPlace>())
			{
				return getRoot<NamedPlace>();
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
}	}
