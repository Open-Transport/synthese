
/** SQLiteCachedResult class implementation.
	@file SQLiteCachedResult.cpp

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

#include "SQLiteCachedResult.h"
#include "SQLiteException.h"

using namespace boost::posix_time;
using namespace boost;

namespace synthese
{
	using namespace util;
	
	namespace db
	{
		SQLiteCachedResult::SQLiteCachedResult (const std::vector<std::string>& columnNames)
			: _pos (-1)
			, _columnNames (columnNames)
		{
		}



		SQLiteCachedResult::SQLiteCachedResult ()
			: _pos (-1)
		{
		}



		SQLiteCachedResult::SQLiteCachedResult (const SQLiteResultSPtr& result)
			: _pos (-1)
		{
			for (int i=0; i<result->getNbColumns (); ++i)
			{
			_columnNames.push_back (result->getColumnName (i));
			}

			while (result->next ())
			{
				addRow (result->getRow ());
			}
		}



		SQLiteCachedResult::~SQLiteCachedResult ()
		{
			for (std::vector<SQLiteResultRow>::iterator it = _rows.begin ();
			 it != _rows.end (); ++it) 
			{
			SQLiteResultRow& row = *it;
			for (SQLiteResultRow::iterator it2 = row.begin ();
				 it2 != row.end (); ++it2) 
			{
				delete *it2;
			}

			}
		}




		void 
		SQLiteCachedResult::reset () const
		{
			_pos = -1;
		}



		bool 
		SQLiteCachedResult::next () const
		{
			++_pos;
			return (_pos < _rows.size ());
		}



		int SQLiteCachedResult::getNbColumns () const
		{
			return _columnNames.size ();
		}



		std::string
		SQLiteCachedResult::getColumnName (int column) const
		{
			return _columnNames.at (column);
		}



		SQLiteValue	SQLiteCachedResult::getValue (int column) const
		{
			return *_rows.at (_pos).at (column);
		}



		boost::shared_ptr<SQLiteValue> SQLiteCachedResult::getValueSPtr( int column ) const
		{
			return _rows.at (_pos).at (column);
		}



		SQLiteResultRow 
		SQLiteCachedResult::getRow () const
		{
			return _rows.at (_pos);
		}
		    


		void 
		SQLiteCachedResult::addRow (const SQLiteResultRow& row)
		{
			_rows.push_back (row);
		}



		void SQLiteCachedResult::addRow (int nbColumns, char** values, char** columns)
		{
			if (_columnNames.size () == 0) 
			{
			for (int i=0; i<nbColumns; ++i) _columnNames.push_back (columns[i]);
			}
			SQLiteResultRow row;
			for (int i=0; i<nbColumns; ++i) 
			{
				if (values[i] == 0)
				{
					row.push_back(shared_ptr<SQLiteValue>(new SQLiteValue("")));
				}
				else
				{
					row.push_back(shared_ptr<SQLiteValue>(new SQLiteValue(values[i])));
				}
			}
			addRow (row);
		}
	}
}
