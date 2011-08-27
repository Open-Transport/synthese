
/** CarPostalFileFormat class header.
	@file CarPostalFileFormat.hpp

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

#ifndef SYNTHESE_CarPostalFileFormat_H__
#define SYNTHESE_CarPostalFileFormat_H__

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
		/// CarPostal file format.
		/// @ingroup m35File refFile
		//////////////////////////////////////////////////////////////////////////
		/// The CarPostal export uses 5 files :
		///	<ul>
		///		<li>BAHNHOF.DAT : Names of commercial stops</li>
		///		<li>KOORD.DAT : Coordinates of commercial stops</li>
		///		<li>ECKDATEN.DAT : Date range of the data</li>
		///		<li>BITFELD.DAT : Calendars</li>
		///		<li>ZUGUDAT.DAT : Routes and service schedules</li>
		///	</ul>
		///
		/// <h2>Import</h2>
		///
		/// The import is separated into two parts :
		///	<ul>
		///		<li>Import of the stops : there is no automated import, because of the need of merging
		///		the data with other sources. This work can only be done manually. A @ref PTStopsImportWizardAdmin "special admin page"
		///		can read such BAHNHOF.DAT and KOORD.DAT and show differences with SYNTHESE.</li>
		///		<li>Import of the services : the import is automated, and is possible only if all stops
		///		are referenced in the SYNTHESE database, and if the line has been manually created in
		///		the SYNTHESE database and linked to the CarPostal database by the operator_code field</li>
		///	</ul>
		///
		///	<h3>Import of the stops</h3>
		///
		/// The physical stops must be linked with the items of BAHNHOF.DAT.
		/// More than one physical stop can be linked with the same CarPostal stop. In this case,
		/// the import will select automatically the actual stop regarding the whole itinerary.
		///
		/// <h3>Import of the services</h3>
		///
		/// The import follows these steps :
		///	<ol>
		///		<li>Load of the date ranges from ECKDATEN.DAT</li>
		///		<li>Load of the bits from BITFELD.DAT</li>
		///		<li>Load of the services from ZUGDAT.DAT. For each service :</li>
		///		<ul>
		///			<li>Fetch of the line by its creator code</li>
		///			<li>Load of the route</li>
		///			<li>Comparison with all existing routes</li>
		///			<li>Creation of a route if necessary</li>
		///			<li>Comparison with all existing services</li>
		///			<li>Creation of a service if ncessary</li>
		///		</ul>
		///	</ol>
		class CarPostalFileFormat:
			public impex::FileFormatTemplate<CarPostalFileFormat>
		{
		public:

			//////////////////////////////////////////////////////////////////////////
			class Importer_:
				public impex::MultipleFileTypesImporter<CarPostalFileFormat>
			{

			public:
				static const std::string FILE_ECKDATEN;
				static const std::string FILE_BITFELD;
				static const std::string FILE_ZUGUDAT;

			private:
				typedef std::map<int, calendar::Calendar> CalendarMap;

				mutable boost::gregorian::date _startDate;
				mutable boost::gregorian::date _endDate;
				mutable CalendarMap _calendarMap;


			protected:

				virtual bool _checkPathsMap() const;

				virtual bool _parse(
					const boost::filesystem::path& filePath,
					std::ostream& os,
					const std::string& key,
					boost::optional<const admin::AdminRequest&> adminRequest
				) const;



			public:
				Importer_(
					util::Env& env,
					const impex::DataSource& dataSource
				):	impex::MultipleFileTypesImporter<CarPostalFileFormat>(env, dataSource),
					impex::Importer(env, dataSource)
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

				virtual db::DBTransaction _save() const;
			};

			typedef impex::NoExportPolicy<CarPostalFileFormat> Exporter_;
		};
	}
}

#endif
