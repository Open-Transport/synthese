
/** PublicBikeStation class implementation.
	@file PublicBikeStation.cpp

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

#include "PublicBikeStation.hpp"

#include "City.h"
#include "CityTableSync.h"
#include "Crossing.h"
#include "Hub.h"
#include "Profile.h"
#include "PublicBikingModule.h"
#include "RoadChunkEdge.hpp"
#include "Session.h"
#include "User.h"
#include "VertexAccessMap.h"

#include <geos/geom/Point.h>

using namespace std;
using namespace boost;
using namespace geos::geom;


namespace synthese
{
	CLASS_DEFINITION(public_biking::PublicBikeStation, "t124_public_bike_stations", 124)
	FIELD_DEFINITION_OF_OBJECT(public_biking::PublicBikeStation, "public_bike_station_id", "public_bike_station_ids")

	FIELD_DEFINITION_OF_TYPE(public_biking::StationNumber, "station_number", SQL_TEXT)
	FIELD_DEFINITION_OF_TYPE(public_biking::CityId, "city_id", SQL_INTEGER)
	FIELD_DEFINITION_OF_TYPE(public_biking::TerminalNumber, "terminal_number", SQL_INTEGER)
	FIELD_DEFINITION_OF_TYPE(public_biking::ProjectedRoadChunk, "projected_road_chunk_id", SQL_INTEGER)
	FIELD_DEFINITION_OF_TYPE(public_biking::ProjectedMetricOffset, "projected_metric_offset", SQL_INTEGER)
	FIELD_DEFINITION_OF_TYPE(public_biking::PublicBikeStationDataSource, "datasource", SQL_TEXT)

	namespace util
	{
		template<> const string FactorableTemplate<geography::NamedPlace,public_biking::PublicBikeStation>::FACTORY_KEY("PublicBikeStation");
	}

	namespace public_biking
	{
		PublicBikeStation::PublicBikeStation(
			util::RegistryKeyType id,
			boost::shared_ptr<Point> geometry,
			bool withIndexation
		):	Registrable(id),
			graph::Hub(),
			graph::Vertex(this, geometry, withIndexation),
			Object<PublicBikeStation, PublicBikeStationSchema>(
				Schema(
					FIELD_VALUE_CONSTRUCTOR(Key, id),
					FIELD_DEFAULT_CONSTRUCTOR(Name),
					FIELD_DEFAULT_CONSTRUCTOR(StationNumber),
					FIELD_DEFAULT_CONSTRUCTOR(CityId),
					FIELD_DEFAULT_CONSTRUCTOR(PublicBikeNetwork),
					FIELD_DEFAULT_CONSTRUCTOR(TerminalNumber),
					FIELD_DEFAULT_CONSTRUCTOR(ProjectedRoadChunk),
					FIELD_DEFAULT_CONSTRUCTOR(ProjectedMetricOffset),
					FIELD_DEFAULT_CONSTRUCTOR(PublicBikeStationDataSource),
					FIELD_VALUE_CONSTRUCTOR(PointGeometry, geometry)
			)	),
			NamedPlaceTemplate<PublicBikeStation>()
		{
		}

		PublicBikeStation::~PublicBikeStation()
		{
			unlink();
		}

		graph::Hub::Vertices PublicBikeStation::getVertices(
			graph::GraphIdType graphId
		) const	{
			Vertices result;
			if(graphId == PublicBikingModule::GRAPH_ID)
			{
				result.push_back(this);
			}
			return result;
		}

		boost::posix_time::time_duration PublicBikeStation::getMinTransferDelay() const
		{
			return posix_time::seconds(30); // Hardcoded, shouldn't it be a property of the station ?
		}

		bool PublicBikeStation::isConnectionAllowed( const graph::Vertex& fromVertex , const graph::Vertex& toVertex ) const
		{
			return true;
		}

		boost::posix_time::time_duration PublicBikeStation::getTransferDelay(
			const graph::Vertex& fromVertex,
			const graph::Vertex& toVertex
		) const	{
			return posix_time::seconds(30); // Hardcoded, shouldn't it be a property of the station ?
		}

		graph::HubScore PublicBikeStation::getScore() const
		{
			return 1;
		}

		const boost::shared_ptr<Point>& PublicBikeStation::getPoint() const
		{
			return WithGeometry<Point>::getGeometry();
		}

		std::string PublicBikeStation::getNameForAllPlacesMatcher(
			std::string text
		) const	{
			return (text.empty() ? getName() : text);
		}

		void PublicBikeStation::getVertexAccessMap(
			graph::VertexAccessMap& result,
			graph::GraphIdType whatToSearch,
			const graph::Vertex& origin,
			bool vertexIsOrigin
		) const {
			if(whatToSearch == PublicBikingModule::GRAPH_ID)
			{
				result.insert(
					this,
					graph::VertexAccess()
				);
			}
		}

		bool PublicBikeStation::containsAnyVertex(
			graph::GraphIdType graphType
		) const	{
			if(graphType == PublicBikingModule::GRAPH_ID)
			{
				return true;
			}
			return false;
		}

		void PublicBikeStation::getVertexAccessMap(
			graph::VertexAccessMap& result,
			const graph::AccessParameters& accessParameters,
			const geography::Place::GraphTypes& whatToSearch
		) const {
			if(whatToSearch.find(PublicBikingModule::GRAPH_ID) != whatToSearch.end())
			{
				result.insert(
					this,
					graph::VertexAccess()
				);
			}
		}

		graph::VertexAccess PublicBikeStation::getVertexAccess( const road::Crossing& crossing ) const
		{
			if(_projectedPoint.getRoadChunk())
			{
				if(_projectedPoint.getRoadChunk()->getFromCrossing() == &crossing)
				{
					return graph::VertexAccess(
						boost::posix_time::minutes(static_cast<long>(_projectedPoint.getMetricOffset() / 50)),
						_projectedPoint.getMetricOffset()
					);
				}

				if(_projectedPoint.getRoadChunk()->getForwardEdge().getNext() &&
					static_cast<const road::Crossing*>(_projectedPoint.getRoadChunk()->getForwardEdge().getNext()->getFromVertex()) == &crossing
				){
					// road chunk metric offsets are expressed from the start of the road
					graph::MetricOffset chunkStartOffset = _projectedPoint.getRoadChunk()->getMetricOffset();
					graph::MetricOffset chunkEndOffset   = _projectedPoint.getRoadChunk()->getForwardEdge().getEndMetricOffset();

					// the metric offset of the projected point is expressed from the start of the chunk
					graph::MetricOffset distanceFromChunkStart = _projectedPoint.getMetricOffset();

					// the distance from the projected point to the chunk end is : chunk size - offset of projected point
					graph::MetricOffset distanceFromChunkEnd   = (chunkStartOffset - chunkEndOffset) - distanceFromChunkStart;

					return graph::VertexAccess(
						boost::posix_time::minutes(static_cast<long>(distanceFromChunkEnd / 50)),
						distanceFromChunkEnd
					);
				}
			}
			return graph::VertexAccess();
		}

		void PublicBikeStation::link( util::Env& env, bool withAlgorithmOptimizations /*= false*/ )
		{
			setGeometry(get<PointGeometry>());
			geography::NamedPlace::setName(get<Name>());

			// City
			if(get<CityId>() != 0)
			{
				geography::City* value(NULL);
				util::RegistryKeyType cityId(
					get<CityId>()
				);
				try
				{
					value = geography::CityTableSync::GetEditable(cityId, env).get();
				}
				catch(util::ObjectNotFoundException<geography::City>&)
				{
					util::Log::GetInstance().warn("Bad value " + lexical_cast<string>(cityId) + " for city in public bike station " + lexical_cast<string>(getKey()));
				}
				geography::NamedPlace::setCity(value);
			}

			// Registration to city matcher
			if(getCity())
			{
				const_cast<geography::City*>(getCity())->addPlaceToMatcher(env.getEditableSPtr(this));
			}

			// Registration to public places matcher
			if(	&env == &util::Env::GetOfficialEnv() &&
				withAlgorithmOptimizations
			){
				PublicBikingModule::GetGeneralPublicBikeStationsMatcher().add(
					getFullName(),
					env.getEditableSPtr(this)
				);
			}

			// Projected point
			if(get<ProjectedRoadChunk>())
			{
				if ((_projectedPoint.getRoadChunk() != get<ProjectedRoadChunk>().get_ptr()) ||
					(_projectedPoint.getMetricOffset() != get<ProjectedMetricOffset>()))
				{
					_projectedPoint = road::Address(get<ProjectedRoadChunk>().get(), get<ProjectedMetricOffset>());
				}
			}
			else
			{
				setProjectedPoint(road::Address());
			}

			if(getProjectedPoint().getRoadChunk() &&
				getProjectedPoint().getRoadChunk()->getFromCrossing())
			{
				getProjectedPoint().getRoadChunk()->getFromCrossing()->addReachableVertex(this);
				if(getProjectedPoint().getRoadChunk()->getForwardEdge().getNext())
				{
					static_cast<road::Crossing*>(getProjectedPoint().getRoadChunk()->getForwardEdge().getNext()->getFromVertex())->addReachableVertex(this);
				}
			}
		}

		void PublicBikeStation::unlink()
		{
			geography::City* city(const_cast<geography::City*>(getCity()));
			if (city != NULL)
			{
				city->removePlaceFromMatcher(*this);
				city->removeIncludedPlace(*this);
			}
			if(util::Env::GetOfficialEnv().contains(*this))
			{
				// General public places
				PublicBikingModule::GetGeneralPublicBikeStationsMatcher().remove(
					getFullName()
				);
			}
			if(	getProjectedPoint().getRoadChunk() &&
				getProjectedPoint().getRoadChunk()->getFromCrossing()
			){
				getProjectedPoint().getRoadChunk()->getFromCrossing()->removeReachableVertex(this);
			}
			else if(getProjectedPoint().getRoadChunk() &&
				getProjectedPoint().getRoadChunk()->getForwardEdge().getFromVertex()
			){
				static_cast<road::Crossing*>(getProjectedPoint().getRoadChunk()->getForwardEdge().getFromVertex())->removeReachableVertex(this);
			}
		}



		std::string PublicBikeStation::getName() const
		{
			return geography::NamedPlace::getName();
		}

		void PublicBikeStation::setName(const std::string& value)
		{
			geography::NamedPlace::setName(value);
			set<Name>(value);
		}

		void PublicBikeStation::setCity(geography::City* value)
		{
			geography::NamedPlace::setCity(value);
			if (value)
			{
				set<CityId>(value->getKey());
			}
		}

		void PublicBikeStation::setNetwork(PublicBikeNetwork* value)
		{
			set<PublicBikeNetwork>(value
				? boost::optional<PublicBikeNetwork&>(*const_cast<PublicBikeNetwork*>(value))
				: boost::none);
		}

		void PublicBikeStation::setProjectedPoint(const road::Address& value)
		{
			_projectedPoint = value;
			set<ProjectedRoadChunk>(value.getRoadChunk()
				? boost::optional<road::RoadChunk&>(*const_cast<road::RoadChunk*>(value.getRoadChunk()))
				: boost::none
			);
			set<ProjectedMetricOffset>(value.getMetricOffset());
		}

		PublicBikeNetwork* PublicBikeStation::getPublicBikeNetwork() const
		{
			if (get<PublicBikeNetwork>())
			{
				return get<PublicBikeNetwork>().get_ptr();
			}

			return NULL;
		}

		bool PublicBikeStation::allowUpdate(const server::Session* session) const
		{
			return session && session->hasProfile() && session->getUser()->getProfile()->isAuthorized<security::GlobalRight>(security::WRITE);
		}

		bool PublicBikeStation::allowCreate(const server::Session* session) const
		{
			return session && session->hasProfile() && session->getUser()->getProfile()->isAuthorized<security::GlobalRight>(security::WRITE);
		}

		bool PublicBikeStation::allowDelete(const server::Session* session) const
		{
			return session && session->hasProfile() && session->getUser()->getProfile()->isAuthorized<security::GlobalRight>(security::DELETE_RIGHT);
		}

		void PublicBikeStation::toParametersMap(
			util::ParametersMap& pm,
			const CoordinatesSystem* coordinatesSystem,
			const std::string& prefix
		) const	{

			pm.insert(prefix + Key::FIELD.name, getKey());
			pm.insert(prefix + Name::FIELD.name, getName());
			pm.insert(prefix + StationNumber::FIELD.name, getStationNumber());
			pm.insert(prefix + CityId::FIELD.name, get<CityId>());

			// Network
			pm.insert(
				prefix + PublicBikeNetwork::FIELD.name,
				(	getPublicBikeNetwork() ?
					getPublicBikeNetwork()->getKey() :
					util::RegistryKeyType(0)
			)	);

			pm.insert(prefix + TerminalNumber::FIELD.name, getTerminalNumber());

			// Projected point
			if(getProjectedPoint().getRoadChunk())
			{
				pm.insert(
					prefix + ProjectedRoadChunk::FIELD.name,
					getProjectedPoint().getRoadChunk()->getKey()
				);
				pm.insert(
					prefix + ProjectedMetricOffset::FIELD.name,
					getProjectedPoint().getMetricOffset()
				);
			}
			else
			{
				pm.insert(
					prefix + ProjectedRoadChunk::FIELD.name,
					string()
				);
				pm.insert(
					prefix + ProjectedMetricOffset::FIELD.name,
					string()
				);
			}

			pm.insert(
				prefix + PublicBikeStationDataSource::FIELD.name,
				impex::DataSourceLinks::Serialize(getDataSourceLinks())
			);

			// Geometry
			if(hasGeometry())
			{
				boost::shared_ptr<geos::geom::Geometry> projected(getGeometry());
				if(	coordinatesSystem->getSRID() !=
					static_cast<CoordinatesSystem::SRID>(getGeometry()->getSRID())
				){
					projected = coordinatesSystem->convertGeometry(*getGeometry());
				}

				geos::io::WKTWriter writer;
				pm.insert(
					prefix + PointGeometry::FIELD.name,
					writer.write(projected.get())
				);
			}
			else
			{
				pm.insert(prefix + PointGeometry::FIELD.name, string());
			}
		}

		std::string PublicBikeStation::getRuleUserName() const
		{
			return "Station de vélo " + getName();
		}

		graph::GraphIdType PublicBikeStation::getGraphType() const
		{
			return PublicBikingModule::GRAPH_ID;
		}
}	}
