
/** ImportableAdmin class header.
	@file ImportableAdmin.hpp

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

#ifndef SYNTHESE_impex_ImportableAdmin_hpp__
#define SYNTHESE_impex_ImportableAdmin_hpp__

#include <string>

namespace synthese
{
	namespace server
	{
		class Request;
	}

	namespace impex
	{
		class Importable;

		//////////////////////////////////////////////////////////////////////////
		/// Helper for Importable admin screens.
		///	@ingroup m16
		class ImportableAdmin
		{
		public:
			static const std::string TAB_DATA_SOURCES;

			static const std::string PARAMETER_DATA_SOURCE_LINKS;

			static void DisplayDataSourcesTab(
				std::ostream& stream,
				const Importable& object,
				const server::Request& updateRequest
			);
		};
	}
}

#endif // SYNTHESE_impex_ImportableAdmin_hpp__
