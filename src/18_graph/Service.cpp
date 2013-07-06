
/** Service class implementation.
	@file Service.cpp

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

#include "Service.h"
#include "Path.h"
#include "Edge.h"
#include "Vertex.h"
#include "ParametersMap.h"

#include <iomanip>
#include <boost/lexical_cast.hpp>
#include <boost/foreach.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>

using namespace std;
using namespace boost;
using namespace boost::posix_time;
using namespace boost::gregorian;

namespace synthese
{
	using namespace util;

	namespace graph
	{
		const string Service::ATTR_SERVICE_ID = "service_id";
		const string Service::ATTR_SERVICE_NUMBER = "service_number";
		const string Service::ATTR_JOURNEY_PATTERN_ID = "journey_pattern_id";

		const time_duration Service::DAY_DURATION(hours(24));



		Service::Service(
			const string& serviceNumber,
			Path* path
		):	RuleUser(),
			_serviceNumber (serviceNumber),
			_path (path)
		{}



		Service::Service(
			RegistryKeyType id
		):	Registrable(id),
			RuleUser(),
			_path(NULL)
		{}



		Service::~Service ()
		{}



		const RuleUser* Service::_getParentRuleUser() const
		{
			return _path;
		}



		const string& Service::getServiceNumber () const
		{
			return _serviceNumber;
		}



		void Service::setServiceNumber(string serviceNumber)
		{
			_serviceNumber = serviceNumber;
		}


		Path* Service::getPath ()
		{
			return _path;
		}


		const Path* Service::getPath () const
		{
			return _path;
		}



		void Service::setPath( Path* path )
		{
			_path = path;
		}



		ptime Service::getOriginDateTime(
			bool RTData,
			const date& departureDate,
			const time_duration& departureTime
		) const	{
			return ptime(
				departureDate,
				departureTime - getDepartureSchedule(RTData,0)
			);
		}

		std::string Service::getTeam() const
		{
			return std::string();
		}



		bool Service::respectsLineTheoryWith(
			bool RTData,
			const Service& other
		) const {
			assert (_path != NULL);

			// Loop on each stop
			bool timeOrder;
			bool orderDefined(false);
			const Path::Edges& edges(_path->getEdges());
			for (Path::Edges::const_iterator it(edges.begin()); it != edges.end(); ++it)
			{
				const size_t i((*it)->getRankInPath());

				if ((*it)->isDeparture())
				{
					/// - Test 1 : Conflict between continuous service range or identical schedule
					if (getDepartureBeginScheduleToIndex(RTData, i) <= other.getDepartureEndScheduleToIndex(RTData, i) && getDepartureEndScheduleToIndex(RTData, i) >= other.getDepartureBeginScheduleToIndex(RTData, i))
						return false;

					/// - Test 2 : Order of times
					if (!orderDefined)
					{
						timeOrder = (getDepartureBeginScheduleToIndex(RTData, i) < other.getDepartureBeginScheduleToIndex(RTData, i));
						orderDefined = true;
					}
					else
					{
						if ((getDepartureBeginScheduleToIndex(RTData, i) < other.getDepartureBeginScheduleToIndex(RTData, i)) != timeOrder)
							return false;
					}
				}
				if ((*it)->isArrival())
				{
					/// - Test 1 : Conflict between continuous service range or identical schedule
					if (getArrivalBeginScheduleToIndex(RTData, i) <= other.getArrivalEndScheduleToIndex(RTData, i) && getArrivalEndScheduleToIndex(RTData, i) >= other.getArrivalBeginScheduleToIndex(RTData, i))
						return false;

					/// - Test 2 : Order of times
					if (!orderDefined)
					{
						timeOrder = (getArrivalBeginScheduleToIndex(RTData, i) < other.getArrivalBeginScheduleToIndex(RTData, i));
						orderDefined = true;
					}
					else
					{
						if ((getArrivalBeginScheduleToIndex(RTData, i) < other.getArrivalBeginScheduleToIndex(RTData, i)) != timeOrder)
							return false;
					}
				}
			}

			// No failure : return OK
			return true;
		}



		bool Service::nonConcurrencyRuleOK(
			ptime &time,
			time_duration &range,
			const graph::Edge& departureEdge,
			const graph::Edge& arrivalEdge,
			std::size_t userClassRank
		) const	{
			// By default we can run for the given range
			return true;
		}



		void Service::clearNonConcurrencyCache() const
		{

		}



		boost::posix_time::time_duration Service::GetTimeOfDay( const boost::posix_time::time_duration& value )
		{
			return
				value >= DAY_DURATION ?
				time_duration(value.hours() % 24, value.minutes(), value.seconds()) :
				value
			;
		}



		void Service::clearRTData()
		{
		}



		void Service::toParametersMap(
			util::ParametersMap& pm,
			bool withAdditionalParameters,
			boost::logic::tribool withFiles,
			std::string prefix
		) const {

			// ID
			pm.insert(prefix + ATTR_SERVICE_ID, getKey());

			// Number
			pm.insert(prefix + ATTR_SERVICE_NUMBER, getServiceNumber());

			// Path id
			if(_path)
			{
				pm.insert(prefix + ATTR_JOURNEY_PATTERN_ID, _path->getKey());
			}
		}
}	}
