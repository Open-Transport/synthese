
/** Departures Tables Module Types definitions file.
	@file DeparturesTableTypes.h

	This file belongs to the SYNTHESE project (public transportation specialized software)
	Copyright (C) 2002 Hugues Romain - RCSmobility <contact@rcsmobility.com>

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

#include "ServicePointer.h"
#include "Registry.h"
#include "Journey.h"
#include "StopArea.hpp"

namespace synthese
{
	namespace pt
	{
		class CommercialLine;
		class LineStop;
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

	typedef std::map<boost::optional<util::RegistryKeyType>, const pt::StopArea*> DisplayedPlacesList;
	typedef std::map<const pt::CommercialLine*, boost::optional<bool> > LineFilter;
	typedef std::map<boost::optional<util::RegistryKeyType>,const pt::StopArea*> ForbiddenPlacesList;
	typedef enum { DISPLAY_ARRIVALS = 0, DISPLAY_DEPARTURES = 1 } DeparturesTableDirection;
	typedef enum { ENDS_ONLY = 1, WITH_PASSING = 0 } EndFilter;

	struct DeparturesTableElementLess : public std::binary_function<graph::ServicePointer, graph::ServicePointer, bool>
	{
		bool operator()(const graph::ServicePointer& _Left, const graph::ServicePointer& _Right) const
		{
			return (_Left.getDepartureDateTime() < _Right.getDepartureDateTime()
				|| ((_Left.getDepartureDateTime() == _Right.getDepartureDateTime()
				&& _Left.getDepartureEdge() < _Right.getDepartureEdge()))
				);
		}
	};

	struct DeparturesTableServiceUseElementLess : public std::binary_function<graph::ServicePointer, graph::ServicePointer, bool>
	{
		bool operator()(const graph::ServicePointer& _Left, const graph::ServicePointer& _Right) const
		{
			return
				_Left.getDepartureDateTime() < _Right.getDepartureDateTime() ||
				((_Left.getDepartureDateTime() == _Right.getDepartureDateTime() && _Left.getArrivalDateTime() < _Right.getArrivalDateTime())) ||
				((_Left.getDepartureDateTime() == _Right.getDepartureDateTime() && _Left.getArrivalDateTime() == _Right.getArrivalDateTime() && _Left.getArrivalEdge() < _Right.getArrivalEdge()))
			;
		}
	};

	typedef std::map<
		const pt::StopArea*,
		std::set<const pt::StopArea*>
	> TransferDestinationsList;


	struct IntermediateStop;

	typedef std::vector<IntermediateStop> ActualDisplayedArrivalsList;

	struct IntermediateStop
	{
		typedef std::set<graph::ServicePointer, DeparturesTableServiceUseElementLess> TransferDestinations;
		const pt::StopArea* place;
		graph::ServicePointer serviceUse;
		TransferDestinations transferDestinations;
		graph::ServicePointer continuationService;
		ActualDisplayedArrivalsList destinationsReachedByContinuationService;

		IntermediateStop(const pt::StopArea* _place, const graph::ServicePointer& _serviceUse) : place(_place), serviceUse(_serviceUse) {}
	};


	typedef std::map<
		graph::ServicePointer,
		ActualDisplayedArrivalsList,
		DeparturesTableElementLess
	> ArrivalDepartureList;

	typedef ArrivalDepartureList::value_type ArrivalDepartureRow;

	struct RoutePlanningListElementLess : public std::binary_function<pt::StopArea*, pt::StopArea*, bool>
	{
		bool operator()(const pt::StopArea* _Left, const pt::StopArea* _Right) const
		{
			return _Left->getFullName() < _Right->getFullName();
		}
	};
	typedef std::map<const pt::StopArea*, graph::Journey, RoutePlanningListElementLess> RoutePlanningList;
	typedef RoutePlanningList::value_type RoutePlanningRow;

	/** @} */
}

#endif // SYNTHESE_Types_H__
