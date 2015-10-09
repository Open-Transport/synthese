
/** OnlineReservationRuleTableSync class implementation.
	@file OnlineReservationRuleTableSync.cpp

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

#include "OnlineReservationRuleTableSync.h"

#include "DBException.hpp"
#include "DBModule.h"
#include "DBResult.hpp"
#include "Profile.h"
#include "ResaRight.h"
#include "ReplaceQuery.h"
#include "ReservationContact.h"
#include "ReservationContactTableSync.h"
#include "WebPageTableSync.h"

#include <sstream>
#include <boost/lexical_cast.hpp>

using namespace std;
using namespace boost;

namespace synthese
{
	using namespace db;
	using namespace util;
	using namespace resa;
	using namespace pt;
	using namespace cms;

	namespace util
	{
		template<> const string FactorableTemplate<DBTableSync,OnlineReservationRuleTableSync>::FACTORY_KEY(
			"31.5 Online Reservation Rule Table Sync"
		);
	}

	namespace db
	{
		template<> const DBTableSync::Format DBTableSyncTemplate<OnlineReservationRuleTableSync>::TABLE(
			"t047_online_reservation_rules"
		);

		template<> const Field DBTableSyncTemplate<OnlineReservationRuleTableSync>::_FIELDS[]=
		{
			Field()
		};

		template<>
		DBTableSync::Indexes DBTableSyncTemplate<OnlineReservationRuleTableSync>::GetIndexes()
		{
			DBTableSync::Indexes r;
			r.push_back(DBTableSync::Index(resa::Contact::FIELD.name.c_str(), ""));
			return r;
		}

		template<> bool DBTableSyncTemplate<OnlineReservationRuleTableSync>::CanDelete(
			const server::Session* session,
			util::RegistryKeyType object_id
		){
			//TODO check user rights
			return true;
		}



		template<> void DBTableSyncTemplate<OnlineReservationRuleTableSync>::BeforeDelete(
			util::RegistryKeyType id,
			db::DBTransaction& transaction
		){
		}



		template<> void DBTableSyncTemplate<OnlineReservationRuleTableSync>::AfterDelete(
			util::RegistryKeyType id,
			db::DBTransaction& transaction
		){
		}



		template<> void DBTableSyncTemplate<OnlineReservationRuleTableSync>::LogRemoval(
			const server::Session* session,
			util::RegistryKeyType id
		){
			//TODO Log the removal
		}

	}

	namespace resa
	{
		OnlineReservationRuleTableSync::SearchResult OnlineReservationRuleTableSync::Search(
			Env& env,
			int first /*= 0*/,
			int number /*= 0*/,
			LinkLevel linkLevel
		){
			stringstream query;
			query
				<< " SELECT *"
				<< " FROM " << TABLE.NAME
				<< " WHERE "
				/// @todo Fill Where criteria
				// eg << TABLE_COL_NAME << " LIKE '%" << Conversion::ToDBString(name, false) << "%'"
				;
			if (number > 0)
				query << " LIMIT " << (number + 1);
			if (first > 0)
				query << " OFFSET " << first;

			return LoadFromQuery(query.str(), env, linkLevel);
		}

		bool OnlineReservationRuleTableSync::allowList(const server::Session* session) const
		{
			return session && session->hasProfile() && session->getUser()->getProfile()->isAuthorized<ResaRight>(security::READ);
		}
	}
}
