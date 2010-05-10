
/** SQLiteLazyResult class implementation.
	@file SQLiteLazyResult.cpp

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

#include "SQLiteLazyResult.h"
#include "SQLiteException.h"

#include <iomanip>
#include <boost/date_time/posix_time/time_formatters.hpp>
#include <boost/date_time/posix_time/time_parsers.hpp>
#include <boost/lexical_cast.hpp>

using namespace std;
using namespace boost;
using namespace boost::posix_time;

namespace synthese
{
	using namespace util;
	
	namespace db
	{
		SQLiteLazyResult::SQLiteLazyResult (SQLiteStatementSPtr statement)
			: _statement (statement)
			, _pos (-1) 
		{
			// std::cerr << "new lazy " << _statement->getSQL () << std::endl;
		}



		SQLiteLazyResult::~SQLiteLazyResult ()
		{
			// std::cerr << "del lazy " << _statement->getSQL () << std::endl;
		}



		void 
		SQLiteLazyResult::reset () const
		{
			int retc = sqlite3_reset (_statement->getStatement ());
			if (retc == SQLITE_OK) 
			{
				_pos = -1;
			}
			else
			{
				throw SQLiteException ("Error while resetting statement (error=" 
						   + lexical_cast<string>(retc) + ")");
			}
		    
		}



		bool 
		SQLiteLazyResult::next () const
		{
			int retc = sqlite3_step (_statement->getStatement ());
			if (retc == SQLITE_ROW) 
			{
				++_pos;
				return true;
			}
			else if (retc == SQLITE_DONE)
			{
				++_pos;
				return false;
			}
			else
			{
				throw SQLiteException ("Error while stepping through statement (error=" 
						   + lexical_cast<string>(retc) + ")");
			}
		}




		int SQLiteLazyResult::getNbColumns () const
		{
			return sqlite3_column_count (_statement->getStatement ());
		}



		string SQLiteLazyResult::getColumnName (int column) const
		{
			return std::string (sqlite3_column_name (_statement->getStatement (), column));
		}



		SQLiteValue SQLiteLazyResult::getValue (int column) const
		{
			if (_pos == -1) throw SQLiteException ("Not inside result (use next ())...");
			return SQLiteValue(sqlite3_column_value (_statement->getStatement (), column));
		}



		boost::shared_ptr<SQLiteValue> SQLiteLazyResult::getValueSPtr( int column ) const
		{
			if (_pos == -1) throw SQLiteException ("Not inside result (use next ())...");
			return shared_ptr<SQLiteValue>(new SQLiteValue(sqlite3_column_value (_statement->getStatement (), column)));
		}
	}
}
