
/** SQLiteResult class header.
	@file SQLiteResult.hpp

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

#ifndef SYNTHESE_db_sqlite_SQLiteResult_h__
#define SYNTHESE_db_sqlite_SQLiteResult_h__

#include "DBResult.hpp"


namespace synthese
{

	namespace db
	{
		//////////////////////////////////////////////////////////////////////////
		/// Implementation class for access to a SQLite query result.
		/// @author Marc Jambert
		/// @ingroup m10
		/// @date 2007
		class SQLiteResult:
			public DBResult
		{
		 public:

		 private:
			sqlite3* _handle;
			sqlite3_stmt* _statement;
			const SQLData _sql;

			SQLiteResult(sqlite3* handle, sqlite3_stmt* statement, const SQLData& sql);

		 public:

			~SQLiteResult ();

			//! @name Query methods.
			//@{
				virtual void reset() const;
				virtual bool next() const;

				virtual int getNbColumns() const;
				virtual std::string getColumnName(int column) const;
				virtual std::string getText(int column) const;
				virtual int getInt(int column) const;
				virtual long long getLongLong(int column) const;
				virtual double getDouble(int column) const;
			//@}

		 private:

			friend class SQLiteDB;
		};
	}
}

#endif // SYNTHESE_db_sqlite_SQLiteResult_h__
