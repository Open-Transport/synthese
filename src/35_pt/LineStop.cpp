
/** LineStop class implementation.
	@file LineStop.cpp

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

#include "LineStop.h"
#include "Registry.h"

#include "Line.h"
#include "Service.h"
#include "ContinuousService.h"
#include "PhysicalStop.h"
#include "StopArea.hpp"

using namespace std;

namespace synthese
{
	using namespace util;
	using namespace geometry;
	using namespace graph;
	using namespace pt;

	namespace util
	{
		template<> const string Registry<pt::LineStop>::KEY("LineStop");
	}

	namespace pt
	{
		LineStop::LineStop(
			RegistryKeyType id,
			Line* line,
			size_t rankInPath,
			bool isDeparture,
			bool isArrival,		
			double metricOffset,
			PhysicalStop* physicalStop
		):	Registrable(id),
			_isDeparture(isDeparture),
			_isArrival(isArrival),
			Edge(line, rankInPath, physicalStop, metricOffset),
			_scheduleInput(true)
		{
			if (physicalStop)
				setPhysicalStop(physicalStop);
		}



		LineStop::~LineStop()
		{
		}



		bool 
		LineStop::seemsGeographicallyConsistent (const LineStop& other) const
		{
			double deltaMO; // meters
			if ( getMetricOffset () > other.getMetricOffset () )
				deltaMO = ( getMetricOffset () - other.getMetricOffset () ) / 1000;
			else
				deltaMO = ( other.getMetricOffset () - getMetricOffset () ) / 1000;

			const int deltaGPS = getFromVertex()->getDistanceTo(*other.getFromVertex()); // kilometers

			if ( deltaMO > 10 * deltaGPS && deltaMO - deltaGPS > 1 )
			{
				return false;
			}
			if ( deltaMO < deltaGPS && deltaGPS - deltaMO > 1 )
			{
				return false;
			}

			return true;
		    
		}



		Line* LineStop::getLine() const
		{
			return static_cast<Line*>(getParentPath());
		}

		PhysicalStop* LineStop::getPhysicalStop() const
		{
			return static_cast<PhysicalStop*>(getFromVertex());
		}

		void LineStop::setLine(Line* line )
		{
			setParentPath(static_cast<Path*>(line));
		}

		void LineStop::setPhysicalStop(PhysicalStop* stop )
		{
			// Saving of the attribute
			setFromVertex(static_cast<Vertex*>(stop));

			// Links from stop to the linestop
			if(stop)
			{
				if(getIsArrival())
					stop->addArrivalEdge((Edge*) this);
				if(getIsDeparture())
					stop->addDepartureEdge((Edge*) this);
			}
		}
		
		
		
		bool LineStop::isDepartureAllowed() const
		{
			return _isDeparture;
		}
		
		
		
		bool LineStop::isArrivalAllowed() const
		{
			return _isArrival;
		}
}	}
