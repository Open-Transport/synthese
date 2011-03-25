
//////////////////////////////////////////////////////////////////////////
///	DesignatedLinePhysicalStopInheritedTybleSync class implementation.
///	@file DesignatedLinePhysicalStopInheritedTybleSync.cpp
///	@author RCSobility
///	@date 2011
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

#include "DesignatedLinePhysicalStopInheritedTableSync.hpp"
#include "StopPointTableSync.hpp"
#include "ReplaceQuery.h"
#include "JourneyPattern.hpp"
#include "JourneyPatternCopy.hpp"

#include <geos/geom/LineString.h>

using namespace std;
using namespace boost;
using namespace geos::geom;

namespace synthese
{
	using namespace db;
	using namespace pt;
	using namespace util;

	template<>
	const string util::FactorableTemplate<LineStopTableSync,DesignatedLinePhysicalStopInheritedTableSync>::FACTORY_KEY("DesignatedLinePhysicalStopInheritedTybleSync");

	namespace db
	{
		template<>
		void SQLiteInheritedTableSyncTemplate<LineStopTableSync,DesignatedLinePhysicalStopInheritedTableSync,DesignatedLinePhysicalStop>::Load(
			DesignatedLinePhysicalStop* obj,
			const SQLiteResultSPtr& rows,
			Env& env,
			LinkLevel linkLevel
		){
			_CommonLoad(obj, rows, env, linkLevel);

			if (linkLevel > FIELDS_ONLY_LOAD_LEVEL)
			{
				util::RegistryKeyType fromPhysicalStopId (
					rows->getLongLong (LineStopTableSync::COL_PHYSICALSTOPID)
				);
				obj->setPhysicalStop(*StopPointTableSync::GetEditable(fromPhysicalStopId, env, linkLevel));

				// Line update
				obj->getLine()->addEdge(*obj);
			}

			if(!rows->getText(LineStopTableSync::COL_SCHEDULEINPUT).empty())
			{
				obj->setScheduleInput(rows->getBool(LineStopTableSync::COL_SCHEDULEINPUT));
			}
			else
			{
				obj->setScheduleInput(true);
			}
			if (linkLevel > FIELDS_ONLY_LOAD_LEVEL)
			{
				// Sublines update
				BOOST_FOREACH(JourneyPatternCopy* copy, static_cast<LineStop*>(obj)->getLine()->getSubLines())
				{
					DesignatedLinePhysicalStop* newEdge(
						new DesignatedLinePhysicalStop(
							0,
							copy,
							obj->getRankInPath(),
							obj->isDeparture(),
							obj->isArrival(),
							obj->getMetricOffset(),
							obj->getPhysicalStop(),
							obj->getScheduleInput()
					)	);
					copy->addEdge(*newEdge);
				}
			}
		}



		template<>
		void SQLiteInheritedTableSyncTemplate<LineStopTableSync,DesignatedLinePhysicalStopInheritedTableSync,DesignatedLinePhysicalStop>::Unlink(
			DesignatedLinePhysicalStop* obj
		){
			obj->clearPhysicalStop();
		}



		template<>
		void SQLiteInheritedTableSyncTemplate<LineStopTableSync,DesignatedLinePhysicalStopInheritedTableSync,DesignatedLinePhysicalStop>::Save(
			DesignatedLinePhysicalStop* object,
			optional<SQLiteTransaction&> transaction
		){
			if(!object->getPhysicalStop()) throw Exception("Linestop save error. Missing physical stop");
			if(!object->getLine()) throw Exception("Linestop Save error. Missing line");

			ReplaceQuery<LineStopTableSync> query(*object);
			query.addField(object->getPhysicalStop()->getKey());
			query.addField(object->getLine()->getKey());
			query.addField(object->getRankInPath());
			query.addField(object->isDepartureAllowed());
			query.addField(object->isArrivalAllowed());
			query.addField(object->getMetricOffset());
			query.addField(object->getScheduleInput());
			query.addField(false);
			query.addField(static_pointer_cast<Geometry,LineString>(object->getGeometry()));
			query.execute(transaction);
		}
	}

	namespace pt
	{
		DesignatedLinePhysicalStopInheritedTableSync::SearchResult DesignatedLinePhysicalStopInheritedTableSync::Search(
			Env& env,
			size_t first,
			boost::optional<std::size_t> number,
			bool orderById,
			bool raisingOrder,
			LinkLevel linkLevel
		){
			SelectQuery<LineStopTableSync> query;
			
			// Ordering
			if(orderById)
			{
				query.addOrderField(TABLE_COL_ID, raisingOrder);
			}
			if (number)
			{
				query.setNumber(*number + 1);
				if (first > 0)
				{
					query.setFirst(first);
				}
			}

			return LoadFromQuery(query.toString(), env, linkLevel);
		}
}	}
