
/** IneoDepartureBoardsFileFormat class header.
	@file IneoDepartureBoardsFileFormat.hpp

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

#ifndef SYNTHESE_IneoDepartureBoardsFileFormat_H__
#define SYNTHESE_IneoDepartureBoardsFileFormat_H__

#include "FileFormatTemplate.h"
#include "DatabaseReadImporter.hpp"
#include "NoExportPolicy.hpp"
#include "PTFileFormat.hpp"

namespace synthese
{
	namespace departure_boards
	{
		class DisplayType;
	}

	namespace data_exchange
	{
		//////////////////////////////////////////////////////////////////////////
		/// Ineo departure boards file format.
		//////////////////////////////////////////////////////////////////////////
		/// @ingroup m35File refFile
		class IneoDepartureBoardsFileFormat:
			public impex::FileFormatTemplate<IneoDepartureBoardsFileFormat>
		{
		public:

			//////////////////////////////////////////////////////////////////////////
			class Importer_:
				public impex::DatabaseReadImporter<IneoDepartureBoardsFileFormat>,
				public data_exchange::PTFileFormat
			{
			public:
				static const std::string PARAMETER_PT_DATASOURCE_ID;
				static const std::string PARAMETER_DEFAULT_DISPLAY_TYPE_ID;

			private:
				//! @name Parameters
				//@{
					boost::shared_ptr<const impex::DataSource> _ptDataSource;
					boost::shared_ptr<const departure_boards::DisplayType> _defaultDisplayType;
				//@}

			protected:

				virtual bool _read(
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

			typedef impex::NoExportPolicy<IneoDepartureBoardsFileFormat> Exporter_;
		};
	}
}

#endif
