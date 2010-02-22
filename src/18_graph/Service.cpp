
/** service class implementation.
	@file service.cpp

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

#include "Service.h"
#include "Path.h"
#include "Edge.h"

#include <iomanip>
#include <boost/lexical_cast.hpp>

using namespace std;
using namespace boost;
using namespace boost::posix_time;
using namespace boost::gregorian;

namespace synthese
{
	using namespace util;

	namespace graph
	{
		Service::Service(
			const string& serviceNumber
			, Path* path
		)	: RuleUser() 
			, _serviceNumber (serviceNumber)
			, _path (path)
		{
		}


		Service::Service(
			RegistryKeyType id
		):	RuleUser(),
			_path(NULL),
			Registrable(id)
		{
		}


		Service::~Service ()
		{
		}



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


		Path* 
		Service::getPath ()
		{
			return _path;
		}


		const Path* 
			Service::getPath () const
		{
			return _path;
		}



		void Service::setPath( Path* path )
		{
			_path = path;
			setPathId(path->getKey());
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

		void Service::setPathId( uid id )
		{
			_pathId = id;
		}

		uid Service::getPathId() const
		{
			return _pathId;
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
				const int i((*it)->getRankInPath());

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
			const date& date,
			const graph::Edge& departureEdge,
			const graph::Edge& arrivalEdge,
			graph::UserClassCode userClass
		) const	{
			return true;
		}



		void Service::clearNonConcurrencyCache() const
		{

		}





		boost::posix_time::time_duration Service::GetTimeOfDay( const boost::posix_time::time_duration& value )
		{
			return time_duration(value.hours() % 24, value.minutes(), value.seconds());
		}



		std::string Service::EncodeSchedule( const boost::posix_time::time_duration& value )
		{
			if(value.is_not_a_date_time())
			{
				return string();
			}

			std::stringstream os;

			os << std::setw( 2 ) << std::setfill ( '0' )
				<< (value.hours() / 24) << ":"
				<< std::setw( 2 ) << std::setfill ( '0' )
				<< (value.hours() % 24) << ":"
				<< std::setw( 2 ) << std::setfill ( '0' )
				<< value.minutes()
			;

			return os.str ();
		}



		boost::posix_time::time_duration Service::DecodeSchedule( const std::string value )
		{
			return time_duration(
				24 * lexical_cast<int>(value.substr(0, 2)) + lexical_cast<int>(value.substr(3, 2)),
				lexical_cast<int>(value.substr(6, 2)),
				0
			);
		}
	}
}
