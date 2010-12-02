
/** Road place class implementation.
	@file RoadPlace.cpp

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

#include <boost/foreach.hpp>

#include "RoadPlace.h"
#include "MainRoadPart.hpp"
#include "RoadModule.h"
#include "RoadChunk.h"
#include "Registry.h"
#include "VertexAccessMap.h"
#include "AllowedUseRule.h"
#include "GraphConstants.h"
#include "Vertex.h"
#include "House.hpp"

#include <geos/geom/Envelope.h>

using namespace std;
using namespace boost;
using namespace geos::geom;

namespace synthese
{
	using namespace util;
	using namespace graph;
	using namespace geography;

	namespace util
	{
		template<> const string Registry<road::RoadPlace>::KEY("RoadPlace");
		template<> const string FactorableTemplate<NamedPlace,road::RoadPlace>::FACTORY_KEY("RoadPlace");
	}

	namespace road
	{
		RoadPlace::RoadPlace(
			RegistryKeyType key
		):	Registrable(key),
			NamedPlaceTemplate<RoadPlace>()
		{
			RuleUser::Rules rules(RuleUser::GetEmptyRules());
			rules[USER_PEDESTRIAN - USER_CLASS_CODE_OFFSET] = AllowedUseRule::INSTANCE.get();
			rules[USER_HANDICAPPED - USER_CLASS_CODE_OFFSET] = AllowedUseRule::INSTANCE.get();
			rules[USER_BIKE - USER_CLASS_CODE_OFFSET] = AllowedUseRule::INSTANCE.get();
			setRules(rules);
		}
		
		
		
		void RoadPlace::addRoad(
			Road& road
		){
			addPath(static_cast<Path*>(&road));
			_isoBarycentre.reset();
		}
		
		
		
		void RoadPlace::removeRoad(
			Road& road
		){
			removePath(static_cast<Path*>(&road));
			_isoBarycentre.reset();
		}



		void RoadPlace::getVertexAccessMap(
			graph::VertexAccessMap& result,
			const graph::AccessParameters& accessParameters,
			const GraphTypes& whatToSearch
		) const	{
			if(whatToSearch.find(RoadModule::GRAPH_ID) == whatToSearch.end()) return;

			BOOST_FOREACH(const Path* road, _paths)
			{
				BOOST_FOREACH(const Edge* edge, road->getEdges())
				{
					result.insert(
						edge->getFromVertex(),
						VertexAccess()
					);
				}
			}
		}



		boost::shared_ptr<geos::geom::Point> RoadPlace::getPoint() const
		{
			if (!_isoBarycentre.get())
			{
				Envelope e;
				BOOST_FOREACH(const Path* road, _paths)
				{
					BOOST_FOREACH(const Edge* edge, road->getEdges())
					{
						if(edge->getFromVertex()->hasGeometry())
						{
							e.expandToInclude(*edge->getFromVertex()->getGeometry()->getCoordinate());
						}
					}
				}
				if(!e.isNull())
				{
					Coordinate c;
					e.centre(c);
					_isoBarycentre.reset(CoordinatesSystem::GetDefaultGeometryFactory().createPoint(c));
				}
			}
			return _isoBarycentre;

		}



		std::string RoadPlace::getNameForAllPlacesMatcher(
			std::string text
		) const	{
			return (text.empty() ? getName() : text);
		}



		std::string RoadPlace::getRuleUserName() const
		{
			return "Route "+ getFullName();
		}



		boost::shared_ptr<House> RoadPlace::getHouse(
			MainRoadChunk::HouseNumber houseNumber
		) const	{
			MainRoadChunk* nearestChunk(NULL);
			MainRoadChunk::HouseNumber difference(0);
			MainRoadChunk::HouseNumber bestNumber(0);
			BOOST_FOREACH(Path* path, getPaths())
			{
				MainRoadPart* roadPart(dynamic_cast<MainRoadPart*>(path));
				if(!roadPart)
				{
					continue;
				}

				BOOST_FOREACH(Edge* edge, path->getEdges())
				{
					MainRoadChunk& chunk(static_cast<MainRoadChunk&>(*edge));
					if(!chunk.getLeftHouseNumberBounds() && !chunk.getRightHouseNumberBounds())
					{
						continue;
					}
					if(chunk.testIfHouseNumberBelongsToChunk(houseNumber))
					{
						return shared_ptr<House>(
							new House(
								chunk,
								houseNumber
						)	);
					}
/* todo
					if(houseNumber > chunk.getLeftHouseNumberBounds()->second)
					{
						if(!nearestChunk || houseNumber - chunk.getHouseNumberBounds()->second < difference)
						{
							bestNumber = chunk.getHouseNumberBounds()->second;
							difference = houseNumber - bestNumber;
							nearestChunk = static_cast<RoadChunk*>(chunk.getNextEdge());
						}
					}
					else
					{
						if(!nearestChunk || chunk.getHouseNumberBounds()->first - houseNumber < difference)
						{
							bestNumber = chunk.getHouseNumberBounds()->first;
							difference = bestNumber - houseNumber;
							nearestChunk = &chunk;
						}
					}
*/				}
			}

			// Nearest existing number
			if(nearestChunk)
			{
				return boost::shared_ptr<House>(
					new House(
						*nearestChunk,
						bestNumber
				)	);
			}


			// No return
			return shared_ptr<House>();
		}
	}
}
