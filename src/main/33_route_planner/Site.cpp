
/** Site class implementation.
	@file Site.cpp

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

#include "33_route_planner/Site.h"
#include "33_route_planner/Types.h"

#include "04_time/DateTime.h"

using namespace boost;
using namespace std;

namespace synthese
{
	using namespace time;
	using namespace std;
	using namespace util;
	using namespace env;
	using namespace interfaces;

	namespace routeplanner
	{
		const string Site::TEMPS_MIN_CIRCULATIONS ("r");
		const string Site::TEMPS_MAX_CIRCULATIONS ("R");

		Site::Site(uid id)
			: Registrable<uid, Site>(id)
		{
			
		}

		shared_ptr<const Interface> Site::getInterface() const
		{
			return _interface;
		}

		bool Site::getOnlineBookingAllowed() const
		{
			return _onlineBookingAllowed;
		}

		bool Site::dateControl() const
		{
			DateTime tempDate;
			return tempDate.getDate() >= _startValidityDate && tempDate.getDate() <= _endValidityDate;
		}

		void Site::setInterface(shared_ptr<const Interface> interf )
		{
			_interface = interf;
		}

		void Site::setStartDate( const synthese::time::Date& dateDebut )
		{
			_startValidityDate = dateDebut;
		}

		void Site::setEndDate( const synthese::time::Date& dateFin )
		{
			_endValidityDate = dateFin;
		}

		void Site::setOnlineBookingAllowed( const bool valeur )
		{
			_onlineBookingAllowed = valeur;
		}

		void Site::setPastSolutionsDisplayed( bool pastSolutions)
		{
			_pastSolutionsDisplayed = pastSolutions;
		}

		void Site::setName( const std::string& name )
		{
			_name = name;
		}

		bool Site::getPastSolutionsDisplayed() const
		{
			return _pastSolutionsDisplayed;
		}

		const time::Date& Site::getStartDate() const
		{
			return _startValidityDate;
		}

		const time::Date& Site::getEndDate() const
		{
			return _endValidityDate;
		}

		const time::Date& Site::getMaxDateInUse() const
		{
			return _maxDateInUse;
		}

		const time::Date& Site::getMinDateInUse() const
		{
			return _minDateInUse;
		}

		Date Site::interpretDate( const std::string& text ) const
		{
			if ( text.empty() )
				return Date();

			if ( text == TEMPS_MIN_CIRCULATIONS)
				return getMinDateInUse ();

			if (text == TEMPS_MAX_CIRCULATIONS)
				return getMaxDateInUse ();

			if (text.size() == 1)
			{
				Date tempDate;
				tempDate.updateDate(text[ 0 ] );
				return tempDate;
			}

			return Date::FromString(text);
		}

		void Site::updateMinMaxDatesInUse (const time::Date& newDate, bool marked)
		{
			if (marked)
			{
				if ( (_minDateInUse == synthese::time::Date::UNKNOWN_DATE) ||
					(newDate < _minDateInUse) ) 
				{
					_minDateInUse = newDate;
				}

				if ( (_maxDateInUse == synthese::time::Date::UNKNOWN_DATE) ||
					(newDate > _maxDateInUse) ) 
				{
					_maxDateInUse = newDate;
				}
			}
			else
			{
				// TODO not written yet...
			}

		}

		void Site::setMaxTransportConnectionsCount( int number )
		{
			_maxTransportConnectionsCount = number;
		}

		int Site::getMaxTransportConnectionsCount() const
		{
			return _maxTransportConnectionsCount;
		}

		AccessParameters Site::getDefaultAccessParameters() const
		{
			AccessParameters ap;
			ap.maxTransportConnectionCount = _maxTransportConnectionsCount;
			
			return ap;
		}
	}
}
