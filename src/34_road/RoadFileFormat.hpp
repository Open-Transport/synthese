
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
#include "MainRoadChunk.hpp"
#include "Road.h"

namespace synthese
{
	namespace impex
	{
		class ImportLogger;
	}

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

		//////////////////////////////////////////////////////////////////////////
		/// Helpers library for road import.
		///	@ingroup m34
		class RoadFileFormat
		{
		public:

			//////////////////////////////////////////////////////////////////////////
			/// Road place import helper.
			//////////////////////////////////////////////////////////////////////////
			/// @param code code of the road place for the data source.
			static RoadPlace* CreateOrUpdateRoadPlace(
				impex::ImportableTableSync::ObjectBySource<RoadPlaceTableSync>& roadPlaces,
				const std::string& code,
				const std::string& name,
				const geography::City& city,
				const impex::DataSource& source,
				util::Env& env,
				const impex::ImportLogger& logger
			);


			static RoadPlace* GetRoadPlace(
				impex::ImportableTableSync::ObjectBySource<RoadPlaceTableSync>& roadPlaces,
				const std::string& code,
				const impex::ImportLogger& logger
			);



			static Crossing* CreateOrUpdateCrossing(
				impex::ImportableTableSync::ObjectBySource<CrossingTableSync>& crossings,
				const std::string& code,
				boost::shared_ptr<geos::geom::Point> geometry,
				const impex::DataSource& source,
				util::Env& env,
				const impex::ImportLogger& logger
			);



			//////////////////////////////////////////////////////////////////////////
			/// Public place import helper.
			//////////////////////////////////////////////////////////////////////////
			/// @param code code of the road place for the data source.
			static PublicPlace* CreateOrUpdatePublicPlace(
				impex::ImportableTableSync::ObjectBySource<PublicPlaceTableSync>& publicPlaces,
				const std::string& code,
				const std::string& name,
				boost::optional<boost::shared_ptr<geos::geom::Point> > geometry,
				const geography::City& city,
				const impex::DataSource& source,
				util::Env& env,
				const impex::ImportLogger& logger
			);



			//////////////////////////////////////////////////////////////////////////
			/// Public place entrance import helper.
			//////////////////////////////////////////////////////////////////////////
			/// @param code code of the road place for the data source.
			static PublicPlaceEntrance* CreateOrUpdatePublicPlaceEntrance(
				impex::ImportableTableSync::ObjectBySource<PublicPlaceEntranceTableSync>& publicPlaceEntrances,
				const std::string& code,
				boost::optional<const std::string&> name,
				graph::MetricOffset metricOffset,
				boost::optional<MainRoadChunk::HouseNumber> number,
				MainRoadChunk& roadChunk,
				PublicPlace& publicPlace,
				const impex::DataSource& source,
				util::Env& env,
				const impex::ImportLogger& logger
			);

		private:
			static void _setGeometryAndHouses(
				MainRoadChunk& chunk,
				boost::shared_ptr<geos::geom::LineString> geometry,
				MainRoadChunk::HouseNumberingPolicy rightHouseNumberingPolicy,
				MainRoadChunk::HouseNumberingPolicy leftHouseNumberingPolicy,
				MainRoadChunk::HouseNumberBounds rightHouseNumberBounds,
				MainRoadChunk::HouseNumberBounds leftHouseNumberBounds
			);

		public:
			static MainRoadChunk* AddRoadChunk(
				RoadPlace& roadPlace,
				Crossing& startNode,
				Crossing& endNode,
				boost::shared_ptr<geos::geom::LineString> geometry,
				MainRoadChunk::HouseNumberingPolicy rightHouseNumberingPolicy,
				MainRoadChunk::HouseNumberingPolicy leftHouseNumberingPolicy,
				MainRoadChunk::HouseNumberBounds rightHouseNumberBounds,
				MainRoadChunk::HouseNumberBounds leftHouseNumberBounds,
				util::Env& env,
				Road::RoadType roadType = Road::ROAD_TYPE_UNKNOWN
			);
		};
}	}

#endif // SYNTHESE_road_RoadFileFormat_hpp__

