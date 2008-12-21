
/** SQLiteTableSync class header.
	@file SQLiteTableSync.h

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

#ifndef SYNTHESE_DB_SQLITETABLESYNC_H
#define SYNTHESE_DB_SQLITETABLESYNC_H

#include "SQLiteTableFormat.h"
#include "SQLiteResult.h"
#include "SQLiteStatement.h"

#include "FactoryBase.h"
#include "UId.h"

#include <string>
#include <vector>
#include <iostream>

#define UPDATEABLE true;
#define NON_UPDATEABLE false;

namespace synthese
{
	namespace db
	{

		class SQLiteSync;
		class SQLiteResult;
		class SQLite;

		/** @defgroup refLS Table synchronizers.
			@ingroup ref
		*/

		////////////////////////////////////////////////////////////////////
		/// Interface for an SQLite table synchronizer.
		///	By convention, the table name must always start with the t letter
		///	followed by a unique 3 digits integer (SQLite does not allow ids starting with number).
		///
		///	@ingroup m10
		////////////////////////////////////////////////////////////////////
		class SQLiteTableSync : public util::FactoryBase<SQLiteTableSync>
		{
		protected:
			////////////////////////////////////////////////////////////////////
			///	SQLiteTableSync constructor.
			////////////////////////////////////////////////////////////////////
			SQLiteTableSync();




		private:
	
		    std::vector<std::string> _selectOnCallbackColumns;

		public:

			////////////////////////////////////////////////////////////////////
			///	SQLiteTableSync destructor.
			////////////////////////////////////////////////////////////////////
			~SQLiteTableSync ();

			//! @name Virtual access to static methods
			//@{
				////////////////////////////////////////////////////////////////////
				///	Table format virtual getter.
				///	@return const SQLiteTableFormat& Format of the table
				///	@author Hugues Romain
				///	@date 2008
				////////////////////////////////////////////////////////////////////
				virtual const SQLiteTableFormat& getFormat() const = 0;

				
				
				////////////////////////////////////////////////////////////////////
				///	Auto increment initializer.
				///	@author Hugues Romain
				///	@date 2008
				////////////////////////////////////////////////////////////////////
				virtual void initAutoIncrement() const = 0;

				

				////////////////////////////////////////////////////////////////////
				///	Launch of the first synchronization between the tables and the
				/// memory.
				///
				/// This method is called when the synchronizer is created
				///	to synchronize it with pre-existing data in db.
				///
				///	It creates ou updates all the needed tables and indexes.
				/// Note : It does not delete any useless table present in the database.
				///
				///	@param sqlite
				///	@param sync
				///	@author Hugues Romain
				///	@date 2008
				////////////////////////////////////////////////////////////////////
				virtual void firstSync(SQLite* sqlite, SQLiteSync* sync) const = 0;


				
				
				////////////////////////////////////////////////////////////////////
				///	Updates the schema of the tables at the program launch.
				///	@param sqlite
				///	@author Hugues Romain
				///	@date 2008
				////////////////////////////////////////////////////////////////////
				virtual void updateSchema(SQLite* sqlite) const = 0;
			//@}

			
			virtual void rowsAdded (SQLite* sqlite, 
						SQLiteSync* sync,
						const SQLiteResultSPtr& rows, bool isFirstSync = false) = 0;

			virtual void rowsUpdated (SQLite* sqlite, 
						SQLiteSync* sync,
						const SQLiteResultSPtr& rows) = 0;

			virtual void rowsRemoved (SQLite* sqlite, 
						SQLiteSync* sync,
						const SQLiteResultSPtr& rows) = 0;

			/** Utility method to get a row by id.
			    This uses a precompiled statement for performance 
			*/
			virtual SQLiteResultSPtr getRowById(
				SQLite* sqlite,
				util::RegistryKeyType id
			) const = 0;

			
		public:


			static std::vector<std::string> 
			    GetTableColumnsDb (synthese::db::SQLite* sqlite,
					       const std::string& tableName);

			static std::string GetSQLSchemaDb (synthese::db::SQLite* sqlite,
							   const std::string& tableName);
			

			static std::string GetTriggerNoUpdateDb (
			    synthese::db::SQLite* sqlite,
			    const std::string& tableName);

			/** Builds the name of the index in the sqlite master table.
				@param tableName Name of the table which belongs the index
				@param name Name of the index within the table
				@return the full name
				@author Hugues Romain
				@date 2007
				
			*/
			static std::string getIndexDBName(const std::string& tableName, const std::string& name);
		};
	}
}

#endif
