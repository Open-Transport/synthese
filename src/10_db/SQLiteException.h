////////////////////////////////////////////////////////////////////////////////
/// SQLiteException class header.
///	@file SQLiteException.h
///	@author Marc Jambert
///
///	This file belongs to the SYNTHESE project (public transportation specialized
///	software)
///	Copyright (C) 2002 Hugues Romain - RCS <contact@reseaux-conseil.com>
///
///	This program is free software; you can redistribute it and/or
///	modify it under the terms of the GNU General Public License
///	as published by the Free Software Foundation; either version 2
///	of the License, or (at your option) any later version.
///
///	This program is distributed in the hope that it will be useful,
///	but WITHOUT ANY WARRANTY; without even the implied warranty of
///	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
///	GNU General Public License for more details.
///
///	You should have received a copy of the GNU General Public License
///	along with this program; if not, write to the Free Software Foundation,
///	Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
////////////////////////////////////////////////////////////////////////////////

#ifndef SYNTHESE_DB_SQLITEEXCEPTION_H
#define SYNTHESE_DB_SQLITEEXCEPTION_H


#include "Exception.h"

#include <string>
#include <iostream>


namespace synthese
{
namespace db
{


/** SQLite related exception class.
	@ingroup m10Exceptions refExceptions

	This exception occurs when the execution of a query generates an error :
		- SQL syntax error
		- SQLite internal error
		- etc.

	An empty result after a select query is not a SQLiteException error (use TableSyncEmptyResultError instead)
*/

class SQLiteException : public synthese::Exception
{
 private:

 public:

    SQLiteException ( const std::string& message);
    ~SQLiteException () throw ();

 private:


};




}

}
#endif

