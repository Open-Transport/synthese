
/** AdvancedSelectTableSync class implementation.
	@file AdvancedSelectTableSync.cpp

	This file belongs to the SYNTHESE project (public transportation specialized software)
	Copyright (C) 2002 Hugues Romain - RCS <contact@reseaux-conseil.com>

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

#include "AdvancedSelectTableSync.h"

#include "LineStopTableSync.h"
#include "LineTableSync.h"
#include "PhysicalStopTableSync.h"
#include "CommercialLineTableSync.h"
#include "ScheduledServiceTableSync.h"
#include "ContinuousServiceTableSync.h"
#include "ServiceDate.h"
#include "ServiceDateTableSync.h"

#include "DBModule.h"

#include "01_util/Exception.h"

#include <sstream>

using namespace std;
using namespace boost;
using namespace boost::gregorian;

namespace synthese
{
	using namespace db;
	using namespace util;
	using namespace pt;
	using namespace graph;

	namespace env
	{


		bool isPlaceServedByCommercialLine(uid lineId, uid placeId)
		{
			stringstream query;
			query
				<< "SELECT ls." << TABLE_COL_ID << " FROM "
				<< LineStopTableSync::TABLE.NAME << " AS ls "
				<< " INNER JOIN " << PhysicalStopTableSync::TABLE.NAME << " AS p ON p." << TABLE_COL_ID << "=ls." << LineStopTableSync::COL_PHYSICALSTOPID
				<< " INNER JOIN " << LineTableSync::TABLE.NAME << " AS l ON l." << TABLE_COL_ID << "=ls." << LineStopTableSync::COL_LINEID
				<< " WHERE "
				<< "p." << PhysicalStopTableSync::COL_PLACEID << "=" << placeId
				<< " AND l." << LineTableSync::COL_COMMERCIAL_LINE_ID << "=" << lineId
				<< " LIMIT 1";
			SQLiteResultSPtr rows(DBModule::GetSQLite()->execQuery(query.str()));
			return rows->next();
		}

		bool isPlaceServedByNetwork( uid networkId, uid placeId )
		{
			stringstream query;
			query
				<< "SELECT ls." << TABLE_COL_ID << " FROM "
				<< LineStopTableSync::TABLE.NAME << " AS ls "
				<< " INNER JOIN " << PhysicalStopTableSync::TABLE.NAME << " AS p ON p." << TABLE_COL_ID << "=ls." << LineStopTableSync::COL_PHYSICALSTOPID
				<< " INNER JOIN " << LineTableSync::TABLE.NAME << " AS l ON l." << TABLE_COL_ID << "=ls." << LineStopTableSync::COL_LINEID
				<< " INNER JOIN " << CommercialLineTableSync::TABLE.NAME << " AS c ON c." << TABLE_COL_ID << "=l." << LineTableSync::COL_COMMERCIAL_LINE_ID
				<< " WHERE "
				<< "p." << PhysicalStopTableSync::COL_PLACEID << "=" << placeId
				<< " AND c." << CommercialLineTableSync::COL_NETWORK_ID << "=" << networkId
				<< " LIMIT 1";
			SQLiteResultSPtr rows(DBModule::GetSQLite()->execQuery(query.str()));
			return rows->next();
		}

		int getRankOfLastDepartureLineStop( uid line )
		{
			stringstream query;
			query
				<< "SELECT "
				<< LineStopTableSync::COL_RANKINPATH
				<< " FROM " << LineStopTableSync::TABLE.NAME
				<< " WHERE "
				<< LineStopTableSync::COL_LINEID << "=" << line
				<< " AND " << LineStopTableSync::COL_ISDEPARTURE << "=1"
				<< " LIMIT 1";
			SQLiteResultSPtr rows(DBModule::GetSQLite()->execQuery(query.str()));
			if (!rows->next())
				throw Exception("The line does not exists or does not contains any line stop");
			return rows->getInt(LineStopTableSync::COL_RANKINPATH);
		}



		env::RunHours getCommercialLineRunHours(
			util::Env& env,
			util::RegistryKeyType id,
			const boost::optional<date>& startDate,
			const boost::optional<date>& endDate
		){
			RunHours result;
			
			ScheduledServiceTableSync::SearchResult services(
				ScheduledServiceTableSync::Search(
					env,
					optional<RegistryKeyType>(),
					id
			)	);
			BOOST_FOREACH(shared_ptr<ScheduledService> serv, services)
			{
				ServiceDateTableSync::SetActiveDates(*serv);
				BOOST_FOREACH(const date& d, serv->getActiveDates())
				{
					if(startDate && d < *startDate) continue;
					if(endDate && d > *endDate) continue;

					const int startHour(Service::GetTimeOfDay(serv->getDepartureBeginScheduleToIndex(false, 0)).hours());
					const int endHour(Service::GetTimeOfDay(serv->getDepartureEndScheduleToIndex(false, 0)).hours());
					if (startHour <= endHour)
					{
						for(int h(startHour); h <= endHour; ++h)
						{
							RunHours::key_type key(make_pair(d, h));
							RunHours::iterator it(result.find(key));
							if (it == result.end())
							{
								result.insert(make_pair(key, 1));
							}
							else
							{
								++it->second;
							}
						}
					}
					else
					{
						for(int h(0); h <= endHour; ++h)
						{
							RunHours::key_type key(make_pair(d, h));
							RunHours::iterator it(result.find(key));
							if (it == result.end())
							{
								result.insert(make_pair(key, 1));
							}
							else
							{
								++it->second;
							}
						}
						for(int h(startHour); h <= 23; ++h)
						{
							RunHours::key_type key(make_pair(d, h));
							RunHours::iterator it(result.find(key));
							if (it == result.end())
							{
								result.insert(make_pair(key, 1));
							}
							else
							{
								++it->second;
							}
						}
					}
				}
			}
			
			ContinuousServiceTableSync::SearchResult cservices(
				ContinuousServiceTableSync::Search(env, optional<RegistryKeyType>(), id)
			);
			BOOST_FOREACH(shared_ptr<ContinuousService> serv, cservices)
			{
				ServiceDateTableSync::SetActiveDates(*serv);
				BOOST_FOREACH(const date& d, serv->getActiveDates())
				{
					if(startDate && d < *startDate) continue;
					if(endDate && d > *endDate) continue;

					const int startHour(Service::GetTimeOfDay(serv->getDepartureBeginScheduleToIndex(false, 0)).hours());
					const int endHour(Service::GetTimeOfDay(serv->getDepartureEndScheduleToIndex(false, 0)).hours());
					if (startHour <= endHour)
					{
						for(int h(startHour); h <= endHour; ++h)
						{
							RunHours::key_type key(make_pair(d, h));
							RunHours::iterator it(result.find(key));
							if (it == result.end())
							{
								result.insert(make_pair(key, 1));
							}
							else
							{
								++it->second;
							}
						}
					}
					else
					{
						for(int h(0); h <= endHour; ++h)
						{
							RunHours::key_type key(make_pair(d, h));
							RunHours::iterator it(result.find(key));
							if (it == result.end())
							{
								result.insert(make_pair(key, 1));
							}
							else
							{
								++it->second;
							}
						}
						for(int h(startHour); h <= 23; ++h)
						{
							RunHours::key_type key(make_pair(d, h));
							RunHours::iterator it(result.find(key));
							if (it == result.end())
							{
								result.insert(make_pair(key, 1));
							}
							else
							{
								++it->second;
							}
						}
					}
				}
			}
			return result;
		}
	}
}
