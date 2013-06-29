
/** RoadShapeFileFormat class header.
	@file RoadShapeFileFormat.hpp
	@author Hugues Romain

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

#ifndef SYNTHESE_road_RoadShapeFileFormat_hpp__
#define SYNTHESE_road_RoadShapeFileFormat_hpp__

#include "FileFormatTemplate.h"
#include "MultipleFileTypesImporter.hpp"
#include "NoExportPolicy.hpp"
#include "RoadFileFormat.hpp"

#include "MainRoadChunk.hpp"
#include "ImportableTableSync.hpp"
#include "RoadPlaceTableSync.h"

#include "Import.hpp"

#include <map>
#include <ostream>
#include <string>

namespace geos
{
	namespace geom
	{
		class LineString;
	}
}

namespace synthese
{
	namespace geography
	{
		class City;
	}

	namespace data_exchange
	{
		//////////////////////////////////////////////////////////////////////////
		/// 34.16 : Road ShapeFile file format class.
		///	@ingroup m34
		/// @author Hugues Romain
		/// @since 3.2.0
		/// @date 2010
		/// @todo Replace cdbfile.h use by shapefil.h dbf support and remove cdbfile.h
		/// @bug the imported files must allow writing (it is not used by synthese)
		/// @bug the circulation side on the streets is forced to right
		//////////////////////////////////////////////////////////////////////////
		class RoadShapeFileFormat:
			public impex::FileFormatTemplate<RoadShapeFileFormat>
		{
		public:

			class Importer_:
				public impex::MultipleFileTypesImporter<RoadShapeFileFormat>,
				public RoadFileFormat
			{
				friend class impex::MultipleFileTypesImporter<RoadShapeFileFormat>;

			private:
				//! @name Tables
				//@{
					static const std::string FILE_ROAD_PLACES;
					static const std::string FILE_ROAD_CHUNKS;
					static const std::string FILE_PUBLIC_PLACES;
				//@}

				//! @name Parameters
				//@{
					static const std::string PARAMETER_FIELD_ROAD_PLACES_CITY_CODE;
					static const std::string PARAMETER_FIELD_ROAD_PLACES_CODE;
					static const std::string PARAMETER_FIELD_ROAD_PLACES_NAME;
					static const std::string PARAMETER_FIELD_ROAD_CHUNKS_FROM_LEFT;
					static const std::string PARAMETER_FIELD_ROAD_CHUNKS_TO_LEFT;
					static const std::string PARAMETER_FIELD_ROAD_CHUNKS_FROM_RIGHT;
					static const std::string PARAMETER_FIELD_ROAD_CHUNKS_TO_RIGHT;
					static const std::string PARAMETER_FIELD_ROAD_CHUNKS_ROAD_PLACE;
					static const std::string PARAMETER_FIELD_ROAD_CHUNKS_CODE;
					static const std::string PARAMETER_FIELD_ROAD_CHUNKS_START_NODE;
					static const std::string PARAMETER_FIELD_ROAD_CHUNKS_END_NODE;
					static const std::string PARAMETER_FIELD_PUBLIC_PLACES_CODE;
					static const std::string PARAMETER_FIELD_PUBLIC_PLACES_NAME;
					static const std::string PARAMETER_FIELD_PUBLIC_PLACES_ROAD_CHUNK;
					static const std::string PARAMETER_FIELD_PUBLIC_PLACES_NUMBER;
					static const std::string PARAMETER_FIELD_PUBLIC_PLACES_CITY_CODE;
				//@}

				//! @name Fields
				//@{
					static const std::string FIELD_GEOMETRY;
				//@}

				std::string _roadPlacesCityCodeField;
				std::string _roadPlacesCodeField;
				std::string _roadPlacesNameField;
				std::string _roadChunksFromLeftField;
				std::string _roadChunksToLeftField;
				std::string _roadChunksFromRightField;
				std::string _roadChunksToRightField;
				std::string _roadChunksRoadPlaceField;
				std::string _roadChunksCodeField;
				std::string _roadChunksStartNodeField;
				std::string _roadChunksEndNodeField;
				std::string _publicPlacesCodeField;
				std::string _publicPlacesNameField;
				std::string _publicPlacesRoadChunkField;
				std::string _publicPlacesNumberField;
				std::string _publicPlacesCityCodeField;

				mutable impex::ImportableTableSync::ObjectBySource<road::RoadPlaceTableSync> _roadPlaces;
				mutable std::map<std::string, road::MainRoadChunk*> _roadChunks;



			protected:

				//////////////////////////////////////////////////////////////////////////
				/// Checks that all necessary input files are available.
				/// @result true if all necessary files are present
				virtual bool _checkPathsMap() const;



				//////////////////////////////////////////////////////////////////////////
				/// Reads an input files and load its content to the memory.
				/// @param filePath file to read
				/// @param key type of the file
				/// @author Hugues Romain
				/// @since 3.2.0
				/// @date 2010
				virtual bool _parse(
					const boost::filesystem::path& filePath,
					const std::string& key
				) const;


			public:
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
				/// @author Hugues Romain
				/// @date 2010
				/// @since 3.1.16
				virtual util::ParametersMap _getParametersMap() const;



				//////////////////////////////////////////////////////////////////////////
				/// Conversion from generic parameters map to attributes.
				/// @param map Parameters map to interpret
				/// @author Hugues Romain
				/// @date 2010
				/// @since 3.1.16
				virtual void _setFromParametersMap(const util::ParametersMap& map);



				//////////////////////////////////////////////////////////////////////////
				/// Saves all imported data.
				/// @return SQL transaction to run
				/// @author Hugues Romain
				/// @since 3.2.0
				/// @date 2010
				virtual db::DBTransaction _save() const;
			};

			typedef impex::NoExportPolicy<RoadShapeFileFormat> Exporter_;

			friend class Importer_;
		};
}	}

#endif
