////////////////////////////////////////////////////////////////////////////////
/// ReservationRuleTableSync class implementation.
///	@file ReservationRuleTableSync.cpp
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

#include "ReservationContactTableSync.h"

#include "DBModule.h"
#include "DBResult.hpp"
#include "DBException.hpp"
#include "Profile.h"
#include "Session.h"
#include "TransportNetworkRight.h"
#include "User.h"

#include <sstream>

using namespace std;
using namespace boost;

namespace synthese
{
	using namespace db;
	using namespace util;
	using namespace pt;
	using namespace security;

	template<> const string util::FactorableTemplate<DBTableSync,ReservationContactTableSync>::FACTORY_KEY = "35.10.06 Reservation contacts";

	namespace db
	{
		template<> const DBTableSync::Format DBTableSyncTemplate<ReservationContactTableSync>::TABLE(
			"t021_reservation_contacts"
		);



		template<> const Field DBTableSyncTemplate<ReservationContactTableSync>::_FIELDS[]=
		{
			Field()
		};



		template<>
		DBTableSync::Indexes DBTableSyncTemplate<ReservationContactTableSync>::GetIndexes()
		{
			return DBTableSync::Indexes();
		}



		template<> bool DBTableSyncTemplate<ReservationContactTableSync>::CanDelete(
			const server::Session* session,
			util::RegistryKeyType object_id
		){
			return session && session->hasProfile() && session->getUser()->getProfile()->isAuthorized<TransportNetworkRight>(DELETE_RIGHT);
		}



		template<> void DBTableSyncTemplate<ReservationContactTableSync>::BeforeDelete(
			util::RegistryKeyType id,
			db::DBTransaction& transaction
		){
		}



		template<> void DBTableSyncTemplate<ReservationContactTableSync>::AfterDelete(
			util::RegistryKeyType id,
			db::DBTransaction& transaction
		){
		}



		template<> void DBTableSyncTemplate<ReservationContactTableSync>::LogRemoval(
			const server::Session* session,
			util::RegistryKeyType id
		){
			//TODO Log the removal
		}
	}

	namespace pt
	{
		ReservationContactTableSync::SearchResult ReservationContactTableSync::Search(
			Env& env,
			boost::optional<const std::string&> name,
			bool orderByName,
			bool raisingOrder,
			size_t first, /*= 0*/
			size_t number, /*= 0*/
			LinkLevel linkLevel
		){
			SelectQuery<ReservationContactTableSync> query;

			// Name filter
			if(name)
			{
				query.addWhereField(Name::FIELD.name, *name, ComposedExpression::OP_LIKE);
			}
			if(orderByName)
			{
				query.addOrderField(Name::FIELD.name, raisingOrder);
			}
			if(number)
			{
				query.setNumber(number);
			}
			if(first)
			{
				query.setFirst(first);
			}

			return LoadFromQuery(query, env, linkLevel);
		}
}	}
