
//////////////////////////////////////////////////////////////////////////
///	DRTAreaTableSync class implementation.
///	@file DRTAreaTableSync.cpp
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

#include <sstream>

#include "DRTAreaTableSync.hpp"
#include "ReplaceQuery.h"
#include "SelectQuery.hpp"
#include "StopAreaTableSync.hpp"
#include "StopPointTableSync.hpp"

#include <boost/algorithm/string/split.hpp>

using namespace std;
using namespace boost;

namespace synthese
{
	using namespace db;
	using namespace util;
	using namespace pt;

	namespace util
	{
		template<> const string FactorableTemplate<DBTableSync,DRTAreaTableSync>::FACTORY_KEY("35.40.05 DRT Areas");
	}

	namespace pt
	{
		const string DRTAreaTableSync::COL_NAME("name");
		const string DRTAreaTableSync::COL_STOPS("stops");
	}
	
	namespace db
	{
		template<> const DBTableSync::Format DBTableSyncTemplate<DRTAreaTableSync>::TABLE(
			"t071_drt_areas"
		);



		template<> const DBTableSync::Field DBTableSyncTemplate<DRTAreaTableSync>::_FIELDS[]=
		{
			DBTableSync::Field(TABLE_COL_ID, SQL_INTEGER),
			DBTableSync::Field(DRTAreaTableSync::COL_NAME, SQL_TEXT),
			DBTableSync::Field(DRTAreaTableSync::COL_STOPS, SQL_TEXT),
			DBTableSync::Field()
		};



		template<> const DBTableSync::Index DBTableSyncTemplate<DRTAreaTableSync>::_INDEXES[]=
		{
			DBTableSync::Index()
		};



		template<> void DBDirectTableSyncTemplate<DRTAreaTableSync,DRTArea>::Load(
			DRTArea* object,
			const db::DBResultSPtr& rows,
			Env& env,
			LinkLevel linkLevel
		){
			object->setName(rows->getText(DRTAreaTableSync::COL_NAME));
			
			if(linkLevel >= UP_LINKS_LOAD_LEVEL)
			{
				object->setStops(DRTAreaTableSync::UnserializeStops(rows->getText(DRTAreaTableSync::COL_STOPS), env));
			}
		}



		template<> void DBDirectTableSyncTemplate<DRTAreaTableSync,DRTArea>::Save(
			DRTArea* object,
			optional<DBTransaction&> transaction
		){
			ReplaceQuery<DRTAreaTableSync> query(*object);
			query.addField(object->getName());
			query.addField(DRTAreaTableSync::SerializeStops(object->getStops()));
			query.execute(transaction);
		}



		template<> void DBDirectTableSyncTemplate<DRTAreaTableSync,DRTArea>::Unlink(
			DRTArea* obj
		){
		}
	}
	
	
	
	namespace pt
	{
		DRTAreaTableSync::SearchResult DRTAreaTableSync::Search(
			util::Env& env,
			// boost::optional<util::RegistryKeyType> parameterId /*= boost::optional<util::RegistryKeyType>()*/,
			size_t first /*= 0*/,
			optional<size_t> number /*= boost::optional<std::size_t>()*/,
			bool orderByName,
			bool raisingOrder,
			util::LinkLevel linkLevel /*= util::FIELDS_ONLY_LOAD_LEVEL */
		){
			SelectQuery<DRTAreaTableSync> query;
			// if(parameterId)
			// {
			// 	query.addWhereField(COL_PARENT_ID, *parentFolderId);
			// }
			// if(orderByName)
			// {
			// 	query.addOrderField(COL_NAME, raisingOrder);
			// }
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



		std::string DRTAreaTableSync::SerializeStops( const DRTArea::Stops& value )
		{
			stringstream s;
			bool first(true);
			BOOST_FOREACH(const DRTArea::Stops::value_type& stop, value)
			{
				if(first)
				{
					first = false;
				}
				else
				{
					s << ",";
				}
				s << stop->getKey();
			}
			return s.str();
		}



		DRTArea::Stops DRTAreaTableSync::UnserializeStops( const std::string& value, util::Env& env )
		{
			DRTArea::Stops result;
			if(!value.empty())
			{
				vector<string> stops;
				split(stops, value, is_any_of(","));
				BOOST_FOREACH(const string& stop, stops)
				{
					try
					{
						RegistryKeyType placeId(lexical_cast<RegistryKeyType>(stop));
						result.insert(
							StopAreaTableSync::GetEditable(placeId, env).get()
						);
						StopPointTableSync::Search(env, placeId);
					}
					catch(ObjectNotFoundException<StopArea>&)
					{

					}
				}
			}
			return result;
		}
	}
}
