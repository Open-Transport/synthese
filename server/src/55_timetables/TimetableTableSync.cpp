
/** TimetableTableSync class implementation.
	@file TimetableTableSync.cpp
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

#include "TimetableTableSync.h"

#include "CalendarTemplateElementTableSync.h"
#include "CalendarTemplateTableSync.h"
#include "CommercialLine.h"
#include "Conversion.h"
#include "DBException.hpp"
#include "DBModule.h"
#include "DBResult.hpp"
#include "Profile.h"
#include "RankUpdateQuery.hpp"
#include "ReplaceQuery.h"
#include "Session.h"
#include "StopArea.hpp"
#include "StopPoint.hpp"
#include "Timetable.h"
#include "TimetableRight.h"
#include "TimetableRow.h"
#include "TimetableRowTableSync.h"
#include "TimetableRowGroupTableSync.hpp"
#include "User.h"

#include <sstream>

using namespace std;
using namespace boost;

namespace synthese
{
	using namespace db;
	using namespace util;
	using namespace timetables;
	using namespace calendar;
	using namespace pt;
	using namespace security;

	namespace util
	{
		template<> const string FactorableTemplate<DBTableSync,TimetableTableSync>::FACTORY_KEY("55.01 Timetables");
	}

	namespace db
	{
		template<> const DBTableSync::Format DBTableSyncTemplate<TimetableTableSync>::TABLE(
			"t052_timetables"
		);

		template<> const Field DBTableSyncTemplate<TimetableTableSync>::_FIELDS[]=
		{
			Field()
		};

		template<>
		DBTableSync::Indexes DBTableSyncTemplate<TimetableTableSync>::GetIndexes()
		{
			DBTableSync::Indexes r;
			r.push_back(
				DBTableSync::Index(
					Book::FIELD.name.c_str(),
					Rank::FIELD.name.c_str(),
			"")	);
			return r;
		}



		template<> bool DBTableSyncTemplate<TimetableTableSync>::CanDelete(
			const server::Session* session,
			util::RegistryKeyType object_id
		){
			return session && session->hasProfile() && session->getUser()->getProfile()->isAuthorized<TimetableRight>(DELETE_RIGHT);
		}



		template<> void DBTableSyncTemplate<TimetableTableSync>::BeforeDelete(
			util::RegistryKeyType id,
			db::DBTransaction& transaction
		){
			Env env;
			TimetableRowTableSync::SearchResult rows(TimetableRowTableSync::Search(env, id));
			BOOST_FOREACH(const TimetableRowTableSync::SearchResult::value_type& row, rows)
			{
				TimetableRowTableSync::Remove(NULL, row->getKey(), transaction, false);
			}
		}



		template<> void DBTableSyncTemplate<TimetableTableSync>::AfterDelete(
			util::RegistryKeyType id,
			db::DBTransaction& transaction
		){
		}



		template<> void DBTableSyncTemplate<TimetableTableSync>::LogRemoval(
			const server::Session* session,
			util::RegistryKeyType id
		){
			//TODO log the removal
		}
	}



	namespace timetables
	{
		TimetableTableSync::SearchResult TimetableTableSync::Search(
			Env& env,
			boost::optional<util::RegistryKeyType> bookId
			, bool orderByParent
			, bool orderByTitle
			, bool raisingOrder
			, int first
			, int number,
			LinkLevel linkLevel
		){
			SelectQuery<TimetableTableSync> query;

			// Selection
			if (bookId)
			{
				query.addWhereField(Book::FIELD.name, *bookId);
			}

			// Ordering
			if (orderByParent)
			{
				query.addOrderField(Book::FIELD.name, raisingOrder);
				query.addOrderField(Rank::FIELD.name, raisingOrder);
			}
			else if (orderByTitle)
			{
				query.addOrderField(Title::FIELD.name, raisingOrder);
			}

			// Size
			if (number > 0)
				query.setNumber(number + 1);
			if (first > 0)
				query.setFirst(first);

			return LoadFromQuery(query, env, linkLevel);
		}



		void TimetableTableSync::Shift( util::RegistryKeyType bookId , int rank , int delta )
		{
			RankUpdateQuery<TimetableTableSync> query(Rank::FIELD.name, delta, rank);
			query.addWhereField(Book::FIELD.name, bookId);
			query.execute();
		}



		optional<size_t> TimetableTableSync::GetMaxRank( util::RegistryKeyType bookId )
		{
			DB* db = DBModule::GetDB();

			SelectQuery<TimetableTableSync> query;

			// Query to select MAX rank AS mr
			query.addTableField(Rank::FIELD.name, "mr");
			query.addWhereField(Book::FIELD.name, bookId);
			query.addOrderField(Rank::FIELD.name, false);
			query.setNumber(1);


			try
			{
				DBResultSPtr rows = db->execQuery(query.toString());
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
}	}
