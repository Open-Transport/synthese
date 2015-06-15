
//////////////////////////////////////////////////////////////////////////
///	AlertTableSync class implementation.
///	@file AlertTableSync.cpp
///	@author Camille Hue
///	@date 2014
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

#include "AlertTableSync.hpp"

#include "Alert.hpp"
//#include "ImportableTableSync.hpp"
/*
#include "ReplaceQuery.h"
#include "SelectQuery.hpp"
*/

#include <sstream>
#include <boost/foreach.hpp>
#include <boost/algorithm/string/split.hpp>
#include <boost/date_time/posix_time/posix_time.hpp>

using namespace std;
using namespace boost;
using namespace boost::algorithm;
using namespace boost::posix_time;
using namespace boost::gregorian;

namespace synthese
{
	using namespace db;
	//using namespace impex;
	using namespace pt;
	using namespace util;
	using namespace regulation;

	namespace util
	{
		template<> const string FactorableTemplate<DBTableSync,AlertTableSync>::FACTORY_KEY("62.01 Alerts");
	}

	namespace db
	{
		template<> const DBTableSync::Format DBTableSyncTemplate<AlertTableSync>::TABLE(
			"t121_alerts"
		);



		template<> const Field DBTableSyncTemplate<AlertTableSync>::_FIELDS[]=
		{
			Field()
		};



		template<>
		DBTableSync::Indexes DBTableSyncTemplate<AlertTableSync>::GetIndexes()
		{
			return DBTableSync::Indexes();
		}



		template<> bool DBTableSyncTemplate<AlertTableSync>::CanDelete(
			const server::Session* session,
			util::RegistryKeyType object_id
		){
			//TODO Check tue user rights
			return true;
		}



		template<> void DBTableSyncTemplate<AlertTableSync>::BeforeDelete(
			util::RegistryKeyType id,
			db::DBTransaction& transaction
		){
		}



		template<> void DBTableSyncTemplate<AlertTableSync>::AfterDelete(
			util::RegistryKeyType id,
			db::DBTransaction& transaction
		){
		}



		template<> void DBTableSyncTemplate<AlertTableSync>::LogRemoval(
			const server::Session* session,
			util::RegistryKeyType id
		){
			//TODO Log the removal
		}
	}



	namespace regulation
	{
		AlertTableSync::SearchResult AlertTableSync::Search(util::Env& env,
                                                            boost::optional<AlertType> alertType = boost::optional<AlertType>())
        {
			SelectQuery<AlertTableSync> query;
			if ((bool) alertType)
			{
				query.addWhereField(Kind::FIELD.name, alertType);
			}
			return LoadFromQuery(query, env, util::UP_LINKS_LOAD_LEVEL);
		}

    }
}

