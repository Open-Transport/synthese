
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
#include "JourneyPattern.hpp"
#include "Vertex.h"

using namespace std;

namespace synthese
{
	using namespace util;
	using namespace graph;
	using namespace pt;

	namespace pt
	{
		LineStop::LineStop(
			RegistryKeyType id,
			JourneyPattern* line,
			size_t rankInPath,
			bool isDeparture,
			bool isArrival,
			double metricOffset,
			Vertex* vertex
		):	Registrable(id),
			_isDeparture(isDeparture),
			_isArrival(isArrival),
			Edge(line, rankInPath, vertex, metricOffset)
		{}



		LineStop::~LineStop()
		{
		}



		bool LineStop::seemsGeographicallyConsistent (const LineStop& other) const
		{
			double deltaMO; // meters
			if ( getMetricOffset () > other.getMetricOffset () )
				deltaMO = ( getMetricOffset () - other.getMetricOffset () );
			else
				deltaMO = ( other.getMetricOffset () - getMetricOffset () );

			const double deltaGPS(
				getFromVertex()->getGeometry()->distance(other.getFromVertex()->getGeometry().get())
			);

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



		JourneyPattern* LineStop::getLine() const
		{
			return static_cast<JourneyPattern*>(getParentPath());
		}


		void LineStop::setLine(JourneyPattern* line )
		{
			setParentPath(static_cast<Path*>(line));
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
