////////////////////////////////////////////////////////////////////////////////
/// DisplayMonitoringStatusTableSync class implementation.
///	@file DisplayMonitoringStatusTableSync.cpp
///	@author Hugues Romain
///	@date 2008-12-19 10:50
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

#include "DisplayMonitoringStatusTableSync.h"

#include "DisplayMaintenanceLog.h"
#include "DisplayMaintenanceRight.h"
#include "DisplayScreenCPU.h"
#include "DisplayScreenTableSync.h"
#include "DisplayScreenCPUTableSync.h"
#include "PtimeField.hpp"
#include "ReplaceQuery.h"
#include "SelectQuery.hpp"
#include "User.h"

#include <sstream>
#include <boost/date_time/posix_time/posix_time_types.hpp>

using namespace std;
using namespace boost;
using namespace boost::posix_time;

namespace synthese
{
	using namespace db;
	using namespace util;
	using namespace departure_boards;

	namespace util
	{
		template<> const string FactorableTemplate<DBTableSync,DisplayMonitoringStatusTableSync>::FACTORY_KEY("54.10 Display monitoring status");
	}

	namespace db
	{
		template<> const DBTableSync::Format DBTableSyncTemplate<DisplayMonitoringStatusTableSync>::TABLE(
			"t057_display_monitoring_status",
			true
		);


		template<> const Field DBTableSyncTemplate<DisplayMonitoringStatusTableSync>::_FIELDS[] =
		{
			Field()
		};

		template<>
		DBTableSync::Indexes DBTableSyncTemplate<DisplayMonitoringStatusTableSync>::GetIndexes()
		{
			DBTableSync::Indexes r;
			r.push_back(DBTableSync::Index(StatusScreen::FIELD.name.c_str(), ""));
			return r;
		}



		template<> bool DBTableSyncTemplate<DisplayMonitoringStatusTableSync>::CanDelete(
			const server::Session* session,
			util::RegistryKeyType object_id
		){
			//TODO test if the user has sufficient right level
			return true;
		}



		template<> void DBTableSyncTemplate<DisplayMonitoringStatusTableSync>::BeforeDelete(
			util::RegistryKeyType id,
			db::DBTransaction& transaction
		){
		}



		template<> void DBTableSyncTemplate<DisplayMonitoringStatusTableSync>::AfterDelete(
			util::RegistryKeyType id,
			db::DBTransaction& transaction
		){
		}



		template<> void DBTableSyncTemplate<DisplayMonitoringStatusTableSync>::LogRemoval(
			const server::Session* session,
			util::RegistryKeyType id
		){
			//TODO Log the removal
		}
	}



	namespace departure_boards
	{
		DisplayMonitoringStatusTableSync::SearchResult DisplayMonitoringStatusTableSync::Search(
			Env& env,
			optional<RegistryKeyType> screenId,
			int first /*= 0*/,
			boost::optional<std::size_t> number /*= 0*/,
			bool orderByScreenId,
			bool orderByTime,
			bool raisingOrder,
			LinkLevel linkLevel
		){
			SelectQuery<DisplayMonitoringStatusTableSync> query;
			if (screenId)
			{
				query.addWhereField(StatusScreen::FIELD.name, *screenId);
			}
			if (orderByScreenId)
			{
				query.addOrderField(StatusScreen::FIELD.name, raisingOrder);
				query.addOrderField(StatusTime::FIELD.name, raisingOrder);
			}
			else if(orderByTime)
			{
				query.addOrderField(StatusTime::FIELD.name, raisingOrder);
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



		boost::shared_ptr<DisplayMonitoringStatus> DisplayMonitoringStatusTableSync::GetStatus(
			const DisplayScreen& screen
		){
			Env env;
			SearchResult entries(
				Search(env, screen.getKey(), 0, 1, false, true, true, FIELDS_ONLY_LOAD_LEVEL)
			);

			if(entries.empty())
			{
				return boost::shared_ptr<DisplayMonitoringStatus>();
			}

			boost::shared_ptr<DisplayMonitoringStatus> status(entries.front());
			if(screen.isDown(*status))
			{
				DisplayMaintenanceLog::AddMonitoringDownEntry(screen);
			}

			return status;
		}



		boost::posix_time::ptime DisplayMonitoringStatusTableSync::GetLastContact(
			const DisplayScreenCPU& cpu
		){
			Env env;
			SearchResult entries(
				Search(env, cpu.getKey(), 0, 1, false, true, true, FIELDS_ONLY_LOAD_LEVEL)
			);

			if(entries.empty())
			{
				return not_a_date_time;
			}

			boost::shared_ptr<DisplayMonitoringStatus> status(entries.front());
			if(cpu.isDown(status->getTime()))
			{
				DisplayMaintenanceLog::AddMonitoringDownEntry(cpu);
			}

			return status->getTime();
		}



		boost::shared_ptr<DisplayMonitoringStatus> DisplayMonitoringStatusTableSync::UpdateStatus(
			Env& env,
			const DisplayScreen& screen,
			bool archive
		){
			boost::shared_ptr<DisplayMonitoringStatus> result;
			if(!archive)
			{
				SearchResult entries(Search(env, screen.getKey(), 0, 1, false, true, false));
				if(!entries.empty())
				{
					result = *entries.begin();
				}
			}
			if(!result)
			{
				result.reset(new DisplayMonitoringStatus);
				result->setScreen(&screen);
			}
			ptime now(microsec_clock::local_time());
			result->setTime(now);
			return result;
		}

		bool DisplayMonitoringStatusTableSync::allowList(const server::Session* session) const
		{
			return session && session->hasProfile() && session->getUser()->getProfile()->isAuthorized<DisplayMaintenanceRight>(security::READ);
		}
	}
}
