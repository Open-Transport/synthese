
/** HandicappedComplianceTableSync class implementation.
	@file HandicappedComplianceTableSync.cpp

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

#include "Conversion.h"
#include "ReservationRule.h"
#include "ReservationRuleTableSync.h"

#include "DBModule.h"
#include "SQLiteResult.h"
#include "SQLite.h"
#include "SQLiteException.h"

#include "HandicappedCompliance.h"
#include "HandicappedComplianceTableSync.h"

using namespace std;
using namespace boost;
using boost::logic::tribool;

namespace synthese
{
	using namespace db;
	using namespace util;
	using namespace env;

	template<> const string util::FactorableTemplate<SQLiteTableSync,env::HandicappedComplianceTableSync>::FACTORY_KEY("15.10.04 Handicapped compliances");

	namespace db
	{
		template<> const std::string SQLiteTableSyncTemplate<HandicappedComplianceTableSync>::TABLE_NAME = "t019_handicapped_compliances";
		template<> const int SQLiteTableSyncTemplate<HandicappedComplianceTableSync>::TABLE_ID = 19;
		template<> const bool SQLiteTableSyncTemplate<HandicappedComplianceTableSync>::HAS_AUTO_INCREMENT = true;

		template<> void SQLiteDirectTableSyncTemplate<HandicappedComplianceTableSync,HandicappedCompliance>::Load(
			HandicappedCompliance* cmp,
			const db::SQLiteResultSPtr& rows,
			Env* env,
			LinkLevel linkLevel
		){
		    tribool status = true;
		    int statusInt (rows->getInt (HandicappedComplianceTableSync::COL_STATUS));

		    if (statusInt < 0)
		    {
			status = boost::logic::indeterminate;
		    }
		    else if (statusInt == 0)
		    {
			status = false;
		    }
		    
		    int capacity (
			rows->getInt (HandicappedComplianceTableSync::COL_CAPACITY));
		    
		    cmp->setCompliant (status);
		    cmp->setCapacity (capacity);

			if (linkLevel > FIELDS_ONLY_LOAD_LEVEL)
			{
				cmp->setReservationRule(
					ReservationRuleTableSync::Get(
						rows->getLongLong(HandicappedComplianceTableSync::COL_RESERVATION_RULE),
						env,
						linkLevel,
						AUTO_CREATE
				)	);
			}
		}


		template<> void SQLiteDirectTableSyncTemplate<HandicappedComplianceTableSync,HandicappedCompliance>::Save(HandicappedCompliance* object)
		{
			SQLite* sqlite = DBModule::GetSQLite();
			stringstream query;
			if (object->getKey() == UNKNOWN_VALUE)
				object->setKey(getId());
			query
				<< "REPLACE INTO " << TABLE_NAME << " VALUES("
				<< object->getKey() << ','
				<< Conversion::ToString(object->isCompliant()) << ","
				<< object->getCapacity() << ","
				<< (object->getReservationRule().get() ? Conversion::ToString(object->getReservationRule()->getKey()) : "0")
				<< ")";
			sqlite->execUpdate(query.str());
		}

		
		template<> void SQLiteDirectTableSyncTemplate<HandicappedComplianceTableSync, HandicappedCompliance>::Unlink(
			HandicappedCompliance* obj,
			Env* env
		){

		}

	}

	namespace env
	{
		const std::string HandicappedComplianceTableSync::COL_STATUS ("status");
		const std::string HandicappedComplianceTableSync::COL_CAPACITY ("capacity");
		const std::string HandicappedComplianceTableSync::COL_RESERVATION_RULE("reservation_rule");

		HandicappedComplianceTableSync::HandicappedComplianceTableSync()
			: SQLiteRegistryTableSyncTemplate<HandicappedComplianceTableSync,HandicappedCompliance>()
		{
			addTableColumn(TABLE_COL_ID, "INTEGER", false);
			addTableColumn(COL_STATUS, "INTEGER");
			addTableColumn(COL_CAPACITY, "INTEGER");
			addTableColumn(COL_RESERVATION_RULE, "INTEGER");
		}

		void HandicappedComplianceTableSync::Search(
			Env& env,
			int first /*= 0*/,
			int number /*= 0*/,
			LinkLevel linkLevel
		){
			stringstream query;
			query
				<< " SELECT *"
				<< " FROM " << TABLE_NAME
				<< " WHERE 1 " 
				;
			if (number > 0)
				query << " LIMIT " << Conversion::ToString(number + 1);
			if (first > 0)
				query << " OFFSET " << Conversion::ToString(first);
			
			LoadFromQuery(query.str(), env, linkLevel);
		}
	}
}

