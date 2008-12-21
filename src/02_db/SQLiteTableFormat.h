
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

#include "02_db/Types.h"

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

			@ingroup m10
		*/
	    class SQLiteTableFormat 
	    {
		public:
			typedef int	TableId;

			struct Field
			{
				std::string	name;
				FieldType	type;
				bool		updatable;

				Field(
					std::string nameA = std::string(),
					FieldType = TEXT,
					bool updatableA = true
				);
			};

			struct Index
			{
				typedef std::vector<std::string>	Fields;
				
				std::string					name;
				Fields	fields;

				
				
				////////////////////////////////////////////////////////////////////
				///	Index constructor.
				///	@param nameA Name of the index
				///	@param fieldsA Fields to index
				///	@author Hugues Romain
				///	@date 2008
				///
				/// There is 3 ways to build an index :
				///		- empty index : use it to mark the end of the CreateIndexes 
				///		  parameters : do not provide any argument
				///		- mono field index : the nameA parameter stores the name of
				///		  the indexed field, the fieldsA parameter is empty
				///		- multi field index : the nameA parameter stores the unique
				///		  name of the index within the table, the fieldsA parameter
				///		  stores the list of indexed fields (vector object)
				////////////////////////////////////////////////////////////////////
				Index(
					std::string nameA = std::string(),
					Fields fieldsA = Fields()
				);

				
				
				////////////////////////////////////////////////////////////////////
				///	Field list creation helper.
				///	@param first First field
				///	Variable parameters number
				///	@return synthese::db::SQLiteTableFormat::Index::Fields
				///	@author Hugues Romain
				///	@date 2008
				////////////////////////////////////////////////////////////////////
				static Fields CreateFieldsList(std::string first, ...);
			};

			typedef std::vector<Field>	Fields;
			typedef std::vector<Index>	Indexes;

		private:
			static std::string _GetSQLType(FieldType type);

		public:
			static Fields CreateFields(Field first, ...);
			static Indexes CreateIndexes(Index first, ...);
			

			std::string					NAME;
			TableId						ID;
			bool						HAS_AUTO_INCREMENT;
			std::string					TRIGGER_OVERRIDE_CLAUSE;
			bool						IGNORE_CALLBACKS_ON_FIRST_SYNC;
			bool						ENABLE_TRIGGERS;
			Fields						FIELDS;
			Indexes						INDEXES;

			
			
			////////////////////////////////////////////////////////////////////
			///	SQLiteTableFormat Constructor.
			///	@param name Name of the table
			///	@param hasAutoIncrement Activation of auto increment
			///	@param triggerOverrideClause Trigger override SQL clause
			///	@param ignoreCallbacksOnFirstSync Ignore callbacks on first sync
			///	@param enableTriggers Activation of triggers
			///	@param fields Fields list
			///	@param indexes Indexes list
			///	@author Hugues Romain
			///	@date 2008
			/// @throws SQLiteException if :
			///		- the table has no field
			///		- at least a field as an empty name
			////////////////////////////////////////////////////////////////////
			SQLiteTableFormat(
				const std::string&	name,
				bool				hasAutoIncrement,
				const std::string&	triggerOverrideClause,
				bool				ignoreCallbacksOnFirstSync,
				bool				enableTriggers,
				const Fields&		fields,
				const Indexes&		indexes
			);

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

			std::string getSQLFieldsSchema()	const;
			
			
			/** Creates the SQL statement to crate an index in the database given a certain format.
				@param index The index to create
				@return std::string The SQL statement
				@author Hugues Romain
				@date 2007
			*/
			std::string getSQLIndexSchema(const Index& index)	const;

			std::string getSQLTriggerNoUpdate() const;

			bool hasTableColumn (const std::string& name) const;

			bool hasNonUpdatableColumn () const;
	    };
	}
}

#endif
