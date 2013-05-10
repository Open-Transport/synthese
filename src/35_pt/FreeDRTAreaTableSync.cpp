
//////////////////////////////////////////////////////////////////////////
///	FreeDRTAreaTableSync class implementation.
///	@file FreeDRTAreaTableSync.cpp
///	@author Hugues Romain
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

#include "FreeDRTAreaTableSync.hpp"

#include "Profile.h"
#include "ReplaceQuery.h"
#include "SelectQuery.hpp"
#include "Session.h"
#include "User.h"
#include "CommercialLineTableSync.h"
#include "CityTableSync.h"
#include "StopAreaTableSync.hpp"
#include "RollingStockTableSync.hpp"
#include "FreeDRTTimeSlotTableSync.hpp"
#include "TransportNetworkRight.h"
#include "PTUseRuleTableSync.h"

#include <sstream>
#include <boost/algorithm/string/split.hpp>

using namespace std;
using namespace boost;
using namespace boost::algorithm;

namespace synthese
{
	using namespace db;
	using namespace util;
	using namespace security;
	using namespace vehicle;
	using namespace pt;
	using namespace geography;
	using namespace graph;

	namespace util
	{
		template<> const string FactorableTemplate<DBTableSync,FreeDRTAreaTableSync>::FACTORY_KEY("35.80 Free DRT Areas");
	}

	namespace pt
	{
		const string FreeDRTAreaTableSync::COL_COMMERCIAL_LINE_ID = "commercial_line_id";
		const string FreeDRTAreaTableSync::COL_TRANSPORT_MODE_ID = "transport_mode_id";
		const string FreeDRTAreaTableSync::COL_NAME = "name";
		const string FreeDRTAreaTableSync::COL_CITIES = "cities";
		const string FreeDRTAreaTableSync::COL_STOP_AREAS = "stop_areas";
		const string FreeDRTAreaTableSync::COL_USE_RULES = "use_rules";
	}

	namespace db
	{
		template<> const DBTableSync::Format DBTableSyncTemplate<FreeDRTAreaTableSync>::TABLE(
			"t082_free_drt_areas"
		);



		template<> const Field DBTableSyncTemplate<FreeDRTAreaTableSync>::_FIELDS[]=
		{
			Field(TABLE_COL_ID, SQL_INTEGER),
			Field(FreeDRTAreaTableSync::COL_COMMERCIAL_LINE_ID, SQL_INTEGER),
			Field(FreeDRTAreaTableSync::COL_TRANSPORT_MODE_ID, SQL_INTEGER),
			Field(FreeDRTAreaTableSync::COL_NAME, SQL_TEXT),
			Field(FreeDRTAreaTableSync::COL_CITIES, SQL_TEXT),
			Field(FreeDRTAreaTableSync::COL_STOP_AREAS, SQL_TEXT),
			Field(FreeDRTAreaTableSync::COL_USE_RULES, SQL_TEXT),
			Field()
		};



		template<>
		DBTableSync::Indexes DBTableSyncTemplate<FreeDRTAreaTableSync>::GetIndexes()
		{
			DBTableSync::Indexes r;
			r.push_back(
				DBTableSync::Index(
					FreeDRTAreaTableSync::COL_COMMERCIAL_LINE_ID.c_str(),
			"")	);
			return r;
		};



		template<> void OldLoadSavePolicy<FreeDRTAreaTableSync,FreeDRTArea>::Load(
			FreeDRTArea* object,
			const db::DBResultSPtr& rows,
			Env& env,
			LinkLevel linkLevel
		){
			// Commercial lines
			object->setLine(NULL);
			object->setNetwork(NULL);
			if(linkLevel >= UP_LINKS_LOAD_LEVEL)
			{
				RegistryKeyType commercialLineId(rows->getLongLong (FreeDRTAreaTableSync::COL_COMMERCIAL_LINE_ID));
				if(commercialLineId) try
				{
					CommercialLine* cline(CommercialLineTableSync::GetEditable(commercialLineId, env, linkLevel).get());
					object->setNetwork(cline->getNetwork());
					object->setLine(cline);
				}
				catch(ObjectNotFoundException<CommercialLine>)
				{
					Log::GetInstance().warn("Bad value " + lexical_cast<string>(commercialLineId) + " for line in free DRT area " + lexical_cast<string>(object->getKey()));
				}
			}

			// Transport mode id
			object->setRollingStock(NULL);
			if(linkLevel >= UP_LINKS_LOAD_LEVEL)
			{
				RegistryKeyType transportModeId(rows->getLongLong (FreeDRTAreaTableSync::COL_TRANSPORT_MODE_ID));
				if(transportModeId)	try
				{
					RollingStock* transportMode(RollingStockTableSync::GetEditable(transportModeId, env, linkLevel).get());
					object->setRollingStock(transportMode);
				}
				catch(ObjectNotFoundException<RollingStock>)
				{
					Log::GetInstance().warn("Bad value " + lexical_cast<string>(transportModeId) + " for transport mode in free DRT area " + lexical_cast<string>(object->getKey()));
				}
			}

			// Name
			object->setName(rows->getText(FreeDRTAreaTableSync::COL_NAME));

			// Cities
			if(linkLevel >= UP_LINKS_LOAD_LEVEL)
			{
				object->setCities(
					FreeDRTAreaTableSync::UnserializeCities(
						rows->getText(FreeDRTAreaTableSync::COL_CITIES),
						env,
						linkLevel
				)	);
			}

			// Stop areas
			if(linkLevel >= UP_LINKS_LOAD_LEVEL)
			{
				object->setStopAreas(
					FreeDRTAreaTableSync::UnserializeStopAreas(
						rows->getText(FreeDRTAreaTableSync::COL_STOP_AREAS),
						env,
						linkLevel
				)	);
			}

			// Use rules
			if(linkLevel >= UP_LINKS_LOAD_LEVEL)
			{
				object->setRules(
					PTUseRuleTableSync::UnserializeUseRules(
						rows->getText(FreeDRTAreaTableSync::COL_USE_RULES),
						env,
						linkLevel
				)	);
			}
		}



		template<> void OldLoadSavePolicy<FreeDRTAreaTableSync,FreeDRTArea>::Save(
			FreeDRTArea* object,
			optional<DBTransaction&> transaction
		){
			ReplaceQuery<FreeDRTAreaTableSync> query(*object);
			query.addField(object->getLine() ? object->getLine()->getKey() : RegistryKeyType(0));
			query.addField(object->getRollingStock() ? object->getRollingStock()->getKey() : RegistryKeyType(0));
			query.addField(object->getName());
			query.addField(FreeDRTAreaTableSync::SerializeCities(object->getCities()));
			query.addField(FreeDRTAreaTableSync::SerializeStopAreas(object->getStopAreas()));
			query.addField(PTUseRuleTableSync::SerializeUseRules(object->getRules()));
			query.execute(transaction);
		}



		template<> void OldLoadSavePolicy<FreeDRTAreaTableSync,FreeDRTArea>::Unlink(
			FreeDRTArea* obj
		){
			if(obj->getLine())
			{
				const_cast<CommercialLine*>(obj->getLine())->removePath(obj);
			}
		}



		template<> bool DBTableSyncTemplate<FreeDRTAreaTableSync>::CanDelete(
			const server::Session* session,
			util::RegistryKeyType object_id
		){
			return session && session->hasProfile() && session->getUser()->getProfile()->isAuthorized<TransportNetworkRight>(DELETE_RIGHT);
		}



		template<> void DBTableSyncTemplate<FreeDRTAreaTableSync>::BeforeDelete(
			util::RegistryKeyType id,
			db::DBTransaction& transaction
		){
			// Services deletion
			Env env;
			FreeDRTTimeSlotTableSync::SearchResult sservices(FreeDRTTimeSlotTableSync::Search(env, id));
			BOOST_FOREACH(const FreeDRTTimeSlotTableSync::SearchResult::value_type& sservice, sservices)
			{
				FreeDRTTimeSlotTableSync::Remove(NULL, sservice->getKey(), transaction, false);
			}
		}



		template<> void DBTableSyncTemplate<FreeDRTAreaTableSync>::AfterDelete(
			util::RegistryKeyType id,
			db::DBTransaction& transaction
		){
		}



		template<> void DBTableSyncTemplate<FreeDRTAreaTableSync>::LogRemoval(
			const server::Session* session,
			util::RegistryKeyType id
		){
		}

	}



	namespace pt
	{
		FreeDRTAreaTableSync::SearchResult FreeDRTAreaTableSync::Search(
			util::Env& env,
			boost::optional<util::RegistryKeyType> lineId,
			size_t first /*= 0*/,
			optional<size_t> number /*= boost::optional<std::size_t>()*/,
			bool orderByName,
			bool raisingOrder,
			util::LinkLevel linkLevel
		){
			SelectQuery<FreeDRTAreaTableSync> query;
			if(lineId)
			{
			 	query.addWhereField(COL_COMMERCIAL_LINE_ID, *lineId);
			}
			if(orderByName)
			{
			 	query.addOrderField(COL_NAME, raisingOrder);
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



		FreeDRTArea::Cities FreeDRTAreaTableSync::UnserializeCities(
			const std::string& value,
			util::Env& env,
			util::LinkLevel linkLevel /*= util::UP_LINKS_LOAD_LEVEL */
		){
			FreeDRTArea::Cities result;
			if(!value.empty())
			{
				vector<string> cities;
				split(cities, value, is_any_of(","));
				BOOST_FOREACH(const string& city, cities)
				{
					try
					{
						RegistryKeyType cityId(lexical_cast<RegistryKeyType>(city));
						result.insert(
							CityTableSync::GetEditable(cityId, env, linkLevel).get()
						);
					}
					catch(ObjectNotFoundException<City>&)
					{
					}
				}
			}
			return result;
		}



		std::string FreeDRTAreaTableSync::SerializeCities( const FreeDRTArea::Cities& value )
		{
			stringstream s;
			bool first(true);
			BOOST_FOREACH(const FreeDRTArea::Cities::value_type& city, value)
			{
				if(first)
				{
					first = false;
				}
				else
				{
					s << ",";
				}
				s << city->getKey();
			}
			return s.str();
		}



		FreeDRTArea::StopAreas FreeDRTAreaTableSync::UnserializeStopAreas(
			const std::string& value,
			util::Env& env,
			util::LinkLevel linkLevel /*= util::UP_LINKS_LOAD_LEVEL */
		){
			FreeDRTArea::StopAreas result;
			if(!value.empty())
			{
				vector<string> stopAreas;
				split(stopAreas, value, is_any_of(","));
				BOOST_FOREACH(const string& stopArea, stopAreas)
				{
					try
					{
						RegistryKeyType stopAreaId(lexical_cast<RegistryKeyType>(stopArea));
						result.insert(
							StopAreaTableSync::GetEditable(stopAreaId, env, linkLevel).get()
						);
					}
					catch(ObjectNotFoundException<City>&)
					{
					}
				}
			}
			return result;

		}



		std::string FreeDRTAreaTableSync::SerializeStopAreas( const FreeDRTArea::StopAreas& value )
		{
			stringstream s;
			bool first(true);
			BOOST_FOREACH(const FreeDRTArea::StopAreas::value_type& stopArea, value)
			{
				if(first)
				{
					first = false;
				}
				else
				{
					s << ",";
				}
				s << stopArea->getKey();
			}
			return s.str();
		}
}	}
