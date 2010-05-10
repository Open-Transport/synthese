
/** SQLiteValue class implementation.
	@file SQLiteValue.cpp

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

#include "SQLiteValue.h"
#include "SQLiteException.h"
#include "Conversion.h"

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
		SQLiteValue::SQLiteValue (sqlite3_value* value)
			: _value ("")
		{
			int type (sqlite3_value_type (value));
			switch (type)
			{
			case SQLITE_INTEGER:
				_value = lexical_cast<string>(sqlite3_value_int64 (value));
				break;

			case SQLITE_FLOAT:
				_value = lexical_cast<string>(sqlite3_value_double (value));
				break;
	
			case SQLITE_BLOB:
				_value.assign(static_cast<const char*>(sqlite3_value_blob(value)), sqlite3_value_bytes (value));
				break;
		
			case SQLITE_NULL:
				_value = "";
				break;
		
			case SQLITE_TEXT:
				_value.assign((const char*) sqlite3_value_text(value));
				break;
			}
		}



		SQLiteValue::SQLiteValue (const std::string& value)
			: _value ()
		{
			_value.assign (value.data ());
		}



		SQLiteValue::~SQLiteValue ()
		{
		}



		const std::string&
		SQLiteValue::getText () const
		{
			return (_value);
		}

		    
		    
		int 
		SQLiteValue::getInt () const
		{
			return Conversion::ToInt (getText ());
		}
		    
		    

		long
		SQLiteValue::getLong () const
		{
			return Conversion::ToLong (getText ());
		}



		bool 
		SQLiteValue::getBool () const
		{
			return Conversion::ToBool (getText ());
		}



		double 
		SQLiteValue::getDouble () const
		{
			return Conversion::ToDouble (getText ());
		}



		const std::string& 
		SQLiteValue::getBlob () const
		{
			return _value;
		}



		long long 
		SQLiteValue::getLongLong () const
		{
			return Conversion::ToLongLong (getText ());
		}



		ptime SQLiteValue::getTimestamp(
		) const	{
			return time_from_string(getText());
		}


		    
		boost::logic::tribool 
		SQLiteValue::getTribool () const
		{
			return Conversion::ToTribool (getText ());
		}
	}
}
