
//////////////////////////////////////////////////////////////////////////
///	HikingTrailTableSync class implementation.
///	@file HikingTrailTableSync.cpp
///	@author Hugues
///	@date 2010
///
///	This file belongs to the SYNTHESE project (public transportation specialized software)
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
///	along with this program; if not, write to the Free Software
///	Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.

#include "HikingTrailTableSync.h"

#include "Conversion.h"
#include "HikingRight.h"
#include "Profile.h"
#include "ReplaceQuery.h"
#include "SelectQuery.hpp"
#include "Session.h"
#include "StopAreaTableSync.hpp"
#include "StopArea.hpp"
#include "User.h"

#include <sstream>

using namespace std;
using namespace boost;

namespace synthese
{
	using namespace db;
	using namespace util;
	using namespace hiking;
	using namespace pt;
	using namespace security;

	namespace util
	{
		template<> const string FactorableTemplate<DBTableSync,HikingTrailTableSync>::FACTORY_KEY("58 Hiking trails");
	}

	namespace hiking
	{
		const string HikingTrailTableSync::COL_NAME ("name");
		const string HikingTrailTableSync::COL_DURATION ("duration");
		const string HikingTrailTableSync::COL_MAP ("map");
		const string HikingTrailTableSync::COL_PROFILE ("profile");
		const string HikingTrailTableSync::COL_URL ("url");
		const string HikingTrailTableSync::COL_STOPS ("stops");
	}

	namespace db
	{
		template<> const DBTableSync::Format DBTableSyncTemplate<HikingTrailTableSync>::TABLE(
			"t064_hiking_trails"
		);



		template<> const Field DBTableSyncTemplate<HikingTrailTableSync>::_FIELDS[]=
		{
			Field(TABLE_COL_ID, SQL_INTEGER),
			Field(HikingTrailTableSync::COL_NAME, SQL_TEXT),
			Field(HikingTrailTableSync::COL_DURATION, SQL_TEXT),
			Field(HikingTrailTableSync::COL_MAP, SQL_TEXT),
			Field(HikingTrailTableSync::COL_PROFILE, SQL_TEXT),
			Field(HikingTrailTableSync::COL_URL, SQL_TEXT),
			Field(HikingTrailTableSync::COL_STOPS, SQL_TEXT),
			Field()
		};



		template<>
		DBTableSync::Indexes DBTableSyncTemplate<HikingTrailTableSync>::GetIndexes()
		{
			return DBTableSync::Indexes();
		}



		template<> void OldLoadSavePolicy<HikingTrailTableSync,HikingTrail>::Load(
			HikingTrail* object,
			const db::DBResultSPtr& rows,
			Env& env,
			LinkLevel linkLevel
		){
			object->setName(rows->getText(HikingTrailTableSync::COL_NAME));
			object->setDuration(rows->getText(HikingTrailTableSync::COL_DURATION));
			object->setMap(rows->getText(HikingTrailTableSync::COL_MAP));
			object->setProfile(rows->getText(HikingTrailTableSync::COL_PROFILE));
			object->setURL(rows->getText(HikingTrailTableSync::COL_URL));

			if(linkLevel >= UP_LINKS_LOAD_LEVEL)
			{
				vector<string> stopids = Conversion::ToStringVector(rows->getText (HikingTrailTableSync::COL_STOPS));
				HikingTrail::Stops stops;
				BOOST_FOREACH(const string& stopid, stopids)
				{
					try
					{
						stops.push_back(
							StopAreaTableSync::GetEditable(lexical_cast<RegistryKeyType>(stopid), env, linkLevel).get()
						);
					}
					catch(ObjectNotFoundException<StopArea>&)
					{
						Log::GetInstance().warn("No such stop "+ stopid +" in HikingTrail "+ lexical_cast<string>(object->getKey()));
					}
					catch(bad_lexical_cast)
					{
						Log::GetInstance().warn("No such stop "+ stopid +" in HikingTrail "+ lexical_cast<string>(object->getKey()));
					}
				}
				object->setStops(stops);
			}
		}



		template<> void OldLoadSavePolicy<HikingTrailTableSync,HikingTrail>::Save(
			HikingTrail* object,
			optional<DBTransaction&> transaction
		){
			stringstream stops;
			{
				bool first(true);
				BOOST_FOREACH(const StopArea* stop, object->getStops())
				{
					if(!first) stops << ",";
					stops << stop->getKey();
					first = false;
				}
			}

			ReplaceQuery<HikingTrailTableSync> query(*object);
			query.addField(object->getName());
			query.addField(object->getDuration());
			query.addField(object->getMap());
			query.addField(object->getProfile());
			query.addField(object->getURL());
			query.addField(stops.str());
			query.execute(transaction);
		}



		template<> void OldLoadSavePolicy<HikingTrailTableSync,HikingTrail>::Unlink(
			HikingTrail* obj
		){
		}



		template<> bool DBTableSyncTemplate<HikingTrailTableSync>::CanDelete(
			const server::Session* session,
			util::RegistryKeyType object_id
		){
			return session && session->hasProfile() && session->getUser()->getProfile()->isAuthorized<HikingRight>(DELETE_RIGHT);
		}



		template<> void DBTableSyncTemplate<HikingTrailTableSync>::BeforeDelete(
			util::RegistryKeyType id,
			db::DBTransaction& transaction
		){
		}



		template<> void DBTableSyncTemplate<HikingTrailTableSync>::AfterDelete(
			util::RegistryKeyType id,
			db::DBTransaction& transaction
		){
		}



		template<> void DBTableSyncTemplate<HikingTrailTableSync>::LogRemoval(
			const server::Session* session,
			util::RegistryKeyType id
		){
		}
	}



	namespace hiking
	{
		HikingTrailTableSync::SearchResult HikingTrailTableSync::Search(
			util::Env& env,
			optional<string> name,
			size_t first /*= 0*/,
			boost::optional<std::size_t> number /*= boost::optional<std::size_t>()*/,
			bool orderByName,
			bool raisingOrder,
			util::LinkLevel linkLevel
		){
			SelectQuery<HikingTrailTableSync> query;
			if(name)
			{
				query.addWhereField(COL_NAME, *name, ComposedExpression::OP_LIKE);
			}
			if(orderByName)
			{
			 	query.addOrderField(COL_NAME, raisingOrder);
			}
			if (number)
			{
				query.setNumber(*number + 1);
			}
			if (first > 0)
			{
				query.setFirst(first);
			}

			return LoadFromQuery(query, env, linkLevel);
		}
	}
}
