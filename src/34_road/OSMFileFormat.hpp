
/** OSMFileFormat class header.
	@file OSMFileFormat.hpp

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

#ifndef SYNTHESE_road_OSMFileFormat_hpp__
#define SYNTHESE_road_OSMFileFormat_hpp__

#include "FileFormatTemplate.h"
#include "OneFileTypeImporter.hpp"
#include "NoExportPolicy.hpp"
#include "OSMElements.h"

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
		class MainRoadPart;

		//////////////////////////////////////////////////////////////////////////
		/// OSM file format.
		//////////////////////////////////////////////////////////////////////////
		/// @ingroup m34
		class OSMFileFormat:
			public impex::FileFormatTemplate<OSMFileFormat>
		{
		public:

			//////////////////////////////////////////////////////////////////////////
			class Importer_:
				public impex::OneFileTypeImporter<OSMFileFormat>
			{
			private:

				typedef std::map<int, boost::shared_ptr<road::Crossing> > _CrossingsMap;
				typedef std::map<int, boost::shared_ptr<geography::City> > _CitiesMap;
				typedef std::map<std::string, boost::shared_ptr<RoadPlace> > _RecentlyCreatedRoadPlaces;

				mutable _CrossingsMap _crossingsMap;
				mutable _RecentlyCreatedRoadPlaces _recentlyCreatedRoadPlaces;

			protected:

				virtual bool _parse(
					const boost::filesystem::path& filePath,
					std::ostream& os,
					boost::optional<const admin::AdminRequest&> adminRequest
				) const;

			public:
				Importer_(
					util::Env& env,
					const impex::DataSource& dataSource
				);

				//////////////////////////////////////////////////////////////////////////
				/// Import screen to include in the administration console.
				/// @param os stream to write the result on
				/// @param request request for display of the administration console
				/// @author Sylvain Pasche
				/// @date 2011
				/// @since 3.3.0
				virtual void displayAdmin(
					std::ostream& os,
					const admin::AdminRequest& request
				) const;



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
					const boost::shared_ptr<road::MainRoadPart> road,
					const boost::shared_ptr<road::Crossing> crossing,
					const boost::optional<boost::shared_ptr<geos::geom::LineString> > geometry,
					std::size_t rank,
					graph::MetricOffset metricOffset
				) const;

			};

			typedef impex::NoExportPolicy<OSMFileFormat> Exporter_;
		};
	}
}

#endif // SYNTHESE_road_OSMFileFormat_hpp__
