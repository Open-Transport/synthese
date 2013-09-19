
/** LineArea class implementation.
	@file LineArea.cpp

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

#include "LineArea.hpp"

#include "AreaGeneratedLineStop.hpp"
#include "DRTArea.hpp"
#include "JourneyPattern.hpp"
#include "LineStopTableSync.h"
#include "StopArea.hpp"
#include "StopPoint.hpp"

#include <boost/foreach.hpp>
#include <geos/geom/LineString.h>

using namespace std;
using namespace boost;


namespace synthese
{
	using namespace db;
	using namespace graph;
	using namespace util;

	namespace pt
	{
		LineArea::LineArea(
			RegistryKeyType id,
			JourneyPattern* journeyPattern,
			std::size_t rankInPath,
			bool isDeparture,
			bool isArrival,
			double metricOffset,
			DRTArea* area,
			bool internalService
		):	Registrable(id),
			LineStop(id, journeyPattern, rankInPath, isDeparture, isArrival, metricOffset, area),
			_internalService(internalService)
		{
			if(area)
			{
				setArea(*area);
			}
		}



		void LineArea::addAllStops(
			bool isForArrival
		){
			BOOST_FOREACH(StopArea* stopArea, getArea()->get<Stops>())
			{
				BOOST_FOREACH(const StopArea::PhysicalStops::value_type& stopPoint, stopArea->getPhysicalStops())
				{
					boost::shared_ptr<AreaGeneratedLineStop> generatedLineStop(
						new AreaGeneratedLineStop(
							getLine(),
							getRankInPath(),
							!isForArrival,
							isForArrival,
							getMetricOffset(),
							const_cast<StopPoint*>(stopPoint.second),
							this
					)	);
					_generatedLineStops.push_back(generatedLineStop);

					
					// Links from stop to the linestop
					if(isForArrival)
					{
						const_cast<StopPoint*>(stopPoint.second)->addArrivalEdge(generatedLineStop.get());
					}
					else
					{
						const_cast<StopPoint*>(stopPoint.second)->addDepartureEdge(generatedLineStop.get());
					}
				}
			}
		}


		void LineArea::setArea( DRTArea& value )
		{
			setFromVertex(static_cast<Vertex*>(&value));
		}



		void LineArea::clearArea()
		{
			DRTArea* area(getArea());
			if(area == NULL)
			{
				return;
			}

			BOOST_FOREACH(GeneratedLineStops::value_type& generatedLineStop, _generatedLineStops)
			{
				generatedLineStop->clearPhysicalStopLinks();
			}
			_generatedLineStops.clear();

			setFromVertex(NULL);
		}


		DRTArea* LineArea::getArea() const
		{
			return static_cast<DRTArea*>(getFromVertex());
		}



		Edge::SubEdges LineArea::getSubEdges() const
		{
			SubEdges result;
			BOOST_FOREACH(const boost::shared_ptr<AreaGeneratedLineStop>& generatedLineStop, _generatedLineStops)
			{
				result.push_back(generatedLineStop.get());
			}
			return result;
		}

		void LineArea::toParametersMap( util::ParametersMap& pm, bool withAdditionalParameters, boost::logic::tribool withFiles /*= boost::logic::indeterminate*/, std::string prefix /*= std::string() */ ) const
		{
			if(!getArea()) throw Exception("LineArea save error. Missing physical stop");
			if(!getLine()) throw Exception("LineArea Save error. Missing line");

			pm.insert(prefix + TABLE_COL_ID, getKey());
			pm.insert(
				prefix + LineStopTableSync::COL_PHYSICALSTOPID,
				getArea()->getKey()
			);
			pm.insert(
				prefix + LineStopTableSync::COL_LINEID,
				getLine()->getKey()
			);
			pm.insert(
				prefix + LineStopTableSync::COL_RANKINPATH,
				getRankInPath()
			);
			pm.insert(
				prefix + LineStopTableSync::COL_ISDEPARTURE,
				isDepartureAllowed()
			);
			pm.insert(
				prefix + LineStopTableSync::COL_ISARRIVAL,
				isArrivalAllowed()
			);
			pm.insert(
				prefix + LineStopTableSync::COL_METRICOFFSET,
				getMetricOffset()
			);
			pm.insert(
				prefix + LineStopTableSync::COL_SCHEDULEINPUT,
				true
			);
			pm.insert(
				prefix + LineStopTableSync::COL_INTERNAL_SERVICE,
				getInternalService()
			);
			pm.insert(
				prefix + TABLE_COL_GEOMETRY,
				static_pointer_cast<geos::geom::Geometry, geos::geom::LineString>(getGeometry())
			);
			pm.insert(
				prefix + LineStopTableSync::COL_RESERVATION_NEEDED,
				true
			);
		}
}	}
