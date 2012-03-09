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
#include "ReplaceQuery.h"
#include "ReservationContact.h"
#include "TransportNetworkRight.h"

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

	namespace pt
	{
		const string ReservationContactTableSync::COL_NAME = "name";
		const string ReservationContactTableSync::COL_PHONEEXCHANGENUMBER = "phone_exchange_number";
		const string ReservationContactTableSync::COL_PHONEEXCHANGEOPENINGHOURS = "phone_exchange_opening_hours";
		const string ReservationContactTableSync::COL_DESCRIPTION = "description";
		const string ReservationContactTableSync::COL_WEBSITEURL = "web_site_url";
	}

	namespace db
	{
		template<> const DBTableSync::Format DBTableSyncTemplate<ReservationContactTableSync>::TABLE(
			"t021_reservation_contacts"
		);



		template<> const Field DBTableSyncTemplate<ReservationContactTableSync>::_FIELDS[]=
		{
			Field(TABLE_COL_ID, SQL_INTEGER),
			Field(ReservationContactTableSync::COL_NAME, SQL_TEXT),
			Field(ReservationContactTableSync::COL_PHONEEXCHANGENUMBER, SQL_TEXT),
			Field(ReservationContactTableSync::COL_PHONEEXCHANGEOPENINGHOURS, SQL_TEXT),
			Field(ReservationContactTableSync::COL_DESCRIPTION, SQL_TEXT),
			Field(ReservationContactTableSync::COL_WEBSITEURL, SQL_TEXT),
			Field()
		};



		template<> const DBTableSync::Index DBTableSyncTemplate<ReservationContactTableSync>::_INDEXES[]=
		{
			DBTableSync::Index()
		};



		template<> void DBDirectTableSyncTemplate<ReservationContactTableSync,ReservationContact>::Load(
			ReservationContact* rr,
			const db::DBResultSPtr& rows,
			Env& env,
			LinkLevel linkLevel
		){
			// Name
			rr->setName(rows->getText(ReservationContactTableSync::COL_NAME));

			string phoneExchangeNumber (
				rows->getText (ReservationContactTableSync::COL_PHONEEXCHANGENUMBER)
			);
			rr->setPhoneExchangeNumber (phoneExchangeNumber);

			string phoneExchangeOpeningHours (
				rows->getText (ReservationContactTableSync::COL_PHONEEXCHANGEOPENINGHOURS)
			);
			rr->setPhoneExchangeOpeningHours (phoneExchangeOpeningHours);

			string description (
				rows->getText (ReservationContactTableSync::COL_DESCRIPTION)
			);
			rr->setDescription (description);

			string webSiteUrl (
				rows->getText (ReservationContactTableSync::COL_WEBSITEURL)
			);
			rr->setWebSiteUrl (webSiteUrl);
		}



		template<> void DBDirectTableSyncTemplate<ReservationContactTableSync,ReservationContact>::Save(
			ReservationContact* object,
			optional<DBTransaction&> transaction
		){
			ReplaceQuery<ReservationContactTableSync> query(*object);
			query.addField(object->getName());
			query.addField(object->getPhoneExchangeNumber());
			query.addField(object->getPhoneExchangeOpeningHours());
			query.addField(object->getDescription());
			query.addField(object->getWebSiteUrl());
			query.execute(transaction);
		}



		template<> void DBDirectTableSyncTemplate<ReservationContactTableSync,ReservationContact>::Unlink(
			ReservationContact* obj
		){
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
		ReservationContactTableSync::ReservationContactTableSync()
			: DBRegistryTableSyncTemplate<ReservationContactTableSync,ReservationContact>()
		{
		}



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
				query.addWhereField(COL_NAME, *name, ComposedExpression::OP_LIKE);
			}
			if(orderByName)
			{
				query.addOrderField(COL_NAME, raisingOrder);
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
