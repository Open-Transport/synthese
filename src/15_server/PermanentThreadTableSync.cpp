////////////////////////////////////////////////////////////////////////////////
///	PermanentThread table synchronizer class implementation.
///	@file PermanentThreadSync.cpp
///	@author Camille Hue
///	@date 2013
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

#include "PermanentThreadTableSync.hpp"

#include "SelectQuery.hpp"
#include "ReplaceQuery.h"

#include <boost/lexical_cast.hpp>

using namespace boost;
using namespace std;

namespace synthese
{
	using namespace db;
	using namespace util;
	using namespace server;

	template<> const string util::FactorableTemplate<DBTableSync,PermanentThreadTableSync>::FACTORY_KEY(
		"15.01 PermanentThread"
	);

	namespace db
	{
		template<> const DBTableSync::Format DBTableSyncTemplate<PermanentThreadTableSync>::TABLE(
			"t115_permanent_threads"
		);

		template<> const Field DBTableSyncTemplate<PermanentThreadTableSync>::_FIELDS[] = { Field() }; // Defined by the record



		template<>
		DBTableSync::Indexes DBTableSyncTemplate<PermanentThreadTableSync>::GetIndexes()
		{
			return DBTableSync::Indexes();
		}



		template<> bool DBTableSyncTemplate<PermanentThreadTableSync>::CanDelete(
			const server::Session* session,
			util::RegistryKeyType object_id
		){
			//TODO test if the device/user has sufficient right level
			return true;
		}



		template<> void DBTableSyncTemplate<PermanentThreadTableSync>::BeforeDelete(
			util::RegistryKeyType id,
			db::DBTransaction& transaction
		){
		}



		template<> void DBTableSyncTemplate<PermanentThreadTableSync>::AfterDelete(
			util::RegistryKeyType id,
			db::DBTransaction& transaction
		){
		}



		template<> void DBTableSyncTemplate<PermanentThreadTableSync>::LogRemoval(
			const server::Session* session,
			util::RegistryKeyType id
		){
			//TODO Log the removal
		}
	}

	namespace server
	{
		PermanentThreadTableSync::SearchResult PermanentThreadTableSync::Search(
			Env& env,
			int first /*= 0*/
			, optional<size_t> number /*= 0*/
			, bool orderByName
			, bool raisingOrder,
			LinkLevel linkLevel
		){
			SelectQuery<PermanentThreadTableSync> query;
			if (orderByName)
			{
				query.addOrderField(Name::FIELD.name, raisingOrder);
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
