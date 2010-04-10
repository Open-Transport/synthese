
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
#include <boost/date_time/posix_time/ptime.hpp>
#include <boost/optional.hpp>

#include "ServiceUse.h"
#include "Registry.h"
#include "Journey.h"
#include "PublicTransportStopZoneConnectionPlace.h"

namespace synthese
{
	namespace pt
	{
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

	typedef std::map<boost::optional<util::RegistryKeyType>, const pt::PublicTransportStopZoneConnectionPlace*> DisplayedPlacesList;
	typedef std::map<util::RegistryKeyType,const pt::Line*> LineFilter;
	typedef std::map<boost::optional<util::RegistryKeyType>,const pt::PublicTransportStopZoneConnectionPlace*> ForbiddenPlacesList;
	typedef enum { DISPLAY_ARRIVALS = 0, DISPLAY_DEPARTURES = 1 } DeparturesTableDirection;
	typedef enum { ENDS_ONLY = 1, WITH_PASSING = 0 } EndFilter;
	
	struct DeparturesTableElementLess : public std::binary_function<graph::ServicePointer, graph::ServicePointer, bool>
	{
		bool operator()(const graph::ServicePointer& _Left, const graph::ServicePointer& _Right) const
		{
			return (_Left.getActualDateTime() < _Right.getActualDateTime()
				|| _Left.getActualDateTime() == _Right.getActualDateTime() 
				&& _Left.getEdge() < _Right.getEdge()
				);
		}
	};

	struct DeparturesTableServiceUseElementLess : public std::binary_function<graph::ServiceUse, graph::ServiceUse, bool>
	{
		bool operator()(const graph::ServiceUse& _Left, const graph::ServiceUse& _Right) const
		{
			return
				_Left.getActualDateTime() < _Right.getActualDateTime() ||
				_Left.getActualDateTime() == _Right.getActualDateTime() && _Left.getSecondActualDateTime() < _Right.getSecondActualDateTime() ||
				_Left.getActualDateTime() == _Right.getActualDateTime() && _Left.getSecondActualDateTime() == _Right.getSecondActualDateTime() && _Left.getSecondEdge() < _Right.getSecondEdge()
			;
		}
	};

	typedef std::map<
		const pt::PublicTransportStopZoneConnectionPlace*,
		std::set<const pt::PublicTransportStopZoneConnectionPlace*>
	> TransferDestinationsList;
	
	struct IntermediateStop
	{
		typedef std::set<graph::ServiceUse, DeparturesTableServiceUseElementLess> TransferDestinations;
		const pt::PublicTransportStopZoneConnectionPlace* place;
		graph::ServiceUse serviceUse;
		TransferDestinations transferDestinations;
		IntermediateStop(const pt::PublicTransportStopZoneConnectionPlace* _place) : place(_place), serviceUse(), transferDestinations() {}
		IntermediateStop(const pt::PublicTransportStopZoneConnectionPlace* _place, const graph::ServiceUse& _serviceUse, const TransferDestinations& _transferDestinations) : place(_place), serviceUse(_serviceUse), transferDestinations(_transferDestinations) {}
	};

	typedef std::vector<IntermediateStop> ActualDisplayedArrivalsList;
	
	typedef std::map<
		graph::ServicePointer,
		ActualDisplayedArrivalsList,
		DeparturesTableElementLess
	> ArrivalDepartureList;
	
	typedef ArrivalDepartureList::value_type ArrivalDepartureRow;

	struct ArrivalDepartureListWithAlarm
	{
		ArrivalDepartureList map;
		const messages::Alarm* alarm;
	};

	struct RoutePlanningListElementLess : public std::binary_function<pt::PublicTransportStopZoneConnectionPlace*, pt::PublicTransportStopZoneConnectionPlace*, bool>
	{
		bool operator()(const pt::PublicTransportStopZoneConnectionPlace* _Left, const pt::PublicTransportStopZoneConnectionPlace* _Right) const
		{
			return _Left->getFullName() < _Right->getFullName();
		}
	};
	typedef std::map<const pt::PublicTransportStopZoneConnectionPlace*, graph::Journey, RoutePlanningListElementLess> RoutePlanningList;
	typedef RoutePlanningList::value_type RoutePlanningRow;
	struct RoutePlanningListWithAlarm { RoutePlanningList map; const messages::Alarm* alarm; };

	/** @} */
}

#endif // SYNTHESE_Types_H__
