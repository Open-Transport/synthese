
/** VinciBikeTableSync class implementation.
	@file VinciBikeTableSync.cpp

	This file belongs to the VINCI BIKE RENTAL SYNTHESE module
	Copyright (C) 2006 Vinci Park 
	Contact : Raphaël Murat - Vinci Park <rmurat@vincipark.com>

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

#include "01_util/Exception.h"

#include "02_db/DBModule.h"

#include "71_vinci_bike_rental/VinciBike.h"
#include "71_vinci_bike_rental/VinciBikeTableSync.h"

using namespace std;
using boost::shared_ptr;

namespace synthese
{
	using namespace db;
	using namespace vinci;
	using namespace util;

	namespace db
	{
		template<> const std::string SQLiteTableSyncTemplate<VinciBike>::TABLE_NAME = "t032_vinci_bike";
		template<> const int SQLiteTableSyncTemplate<VinciBike>::TABLE_ID = 32;
		template<> const bool SQLiteTableSyncTemplate<VinciBike>::HAS_AUTO_INCREMENT = true;

		template<> void SQLiteTableSyncTemplate<VinciBike>::load(VinciBike* bike, const db::SQLiteResult& rows, int rowId)
		{
			bike->setKey(Conversion::ToLongLong(rows.getColumn(rowId, VinciBikeTableSync::TABLE_COL_ID)));
			bike->_number = rows.getColumn(rowId, VinciBikeTableSync::TABLE_COL_NUMBER);
			bike->_markedNumber = rows.getColumn(rowId, VinciBikeTableSync::TABLE_COL_MARKED_NUMBER);
		}

		template<> void SQLiteTableSyncTemplate<VinciBike>::save(VinciBike* bike)
		{
			const db::SQLiteQueueThreadExec* sqlite = DBModule::GetSQLite();
			stringstream query;
			if (bike->getKey() != 0)
			{	//UPODATE
				query << "UPDATE " << TABLE_NAME << " SET "
					<< VinciBikeTableSync::TABLE_COL_ID << "=" << Conversion::ToString(bike->getKey())
					<< "," << VinciBikeTableSync::TABLE_COL_NUMBER << "=" << Conversion::ToSQLiteString(bike->_number)
					<< "," << VinciBikeTableSync::TABLE_COL_MARKED_NUMBER << "=" << Conversion::ToSQLiteString(bike->_markedNumber)
					;
			}
			else
			{	// INSERT
				bike->setKey(getId(1,1)); /// @todo Handle grid number
				query << "INSERT INTO " << TABLE_NAME << " VALUES("
					<< Conversion::ToString(bike->getKey())
					<< "," << Conversion::ToSQLiteString(bike->_number)
					<< "," << Conversion::ToSQLiteString(bike->_markedNumber)
					<< ")";
			}
			sqlite->execUpdate(query.str());
		}

	}

	namespace vinci
	{
		const std::string VinciBikeTableSync::TABLE_COL_ID = "id";
		const std::string VinciBikeTableSync::TABLE_COL_NUMBER = "number";
		const std::string VinciBikeTableSync::TABLE_COL_MARKED_NUMBER = "marked_number";

		VinciBikeTableSync::VinciBikeTableSync()
			: db::SQLiteTableSyncTemplate<VinciBike>(TABLE_NAME, true, true, TRIGGERS_ENABLED_CLAUSE)
		{
			addTableColumn(TABLE_COL_ID, "INTEGER", false);
			addTableColumn(TABLE_COL_NUMBER, "TEXT", true);
			addTableColumn(TABLE_COL_MARKED_NUMBER, "TEXT", true);
		}

		
		/** Action to do on user creation.
		No action because the users are not permanently loaded in ram.
		*/
		void VinciBikeTableSync::rowsAdded (const db::SQLiteQueueThreadExec* sqlite, 
			db::SQLiteSync* sync,
			const db::SQLiteResult& rows, bool isFirstSync)
		{}

		/** Action to do on user creation.
		Updates the users objects in the opened sessions.
		*/
		void VinciBikeTableSync::rowsUpdated (const db::SQLiteQueueThreadExec* sqlite, 
			db::SQLiteSync* sync,
			const db::SQLiteResult& rows)
		{}

		/** Action to do on user deletion.
		Closes the sessions of the deleted user.
		*/
		void VinciBikeTableSync::rowsRemoved (const db::SQLiteQueueThreadExec* sqlite, 
			db::SQLiteSync* sync,
			const db::SQLiteResult& rows)
		{}

		std::vector<shared_ptr<VinciBike> > VinciBikeTableSync::search(
			const std::string& id
			, const std::string& cadre 
			, int first /*= 0*/
			, int number /*= 0*/
			, bool orderByNumber
			, bool orderByCadre
			, bool raisingOrder
			)
		{
			const db::SQLiteQueueThreadExec* sqlite = DBModule::GetSQLite();

			stringstream query;
			query 
				<< " SELECT * FROM " << TABLE_NAME
				<< " WHERE 1 ";
			if (!id.empty())
				query << " AND " << TABLE_COL_NUMBER << "='" << id << "' ";
			if (!cadre.empty())
				query << " AND " << TABLE_COL_MARKED_NUMBER << " LIKE '%" << cadre << "%' ";
			if (orderByNumber)
				query << " ORDER BY " << TABLE_COL_NUMBER << (raisingOrder ? " ASC" : " DESC");
			if (orderByCadre)
				query << " ORDER BY " << TABLE_COL_MARKED_NUMBER << (raisingOrder ? " ASC" : " DESC");
			if (number > 0)
				query << " LIMIT " << (number + 1);
			if (number > 0)
				query << " OFFSET " << first;

			SQLiteResult result = sqlite->execQuery(query.str());
			vector<shared_ptr<VinciBike> > bikes;
			for (int i=0; i<result.getNbRows(); ++i)
			{
				shared_ptr<VinciBike> bike(new VinciBike);
				try
				{
					load(bike, result, i);
					bikes.push_back(bike);
				}
				catch (Exception e)
				{

				}
			}
			return bikes;
		}

	}
}

