
/** PedestrianComplianceTableSync class implementation.
	@file PedestrianComplianceTableSync.cpp

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

#include "DBModule.h"
#include "SQLiteResult.h"
#include "SQLite.h"
#include "SQLiteException.h"

#include "PedestrianComplianceTableSync.h"

using namespace std;
using namespace boost;
using boost::logic::tribool;

namespace synthese
{
	using namespace db;
	using namespace util;
	using namespace env;

	template<> const string util::FactorableTemplate<SQLiteTableSync,PedestrianComplianceTableSync>::FACTORY_KEY("15.10.05 Pedestrian compliances");

	namespace db
	{
		template<> const std::string SQLiteTableSyncTemplate<PedestrianComplianceTableSync>::TABLE_NAME = "t018_pedestrian_compliances";
		template<> const int SQLiteTableSyncTemplate<PedestrianComplianceTableSync>::TABLE_ID = 18;
		template<> const bool SQLiteTableSyncTemplate<PedestrianComplianceTableSync>::HAS_AUTO_INCREMENT = true;

		template<> void SQLiteDirectTableSyncTemplate<PedestrianComplianceTableSync,PedestrianCompliance>::Load(
			PedestrianCompliance* cmp,
			const db::SQLiteResultSPtr& rows,
			Env* env,
			LinkLevel linkLevel
		){
			tribool status = true;
			int statusInt (
			    rows->getInt (PedestrianComplianceTableSync::COL_STATUS));
			if (statusInt < 0)
			{
				status = boost::logic::indeterminate;
			}
			else if (statusInt == 0)
			{
				status = false;
			}

			int capacity (
				rows->getInt (PedestrianComplianceTableSync::COL_CAPACITY));

			cmp->setCompliant (status);
			cmp->setCapacity (capacity);
		}

		template<> void SQLiteDirectTableSyncTemplate<PedestrianComplianceTableSync,PedestrianCompliance>::Save(PedestrianCompliance* object)
		{
			SQLite* sqlite = DBModule::GetSQLite();
			stringstream query;
			if (object->getKey() > 0)
			{
				query
					<< "UPDATE " << TABLE_NAME << " SET "
					/// @todo fill fields [,]FIELD=VALUE
					<< " WHERE " << TABLE_COL_ID << "=" << Conversion::ToString(object->getKey());
			}
			else
			{
				object->setKey(getId());
                query
					<< " INSERT INTO " << TABLE_NAME << " VALUES("
					<< Conversion::ToString(object->getKey())
					/// @todo fill other fields separated by ,
					<< ")";
			}
			sqlite->execUpdate(query.str());
		}



		template<> void SQLiteDirectTableSyncTemplate<PedestrianComplianceTableSync,PedestrianCompliance>::Unlink(
			PedestrianCompliance* obj,
			Env* env)
		{

		}

	}

	namespace env
	{
		const std::string PedestrianComplianceTableSync::COL_STATUS ("status");
		const std::string PedestrianComplianceTableSync::COL_CAPACITY ("capacity");

		PedestrianComplianceTableSync::PedestrianComplianceTableSync()
			: SQLiteRegistryTableSyncTemplate<PedestrianComplianceTableSync,PedestrianCompliance>()
		{
			addTableColumn(TABLE_COL_ID, "INTEGER", false);
			addTableColumn (COL_STATUS, "INTEGER");
			addTableColumn (COL_CAPACITY, "INTEGER");
		}

		void PedestrianComplianceTableSync::Search(
			Env& env,
			int first /*= 0*/, int number, /*= 0*/
			LinkLevel linkLevel)
		{
			stringstream query;
			query
				<< " SELECT *"
				<< " FROM " << TABLE_NAME
				<< " WHERE " 
				/// @todo Fill Where criteria
				// eg << TABLE_COL_NAME << " LIKE '%" << Conversion::ToSQLiteString(name, false) << "%'"
				;
			if (number > 0)
				query << " LIMIT " << Conversion::ToString(number + 1);
			if (first > 0)
				query << " OFFSET " << Conversion::ToString(first);

			LoadFromQuery(query.str(), env, linkLevel);
		}
	}
}

