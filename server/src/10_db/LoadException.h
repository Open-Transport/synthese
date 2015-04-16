
/** LoadException class header.
	@file LoadException.h

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

#ifndef SYNTHESE_db_LoadException_h__
#define SYNTHESE_db_LoadException_h__

#include "Exception.h"

#include <boost/lexical_cast.hpp>

namespace synthese
{
	namespace db
	{
		/** LoadException class template.
				- class C : Class of the table sync of the current object
			@ingroup m10Exceptions refExceptions
		*/
		template<class C>
		class LoadException:
			public synthese::Exception
		{
		public:
			LoadException(
				const db::DBResultSPtr& row,
				const std::string& field,
				const std::string& text
			):	Exception(
					"There was a load error in "+ C::TABLE.NAME +
					" table at row "+ boost::lexical_cast<std::string>(row->getLongLong(TABLE_COL_ID)) +
					" in field "+ field +" : "+ text
				)
			{}
		};
	}
}

#endif // SYNTHESE_db_LinkException_h__
