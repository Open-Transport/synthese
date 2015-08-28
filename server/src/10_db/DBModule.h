////////////////////////////////////////////////////////////////////////////////
/// DBModule class header.
///	@file DBModule.h
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

#ifndef SYNTHESE_DBModule_H__
#define SYNTHESE_DBModule_H__

#include "ModuleClassTemplate.hpp"

#include "DB.hpp"
#include "DBConstants.h"
#include "DBTypes.h"
#include "FrameworkTypes.hpp"
#include "Registry.h"
#include "UtilTypes.h"

#include <map>
#include <boost/filesystem/path.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/shared_ptr.hpp>



namespace synthese
{
	class ObjectBase;

	namespace util
	{
		class Env;
	}

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
		class ConditionalSynchronizationPolicyBase;
		class DBTableSync;
		class DBTransaction;
		class DBDirectTableSync;

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
			typedef std::map<util::RegistryTableType, boost::shared_ptr<DBTableSync> > TablesByIdMap;
			typedef std::set<boost::shared_ptr<ConditionalSynchronizationPolicyBase> > ConditionalTableSyncsToReload;

			static const std::string PARAMETER_NODE_ID;
			static const std::string PARAMETER_SQL_TRACE;

		private:
			static boost::posix_time::time_duration DURATION_BETWEEN_CONDITONAL_SYNCS;

			static boost::shared_ptr<DB::ConnectionInfo> _ConnectionInfo;
			static boost::shared_ptr<DB> _Db;
			static SubClassMap		_subClassMap;
			static TablesByNameMap	_tableSyncMap;
			static TablesByIdMap	_idTableSyncMap;
			static ConditionalTableSyncsToReload _conditionalTableSyncsToReload;
			static util::RegistryNodeType _nodeId;
			static bool _sqlTrace;
			static unsigned int _thrCount;
			static bool _conditionalTablesUpdateActive;



		public:
			DBModule() { };

			virtual ~DBModule() { };

			static void SetConnectionString(const std::string& connectionString);
			static util::RegistryNodeType GetNodeId(){ return _nodeId; }
			static bool IsSqlTraceActive(){ return _sqlTrace; }


			static DB* GetDB();
			static boost::shared_ptr<DB> GetDBSPtr();

			static const TablesByIdMap& GetTablesById(){ return _idTableSyncMap; }


			//////////////////////////////////////////////////////////////////////////
			/// Returns a database object that can be used to access a non-Synthese
			/// database. The database object won't execute Synthese specific tasks,
			/// such as setting up the projection table.
			/// @param connectionString Database connection string.
			/// @return boost::shared_ptr<DB> Database object.
			/// @author Sylvain Pasche
			/// @date 2012
			/// @since 3.3.0
			static boost::shared_ptr<DB> GetDBForStandaloneUse(const std::string& connectionString);



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
			/// Gets the list of the table to reload massively every minute.
			static ConditionalTableSyncsToReload GetConditionalTableSyncsToReload(){ return _conditionalTableSyncsToReload; }



			//////////////////////////////////////////////////////////////////////////
			/// Gets the table sync by its id key.
			/// @param tableId id of the table sync to return
			/// @return the specified table sync
			/// @author Hugues Romain
			/// @date 2010
			/// @since 3.1.16
			/// @throws DBException if the table was not found
			static boost::shared_ptr<DBTableSync> GetTableSync(util::RegistryTableType tableId);



			static boost::shared_ptr<const util::Registrable> GetObject(
				util::RegistryKeyType id,
				util::Env& env
			);



			static boost::shared_ptr<util::Registrable> GetEditableObject(
				util::RegistryKeyType id,
				util::Env& env
			);



			//////////////////////////////////////////////////////////////////////////
			/// Saves an object into the database without specifying which table sync to use.
			/// @param object the object to save
			/// @param transaction the transaction to populate (undefined = direct save)
			/// @pre the object must have its key defined : auto increment is not available
			/// @author Hugues Romain
			/// @date 2012
			/// @since 3.4.0
			static void SaveObject(
				const util::Registrable& object,
				boost::optional<DBTransaction&> transaction = boost::optional<DBTransaction&>()
			);



			static void DeleteObject(
				util::RegistryKeyType id,
				boost::optional<DBTransaction&> transaction = boost::optional<DBTransaction&>()
			);



			//////////////////////////////////////////////////////////////////////////
			/// @since 3.2.0
			static const TablesByNameMap& GetTablesByName(){ return _tableSyncMap; }

			/** Called whenever a parameter registered by this module is changed
			*/
			static void ParameterCallback (const std::string& name,
							const std::string& value);

			static void AddSubClass(util::RegistryKeyType, const std::string&);
			static std::string GetSubClass(util::RegistryKeyType id);



			static void LoadObjects(
				const LinkedObjectsIds& ids,
				util::Env& env,
				util::LinkLevel linkLevel = util::UP_LINKS_LOAD_LEVEL
			);

			static void SaveEntireEnv(
				const util::Env& env,
				boost::optional<DBTransaction&> transaction = boost::optional<DBTransaction&>()
			);


			static void UpdateConditionalTableSyncEnv();
			static void ActivateConditionalTablesUpdate();
			static void DeactivateConditionalTablesUpdate();



			//////////////////////////////////////////////////////////////////////////
			/// Creates an object into the database specifying which table sync to use.
			/// @param object the object to create
			/// @param tableSync the table sync to use
			/// @author Thomas Puigt
			/// @date 2014
			/// @since 3.8.0
			static void CreateObject(
				ObjectBase* objectBase,
				DBDirectTableSync* tableSync
			);
		};
	}

	/** @} */

}
#endif // SYNTHESE_DBModule_H__

