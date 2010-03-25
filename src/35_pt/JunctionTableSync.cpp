
//////////////////////////////////////////////////////////////////////////
///	JunctionTableSync class implementation.
///	@file JunctionTableSync.cpp
///	@author Hugues Romain
///	@date 2010
///
///	This file belongs to the SYNTHESE project (public transportation specialized software)
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
///	along with this program; if not, write to the Free Software
///	Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.

#include <sstream>

#include "JunctionTableSync.hpp"
#include "ReplaceQuery.h"
#include "DBModule.h"
#include "SQLiteResult.h"
#include "SQLite.h"
#include "SQLiteException.h"
#include "DataSourceTableSync.h"
#include "PhysicalStopTableSync.h"

using namespace std;
using namespace boost;
using namespace boost::posix_time;

namespace synthese
{
	using namespace db;
	using namespace util;
	using namespace pt;
	using namespace impex;

	namespace util
	{
		template<> const string FactorableTemplate<SQLiteTableSync,JunctionTableSync>::FACTORY_KEY("35.60 Junctions");
	}

	namespace pt
	{
		const std::string JunctionTableSync::COL_START_PHYSICAL_STOP_ID("start_physical_stop");
		const std::string JunctionTableSync::COL_END_PHYSICAL_STOP_ID("end_physical_stop");
		const std::string JunctionTableSync::COL_LENGTH("length");
		const std::string JunctionTableSync::COL_DURATION("duration");
		const std::string JunctionTableSync::COL_BIDIRECTIONAL("bidirectional");
	}
	
	namespace db
	{
		template<> const SQLiteTableSync::Format SQLiteTableSyncTemplate<JunctionTableSync>::TABLE(
			"t066_junctions"
		);



		template<> const SQLiteTableSync::Field SQLiteTableSyncTemplate<JunctionTableSync>::_FIELDS[]=
		{
			SQLiteTableSync::Field(TABLE_COL_ID, SQL_INTEGER, false),
			SQLiteTableSync::Field(JunctionTableSync::COL_START_PHYSICAL_STOP_ID, SQL_INTEGER),
			SQLiteTableSync::Field(JunctionTableSync::COL_END_PHYSICAL_STOP_ID, SQL_INTEGER),
			SQLiteTableSync::Field(JunctionTableSync::COL_LENGTH, SQL_INTEGER),
			SQLiteTableSync::Field(JunctionTableSync::COL_DURATION, SQL_INTEGER),
			SQLiteTableSync::Field(JunctionTableSync::COL_BIDIRECTIONAL, SQL_INTEGER),
			SQLiteTableSync::Field()
		};



		template<> const SQLiteTableSync::Index SQLiteTableSyncTemplate<JunctionTableSync>::_INDEXES[]=
		{
			SQLiteTableSync::Index(
				JunctionTableSync::COL_START_PHYSICAL_STOP_ID.c_str(),
				JunctionTableSync::COL_END_PHYSICAL_STOP_ID.c_str(),
			""),
			SQLiteTableSync::Index(
				JunctionTableSync::COL_END_PHYSICAL_STOP_ID.c_str(),
				JunctionTableSync::COL_START_PHYSICAL_STOP_ID.c_str(),
			""),
			SQLiteTableSync::Index()
		};



		template<> void SQLiteDirectTableSyncTemplate<JunctionTableSync,Junction>::Load(
			Junction* object,
			const db::SQLiteResultSPtr& rows,
			Env& env,
			LinkLevel linkLevel
		){
			if(linkLevel >= UP_LINKS_LOAD_LEVEL)
			{
				RegistryKeyType sid(rows->getLongLong(JunctionTableSync::COL_START_PHYSICAL_STOP_ID));
				RegistryKeyType eid(rows->getLongLong(JunctionTableSync::COL_END_PHYSICAL_STOP_ID));
				double length(rows->getDouble(JunctionTableSync::COL_LENGTH));
				time_duration duration(minutes(rows->getInt(JunctionTableSync::COL_DURATION)));
				bool bidir(rows->getBool(JunctionTableSync::COL_BIDIRECTIONAL));

				if(sid > 0 && eid > 0)
				{
					try
					{
						object->setStops(
							PhysicalStopTableSync::GetEditable(sid, env, linkLevel).get(),
							PhysicalStopTableSync::GetEditable(eid, env, linkLevel).get(),
							length,
							duration,
							bidir
						);
					}
					catch(ObjectNotFoundException<PhysicalStop>& e)
					{
						Log::GetInstance().warn("No such stop in Junction "+ lexical_cast<string>(object->getKey()));
					}
				}
			}
		}



		template<> void SQLiteDirectTableSyncTemplate<JunctionTableSync,Junction>::Save(
			Junction* object,
			optional<SQLiteTransaction&> transaction
		){
			ReplaceQuery<JunctionTableSync> query(*object);
			query.addField(object->isValid() ? object->getStart()->getKey() : RegistryKeyType(0));
			query.addField(object->isValid() ? object->getEnd()->getKey() : RegistryKeyType(0));
			query.addField(object->isValid() ? object->getLength() : double(0));
			query.addField(object->isValid() ? object->getDuration().total_seconds() / 60 : 0);
			query.addField(object->getBack() != NULL);
			query.execute(transaction);
		}



		template<> void SQLiteDirectTableSyncTemplate<JunctionTableSync,Junction>::Unlink(
			Junction* obj
		){
		}
	}
	
	
	
	namespace pt
	{
		JunctionTableSync::SearchResult JunctionTableSync::Search(
			util::Env& env,
			boost::optional<util::RegistryKeyType> startStopFilter,
			boost::optional<util::RegistryKeyType> endStopFilter,
			size_t first /*= 0*/,
			optional<size_t> number /*= boost::optional<std::size_t>()*/,
			bool orderByStop,
			bool raisingOrder,
			util::LinkLevel linkLevel /*= util::FIELDS_ONLY_LOAD_LEVEL */
		){
			stringstream query;
			query
				<< " SELECT *"
				<< " FROM " << TABLE.NAME
				<< " WHERE 1 "
				;
			if(startStopFilter)
			{
			 	query << " AND " << COL_START_PHYSICAL_STOP_ID << "=" << *startStopFilter;
			}
			if(endStopFilter)
			{
				query << " AND " << COL_END_PHYSICAL_STOP_ID << "=" << *endStopFilter;
			}
			if(orderByStop)
			{
			 	query << " ORDER BY " << COL_START_PHYSICAL_STOP_ID << " " << (raisingOrder ? "ASC" : "DESC") << "," <<
					COL_END_PHYSICAL_STOP_ID << " " << (raisingOrder ? "ASC" : "DESC")
				;
			}
			if (number)
			{
				query << " LIMIT " << (*number + 1);
				if (first > 0)
				{
					query << " OFFSET " << first;
			}	}

			return LoadFromQuery(query.str(), env, linkLevel);
		}
	}
}
