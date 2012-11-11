
/** SQLiteResult class implementation.
	@file SQLiteResult.cpp

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

#include "101_sqlite/SQLiteDB.h"
#include "101_sqlite/SQLiteResult.hpp"
#include "101_sqlite/SQLiteException.hpp"

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
		SQLiteResult::SQLiteResult(sqlite3* handle, sqlite3_stmt* statement, const SQLData& sql) :
			_handle(handle),
			_statement(statement),
			_sql(sql)
		{
		}



		SQLiteResult::~SQLiteResult ()
		{
			sqlite3_finalize(_statement);
		}



		void
		SQLiteResult::reset() const
		{
			int retc = sqlite3_reset(_statement);
			SQLiteDB::_ThrowIfError(_handle, retc, "Error while resetting statement");
			resetPosition();
		}



		bool
		SQLiteResult::next() const
		{
			int retc = sqlite3_step(_statement);
			if (retc == SQLITE_ROW)
			{
				incrementPosition();
				return true;
			}
			else if (retc == SQLITE_DONE)
			{
				incrementPosition();
				return false;
			}
			SQLiteDB::_ThrowIfError(_handle, retc, "Error while stepping through statement");
			return false;
		}




		int SQLiteResult::getNbColumns() const
		{
			return sqlite3_column_count(_statement);
		}



		string SQLiteResult::getColumnName(int column) const
		{
			return std::string(sqlite3_column_name(_statement, column));
		}



		std::string SQLiteResult::getText(int column) const
		{
			ensurePosition();
			sqlite3_value* value = sqlite3_column_value(_statement, column);
			if(!value)
			{
				return string();
			}
			return string(
				(char*) sqlite3_value_blob(value),
				sqlite3_value_bytes(value)
			);
		}



		int SQLiteResult::getInt(int column) const
		{
			ensurePosition();
			return sqlite3_column_int(_statement, column);
		}



		long long SQLiteResult::getLongLong(int column) const
		{
			ensurePosition();
			return sqlite3_column_int64(_statement, column);
		}



		double SQLiteResult::getDouble(int column) const
		{
			return sqlite3_column_double(_statement, column);
		}
	}
}
