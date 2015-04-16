
/** LinkException class header.
	@file LinkException.h

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

#ifndef SYNTHESE_db_LinkException_h__
#define SYNTHESE_db_LinkException_h__

#include "Exception.h"

namespace synthese
{
	namespace db
	{
		/** LinkException class template.
			A link exception is thrown by a table sync loader when an inconsistent object is loaded due to
			bad link informations.
			Examples :
			 - an id points to an inexistent object, whereas the link is compulsory
			 - an id points to an existing object, but the load of the object has failed
			 - etc.

			Template class C : Class of the table sync of the current object
			@ingroup m10Exceptions refExceptions
		*/
		template<class C>
		class LinkException : public synthese::Exception
		{
		public:
			LinkException(
				const db::DBResultSPtr& row,
				const std::string& field,
				const Exception& e
			):	Exception(
					"There was a link error in "+ C::TABLE.NAME +
					" table at row "+ row->getText(TABLE_COL_ID) +
					" when loading object "+ row->getText(field) +
					" at field "+ field +
					" : "+ e.getMessage()
				)
			{}
		};
	}
}

#endif // SYNTHESE_db_LinkException_h__
