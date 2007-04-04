
/** SQLiteTableFormat class header.
	@file SQLiteTableFormat.h

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

#ifndef SYNTHESE_DB_SQLITETABLEFORMAT_H
#define SYNTHESE_DB_SQLITETABLEFORMAT_H

#include <string>
#include <vector>
#include <utility>
#include <map>

namespace synthese
{
	namespace db
	{
	    typedef struct
		{
			std::string name;
			std::string type;
			bool updatable;
	    } SQLiteTableColumnFormat;

		typedef std::pair<std::string, std::vector<std::string> > SQLiteTableIndexFormat;
		typedef std::map<std::string, std::vector<std::string> > SQLiteTableIndexMap;
	    

		/** Table format describer class.

			This class describes a SQLite table in two ways :
				- the columns
				- the indexes

			@ingroup m02
		*/
	    class SQLiteTableFormat 
	    {
		private:

			std::vector<SQLiteTableColumnFormat>	_columns;
			SQLiteTableIndexMap						_indexes;
			bool									_hasNonUpdatableColumn;
			
		protected:

		public:
			    
			SQLiteTableFormat ();
			~SQLiteTableFormat ();

			bool hasTableColumn (const std::string& name) const;

			
			void addTableColumn (const std::string& name,
						 const std::string& type,
						 bool updatable);

			/** Adds an index in the table description.
				@param columns Vector of column names
				@param name Name of the index. Optional : if not specified or empty, the name is the concatenation of the columns names
			*/
			void addTableIndex(const std::vector<std::string>& columns, std::string name = "");

			/** Indexes getter.
				@return SQLiteTableIndexMap& The index map
				@author Hugues Romain
				@date 2007
				
			*/
			SQLiteTableIndexMap& getTableIndexes();

			int getTableColumnCount () const;


			const SQLiteTableColumnFormat& getTableColumn (int index) const;
			bool hasNonUpdatableColumn () const;
	    };
	}
}

#endif
