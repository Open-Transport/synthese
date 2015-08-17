
/** CalendarOGTFileFormat class header.
	@file CalendarOGTFileFormat.hpp

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

#ifndef SYNTHESE_CalendarOGTFileFormat_H__
#define SYNTHESE_CalendarOGTFileFormat_H__

#include <map>
#include "FileFormatTemplate.h"
#include "NoExportPolicy.hpp"
#include "OneFileTypeImporter.hpp"
#include "CalendarFileFormat.hpp"
#include "OGTFileFormat.hpp"
#include "CalendarTemplate.h"
#include "CalendarTemplateElement.h"
#include "ImportableTableSync.hpp"

namespace synthese
{
	namespace data_exchange
	{
		//////////////////////////////////////////////////////////////////////////
		/// Calendar+OGT file format.
		//////////////////////////////////////////////////////////////////////////
		/// @ingroup m35File refFile
		class CalendarOGTFileFormat:
			public impex::FileFormatTemplate<CalendarOGTFileFormat>
		{
		public:

			//////////////////////////////////////////////////////////////////////////
			class Importer_:
				public impex::OneFileTypeImporter<CalendarOGTFileFormat>,
				public CalendarFileFormat
			{
			private:
				static const std::string SEP;

				typedef std::vector<std::string> DayList;
				typedef std::map<std::string, DayList> CalendarDatesMap;

				mutable OGTFileFormat::Importer_ _ogtImporter;
				mutable impex::ImportableTableSync::ObjectBySource<calendar::CalendarTemplateTableSync> _calendarTemplates;

			protected:
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

				virtual bool afterParsing();

				//////////////////////////////////////////////////////////////////////////
				/// Conversion from attributes to generic parameter maps.
				/// @return Generated parameters map
				virtual util::ParametersMap _getParametersMap() const;

				//////////////////////////////////////////////////////////////////////////
				/// Conversion from generic parameters map to attributes.
				/// @param map Parameters map to interpret
				virtual void _setFromParametersMap(const util::ParametersMap& map);

				virtual db::DBTransaction _save() const;
			};

			typedef impex::NoExportPolicy<CalendarOGTFileFormat> Exporter_;
		};
}	}

#endif
