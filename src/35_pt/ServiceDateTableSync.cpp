
/** ServiceDateTableSync class implementation.
	@file ServiceDateTableSync.cpp

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

#include <sstream>

#include "Registry.h"

#include "DBModule.h"
#include "SQLiteResult.h"
#include "SQLite.h"
#include "SQLiteException.h"

#include "ServiceDate.h"
#include "ServiceDateTableSync.h"
#include "NonPermanentService.h"
#include "Path.h"
#include "Fetcher.h"

using namespace std;
using namespace boost;
using namespace boost::gregorian;

namespace synthese
{
	using namespace db;
	using namespace util;
	using namespace env;
	using namespace pt;

	template<> const string util::FactorableTemplate<SQLiteTableSync,ServiceDateTableSync>::FACTORY_KEY("15.70.01 Service dates");

	namespace pt
	{
		const string ServiceDateTableSync::COL_SERVICEID ("service_id");
		const string ServiceDateTableSync::COL_DATE("date");
	}

	namespace db
	{
		template<> const SQLiteTableSync::Format SQLiteTableSyncTemplate<ServiceDateTableSync>::TABLE(
			"t005_service_dates"
			);

		template<> const SQLiteTableSync::Field SQLiteTableSyncTemplate<ServiceDateTableSync>::_FIELDS[]=
		{
			SQLiteTableSync::Field(TABLE_COL_ID, SQL_INTEGER, false),
			SQLiteTableSync::Field(ServiceDateTableSync::COL_SERVICEID, SQL_INTEGER, false),
			SQLiteTableSync::Field(ServiceDateTableSync::COL_DATE, SQL_DATE, false),
			SQLiteTableSync::Field()
		};

		template<> const SQLiteTableSync::Index SQLiteTableSyncTemplate<ServiceDateTableSync>::_INDEXES[]=
		{
			SQLiteTableSync::Index(ServiceDateTableSync::COL_SERVICEID.c_str(), ServiceDateTableSync::COL_DATE.c_str(), ""),
			SQLiteTableSync::Index()
		};
		
		
		template<> void SQLiteDirectTableSyncTemplate<ServiceDateTableSync,ServiceDate>::Load(
			ServiceDate* ss,
			const db::SQLiteResultSPtr& rows,
			Env& env,
			LinkLevel linkLevel
		){
			const date d(rows->getDate(ServiceDateTableSync::COL_DATE));
			ss->setDate(d);
			
			shared_ptr<NonPermanentService> service(
				Fetcher<NonPermanentService>::FetchEditable(
					rows->getLongLong(ServiceDateTableSync::COL_SERVICEID),
					env, linkLevel
			)	);

			ss->setService(service.get());
			
			if(linkLevel > FIELDS_ONLY_LOAD_LEVEL)
			{
				service->setActive(d);
			}
		}
		
		
		template<> void SQLiteDirectTableSyncTemplate<ServiceDateTableSync,ServiceDate>::Save(
			ServiceDate* object,
			optional<SQLiteTransaction&> transaction
		){
			if(!object->getService()) return;
			
			if (object->getKey() <= 0)
				object->setKey(getId());

			stringstream query;
			query
				<< " REPLACE INTO " << TABLE.NAME << " VALUES("
				<< object->getKey()
				<< "," << object->getService()->getKey()
				<< ",'" << to_iso_extended_string(object->getDate()) << "'"
				<< ")";
			DBModule::GetSQLite()->execUpdate(query.str(), transaction);
		}



		template<> void SQLiteDirectTableSyncTemplate<ServiceDateTableSync,ServiceDate>::Unlink(
			ServiceDate* ss
		){
			if(!ss->getService()) return;
			ss->getService()->setInactive(ss->getDate());
		}

	}

	namespace pt
	{
		void ServiceDateTableSync::DeleteDatesFromNow(
			util::RegistryKeyType serviceId
		){
			date now(day_clock::local_day());
			stringstream query;
			query <<
				"DELETE FROM " << TABLE.NAME <<
				" WHERE " << COL_SERVICEID << "=" << serviceId <<
				" AND " << COL_DATE << ">'" << to_iso_extended_string(now) << "'"
			;
			DBModule::GetSQLite()->execUpdate(query.str());
		}



		void ServiceDateTableSync::SetActiveDates(
			NonPermanentService& service
		){
			try
			{
				stringstream query;
				query
					<< "SELECT " << COL_DATE << " FROM " << TABLE.NAME << " WHERE " << COL_SERVICEID << "="
					<< service.getKey() << " ORDER BY " << COL_DATE;
				
				SQLiteResultSPtr rows = DBModule::GetSQLite()->execQuery(query.str());
				service.clear();
				while(rows->next())
				{
					service.setActive(rows->getDate(COL_DATE));
				}
			}
			catch(SQLiteException& e)
			{
				throw Exception(e.getMessage());
			}
		}
	}
}
