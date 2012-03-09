
/** DumpFileFormat class header.
	@file DumpFileFormat.hpp

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


#ifndef SYNTHESE_IMPEX_DumpFileFormat_H
#define SYNTHESE_IMPEX_DumpFileFormat_H

#include "FileFormatTemplate.h"
#include "OneFileTypeImporter.hpp"
#include "OneFileExporter.hpp"

#include <iostream>
#include <vector>

namespace synthese
{
	namespace util
	{
		class Registrable;
	}

	class ObjectBase;

	namespace impex
	{
		//////////////////////////////////////////////////////////////////////////
		/// Dump file format.
		/// @ingroup m16File refFile
		///
		///  - Extraction and import of data at a generic SQL dump format.
		///  - SVN backend
		///	 - Inter-SYNTHESE service
		class DumpFileFormat:
			public impex::FileFormatTemplate<DumpFileFormat>
		{
		public:
			class Importer_:
				public impex::OneFileTypeImporter<Importer_>
			{
			private:
				ObjectBase* _readFileSystem(
					const boost::filesystem::path filePath
				) const;


			public:

				Importer_(
					util::Env& env,
					const impex::DataSource& dataSource
				);



				//////////////////////////////////////////////////////////////////////////
				/// Conversion from attributes to generic parameter maps.
				/// @return Generated parameters map
				/// @author Hugues Romain
				/// @date 2010
				/// @since 3.1.16
				virtual util::ParametersMap _getParametersMap() const;



				//////////////////////////////////////////////////////////////////////////
				/// Conversion from generic parameters map to attributes.
				/// @param map Parameters map to interpret
				/// @author Hugues Romain
				/// @date 2010
				/// @since 3.1.16
				virtual void _setFromParametersMap(const util::ParametersMap& map);



				//////////////////////////////////////////////////////////////////////////
				/// Dump import.
				/// @param filePath path of the file to import (the dump file)
				/// @param os stream to write information messages on
				/// @author Hugues Romain
				///
				virtual bool _parse(
					const boost::filesystem::path& filePath,
					std::ostream& os,
					boost::optional<const admin::AdminRequest&> adminRequest
				) const;

				virtual db::DBTransaction _save() const;



				//////////////////////////////////////////////////////////////////////////
				/// Import screen to include in the administration console.
				/// @param os stream to write the result on
				/// @param request request for display of the administration console
				/// @author Hugues Romain
				/// @since 3.2.0
				/// @date 2010
				virtual void displayAdmin(
					std::ostream& os,
					const admin::AdminRequest& request
				) const;

			};

			class Exporter_:
				public impex::OneFileExporter<DumpFileFormat>
			{
			public:
				static const std::string PARAMETER_OBJECT_ID;
				static const std::string PARAMETER_PATH;

			private:
				//! @name Export parameters
				//@{
					boost::shared_ptr<const ObjectBase> _object;
					boost::filesystem::path _path;
				//@}

					void _dumpToFileSystem(
						const ObjectBase& object,
						const boost::filesystem::path directory
					) const;

			public:
				Exporter_()
				{}

				//! @name Setters
				//@{
					void setObject(boost::shared_ptr<const ObjectBase> value){ _object = value; }
				//@}

				virtual util::ParametersMap getParametersMap() const;

				virtual void setFromParametersMap(const util::ParametersMap& map);

				/** 
				 */
				virtual void build(std::ostream& os) const;

				virtual std::string getOutputMimeType() const { return "text/plain"; }
			};

			friend class Importer_;
			friend class Exporter_;
		};
	}
}

#endif
