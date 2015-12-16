
/** InterSYNTHESEPackageFileFormat class header.
	@file InterSYNTHESEPackageFileFormat.hpp

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

#ifndef SYNTHESE_InterSYNTHESEPackageFileFormat_H__
#define SYNTHESE_InterSYNTHESEPackageFileFormat_H__

#include "FileFormatTemplate.h"
#include "ConnectionImporter.hpp"
#include "NoExportPolicy.hpp"

#include <iostream>
#include <map>
#include <string>
#include <vector>

namespace synthese
{
	namespace inter_synthese
	{
		class InterSYNTHESEPackageContent;

		//////////////////////////////////////////////////////////////////////////
		/// Inter-SYNTHESE package file format.
		//////////////////////////////////////////////////////////////////////////
		/// @ingroup m19File refFile
		class InterSYNTHESEPackageFileFormat:
			public impex::FileFormatTemplate<InterSYNTHESEPackageFileFormat>
		{
		public:

			virtual ~InterSYNTHESEPackageFileFormat() {}

			//////////////////////////////////////////////////////////////////////////
			class Importer_:
				public impex::ConnectionImporter<InterSYNTHESEPackageFileFormat>
			{
			public:
				static const std::string PARAMETER_URL;
				static const std::string PARAMETER_USER;
				static const std::string PARAMETER_PASSWORD;
				static const std::string PARAMETER_LOCK;
				static const std::string PARAMETER_LOCK_SERVER_NAME;
				static const std::string PARAMETER_FILE_PATH;
				static const std::string PARAMETER_NO_SUPPRESS_TOP_LEVEL;
				static const std::string PARAMETER_NO_SUPPRESS_ANYTHING;

			private:
				std::string _smartURL;
				std::string _user;
				std::string _password;
				bool _lock;
				std::string _lockServerName;
				std::string _filePath;
				mutable std::auto_ptr<InterSYNTHESEPackageContent> _content;
				bool _noSuppressTopLevel;
				bool _noSuppressAnything;

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
				virtual ~Importer_() {}

				const std::string& getSmartURL() const { return _smartURL; }
				const std::string& getUser() const { return _user; }
				const std::string& getPassword() const { return _password; }



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

			typedef impex::NoExportPolicy<InterSYNTHESEPackageFileFormat> Exporter_;
		};
	}
}

#endif
