
/** DBResult class implementation.
	@file DBResult.cpp

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

#include "DBResult.hpp"
#include "DBException.hpp"
#include "DBConstants.h"
#include "DBModule.h"
#include "Conversion.h"

#include <iomanip>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/lexical_cast.hpp>

using namespace std;
using namespace boost;
using namespace boost::posix_time;
using namespace boost::gregorian;
using namespace geos::geom;

namespace synthese
{
	using namespace util;
	namespace db
	{
		DBResult::DBResult ()
		{
			resetPosition();
		}



		DBResult::~DBResult ()
		{
			resetPosition();
		}



		unsigned int DBResult::_getColumnIndexInternal(const std::string& columnName) const
		{
			int index = getColumnIndex(columnName);
			if (index == -1)
				throw DBException("No such column " + columnName);
			return (unsigned int)index;
		}



		int
		DBResult::getColumnIndex (const std::string& columnName) const
		{
			for (int i=0; i<getNbColumns (); ++i)
			{
				if (getColumnName (i) == columnName) return i;
			}
			return -1;
		}



		std::string	DBResult::getText (const std::string& name) const
		{
			return getText(_getColumnIndexInternal(name));
		}



		int
		DBResult::getInt (const std::string& name) const
		{
			return getInt(_getColumnIndexInternal(name));
		}



		boost::optional<int> DBResult::getOptionalInt( const std::string& name ) const
		{
			string text(getText(name));
			if(text.empty()) return optional<int>();
			return lexical_cast<int>(text);
		}



		boost::optional<size_t> DBResult::getOptionalUnsignedInt( const std::string& name ) const
		{
			string text(getText(name));
			if(text.empty()) return optional<size_t>();
			return lexical_cast<size_t>(text);
		}



		long long DBResult::getLongLong (const std::string& name) const
		{
			return getLongLong(_getColumnIndexInternal(name));
		}



		bool DBResult::getBool (int column) const
		{
			return getLongLong(column) > 0;
		}



		bool DBResult::getBool (const std::string& name) const
		{
			return getBool(_getColumnIndexInternal(name));
		}



		boost::logic::tribool
		DBResult::getTribool (int column) const
		{
			return Conversion::ToTribool(getText(column));
		}



		boost::logic::tribool
		DBResult::getTribool (const std::string& name) const
		{
			return getTribool(_getColumnIndexInternal(name));
		}



		double
		DBResult::getDouble (const std::string& name) const
		{
			return getDouble(_getColumnIndexInternal(name));
		}



		std::string
		DBResult::getBlob (int column) const
		{
			return getText(column);
		}



		std::string
		DBResult::getBlob (const std::string& name) const
		{
			return getBlob(_getColumnIndexInternal(name));
		}



		std::vector<int> DBResult::computeMaxColWidths () const
		{
			std::vector<int> widths;
			for (int c=0; c<getNbColumns (); ++c)
			{
				size_t max = 0;
				std::string name (getColumnName (c));
				if (name.length () > max) max = name.length ();

				while (next())
				{
					std::string value (getText (c));
					if (value.length () > max) max = value.length ();
				}
				reset();
				widths.push_back (max);
			}
			return widths;
		}



		RegistryKeyType DBResult::getKey() const
		{
			return getLongLong(TABLE_COL_ID);
		}



		boost::posix_time::ptime DBResult::getDateTime( const std::string& name ) const
		{
			try
			{
				string text(getText(name));
				if(text.empty())
				{
					return ptime(not_a_date_time);
				}
				return time_from_string(text);
			}
			catch(...)
			{
				return ptime(not_a_date_time);
			}
		}



		boost::gregorian::date DBResult::getDate( const std::string& name ) const
		{
			try
			{
				string text(getText(name));
				if(!text.empty())
				{
					return from_string(text);
				}
			}
			catch(...)
			{
			}
			return date(not_a_date_time);
		}



		boost::posix_time::time_duration DBResult::getHour( const std::string& name ) const
		{
			try
			{
				string text(getText(name));
				if(!text.empty())
				{
					return duration_from_string(text);
				}
			}
			catch(...)
			{
			}
			return time_duration(not_a_date_time);
		}



		void DBResult::resetPosition() const
		{
			_pos = -1;
		}



		void DBResult::incrementPosition() const
		{
			_pos++;
		}



		void DBResult::ensurePosition() const
		{
			if (_pos == -1) throw DBException ("Not inside result (use next ())...");
		}



		std::string DBResult::getValue(
			const std::string& name,
			bool exceptionIfMissing
		) const	{
			try
			{
				return getText(name);
			}
			catch (DBException& e)
			{
				if(exceptionIfMissing)
				{
					throw e;
				}
				return string();
			}
		}



		bool DBResult::isDefined( const std::string& fieldName ) const
		{
			return getColumnIndex(fieldName) != -1;
		}



		Record::FieldNames DBResult::getFieldNames() const
		{
			FieldNames result;
			size_t nbcol(getNbColumns());
			for(size_t icol(0); icol<nbcol; ++icol)
			{
				result.push_back(getColumnName(icol));
			}
			return result;
		}



		std::ostream& operator<< ( std::ostream& os, const DBResult& op )
		{
			std::vector<int> widths (op.computeMaxColWidths ());

			for (int c=0; c<op.getNbColumns (); ++c)
			{
				os << std::setw (widths.at(c)) << std::setfill (' ') << op.getColumnName (c);
				if (c != op.getNbColumns ()-1) os << " | ";
			}
			os << std::endl;

			while (op.next ())
			{
				for (int c=0; c<op.getNbColumns (); ++c)
				{
					os << std::setw (widths.at(c)) << std::setfill (' ') << op.getText (c);
					if (c != op.getNbColumns ()-1) os << " | ";
				}
				os << std::endl;
			}
			op.reset();
			return os;
		}
	}
}
