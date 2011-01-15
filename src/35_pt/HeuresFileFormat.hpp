
/** HeuresFileFormat class header.
	@file HeuresFileFormat.h

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

#ifndef SYNTHESE_HeuresFileFormat_H__
#define SYNTHESE_HeuresFileFormat_H__

#include "FileFormatTemplate.h"
#include "Calendar.h"
#include "MultipleFileTypesImporter.hpp"
#include "NoExportPolicy.hpp"

#include <iostream>
#include <map>
#include <string>
#include <vector>
#include <boost/date_time/gregorian/gregorian.hpp>

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

		//////////////////////////////////////////////////////////////////////////
		/// Heures file format.
		/// @ingroup m35File refFile
		//////////////////////////////////////////////////////////////////////////
		/// The Heures schedules import uses 3 files :
		///	<ul>
		///		<li>itinerai.tmp : Routes</li>
		///		<li>troncons.tmp : Schedules</li>
		///		<li>services.tmp : Calendars</li>
		///	</ul>
		///
		class HeuresFileFormat:
			public impex::FileFormatTemplate<HeuresFileFormat>
		{
		public:

			//////////////////////////////////////////////////////////////////////////
			class Importer_:
				public impex::MultipleFileTypesImporter<HeuresFileFormat>
			{

			public:
				static const std::string FILE_ITINERAI;
				static const std::string FILE_TRONCONS;
				static const std::string FILE_SERVICES;

				static const std::string PARAMETER_START_DATE;
				static const std::string PARAMETER_END_DATE;

			private:
				
				boost::gregorian::date _startDate;
				boost::gregorian::date _endDate;
				
				

			protected:

				virtual bool _controlPathsMap() const;
				
				virtual bool _parse(
					const boost::filesystem::path& filePath,
					std::ostream& os,
					const std::string& key,
					boost::optional<const admin::AdminRequest&> adminRequest
				) const;

		
			
			public:
				Importer_(const impex::DataSource& dataSource):
					impex::MultipleFileTypesImporter<HeuresFileFormat>(dataSource)
				{}

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

				virtual db::SQLiteTransaction _save() const;
			};

			typedef impex::NoExportPolicy<HeuresFileFormat> Exporter_;
		};
	}
}

#endif
