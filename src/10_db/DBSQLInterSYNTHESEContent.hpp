
/** DBSQLInterSYNTHESEContent class header.
	@file DBSQLInterSYNTHESEContent.hpp

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

#ifndef SYNTHESE_db_DBSQLInterSYNTHESEContent_hpp__
#define SYNTHESE_db_DBSQLInterSYNTHESEContent_hpp__

#include "InterSYNTHESEContent.hpp"

#include "UtilTypes.h"

namespace synthese
{
	namespace db
	{
		/** DBSQLInterSYNTHESEContent class.
			@ingroup m10
		*/
		class DBSQLInterSYNTHESEContent:
			public inter_synthese::InterSYNTHESEContent
		{
			util::RegistryTableType _tableId;
			std::string _sql;

		public:
			DBSQLInterSYNTHESEContent(
				util::RegistryTableType tableId,
				const std::string& sql
			);

			virtual std::string getPerimeter() const;
			virtual std::string getContent() const;
		};
	}
}

#endif // SYNTHESE_db_DBSQLInterSYNTHESEContent_hpp__
