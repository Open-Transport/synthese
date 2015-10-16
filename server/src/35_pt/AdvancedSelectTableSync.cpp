
/** AdvancedSelectTableSync class implementation.
	@file 35_pt/AdvancedSelectTableSync.cpp

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

#include "AdvancedSelectTableSync.h"

#include "LineStopTableSync.h"
#include "JourneyPatternTableSync.hpp"
#include "StopPointTableSync.hpp"
#include "CommercialLineTableSync.h"
#include "ScheduledServiceTableSync.h"
#include "ContinuousServiceTableSync.h"

#include "DBModule.h"

#include "Exception.h"

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

	namespace pt
	{


		bool isPlaceServedByCommercialLine(util::RegistryKeyType lineId, util::RegistryKeyType placeId)
		{
			stringstream query;
			query
				<< "SELECT ls." << TABLE_COL_ID << " FROM "
				<< LineStopTableSync::TABLE.NAME << " AS ls "
				<< " INNER JOIN " << StopPointTableSync::TABLE.NAME << " AS p ON p." << TABLE_COL_ID << "=ls." << LineNode::FIELD.name
				<< " INNER JOIN " << JourneyPatternTableSync::TABLE.NAME << " AS l ON l." << TABLE_COL_ID << "=ls." << Line::FIELD.name
				<< " WHERE "
				<< "p." << StopPointTableSync::COL_PLACEID << "=" << placeId
				<< " AND l." << JourneyPatternCommercialLine::FIELD.name << "=" << lineId
				<< " LIMIT 1";
			DBResultSPtr rows(DBModule::GetDB()->execQuery(query.str()));
			return rows->next();
		}

		bool isPlaceServedByNetwork( util::RegistryKeyType networkId, util::RegistryKeyType placeId )
		{
			stringstream query;
			query
				<< "SELECT ls." << TABLE_COL_ID << " FROM "
				<< LineStopTableSync::TABLE.NAME << " AS ls "
				<< " INNER JOIN " << StopPointTableSync::TABLE.NAME << " AS p ON p." << TABLE_COL_ID << "=ls." << LineNode::FIELD.name
				<< " INNER JOIN " << JourneyPatternTableSync::TABLE.NAME << " AS l ON l." << TABLE_COL_ID << "=ls." << Line::FIELD.name
				<< " INNER JOIN " << CommercialLineTableSync::TABLE.NAME << " AS c ON c." << TABLE_COL_ID << "=l." << JourneyPatternCommercialLine::FIELD.name
				<< " WHERE "
				<< "p." << StopPointTableSync::COL_PLACEID << "=" << placeId
				<< " AND c." << Network::FIELD.name << "=" << networkId
				<< " LIMIT 1";
			DBResultSPtr rows(DBModule::GetDB()->execQuery(query.str()));
			return rows->next();
		}

		int getRankOfLastDepartureLineStop( util::RegistryKeyType line )
		{
			stringstream query;
			query
				<< "SELECT "
				<< RankInPath::FIELD.name
				<< " FROM " << LineStopTableSync::TABLE.NAME
				<< " WHERE "
				<< Line::FIELD.name << "=" << line
				<< " AND " << IsDeparture::FIELD.name << "=1"
				<< " LIMIT 1";
			DBResultSPtr rows(DBModule::GetDB()->execQuery(query.str()));
			if (!rows->next())
				throw Exception("The line does not exists or does not contains any line stop");
			return rows->getInt(RankInPath::FIELD.name);
		}



		pt::RunHours getCommercialLineRunHours(
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
					id,
					optional<RegistryKeyType>(),
					optional<string>(),
					false,
					0,
					optional<size_t>(),
					true,
					true,
					UP_DOWN_LINKS_LOAD_LEVEL
			)	);
			BOOST_FOREACH(const boost::shared_ptr<ScheduledService>& serv, services)
			{
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
				ContinuousServiceTableSync::Search(
					env, 
					optional<RegistryKeyType>(), 
					id,
					0,
					optional<size_t>(),
					true,
					true,
					UP_DOWN_LINKS_LOAD_LEVEL)
			);
			BOOST_FOREACH(const boost::shared_ptr<ContinuousService>& serv, cservices)
			{
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
