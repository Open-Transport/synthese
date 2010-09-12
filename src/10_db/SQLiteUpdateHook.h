////////////////////////////////////////////////////////////////////////////////
/// SQLiteUpdateHook class header.
///	@file SQLiteUpdateHook.h
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

#ifndef SYNTHESE_UTIL_SQLITEUPDATEHOOK_H
#define SYNTHESE_UTIL_SQLITEUPDATEHOOK_H


#include "01_util/threads/ThreadExec.h"

#include <vector>
#include <string>
#include <deque>

#include <boost/filesystem/path.hpp>

#include <spatialite/sqlite3.h>


namespace synthese
{


namespace db
{

    class SQLiteHandle;



    typedef 
	struct 
	{
	    int opType;
	    std::string dbName; 
	    std::string tbName; 
	    sqlite_int64 rowId;
	} SQLiteEvent;



/** SQLite update hook interface.

 @ingroup m10
*/
 class SQLiteUpdateHook
{
 public:

 private:

 protected:

    SQLiteUpdateHook () {};
    virtual ~SQLiteUpdateHook () {};

 public:

    /** Method callback on hook registration with the SQLiteHandle object.
     */
    virtual void registerCallback (SQLiteHandle* emitter) = 0;
    
    virtual void eventCallback (SQLiteHandle* emitter,
				const SQLiteEvent& event) = 0;


};



}
}


#endif

