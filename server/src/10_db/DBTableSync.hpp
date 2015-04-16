////////////////////////////////////////////////////////////////////////////////
/// DBTableSync class header.
///	@file DBTableSync.hpp
///	@author Hugues Romain
///
///	This file belongs to the SYNTHESE project (public transportation specialized
///	software)
///	Copyright (C) 2002 Hugues Romain - RCSmobility <contact@rcsmobility.com>
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

#ifndef SYNTHESE_db_DBTableSync_hpp__
#define SYNTHESE_db_DBTableSync_hpp__

#include "DBConstants.h"
#include "DBTypes.h"
#include "DBResult.hpp"
#include "FactoryBase.h"
#include "Field.hpp"

#include <string>
#include <vector>
#include <iostream>

namespace synthese
{
	namespace server
	{
		class Session;
	}

	namespace db
	{
		typedef std::vector<util::RegistryKeyType> RowIdList;
		typedef std::vector<std::pair<util::RegistryKeyType, std::string> > RowsList;

		class DBException;
		class DBResult;
		class DB;
		class DBTransaction;

		/// @defgroup refLS Table synchronizers.
		///	@ingroup ref

		////////////////////////////////////////////////////////////////////
		/// Interface for a table synchronizer.
		///	By convention, the table name must always start with the t letter
		///	followed by a unique 3 digits integer (some databases do not allow ids starting with number).
		///
		///	@ingroup m10
		class DBTableSync:
			public util::FactoryBase<DBTableSync>
		{
		public:
			////////////////////////////////////////////////////////////////////
			/// Index of a table.
			struct Index
			{
				typedef std::vector<std::string>	Fields;

				Fields			fields;



				////////////////////////////////////////////////////////////////////
				///	Index constructor.
				///	@param first First field
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
				Index(
					const char* first,
					...
				);



				////////////////////////////////////////////////////////////////////
				///	Empty index constructor.
				///	@author Hugues Romain
				///	@date 2008
				/// Can be used as mark to end an Index array.
				Index();



				////////////////////////////////////////////////////////////////////
				///	Tests if the index is empty.
				///	@return bool true if the index is empty
				///	@author Hugues Romain
				///	@date 2008
				bool empty() const;
			};

			typedef std::vector<DBTableSync::Index> Indexes;





			////////////////////////////////////////////////////////////////////////
			/// Format of a table.
			/// Other informations than fields and indexes
			struct Format
			{
				std::string					NAME;
				util::RegistryTableType		ID;
				bool						HAS_AUTO_INCREMENT;
				bool						IGNORE_CALLBACKS_ON_FIRST_SYNC;

				/// Statistics variables used by tests.
				mutable bool CreatedTable;
				mutable bool MigratedSchema;
				mutable int CreatedIndexes;


				////////////////////////////////////////////////////////////////////
				///	Table format Constructor.
				///	@param name Name of the table
				///	@param ignoreCallbacksOnFirstSync Ignore callbacks on first sync
				///	@param hasAutoIncrement Activation of auto increment
				///	@author Hugues Romain
				///	@date 2008
				/// @throws DBException if :
				///		- the table has no field
				///		- at least a field as an empty name
				//lint --e{1712}
				////////////////////////////////////////////////////////////////////
				Format(
					const std::string&	name,
					const bool			ignoreCallbacksOnFirstSync = false,
					const bool			hasAutoIncrement = true
				);
			};


		protected:
			////////////////////////////////////////////////////////////////////
			///	DBTableSync constructor.
			DBTableSync();


		private:

		    std::vector<std::string> _selectOnCallbackColumns;

		public:
			////////////////////////////////////////////////////////////////////
			///	DBTableSync destructor.
			virtual ~DBTableSync ();

			//! @name Virtual access to static methods
			//@{
				////////////////////////////////////////////////////////////////////
				///	Table format virtual getter.
				///	@return const Format& Format of the table
				///	@author Hugues Romain
				///	@date 2008
				virtual const Format& getFormat() const = 0;



				//////////////////////////////////////////////////////////////////////////
				/// Table fields virtual getter.
				/// @return list of fields of the table
				/// @author Hugues Romain
				/// @date 2012
				virtual FieldsList getFieldsList() const = 0;



				//////////////////////////////////////////////////////////////////////////
				/// Checks if a field exists in the table.
				/// @param fieldName the name of the field to check
				/// @return true if the field exists in the table
				/// @date 2013
				/// @author Hugues Romain
				bool hasField(const std::string& fieldName) const;



				////////////////////////////////////////////////////////////////////
				///	Auto increment initializer.
				///	@author Hugues Romain
				///	@date 2008
				virtual void initAutoIncrement() const = 0;



				////////////////////////////////////////////////////////////////////
				///	Auto increment updater.
				/// The auto increment is updated if :
				///  - the id has been generated by the current node
				///  - the id is greater than the max existing id 
				/// @param id the id to check
				virtual void updateAutoIncrement(
					util::RegistryKeyType id
				) const = 0;



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
				///	@param db
				///	@param sync
				///	@author Hugues Romain
				///	@date 2008
				virtual void firstSync(DB* db) const = 0;




				////////////////////////////////////////////////////////////////////
				///	Updates the schema of the tables at the program launch.
				///	@param db
				///	@author Hugues Romain
				///	@date 2008
				virtual void updateSchema(DB* db) const = 0;



				//////////////////////////////////////////////////////////////////////////
				/// Tests if specified object can be deleted from the table, according to
				/// the current user rights if a session is opened.
				/// @param session currently opened session
				/// @param object_id id of the object to remove
				/// @param recursively test for a recursive deletion
				/// @author Hugues Romain
				/// @since 2011
				/// @version 3.3.0
				virtual bool canDelete(
					const server::Session* session,
					util::RegistryKeyType object_id
				) const = 0;



				////////////////////////////////////////////////////////////////////
				/// Utility method to delete a record specified by its id.
				/// @param id id of the record to delete
				/// @param recursively delete the records of the tables that point to the deleted record.
				virtual void deleteRecord(
					const server::Session* session,
					util::RegistryKeyType id,
					DBTransaction& transaction,
					bool log = true
				) const = 0;
			//@}


			virtual void rowsAdded(
				DB* db,
				const DBResultSPtr& rows
			) const = 0;

			virtual void rowsUpdated(
				DB* db,
				const DBResultSPtr& rows
			) const = 0;

			virtual void rowsRemoved(
				DB* db,
				const RowIdList& rowIds
			) const = 0;

			virtual util::RegistryKeyType getNewId() const = 0;



			////////////////////////////////////////////////////////////////////
			/// Utility method to get a row by id.
			virtual DBResultSPtr getRow(
				util::RegistryKeyType id
			) const = 0;

			virtual RowsList SearchForAutoComplete(
				const boost::optional<std::string> prefix,
				const boost::optional<std::size_t> limit,
				const boost::optional<std::string> optionalParameter
				) const
			{
				RowsList result;

				result.push_back(std::make_pair(0, "non definie"));
				return result;
			}
		};
}	}

#endif // SYNTHESE_db_DBTableSync_hpp__
