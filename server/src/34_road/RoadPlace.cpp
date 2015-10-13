
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

#include "RoadPlace.h"

#include "CityTableSync.h"
#include "ImportableTableSync.hpp"
#include "Road.h"
#include "RoadChunkEdge.hpp"
#include "RoadModule.h"
#include "RoadPath.hpp"
#include "RoadChunk.h"
#include "RoadPlaceTableSync.h"
#include "VertexAccessMap.h"
#include "AllowedUseRule.h"
#include "GraphConstants.h"
#include "Vertex.h"
#include "House.hpp"

#include <boost/foreach.hpp>
#include <geos/geom/Envelope.h>

using namespace std;
using namespace boost;
using namespace geos::geom;

namespace synthese
{
	using namespace impex;
	using namespace util;
	using namespace graph;
	using namespace geography;
	using namespace road;

	namespace util
	{
		template<> const string FactorableTemplate<NamedPlace,road::RoadPlace>::FACTORY_KEY("RoadPlace");
	}

	CLASS_DEFINITION(RoadPlace, "t060_road_places", 60)
	FIELD_DEFINITION_OF_OBJECT(RoadPlace, "road_place_id", "road_place_ids")
	FIELD_DEFINITION_OF_TYPE(IsCityMainRoad, "is_city_main_road", SQL_BOOLEAN)

	namespace road
	{
		const string RoadPlace::DATA_ID = "id";
		const string RoadPlace::DATA_NAME = "name";
		const string RoadPlace::DATA_X = "x";
		const string RoadPlace::DATA_Y = "y";

		RoadPlace::RoadPlace(
			RegistryKeyType id
		):	Registrable(id),
			Object<RoadPlace, RoadPlaceSchema>(
				Schema(
					FIELD_VALUE_CONSTRUCTOR(Key, id),
					FIELD_DEFAULT_CONSTRUCTOR(NamedPlaceField),
					FIELD_DEFAULT_CONSTRUCTOR(impex::DataSourceLinks),
					FIELD_DEFAULT_CONSTRUCTOR(IsCityMainRoad)
			)	),
			NamedPlaceTemplate<RoadPlace>()
		{
			RuleUser::Rules rules(RuleUser::GetEmptyRules());
			rules[USER_PEDESTRIAN - USER_CLASS_CODE_OFFSET] = AllowedUseRule::INSTANCE.get();
			rules[USER_HANDICAPPED - USER_CLASS_CODE_OFFSET] = AllowedUseRule::INSTANCE.get();
			rules[USER_BIKE - USER_CLASS_CODE_OFFSET] = AllowedUseRule::INSTANCE.get();
			rules[USER_CAR - USER_CLASS_CODE_OFFSET] = AllowedUseRule::INSTANCE.get();
			setRules(rules);
		}



		RoadPlace::~RoadPlace()
		{
			BOOST_FOREACH(Road* road, _roads)
			{
				road->set<RoadPlace>(boost::none);
				road->getForwardPath().setPathGroup(NULL);
				road->getReversePath().setPathGroup(NULL);
			}
		}



		void RoadPlace::addRoad(
			RoadPath& road
		){
			addPath(static_cast<Path*>(&road));
			_isoBarycentre.reset();
		}



		void RoadPlace::addRoad( Road& road ) const
		{
			_roads.insert(&road);
			_isoBarycentre.reset();
		}



		void RoadPlace::removeRoad(
			RoadPath& road
		){
			removePath(static_cast<Path*>(&road));
			_isoBarycentre.reset();
		}



		void RoadPlace::removeRoad( Road& road ) const
		{
			_roads.erase(&road);
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
			HouseNumber houseNumber,
			bool numberAtBeginning
		) const	{
			RoadChunk* nearestChunk(NULL);
			HouseNumber difference(numeric_limits<HouseNumber>::max());
			HouseNumber bestNumber(0);
			BOOST_FOREACH(Path* path, getPaths())
			{
				RoadPath* roadPart(dynamic_cast<RoadPath*>(path));
				if(!roadPart)
				{
					continue;
				}

				BOOST_FOREACH(Edge* edge, path->getEdges())
				{
					RoadChunk& chunk(*static_cast<RoadChunkEdge&>(*edge).getRoadChunk());
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
						HouseNumber currentDifference(numeric_limits<HouseNumber>::max());
						HouseNumber currentBestNumber(0);

						if(chunk.getLeftHouseNumberBounds())
						{
							if( (	chunk.getLeftHouseNumberingPolicy() == ODD_NUMBERS  && houseNumber % 2
								) ||
								(	chunk.getLeftHouseNumberingPolicy() == EVEN_NUMBERS && !(houseNumber % 2)
								) ||
								(	chunk.getLeftHouseNumberingPolicy() == ALL_NUMBERS
							)	){
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
									}
								}
								else
								{
									if(currentDifference > chunk.getLeftHouseNumberBounds()->second - houseNumber)
									{
										currentDifference = chunk.getLeftHouseNumberBounds()->second - houseNumber;
										currentBestNumber = chunk.getLeftHouseNumberBounds()->second;
									}
								}
							}
						}

						if(chunk.getRightHouseNumberBounds())
						{
							if( (	chunk.getRightHouseNumberingPolicy() == ODD_NUMBERS  && houseNumber % 2
								) ||
								(	chunk.getRightHouseNumberingPolicy() == EVEN_NUMBERS && !(houseNumber % 2)
								) ||
								(	chunk.getRightHouseNumberingPolicy() == ALL_NUMBERS
							)	){

								//Right distance calculation
								if( chunk.getRightHouseNumberBounds()->first < houseNumber )
								{
									if(currentDifference > houseNumber - chunk.getRightHouseNumberBounds()->first)
									{
										currentDifference = houseNumber - chunk.getRightHouseNumberBounds()->first;
										currentBestNumber = chunk.getRightHouseNumberBounds()->first;
									}
								}
								else
								{
									if(currentDifference > chunk.getRightHouseNumberBounds()->first - houseNumber)
									{
										currentDifference = chunk.getRightHouseNumberBounds()->first - houseNumber;
										currentBestNumber = chunk.getRightHouseNumberBounds()->first;
									}
								}

								if( chunk.getRightHouseNumberBounds()->second < houseNumber )
								{
									if(currentDifference > houseNumber - chunk.getRightHouseNumberBounds()->second)
									{
										currentDifference = houseNumber - chunk.getRightHouseNumberBounds()->second;
										currentBestNumber = chunk.getRightHouseNumberBounds()->second;
									}
								}
								else
								{
									if(currentDifference > chunk.getRightHouseNumberBounds()->second - houseNumber)
									{
										currentDifference = chunk.getRightHouseNumberBounds()->second - houseNumber;
										currentBestNumber = chunk.getRightHouseNumberBounds()->second;
									}
								}
							}
						}

						if(difference > currentDifference )
						{
							difference = currentDifference;
							bestNumber = currentBestNumber;
							nearestChunk = &chunk;
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
				getCity()->toParametersMap(pm, NULL, prefix + "city_");
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

		void RoadPlace::link(
			util::Env& env,
			bool withAlgorithmOptimizations /*= false*/
		){

			City* city(const_cast<City*>(getCity()));
			if (city) {
				if(!getName().empty())
				{
					city->addPlaceToMatcher(env.getEditableSPtr(this));
				}

				if(getIsCityMainRoad())
				{
					city->addIncludedPlace(*this);
				}
				else
				{
					city->removeIncludedPlace(*this);
				}
			}

			// Registration to all places matcher
			if(	&env == &Env::GetOfficialEnv() &&
				withAlgorithmOptimizations
			){
				GeographyModule::GetGeneralAllPlacesMatcher().add(
					getFullName(),
					env.getEditableSPtr(this)
				);
			}

			// Registration to road places matcher
			if(	&env == &Env::GetOfficialEnv() &&
				withAlgorithmOptimizations
			){
				RoadModule::GetGeneralRoadsMatcher().add(
					getFullName(),
					env.getEditableSPtr(this)
				);
			}
		}



		void RoadPlace::unlink()
		{
			// City matcher
			City* city(const_cast<City*>(getCity()));
			if (city)
			{
				city->removePlaceFromMatcher(*this);
			}

			if(Env::GetOfficialEnv().contains(*this))
			{
				// General all places
				GeographyModule::GetGeneralAllPlacesMatcher().remove(
					getFullName()
				);
			}

			if(Env::GetOfficialEnv().contains(*this))
			{
				// General public places
				RoadModule::GetGeneralRoadsMatcher().remove(
					getFullName()
				);
			}
		}

		bool RoadPlace::allowUpdate(const server::Session* session) const
		{
			return true;
		}

		bool RoadPlace::allowCreate(const server::Session* session) const
		{
			return true;
		}

		bool RoadPlace::allowDelete(const server::Session* session) const
		{
			return true;
		}

}	}
