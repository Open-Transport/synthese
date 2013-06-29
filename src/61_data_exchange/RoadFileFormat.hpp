
/** RoadFileFormat class header.
	@file RoadFileFormat.hpp

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

#ifndef SYNTHESE_road_RoadFileFormat_hpp__
#define SYNTHESE_road_RoadFileFormat_hpp__

#include "ImportableTableSync.hpp"
#include "Importer.hpp"

#include "MainRoadChunk.hpp"
#include "Road.h"

namespace synthese
{
	namespace geography
	{
		class City;
	}

	namespace road
	{
		class PublicPlace;
		class PublicPlaceEntrance;
		class PublicPlaceEntranceTableSync;
		class PublicPlaceTableSync;
		class RoadPlace;
		class RoadPlaceTableSync;
		class Crossing;
		class CrossingTableSync;
	}

	namespace data_exchange
	{		
		//////////////////////////////////////////////////////////////////////////
		/// Helpers library for road import.
		///	@ingroup m34
		class RoadFileFormat:
			public virtual impex::Importer
		{
		protected:
			RoadFileFormat(
				util::Env& env,
				const impex::Import& import,
				impex::ImportLogLevel minLogLevel,
				const std::string& logPath,
				boost::optional<std::ostream&> outputStream,
				util::ParametersMap& pm
			);


		public:

			//////////////////////////////////////////////////////////////////////////
			/// Road place import helper.
			//////////////////////////////////////////////////////////////////////////
			/// @param code code of the road place for the data source.
			road::RoadPlace* _createOrUpdateRoadPlace(
				impex::ImportableTableSync::ObjectBySource<road::RoadPlaceTableSync>& roadPlaces,
				const std::string& code,
				const std::string& name,
				const geography::City& city
			) const;



			road::RoadPlace* _getRoadPlace(
				impex::ImportableTableSync::ObjectBySource<road::RoadPlaceTableSync>& roadPlaces,
				const std::string& code
			) const;



			road::Crossing* _createOrUpdateCrossing(
				impex::ImportableTableSync::ObjectBySource<road::CrossingTableSync>& crossings,
				const std::string& code,
				boost::shared_ptr<geos::geom::Point> geometry
			) const;



			//////////////////////////////////////////////////////////////////////////
			/// Public place import helper.
			//////////////////////////////////////////////////////////////////////////
			/// @param code code of the road place for the data source.
			road::PublicPlace* _createOrUpdatePublicPlace(
				impex::ImportableTableSync::ObjectBySource<road::PublicPlaceTableSync>& publicPlaces,
				const std::string& code,
				const std::string& name,
				boost::optional<boost::shared_ptr<geos::geom::Point> > geometry,
				const geography::City& city
			) const;



			//////////////////////////////////////////////////////////////////////////
			/// Public place entrance import helper.
			//////////////////////////////////////////////////////////////////////////
			/// @param code code of the road place for the data source.
			road::PublicPlaceEntrance* _createOrUpdatePublicPlaceEntrance(
				impex::ImportableTableSync::ObjectBySource<road::PublicPlaceEntranceTableSync>& publicPlaceEntrances,
				const std::string& code,
				boost::optional<const std::string&> name,
				graph::MetricOffset metricOffset,
				boost::optional<road::MainRoadChunk::HouseNumber> number,
				road::MainRoadChunk& roadChunk,
				road::PublicPlace& publicPlace
			) const;

		private:
			static void _setGeometryAndHouses(
				road::MainRoadChunk& chunk,
				boost::shared_ptr<geos::geom::LineString> geometry,
				road::MainRoadChunk::HouseNumberingPolicy rightHouseNumberingPolicy,
				road::MainRoadChunk::HouseNumberingPolicy leftHouseNumberingPolicy,
				road::MainRoadChunk::HouseNumberBounds rightHouseNumberBounds,
				road::MainRoadChunk::HouseNumberBounds leftHouseNumberBounds
			);

		public:
			road::MainRoadChunk* _addRoadChunk(
				road::RoadPlace& roadPlace,
				road::Crossing& startNode,
				road::Crossing& endNode,
				boost::shared_ptr<geos::geom::LineString> geometry,
				road::MainRoadChunk::HouseNumberingPolicy rightHouseNumberingPolicy,
				road::MainRoadChunk::HouseNumberingPolicy leftHouseNumberingPolicy,
				road::MainRoadChunk::HouseNumberBounds rightHouseNumberBounds,
				road::MainRoadChunk::HouseNumberBounds leftHouseNumberBounds,
				road::Road::RoadType roadType = road::Road::ROAD_TYPE_UNKNOWN
			) const;
		};
}	}

#endif // SYNTHESE_road_RoadFileFormat_hpp__

