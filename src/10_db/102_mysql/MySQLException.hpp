
/** MySQLException class header.
	@file MySQLException.hpp
	@author Sylvain Pasche

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

#ifndef SYNTHESE_db_mysql_MySQLException_hpp__
#define SYNTHESE_db_mysql_MySQLException_hpp__

#include "10_db/DBException.hpp"

#include <string>

namespace synthese
{
	namespace db
	{
		//////////////////////////////////////////////////////////////////////////
		/// Exception class for MySQL specific database errors.
		///
		/// @author Sylvain Pasche
		/// @date 2011
		//////////////////////////////////////////////////////////////////////////
		class MySQLException : public DBException
		{
		private:

			unsigned int _errno;

		public:

			MySQLException(const std::string& message, unsigned int mysql_errno = 0);
			~MySQLException() throw ();

			unsigned int getErrno() const;
		};
	}
}
#endif // SYNTHESE_db_mysql_MySQLException_hpp__
