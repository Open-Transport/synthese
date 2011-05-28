
/** OGTFileFormat class header.
	@file IneoFileFormat.hpp

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

#ifndef SYNTHESE_OGTFileFormat_H__
#define SYNTHESE_OGTFileFormat_H__

#include "FileFormatTemplate.h"
#include "OneFileTypeImporter.hpp"
#include "NoExportPolicy.hpp"
#include "ImportableTableSync.hpp"
#include "CommercialLineTableSync.h"
#include "StopPointTableSync.hpp"
#include "JourneyPattern.hpp"
#include "SchedulesBasedService.h"
#include "PTDataCleanerFileFormat.hpp"

#include <expat.h>

namespace synthese
{
	namespace calendar
	{
		class Calendar;
	}

	namespace pt
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
				public PTDataCleanerFileFormat
			{
			public:
				static const std::string PARAMETER_CALENDAR_ID;
				
			private:

				//! @name Parameters
				//@{
				//@}

					class ExpatParser
					{
					public:
						ExpatParser(
							const impex::DataSource& dataSource,
							util::Env& env,
							std::ostream& stream,
							const calendar::Calendar& calendar
						);
						virtual ~ExpatParser();
						
						virtual void parse(std::istream &data) throw(std::runtime_error);

					private:
						static void startElement(void *d, const XML_Char* name, const XML_Char** attrs) throw(Exception);
						static void endElement(void *d, const XML_Char* name) throw(Exception);
						static void characters(void*, const XML_Char* txt, int txtlen);
						
						struct expat_user_data
						{
							const calendar::Calendar& _calendar;
							const impex::DataSource& _dataSource;
							util::Env& _env;
							std::ostream& _stream;
							std::string curTag;
							CommercialLine* line;
							bool importTrip;
							std::string tripNumber;
							JourneyPattern::StopsWithDepartureArrivalAuthorization tripStops;
							SchedulesBasedService::Schedules departureSchedules;
							SchedulesBasedService::Schedules arrivalSchedules;
							boost::posix_time::time_duration arrivalSchedule;
							impex::ImportableTableSync::ObjectBySource<StopPointTableSync> stopPoints;

							expat_user_data(
								const impex::DataSource& dataSource,
								util::Env& env,
								std::ostream& stream,
								const calendar::Calendar& calendar
							);
						};
						
						expat_user_data user_data;
						
						typedef std::map<std::string, std::string> AttributeMap;
						static AttributeMap createAttributeMap(const XML_Char **attrs);
					};


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
				/// @since 3.2.0
				/// @date 2010
				virtual void displayAdmin(
					std::ostream& os,
					const admin::AdminRequest& request
				) const;



				virtual db::DBTransaction _save() const;
			};

			typedef impex::NoExportPolicy<OGTFileFormat> Exporter_;
		};
	}
}

#endif
