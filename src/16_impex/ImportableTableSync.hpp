
/** ImportableTableSync class header.
	@file ImportableTableSync.hpp

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

#ifndef SYNTHESE_impex_ImportableTableSync_hpp__
#define SYNTHESE_impex_ImportableTableSync_hpp__

#include "Importable.h"

#include <string>

namespace synthese
{
	namespace util
	{
		class Env;
	}

	namespace impex
	{
		//////////////////////////////////////////////////////////////////////////
		/// Table sync helpers for Importable child classes.
		///	@ingroup m16
		/// @author Hugues Romain
		/// @date 2010
		/// @since 3.2.1
		class ImportableTableSync
		{
		private:
			static const std::string SOURCES_SEPARATOR;
			static const std::string FIELDS_SEPARATOR;

		public:
			static const std::string COL_DATA_SOURCE_LINKS;


			//////////////////////////////////////////////////////////////////////////
			/// Builds storage string.
			/// Each couple datasource/id is separated by , . The source and the id are
			/// separated by | . If a datasource does not define any id then the , is
			/// omitted
			//////////////////////////////////////////////////////////////////////////
			/// @param object the object to serialize
			/// @return the serialized string corresponding to the object
			/// @author Hugues Romain
			/// @since 3.2.1
			/// @date 2010
			static std::string SerializeDataSourceLinks(
				const Importable::DataSourceLinks& object
			);



			//////////////////////////////////////////////////////////////////////////
			/// Reads storage string.
			//////////////////////////////////////////////////////////////////////////
			/// @param object the object to update
			/// @param serializedString string to read
			/// @param env environment to populate when loading data sources
			/// @author Hugues Romain
			/// @since 3.2.1
			/// @date 2010
			static Importable::DataSourceLinks GetDataSourceLinksFromSerializedString(
				const std::string& serializedString,
				util::Env& env
			);
		};
	}
}

#endif // SYNTHESE_impex_ImportableTableSync_hpp__
