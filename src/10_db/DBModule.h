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

#include "DB.hpp"
#include "ModuleClassTemplate.hpp"
#include "DBConstants.h"
#include "DBTypes.h"
#include "Registry.h"

#include <map>
#include <boost/filesystem/path.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/shared_ptr.hpp>


namespace synthese
{
	class CoordinatesSystem;

	/** @defgroup m10Exceptions 10 Exceptions
		@ingroup m10
	
		@defgroup m10 10 database access
		@ingroup m1

	@{
	*/

	/** 10 database access module namespace.
	*/
	namespace db
	{
		class DBTableSync;

		//////////////////////////////////////////////////////////////
		/// Database handling module class.
		///
		class DBModule:
			public server::ModuleClassTemplate<DBModule>
		{
			friend class server::ModuleClassTemplate<DBModule>;
			// For accessing ModuleClass::RegisterParameter.
			friend class DB;

			
		public:
			typedef std::map<util::RegistryKeyType, std::string> SubClassMap;
			typedef std::map<std::string, boost::shared_ptr<DBTableSync> > TablesByNameMap;
			typedef std::map<int, boost::shared_ptr<DBTableSync> > TablesByIdMap;


		private:
			static boost::shared_ptr<DB::ConnectionInfo> _ConnectionInfo;
			static boost::shared_ptr<DB> _Db;
			static SubClassMap		_subClassMap;
			static TablesByNameMap	_tableSyncMap;
			static TablesByIdMap	_idTableSyncMap;
			static const CoordinatesSystem* _storageCoordinatesSystem;


		public:
			DBModule() { };
			
			virtual ~DBModule() { };

			static void SetConnectionString(const std::string& connectionString);

			static DB* GetDB();

			//////////////////////////////////////////////////////////////////////////
			/// Gets the table sync by table name.
			/// @param tableName name of the table
			/// @return the specified table sync
			/// @author Hugues Romain
			/// @date 2010
			/// @since 3.1.16
			/// @throws DBException if the table was not found
			static boost::shared_ptr<DBTableSync> GetTableSync(const std::string& tableName);
			


			//////////////////////////////////////////////////////////////////////////
			/// Gets the table sync by its id key.
			/// @param tableId id of the table sync to return
			/// @return the specified table sync
			/// @author Hugues Romain
			/// @date 2010
			/// @since 3.1.16
			/// @throws DBException if the table was not found
			static boost::shared_ptr<DBTableSync> GetTableSync(int tableId);

			//////////////////////////////////////////////////////////////////////////
			/// @since 3.2.0
			static const TablesByNameMap& GetTablesByName(){ return _tableSyncMap; }

		    /** Called whenever a parameter registered by this module is changed
		     */
		    static void ParameterCallback (const std::string& name, 
						   const std::string& value);

			static void AddSubClass(util::RegistryKeyType, const std::string&);
			static std::string GetSubClass(util::RegistryKeyType id);

			static const CoordinatesSystem& GetStorageCoordinatesSystem() { return *_storageCoordinatesSystem; }
			static void SetStorageCoordinatesSystem(const CoordinatesSystem& value) { _storageCoordinatesSystem = &value; }
			static void ClearStorageCoordinatesSystem() { _storageCoordinatesSystem = 0; }
		};
	}

	/** @} */

}
#endif // SYNTHESE_DBModule_H__

