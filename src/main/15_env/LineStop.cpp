
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

#include "SquareDistance.h"
#include "Line.h"
#include "Service.h"
#include "ContinuousService.h"
#include "PhysicalStop.h"



namespace synthese
{
	namespace env
	{

		LineStop::LineStop (const uid id,
					const Line* line,
					int rankInPath,
					bool isDeparture,
					bool isArrival,		
					double metricOffset)
			: synthese::util::Registrable<uid,LineStop> (id)
			, Edge (isDeparture, isArrival, line, rankInPath)
			, _metricOffset (metricOffset)
		{
		}



		LineStop::~LineStop()
		{
		}



		const Vertex* 
		LineStop::getFromVertex () const
		{
			return _physicalStop;
		}





		double
		LineStop::getMetricOffset () const
		{
			return _metricOffset;
		}


		bool 
		LineStop::seemsGeographicallyConsistent (const LineStop& other) const
		{
			double deltaMO; // meters
			if ( getMetricOffset () > other.getMetricOffset () )
				deltaMO = ( getMetricOffset () - other.getMetricOffset () ) / 1000;
			else
				deltaMO = ( other.getMetricOffset () - getMetricOffset () ) / 1000;

			int deltaGPS = SquareDistance ( 
			*getFromVertex (), 
			*other.getFromVertex () ).getDistance(); // kilometers

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


		void 
		LineStop::setMetricOffset (double metricOffset)
		{
			_metricOffset = metricOffset;
		}

		Line* LineStop::getLine() const
		{
			return (Line*) getParentPath();
		}

		const PhysicalStop* LineStop::getPhysicalStop() const
		{
			return (const PhysicalStop*) _physicalStop;
		}

		void LineStop::setLine( const Line* line )
		{
			setParentPath((Path*) line);
		}

		void LineStop::setPhysicalStop(PhysicalStop* stop )
		{
			// Saving of the attribute
			_physicalStop = stop;

			// Links from stop to the linestop
			if (isArrival())
				stop->addArrivalEdge((Edge*) this);
			if (isDeparture())
				stop->addDepartureEdge((Edge*) this);
		}
	}
}
