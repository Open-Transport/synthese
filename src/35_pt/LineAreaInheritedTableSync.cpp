
//////////////////////////////////////////////////////////////////////////
///	LineAreaInheritedTableSync class implementation.
///	@file LineAreaInheritedTableSync.cpp
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

#include "LineAreaInheritedTableSync.hpp"
#include "DRTAreaTableSync.hpp"
#include "ReplaceQuery.h"
#include "JourneyPattern.hpp"
#include "LinkException.h"
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
	const string util::FactorableTemplate<LineStopTableSync,LineAreaInheritedTableSync>::FACTORY_KEY("LineAreaInheritedTableSync");

	namespace db
	{
		template<>
		void DBInheritedTableSyncTemplate<LineStopTableSync,LineAreaInheritedTableSync,LineArea>::Load(
			LineArea* obj,
			const DBResultSPtr& rows,
			Env& env,
			LinkLevel linkLevel
		){
			_CommonLoad(obj, rows, env, linkLevel);

			obj->setInternalService(rows->getBool(LineStopTableSync::COL_INTERNAL_SERVICE));

			if (linkLevel > FIELDS_ONLY_LOAD_LEVEL)
			{
				RegistryKeyType areaId(rows->getLongLong(LineStopTableSync::COL_PHYSICALSTOPID));
				if(areaId) try
				{
					obj->setArea(
						*DRTAreaTableSync::GetEditable(areaId, env, linkLevel)
					);
				}
				catch(ObjectNotFoundException<DRTArea>& e)
				{
					throw LinkException<DRTAreaTableSync>(rows, LineStopTableSync::COL_PHYSICALSTOPID, e);
				}

				// Line update
				obj->getLine()->addEdge(*obj);
			}

			if (linkLevel > FIELDS_ONLY_LOAD_LEVEL)
			{
				// Sublines update
				BOOST_FOREACH(JourneyPatternCopy* copy, static_cast<LineStop*>(obj)->getLine()->getSubLines())
				{
					LineArea* newEdge(
						new LineArea(
							0,
							copy,
							obj->getRankInPath(),
							obj->isDeparture(),
							obj->isArrival(),
							obj->getMetricOffset(),
							obj->getArea(),
							obj->getInternalService()
					)	);
					copy->addEdge(*newEdge);
				}
			}
		}



		template<>
		void DBInheritedTableSyncTemplate<LineStopTableSync,LineAreaInheritedTableSync,LineArea>::Unlink(
			LineArea* obj
		){

		}



		template<>
		void DBInheritedTableSyncTemplate<LineStopTableSync,LineAreaInheritedTableSync,LineArea>::Save(
			LineArea* object,
			optional<DBTransaction&> transaction
		){
			// The query
			if(!object->getArea()) throw Exception("LineArea save error. Missing physical stop");
			if(!object->getLine()) throw Exception("LineArea Save error. Missing line");

			ReplaceQuery<LineStopTableSync> query(*object);
			query.addField(object->getArea()->getKey());
			query.addField(object->getLine()->getKey());
			query.addField(object->getRankInPath());
			query.addField(object->isDepartureAllowed());
			query.addField(object->isArrivalAllowed());
			query.addField(object->getMetricOffset());
			query.addField(true);
			query.addField(object->getInternalService());
			query.addField(static_pointer_cast<Geometry,LineString>(object->getGeometry()));
			query.execute(transaction);
		}
	}

	namespace pt
	{
		LineAreaInheritedTableSync::SearchResult LineAreaInheritedTableSync::Search(
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
