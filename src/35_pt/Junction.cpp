
/** Junction class implementation.
	@file Junction.cpp

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

#include "Junction.hpp"
#include "JunctionStop.hpp"
#include "PermanentService.h"
#include "PhysicalStop.h"

using namespace boost;
using namespace boost::posix_time;
using namespace std;

namespace synthese
{
	using namespace util;
	using namespace graph;
	using namespace pt;

	namespace util
	{
		template<> const string Registry<pt::Junction>::KEY("Junction");
	}

	namespace pt
	{
		Junction::Junction( util::RegistryKeyType id /*= UNKNOWN_VALUE */ ):
			Registrable(id)
		{

		}


		Junction::~Junction()
		{
			for (Path::Edges::iterator it(_edges.begin()); it != _edges.end(); ++it)
			{
				delete *it;
			}
			for (ServiceSet::iterator it(_services.begin()); it != _services.end(); ++it)
			{
				delete *it;
			}
		}



		void Junction::setStops(
			PhysicalStop* start,
			PhysicalStop* end,
			double length,
			boost::posix_time::time_duration duration,
			bool doBack
		){
			// Cleaning existing data
			if(!_edges.empty())
			{
				for (Path::Edges::iterator it(_edges.begin()); it != _edges.end(); ++it)
				{
					delete *it;
				}
				for (ServiceSet::iterator it(_services.begin()); it != _services.end(); ++it)
				{
					delete *it;
				}
			}

			// Generation of edges
			Edge* startEdge(new JunctionStop(
					this,
					start
			)	);
			addEdge(startEdge);

			Edge* endEdge(new JunctionStop(
					length,
					this,
					end
			)	);
			addEdge(endEdge);

			// Generation of service
			Service* service(new PermanentService(
				UNKNOWN_VALUE,
				this,
				duration
			)	);
			addService(service, false);


			// Generation of back
			if(_back)
			{
				_back.reset(new Junction);
				_back->setStops(end, start, length, duration, false);
			}
		}



		boost::shared_ptr<Junction> Junction::getBack() const
		{
			return _back;
		}



		PhysicalStop* Junction::getStart() const
		{
			assert(isValid());
			return dynamic_cast<PhysicalStop*>((*_edges.begin())->getFromVertex());
		}



		PhysicalStop* Junction::getEnd() const
		{
			assert(isValid());
			return dynamic_cast< PhysicalStop*>((*(_edges.begin()+1))->getFromVertex());
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
	}
}
