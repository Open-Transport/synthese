
/** House class implementation.
	@file House.cpp

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

#include "House.hpp"
#include "Road.h"
#include "Path.h"
#include "RoadPath.hpp"
#include "RoadPlace.h"
#include "RoadChunkEdge.hpp"
#include "RoadPlaceTableSync.h"

#include <boost/lexical_cast.hpp>
#include "EdgeProjector.hpp"

using namespace boost;
using namespace std;

namespace synthese
{
	using namespace geography;
	using namespace graph;
	using namespace algorithm;
	using namespace road;
	using namespace util;

	namespace util
	{
		template<> const string FactorableTemplate<NamedPlace,road::House>::FACTORY_KEY("House");
	}


	CLASS_DEFINITION(House, "t078_houses", 78)
	FIELD_DEFINITION_OF_OBJECT(House, "house_id", "house_ids")

	FIELD_DEFINITION_OF_TYPE(Number, "number", SQL_INTEGER)

	namespace road
	{
		const std::string House::DATA_ROAD_PREFIX = "road_";
		const std::string House::DATA_NUMBER = "number";
		const std::string House::DATA_X = "x";
		const std::string House::DATA_Y = "y";



		House::House(
			RoadChunk& chunk,
			HouseNumber houseNumber,
			bool numberAtBeginning,
			std::string separator
		):	Registrable(0),
			Address(
				chunk,
				chunk.getHouseNumberMetricOffset(houseNumber),
				houseNumber
			),
			Object<House, HouseSchema>(
				Schema(
					FIELD_VALUE_CONSTRUCTOR(Key, 0),
					FIELD_DEFAULT_CONSTRUCTOR(impex::DataSourceLinks),
					FIELD_VALUE_CONSTRUCTOR(RoadPlace, chunk.getRoad()->get<RoadPlace>()),
					FIELD_VALUE_CONSTRUCTOR(Number, houseNumber),
					FIELD_DEFAULT_CONSTRUCTOR(PointGeometry)
			)),
			_numberAtBeginning(numberAtBeginning)
		{
			setName(
				string(
					(numberAtBeginning ? lexical_cast<string>(houseNumber) : chunk.getRoad()->get<RoadPlace>()->getName()) +
					separator +
					(numberAtBeginning ? chunk.getRoad()->get<RoadPlace>()->getName() : lexical_cast<string>(houseNumber))
			)	);
			setCity(chunk.getRoad()->get<RoadPlace>()->getCity());
		}



		House::House(
			RoadChunk& chunk,
			double metricOffset,
			bool numberAtBeginning,
			std::string separator
		):	Registrable(0),
			Address(
				chunk,
				metricOffset,
				chunk.getHouseNumberFromOffset(metricOffset)
			),
			Object<House, HouseSchema>(
				Schema(
					FIELD_VALUE_CONSTRUCTOR(Key, 0),
					FIELD_DEFAULT_CONSTRUCTOR(impex::DataSourceLinks),
					FIELD_VALUE_CONSTRUCTOR(RoadPlace, chunk.getRoad()->get<RoadPlace>()),
					FIELD_VALUE_CONSTRUCTOR(Number, chunk.getHouseNumberFromOffset(metricOffset)),
					FIELD_DEFAULT_CONSTRUCTOR(PointGeometry)
			)),
			_numberAtBeginning(numberAtBeginning)
		{
			setName(
				string(
					(numberAtBeginning ? lexical_cast<string>(*(this->getHouseNumber())) : chunk.getRoad()->get<RoadPlace>()->getName()) +
					separator +
					(numberAtBeginning ? chunk.getRoad()->get<RoadPlace>()->getName() : lexical_cast<string>(*(this->getHouseNumber())))
			)	);
			setCity(chunk.getRoad()->get<RoadPlace>()->getCity());
		}



		House::House(
			util::RegistryKeyType key
		):	Registrable(key),
			Object<House, HouseSchema>(
				Schema(
					FIELD_VALUE_CONSTRUCTOR(Key, key),
					FIELD_DEFAULT_CONSTRUCTOR(impex::DataSourceLinks),
					FIELD_DEFAULT_CONSTRUCTOR(RoadPlace),
					FIELD_DEFAULT_CONSTRUCTOR(Number),
					FIELD_DEFAULT_CONSTRUCTOR(PointGeometry)
			)),
			_numberAtBeginning(true)
		{
		}



		const boost::shared_ptr<geos::geom::Point>& House::getPoint() const
		{
			return this->getGeometry();
		}


		void House::setRoadChunkFromRoadPlace(
			boost::shared_ptr<RoadPlace> roadPlace,
			double maxDistance
		){
			// ToDo : Chercher le RoadChunk le plus proche de la géométrie de l'objet House
			std::vector<RoadChunk*> roads;
			RoadPlace::Roads paths(roadPlace->getRoads());

			BOOST_FOREACH(Road* path, paths)
			{
				std::vector<Edge*> edges(path->getForwardPath().getEdges());
				BOOST_FOREACH(Edge* edge,edges)
				{
					roads.push_back(dynamic_cast<RoadChunkEdge*>(edge)->getRoadChunk());
				}
			}

			EdgeProjector<RoadChunk*> projector(roads, maxDistance);

			const boost::shared_ptr<geos::geom::Point>& geometry = this->getGeometry();
			if (geometry.get())
			{
				EdgeProjector<RoadChunk*>::PathNearby projection(projector.projectEdge(*(this->getGeometry()->getCoordinate())));
				this->setRoadChunk(projection.get<1>());
				if(projection.get<1>())
				{
					projection.get<1>()->addHouse(*this);
				}
			}
		}



		void House::getVertexAccessMap(
			graph::VertexAccessMap& result,
			const graph::AccessParameters& accessParameters,
			const GraphTypes& whatToSearch
		) const	{
			return Address::getVertexAccessMap(result, accessParameters, whatToSearch);
		}



		std::string House::getNameForAllPlacesMatcher( std::string text /*= std::string() */ ) const
		{
			return text;
		}



		void House::toParametersMap(
			util::ParametersMap& pm,
			const CoordinatesSystem* coordinatesSystem,
			const std::string& prefix
		) const	{

			// Road place informations
			if (getRoadChunk() != 0)
			{
				getRoadChunk()->getRoad()->get<RoadPlace>()->toParametersMap(pm, coordinatesSystem, DATA_ROAD_PREFIX);
			}

			// Number
			if(getHouseNumber())
			{
				pm.insert(DATA_NUMBER, lexical_cast<string>(*getHouseNumber()));
			}

			// Location
			if(getGeometry().get())
			{
				boost::shared_ptr<Geometry> center(
					coordinatesSystem->convertPoint(*getGeometry())
				);
				pm.insert(DATA_X, center->getX());
				pm.insert(DATA_Y, center->getY());
			}
		}



		void House::toParametersMap(
			util::ParametersMap& pm,
			bool withAdditionalParameters,
			boost::logic::tribool withFiles,
			std::string prefix
		) const	{
			toParametersMap(
				pm,
				&CoordinatesSystem::GetInstanceCoordinatesSystem(),
				prefix
			);
		}



		bool House::_getCityNameBeforePlaceName() const
		{
			return !_numberAtBeginning;
		}


		void House::link(
			util::Env& env,
			bool withAlgorithmOptimizations /*= false*/
		){

			boost::shared_ptr<RoadPlace> roadPlace(get<RoadPlace>() ? get<RoadPlace>().get_ptr() : NULL);
			if (!roadPlace) return;
			try
			{
				setRoadChunkFromRoadPlace(roadPlace);
			}
			catch(ObjectNotFoundException<RoadPlace>&)
			{
				Log::GetInstance().warn("No such road place "+ lexical_cast<string>(roadPlace->getKey()) +" in house "+ lexical_cast<string>(getKey()));
				throw;
			}
			catch(EdgeProjector<RoadChunk*>::NotFoundException&)
			{
				Log::GetInstance().warn("No chunk was found near the house "+ lexical_cast<string>(getKey()) +" in the road place "+ lexical_cast<string>(roadPlace->getKey()));
				throw;
			}
		}

		void House::unlink() {

		}

		bool House::allowUpdate(const server::Session* session) const
		{
			return true;
		}

		bool House::allowCreate(const server::Session* session) const
		{
			return true;
		}

		bool House::allowDelete(const server::Session* session) const
		{
			return true;
		}

}	}
