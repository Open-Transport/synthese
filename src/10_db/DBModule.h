////////////////////////////////////////////////////////////////////////////////
/// DBModule class header.
///	@file DBModule.h
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

#ifndef SYNTHESE_DBModule_H__
#define SYNTHESE_DBModule_H__

#include <boost/filesystem/path.hpp>

#include "ModuleClassTemplate.hpp"
#include "DBConstants.h"
#include "DBTypes.h"
#include "Registry.h"

namespace synthese
{
	/** @defgroup m10Exceptions 10 Exceptions
		@ingroup m10
	
		@defgroup m10 10 SQLite database access
		@ingroup m1

	@{
	*/

	/** 10 SQLite database access module namespace.
	*/
	namespace db
	{

	    class SQLite;
	    class SQLiteHandle;
		class SQLiteTableSync;

//		static const std::string TRIGGERS_ENABLED_CLAUSE;

		//////////////////////////////////////////////////////////////
		/// Database handling module class.
		///
		class DBModule:
			public server::ModuleClassTemplate<DBModule>
		{
			friend class server::ModuleClassTemplate<DBModule>;
			
		public:
			typedef std::map<util::RegistryKeyType, std::string>	SubClassMap;

			static const std::string _INSTANCE_COORDINATES_SYSTEM;
			
		private:

		    static SQLiteHandle*	_sqlite;
			static SubClassMap		_subClassMap;
			static std::map<std::string,std::string>	_tableSyncMap;
			static std::map<int,std::string>	_idTableSyncMap;
		    static boost::filesystem::path _DatabasePath;
			static SRID _instanceSRID;
			static SRID _storageSRID;

		public:

		    static const boost::filesystem::path& GetDatabasePath ();
		    static void SetDatabasePath (const boost::filesystem::path& databasePath);

		    static SQLite* GetSQLite ();

			//////////////////////////////////////////////////////////////////////////
			/// Gets the table sync by table name.
			/// @param tableName name of the table
			/// @return the specified table sync
			/// @author Hugues Romain
			/// @date 2010
			/// @since 3.1.16
			static boost::shared_ptr<SQLiteTableSync> GetTableSync(const std::string& tableName);
			


			//////////////////////////////////////////////////////////////////////////
			/// Gets the table sync by its id key.
			/// @param tableId id of the table sync to return
			/// @return the specified table sync
			/// @author Hugues Romain
			/// @date 2010
			/// @since 3.1.16
			static boost::shared_ptr<SQLiteTableSync> GetTableSync(int tableId);

		    /** Called whenever a parameter registered by this module is changed
		     */
		    static void ParameterCallback (const std::string& name, 
						   const std::string& value);

			static void AddSubClass(util::RegistryKeyType, const std::string&);
			static std::string GetSubClass(util::RegistryKeyType id);

			static void ChangeInstanceCoordinatesSystem(
				const std::string&,
				const std::string& value
			);

			static SRID GetInstanceSRID(){ return _instanceSRID; }
			static SRID GetStorageSRID(){ return _storageSRID; }
		};
	}

	/** @} */

}
#endif // SYNTHESE_DBModule_H__

