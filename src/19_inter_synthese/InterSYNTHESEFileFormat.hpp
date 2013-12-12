
/** InterSYNTHESEFileFormat class header.
	@file InterSYNTHESEFileFormat.hpp

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

#ifndef SYNTHESE_InterSYNTHESEFileFormat_H__
#define SYNTHESE_InterSYNTHESEFileFormat_H__

#include "FileFormatTemplate.h"
#include "ConnectionImporter.hpp"
#include "NoExportPolicy.hpp"

#include <iostream>
#include <map>
#include <string>
#include <vector>
#include <boost/tuple/tuple.hpp>
#include <boost/date_time/gregorian/gregorian.hpp>

namespace synthese
{
	namespace inter_synthese
	{
		class InterSYNTHESEIdFilter;



		//////////////////////////////////////////////////////////////////////////
		/// Inter-SYNTHESE file format.
		//////////////////////////////////////////////////////////////////////////
		/// @ingroup m19File refFile
		class InterSYNTHESEFileFormat:
			public impex::FileFormatTemplate<InterSYNTHESEFileFormat>
		{
		public:

			//////////////////////////////////////////////////////////////////////////
			class Importer_:
				public impex::ConnectionImporter<InterSYNTHESEFileFormat>
			{
			public:
				static const std::string PARAMETER_SLAVE_ID;
				static const std::string PARAMETER_ID_FILTER;

			private:
				util::RegistryKeyType _slaveId;
				boost::shared_ptr<InterSYNTHESEIdFilter> _idFilter;

			protected:


				virtual bool _read() const;


			public:
				Importer_(
					util::Env& env,
					const impex::Import& import,
					impex::ImportLogLevel minLogLevel,
					const std::string& logPath,
					boost::optional<std::ostream&> outputStream,
					util::ParametersMap& pm
				);



				util::RegistryKeyType getSlaveId() const { return _slaveId; }

				//////////////////////////////////////////////////////////////////////////
				/// Conversion from attributes to generic parameter maps.
				/// @return Generated parameters map
				/// @author Hugues Romain
				/// @date 2013
				/// @since 3.6.0
				virtual util::ParametersMap _getParametersMap() const;



				//////////////////////////////////////////////////////////////////////////
				/// Conversion from generic parameters map to attributes.
				/// @param map Parameters map to interpret
				/// @author Hugues Romain
				/// @date 2013
				/// @since 3.6.0
				virtual void _setFromParametersMap(const util::ParametersMap& map);



				virtual db::DBTransaction _save() const;
			};

			typedef impex::NoExportPolicy<InterSYNTHESEFileFormat> Exporter_;
		};
	}
}

#endif
