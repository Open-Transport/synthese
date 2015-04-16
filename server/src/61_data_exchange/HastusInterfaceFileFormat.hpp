
/** HastusInterfaceFileFormat class header.
	@file HastusInterfaceFileFormat.hpp

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


#ifndef SYNTHESE_IMPEX_HastusFileFormat_H
#define SYNTHESE_IMPEX_HastusFileFormat_H

#include "CalendarFileFormat.hpp"
#include "FileFormatTemplate.h"
#include "OneFileTypeImporter.hpp"
#include "PTDataCleanerFileFormat.hpp"
#include "PTFileFormat.hpp"
#include "NoExportPolicy.hpp"

#include "CalendarTemplateTableSync.h"
#include "CommercialLineTableSync.h"
#include "GraphTypes.h"
#include "ScheduledService.h"
#include "StopPointTableSync.hpp"

namespace synthese
{
	namespace pt
	{
		class TransportNetwork;
	}

	namespace data_exchange
	{
		//////////////////////////////////////////////////////////////////////////
		/// Hastus Interface file format.
		/// @ingroup m61File refFile
		///
		class HastusInterfaceFileFormat:
			public impex::FileFormatTemplate<HastusInterfaceFileFormat>
		{
		public:
			class Importer_:
				public impex::OneFileTypeImporter<Importer_>,
				public PTDataCleanerFileFormat,
				public PTFileFormat,
				public CalendarFileFormat
			{
			private:
				//! @name Import parameters
				//@{
					boost::shared_ptr<pt::TransportNetwork> _network;
					bool _fileNameIsACalendar;
				//@}

				struct Record
				{
					double recordNumber;
					std::string content;
				};

				mutable std::ifstream _file;
				mutable boost::optional<Record> _record;
				mutable impex::ImportableTableSync::ObjectBySource<calendar::CalendarTemplateTableSync> _calendars;
				mutable impex::ImportableTableSync::ObjectBySource<pt::CommercialLineTableSync> _lines;
				mutable impex::ImportableTableSync::ObjectBySource<pt::StopPointTableSync> _stops;

				struct TemporaryService
				{
					std::string lineCode;
					std::string calendar;
					std::string code;
					std::string routeCode;
					pt::ScheduledService::Schedules schedules;
					std::vector<graph::MetricOffset> scheduledStopsDistances;
					std::vector<std::string> scheduledStops;
					pt::ScheduledService::Schedules allSchedules;
					std::vector<std::string> stops;
					bool wayBack;
				};

				//////////////////////////////////////////////////////////////////////////
				/// @return Record : recordNumber == 0 => end of file
				void _loadNextRecord(
					double recordNumber
				) const;

				void _loadNextRecord(
				) const;

				void _openFile(
					const boost::filesystem::path& filePath
				) const;


				//////////////////////////////////////////////////////////////////////////
				/// Gets a text field in the file.
				/// @param start the position of the field in the line according to the file
				/// format specification (begins at 1 instead of 0 as usually)
				/// @param length length of the field
				/// @return the returned text is trimmed
				std::string _getTextField(
					std::size_t start,
					std::size_t length
				) const;



				//////////////////////////////////////////////////////////////////////////
				/// Gets a fields vector in the file.
				/// @param numberPosition the position of the length field in the first line according to the file
				/// format specification (begins at 1 instead of 0 as usually)
				/// @param startPosition the position of the first field in the first line according to the file
				/// format specification (begins at 1 instead of 0 as usually)
				/// @param recordLength length of each field
				/// @return all the returned text elements are trimmed
				std::vector<std::string> _getNextVector(
					double recordNumber,
					std::size_t numberPosition,
					std::size_t startPosition,
					std::size_t recordLength
				) const;

				bool _eof() const;

			public:
				static const std::string PARAMETER_TRANSPORT_NETWORK_ID;
				static const std::string PARAMETER_FILE_NAME_IS_A_CALENDAR;

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
				/// File import.
				/// @param filePath path of the file to import
				/// @author Hugues Romain
				virtual bool _parse(
					const boost::filesystem::path& filePath
				) const;



				virtual db::DBTransaction _save() const;
			};

			typedef impex::NoExportPolicy<HastusInterfaceFileFormat> Exporter_;

			friend class Importer_;
		};
}	}

#endif
