
/** Junction class implementation.
	@file Junction.cpp

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

#include "Junction.hpp"

#include "JunctionStop.hpp"
#include "JunctionTableSync.hpp"
#include "PermanentService.h"
#include "StopPointTableSync.hpp"
#include "StopArea.hpp"
#include "AllowedUseRule.h"

using namespace boost;
using namespace boost::posix_time;
using namespace std;

namespace synthese
{
	using namespace db;
	using namespace util;
	using namespace graph;
	using namespace pt;

	namespace util
	{
		template<> const string Registry<pt::Junction>::KEY("Junction");
	}

	namespace pt
	{
		Junction::Junction(
			util::RegistryKeyType id
		):	Registrable(id)
		{
			// Default use rules
			RuleUser::Rules rules(getRules());
			rules[USER_PEDESTRIAN - USER_CLASS_CODE_OFFSET] = AllowedUseRule::INSTANCE.get();
			rules[USER_BIKE - USER_CLASS_CODE_OFFSET] = AllowedUseRule::INSTANCE.get();
			rules[USER_HANDICAPPED - USER_CLASS_CODE_OFFSET] = AllowedUseRule::INSTANCE.get();
			setRules(rules);
		}



		Junction::~Junction()
		{
			_clean();
		}



		void Junction::setStops(
			StopPoint* start,
			StopPoint* end,
			double length,
			boost::posix_time::time_duration duration,
			bool doBack
		){
			// Cleaning existing data
			if(!_edges.empty())
			{
				_clean();
			}
			_back.reset();

			// Generation of edges
			Edge* startEdge(new JunctionStop(
					this,
					start
			)	);
			addEdge(*startEdge);
			start->addDepartureEdge(startEdge);

			Edge* endEdge(new JunctionStop(
					length,
					this,
					end
			)	);
			addEdge(*endEdge);
			end->addArrivalEdge(endEdge);

			// Generation of service
			Service* service(new PermanentService(
					0,
					this,
					duration
			)	);
			addService(*service, false);
			service->setPath(this);


			// Generation of back
			if(doBack)
			{
				_back.reset(new Junction);
				_back->setStops(end, start, length, duration, false);
			}
		}



		boost::shared_ptr<Junction> Junction::getBack() const
		{
			return _back;
		}



		StopPoint* Junction::getStart() const
		{
			assert(isValid());
			return dynamic_cast<StopPoint*>((*_edges.begin())->getFromVertex());
		}



		StopPoint* Junction::getEnd() const
		{
			assert(isValid());
			return dynamic_cast< StopPoint*>((*(_edges.begin()+1))->getFromVertex());
		}



		bool Junction::isValid() const
		{
			return _edges.size() == 2 && _services.size() == 1 && static_cast<PermanentService*>(*_services.begin())->getDuration();
		}



		double Junction::getLength() const
		{
			assert(isValid());
			return (*(_edges.begin()+1))->getMetricOffset();
		}



		boost::posix_time::time_duration Junction::getDuration() const
		{
			assert(isValid());
			return *static_cast<PermanentService*>(*_services.begin())->getDuration();
		}



		bool Junction::isPedestrianMode() const
		{
			return true;
		}



		bool Junction::isRoad() const
		{
			return true;
		}



		std::string Junction::getRuleUserName() const
		{
			return "Jonction " + getStart()->getConnectionPlace()->getFullName() + " -> " + getEnd()->getConnectionPlace()->getFullName();
		}



		bool Junction::isActive( const boost::gregorian::date& date ) const
		{
			return true;
		}



		void Junction::_clean()
		{
			// Edges
			for (Path::Edges::iterator it(_edges.begin()); it != _edges.end(); ++it)
			{
				Vertex* stop((*it)->getFromVertex());

				// Removing edge from stop point
				if(it == _edges.begin())
				{
					stop->removeDepartureEdge(*it);
				}
				else
				{
					stop->removeArrivalEdge(*it);
				}

				delete *it;
			}

			// Services
			for (ServiceSet::iterator it(_services.begin()); it != _services.end(); ++it)
			{
				delete *it;
			}
		}


		
		void Junction::toParametersMap( util::ParametersMap& pm, bool withAdditionalParameters, boost::logic::tribool withFiles /*= boost::logic::indeterminate*/, std::string prefix /*= std::string() */ ) const
		{
			pm.insert(TABLE_COL_ID, getKey());
			pm.insert(
				JunctionTableSync::COL_START_PHYSICAL_STOP_ID,
				isValid() ? getStart()->getKey() : RegistryKeyType(0)
			);
			pm.insert(
				JunctionTableSync::COL_END_PHYSICAL_STOP_ID,
				isValid() ? getEnd()->getKey() : RegistryKeyType(0)
			);
			pm.insert(
				JunctionTableSync::COL_LENGTH,
				isValid() ? getLength() : double(0)
			);
			pm.insert(
				JunctionTableSync::COL_DURATION,
				isValid() ? getDuration().total_seconds() / 60 : 0
			);
			pm.insert(
				JunctionTableSync::COL_BIDIRECTIONAL,
				getBack() != NULL
			);
		}



		bool Junction::loadFromRecord( const Record& record, util::Env& env )
		{
			bool result(false);

			RegistryKeyType sid(isValid() ? getStart()->getKey() : RegistryKeyType(0));
			if(record.isDefined(JunctionTableSync::COL_START_PHYSICAL_STOP_ID))
			{
				RegistryKeyType value(record.getDefault<RegistryKeyType>(JunctionTableSync::COL_START_PHYSICAL_STOP_ID, 0));
				if(sid != value)
				{
					result = true;
					sid = value;
				}
			}

			RegistryKeyType eid(isValid() ? getEnd()->getKey() : RegistryKeyType(0));
			if(record.isDefined(JunctionTableSync::COL_END_PHYSICAL_STOP_ID))
			{
				RegistryKeyType value(record.getDefault<RegistryKeyType>(JunctionTableSync::COL_END_PHYSICAL_STOP_ID, 0));
				if(eid != value)
				{
					eid = value;
					result = true;
				}
			}

			double length(isValid() ? getLength() : double(0));
			if(record.isDefined(JunctionTableSync::COL_LENGTH))
			{
				double value(record.getDefault<double>(JunctionTableSync::COL_LENGTH, 0.0));
				if(value != length)
				{
					length = value;
					result = true;
				}
			}

			time_duration duration(minutes(isValid() ? getDuration().total_seconds() / 60 : 0));
			if(record.isDefined(JunctionTableSync::COL_DURATION))
			{
				time_duration value(minutes(record.getDefault<long>(JunctionTableSync::COL_DURATION, 0)));
				if(value != duration)
				{
					duration = value;
					result = true;
				}
			}

			bool bidir(getBack() != NULL);
			if(record.isDefined(JunctionTableSync::COL_BIDIRECTIONAL))
			{
				bool value(record.getDefault<bool>(JunctionTableSync::COL_BIDIRECTIONAL, true));
				if(value != bidir)
				{
					bidir = value;
					result = true;
				}
			}

			if(result && sid > 0 && eid > 0)
			{
				try
				{
					setStops(
						StopPointTableSync::GetEditable(sid, env).get(),
						StopPointTableSync::GetEditable(eid, env).get(),
						length,
						duration,
						bidir
					);
				}
				catch(ObjectNotFoundException<StopPoint>&)
				{
					Log::GetInstance().warn("No such stop in Junction "+ lexical_cast<string>(getKey()));
				}
			}

			return result;
		}


		 
		synthese::SubObjects Junction::getSubObjects() const
		{
			SubObjects result;		
			return result;
		}



		synthese::LinkedObjectsIds Junction::getLinkedObjectsIds( const Record& record ) const
		{
			LinkedObjectsIds result;
			RegistryKeyType sid(record.getDefault<RegistryKeyType>(JunctionTableSync::COL_START_PHYSICAL_STOP_ID, 0));
			if(sid)
			{
				result.push_back(sid);
			}
			RegistryKeyType eid(record.getDefault<RegistryKeyType>(JunctionTableSync::COL_END_PHYSICAL_STOP_ID, 0));
			if(eid)
			{
				result.push_back(eid);
			}
			return result;
		}



		void Junction::link( util::Env& env, bool withAlgorithmOptimizations /*= false*/ )
		{

		}

}	}
