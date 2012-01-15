
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
#include "RoadPlace.h"
#include "MainRoadPart.hpp"

#include <boost/lexical_cast.hpp>
#include "EdgeProjector.hpp"

using namespace boost;
using namespace std;

namespace synthese
{
	using namespace geography;
	using namespace graph;
	using namespace algorithm;

	namespace util
	{
		template<> const string FactorableTemplate<NamedPlace,road::House>::FACTORY_KEY("House");
		template<> const string Registry<road::House>::KEY("House");
	}

	namespace road
	{
		const std::string House::DATA_ROAD_PREFIX = "road_";
		const std::string House::DATA_NUMBER = "number";
		const std::string House::DATA_X = "x";
		const std::string House::DATA_Y = "y";



		House::House(
			MainRoadChunk& chunk,
			MainRoadChunk::HouseNumber houseNumber,
			bool numberAtBeginning,
			std::string separator
		):	Address(
				chunk,
				chunk.getHouseNumberMetricOffset(houseNumber),
				houseNumber
			), Registrable(0)
		{
			setName(string(
					(numberAtBeginning ? lexical_cast<string>(houseNumber) : chunk.getRoad()->getRoadPlace()->getName()) +
					separator +
					(numberAtBeginning ? chunk.getRoad()->getRoadPlace()->getName() : lexical_cast<string>(houseNumber))
			)	);
			setCity(chunk.getRoad()->getRoadPlace()->getCity());
		}



		House::House(
			util::RegistryKeyType key
		):
			Registrable(key)
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
			std::vector<MainRoadChunk*> roads;
			std::set<Path*> paths(roadPlace->getPaths());

			BOOST_FOREACH(Path* path,paths)
			{
				if(!dynamic_cast<MainRoadPart*>(path))
				{
					continue;
				}
				std::vector<Edge*> edges(path->getEdges());
				BOOST_FOREACH(Edge* edge,edges)
				{
					roads.push_back(dynamic_cast<MainRoadChunk*>(edge));
				}
			}

			EdgeProjector<MainRoadChunk*> projector(roads, maxDistance);

			EdgeProjector<MainRoadChunk*>::PathNearby projection(projector.projectEdge(*(this->getGeometry()->getCoordinate())));
			this->setRoadChunk(projection.get<1>());
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
			const std::string& prefix
		) const	{

			// Road place informations
			getRoadChunk()->getRoad()->getRoadPlace()->toParametersMap(pm, DATA_ROAD_PREFIX);

			// Number
			if(getHouseNumber())
			{
				pm.insert(DATA_NUMBER, *getHouseNumber());
			}

			// Location
			if(getGeometry().get())
			{
				pm.insert(DATA_X, getGeometry()->getX());
				pm.insert(DATA_Y, getGeometry()->getY());
			}
		}



		void House::toParametersMap( util::ParametersMap& pm ) const
		{
			string emptyPrefix;
			toParametersMap(pm, emptyPrefix);
		}
}	}
