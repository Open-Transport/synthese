
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

#include "34_departures_table/DisplayScreen.h"
#include "34_departures_table/DisplayType.h"

using namespace std;

namespace synthese
{
	using namespace util;
	using namespace env;
	using namespace time;

	namespace departurestable
	{
		DisplayScreen::DisplayScreen()
			: Registrable<uid, DisplayScreen>()
			, _originsOnly(WITH_PASSING)
			, _localization(NULL)
			, _destinationForceDelay(120)	// default = 2 hours
			, _maxDelay(24 * 60)			// default = 24 hours
			, _displayType(NULL)
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
					_physicalStops
					, _direction
					, _originsOnly
					, _forbiddenLines
					, _displayedPlaces
					, _forbiddenArrivalPlaces
					, startDateTime
					, endDateTime
					, _displayType->getRowNumber()
				);

			case WITH_FORCED_DESTINATIONS_METHOD:
				return (ArrivalDepartureTableGenerator*) new ForcedDestinationsArrivalDepartureTableGenerator(
					_physicalStops
					, _direction
					, _originsOnly
					, _forbiddenLines
					, _displayedPlaces
					, _forbiddenArrivalPlaces
					, startDateTime
					, endDateTime
					, _displayType->getRowNumber()
					, _forcedDestinations
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

		void DisplayScreen::display( std::ostream& stream ) const
		{
			
		}

	}
}
