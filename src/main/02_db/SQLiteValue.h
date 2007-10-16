
/** SQLiteValue class header.
	@file SQLiteValue.h

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


#ifndef SYNTHESE_DB_SQLITEVALUE_H
#define SYNTHESE_DB_SQLITEVALUE_H


#include "02_db/SQLiteValue.h"
#include <sqlite3.h>


#include <boost/date_time/posix_time/posix_time_types.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>

#include <boost/logic/tribool.hpp>


namespace synthese
{

namespace db
{

    

/** Interface for access to an SQLite value.

 @ingroup m02
*/
class SQLiteValue
{
 public:

 private:

    std::string _value;  

 public:

    SQLiteValue (sqlite3_value* value); 
    SQLiteValue (const std::string& value); 

    virtual ~SQLiteValue ();

    virtual const std::string& getText () const;
    virtual int getInt () const;
    virtual long getLong () const;
    virtual bool getBool () const;
    virtual boost::logic::tribool getTribool () const;
    virtual double getDouble () const;
    virtual const std::string& getBlob () const;
    virtual long long getLongLong () const;
    virtual boost::posix_time::ptime getTimestamp () const;

  private:


};


}
}


#endif

