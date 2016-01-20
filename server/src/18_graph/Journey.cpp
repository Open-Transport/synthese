
/** Journey class implementation.
	@file Journey.cpp

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

#include "Journey.h"

#include "UseRule.h"
#include "Path.h"
#include "Service.h"
#include "Vertex.h"
#include "Edge.h"
#include "VertexAccessMap.h"
#include "Hub.h"

#include "UtilConstants.h"

#include <limits>
#include <boost/foreach.hpp>

#undef max

using namespace boost;
using namespace std;
using namespace boost::posix_time;

namespace synthese
{
	namespace graph
	{
		Journey::Journey(
		):	_effectiveDuration(posix_time::seconds(0)),
			_transportConnectionCount (0),
			_distance (0)
		{}



		Journey::~Journey ()
		{}



		size_t Journey::size(
		) const	{
			return _journeyLegs.size();
		}



		const ServicePointer& Journey::getJourneyLeg(
			size_t index
		) const	{
			assert(!empty());

			return _journeyLegs.at(index);
		}



		const ServicePointer& Journey::getFirstJourneyLeg () const
		{
			assert(!empty());

			return *_journeyLegs.begin();
		}



		const ServicePointer& Journey::getLastJourneyLeg () const
		{
			assert(!empty());

			return *_journeyLegs.rbegin();
		}



		const Edge* Journey::getOrigin(
		) const {
			assert(!empty());

			return getFirstJourneyLeg ().getDepartureEdge();
		}



		const Edge* Journey::getDestination(
		) const {
			assert(!empty());

			return getLastJourneyLeg ().getArrivalEdge();
		}



		void Journey::_updateInternalData( const ServicePointer& leg )
		{
			if(leg.getService() &&
				!leg.getService()->getPath()->isRoad())
			{
				++_transportConnectionCount;
			}
			_effectiveDuration += leg.getDuration();
			_distance += leg.getDistance();
		}



		void Journey::prepend(
			const ServicePointer& leg
		){
			// Update of transport connections count
			// Put this code always before push_front
			_updateInternalData(leg);

			// Storage of the service pointer
			_journeyLegs.push_front(leg);
		}



		void Journey::append(
			const ServicePointer& leg
		){
			// Update of transport connections count
			// Put this code always before push_front
			_updateInternalData(leg);

			// Storage of the service pointer
			_journeyLegs.push_back(leg);
		}



		void Journey::prepend(
			const Journey& journey
		){
			for(ServiceUses::const_reverse_iterator it(journey._journeyLegs.rbegin());
				it != journey._journeyLegs.rend();
				++it
			)	prepend(*it);
		}



		void Journey::append(
			const Journey& journey
		){
			for(ServiceUses::const_iterator it(journey._journeyLegs.begin());
				it != journey._journeyLegs.end();
				++it
			)	append(*it);
		}



		posix_time::time_duration Journey::getContinuousServiceRange(
		) const	{
			time_duration result(not_a_date_time);
			BOOST_FOREACH(const ServicePointer& leg, _journeyLegs)
			{
				if(	result.is_not_a_date_time() ||
					leg.getServiceRange() < result
				){
					result = leg.getServiceRange();
				}
				if (result.total_seconds() == 0) break;
			}
			return result;
		}



		void Journey::clear(
		){
			_effectiveDuration = posix_time::seconds(0);
			_transportConnectionCount = 0;
			_distance = 0;
			_journeyLegs.clear();
		}



		bool Journey::empty() const
		{
			return _journeyLegs.empty();
		}



		void Journey::shift(
			posix_time::time_duration duration
		){
			for(ServiceUses::iterator it(_journeyLegs.begin()); it != _journeyLegs.end(); ++it)
			{
				it->shift(duration);
			}
		}



		boost::logic::tribool Journey::getReservationCompliance(
			bool ignoreReservationDeadline,
			UseRule::ReservationDelayType reservationRulesDelayType
		) const
		{
			boost::logic::tribool result(false);
			BOOST_FOREACH(const ServicePointer& su, _journeyLegs)
			{
				if(!su.getDepartureEdge()->getReservationNeeded())
				{
					continue;
				}
				const UseRule::ReservationAvailabilityType& resa(
					su.getUseRule().getReservationAvailability(su, ignoreReservationDeadline, reservationRulesDelayType)
				);
				if(resa == UseRule::RESERVATION_COMPULSORY_POSSIBLE)
				{
					return true;
				}
				if(resa == UseRule::RESERVATION_OPTIONAL_POSSIBLE)
				{
					result = boost::logic::indeterminate;
				}
			}
			return result;
		}



		ptime Journey::getReservationDeadLine(
			UseRule::ReservationDelayType reservationRulesDelayType
		) const
		{
			ptime result(not_a_date_time);
			boost::logic::tribool compliance(getReservationCompliance(true));
			BOOST_FOREACH(const ServicePointer& su, _journeyLegs)
			{
				const UseRule::ReservationAvailabilityType& resa(
					su.getUseRule().getReservationAvailability(su, true, reservationRulesDelayType)
				);
				if(	(	boost::logic::indeterminate(compliance) &&
						resa == UseRule::RESERVATION_OPTIONAL_POSSIBLE
					)||(compliance == true &&
						resa == UseRule::RESERVATION_COMPULSORY_POSSIBLE
				)	){
					ptime deadLine(su.getReservationDeadLine(reservationRulesDelayType));
					if (result.is_not_a_date_time() || deadLine < result)
						result = deadLine;
				}
			}
			return result;
		}



		const boost::posix_time::ptime& Journey::getFirstDepartureTime() const
		{
			return getFirstJourneyLeg ().getDepartureDateTime();
		}



		boost::posix_time::ptime Journey::getLastDepartureTime() const
		{
			return getFirstJourneyLeg ().getDepartureDateTime() + getContinuousServiceRange();
		}



		const boost::posix_time::ptime& Journey::getFirstArrivalTime() const
		{
			return getLastJourneyLeg().getArrivalDateTime();
		}



		boost::posix_time::ptime Journey::getLastArrivalTime() const
		{
			return getLastJourneyLeg().getArrivalDateTime() + getContinuousServiceRange();
		}



		boost::posix_time::time_duration Journey::getDuration() const
		{
			if(empty())
			{
				return posix_time::time_duration(not_a_date_time);
			}
			return getFirstArrivalTime() - getFirstDepartureTime();
		}



		void Journey::forceContinuousServiceRange( boost::posix_time::time_duration duration )
		{
			BOOST_FOREACH(ServicePointer& service, _journeyLegs)
			{
				service.setServiceRange(duration);
			}
		}
}	}
