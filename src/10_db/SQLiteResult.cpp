
/** SQLiteResult class implementation.
	@file SQLiteResult.cpp

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

#include "SQLiteResult.h"
#include "SQLiteException.h"
#include "DBConstants.h"
#include "DBModule.h"
#include "CoordinatesSystem.hpp"

#include <iomanip>
#include <spatialite/sqlite3.h>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/lexical_cast.hpp>
#include <geos/io/WKBReader.h>
#include <geos/io/WKTReader.h>
#include <geos/geom/Geometry.h>
#include <geos/io/ParseException.h>

using namespace std;
using namespace boost;
using namespace boost::posix_time;
using namespace boost::gregorian;
using namespace geos::geom;
using namespace geos::io;

namespace synthese
{
	using namespace util;
	namespace db
	{


		SQLiteResult::SQLiteResult ()
		{
		}



		SQLiteResult::~SQLiteResult ()
		{
		}



		int
		SQLiteResult::getColumnIndex (const std::string& columnName) const
		{
			for (int i=0; i<getNbColumns (); ++i)
			{
				if (getColumnName (i) == columnName) return i;
			}
			return -1;
		}



		SQLiteValue SQLiteResult::getValue (const std::string& name) const
		{
			int index = getColumnIndex (name);
			if (index == -1)
			{
				throw SQLiteException ("No such column " + name);
			}
			return getValue (index);
		}



		std::string	SQLiteResult::getText (int column) const
		{
			return getValue(column).getText ();
		}



		std::string	SQLiteResult::getText (const std::string& name) const
		{
			int index = getColumnIndex (name);
			if (index == -1)
			throw SQLiteException ("No such column " + name);
			return getText(index);
		}




		int 
		SQLiteResult::getInt (int column) const
		{
			return getValue(column).getInt ();
		}

		int 
		SQLiteResult::getInt (const std::string& name) const
		{
			int index = getColumnIndex (name);
			if (index == -1)
			throw SQLiteException ("No such column " + name);
			return getInt(index);
		}



		long 
		SQLiteResult::getLong (int column) const
		{
			return getValue(column).getLong ();
		}



		long SQLiteResult::getLong (const std::string& name) const
		{
			int index = getColumnIndex (name);
			if (index == -1)
			throw SQLiteException ("No such column " + name);
			return getLong (index);
		}



		bool SQLiteResult::getBool (int column) const
		{
			return getValue(column).getBool ();
		}



		bool SQLiteResult::getBool (const std::string& name) const
		{
			int index = getColumnIndex (name);
			if (index == -1)
			throw SQLiteException ("No such column " + name);
			return getBool (index);
		}



		boost::logic::tribool
		SQLiteResult::getTribool (int column) const
		{
			return getValue(column).getTribool ();
		}



		boost::logic::tribool 
		SQLiteResult::getTribool (const std::string& name) const
		{
			int index = getColumnIndex (name);
			if (index == -1)
			throw SQLiteException ("No such column " + name);
			return getTribool (index);
		}



		double 
		SQLiteResult::getDouble (int column) const
		{
			return getValue (column).getDouble ();
		}



		double 
		SQLiteResult::getDouble (const std::string& name) const
		{
			int index = getColumnIndex (name);
			if (index == -1)
			throw SQLiteException ("No such column " + name);
			return getDouble (index);
		}



		std::string 
		SQLiteResult::getBlob (int column) const
		{
			return getValue (column).getBlob ();
		}

		std::string 
		SQLiteResult::getBlob (const std::string& name) const
		{
			int index = getColumnIndex (name);
			if (index == -1)
			throw SQLiteException ("No such column " + name);
			return getBlob (index);
		}



		long long SQLiteResult::getLongLong (int column) const
		{
			return getValue (column).getLongLong ();
		}



		long long SQLiteResult::getLongLong (const std::string& name) const
		{
			int index = getColumnIndex (name);
			if (index == -1)
			throw SQLiteException ("No such column " + name);
			return getLongLong (index);

		}



		boost::posix_time::ptime 
		SQLiteResult::getTimestamp (int column) const
		{
			return getValue (column).getTimestamp ();
		}


		boost::posix_time::ptime
		SQLiteResult::getTimestamp (const std::string& name) const
		{
			int index = getColumnIndex (name);
			if (index == -1)
			throw SQLiteException ("No such column " + name);
			return getTimestamp (index);
		}


		    
		SQLiteResultRow 
		SQLiteResult::getRow () const
		{
			SQLiteResultRow row;
			for (int i=0; i<getNbColumns (); ++i)
			{
				row.push_back(getValueSPtr(i));
			}
			return row;
		}



		std::vector<int> SQLiteResult::computeMaxColWidths () const
		{
			reset ();

			std::vector<int> widths;
			for (int c=0; c<getNbColumns (); ++c) 
			{
				size_t max = 0;
				std::string name (getColumnName (c));
				if (name.length () > max) max = name.length ();

				while (next ())
				{
					std::string value (getText (c));
					if (value.length () > max) max = value.length ();
				}
				widths.push_back (max);
			}
			return widths;
		}



		RegistryKeyType SQLiteResult::getKey() const
		{
			return getLongLong(TABLE_COL_ID);
		}

		boost::optional<int> SQLiteResult::getOptionalInt( const std::string& name ) const
		{
			string text(getText(name));
			if(text.empty()) return optional<int>();
			return lexical_cast<int>(text);
		}

		boost::optional<size_t> SQLiteResult::getOptionalUnsignedInt( const std::string& name ) const
		{
			string text(getText(name));
			if(text.empty()) return optional<size_t>();
			return lexical_cast<size_t>(text);
		}



		boost::posix_time::ptime SQLiteResult::getDateTime( const std::string& name ) const
		{
			try
			{
				string text(getText(name));
				return time_from_string(text);
			}
			catch(...)
			{
				return ptime(not_a_date_time);
			}
		}



		boost::gregorian::date SQLiteResult::getDate( const std::string& name ) const
		{
			try
			{
				string text(getText(name));
				return from_string(text);
			}
			catch(...)
			{
				return date(not_a_date_time);
			}
		}



		boost::posix_time::time_duration SQLiteResult::getHour( const std::string& name ) const
		{
			try
			{
				string text(getText(name));
				return duration_from_string(text);
			}
			catch(...)
			{
				return time_duration(not_a_date_time);
			}
		}



		boost::shared_ptr<Geometry> SQLiteResult::getGeometry(
			const std::string& col,
			bool isWKB
		) const	{
			string colStr(getText(col));
			
			if(colStr.empty())
			{
				return shared_ptr<Geometry>();
			}

			try
			{
				if(isWKB)
				{
					stringstream str(colStr);
					WKBReader reader(DBModule::GetStorageCoordinatesSystem().getGeometryFactory());
					return 
						CoordinatesSystem::GetInstanceCoordinatesSystem().convertGeometry(
							*shared_ptr<Geometry>(reader.read(str))
						);
				}
				else
				{
					WKTReader reader(&DBModule::GetStorageCoordinatesSystem().getGeometryFactory());
					return 
						CoordinatesSystem::GetInstanceCoordinatesSystem().convertGeometry(
							*shared_ptr<Geometry>(reader.read(colStr))
						);
				}
			}
			catch(geos::io::ParseException& e)
			{
				return shared_ptr<Geometry>();
			}
		}



		std::ostream& operator<< ( std::ostream& os, const SQLiteResult& op )
		{
			std::vector<int> widths (op.computeMaxColWidths ());

			for (int c=0; c<op.getNbColumns (); ++c) 
			{
				os << std::setw (widths.at(c)) << std::setfill (' ') << op.getColumnName (c);
				if (c != op.getNbColumns ()-1) os << " | ";
			}
			os << std::endl;

			op.reset ();
			while (op.next ())
			{
				for (int c=0; c<op.getNbColumns (); ++c) 
				{
					os << std::setw (widths.at(c)) << std::setfill (' ') << op.getText (c);
					if (c != op.getNbColumns ()-1) os << " | ";
				}
				os << std::endl;
			}
			return os;
		}
	}
}
