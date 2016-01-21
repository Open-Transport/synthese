
/** ServicePointer class implementation.
	@file ServicePointer.cpp

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

#include "ServicePointer.h"
#include "Service.h"
#include "UseRule.h"
#include "Edge.h"
#include "AccessParameters.h"
#include "Vertex.h"
#include "AreaGeneratedLineStop.hpp"

#include <geos/geom/LineString.h>
#include <geos/geom/GeometryFactory.h>
#include <geos/geom/CoordinateSequenceFactory.h>

using namespace boost;
using namespace boost::posix_time;
using namespace geos::geom;

namespace synthese
{
	namespace graph
	{
		ServicePointer::ServicePointer(
			bool THData,
			bool RTData,
			size_t userClassRank,
			const Service& service,
			const boost::posix_time::ptime& originDateTime
		):	_departureEdge(NULL),
			_realTimeDepartureVertex(NULL),
			_arrivalEdge(NULL),
			_realTimeArrivalVertex(NULL),
			_userClassRank(userClassRank),
			_THData(THData),
			_RTData(RTData),
			_service(&service),
			_originDateTime(originDateTime),
			_range(posix_time::seconds(0)),
			_canceled(false)
		{}



		ServicePointer::ServicePointer(
			bool THData,
			bool RTData,
			size_t userClassRank,
			const Service& service,
			const gregorian::date& date,
			const Edge& departureEdge,
			const Edge& arrivalEdge
		):	_departureEdge(NULL),
			_realTimeDepartureVertex(NULL),
			_arrivalEdge(NULL),
			_realTimeArrivalVertex(NULL),
			_userClassRank(userClassRank),
			_THData(THData),
			_RTData(RTData),
			_service(&service),
			_originDateTime(ptime(date, service.getDepartureSchedule(RTData, 0))),
			_range(posix_time::seconds(0)),
			_canceled(false)
		{
			AccessParameters ap(userClassRank+ USER_CLASS_CODE_OFFSET);
			_service->completeServicePointer(*this, arrivalEdge, ap);
			_service->completeServicePointer(*this, departureEdge, ap);
		}



		ServicePointer::ServicePointer():
			_departureEdge(NULL),
			_realTimeDepartureVertex(NULL),
			_arrivalEdge(NULL),
			_realTimeArrivalVertex(NULL),
			_userClassRank(0),
			_THData(true),
			_RTData(false),
			_service(NULL),
			_range(posix_time::seconds(0)),
			_canceled(false)
		{}



		ServicePointer::ServicePointer(
			const ServicePointer& partiallyFilledPointer,
			const Edge& edge,
			const AccessParameters& accessParameters
		):	_departureEdge(NULL),
			_arrivalEdge(NULL),
			_userClassRank(partiallyFilledPointer._userClassRank),
			_THData(partiallyFilledPointer._THData),
			_RTData(partiallyFilledPointer._RTData),
			_service(partiallyFilledPointer._service),
			_originDateTime(partiallyFilledPointer._originDateTime),
			_range(partiallyFilledPointer._range),
			_canceled(false)
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
			_service->completeServicePointer(*this, edge, accessParameters);
		}


		const UseRule& ServicePointer::getUseRule() const
		{
			return _service->getUseRule(_userClassRank);
		}



		UseRule::RunPossibilityType ServicePointer::isUseRuleCompliant(
			bool ignoreReservation,
			UseRule::ReservationDelayType reservationRulesDelayType
		)	{
			ptime originDateTimeRef(_originDateTime);

			// Checks for complete service pointers
			if(_departureEdge && _arrivalEdge)
			{
				// Check of real time vertices
				if(	_RTData && (!_realTimeArrivalVertex || !_realTimeDepartureVertex) && !_canceled)
				{
					return UseRule::RUN_NOT_POSSIBLE;
				}

				// Check of non concurrency rules // RULE-213
				if(!_service->nonConcurrencyRuleOK(
						originDateTimeRef,
						_range,
						*getDepartureEdge(),
						*getArrivalEdge(),
						_userClassRank
					)
				)
				{
					return UseRule::RUN_NOT_POSSIBLE;
				}
			}

			// Check of use rule
			if(getUseRule().isRunPossible(*this, ignoreReservation, reservationRulesDelayType) != UseRule::RUN_POSSIBLE)
			{
				return UseRule::RUN_NOT_POSSIBLE;
			}

			// All checks are OK
			if(originDateTimeRef != _originDateTime)
			{
				shift(originDateTimeRef - _originDateTime);
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



		void ServicePointer::setDepartureInformations(
			const graph::Edge& edge,
			const boost::posix_time::ptime& dateTime,
			const boost::posix_time::ptime& theoreticalDateTime
		){
			_departureEdge = &edge;
			_departureTime = dateTime;
			_theoreticalDepartureTime = theoreticalDateTime;
			_realTimeDepartureVertex = NULL;
			_canceled = true;
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



		void ServicePointer::setArrivalInformations(
			const graph::Edge& edge,
			const boost::posix_time::ptime& dateTime,
			const boost::posix_time::ptime& theoreticalDateTime
		){
			_arrivalEdge = &edge;
			_arrivalTime = dateTime;
			_theoreticalArrivalTime = theoreticalDateTime;
			_realTimeArrivalVertex = NULL;
			_canceled = true;
		}



		double ServicePointer::getDistance() const
		{
			assert(_arrivalEdge && _departureEdge);

			if(_customGeometry)
			{
				return std::max(_customGeometry->getLength(), 1.0);
			}
			else
			{
				return _arrivalEdge->getMetricOffset() - _departureEdge->getMetricOffset();
			}
		}



		boost::posix_time::time_duration ServicePointer::getDuration() const
		{
			return getArrivalDateTime() - getDepartureDateTime();
		}



		void ServicePointer::shift( boost::posix_time::time_duration duration )
		{
			if (duration.total_seconds() == 0)
			{
				return;
			}

			if(_departureEdge)
			{
				_departureTime += duration;
				_theoreticalDepartureTime += duration;
			}
			if(_arrivalEdge)
			{
				_arrivalTime += duration;
				_theoreticalArrivalTime += duration;
			}
			_originDateTime += duration;
		}



		boost::posix_time::ptime ServicePointer::getReservationDeadLine(
			UseRule::ReservationDelayType reservationRulesDelayType
		) const
		{
			assert(_departureEdge);

			UseRule::ReservationAvailabilityType resa(getUseRule().getReservationAvailability(*this, true));
			if(	resa == UseRule::RESERVATION_COMPULSORY_POSSIBLE ||
				resa == UseRule::RESERVATION_OPTIONAL_POSSIBLE
			){
				return getUseRule().getReservationDeadLine(
					_originDateTime,
					getDepartureDateTime(),
					reservationRulesDelayType
				);
			}
			return ptime(not_a_date_time);
		}



		void ServicePointer::setCustomGeometry(boost::shared_ptr<geos::geom::LineString> geometry)
		{
			_customGeometry = boost::shared_ptr<geos::geom::LineString>(
				CoordinatesSystem::GetDefaultGeometryFactory().createLineString(*geometry)
			);
		}



		boost::shared_ptr<geos::geom::LineString> ServicePointer::getGeometry() const
		{
			assert(_departureEdge);
			assert(_arrivalEdge);

			if(_customGeometry)
			{
				return _customGeometry;
			}

			const GeometryFactory& geometryFactory(
				CoordinatesSystem::GetDefaultGeometryFactory()
			);

			CoordinateSequence* cs(geometryFactory.getCoordinateSequenceFactory()->create(0, 2));
			bool drtAreaSequence = false;
			bool hasDRTArea = false;
			Coordinate previousCoordinates;
			previousCoordinates.setNull();
			for(const Edge* edge(_departureEdge); edge != _arrivalEdge; edge = edge->getNext())
			{
				if (!edge)
				{
					edge = _arrivalEdge;
				}
				if(dynamic_cast<const pt::AreaGeneratedLineStop*>(edge))
				{
					hasDRTArea = true;	
					if(!drtAreaSequence) // True only for first DRTArea visited
					{
						drtAreaSequence = true;
						if(edge->getFromVertex()->getGeometry())
						{
							cs->add(*edge->getFromVertex()->getGeometry()->getCoordinate(),false);
						}
					}
					continue;
				}
				else
				{
					if(drtAreaSequence) // True if a DRTArea sequence is followed by a stop sequence
					{
						cs->add(*edge->getFromVertex()->getGeometry()->getCoordinate(),false);
						drtAreaSequence = false;
					}
				}
				
				boost::shared_ptr<LineString> geometry;
				try
				{
					geometry = edge->getRealGeometry();
				}
				catch (...)
				{
					if (edge == _arrivalEdge)
					{
						break;
					}
					continue;
				}
				if(!geometry.get() || geometry->isEmpty())
				{
					if (edge == _arrivalEdge)
					{
						break;
					}
					continue;
				}
				for(size_t i(0); i<geometry->getNumPoints(); ++i)
				{
					cs->add(geometry->getCoordinateN(i));
				}
				if (edge == _arrivalEdge)
				{
					break;
				}
			}
			if(drtAreaSequence) // Service end by DRTAreas
			{
				cs->add(*_arrivalEdge->getFromVertex()->getGeometry()->getCoordinate(),false);
			}
			cs->removeRepeatedPoints();
			if(cs->size() < 2)
			{
				return boost::shared_ptr<LineString>();
			}
			else if (hasDRTArea)
			{
				CoordinateSequence* csTwoPoints(geometryFactory.getCoordinateSequenceFactory()->create(0, 2));
				csTwoPoints->add(cs->getAt(0));
				csTwoPoints->add(cs->getAt(cs->getSize()-1));
				return boost::shared_ptr<LineString>(geometryFactory.createLineString(csTwoPoints));
			}
			else
			{
				return boost::shared_ptr<LineString>(geometryFactory.createLineString(cs));
			}
		}



		bool ServicePointer::operator==( const ServicePointer& rhs ) const
		{
			return
				_departureEdge == rhs._departureEdge &&
				_departureTime == rhs._departureTime &&
				_realTimeDepartureVertex == rhs._realTimeDepartureVertex &&
				_theoreticalDepartureTime == rhs._theoreticalDepartureTime &&
				_arrivalEdge == rhs._arrivalEdge &&
				_arrivalTime == rhs._arrivalTime &&
				_realTimeArrivalVertex == rhs._realTimeArrivalVertex &&
				_theoreticalArrivalTime == rhs._theoreticalArrivalTime &&
				_userClassRank == rhs._userClassRank &&
				_THData == rhs._THData &&
				_RTData == rhs._RTData &&
				_service == rhs._service &&
				_originDateTime == rhs._originDateTime &&
				_range == rhs._range &&
				_canceled == rhs._canceled;
		}



		bool ServicePointer::operator!=( const ServicePointer& rhs ) const
		{
			return !operator==(rhs);
		}



		bool ServicePointer::hysteresisCompare(
			const ServicePointer& rhs,
			const boost::posix_time::time_duration& hysteresis
		) const {
			return !operator==(rhs) &&
				(	_departureTime - rhs._departureTime >= hysteresis ||
					rhs._departureTime - _departureTime >= hysteresis);
		}
}	}
