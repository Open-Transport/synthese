
/** ServicePointer class implementation.
	@file ServicePointer.cpp

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

#include "ServicePointer.h"
#include "Service.h"
#include "UseRule.h"
#include "Edge.h"

using namespace boost;
using namespace boost::posix_time;

namespace synthese
{
	namespace graph
	{
		ServicePointer::ServicePointer(
			bool RTData,
			size_t userClassRank,
			const Service& service,
			const boost::posix_time::ptime& originDateTime
		):	_RTData(RTData),
			_service(&service),
			_range(posix_time::seconds(0)),
			_departureEdge(NULL),
			_realTimeDepartureVertex(NULL),
			_arrivalEdge(NULL),
			_realTimeArrivalVertex(NULL),
			_userClassRank(userClassRank),
			_originDateTime(originDateTime)
		{}



		ServicePointer::ServicePointer():
			_RTData(false),
			_service(NULL),
			_departureEdge(NULL),
			_realTimeDepartureVertex(NULL),
			_arrivalEdge(NULL),
			_realTimeArrivalVertex(NULL),
			_range(posix_time::seconds(0)),
			_userClassRank(0)
		{}



		ServicePointer::ServicePointer(
			const ServicePointer& partiallyFilledPointer,
			const Edge& edge
		):	_RTData(partiallyFilledPointer._RTData),
			_service(partiallyFilledPointer._service),
			_range(partiallyFilledPointer._range),
			_userClassRank(partiallyFilledPointer._userClassRank),
			_originDateTime(partiallyFilledPointer._originDateTime),
			_departureEdge(NULL),
			_arrivalEdge(NULL)
		{
			assert(!_departureEdge || !_arrivalEdge);

			if(partiallyFilledPointer.getDepartureEdge())
			{
				setDepartureInformations(
					*partiallyFilledPointer.getDepartureEdge(),
					partiallyFilledPointer.getDepartureDateTime(),
					partiallyFilledPointer.getTheoreticalDepartureDateTime(),
					*partiallyFilledPointer.getRealTimeDepartureVertex()
				);
			}
			else
			{
				setArrivalInformations(
					*partiallyFilledPointer.getArrivalEdge(),
					partiallyFilledPointer.getArrivalDateTime(),
					partiallyFilledPointer.getTheoreticalArrivalDateTime(),
					*partiallyFilledPointer.getRealTimeArrivalVertex()
				);
			}
			complete(edge);
		}


		const UseRule& ServicePointer::getUseRule() const
		{
			return _service->getUseRule(_userClassRank);
		}

		

		UseRule::RunPossibilityType ServicePointer::isUseRuleCompliant(
		) const	{
			if(getUseRule().isRunPossible(*this) != UseRule::RUN_POSSIBLE)
			{
				return UseRule::RUN_NOT_POSSIBLE;
			}
			if(_departureEdge && _arrivalEdge)
			{
				return _service->nonConcurrencyRuleOK(
					_originDateTime.date(),
					*getDepartureEdge(),
					*getArrivalEdge(),
					_userClassRank
				) ?
				UseRule::RUN_POSSIBLE :
				UseRule::RUN_NOT_POSSIBLE;
			}
			return UseRule::RUN_POSSIBLE;
		}



		void ServicePointer::setDepartureInformations(
			const graph::Edge& edge,
			const boost::posix_time::ptime& dateTime,
			const boost::posix_time::ptime& theoreticalDateTime,
			const Vertex& realTimeVertex
		){
			_departureEdge = &edge;
			_departureTime = dateTime;
			_theoreticalDepartureTime = theoreticalDateTime;
			_realTimeDepartureVertex = &realTimeVertex;
		}



		void ServicePointer::setArrivalInformations(
			const graph::Edge& edge,
			const boost::posix_time::ptime& dateTime,
			const boost::posix_time::ptime& theoreticalDateTime,
			const Vertex& realTimeVertex
		){
			_arrivalEdge = &edge;
			_arrivalTime = dateTime;
			_theoreticalArrivalTime = theoreticalDateTime;
			_realTimeArrivalVertex = &realTimeVertex;
		}



		void ServicePointer::complete( const Edge& edge )
		{
			assert(_departureEdge || _arrivalEdge);

			_service->completeServicePointer(*this, edge);
		}



		double ServicePointer::getDistance() const
		{
			assert(_arrivalEdge && _departureEdge);

			return _arrivalEdge->getMetricOffset() - _departureEdge->getMetricOffset();
		}



		boost::posix_time::time_duration ServicePointer::getDuration() const
		{
			return getArrivalDateTime() - getDepartureDateTime();
		}



		void ServicePointer::shift( boost::posix_time::time_duration duration )
		{
			if (duration.total_seconds() == 0)
				return;

			if(_departureEdge)
			{
				_departureTime += duration;
			}
			if(_arrivalEdge)
			{
				_arrivalTime += duration;
			}
			_originDateTime += duration;
			setServiceRange(getServiceRange() - duration);
		}



		boost::posix_time::ptime ServicePointer::getReservationDeadLine() const
		{
			assert(_departureEdge);

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
	}
}
