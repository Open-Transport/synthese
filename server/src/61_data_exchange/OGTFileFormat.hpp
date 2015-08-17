
/** OGTFileFormat class header.
	@file OGTFileFormat.hpp

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

#ifndef SYNTHESE_OGTFileFormat_H__
#define SYNTHESE_OGTFileFormat_H__

#include "FileFormatTemplate.h"
#include "NoExportPolicy.hpp"
#include "OneFileTypeImporter.hpp"
#include "PTDataCleanerFileFormat.hpp"
#include "PTFileFormat.hpp"

#include "ImportableTableSync.hpp"
#include "CommercialLineTableSync.h"
#include "StopPointTableSync.hpp"
#include "JourneyPattern.hpp"
#include "SchedulesBasedService.h"

#include <expat.h>

namespace synthese
{
	namespace calendar
	{
		class Calendar;
	}

	namespace data_exchange
	{
		//////////////////////////////////////////////////////////////////////////
		/// OGT file format.
		//////////////////////////////////////////////////////////////////////////
		/// @ingroup m35File refFile
		class OGTFileFormat:
			public impex::FileFormatTemplate<OGTFileFormat>
		{
		public:

			//////////////////////////////////////////////////////////////////////////
			class Importer_:
				public impex::OneFileTypeImporter<OGTFileFormat>,
				public PTDataCleanerFileFormat,
				public PTFileFormat
			{
			public:

			private:

				class ExpatParser
				{
				public:
					ExpatParser(
						const impex::DataSource& dataSource,
						const Importer_& importer
					);
					virtual ~ExpatParser();

					virtual void parse(std::istream &data) throw(std::runtime_error);

				private:
					static void startElement(void *d, const XML_Char* name, const XML_Char** attrs) throw(Exception);
					static void endElement(void *d, const XML_Char* name) throw(Exception);
					static void characters(void*, const XML_Char* txt, int txtlen);

					struct expat_user_data
					{
						const impex::DataSource& _dataSource;
						const Importer_& _importer;
						pt::CommercialLine* line;
						bool wayBack;
						bool importTrip;
						std::string tripNumber;
						pt::JourneyPattern::StopsWithDepartureArrivalAuthorization tripStops;
						pt::SchedulesBasedService::Schedules departureSchedules;
						pt::SchedulesBasedService::Schedules arrivalSchedules;
						boost::posix_time::time_duration arrivalSchedule;
						impex::ImportableTableSync::ObjectBySource<pt::StopPointTableSync> stopPoints;

						expat_user_data(
							const impex::DataSource& dataSource,
							const Importer_& importer
						);
					};

					expat_user_data user_data;

					typedef std::map<std::string, std::string> AttributeMap;
					static AttributeMap createAttributeMap(const XML_Char **attrs);
				};

				friend class ExpatParser;

			public:

				virtual bool _parse(
					const boost::filesystem::path& filePath
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

			typedef impex::NoExportPolicy<OGTFileFormat> Exporter_;
		};
}	}

#endif
