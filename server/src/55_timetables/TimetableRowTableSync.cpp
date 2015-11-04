
/** TimetableRowTableSync class implementation.
	@file TimetableRowTableSync.cpp
	@author Hugues Romain
	@date 2008

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

#include "TimetableRowTableSync.h"

#include "DBModule.h"
#include "DBResult.hpp"
#include "Profile.h"
#include "RankUpdateQuery.hpp"
#include "ReplaceQuery.h"
#include "SelectQuery.hpp"
#include "Session.h"
#include "StopAreaTableSync.hpp"
#include "TimetableRight.h"
#include "User.h"

using namespace std;
using namespace boost;

namespace synthese
{
	using namespace db;
	using namespace util;
	using namespace timetables;
	using namespace pt;
	using namespace security;

	namespace util
	{
		template<> const string FactorableTemplate<DBTableSync,TimetableRowTableSync>::FACTORY_KEY("55.02 Timetable rows");
	}

	namespace db
	{
		template<> const DBTableSync::Format DBTableSyncTemplate<TimetableRowTableSync>::TABLE(
			"t053_timetable_rows"
		);


		template<> const Field DBTableSyncTemplate<TimetableRowTableSync>::_FIELDS[]=
		{
			Field()
		};

		template<> DBTableSync::Indexes DBTableSyncTemplate<TimetableRowTableSync>::GetIndexes()
		{
			DBTableSync::Indexes r;
			r.push_back(
				DBTableSync::Index(
					TimetableParent::FIELD.name.c_str(),
					Rank::FIELD.name.c_str(),
			"")	);
			return r;
		}



		template<> bool DBTableSyncTemplate<TimetableRowTableSync>::CanDelete(
			const server::Session* session,
			util::RegistryKeyType object_id
		){
			return session && session->hasProfile() && session->getUser()->getProfile()->isAuthorized<TimetableRight>(WRITE);
		}



		template<> void DBTableSyncTemplate<TimetableRowTableSync>::BeforeDelete(
			util::RegistryKeyType id,
			db::DBTransaction& transaction
		){
		}



		template<> void DBTableSyncTemplate<TimetableRowTableSync>::AfterDelete(
			util::RegistryKeyType id,
			db::DBTransaction& transaction
		){
			Env env;
			boost::shared_ptr<const TimetableRow> row(TimetableRowTableSync::Get(id, env));
			TimetableRowTableSync::Shift(row->getTimetableId(), row->getRank(), -1, transaction);
		}



		template<> void DBTableSyncTemplate<TimetableRowTableSync>::LogRemoval(
			const server::Session* session,
			util::RegistryKeyType id
		){
			//TODO log the removal
		}
	}



	namespace timetables
	{
		TimetableRowTableSync::SearchResult TimetableRowTableSync::Search(
			Env& env,
			optional<RegistryKeyType> timetableId
			, bool orderByTimetable
			, bool raisingOrder
			, int first
			, boost::optional<std::size_t> number,
			LinkLevel linkLevel
		){
			SelectQuery<TimetableRowTableSync> query;

			// Selection
			if (timetableId)
			{
				query.addWhereField(TimetableParent::FIELD.name, *timetableId);
			}

			// Ordering
			if (orderByTimetable)
			{
				query.addOrderField(TimetableParent::FIELD.name, raisingOrder);
				query.addOrderField(Rank::FIELD.name, raisingOrder);
			}

			// Size
			if (number)
				query.setNumber(*number + 1);
			if (first > 0)
				query.setFirst(first);

			return LoadFromQuery(query, env, linkLevel);
		}



		void TimetableRowTableSync::Shift(
			RegistryKeyType timetableId,
			int rank,
			int delta,
			optional<DBTransaction&> transaction
		){
			RankUpdateQuery<TimetableRowTableSync> query(Rank::FIELD.name, delta, rank);
			query.addWhereField(TimetableParent::FIELD.name, timetableId);
			query.execute(transaction);
		}



		optional<size_t> TimetableRowTableSync::GetMaxRank( util::RegistryKeyType timetableId )
		{
			DB* db = DBModule::GetDB();

			stringstream query;

			// Content
			query
				<< "SELECT MAX(" << Rank::FIELD.name << ") AS mr "
				<< " FROM " << TABLE.NAME
				<< " WHERE " << TimetableId::FIELD.name << "=" << timetableId
			;

			try
			{
				DBResultSPtr rows = db->execQuery(query.str());
				while (rows->next ())
				{
					return rows->getOptionalUnsignedInt("mr");
				}
				return optional<size_t>();
			}
			catch(DBException& e)
			{
				throw Exception(e.getMessage());
			}
		}

		bool TimetableRowTableSync::allowList(const server::Session* session) const
		{
			return session && session->hasProfile() && session->getUser()->getProfile()->isAuthorized<TimetableRight>(security::READ);
		}
	}
}
