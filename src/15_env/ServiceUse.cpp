
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

#include "15_env/ServiceUse.h"
#include "15_env/Service.h"
#include "15_env/Edge.h"
#include "15_env/ReservationRule.h"

namespace synthese
{
	using namespace time;

	namespace env
	{

		ServiceUse::ServiceUse( const ServicePointer& servicePointer, const Edge* edge)
			: ServicePointer(servicePointer)
			, _secondEdge(edge)
			, _secondActualDateTime(servicePointer.getService()->getLeaveTime(servicePointer, edge))
		{
		}



		const env::Edge* ServiceUse::getDepartureEdge() const
		{
			return (_determinationMethod == DEPARTURE_TO_ARRIVAL)
				? _edge
				: _secondEdge
				;
		}

		const env::Edge* ServiceUse::getArrivalEdge() const
		{
			return (_determinationMethod == ARRIVAL_TO_DEPARTURE)
				? _edge
				: _secondEdge
				;
		}

		const time::DateTime& ServiceUse::getDepartureDateTime() const
		{
			return  (_determinationMethod == DEPARTURE_TO_ARRIVAL)
				? _actualTime
				: _secondActualDateTime
				;
		}

		const time::DateTime& ServiceUse::getArrivalDateTime() const
		{
			return  (_determinationMethod == ARRIVAL_TO_DEPARTURE)
				? _actualTime
				: _secondActualDateTime
				;
		}

		const env::Edge* ServiceUse::getSecondEdge() const
		{
			return _secondEdge;
		}

		const time::DateTime& ServiceUse::getSecondActualDateTime() const
		{
			return _secondActualDateTime;
		}

		int ServiceUse::getDuration() const
		{
			return getArrivalDateTime() - getDepartureDateTime();
		}

		int ServiceUse::getDistance() const
		{
			return static_cast<int>(getArrivalEdge()->getMetricOffset() - getDepartureEdge()->getMetricOffset());
		}

		void ServiceUse::shift( int duration)
		{
			if (duration == 0)
				return;

			_actualTime += duration;
			_originDateTime += duration;
			_secondActualDateTime += duration;
			setServiceRange(getServiceRange() - duration);
		}

		bool ServiceUse::isReservationRuleCompliant( const time::DateTime& computingDateTime ) const
		{
			if (_service->getReservationRule()->getType() != RESERVATION_FORBIDDEN)
				return _service->getReservationRule()->isRunPossible(
					_originDateTime,
					(_service->getReservationRule()->getType() == RESERVATION_MIXED_BY_DEPARTURE_PLACE) ? false : true,
					computingDateTime,
					getDepartureDateTime()
				);

			return true;

		}

		time::DateTime ServiceUse::getReservationDeadLine() const
		{
			if (_service->getReservationRule()->getType() != RESERVATION_FORBIDDEN)
				return _service->getReservationRule()->getReservationDeadLine(_originDateTime, getDepartureDateTime());

			return DateTime(TIME_UNKNOWN);
		}



		void ServiceUse::reverse()
		{
			// Reverse edge
			const Edge* edge(_secondEdge);
			_secondEdge = _edge;
			_edge = edge;

			// Reverse time
			DateTime dateTime(_secondActualDateTime);
			_secondActualDateTime = _actualTime;
			_actualTime = dateTime;

			_determinationMethod = (_determinationMethod == DEPARTURE_TO_ARRIVAL) ? ARRIVAL_TO_DEPARTURE : DEPARTURE_TO_ARRIVAL;
		}
	}
}
