////////////////////////////////////////////////////////////////////////////////
/// SQLiteTableSync::Formatclass header.
///	@file SQLiteTableFormat.h
///	@author Hugues Romain
///
///	This file belongs to the SYNTHESE project (public transportation specialized
///	software)
///	Copyright (C) 2002 Hugues Romain - RCS <contact@reseaux-conseil.com>
///
///	This program is free software; you can redistribute it and/or
///	modify it under the terms of the GNU General Public License
///	as published by the Free Software Foundation; either version 2
///	of the License, or (at your option) any later version.
///
///	This program is distributed in the hope that it will be useful,
///	but WITHOUT ANY WARRANTY; without even the implied warranty of
///	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
///	GNU General Public License for more details.
///
///	You should have received a copy of the GNU General Public License
///	along with this program; if not, write to the Free Software Foundation,
///	Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
////////////////////////////////////////////////////////////////////////////////

#ifndef SYNTHESE_DB_SQLITETABLEFORMAT_H
#define SYNTHESE_DB_SQLITETABLEFORMAT_H

#include <string>
#include <vector>
#include <utility>
#include <map>
#include <boost/array.hpp>

#include "02_db/Types.h"

namespace synthese
{
	namespace db
	{
		typedef std::pair<std::string, std::vector<std::string> > SQLiteTableIndexFormat;
		typedef std::map<std::string, std::vector<std::string> > SQLiteTableIndexMap;
	    

		////////////////////////////////////////////////////////////////////
		/// Table format describer class.
		///
		///	This class describes a SQLite table in two ways :
		///		- the columns
		///		- the indexes
		///
		///	@ingroup m10
		class SQLiteTableSync::Format
	    {

			~SQLiteTableFormat ();



		private:
			////////////////////////////////////////////////////////////////////
			///	Determines table id from its name.
			///	@param tableName Name of the table
			///	@return SQLiteTableFormat::TableId ID of the table
			/// @throws SQLiteException if the ID cannot be extracted from the name
			///	@author Hugues Romain
			///	@date 2008
			////////////////////////////////////////////////////////////////////
			static TableId _ParseTableId(
				const std::string& tableName
				);

		public:

			
			
			////////////////////////////////////////////////////////////////////
			///	Creates the index name in the database.
			///	@param index
			///	@return the real index name in the database is the concatenation of the table name and the name specified in the table format separated by a _ character.
			///	@author Hugues Romain
			///	@date 2008
			////////////////////////////////////////////////////////////////////
			std::string getIndexDBName(const Index& index)	const;


			std::string getSQLTriggerNoUpdate() const;

			bool hasTableColumn (const std::string& name) const;

			bool hasNonUpdatableColumn () const;
	    };
	}
}

#endif
