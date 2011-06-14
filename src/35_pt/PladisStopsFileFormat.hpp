/** PladisStopsFileFormat class header.
	@file PladisStopsFileFormat.hpp

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

#ifndef SYNTHESE_PladisStopsFileFormat_H__
#define SYNTHESE_PladisStopsFileFormat_H__

#include "FileFormatTemplate.h"
#include "MultipleFileTypesImporter.hpp"
#include "NoExportPolicy.hpp"

#include <iostream>
#include <map>
#include <string>
#include <vector>
#include <boost/date_time/gregorian/gregorian.hpp>

namespace geos
{
	namespace geom
	{
		class Point;
	}
}

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
		class StopPoint;

		//////////////////////////////////////////////////////////////////////////
		/// Pladis (CarPostal) file format for stops integration.
		/// @ingroup m35File refFile
		//////////////////////////////////////////////////////////////////////////
		/// The stops part of Pladis export uses 2 files :
		///	<ul>
		///		<li>BAHNHOF.DAT : Names of commercial stops</li>
		///		<li>KOORD.DAT : Coordinates of commercial stops</li>
		///	</ul>
		///
		/// <h2>Import</h2>
		///
		///	Import of the stops : there is no automated import, because of the need of merging
		///	the data with other sources. This work can only be done manually. A @ref PTStopsImportWizardAdmin "special admin page"
		///	can read such BAHNHOF.DAT and KOORD.DAT and show differences with SYNTHESE.</li>
		///
		/// The physical stops must be linked with the items of BAHNHOF.DAT.
		/// More than one physical stop can be linked with the same CarPostal stop. In this case,
		/// the import will select automatically the actual stop regarding the whole itinerary.
		class PladisStopsFileFormat:
			public impex::FileFormatTemplate<PladisStopsFileFormat>
		{
		public:

			class Importer_:
				public impex::MultipleFileTypesImporter<PladisStopsFileFormat>
			{
			public:
				static const std::string FILE_BAHNHOFS;
				static const std::string FILE_KOORDS;

			private:
				struct Bahnhof
				{
					std::string operatorCode;
					std::string cityName;
					std::string name;
					boost::shared_ptr<geos::geom::Point> coords;
					boost::shared_ptr<geos::geom::Point> projected;
					boost::shared_ptr<StopPoint> stop;
				};

				typedef std::map<std::string, Bahnhof> Bahnhofs;

				mutable Bahnhofs _nonLinkedBahnhofs;
				mutable Bahnhofs _linkedBahnhofs;

			protected:

				virtual bool _controlPathsMap() const;

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
				):	impex::MultipleFileTypesImporter<PladisStopsFileFormat>(env, dataSource),
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

			typedef impex::NoExportPolicy<PladisStopsFileFormat> Exporter_;
		};
}	}

#endif
