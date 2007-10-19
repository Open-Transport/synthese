
/** LinkException class header.
	@file LinkException.h

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

#ifndef SYNTHESE_db_LinkException_h__
#define SYNTHESE_db_LinkException_h__

#include "01_util/Exception.h"
#include "01_util/UId.h"
#include "01_util/Conversion.h"

namespace synthese
{
	namespace db
	{
		/** LinkException class template.
				- class C : Class of the table sync of the current object
			@ingroup m10Exceptions refExceptions
		*/
		template<class C>
		class LinkException : public util::Exception
		{
		public:
			LinkException(uid currentId, const std::string& field, const util::Exception& e)
				: Exception("There was an error in "+ C::TABLE_NAME +" table at row "+ Conversion::ToString(currentId) +" in field "+ field +" : "+ e.getMessage())
			{	}

		};
	}
}

#endif // SYNTHESE_db_LinkException_h__
