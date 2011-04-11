////////////////////////////////////////////////////////////////////////////////
/// ReservationRuleTableSync class implementation.
///	@file ReservationRuleTableSync.cpp
///	@author Hugues Romain
///
///	This file belongs to the SYNTHESE project (public transportation specialized
///	software)
///	Copyright (C) 2002 Hugues Romain - RCS <contact@reseaux-conseil.com>
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

#include <sstream>

#include "ReservationContactTableSync.h"
#include "DBModule.h"
#include "DBResult.hpp"
#include "DBException.hpp"
#include "ReplaceQuery.h"
#include "TransportNetworkRight.h"

using namespace std;
using namespace boost;

namespace synthese
{
	using namespace db;
	using namespace util;
	using namespace pt;
	using namespace security;

	template<> const string util::FactorableTemplate<DBTableSync,ReservationContactTableSync>::FACTORY_KEY("35.10.06 Reservation contacts");

	namespace pt
	{
		const string ReservationContactTableSync::COL_PHONEEXCHANGENUMBER ("phone_exchange_number");
		const string ReservationContactTableSync::COL_PHONEEXCHANGEOPENINGHOURS ("phone_exchange_opening_hours");
		const string ReservationContactTableSync::COL_DESCRIPTION ("description");
		const string ReservationContactTableSync::COL_WEBSITEURL ("web_site_url");
	}

	namespace db
	{
		template<> const DBTableSync::Format DBTableSyncTemplate<ReservationContactTableSync>::TABLE(
			"t021_reservation_contacts"
		);

		template<> const DBTableSync::Field DBTableSyncTemplate<ReservationContactTableSync>::_FIELDS[]=
		{
			DBTableSync::Field(TABLE_COL_ID, SQL_INTEGER),
			DBTableSync::Field(ReservationContactTableSync::COL_PHONEEXCHANGENUMBER, SQL_TEXT),
			DBTableSync::Field(ReservationContactTableSync::COL_PHONEEXCHANGEOPENINGHOURS, SQL_TEXT),
			DBTableSync::Field(ReservationContactTableSync::COL_DESCRIPTION, SQL_TEXT),
			DBTableSync::Field(ReservationContactTableSync::COL_WEBSITEURL, SQL_TEXT),
			DBTableSync::Field()
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
			string phoneExchangeNumber (
				rows->getText (ReservationContactTableSync::COL_PHONEEXCHANGENUMBER)
			);

			string phoneExchangeOpeningHours (
				rows->getText (ReservationContactTableSync::COL_PHONEEXCHANGEOPENINGHOURS)
			);

			string description (
				rows->getText (ReservationContactTableSync::COL_DESCRIPTION)
			);

			string webSiteUrl (
				rows->getText (ReservationContactTableSync::COL_WEBSITEURL)
			);

			rr->setPhoneExchangeNumber (phoneExchangeNumber);
			rr->setPhoneExchangeOpeningHours (phoneExchangeOpeningHours);
			rr->setDescription (description);
			rr->setWebSiteUrl (webSiteUrl);
		}


		template<> void DBDirectTableSyncTemplate<ReservationContactTableSync,ReservationContact>::Save(
			ReservationContact* object,
			optional<DBTransaction&> transaction
		){
			ReplaceQuery<ReservationContactTableSync> query(*object);
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



		void ReservationContactTableSync::Search(
			Env& env,
			int first, /*= 0*/
			int number, /*= 0*/
			LinkLevel linkLevel
		){
			stringstream query;
			query
				<< " SELECT *"
				<< " FROM " << TABLE.NAME
				<< " WHERE 1 " 
				/// @todo Fill Where criteria
				// eg << TABLE_COL_NAME << " LIKE '%" << Conversion::ToDBString(name, false) << "%'"
				;
			if (number > 0)
				query << " LIMIT " << Conversion::ToString(number + 1);
			if (first > 0)
				query << " OFFSET " << Conversion::ToString(first);

			LoadFromQuery(query.str(), env, linkLevel);
		}
	}
}
