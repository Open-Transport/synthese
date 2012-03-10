
/** DBModule class implementation.
	@file DBModule.cpp

	This file belongs to the SYNTHESE project (public transportation specialized software)
	Copyright (C) 2002 Hugues Romain - RCSmobility <contact@rcsmobility.com>

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

#include "DBModule.h"

#include "DB.hpp"
#include "DBDirectTableSync.hpp"
#include "DBTableSync.hpp"
#include "DBException.hpp"
#include "DBTransaction.hpp"
#include "Log.h"
#include "Factory.h"
#include "CoordinatesSystem.hpp"

#include <iostream>
#include <boost/foreach.hpp>
#include <boost/lexical_cast.hpp>

using namespace std;
using namespace boost;

namespace synthese
{
	using namespace util;
	using namespace db;
	using namespace server;
	typedef db::DB::ConnectionInfo ConnectionInfo;

	template<> const string util::FactorableTemplate<ModuleClass,DBModule>::FACTORY_KEY("10_db");

	namespace db
	{
		DBModule::TablesByNameMap	DBModule::_tableSyncMap;
		DBModule::TablesByIdMap	DBModule::_idTableSyncMap;

		boost::shared_ptr<ConnectionInfo> DBModule::_ConnectionInfo;
		boost::shared_ptr<DB> DBModule::_Db;
		DBModule::SubClassMap DBModule::_subClassMap;

		const CoordinatesSystem* DBModule::_storageCoordinatesSystem(NULL);
	}

	namespace server
	{
		template<> const string ModuleClassTemplate<DBModule>::NAME("Base de données");



		template<> void ModuleClassTemplate<DBModule>::PreInit()
		{
			if (!DBModule::_ConnectionInfo)
			{
				throw DBException("No ConnectionInfo. DBModule::SetConnectionString() must be called");
			}
			DBModule::_Db.reset(util::Factory<DB>::create(DBModule::_ConnectionInfo->backend));

			DBModule::GetDB()->setConnectionInfo(DBModule::_ConnectionInfo);
			DBModule::GetDB()->preInit();
		}



		template<> void ModuleClassTemplate<DBModule>::Init()
		{
			// Table sync registration
			DBModule::_tableSyncMap.clear();
			vector<shared_ptr<DBTableSync> > tableSyncs(Factory<DBTableSync>::GetNewCollection());
			BOOST_FOREACH(const shared_ptr<DBTableSync>& sync, tableSyncs)
			{
				DBModule::_tableSyncMap[sync->getFormat().NAME] = sync;
				DBModule::_idTableSyncMap[sync->getFormat().ID] = sync;
			}

			// DB initialization
			DBModule::GetDB()->init();
		}



		template<> void ModuleClassTemplate<DBModule>::End()
		{
			DBModule::_ConnectionInfo.reset();
			DBModule::_Db.reset();
		}
	}

	namespace db
	{
		void DBModule::SetConnectionString(const std::string& connectionString)
		{
			_ConnectionInfo.reset(new ConnectionInfo(connectionString));
		}



		boost::shared_ptr<DBTableSync> DBModule::GetTableSync(const std::string& tableName)
		{
			TablesByNameMap::const_iterator it(_tableSyncMap.find(tableName));
			if (it == _tableSyncMap.end())
			{
				throw DBException("Table not found in database");
			}
			return it->second;
		}



		boost::shared_ptr<DBTableSync> DBModule::GetTableSync(
			util::RegistryTableType tableId
		){
			TablesByIdMap::const_iterator it(_idTableSyncMap.find(tableId));
			if (it == _idTableSyncMap.end())
			{
				throw DBException("Table not found in database");
			}
			return it->second;
		}



		DB* DBModule::GetDB()
		{
			assert(_Db.get());
			return _Db.get();
		}



		shared_ptr<DB> DBModule::GetDBSPtr()
		{
			assert(_Db.get());
			return _Db;
		}


		boost::shared_ptr<DB> DBModule::GetDBForStandaloneUse(const string& connectionString)
		{
			shared_ptr<DB::ConnectionInfo> ci(new DB::ConnectionInfo(connectionString));

			shared_ptr<DB> db(util::Factory<DB>::create(ci->backend));
			db->setStandaloneUse(true);
			db->setConnectionInfo(ci);
			db->initForStandaloneUse();

			return db;
		}



		void DBModule::AddSubClass( util::RegistryKeyType id, const string& subclass)
		{
			_subClassMap[id] = subclass;
		}



		string DBModule::GetSubClass(util::RegistryKeyType id )
		{
			SubClassMap::const_iterator it(_subClassMap.find(id));
			return (it == _subClassMap.end()) ? string() : it->second;
		}



		void DBModule::LoadObjects( const LinkedObjectsIds& ids, util::Env& env, LinkLevel linkLevel )
		{
			BOOST_FOREACH(RegistryKeyType id, ids)
			{
				shared_ptr<DBTableSync> tableSync(GetTableSync(decodeTableId(id)));
				if(!dynamic_cast<DBDirectTableSync*>(tableSync.get()))
				{
					continue;
				}
				dynamic_cast<DBDirectTableSync&>(*tableSync).getRegistrable(id, env, linkLevel);
			}
		}



		void DBModule::SaveEntireEnv(
			const util::Env& env,
			boost::optional<DBTransaction&> transaction
		){
			BOOST_FOREACH(const Env::RegistryMap::value_type& registry, env.getMap())
			{
				DBDirectTableSync* tableSync(NULL);
				BOOST_FOREACH(const RegistryBase::RegistrablesVector::value_type& item, registry.second->getRegistrablesVector())
				{
					if(!tableSync)
					{
						shared_ptr<DBTableSync> tableSyncR(GetTableSync(decodeTableId(item->getKey())));
						tableSync = dynamic_cast<DBDirectTableSync*>(tableSyncR.get());
						if(!tableSync)
						{
							break;
						}
					}
					tableSync->saveRegistrable(*item, transaction);
				}
			}
		}



		boost::shared_ptr<const util::Registrable> DBModule::GetObject(
			util::RegistryKeyType id,
			util::Env& env
		){
			return const_pointer_cast<const util::Registrable>(GetEditableObject(id, env));
		}



		boost::shared_ptr<util::Registrable> DBModule::GetEditableObject( util::RegistryKeyType id, util::Env& env )
		{
			RegistryTableType tableId(decodeTableId(id));
			shared_ptr<DBDirectTableSync> tableSync(
				dynamic_pointer_cast<DBDirectTableSync, DBTableSync>(
				GetTableSync(tableId)
				)	);
			if(!tableSync.get())
			{
				throw Exception("Incompatible table");
			}
			return tableSync->getEditableRegistrable(id, env);
		}
}	}
