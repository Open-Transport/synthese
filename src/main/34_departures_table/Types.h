
/** Departures Tables Module Types definitions file.
	@file 34_departures_table/Types.h

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

#ifndef SYNTHESE_DeparturesTable_Types_H__
#define SYNTHESE_DeparturesTable_Types_H__

#include <functional>
#include <utility>
#include <vector>
#include <map>
#include <set>

#include "04_time/DateTime.h"

namespace synthese
{
	namespace env
	{
		class ConnectionPlace;
		class Line;
		class LineStop;
	}

	namespace messages
	{
		class Alarm;
	}

	/** @addtogroup m34
	@{
	*/

	typedef std::set<const env::ConnectionPlace*> DisplayedPlacesList;
	typedef std::set<const env::Line*> LineFilter;
	typedef std::set<const env::ConnectionPlace*> ForbiddenPlacesList;
	typedef enum { DISPLAY_ARRIVALS = 0, DISPLAY_DEPARTURES = 1 } DeparturesTableDirection;
	typedef enum { ENDS_ONLY = 0, WITH_PASSING = 1 } EndFilter;
	struct DeparturesTableElement { const env::LineStop* linestop; int serviceNumber; time::DateTime realDepartureTime; bool blinking; };
	struct DeparturesTableElementLess : public std::binary_function<DeparturesTableElement, DeparturesTableElement, bool>
	{
		bool operator()(const DeparturesTableElement& _Left, const DeparturesTableElement& _Right) const
		{
			return (_Left.realDepartureTime < _Right.realDepartureTime
				|| _Left.realDepartureTime == _Right.realDepartureTime 
				&& _Left.linestop != _Right.linestop
				);
		}
	};
	typedef std::vector<const env::ConnectionPlace*> ActualDisplayedArrivalsList;
	typedef std::pair<DeparturesTableElement, ActualDisplayedArrivalsList> ArrivalDepartureRow;
	typedef std::map<DeparturesTableElement, ActualDisplayedArrivalsList, DeparturesTableElementLess> ArrivalDepartureList;
	struct ArrivalDepartureListWithAlarm { ArrivalDepartureList map; const messages::Alarm* alarm; };

	typedef enum
	{
		DISPLAY_STATUS_UNKNOWN = -1
		, DISPLAY_STATUS_OK = 0
		, DISPLAY_STATUS_NO_NEWS_WARNING = 50
		, DISPLAY_STATUS_HARDWARE_WARNING = 60
		, DISPLAY_STATUS_NO_NEWS_ERROR = 80
		, DISPLAY_STATUS_HARDWARE_ERROR = 90
		, DISPLAY_STATUS_DISCONNECTED = 100
	} DisplayStatus;

	typedef enum
	{
		DISPLAY_DATA_UNKNOWN = -1
		, DISPLAY_DATA_OK = 0
		, DISPLAY_DATA_NO_LINES = 50
		, DISPLAY_DATA_CORRUPTED = 90
	} DisplayDataControlResult;

	/** @} */
}

#endif // SYNTHESE_Types_H__
