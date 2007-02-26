
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

#include "34_departures_table/BroadcastPoint.h"
#include "34_departures_table/DisplayScreen.h"
#include "34_departures_table/DisplayType.h"
#include "34_departures_table/DisplayMaintenanceLog.h"
#include "34_departures_table/DeparturesTableInterfacePage.h"

using namespace std;

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
			, _localization(NULL)
			, _destinationForceDelay(120)	// default = 2 hours
			, _maxDelay(24 * 60)			// default = 24 hours
			, _displayType(NULL)
			, _wiringCode(0)
			, _blinkingDelay(1)
			, _clearingDelay(0)
			, _firstRow(0)
			, _maintenanceChecksPerDay(24 * 60)		// default = 1 check per minute
			, _allPhysicalStopsDisplayed(false)
			, _trackNumberDisplay(false)
			, _serviceNumberDisplay(false)
		{
		}

		DisplayScreen::~DisplayScreen(void)
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

		void DisplayScreen::addForbiddenPlace(const ConnectionPlace* place)
		{
			_forbiddenArrivalPlaces.insert(place);
		}


		/** Modificateur du point d'arrêt.
		*/
		void DisplayScreen::setLocalization(const BroadcastPoint* bp)
		{
			_localization = bp;
		}


		void DisplayScreen::addDisplayedPlace(const ConnectionPlace* __PointArret)
		{
			_displayedPlaces.insert(__PointArret);
		}

		

		void DisplayScreen::addForcedDestination(const ConnectionPlace* place)
		{
			_forcedDestinations.insert(place);
		}



		ArrivalDepartureTableGenerator* DisplayScreen::getGenerator(const DateTime& startDateTime) const
		{
			// End time
			DateTime endDateTime = startDateTime;
			endDateTime += _maxDelay;

			// Construction of the generator
			switch (_generationMethod)
			{
			case STANDARD_METHOD:
				return (ArrivalDepartureTableGenerator*) new StandardArrivalDepartureTableGenerator(
					getPhysicalStops()
					, _direction
					, _originsOnly
					, _forbiddenLines
					, _displayedPlaces
					, _forbiddenArrivalPlaces
					, startDateTime
					, endDateTime
					, _blinkingDelay
					, _displayType->getRowNumber()
				);

			case WITH_FORCED_DESTINATIONS_METHOD:
				return (ArrivalDepartureTableGenerator*) new ForcedDestinationsArrivalDepartureTableGenerator(
					getPhysicalStops()
					, _direction
					, _originsOnly
					, _forbiddenLines
					, _displayedPlaces
					, _forbiddenArrivalPlaces
					, startDateTime
					, endDateTime
					, _displayType->getRowNumber()
					, _forcedDestinations
					, _blinkingDelay
					, _destinationForceDelay
				);
			}
		}

		const BroadcastPoint* DisplayScreen::getLocalization() const
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

		void DisplayScreen::setType( const DisplayType* displayType)
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

		void DisplayScreen::addPhysicalStop( const env::PhysicalStop* physicalStop)
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
			if (!_displayType || !_displayType->getInterface())
				return;

			try
			{
				ArrivalDepartureTableGenerator* generator = getGenerator(date);
				const ArrivalDepartureList& result = generator->generate();
				const DeparturesTableInterfacePage* const page = _displayType->getInterface()->getPage<DeparturesTableInterfacePage>();
				page->display(stream, getTitle(), getWiringCode(), result);
				delete generator;
			}
			catch (InterfacePageException e)
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

		const PhysicalStopsSet& DisplayScreen::getPhysicalStops(bool result) const
		{
			return (_allPhysicalStopsDisplayed && _localization && result)
				? _localization->getConnectionPlace()->getPhysicalStops()
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

		int DisplayScreen::getMaintenananceChecksPerDay() const
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

		void DisplayScreen::recordSupervision( const std::string& supervisionValue ) const
		{
			DisplayMaintenanceLog* log = (DisplayMaintenanceLog*) Factory<DBLog>::create<DisplayMaintenanceLog>();
			/// @todo implementation
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
			if (_localization == NULL)
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

		std::map<std::string, std::pair<uid, std::string> > DisplayScreen::getSortedAvaliableDestinationsLabels(const std::set<const ConnectionPlace*>& placesToAvoid) const
		{
			map<std::string, std::pair<uid, string> > m;
			for (PhysicalStopsSet::const_iterator it = getPhysicalStops().begin(); it != getPhysicalStops().end(); ++it)
			{
				const PhysicalStop* p = *it;
				const std::set<const Edge*>& edges = p->getDepartureEdges();
				for (std::set<const Edge*>::const_iterator ite = edges.begin(); ite != edges.end(); ++ite)
				{
					for (const Edge* edge= (*ite)->getFollowingArrivalForFineSteppingOnly(); edge != NULL; edge = edge->getFollowingArrivalForFineSteppingOnly())
						m.insert(make_pair(edge->getConnectionPlace()->getFullName(), make_pair(edge->getConnectionPlace()->getKey(), edge->getConnectionPlace()->getFullName())));
				}
			}
			return m;
		}

		void DisplayScreen::removeForcedDestination( const env::ConnectionPlace* place)
		{
			DisplayedPlacesList::iterator it = _forcedDestinations.find(place);
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

		void DisplayScreen::removePhysicalStop( const env::PhysicalStop* stop)
		{
			PhysicalStopsSet::iterator it = _physicalStops.find(stop);
			if (it != _physicalStops.end())
				_physicalStops.erase(it);
		}

		void DisplayScreen::removeDisplayedPlace( const env::ConnectionPlace* place)
		{
			DisplayedPlacesList::iterator it = _displayedPlaces.find(place);
			if (it != _displayedPlaces.end())
				_displayedPlaces.erase(it);
		}

		void DisplayScreen::removeForbiddenPlace( const env::ConnectionPlace* place)
		{
			DisplayedPlacesList::iterator it = _forbiddenArrivalPlaces.find(place);
			if (it != _forbiddenArrivalPlaces.end())
				_forbiddenArrivalPlaces.erase(it);
		}

	}
}
