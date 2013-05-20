
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
#include "StopArea.hpp"

#include <boost/foreach.hpp>

using namespace std;
using namespace boost;


namespace synthese
{
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



		void LineArea::_addAllStops(
			bool isForArrival
		){
			BOOST_FOREACH(StopArea* stopArea, getArea()->getStops())
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
				}
			}
		}


		void LineArea::setArea( DRTArea& value )
		{
			setFromVertex(static_cast<Vertex*>(&value));

			// Add links and generated line stops here
			if(isArrivalAllowed() && !_internalService)
			{
				_addAllStops(true);
			}
			if(isDepartureAllowed())
			{
				_addAllStops(false);
			}
			if(isArrivalAllowed() && _internalService)
			{
				_addAllStops(true);
			}
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
				generatedLineStop->clearPhysicalStop();
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
}	}
