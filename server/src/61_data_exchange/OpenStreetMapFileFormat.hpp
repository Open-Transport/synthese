
/** OpenStreetMapFileFormat class header.
	@file OpenStreetMapFileFormat.hpp

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

#ifndef SYNTHESE_road_OpenStreetMapFileFormat_hpp__
#define SYNTHESE_road_OpenStreetMapFileFormat_hpp__

#include "FileFormatTemplate.h"
#include "OneFileTypeImporter.hpp"
#include "NoExportPolicy.hpp"
#include "OSMElements.h"

#include "GraphTypes.h"

#include <iostream>
#include <map>
#include <string>
#include <vector>

namespace synthese
{
	namespace geography
	{
		class City;
	}

	namespace road
	{
		class Crossing;
		class Road;
		class RoadChunk;
		class RoadPlace;
	}

	namespace data_exchange
	{
		//////////////////////////////////////////////////////////////////////////
		/// OSM file format.
		//////////////////////////////////////////////////////////////////////////
		/// @ingroup m34
		class OpenStreetMapFileFormat:
			public impex::FileFormatTemplate<OpenStreetMapFileFormat>
		{
		public:

			//////////////////////////////////////////////////////////////////////////
			class Importer_:
				public impex::OneFileTypeImporter<OpenStreetMapFileFormat>
			{
			private:

				static const std::string PARAMETER_ADD_CENTRAL_CHUNK_REFERENCE;

				typedef std::map<util::RegistryKeyType, std::vector<osm::NodePtr> > ChunksAssociatedHousesList;
				typedef std::map<unsigned long long int, boost::shared_ptr<road::Crossing> > _CrossingsMap;
				typedef std::map<unsigned long long int, boost::shared_ptr<geography::City> > _CitiesMap;
				typedef std::map<std::string, boost::shared_ptr<road::RoadPlace> > _RecentlyCreatedRoadPlaces;
				typedef std::map<unsigned long long int, boost::shared_ptr<road::RoadPlace> > _LinkBetweenWayAndRoadPlaces;
				typedef std::map<unsigned long long int, boost::shared_ptr<road::Road> > _RecentlyCreatedRoadParts;

				mutable _CrossingsMap _crossingsMap;
				mutable _RecentlyCreatedRoadPlaces _recentlyCreatedRoadPlaces;
				mutable _RecentlyCreatedRoadParts _recentlyCreatedRoadParts;
				mutable _LinkBetweenWayAndRoadPlaces _linkBetweenWayAndRoadPlaces;
				mutable ChunksAssociatedHousesList _chunksAssociatedHousesList;

				bool _addCentralChunkReference;

			protected:

				virtual bool _parse(
					const boost::filesystem::path& filePath
				) const;

			public:

				typedef enum {
					TWO_WAYS,
					ONE_WAY,
					REVERSED_ONE_WAY
				} TraficDirection;

				Importer_(
					util::Env& env,
					const impex::Import& import,
					impex::ImportLogLevel minLogLevel,
					const std::string& logPath,
					boost::optional<std::ostream&> outputStream,
					util::ParametersMap& pm
				);



				//////////////////////////////////////////////////////////////////////////
				/// Conversion from attributes to generic parameter maps.
				/// @return Generated parameters map
				/// @author Sylvain Pasche
				/// @date 2011
				/// @since 3.3.0
				virtual util::ParametersMap _getParametersMap() const;



				//////////////////////////////////////////////////////////////////////////
				/// Conversion from generic parameters map to attributes.
				/// @param map Parameters map to interpret
				/// @author Sylvain Pasche
				/// @date 2011
				/// @since 3.3.0
				virtual void _setFromParametersMap(const util::ParametersMap& map);



				//////////////////////////////////////////////////////////////////////////
				/// Saves all imported data.
				/// @return SQL transaction to run
				/// @author Sylvain Pasche
				/// @since 3.3.0
				/// @date 2011
				virtual db::DBTransaction _save() const;

			private:

				boost::shared_ptr<road::RoadPlace> _getOrCreateRoadPlace(
					osm::WayPtr &way,
					boost::shared_ptr<geography::City> city
				) const;

				boost::shared_ptr<road::Crossing> _getOrCreateCrossing(
					osm::NodePtr &node,
					boost::shared_ptr<geos::geom::Point> position
				) const;

				void _createRoadChunk(
					const boost::shared_ptr<road::Road> road,
					const boost::shared_ptr<road::Crossing> crossing,
					const boost::optional<boost::shared_ptr<geos::geom::LineString> > geometry,
					std::size_t rank,
					graph::MetricOffset metricOffset,
					TraficDirection traficDirection = TWO_WAYS,
					double maxSpeed = 50 / 3.6,
					bool isNonWalkable = false,
					bool isNonDrivable = false,
					bool isNonBikable = false
				) const;

				void _projectHouseAndUpdateChunkHouseNumberBounds(
					const osm::NodePtr& house,
					std::vector<road::RoadChunk*>& refRoadChunks,
					const bool autoUpdatePolicy = false
				) const;

				void _updateHouseNumberingPolicyAccordingToAssociatedHouseNumbers(
					road::RoadChunk* chunk
				) const;

				void _reorderHouseNumberingBounds(
					boost::shared_ptr<road::RoadChunk> chunk
				) const;

				std::string _toAlphanumericString(
					const std::string& input
				) const;
			};

			typedef impex::NoExportPolicy<OpenStreetMapFileFormat> Exporter_;
		};
	}
}

#endif // SYNTHESE_road_OpenStreetMapFileFormat_hpp__
