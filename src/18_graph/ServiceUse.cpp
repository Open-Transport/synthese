
/** ServiceUse class implementation.
	@file ServiceUse.cpp

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

#include "ServiceUse.h"
#include "Service.h"
#include "Edge.h"
#include "UseRule.h"

using namespace boost;
using namespace boost::posix_time;

namespace synthese
{
	namespace graph
	{
		ServiceUse::ServiceUse(
			const ServicePointer& servicePointer,
			const Edge* edge
		):	ServicePointer(servicePointer),
			_secondEdge(edge),
			_secondRTVertex(edge->getFromVertex()),
			_secondActualDateTime(servicePointer.getService()->getLeaveTime(servicePointer, edge))
		{
		}



		ServiceUse::ServiceUse():
			ServicePointer(),
			_secondEdge(NULL),
			_secondRTVertex(NULL)
		{
		}


		const Edge* ServiceUse::getDepartureEdge() const
		{
			return (_determinationMethod == DEPARTURE_TO_ARRIVAL)
				? _edge
				: _secondEdge
				;
		}

		const Edge* ServiceUse::getArrivalEdge() const
		{
			return (_determinationMethod == ARRIVAL_TO_DEPARTURE)
				? _edge
				: _secondEdge
				;
		}

		const ptime& ServiceUse::getDepartureDateTime() const
		{
			return  (_determinationMethod == DEPARTURE_TO_ARRIVAL)
				? _actualTime
				: _secondActualDateTime
				;
		}

		const ptime& ServiceUse::getArrivalDateTime() const
		{
			return  (_determinationMethod == ARRIVAL_TO_DEPARTURE)
				? _actualTime
				: _secondActualDateTime
				;
		}

		const Edge* ServiceUse::getSecondEdge() const
		{
			return _secondEdge;
		}

		const ptime& ServiceUse::getSecondActualDateTime() const
		{
			return _secondActualDateTime;
		}

		posix_time::time_duration ServiceUse::getDuration() const
		{
			return getArrivalDateTime() - getDepartureDateTime();
		}

		double ServiceUse::getDistance() const
		{
			return getArrivalEdge()->getMetricOffset() - getDepartureEdge()->getMetricOffset();
		}

		void ServiceUse::shift(posix_time::time_duration duration)
		{
			if (duration.total_seconds() == 0)
				return;

			_actualTime += duration;
			_originDateTime += duration;
			_secondActualDateTime += duration;
			setServiceRange(getServiceRange() - duration);
		}



		UseRule::RunPossibilityType ServiceUse::isUseRuleCompliant(
		) const	{
			return
				getUseRule().isRunPossible(*this) == UseRule::RUN_POSSIBLE &&
				_service->nonConcurrencyRuleOK(
					_originDateTime.date(),
					*getDepartureEdge(),
					*getArrivalEdge(),
					_userClassRank
				) ?
				UseRule::RUN_POSSIBLE :
				UseRule::RUN_NOT_POSSIBLE
			;
		}



		ptime ServiceUse::getReservationDeadLine() const
		{
			UseRule::ReservationAvailabilityType resa(getUseRule().getReservationAvailability(*this));
			if(	resa == UseRule::RESERVATION_COMPULSORY_POSSIBLE ||
				resa == UseRule::RESERVATION_OPTIONAL_POSSIBLE
			){
				return getUseRule().getReservationDeadLine(
					_originDateTime,
					getDepartureDateTime()
				);
			}
			return ptime(not_a_date_time);
		}



		void ServiceUse::reverse()
		{
			// Reverse edge
			const Edge* edge(_secondEdge);
			const Vertex* vertex(_secondRTVertex);
			_secondEdge = _edge;
			_edge = edge;
			_secondRTVertex = _RTVertex;
			_RTVertex = vertex;

			// Reverse time
			ptime dateTime(_secondActualDateTime);
			_secondActualDateTime = _actualTime;
			_actualTime = dateTime;

			_determinationMethod = (_determinationMethod == DEPARTURE_TO_ARRIVAL) ? ARRIVAL_TO_DEPARTURE : DEPARTURE_TO_ARRIVAL;
		}



		const Vertex* ServiceUse::getSecondVertex() const
		{
			return _secondRTVertex;
		}



		const Vertex* ServiceUse::getDepartureRTVertex() const
		{
			return (_determinationMethod == DEPARTURE_TO_ARRIVAL)
				? _RTVertex
				: _secondRTVertex
			;
		}



		const Vertex* ServiceUse::getArrivalRTVertex() const
		{
			return (_determinationMethod == DEPARTURE_TO_ARRIVAL)
				? _secondRTVertex
				: _RTVertex
			;
		}
	}
}
