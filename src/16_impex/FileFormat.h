/** Import/export file format class header.
	@file FileFormat.h

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

#ifndef SYNTHESE_impex_FileFormat_h__
#define SYNTHESE_impex_FileFormat_h__

#include "FactoryBase.h"

#include "ImpExTypes.hpp"

#include <boost/optional.hpp>
#include <boost/shared_ptr.hpp>

////////////////////////////////////////////////////////////////////
/// @defgroup refFile 16 File formats
///	@ingroup ref

namespace synthese
{
	namespace util
	{
		class Env;
		class ParametersMap;
	}

	namespace impex
	{
		class Import;
		class Importer;
		class Exporter;

		class FileFormat:
			public util::FactoryBase<FileFormat>
		{
		public:
			static const std::string ATTR_KEY;
			static const std::string ATTR_CAN_IMPORT;
			static const std::string ATTR_CAN_EXPORT;

			virtual bool canImport() const = 0;
			virtual bool canExport() const = 0;

			virtual boost::shared_ptr<Importer> getImporter(
				util::Env& env,
				const Import& import,
				ImportLogLevel minLogLevel,
				const std::string& logPath,
				boost::optional<std::ostream&> outputStream,
				util::ParametersMap& pm
			) const = 0;

			virtual boost::shared_ptr<Exporter> getExporter(
			) const	= 0;


			void toParametersMap(
				util::ParametersMap& pm
			) const;
		};
}	}

#endif // SYNTHESE_impex_Import_h__
