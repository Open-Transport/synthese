
/** ObitiFileFormat class header.
	@file ObitiFileFormat.hpp

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

#ifndef SYNTHESE_ObitiFileFormat_H__
#define SYNTHESE_ObitiFileFormat_H__

#include "FileFormatTemplate.h"
#include "MultipleFileTypesImporter.hpp"
#include "NoExportPolicy.hpp"
#include "PTDataCleanerFileFormat.hpp"
#include "PTFileFormat.hpp"

#include "ImportableTableSync.hpp"
#include "StopPointTableSync.hpp"
#include "StopAreaTableSync.hpp"
#include "CommercialLineTableSync.h"

#include <fstream>
#include <iostream>
#include <map>
#include <string>
#include <vector>

namespace synthese
{
	namespace util
	{
		class Registrable;
	}

	namespace geography
	{
		class City;
	}

	namespace pt
	{
		class JourneyPattern;
		class CommercialLine;
		class ScheduledService;
		class TransportNetwork;
		class PTUseRule;
		class ServiceCalendarLink;
	}

	namespace data_exchange
	{
		//////////////////////////////////////////////////////////////////////////
		/// Obiti file format.
		//////////////////////////////////////////////////////////////////////////
		/// @ingroup m61File refFile
		class ObitiFileFormat:
			public impex::FileFormatTemplate<ObitiFileFormat>
		{
		public:

			//////////////////////////////////////////////////////////////////////////
			class Importer_:
				public impex::MultipleFileTypesImporter<ObitiFileFormat>,
				public PTDataCleanerFileFormat,
				public PTFileFormat
			{
			public:
				static const std::string FILE_ARRET;
				static const std::string FILE_ITINERAIRES;
				static const std::string FILE_LIGNE;
				static const std::string PATH_HORAIRES;

				static const std::string PARAMETER_ROLLING_STOCK_ID;
				static const std::string PARAMETER_IMPORT_STOP_AREA;
				static const std::string PARAMETER_STOP_AREA_DEFAULT_TRANSFER_DURATION;
				static const std::string PARAMETER_USE_RULE_BLOCK_ID_MASK;
				static const std::string PARAMETER_STOPS_DATA_SOURCE_ID;
				static const std::string PARAMETER_STOPS_FROM_DATA_SOURCE;

				static const std::string PARAMETER_PERIOD_CALENDAR_FIELD;
				static const std::string PARAMETER_DAYS_CALENDAR_FIELD;
				static const std::string PARAMETER_NUMBER_OF_OTHER_PARAMETERS;
				static const std::string PARAMETER_BACKWARD_IN_SAME_FILE;

			private:
				static const std::string SEP;

				boost::shared_ptr<vehicle::RollingStock> _rollingStock;
				std::string _periodCalendarField;
				std::string _daysCalendarField;
				size_t _numberOfOtherParameters;
				bool _backwardInSameFile;
				bool _stopsFromDataSource;
				boost::shared_ptr<impex::DataSource> _stopsDataSource;

				bool _interactive;
				boost::posix_time::time_duration _stopAreaDefaultTransferDuration;

				typedef std::map<std::string, std::size_t> FieldsMap;
				mutable FieldsMap _fieldsMap;

				mutable std::vector<std::string> _line;

				typedef std::map<std::string, const pt::PTUseRule*> PTUseRuleBlockMasks;
				PTUseRuleBlockMasks _ptUseRuleBlockMasks;
				static std::string _serializePTUseRuleBlockMasks(const PTUseRuleBlockMasks& object);

				struct LineObitiElement
				{
					std::string name;
					std::string shortname;
					bool backward;
				};
				typedef std::map<std::string, LineObitiElement> LinesMap;
				mutable LinesMap _linesMap;

				void _loadFieldsMap(const std::string& line) const;
				std::string _getValue(const std::string& field) const;
				void _loadLine(const std::string& line) const;
				void _firstLine(std::ifstream& inFile, std::string& line, std::streampos pos) const;
				bool _moveToField(std::ifstream& inFile, const std::string& field) const;

				mutable impex::ImportableTableSync::ObjectBySource<pt::CommercialLineTableSync> _lines;
				mutable impex::ImportableTableSync::ObjectBySource<pt::StopAreaTableSync> _stopAreas;
				mutable impex::ImportableTableSync::ObjectBySource<pt::StopPointTableSync> _stopPoints;


			protected:

				virtual bool _checkPathsMap() const;

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
				/// @date 2011
				/// @since 3.2.1
				virtual util::ParametersMap _getParametersMap() const;



				//////////////////////////////////////////////////////////////////////////
				/// Conversion from generic parameters map to attributes.
				/// @param map Parameters map to interpret
				/// @author Hugues Romain
				/// @date 2011
				/// @since 3.2.1
				virtual void _setFromParametersMap(const util::ParametersMap& map);



				virtual db::DBTransaction _save() const;
			};

			typedef impex::NoExportPolicy<ObitiFileFormat> Exporter_;
		};
	}
}

#endif
