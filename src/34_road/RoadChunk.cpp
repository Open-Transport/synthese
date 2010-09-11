
/** RoadChunk class implementation.
	@file RoadChunk.cpp

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

#include "RoadChunk.h"
#include "Road.h"
#include "Crossing.h"
#include "Registry.h"

#include <geos/geom/LineString.h>
#include <geos/linearref/LengthIndexedLine.h>

using namespace std;
using namespace boost;
using namespace geos::geom;
using namespace geos::linearref;

namespace synthese
{
	using namespace graph;
	using namespace geography;
	
	namespace util
	{
		template<> const string Registry<road::RoadChunk>::KEY("RoadChunk");
	}

	namespace road
	{
		RoadChunk::RoadChunk(
			util::RegistryKeyType id,
			Crossing* fromCrossing,
			int rankInRoad,
			Road* street,
			double metricOffset
		):	util::Registrable(id),
			Edge(
				street,
				rankInRoad,
				fromCrossing,
				metricOffset
			)
		{
			if(fromCrossing)
			{
				setFromCrossing(fromCrossing);
			}
		}

		
		
		RoadChunk::~RoadChunk ()
		{
		}



		void RoadChunk::setRoad(Road* road)
		{
			_parentPath = static_cast<Path*>(road);
		}
		
		

		bool RoadChunk::isDepartureAllowed() const
		{
			return true;
		}
		
		
		
		bool RoadChunk::isArrivalAllowed() const
		{
			return true;
		}
		


		Crossing* RoadChunk::getFromCrossing() const
		{
			return static_cast<Crossing*>(_fromVertex);
		}



		void RoadChunk::setFromCrossing(Crossing* fromAddress )
		{
			_fromVertex = static_cast<Vertex*>(fromAddress);

			// Links from stop to the linestop
			fromAddress->addArrivalEdge(static_cast<Edge*>(this));
			fromAddress->addDepartureEdge(static_cast<Edge*>(this));

			markServiceIndexUpdateNeeded(false);
		}



		Road* RoadChunk::getRoad() const
		{
			return static_cast<Road*>(_parentPath);
		}



		geography::GeoPoint RoadChunk::getGeoPoint( double metricOffset ) const
		{
			shared_ptr<LineString> geometry(getGeometry());
			if(!geometry.get())
			{
				return *getFromVertex();
			}
			if(metricOffset > geometry->getLength())
			{
				if(getFollowingArrivalForFineSteppingOnly())
				{
					return static_cast<RoadChunk*>(getFollowingArrivalForFineSteppingOnly())->getGeoPoint(metricOffset - geometry->getLength());
				}
				else
				{
					return *getFromVertex();
				}
			}
			return GeoPoint(LengthIndexedLine(geometry.get()).extractPoint(metricOffset));
		}



		RoadChunk* RoadChunk::getReverseChunk() const
		{
			if(!getRoad() || !getRoad()->getReverseRoad())
			{
				return NULL;
			}
			return static_cast<RoadChunk*>(getRoad()->getReverseRoad()->getEdges().operator[](-getRankInPath()));
		}



		bool RoadChunk::testIfHouseNumberBelongsToChunk( HouseNumber houseNumber ) const
		{
			if(!_houseNumberBounds)
			{
				return false;
			}

			if(houseNumber < _houseNumberBounds->first && houseNumber > _houseNumberBounds->second)
			{
				return false;
			}

			switch(_houseNumberingPolicy)
			{
			case ALL: return true;
			case ODD: return houseNumber % 2;
			case EVEN: return !(houseNumber % 2); 
			}

			// Never execute
			return false;
		}



		RoadChunk::MetricOffset RoadChunk::getHouseNumberMetricOffset( HouseNumber houseNumber ) const
		{
			return getMetricOffset() +
				((houseNumber - _houseNumberBounds->first) / (_houseNumberBounds->second - _houseNumberBounds->first)) *
				(getEndMetricOffset() - getMetricOffset())
				;
		}
	}
}
