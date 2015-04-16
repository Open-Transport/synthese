
/** MySQLDB class implementation.
	@file MySQLDB.cpp
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

#include "102_mysql/MySQLDB.hpp"
#include "102_mysql/MySQLResult.hpp"
#include "MySQLException.hpp"

#include <boost/lexical_cast.hpp>
#include <my_global.h>
#include <mysql.h>

using namespace std;
using boost::lexical_cast;

namespace synthese
{
	namespace db
	{
		MySQLResult::MySQLResult(
			MySQLDB* db,
			const SQLData& sql
		) :
			_db(db),

			_result(NULL),
			_row(NULL)
		{
			boost::recursive_mutex::scoped_lock lock(_db->_connectionMutex);

			_db->_doQuery(sql);

			_result = mysql_store_result(_db->_connection);
			if (!_result)
			{
				_db->_throwException("MySQL error in mysql_store_result()");
			}
		}



		MySQLResult::~MySQLResult()
		{
			if (_result)
				mysql_free_result(_result);

			boost::recursive_mutex::scoped_lock lock(_db->_connectionMutex);

			// Consume remaining results in case of a multi statement query.
			// TODO: the remainding queries should also be available through the API.
			int status;
			while (mysql_more_results(_db->_connection))
			{
				if ((status = mysql_next_result(_db->_connection)) == -1)
				{
					break;
				}
				if (status > 0)
				{
					_db->_throwException("Could not execute statement");
				}
				MYSQL_RES *res = mysql_store_result(_db->_connection);
				if (res)
				{
					mysql_free_result(res);
				}
				else
				{
					_db->_throwException("MySQL error in mysql_store_result()");
				}
			}
		}



		template<class T, T DEFAULT_VALUE>
		T MySQLResult::_getValue(int column) const
		{
			ensurePosition();
			if (!_row[column])
				return DEFAULT_VALUE;
			return lexical_cast<T>(_row[column]);
		}



		void MySQLResult::reset() const
		{
			mysql_data_seek(_result, 0);
			resetPosition();
		};



		bool MySQLResult::next() const
		{
			_row = mysql_fetch_row(_result);
			_lengths = mysql_fetch_lengths(_result);
			incrementPosition();
			return _row != NULL;
		}



		int MySQLResult::getNbColumns() const
		{
			assert(_result);
			return mysql_num_fields(_result);
		}



		std::string MySQLResult::getColumnName(int column) const
		{
			assert(_result);
			return mysql_fetch_field_direct(_result, column)->name;
		}



		std::string MySQLResult::getText(int column) const
		{
			ensurePosition();
			// TODO: what if result is not a string? Maybe convert with lexical_cast.
			const char* text = _row[column];
			if (!text)
			{
				return string();
			}
			return std::string(text, _lengths[column]);
		}



		int MySQLResult::getInt(int column) const
		{
			return _getValue<int, 0>(column);
		}



		long long MySQLResult::getLongLong(int column) const
		{
			return _getValue<long long, 0>(column);
		}



		double MySQLResult::getDouble(int column) const
		{
			// Can't use _getValue, gcc dislikes having a double as template parameter.
			ensurePosition();
			if (!_row[column])
				return 0.0;
			return lexical_cast<double>(_row[column]);
		}
	}
}
