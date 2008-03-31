
/** SQLiteTableFormat class implementation.
	@file SQLiteTableFormat.cpp

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

#include <sstream>

#include "SQLiteTableFormat.h"
#include "SQLiteException.h"

using namespace std;

namespace synthese
{
	namespace db
	{
		SQLiteTableFormat::SQLiteTableFormat ()
			: _hasNonUpdatableColumn (false)
		{

		}





		SQLiteTableFormat::~SQLiteTableFormat ()
		{

		}




		bool 
		SQLiteTableFormat::hasTableColumn (const std::string& name) const
		{
			for (std::vector<SQLiteTableColumnFormat>::const_iterator it = _columns.begin ();
			 it != _columns.end (); ++it)
			{
			if (it->name == name) return true;
			}
			return false;
		}



				    


		void 
		SQLiteTableFormat::addTableColumn (const std::string& name,
						   const std::string& type,
						   bool updatable)
		{
			SQLiteTableColumnFormat column;
			column.name = name;
			column.type = type;
			column.updatable = updatable;
			_columns.push_back (column);
		    
			if (updatable == false)
			{
			_hasNonUpdatableColumn = true;
			}

		}




		int 
		SQLiteTableFormat::getTableColumnCount () const
		{
			return _columns.size ();
		}





		const SQLiteTableColumnFormat& 
		SQLiteTableFormat::getTableColumn (int index) const
		{
			return _columns.at (index);
		}




		bool 
		SQLiteTableFormat::hasNonUpdatableColumn () const
		{
			return _hasNonUpdatableColumn;
		}

		void SQLiteTableFormat::addTableIndex(const std::vector<std::string>& columns,  std::string name)
		{
			// Name of the index
			if (name.empty())
			{
				stringstream s;
				for (vector<string>::const_iterator it = columns.begin(); it != columns.end(); ++it)
					s << *it << "_";
				name = s.str();
			}
			
			// If the index exists, the old is removed
			SQLiteTableIndexMap::iterator it = _indexes.find(name);
			if (it != _indexes.end())
				_indexes.erase(it);

			// Storage of the index
			_indexes.insert(make_pair(name, columns));
		}

		SQLiteTableIndexMap& SQLiteTableFormat::getTableIndexes()
		{
			return _indexes;
		}

	}
}
