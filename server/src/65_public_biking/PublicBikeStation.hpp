
/** PublicBikeStation class header.
	@file PublicBikeStation.hpp

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

#ifndef SYNTHESE_public_biking_PublicBikeStation_hpp__
#define SYNTHESE_public_biking_PublicBikeStation_hpp__

#include "Object.hpp"

#include "Address.h"
#include "DataSourceLinksField.hpp"
#include "GeometryField.hpp"
#include "Hub.h"
#include "ImportableTemplate.hpp"
#include "NamedPlaceTemplate.h"
#include "PointerField.hpp"
#include "PublicBikeNetwork.hpp"
#include "Vertex.h"

namespace synthese
{
	namespace road
	{
		class Address;
		class RoadChunk;
	}

	namespace public_biking
	{
		class PublicBikeNetwork;

		FIELD_STRING(StationNumber)
		FIELD_ID(CityId)
		FIELD_INT(TerminalNumber)
		FIELD_POINTER(ProjectedRoadChunk, road::RoadChunk)
		FIELD_DOUBLE(ProjectedMetricOffset)
		FIELD_DATASOURCE_LINKS(PublicBikeStationDataSource)

		typedef boost::fusion::map<
			FIELD(Key),
			FIELD(Name),
			FIELD(StationNumber),
			FIELD(CityId),
			FIELD(public_biking::PublicBikeNetwork),
			FIELD(TerminalNumber),
			FIELD(ProjectedRoadChunk),
			FIELD(ProjectedMetricOffset),
			FIELD(PublicBikeStationDataSource),
			FIELD(PointGeometry)
		> PublicBikeStationSchema;

		//////////////////////////////////////////////////////////////////////////
		/// Public bike station.
		///	@ingroup m65
		/// @author Camille Hue
		//////////////////////////////////////////////////////////////////////////
		class PublicBikeStation:
			public graph::Hub,
			public graph::Vertex,
			public Object<PublicBikeStation, PublicBikeStationSchema>,
			public geography::NamedPlaceTemplate<PublicBikeStation>,
			public impex::ImportableTemplate<PublicBikeStation>
		{
		private:
			road::Address _projectedPoint;

		public:

			PublicBikeStation(
				util::RegistryKeyType id = 0,
				boost::shared_ptr<geos::geom::Point> geometry = boost::shared_ptr<geos::geom::Point>(),
				bool withIndexation = true
			);

			~PublicBikeStation();

			//! @name Getters
			//@{
				std::string getStationNumber() const { return get<StationNumber>(); }
				PublicBikeNetwork* getPublicBikeNetwork() const;
				int getTerminalNumber() const { return get<TerminalNumber>(); }
				const road::Address& getProjectedPoint() const { return _projectedPoint; }
				virtual std::string getName() const;
			//@}

			//! @name Setters
			//@{
				void setStationNumber(const std::string& value) { set<StationNumber>(value); }
				void setNetwork(PublicBikeNetwork* value);
				void setTerminalNumber(int value) { set<TerminalNumber>(value); }
				void setProjectedPoint(const road::Address& value);
				virtual void setName(const std::string& value);
				virtual void setCity(geography::City* value);
			//@}


			//! @name Virtual queries for Hub interface
			//@{
				virtual Vertices getVertices(
					graph::GraphIdType graphId
				) const;

				virtual boost::posix_time::time_duration	getMinTransferDelay() const;

				virtual bool isConnectionAllowed(
					const graph::Vertex& fromVertex,
					const graph::Vertex& toVertex
				) const;

				virtual boost::posix_time::time_duration getTransferDelay(
					const graph::Vertex& fromVertex,
					const graph::Vertex& toVertex
				) const;

				virtual graph::HubScore getScore() const;

				virtual const boost::shared_ptr<geos::geom::Point>& getPoint() const;

				virtual std::string getNameForAllPlacesMatcher(
					std::string text = std::string()
				) const;

				virtual void getVertexAccessMap(
					graph::VertexAccessMap& result,
					graph::GraphIdType whatToSearch,
					const graph::Vertex& origin,
					bool vertexIsOrigin
				) const;

				virtual bool containsAnyVertex(graph::GraphIdType graphType) const;
			//@}



			//! @name Virtual queries for Place interface
			//@{
				virtual void getVertexAccessMap(
					graph::VertexAccessMap& result,
					const graph::AccessParameters& accessParameters,
					const geography::Place::GraphTypes& whatToSearch
				) const;

				virtual std::string getRuleUserName() const;

				virtual graph::GraphIdType getGraphType() const;
			//@}


			//! @name Services
			//@{
				virtual void link(util::Env& env, bool withAlgorithmOptimizations = false);
				virtual void unlink();

				void toParametersMap(
					util::ParametersMap& pm,
					bool withAdditionalParameters,
					const CoordinatesSystem& coordinatesSystem,
					std::string prefix = std::string()
				) const;
			//@}

			virtual bool allowUpdate(const server::Session* session) const;
			virtual bool allowCreate(const server::Session* session) const;
			virtual bool allowDelete(const server::Session* session) const;
		};
}	}

#endif // SYNTHESE_public_biking_PublicBikeStation_hpp__
