
/** CallStatisticsTableSync class implementation.
	@file CallStatisticsTableSync.cpp

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

#include "CallStatisticsTableSync.h"

#include "DBLogEntryTableSync.h"

#include <boost/optional.hpp>
#include <sstream>

using namespace std;
using namespace boost;

namespace synthese
{
	using namespace db;
	using namespace dblog;

	namespace resa
	{
		CallStatisticsTableSync::CallCountSearchResult CallStatisticsTableSync::CountCalls(
			boost::gregorian::date_period period,
			Step step,
			logic::tribool autoresa
		){
			CallCountSearchResult r;
			stringstream s;
			DB* db = DBModule::GetDB();

			s << "SELECT " <<
				db->getSQLConvertInteger(DBLogEntryTableSync::COL_CONTENT) << " AS type," <<
				db->getSQLDateFormat(GetGroupBy(step), DBLogEntryTableSync::COL_DATE) << " AS step," <<
				"COUNT(" << TABLE_COL_ID << ") AS number" <<
				" FROM " << DBLogEntryTableSync::TABLE.NAME <<
				" WHERE " <<
					DBLogEntryTableSync::COL_LOG_KEY << "='" << ResaDBLog::FACTORY_KEY << "' AND " <<
					DBLogEntryTableSync::COL_DATE << ">='" << gregorian::to_iso_extended_string(period.begin())  << " 00:00:00' AND " <<
					DBLogEntryTableSync::COL_DATE << "<'" << gregorian::to_iso_extended_string(period.end()) << " 00:00:00'";
			if(!indeterminate(autoresa))
				s << " AND " << DBLogEntryTableSync::COL_OBJECT_ID << (autoresa ? "=" : "!=") << DBLogEntryTableSync::COL_USER_ID;
			s <<
				" GROUP BY " << db->getSQLDateFormat(GetGroupBy(step), DBLogEntryTableSync::COL_DATE) << "," <<
				db->getSQLConvertInteger(DBLogEntryTableSync::COL_CONTENT) <<
				" ORDER BY " << db->getSQLConvertInteger(DBLogEntryTableSync::COL_CONTENT)
			;
			optional<ResaDBLog::_EntryType> lastStep;
			DBResultSPtr rows = db->execQuery(s.str());
			while (rows->next ())
			{
				if(!lastStep || *lastStep != static_cast<ResaDBLog::_EntryType>(rows->getInt("type")))
				{
					r.insert(make_pair(static_cast<ResaDBLog::_EntryType>(rows->getInt("type")), CallCountSearchResult::mapped_type()));
				}
				lastStep = optional<ResaDBLog::_EntryType>(static_cast<ResaDBLog::_EntryType>(rows->getInt("type")));
				CallCountSearchResult::iterator it(r.find(static_cast<ResaDBLog::_EntryType>(rows->getInt("type"))));
				it->second.insert(make_pair(rows->getText("step"), rows->getInt("number")));
			}
			return r;
		}



		std::string CallStatisticsTableSync::GetGroupBy( Step step )
		{
			if(step == HOUR_STEP)
				return "%H";
			if(step == DAY_STEP)
				return "%Y-%m-%d";
			if(step == WEEK_DAY_STEP)
				return "%w";
			if(step == WEEK_STEP)
				return "%W";
			if(step == MONTH_STEP)
				return "%Y-%m";
			if(step == YEAR_STEP)
				return "%Y";
			assert(false);
			return "%Y-%m-%d";
		}
	}
}
