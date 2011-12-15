
/** FreeDRTArea class implementation.
	@file FreeDRTArea.cpp

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

#include "FreeDRTArea.hpp"
#include "FreeDRTTimeSlot.hpp"
#include "NamedPlace.h"
#include "CommercialLine.h"
#include "RollingStock.hpp"
#include "PTModule.h"

#include <boost/foreach.hpp>

using namespace std;
using namespace boost;
using namespace geos::geom;
using namespace boost::posix_time;

namespace synthese
{
	using namespace util;
	using namespace graph;
	using namespace geography;
	
	namespace util
	{
		template<> const string Registry<pt::FreeDRTArea>::KEY("FreeDRTArea");
	}

	namespace pt
	{
		FreeDRTArea::FreeDRTArea(
			util::RegistryKeyType key /*= 0 */
		):	Registrable(key)
		{}



		bool FreeDRTArea::includesPlace( const geography::Place& place ) const
		{
			// TODO implementation

			return false;
		}



		void FreeDRTArea::addTimeSlot(
			FreeDRTTimeSlot& value
		){
			Path::addService(value, false);
		}



		void FreeDRTArea::removeTimeSlot(
			FreeDRTTimeSlot& value
		){
			Path::removeService(value);
		}



		graph::Journey FreeDRTArea::getJourney(
			Direction direction,
			const boost::posix_time::ptime time,
			const geography::Place& from,
			const geography::Place& to
		) const	{

			// Check of geometries availability
			if(!from.getPoint().get())
			{
				throw MissingGeometryInPlaceException(*this, from);
			}
			if(!to.getPoint().get())
			{
				throw MissingGeometryInPlaceException(*this, to);
			}

			// Search for the best time slot
			const FreeDRTTimeSlot* bestTimeSlot(NULL);
			BOOST_FOREACH(const Service* service, _services)
			{
				// Declarations
				const FreeDRTTimeSlot& timeSlot(static_cast<const FreeDRTTimeSlot&>(*service));

				// Checks if the time slot is active at the specified date
				if(!timeSlot.isActive(time.date()))
				{
					continue;
				}

				// Journey duration
				time_duration journeyDuration(
					timeSlot.getJourneyDuration(from, to)
				);

				// Check the time
				if(direction == DEPARTURE_TO_ARRIVAL)
				{
					time_duration lastDeparture(timeSlot.getLastArrival());
					lastDeparture -= journeyDuration;
					if(lastDeparture > time.time_of_day())
					{
						continue;
					}
				}
				else
				{
					time_duration firstArrival(timeSlot.getFirstDeparture());
					firstArrival += journeyDuration;
					if(firstArrival < time.time_of_day())
					{
						continue;
					}
				}

				// If first available time slot then it is the best
				if(!bestTimeSlot)
				{
					bestTimeSlot = &timeSlot;
					continue;
				}

				// Time slot time comparison
				if(direction == DEPARTURE_TO_ARRIVAL)
				{
					if(bestTimeSlot->getFirstDeparture() < timeSlot.getFirstDeparture())
					{
						bestTimeSlot = &timeSlot;
					}
				}
				else
				{
					if(bestTimeSlot->getLastArrival() > timeSlot.getLastArrival())
					{
						bestTimeSlot = &timeSlot;
					}
				}
			}

			// Journey build
			if(direction == DEPARTURE_TO_ARRIVAL)
			{
				return bestTimeSlot->getJourneyDepartureToArrival(time, from, to);
			}
			else
			{
				return bestTimeSlot->getJourneyArrivalToDeparture(time, from, to);
			}
		}



		CommercialLine* FreeDRTArea::getLine() const
		{
			return static_cast<CommercialLine*>(_pathGroup);
		}



		void FreeDRTArea::setLine( CommercialLine* value )
		{
			_pathGroup = value;
		}



		void FreeDRTArea::setRollingStock( RollingStock* value)
		{
			_pathClass = static_cast<PathClass*>(value);
		}



		RollingStock* FreeDRTArea::getRollingStock() const
		{
			return static_cast<RollingStock*>(_pathClass);
		}



		FreeDRTArea::ReachableStopAreas FreeDRTArea::getReachableStopAreas(
			Direction direction,
			const boost::posix_time::ptime time,
			const geography::Place& place
		) const	{

			ReachableStopAreas result;

			// TODO implementation

			return result;
		}



		std::string FreeDRTArea::getRuleUserName() const
		{
			return "Zone " + getName();
		}



		bool FreeDRTArea::isPedestrianMode() const
		{
			return false;
		}



		bool FreeDRTArea::isActive( const boost::gregorian::date& date ) const
		{
			return true;
		}



		bool FreeDRTArea::isDepartureAllowed() const
		{
			return true;
		}



		bool FreeDRTArea::isArrivalAllowed() const
		{
			return true;
		}



		graph::GraphIdType FreeDRTArea::getGraphType() const
		{
			return PTModule::GRAPH_ID;
		}



		FreeDRTArea::MissingGeometryInPlaceException::MissingGeometryInPlaceException(
			const FreeDRTArea& area,
			const geography::Place& place
		):	Exception(
				"The place "+ (dynamic_cast<const NamedPlace*>(&place) ? dynamic_cast<const NamedPlace&>(place).getFullName() : string()) +
				" is not located. The Free DRT Area "+ area.getName() +" ("+ lexical_cast<string>(area.getKey()) +
				") cannot use it."
			)
		{}
}	}
