/** HeuresStopsFileFormat class header.
	@file HeuresStopsFileFormat.hpp

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

#ifndef SYNTHESE_HeuresStopsFileFormat_H__
#define SYNTHESE_HeuresStopsFileFormat_H__

#include "FileFormatTemplate.h"
#include "OneFileTypeImporter.hpp"
#include "NoExportPolicy.hpp"

#include <iostream>
#include <map>
#include <string>
#include <vector>

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
		/// Heures (Lumiplan) file format for stops integration.
		/// @ingroup m35File refFile
		//////////////////////////////////////////////////////////////////////////
		/// The stops part of Heures export uses 1 file :
		///	<ul>
		///		<li>pointsarrets.tmp : List of physical stops</li>
		///	</ul>
		///
		/// <h2>Import</h2>
		///
		///	Import of the stops : there is no automated import, because of the need of merging
		///	the data with other sources. This work can only be done manually. A @ref PTStopsImportWizardAdmin "special admin page"
		///	can read pointsarrets.tmp and show missing stops in SYNTHESE.</li>
		class HeuresStopsFileFormat:
			public impex::FileFormatTemplate<HeuresStopsFileFormat>
		{
		public:

			class Importer_:
				public impex::OneFileTypeImporter<HeuresStopsFileFormat>
			{
				struct PointArret
				{
					std::string operatorCode;
					std::string name;
					boost::shared_ptr<StopPoint> stop;

					PointArret() {}
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
				):	impex::OneFileTypeImporter<HeuresStopsFileFormat>(env, dataSource)
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

			typedef impex::NoExportPolicy<HeuresStopsFileFormat> Exporter_;
		};
}	}

#endif
