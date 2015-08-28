/** AlarmTableSync class implementation.
	@file AlarmTableSync.cpp

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

#include "AlarmTableSync.h"

#include "AlarmObjectLinkTableSync.h"
#include "Alarm.h"
#include "DataSource.h"
#include "DataSourceLinksField.hpp"
#include "DBResult.hpp"
#include "ImportableTableSync.hpp"
#include "MessageAlternativeTableSync.hpp"
#include "MessagesLibraryLog.h"
#include "MessagesLibraryRight.h"
#include "MessagesRight.h"
#include "MessagesSectionTableSync.hpp"
#include "MessagesLog.h"
#include "MessagesTypes.h"
#include "Profile.h"
#include "ReplaceQuery.h"
#include "ScenarioCalendarTableSync.hpp"

#include "ScenarioTemplate.h"
#include "ScenarioTemplateTableSync.h"
#include "SentScenario.h"
#include "Session.h"
#include "User.h"

#include <boost/optional/optional_io.hpp>

using namespace std;
using namespace boost;

namespace synthese
{
	using namespace db;
	using namespace messages;
	using namespace impex;
	using namespace util;
	using namespace security;

	namespace util
	{
		template<> const string FactorableTemplate<DBTableSync,AlarmTableSync>::FACTORY_KEY("17.10.01 Alarms");
	}

	namespace messages
	{
	}

	namespace db
	{
		template<> const DBTableSync::Format DBTableSyncTemplate<AlarmTableSync>::TABLE(
			"t003_alarms"
		);

		template<> const Field DBTableSyncTemplate<AlarmTableSync>::_FIELDS[]=
		{
		};

		
		template<>
		DBTableSync::Indexes DBTableSyncTemplate<AlarmTableSync>::GetIndexes()
		{
			DBTableSync::Indexes r;
			r.push_back(DBTableSync::Index(ParentScenario::FIELD.name.c_str(), ""));
			return r;
		};


		template<> bool DBTableSyncTemplate<AlarmTableSync>::CanDelete(
			const server::Session* session,
			util::RegistryKeyType object_id
		){
			try
			{
				Env env;
				boost::shared_ptr<const Alarm> alarm(AlarmTableSync::Get(object_id, env));
				return session && session->hasProfile() && session->getUser()->getProfile()->isAuthorized<MessagesLibraryRight>(DELETE_RIGHT);
			}
			catch (ObjectNotFoundException<AlarmObjectLink>&)
			{
				return false;
			}
		}



		template<> void DBTableSyncTemplate<AlarmTableSync>::BeforeDelete(
			util::RegistryKeyType id,
			db::DBTransaction& transaction
		){
			Env env;

			// Links
			AlarmObjectLinkTableSync::RemoveByMessage(id);

			// Message alternatives
			MessageAlternativeTableSync::SearchResult alternatives(
				MessageAlternativeTableSync::Search(env, id)
			);
			BOOST_FOREACH(const boost::shared_ptr<MessageAlternative>& alternative, alternatives)
			{
				MessageAlternativeTableSync::Remove(NULL, alternative->getKey(), transaction, false);
			}

		}



		template<> void DBTableSyncTemplate<AlarmTableSync>::AfterDelete(
			util::RegistryKeyType id,
			db::DBTransaction& transaction
		){
		}



		template<> void DBTableSyncTemplate<AlarmTableSync>::LogRemoval(
			const server::Session* session,
			util::RegistryKeyType id
		){
			Env env;
			boost::shared_ptr<const Alarm> alarm(AlarmTableSync::Get(id, env));
			MessagesLibraryLog::AddDeleteEntry(static_cast<const Alarm*>(alarm.get()), (session ? session->getUser().get() : NULL));
		}
	}


	namespace messages
	{

		// TODO check search is same for sent an d template
		
		AlarmTableSync::SearchResult AlarmTableSync::Search(
			util::Env& env,
			boost::optional<util::RegistryKeyType> scenarioId /*= boost::optional<util::RegistryKeyType>() */,
			int first /*= 0 */,
			boost::optional<std::size_t> number /*= boost::optional<std::size_t>() */,
			bool orderByLevel /*= false */,
			bool raisingOrder /*= false*/,
			util::LinkLevel linkLevel /*= util::UP_LINKS_LOAD_LEVEL */
		){
			AlarmTableSync::SearchResult result;
			Search(env, std::back_inserter(result), scenarioId, first, number, orderByLevel, linkLevel);
			return result;
		}
}	}


