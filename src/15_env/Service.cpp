
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

using namespace std;

namespace synthese
{
	using namespace time;

	namespace env
	{
		Service::Service(
			const string& serviceNumber
			, Path* path
		)	: Complyer () 
			, _serviceNumber (serviceNumber)
			, _path (path)
		{
			setComplianceParent(path);
		}


		Service::Service()
		: Complyer()
		, _path(NULL)
		{

		}


		Service::~Service ()
		{
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
			setPathId(path->getId());
			setComplianceParent(path);
		}

		DateTime Service::getOriginDateTime(const Date& departureDate, const Schedule& departureTime) const
		{
			DateTime originDateTime(departureDate);
			originDateTime -= (departureTime.getDaysSinceDeparture() - getDepartureSchedule().getDaysSinceDeparture());
			originDateTime.setHour(getDepartureSchedule().getHour());
			return originDateTime;
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

		bool Service::respectsLineTheoryWith( const Service& other ) const
		{
			assert (_path != NULL);

			// Loop on each stop
			bool timeOrder;
			bool orderDefined(false);
			const Path::Edges& edges(_path->getEdges());
			for (Path::Edges::const_iterator it(edges.begin()); it != edges.end(); ++it)
			{
				int i((*it)->getRankInPath());

				if ((*it)->isDeparture())
				{
					/// - Test 1 : Conflict between continuous service range or identical schedule
					if (getDepartureBeginScheduleToIndex(i) <= other.getDepartureEndScheduleToIndex(i) && getDepartureEndScheduleToIndex(i) >= other.getDepartureBeginScheduleToIndex(i))
						return false;

					/// - Test 2 : Order of times
					if (!orderDefined)
					{
						timeOrder = (getDepartureBeginScheduleToIndex(i) < other.getDepartureBeginScheduleToIndex(i));
						orderDefined = true;
					}
					else
					{
						if ((getDepartureBeginScheduleToIndex(i) < other.getDepartureBeginScheduleToIndex(i)) != timeOrder)
							return false;
					}
				}
				if ((*it)->isArrival())
				{
					/// - Test 1 : Conflict between continuous service range or identical schedule
					if (getArrivalBeginScheduleToIndex(i) <= other.getArrivalEndScheduleToIndex(i) && getArrivalEndScheduleToIndex(i) >= other.getArrivalBeginScheduleToIndex(i))
						return false;

					/// - Test 2 : Order of times
					if (!orderDefined)
					{
						timeOrder = (getArrivalBeginScheduleToIndex(i) < other.getArrivalBeginScheduleToIndex(i));
						orderDefined = true;
					}
					else
					{
						if ((getArrivalBeginScheduleToIndex(i) < other.getArrivalBeginScheduleToIndex(i)) != timeOrder)
							return false;
					}
				}
			}

			// No failure : return OK
			return true;
		}
	}
}
