
/** DB class implementation.
	@file DB.cpp
	@author Sylvain Pasche
	@date 2011

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

#include "DB.hpp"

#include "CoordinatesSystem.hpp"
#include "DBException.hpp"
#include "DBInterSYNTHESE.hpp"
#include "DBModule.h"
#include "Factory.h"
#include "InterSYNTHESEContent.hpp"
#include "InterSYNTHESEModule.hpp"
#include "DBTableSync.hpp"
#include "DBTransaction.hpp"
#include "Log.h"
#include "ObjectBase.hpp"

#include <boost/lexical_cast.hpp>
#include <boost/shared_ptr.hpp>
#include <string>
#ifdef DO_VERIFY_TRIGGER_EVENTS
#include <boost/functional/hash.hpp>
#endif

using namespace boost;
using namespace std;
using boost::shared_ptr;
using boost::lexical_cast;
using boost::recursive_mutex;

namespace synthese
{
	using namespace inter_synthese;
	using namespace util;

	namespace db
	{
		DB::ConnectionInfo::ConnectionInfo(const string& connectionString) :
			port(0), debug(false), triggerCheck(true), noTrigger(false)
		{
			string::const_iterator it = connectionString.begin(),
				end = connectionString.end();

			string::size_type pos = connectionString.find("://");
			if (pos == 0 || pos == string::npos)
			{
				throw InvalidConnectionStringException("Can't parse backend");
			}

			this->backend.assign(it, it + pos);
			it += pos + 3;

			if (!util::Factory<DB>::contains(this->backend))
			{
				throw InvalidConnectionStringException("No factory for backend: " + this->backend);
			}

			string::const_iterator paramStart, valueStart;
			string param, value;

			while (it != end) {
				paramStart = it;
				while (it != end && *it != '=')
					it++;
				if (it == end)
					throw InvalidConnectionStringException("Can't parse parameter");
				param.assign(paramStart, it);

				valueStart = ++it;
				if (valueStart == end)
					throw InvalidConnectionStringException("Can't parse value");
				while (it != end && *it != ',')
					it++;
				value.assign(valueStart, it);
				if (it != end)
					it++;

				if (param == "path") { this->path = value; }
				else if (param == "host") { this->host = value; }
				else if (param == "user") { this->user = value; }
				else if (param == "passwd") { this->passwd = value; }
				else if (param == "db") { this->db = value; }
				else if (param == "triggerHost") { this->triggerHost = value; }
				else if (param == "port") { this->port = boost::lexical_cast<int>(value); }
				else if (param == "debug") { this->debug = boost::lexical_cast<bool>(value); }
				else if (param == "triggerCheck") { this->triggerCheck = boost::lexical_cast<bool>(value); }
				else if (param == "noTrigger") { this->noTrigger = boost::lexical_cast<bool>(value); }
				else
				{
					throw InvalidConnectionStringException("Unknown parameter " + param);
				}
			}
		}



		DB::DBModifEvent::DBModifEvent(std::string _table, DBModifType _type, util::RegistryKeyType _id) :
			table(_table), type(_type), id(_id)
		{ }



		DB::DBModifEvent::DBModifEvent()
		{ }



#ifdef DO_VERIFY_TRIGGER_EVENTS
		bool operator==(DB::DBModifEvent const& me1, DB::DBModifEvent const& me2)
		{
			// SQLite hook sends INSERT events with REPLACE INTO when the data is already
			// there (it should rather be an UPDATE). Consider INSERT and UPDATE as equal.
			DB::DBModifType type1(me1.type), type2(me2.type);
			if (type1 == DB::MODIF_UPDATE) type1 = DB::MODIF_INSERT;
			if (type2 == DB::MODIF_UPDATE) type2 = DB::MODIF_INSERT;
			return me1.table == me2.table && type1 == type2 && me1.id == me2.id;
		}



		std::size_t hash_value(DB::DBModifEvent const& modifEvent) {
			std::size_t seed = 0;
			boost::hash_combine(seed, modifEvent.table);

			// See comment in operator==
			DB::DBModifType type(modifEvent.type);
			if (type == DB::MODIF_UPDATE) type = DB::MODIF_INSERT;

			boost::hash_combine(seed, type);
			boost::hash_combine(seed, modifEvent.id);
			return seed;
		}
#endif



		// All coordinates are stored in WGS84 (lat-lon).
		const CoordinatesSystem::SRID DB::_STORAGE_COORD_SYSTEM_SRID(4326);
		// The default instance coordinates system is Lambert zone II.
		// It can be changed using a parameter.
		const CoordinatesSystem::SRID DB::_DEFAULT_INSTANCE_COORD_SYSTEM_SRID(27572);

		DB::DB() :
			_schemaUpdated(false),
			_standalone(false)
		{
		}



		DB::~DB()
		{
			if(_standalone)
				return;

			CoordinatesSystem::ClearCoordinatesSystems();
			// FIXME : the next line caused a double free
			// DBModule::UnregisterParameter(CoordinatesSystem::_INSTANCE_COORDINATES_SYSTEM);
		}



		void DB::setConnectionInfo(boost::shared_ptr<ConnectionInfo> connInfo)
		{
			_connInfo = connInfo;
		}



		void DB::setStandaloneUse(bool standalone)
		{
			_standalone = standalone;
		}



		void DB::preInit()
		{
			initDatabase();

			if (!doesTableExists("spatial_ref_sys"))
			{
				initSpatialRefSysTable();
			}

			DBTransaction t;
#include "spatial_ref_sys_sql.inc.h"
			t.run();

			DBResultSPtr systems(DBModule::GetDB()->execQuery("SELECT * FROM spatial_ref_sys;"));
			while(systems->next())
			{
				CoordinatesSystem::AddCoordinatesSystem(
					systems->getInt("auth_srid"),
					systems->getText("ref_sys_name"),
					systems->getText("proj4text")
				);
			}

			CoordinatesSystem::SetStorageCoordinatesSystem(
				CoordinatesSystem::GetCoordinatesSystem(_STORAGE_COORD_SYSTEM_SRID)
			);
			DBModule::RegisterParameter(
				CoordinatesSystem::_INSTANCE_COORDINATES_SYSTEM,
				boost::lexical_cast<std::string>(_DEFAULT_INSTANCE_COORD_SYSTEM_SRID),
				CoordinatesSystem::ChangeInstanceCoordinatesSystem
			);
		}



		void DB::init()
		{
			recursive_mutex::scoped_lock lock(_tableSynchronizersMutex);
			vector<boost::shared_ptr<DBTableSync> > tableSyncs(Factory<DBTableSync>::GetNewCollection());

			// Call the update schema step on all synchronizers.
			BOOST_FOREACH(const boost::shared_ptr<DBTableSync> tableSync, tableSyncs)
			{
				Log::GetInstance().info("Updating schema for table " + tableSync->getFactoryKey() + "...");
			    try
			    {
					tableSync->updateSchema(this);
			    }
			    catch (std::exception& e)
			    {
					Log::GetInstance().error("Error during schema update of " + tableSync->getFactoryKey() + ".", e);
					if (_connInfo->debug)
						throw;
			    }
			}

			_schemaUpdated = true;

			Log::GetInstance().info("Initializing local auto-increments...");
			BOOST_FOREACH(const boost::shared_ptr<DBTableSync> tableSync, tableSyncs)
			{
				tableSync->initAutoIncrement();
			}

			// Call the first sync step on all synchronizers.
			BOOST_FOREACH(const boost::shared_ptr<DBTableSync> tableSync, tableSyncs)
			{
			    Log::GetInstance().info("Loading table " + tableSync->getFactoryKey() +"...");
			    try
			    {
					tableSync->firstSync(this);
			    }
			    catch (std::exception& e)
			    {
					Log::GetInstance().error("Unattended error during first sync of " + tableSync->getFactoryKey() +
							  ". In-memory data might be inconsistent.", e);
					if (_connInfo->debug)
						throw;
			    }
			}

#ifdef DO_VERIFY_TRIGGER_EVENTS
			_recordedEvents.clear();
#endif
		}



		void DB::execUpdate(
			const SQLData& sql,
			boost::optional<DBTransaction&> transaction
		){
			if(transaction)
			{
				transaction->addQuery(sql);
				return;
			}
			_doExecUpdate(sql);
		}



		void DB::addDBModifEvent(const DBModifEvent& modifEvent, boost::optional<DBTransaction&> transaction)
		{
			if (transaction)
			{
				transaction->addDBModifEvent(modifEvent);
				return;
			}
			_dispatchDBModifEvent(modifEvent);
		}




#ifdef DO_VERIFY_TRIGGER_EVENTS
		void DB::_recordDBModifEvents(const std::vector<DBModifEvent>& modifEvents)
		{
			const DBModule::TablesByNameMap& tm(DBModule::GetTablesByName());

			BOOST_FOREACH(const DBModifEvent& modifEvent, modifEvents)
			{
				DBModule::TablesByNameMap::const_iterator it(tm.find(modifEvent.table));
				if(it == tm.end())
					continue;

				Log::GetInstance().trace("Recording event: DBModifEvent: table: " + modifEvent.table +
					" type: " + lexical_cast<string>(modifEvent.type) +
					" id: " + lexical_cast<string>(modifEvent.id));

				_recordedEvents.insert(modifEvent);
			}
		}



		void DB::checkModificationEvents()
		{
			if (_recordedEvents.empty())
				return;
			if (!_dbModifCheckEnabled())
			{
				_recordedEvents.clear();
				return;
			}

			BOOST_FOREACH(const DBModifEvent& modifEvent, _recordedEvents)
			{
				Log::GetInstance().warn("unbalanced event: DBModifEvent: table: " + modifEvent.table +
					" type: " + lexical_cast<string>(modifEvent.type) +
					" id: " + lexical_cast<string>(modifEvent.id));
			}
			int size = _recordedEvents.size();
			_recordedEvents.clear();
			throw DBException("Got " + lexical_cast<string>(size) +
				" DBModifEvent(s) from the trigger which weren't balanced with row* calls");
		}
#endif




		void DB::initDatabase()
		{
		}



		void DB::afterUpdateSchema(
			const std::string& tableName,
			const FieldsList& fields
		){}



		std::string DB::_getIndexName(const std::string& tableName, const DBTableSync::Index& index)
		{
			std::stringstream s;
			s << tableName;
			BOOST_FOREACH(const std::string& field, index.fields)
			{
				s << "_" << field;
			}
			return s.str();
		}


		void DB::_finishTransaction(const DBTransaction& transaction)
		{
			BOOST_FOREACH(const DBModifEvent& modifEvent, transaction.getDBModifEvents())
			{
				_dispatchDBModifEvent(modifEvent);
			}
		}


		void DB::_dispatchDBModifEvent(const DBModifEvent& modifEvent)
		{
#ifdef DO_VERIFY_TRIGGER_EVENTS
			if (_recordedEvents.erase(modifEvent) == 0 &&
				_dbModifCheckEnabled())
			{
				throw DBException("rowAdded: event not recorded from trigger. "
					"table: " + modifEvent.table +
					" type: " + lexical_cast<string>(modifEvent.type) +
					" id: " + lexical_cast<string>(modifEvent.id));
			}
#endif

			if (_schemaUpdated == false) return;

			recursive_mutex::scoped_lock lock(_tableSynchronizersMutex);
			const DBModule::TablesByNameMap& tm(DBModule::GetTablesByName());

			Log::GetInstance().trace(
				"Dispatching event: table: " + modifEvent.table + " type: " +
				lexical_cast<string>(modifEvent.type) + " id: " + lexical_cast<string>(modifEvent.id)
			);

			DBModule::TablesByNameMap::const_iterator it(tm.find(modifEvent.table));
			if(it == tm.end())
			{
				// This is not fatal, some backends can fire events on tables we're not interested in (SQLite fires events on index tables for instance).
				return;
			}
			DBTableSync* tableSync = it->second.get();

			if (modifEvent.type == MODIF_INSERT)
			{
				tableSync->rowsAdded(this, tableSync->getRow(modifEvent.id));
			}
			else if (modifEvent.type == MODIF_UPDATE)
			{
				tableSync->rowsUpdated(this, tableSync->getRow(modifEvent.id));
			}
			else if (modifEvent.type == MODIF_DELETE)
			{
				RowIdList rowIds;
				rowIds.push_back(modifEvent.id);

				tableSync->rowsRemoved(this, rowIds);
			}
		}



		void DB::replaceStmt(
			ObjectBase& o,
			optional<DBTransaction&> transaction
		){
			// Query
			DBRecord r(*DBModule::GetTableSync(o.getClassNumber()));
			if(!o.getKey())
			{
				o.setKey(r.getTable()->getNewId());
			}
			r.setContent(o.toDBContent());

			replaceStmt(o.getKey(), r, transaction);
		}



		void DB::replaceStmt(
			util::RegistryKeyType id,
			const DBRecord& r,
			boost::optional<DBTransaction&> transaction
		){
			if(transaction)
			{
				transaction->addReplaceStmt(r);
			}
			else
			{
				saveRecord(r);
			}
			addDBModifEvent(
				DBModifEvent(
					r.getTable()->getFormat().NAME,
					MODIF_INSERT,
					id
				),
				transaction
			);

#ifdef DO_VERIFY_TRIGGER_EVENTS
			checkModificationEvents();
#endif

			// Inter-SYNTHESE sync
			if(Factory<InterSYNTHESESyncTypeFactory>::size()) // Avoid in unit tests
			{
				inter_synthese::InterSYNTHESEContent content(
					DBInterSYNTHESE::FACTORY_KEY,
					lexical_cast<string>(r.getTable()->getFormat().ID),
					DBInterSYNTHESE::GetReplaceStmtContent(r)
				);
				inter_synthese::InterSYNTHESEModule::Enqueue(
					content,
					transaction
				);
			}
		}



		void DB::deleteStmt(
			util::RegistryKeyType objectId,
			boost::optional<DBTransaction&> transaction
		){
			RegistryTableType tableId(
				decodeTableId(objectId)
			);

			if(transaction)
			{
				transaction->addDeleteStmt(objectId);
			}
			else
			{
				deleteRow(objectId);
			}
			addDBModifEvent(
				DBModifEvent(
					DBModule::GetTableSync(tableId)->getFormat().NAME,
					MODIF_DELETE,
					objectId
				),
				transaction
			);

#ifdef DO_VERIFY_TRIGGER_EVENTS
			checkModificationEvents();
#endif

			// Inter-SYNTHESE sync
			if(Factory<InterSYNTHESESyncTypeFactory>::size()) // Avoid in unit tests
			{
				inter_synthese::InterSYNTHESEContent content(
					DBInterSYNTHESE::FACTORY_KEY,
					lexical_cast<string>(tableId),
					DBInterSYNTHESE::GetDeleteStmtContent(objectId)
				);
				inter_synthese::InterSYNTHESEModule::Enqueue(
					content,
					transaction
				);
			}
		}
}	}
