
/** DeadRun class implementation.
	@file DeadRun.cpp

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

#include "DeadRun.hpp"
#include "DeadRunEdge.hpp"
#include "Depot.hpp"
#include "StopPoint.hpp"
#include "ForbiddenUseRule.h"

using namespace std;

namespace synthese
{
	using namespace util;
	using namespace pt;
	using namespace graph;

	namespace util
	{
		template<>
		const std::string Registry<pt_operation::DeadRun>::KEY("DeadRun");
	}

	namespace pt_operation
	{
		DeadRun::DeadRun(
			RegistryKeyType id,
			string number
		):	Registrable(id),
			SchedulesBasedService(number, NULL),
			_network(NULL)
		{
			setPath(this);
			_vertices.push_back(NULL);
			_vertices.push_back(NULL);

			// Default use rules
			RuleUser::Rules rules(RuleUser::GetEmptyRules());
			rules[USER_PEDESTRIAN - USER_CLASS_CODE_OFFSET] = ForbiddenUseRule::INSTANCE.get();
			rules[USER_BIKE - USER_CLASS_CODE_OFFSET] = ForbiddenUseRule::INSTANCE.get();
			rules[USER_HANDICAPPED - USER_CLASS_CODE_OFFSET] = ForbiddenUseRule::INSTANCE.get();
			rules[USER_CAR - USER_CLASS_CODE_OFFSET] = ForbiddenUseRule::INSTANCE.get();
			Path::setRules(rules);
		}



		DeadRun::~DeadRun()
		{
			setUndefined();
		}



		void DeadRun::setRoute(
			Depot& depot,
			pt::StopPoint& stop,
			double length,
			bool fromDepotToStop
		){
			// Cleaning existing data
			if(!_edges.empty())
			{
				setUndefined();
			}

			// Generation of edges
			Edge* startEdge(
				fromDepotToStop ?
				new DeadRunEdge(*this, depot) :
				new DeadRunEdge(*this, stop)
			);
			addEdge(*startEdge);

			Edge* endEdge(
				fromDepotToStop ?
				new DeadRunEdge(length, *this, stop) :
				new DeadRunEdge(length, *this, depot)
			);
			addEdge(*endEdge);
		}



		void DeadRun::setUndefined()
		{
			// Edges
			for (Path::Edges::iterator it(_edges.begin()); it != _edges.end(); ++it)
			{
				delete *it;
			}
			_edges.clear();
		}



		bool DeadRun::isUndefined() const
		{
			return _edges.size() != 2;
		}



		bool DeadRun::getFromDepotToStop() const
		{
			if(isUndefined())
			{
				return true;
			}

			return dynamic_cast<Depot*>(_edges[0]->getFromVertex()) != NULL;
		}



		bool DeadRun::isContinuous() const
		{
			return false;
		}



		boost::posix_time::time_duration DeadRun::getDepartureBeginScheduleToIndex(bool RTData, size_t rankInPath) const
		{
			return getDepartureSchedules(true, RTData)[rankInPath];
		}



		boost::posix_time::time_duration DeadRun::getDepartureEndScheduleToIndex(bool RTData, size_t rankInPath) const
		{
			return getDepartureSchedules(true, RTData)[rankInPath];
		}



		boost::posix_time::time_duration DeadRun::getArrivalBeginScheduleToIndex(bool RTData, size_t rankInPath) const
		{
			return getArrivalSchedules(true, RTData)[rankInPath];
		}



		boost::posix_time::time_duration DeadRun::getArrivalEndScheduleToIndex(bool RTData, size_t rankInPath) const
		{
			return getArrivalSchedules(true, RTData)[rankInPath];
		}



		graph::ServicePointer DeadRun::getFromPresenceTime(const AccessParameters&, bool, bool,bool, const synthese::graph::Edge &,const boost::posix_time::ptime &,bool,bool,bool,bool,graph::UseRule::ReservationDelayType) const
		{
			return ServicePointer();
		}



		void DeadRun::completeServicePointer(synthese::graph::ServicePointer &,const synthese::graph::Edge &,const synthese::graph::AccessParameters &) const
		{
		}


		bool DeadRun::isPedestrianMode(void) const
		{
			return false;
		}



		bool DeadRun::isActive(const boost::gregorian::date& date) const
		{
			return Calendar::isActive(date);
		}



		std::string DeadRun::getRuleUserName() const
		{
			return "Haut-le-pied" + getServiceNumber();
		}



		Depot* DeadRun::getDepot() const
		{
			if(isUndefined())
			{
				return NULL;
			}

			return static_cast<Depot*>(getEdge(getFromDepotToStop() ? 0 : 1)->getFromVertex());
		}



		pt::StopPoint* DeadRun::getStop() const
		{
			if(isUndefined())
			{
				return NULL;
			}

			return static_cast<StopPoint*>(getEdge(getFromDepotToStop() ? 1 : 0)->getFromVertex());
		}
}	}
