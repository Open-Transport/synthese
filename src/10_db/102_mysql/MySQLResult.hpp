
/** MySQLResult class header.
	@file MySQLResult.hpp
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

#ifndef SYNTHESE_db_mysql_MySQLResult_h__
#define SYNTHESE_db_mysql_MySQLResult_h__

#include "DBResult.hpp"
#include "DB.hpp"
#include "102_mysql/MySQLException.hpp"

struct st_mysql;
typedef struct st_mysql MYSQL;
struct st_mysql_res;
typedef struct st_mysql_res MYSQL_RES;
typedef char **MYSQL_ROW;

namespace synthese
{
	namespace db
	{

		//////////////////////////////////////////////////////////////////////////
		/// Encapsulates a MySQL database result.
		///
		/// @author Sylvain Pasche
		/// @date 2011
		//////////////////////////////////////////////////////////////////////////
		class MySQLResult : public DBResult
		{
		private:

			MySQLDB* _db;
			MYSQL_RES* _result;
			mutable MYSQL_ROW _row;
			mutable unsigned long* _lengths;

			template<class T, T DEFAULT_VALUE>
			T _getValue(int column) const;

		public:

			MySQLResult(
				MySQLDB* db,
				const SQLData& sql
			);
			virtual ~MySQLResult();

			//! @name Query methods.
			//@{
			virtual void reset() const;
			virtual bool next() const;

			virtual int getNbColumns() const;

			virtual std::string getColumnName(int column) const;

			//virtual DBValue getValue(int column) const;
			virtual std::string getText(int column) const;
			virtual int getInt(int column) const;
			virtual long long getLongLong(int column) const;
			virtual double getDouble(int column) const;
			//@}

		};
	}
}


#endif // SYNTHESE_db_mysql_MySQLResult_h__
