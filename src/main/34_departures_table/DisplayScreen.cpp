
/** DisplayScreen class implementation.
	@file DisplayScreen.cpp

	This file belongs to the SYNTHESE project (public transportation specialized software)
	Copyright (C) 2002 Hugues Romain - RCS <contact@reseaux-conseil.com>

	This program is free software; you can redistribute it and/or
	modify it under the terms of the GNU General Public License
	as published by the Free Software Foundation; either version 2
	of the License, or (at your option) any later version.

	This program is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with this program; if not, write to the Free Software
	Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
*/

#include <sstream>

#include "11_interfaces/Interface.h"
#include "11_interfaces/InterfacePageException.h"

#include "15_env/ConnectionPlace.h"
#include "15_env/PhysicalStop.h"
#include "15_env/Edge.h"
#include "15_env/Types.h"

#include "34_departures_table/DisplayScreen.h"
#include "34_departures_table/DisplayType.h"
#include "34_departures_table/DisplayMaintenanceLog.h"
#include "34_departures_table/DeparturesTableInterfacePage.h"
#include "34_departures_table/DisplayScreenAlarmRecipient.h"

using namespace std;
using namespace boost;

namespace synthese
{
	using namespace util;
	using namespace env;
	using namespace time;
	using namespace dblog;
	using namespace interfaces;

	namespace departurestable
	{
		DisplayScreen::DisplayScreen()
			: Registrable<uid, DisplayScreen>()
			, _direction(DISPLAY_DEPARTURES)
			, _generationMethod(STANDARD_METHOD)
			, _originsOnly(WITH_PASSING)
			, _destinationForceDelay(120)	// default = 2 hours
			, _maxDelay(24 * 60)			// default = 24 hours
			, _wiringCode(0)
			, _blinkingDelay(1)
			, _clearingDelay(0)
			, _firstRow(0)
			, _maintenanceChecksPerDay(24 * 60)		// default = 1 check per minute
			, _allPhysicalStopsDisplayed(false)
			, _trackNumberDisplay(false)
			, _serviceNumberDisplay(false)
			, _displayType(NULL)
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

		void DisplayScreen::addForbiddenPlace(const env::ConnectionPlace* place)
		{
			_forbiddenArrivalPlaces.insert(place);
		}


		/** Modificateur du point d'arr�t.
		*/
		void DisplayScreen::setLocalization(shared_ptr<const ConnectionPlace> bp)
		{
			_localization = bp;
		}


		void DisplayScreen::addDisplayedPlace(const env::ConnectionPlace* __PointArret)
		{
			_displayedPlaces.insert(__PointArret);
		}

		

		void DisplayScreen::addForcedDestination(const env::ConnectionPlace* place)
		{
			_forcedDestinations.insert(place);
		}



		shared_ptr<ArrivalDepartureTableGenerator> DisplayScreen::getGenerator(const DateTime& startDateTime) const
		{
			// End time
			DateTime realStartDateTime(startDateTime);
			realStartDateTime += (-_clearingDelay + 1);
			DateTime endDateTime(realStartDateTime);
			endDateTime += _maxDelay;

			// Construction of the generator
			switch (_generationMethod)
			{
			case STANDARD_METHOD:
				return shared_ptr<ArrivalDepartureTableGenerator>((ArrivalDepartureTableGenerator*) new StandardArrivalDepartureTableGenerator(
					getPhysicalStops()
					, _direction
					, _originsOnly
					, _forbiddenLines
					, _displayedPlaces
					, _forbiddenArrivalPlaces
					, realStartDateTime
					, endDateTime
					, _blinkingDelay
					, _displayType->getRowNumber()
				));

			case WITH_FORCED_DESTINATIONS_METHOD:
				return shared_ptr<ArrivalDepartureTableGenerator>((ArrivalDepartureTableGenerator*) new ForcedDestinationsArrivalDepartureTableGenerator(
					getPhysicalStops()
					, _direction
					, _originsOnly
					, _forbiddenLines
					, _displayedPlaces
					, _forbiddenArrivalPlaces
					, realStartDateTime
					, endDateTime
					, _displayType->getRowNumber()
					, _forcedDestinations
					, _destinationForceDelay
					, _blinkingDelay
				));
			}
		}

		shared_ptr<const ConnectionPlace> DisplayScreen::getLocalization() const
		{
			return _localization;
		}

		const string& DisplayScreen::getLocalizationComment() const
		{
			return _localizationComment;
		}

		void DisplayScreen::setLocalizationComment( const std::string& text)
		{
			_localizationComment = text;
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

		void DisplayScreen::addPhysicalStop(const PhysicalStop* physicalStop)
		{
			_physicalStops.insert(physicalStop);
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

		void DisplayScreen::setMaintenanceChecksPerDay( int number )
		{
			_maintenanceChecksPerDay = number;
		}

		void DisplayScreen::setMaintenanceIsOnline( bool value )
		{
			_maintenanceIsOnline = value;
		}

		void DisplayScreen::setMaintenanceMessage( const std::string& message )
		{
			_maintenanceMessage = message;
		}

		void DisplayScreen::display( std::ostream& stream, const DateTime& date ) const
		{
			if (!_displayType || !_displayType->getInterface() || !_maintenanceIsOnline)
				return;

			try
			{
				shared_ptr<ArrivalDepartureTableGenerator> generator = getGenerator(date);
				ArrivalDepartureListWithAlarm displayedObject;
				displayedObject.map = generator->generate();
				displayedObject.alarm = DisplayScreenAlarmRecipient::getAlarm(this);
				shared_ptr<const DeparturesTableInterfacePage> page(_displayType->getInterface()->getPage<DeparturesTableInterfacePage>());
				VariablesMap variables;
				page->display(stream, variables, getTitle(), getWiringCode(), getServiceNumberDisplay(), displayedObject);
			}
			catch (InterfacePageException& e)
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

		const PhysicalStops& DisplayScreen::getPhysicalStops(bool result) const
		{
			return (_allPhysicalStopsDisplayed && _localization && result)
				? _localization->getPhysicalStops()
				: _physicalStops;
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

		int DisplayScreen::getMaintenanceChecksPerDay() const
		{
			return _maintenanceChecksPerDay;
		}

		bool DisplayScreen::getIsOnline() const
		{
			return _maintenanceIsOnline;
		}

		const std::string& DisplayScreen::getMaintenanceMessage() const
		{
			return _maintenanceMessage;
		}

		const DisplayType* DisplayScreen::getType() const
		{
			return _displayType;
		}

		void DisplayScreen::recordSupervision(const std::string& text ) const
		{

			shared_ptr<const Interface> interf(_displayType->getInterface());
//			DisplayMaintenanceLog::addControlEntry(this, level, text);
			
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
			if (!_localization.get())
				return "(not localized)";
			else
			{
				stringstream s;
				s << _localization->getFullName();
				if (_localizationComment.size())
					s << "/" << _localizationComment;
				return s.str();
			}
		}

		std::vector<std::pair<uid, std::string> > DisplayScreen::getSortedAvaliableDestinationsLabels(const std::set<const env::ConnectionPlace*>& placesToAvoid) const
		{
			map<std::string, std::pair<uid, string> > m;
			for (PhysicalStops::const_iterator it = getPhysicalStops().begin(); it != getPhysicalStops().end(); ++it)
			{
				const PhysicalStop* p = *it;
				const std::set<const Edge*>& edges = p->getDepartureEdges();
				for (std::set<const Edge*>::const_iterator ite = edges.begin(); ite != edges.end(); ++ite)
				{
					for (const Edge* edge= (*ite)->getFollowingArrivalForFineSteppingOnly(); edge != NULL; edge = edge->getFollowingArrivalForFineSteppingOnly())
						m.insert(make_pair(edge->getConnectionPlace()->getFullName(), make_pair(edge->getConnectionPlace()->getKey(), edge->getConnectionPlace()->getFullName())));
				}
			}
			vector<pair<uid, string> > v;
			for (map<string, pair<uid, string> >::const_iterator it = m.begin(); it != m.end(); ++it)
				v.push_back(it->second);
			return v;
		}

		void DisplayScreen::removeForcedDestination(shared_ptr<const ConnectionPlace> place)
		{
			DisplayedPlacesList::iterator it = _forcedDestinations.find(place.get());
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

		void DisplayScreen::removePhysicalStop(shared_ptr<const PhysicalStop> stop)
		{
			PhysicalStops::iterator it = _physicalStops.find(stop.get());
			if (it != _physicalStops.end())
				_physicalStops.erase(it);
		}

		void DisplayScreen::removeDisplayedPlace(shared_ptr<const ConnectionPlace> place)
		{
			DisplayedPlacesList::iterator it = _displayedPlaces.find(place.get());
			if (it != _displayedPlaces.end())
				_displayedPlaces.erase(it);
		}

		void DisplayScreen::removeForbiddenPlace(shared_ptr<const ConnectionPlace> place)
		{
			DisplayedPlacesList::iterator it = _forbiddenArrivalPlaces.find(place.get());
			if (it != _forbiddenArrivalPlaces.end())
				_forbiddenArrivalPlaces.erase(it);
		}

		void DisplayScreen::copy( boost::shared_ptr<const DisplayScreen> other )
		{
			setAllPhysicalStopsDisplayed(other->getAllPhysicalStopsDisplayed());
			setBlinkingDelay(other->getBlinkingDelay());
			setClearingDelay(other->getClearingDelay());
			setDestinationForceDelay(other->getForceDestinationDelay());
			setDirection(other->getDirection());
			setFirstRow(other->getFirstRow());
			setGenerationMethod(other->getGenerationMethod());
			setLocalization(other->getLocalization());
			setMaintenanceChecksPerDay(other->getMaintenanceChecksPerDay());
			setMaxDelay(other->getMaxDelay());
			setOriginsOnly(other->getEndFilter());
			setServiceNumberDisplay(other->getServiceNumberDisplay());
			setTitle(other->getTitle());
			setTrackNumberDisplay(other->getTrackNumberDisplay());
			setType(other->getType());
			setWiringCode(other->getWiringCode());
			for (DisplayedPlacesList::const_iterator it = other->getDisplayedPlaces().begin(); it != other->getDisplayedPlaces().end(); ++it)
				addDisplayedPlace(*it);
			for (DisplayedPlacesList::const_iterator it = other->getForcedDestinations().begin(); it != other->getForcedDestinations().end(); ++it)
				addForcedDestination(*it);
			for (ForbiddenPlacesList::const_iterator it = other->getForbiddenPlaces().begin(); it != other->getForbiddenPlaces().end(); ++it)
				addForbiddenPlace(*it);
			for (PhysicalStops::const_iterator it = other->getPhysicalStops(false).begin(); it != other->getPhysicalStops(false).end(); ++it)
				addPhysicalStop(*it);
		}

		const DisplayScreen::Complements& DisplayScreen::getComplements() const
		{
			return _complements;
		}

		void DisplayScreen::setComplements( const Complements& complements )
		{
			_complements = complements;
		}
	}
}
