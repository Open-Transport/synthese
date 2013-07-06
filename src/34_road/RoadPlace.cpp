
/** Road place class implementation.
	@file RoadPlace.cpp

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

#include <boost/foreach.hpp>

#include "RoadPlace.h"
#include "MainRoadPart.hpp"
#include "RoadModule.h"
#include "RoadChunk.h"
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
		const string RoadPlace::DATA_ID = "id";
		const string RoadPlace::DATA_NAME = "name";
		const string RoadPlace::DATA_X = "x";
		const string RoadPlace::DATA_Y = "y";



		RoadPlace::RoadPlace(
			RegistryKeyType key
		):	Registrable(key),
			NamedPlaceTemplate<RoadPlace>()
		{
			RuleUser::Rules rules(RuleUser::GetEmptyRules());
			rules[USER_PEDESTRIAN - USER_CLASS_CODE_OFFSET] = AllowedUseRule::INSTANCE.get();
			rules[USER_HANDICAPPED - USER_CLASS_CODE_OFFSET] = AllowedUseRule::INSTANCE.get();
			rules[USER_BIKE - USER_CLASS_CODE_OFFSET] = AllowedUseRule::INSTANCE.get();
			rules[USER_CAR - USER_CLASS_CODE_OFFSET] = AllowedUseRule::INSTANCE.get();
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



		const boost::shared_ptr<geos::geom::Point>& RoadPlace::getPoint() const
		{		
			if (!_isoBarycentre.get())
			{
				int nbPoint = 0;
				BOOST_FOREACH(const Path* road, _paths)
				{
					BOOST_FOREACH(const Edge* edge, road->getEdges())
					{
						if(edge->getFromVertex()->hasGeometry())
						{
							nbPoint++;
						}
					}
				}
				nbPoint = (nbPoint + 1 ) / 2; // We stop at the point in the middle
				int nbCurPoint = 0;
				BOOST_FOREACH(const Path* road, _paths)
				{
					BOOST_FOREACH(const Edge* edge, road->getEdges())
					{
						if(edge->getFromVertex()->hasGeometry())
						{
							nbCurPoint++;
							if(nbCurPoint == nbPoint)
							{
								_isoBarycentre = edge->getFromVertex()->getGeometry();
								break;
							}
						}
					}
					if(nbCurPoint == nbPoint)break;
				}
				if(!_isoBarycentre.get())
				{
					Coordinate c;
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
			MainRoadChunk::HouseNumber houseNumber,
			bool numberAtBeginning
		) const	{
			MainRoadChunk* nearestChunk(NULL);
			MainRoadChunk::HouseNumber difference(MainRoadChunk::HouseNumber_MAX);
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
						return boost::shared_ptr<House>(
							new House(
								chunk,
								houseNumber,
								numberAtBeginning
						)	);
					}
					else
					{
						MainRoadChunk::HouseNumber currentDifference(MainRoadChunk::HouseNumber_MAX);
						MainRoadChunk::HouseNumber currentBestNumber(0);

						bool closestIsFirst = true;

						if(chunk.getLeftHouseNumberBounds())
						{
							if( (  (   chunk.getLeftHouseNumberingPolicy() == MainRoadChunk::ODD)  && (houseNumber % 2)    )
									||
								(  (   chunk.getLeftHouseNumberingPolicy() == MainRoadChunk::EVEN) && (!(houseNumber % 2)) )
									||
								(  chunk.getLeftHouseNumberingPolicy() == MainRoadChunk::ALL )
							  )
							{
								//Left distance calculation
								if( chunk.getLeftHouseNumberBounds()->first < houseNumber )
								{
									currentDifference = houseNumber - chunk.getLeftHouseNumberBounds()->first;
									currentBestNumber = chunk.getLeftHouseNumberBounds()->first;
								}
								else
								{
									currentDifference = chunk.getLeftHouseNumberBounds()->first - houseNumber;
									currentBestNumber = chunk.getLeftHouseNumberBounds()->first;
								}

								if( chunk.getLeftHouseNumberBounds()->second < houseNumber )
								{
									if(currentDifference > houseNumber - chunk.getLeftHouseNumberBounds()->second)
									{
										currentDifference = houseNumber - chunk.getLeftHouseNumberBounds()->second;
										currentBestNumber = chunk.getLeftHouseNumberBounds()->second;
										closestIsFirst = false;
									}
								}
								else
								{
									if(currentDifference > chunk.getLeftHouseNumberBounds()->second - houseNumber)
									{
										currentDifference = chunk.getLeftHouseNumberBounds()->second - houseNumber;
										currentBestNumber = chunk.getLeftHouseNumberBounds()->second;
										closestIsFirst = false;
									}
								}
							}
						}

						if(chunk.getRightHouseNumberBounds())
						{
							if( (  (   chunk.getRightHouseNumberingPolicy() == MainRoadChunk::ODD)  && (houseNumber % 2)    )
									||
								(  (   chunk.getRightHouseNumberingPolicy() == MainRoadChunk::EVEN) && (!(houseNumber % 2)) )
									||
								(  chunk.getRightHouseNumberingPolicy() == MainRoadChunk::ALL )
							  )
							{
								//Right distance calculation
								if( chunk.getRightHouseNumberBounds()->first < houseNumber )
								{
									if(currentDifference > houseNumber - chunk.getRightHouseNumberBounds()->first)
									{
										currentDifference = houseNumber - chunk.getRightHouseNumberBounds()->first;
										currentBestNumber = chunk.getRightHouseNumberBounds()->first;
										closestIsFirst = true;
									}
								}
								else
								{
									if(currentDifference > chunk.getRightHouseNumberBounds()->first - houseNumber)
									{
										currentDifference = chunk.getRightHouseNumberBounds()->first - houseNumber;
										currentBestNumber = chunk.getRightHouseNumberBounds()->first;
										closestIsFirst = true;
									}
								}

								if( chunk.getRightHouseNumberBounds()->second < houseNumber )
								{
									if(currentDifference > houseNumber - chunk.getRightHouseNumberBounds()->second)
									{
										currentDifference = houseNumber - chunk.getRightHouseNumberBounds()->second;
										currentBestNumber = chunk.getRightHouseNumberBounds()->second;
										closestIsFirst = false;
									}
								}
								else
								{
									if(currentDifference > chunk.getRightHouseNumberBounds()->second - houseNumber)
									{
										currentDifference = chunk.getRightHouseNumberBounds()->second - houseNumber;
										currentBestNumber = chunk.getRightHouseNumberBounds()->second;
										closestIsFirst = false;
									}
								}
							}
						}

						if(difference > currentDifference )
						{
							difference = currentDifference;
							bestNumber = currentBestNumber;
							if(closestIsFirst)
								nearestChunk = &chunk;
							else
								nearestChunk = static_cast<MainRoadChunk*>(chunk.getNext());
						}
					}
				}
			}

			// Nearest existing number
			if(nearestChunk)
			{
				return boost::shared_ptr<House>(
					new House(
						*nearestChunk,
						bestNumber,
						numberAtBeginning
				)	);
			}


			// No return
			return boost::shared_ptr<House>();
		}



		void RoadPlace::toParametersMap(
			util::ParametersMap& pm,
			const CoordinatesSystem* coordinatesSystem,
			const std::string& prefix
		) const	{

			// ID
			pm.insert(prefix + DATA_ID, getKey());

			// Name
			pm.insert(prefix + DATA_NAME, getName());

			// City
			if(getCity())
			{
				getCity()->toParametersMap(pm, NULL, prefix);
			}

			// Coordinates
			if(coordinatesSystem && getPoint())
			{
				boost::shared_ptr<Point> pg(
					coordinatesSystem->convertPoint(*getPoint())
				);
				{
					stringstream s;
					s << std::fixed << pg->getX();
					pm.insert(prefix + DATA_X, s.str());
				}
				{
					stringstream s;
					s << std::fixed << pg->getY();
					pm.insert(prefix + DATA_Y, s.str());
				}
			}
		}

		void RoadPlace::toParametersMap(
			util::ParametersMap& pm,
			bool withAdditionalParameters,
			boost::logic::tribool withFiles,
			std::string prefix
		) const {
			toParametersMap(
				pm,
				&CoordinatesSystem::GetInstanceCoordinatesSystem(),
				prefix
			);
		}
}	}
