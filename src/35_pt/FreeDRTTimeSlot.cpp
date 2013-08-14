
/** FreeDRTTimeSlot class implementation.
	@file FreeDRTTimeSlot.cpp

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

#include "FreeDRTTimeSlot.hpp"
#include "Place.h"
#include "FreeDRTArea.hpp"

#include <geos/geom/Point.h>

using namespace std;
using namespace boost::posix_time;

namespace synthese
{
	using namespace util;
	using namespace graph;

	namespace util
	{
		template<> const string Registry<pt::FreeDRTTimeSlot>::KEY("FreeDRTTimeSlot");
	}

	namespace pt
	{
		FreeDRTTimeSlot::FreeDRTTimeSlot(
			RegistryKeyType id
		):
			Registrable(id),
			NonPermanentService(id),
			_firstDeparture(not_a_date_time),
			_lastArrival(not_a_date_time),
			_commercialSpeed(0),
			_maxSpeed(0)
		{
		}



		boost::posix_time::time_duration FreeDRTTimeSlot::getJourneyDuration(
			const geography::Place& from,
			const geography::Place& to
		) const	{

			// Distance
			double distance(from.getPoint()->distance(to.getPoint().get()));

			// Time
			return minutes(long(0.06 * (distance / _commercialSpeed)));
		}



		Journey FreeDRTTimeSlot::getJourneyDepartureToArrival(
			const boost::posix_time::ptime time,
			const geography::Place& from,
			const geography::Place& to
		) const	{


			ServicePointer sp;

			// TODO Implementation

			return Journey();
		}



		Journey FreeDRTTimeSlot::getJourneyArrivalToDeparture(
			const boost::posix_time::ptime time,
			const geography::Place& from,
			const geography::Place& to
		) const	{

			ServicePointer sp;

			// TODO Implementation

			return Journey();

		}


		FreeDRTArea* FreeDRTTimeSlot::getArea() const
		{
			return static_cast<FreeDRTArea*>(_path);
		}



		void FreeDRTTimeSlot::setArea( FreeDRTArea* value )
		{
			_path = static_cast<Path*>(value);
		}



		void FreeDRTTimeSlot::_computeNextRTUpdate()
		{

		}



		bool FreeDRTTimeSlot::isContinuous() const
		{
			return true;
		}



		boost::posix_time::time_duration FreeDRTTimeSlot::getDepartureSchedule( bool RTData, std::size_t rank ) const
		{
			return _firstDeparture;
		}



		boost::posix_time::time_duration FreeDRTTimeSlot::getArrivalSchedule( bool RTData, std::size_t rank ) const
		{
			return _lastArrival;
		}



		const boost::posix_time::time_duration& FreeDRTTimeSlot::getLastArrivalSchedule( bool RTData ) const
		{
			return _lastArrival;
		}



		ServicePointer FreeDRTTimeSlot::getFromPresenceTime(
			const AccessParameters& accessParameters,
			bool THData,
			bool RTData,
			bool getDeparture,
			const graph::Edge& edge,
			const boost::posix_time::ptime& presenceDateTime,
			bool checkIfTheServiceIsReachable,
			bool inverted,
			bool ignoreReservation,
			bool allowCanceled
		) const	{
			return ServicePointer();
		}



		void FreeDRTTimeSlot::completeServicePointer( graph::ServicePointer& servicePointer, const graph::Edge& edge, const graph::AccessParameters& accessParameters ) const
		{

		}



		boost::posix_time::time_duration FreeDRTTimeSlot::getDepartureBeginScheduleToIndex( bool RTData, std::size_t rankInPath ) const
		{
			return _firstDeparture;
		}



		boost::posix_time::time_duration FreeDRTTimeSlot::getDepartureEndScheduleToIndex( bool RTData, std::size_t rankInPath ) const
		{
			return _lastArrival;
		}



		boost::posix_time::time_duration FreeDRTTimeSlot::getArrivalBeginScheduleToIndex( bool RTData, std::size_t rankInPath ) const
		{
			return _firstDeparture;
		}



		boost::posix_time::time_duration FreeDRTTimeSlot::getArrivalEndScheduleToIndex( bool RTData, std::size_t rankInPath ) const
		{
			return _lastArrival;
		}



		graph::UseRule::ReservationAvailabilityType FreeDRTTimeSlot::getReservationAbility(
			const boost::gregorian::date& date,
			std::size_t userClassRank
		) const	{

			if(!isActive(date))
			{
				return UseRule::RESERVATION_FORBIDDEN;
			}

			return UseRule::RESERVATION_COMPULSORY_POSSIBLE; // TODO temporary
		}



		boost::posix_time::ptime FreeDRTTimeSlot::getReservationDeadLine(
			const boost::gregorian::date& date,
			std::size_t userClassRank
		) const	{

			return ptime(date, _lastArrival); // TODO apply reservation rule delay
		}
}	}
