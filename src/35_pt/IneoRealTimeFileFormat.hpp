
/** IneoRealTimeFileFormat class header.
	@file IneoRealTimeFileFormat.hpp

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

#ifndef SYNTHESE_IneoRealTimeFileFormat_H__
#define SYNTHESE_IneoRealTimeFileFormat_H__

#include "FileFormatTemplate.h"
#include "DatabaseReadImporter.hpp"
#include "NoExportPolicy.hpp"

namespace synthese
{
	namespace pt
	{
		class ScheduledService;

		//////////////////////////////////////////////////////////////////////////
		/// Ineo real time file format.
		//////////////////////////////////////////////////////////////////////////
		/// @ingroup m35File refFile
		class IneoRealTimeFileFormat:
			public impex::FileFormatTemplate<IneoRealTimeFileFormat>
		{
		public:

			//////////////////////////////////////////////////////////////////////////
			class Importer_:
				public impex::DatabaseReadImporter<IneoRealTimeFileFormat>
			{
			public:
				static const std::string PARAMETER_PLANNED_DATASOURCE_ID;
				static const std::string PARAMETER_COURSE_ID;
				static const std::string PARAMETER_DB_CONN_STRING;	//!< Connection string for accessing the real-time db.
				static const std::string PARAMETER_STOP_CODE_PREFIX;

			private:
				//! @name Parameters
				//@{
					boost::shared_ptr<const impex::DataSource> _plannedDataSource;
					boost::optional<std::string> _courseId;
					mutable std::set<ScheduledService*> _services;
					boost::optional<std::string> _dbConnString;
					std::string _stopCodePrefix;
				//@}

			protected:

				virtual bool _read(
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

			typedef impex::NoExportPolicy<IneoRealTimeFileFormat> Exporter_;
		};
	}
}

#endif
