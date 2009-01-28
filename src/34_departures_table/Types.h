
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

#include "ServicePointer.h"
#include "DateTime.h"
#include "UId.h"


namespace synthese
{
	namespace env
	{
		class PublicTransportStopZoneConnectionPlace;
		class Line;
		class LineStop;
	}

	namespace messages
	{
		class Alarm;
	}

	/** @addtogroup m54
	@{
	*/

	//lint --e{1509}

	typedef enum {
		AT_LEAST_ONE_BROADCASTPOINT,
		NO_BROADCASTPOINT,
		WITH_OR_WITHOUT_ANY_BROADCASTPOINT
		} BroadcastPointsPresence;

	typedef std::map<uid,const env::PublicTransportStopZoneConnectionPlace*> DisplayedPlacesList;
	typedef std::map<uid,const env::Line*> LineFilter;
	typedef std::map<uid,const env::PublicTransportStopZoneConnectionPlace*> ForbiddenPlacesList;
	typedef enum { DISPLAY_ARRIVALS = 0, DISPLAY_DEPARTURES = 1 } DeparturesTableDirection;
	typedef enum { ENDS_ONLY = 0, WITH_PASSING = 1 } EndFilter;
	struct DeparturesTableElement { 
		const graph::ServicePointer servicePointer;
		bool blinking;
		DeparturesTableElement(
			const graph::ServicePointer& __servicePointer,
			bool __blinking
		):	servicePointer(__servicePointer),
			blinking(__blinking)
		{}
	};
	struct DeparturesTableElementLess : public std::binary_function<DeparturesTableElement, DeparturesTableElement, bool>
	{
		bool operator()(const DeparturesTableElement& _Left, const DeparturesTableElement& _Right) const
		{
			return (_Left.servicePointer.getActualDateTime() < _Right.servicePointer.getActualDateTime()
				|| _Left.servicePointer.getActualDateTime() == _Right.servicePointer.getActualDateTime() 
				&& _Left.servicePointer.getEdge() < _Right.servicePointer.getEdge()
				);
		}
	};
	typedef std::vector<const env::PublicTransportStopZoneConnectionPlace*> ActualDisplayedArrivalsList;
	typedef std::pair<DeparturesTableElement, ActualDisplayedArrivalsList> ArrivalDepartureRow;
	typedef std::map<DeparturesTableElement, ActualDisplayedArrivalsList, DeparturesTableElementLess> ArrivalDepartureList;
	struct ArrivalDepartureListWithAlarm { ArrivalDepartureList map; const messages::Alarm* alarm; };

	/** @} */
}

#endif // SYNTHESE_Types_H__
