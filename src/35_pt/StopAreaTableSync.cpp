////////////////////////////////////////////////////////////////////////////////
/// StopAreaTableSync class implementation.
///	@file StopAreaTableSync.cpp
///	@author Hugues Romain
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

#include "StopAreaTableSync.hpp"

#include "DataSourceLinksField.hpp"
#include "Profile.h"
#include "ReplaceQuery.h"
#include "Session.h"
#include "User.h"
#include "CityTableSync.h"
#include "SelectQuery.hpp"
#include "ImportableTableSync.hpp"
#include "StopPointTableSync.hpp"
#include "TransportNetworkRight.h"
#include "PTModule.h"
#include "PTUseRuleTableSync.h"
#include "AllowedUseRule.h"
#include "ForbiddenUseRule.h"

#include <geos/geom/Point.h>

#include <boost/algorithm/string.hpp>
#include <boost/tokenizer.hpp>
#include <assert.h>

using namespace std;
using namespace boost;
using namespace boost::algorithm;
using namespace geos::geom;

namespace synthese
{
	using namespace db;
	using namespace pt;
	using namespace util;
	using namespace geography;
	using namespace road;
	using namespace impex;
	using namespace security;
	using namespace graph;

	template<> const string util::FactorableTemplate<DBTableSync,pt::StopAreaTableSync>::FACTORY_KEY("35.40.01 Connection places");
	template<> const string FactorableTemplate<Fetcher<NamedPlace>, StopAreaTableSync>::FACTORY_KEY("7");

	namespace pt
	{
		const string StopAreaTableSync::TABLE_COL_NAME = "name";
		const string StopAreaTableSync::TABLE_COL_CITYID = "city_id";
		const string StopAreaTableSync::TABLE_COL_CONNECTIONTYPE = "connection_type";
		const string StopAreaTableSync::TABLE_COL_ISCITYMAINCONNECTION = "is_city_main_connection";
		const string StopAreaTableSync::TABLE_COL_DEFAULTTRANSFERDELAY = "default_transfer_delay";
		const string StopAreaTableSync::TABLE_COL_TRANSFERDELAYS = "transfer_delays";
		const string StopAreaTableSync::COL_NAME13 = "short_display_name";
		const string StopAreaTableSync::COL_NAME26 = "long_display_name";
		const string StopAreaTableSync::COL_CODE_BY_SOURCE = "code_by_source";
		const string StopAreaTableSync::COL_TIMETABLE_NAME = "timetable_name";
		const string StopAreaTableSync::COL_HANDICAPPED_COMPLIANCE_ID = "handicapped_compliance_id";
		const string StopAreaTableSync::COL_X = "x";
		const string StopAreaTableSync::COL_Y = "y";

		const string StopAreaTableSync::FORBIDDEN_DELAY_SYMBOL = "F";
	}

	namespace db
	{
		template<> const DBTableSync::Format DBTableSyncTemplate<StopAreaTableSync>::TABLE(
			"t007_connection_places"
		);

		template<> const Field DBTableSyncTemplate<StopAreaTableSync>::_FIELDS[] =
		{
			Field(TABLE_COL_ID, SQL_INTEGER),
			Field(StopAreaTableSync::TABLE_COL_NAME, SQL_TEXT),
			Field(StopAreaTableSync::TABLE_COL_CITYID, SQL_INTEGER),
			Field(StopAreaTableSync::TABLE_COL_CONNECTIONTYPE, SQL_INTEGER),
			Field(StopAreaTableSync::TABLE_COL_ISCITYMAINCONNECTION, SQL_BOOLEAN),
			Field(StopAreaTableSync::TABLE_COL_DEFAULTTRANSFERDELAY, SQL_INTEGER),
			Field(StopAreaTableSync::TABLE_COL_TRANSFERDELAYS, SQL_TEXT),
			Field(StopAreaTableSync::COL_NAME13, SQL_TEXT),
			Field(StopAreaTableSync::COL_NAME26, SQL_TEXT),
			Field(StopAreaTableSync::COL_CODE_BY_SOURCE, SQL_TEXT),
			Field(StopAreaTableSync::COL_TIMETABLE_NAME, SQL_TEXT),
			Field(StopAreaTableSync::COL_HANDICAPPED_COMPLIANCE_ID, SQL_INTEGER),
			Field(StopAreaTableSync::COL_X, SQL_DOUBLE),
			Field(StopAreaTableSync::COL_Y, SQL_DOUBLE),
			Field(TABLE_COL_GEOMETRY, SQL_GEOM_POINT),
			Field()
		};

		template<>
		DBTableSync::Indexes DBTableSyncTemplate<StopAreaTableSync>::GetIndexes()
		{
			DBTableSync::Indexes r;
			r.push_back(DBTableSync::Index(StopAreaTableSync::TABLE_COL_CITYID.c_str(), StopAreaTableSync::TABLE_COL_NAME.c_str(), ""));
			r.push_back(DBTableSync::Index(StopAreaTableSync::COL_CODE_BY_SOURCE.c_str(), ""));
			return r;
		}


		template<> void OldLoadSavePolicy<StopAreaTableSync,StopArea>::Load(
			StopArea* cp,
			const db::DBResultSPtr& rows,
			Env& env,
			LinkLevel linkLevel
		){
			DBModule::LoadObjects(cp->getLinkedObjectsIds(*rows), env, linkLevel);
			cp->loadFromRecord(*rows, env);
			if(linkLevel > util::FIELDS_ONLY_LOAD_LEVEL)
			{
				cp->link(env, linkLevel == util::ALGORITHMS_OPTIMIZATION_LOAD_LEVEL);
			}
		}



		template<> void OldLoadSavePolicy<StopAreaTableSync,StopArea>::Save(
			StopArea* object,
			optional<DBTransaction&> transaction
		){
			ReplaceQuery<StopAreaTableSync> query(*object);

			// Name
			query.addField(object->getName());

			// City
			query.addField(object->getCity() ? object->getCity()->getKey() : RegistryKeyType(0));

			// Transfer allowed
			query.addField(object->getAllowedConnection());

			// Is a main stop of the city
			query.addField(object->getCity() ? object->getCity()->includes(*object) : false);

			// Default transfer delay
			query.addField(object->getDefaultTransferDelay().total_seconds() / 60);

			// Transfer delay matrix
			query.addField(StopArea::SerializeTransferDelaysMatrix(object->getTransferDelays()));

			// Name 13
			query.addField(object->getName13());

			// Name 26
			query.addField(object->getName26());

			// Data source links
			query.addField(
				DataSourceLinks::Serialize(
					object->getDataSourceLinks()
			)	);

			// Timetable name
			query.addField(object->getTimetableName());

			// Handicapped compliance
			query.addField(
				object->getRule(USER_HANDICAPPED) && dynamic_cast<const PTUseRule*>(object->getRule(USER_HANDICAPPED)) ?
				static_cast<const PTUseRule*>(object->getRule(USER_HANDICAPPED))->getKey() : RegistryKeyType(0)
			);

			// X Y (deprecated)
			if(object->getLocation())
			{
				query.addField(object->getLocation()->getX());
				query.addField(object->getLocation()->getY());
			}
			else
			{
				query.addFieldNull();
				query.addFieldNull();
			}

			// Geometry
			if(object->getLocation())
			{
				query.addField(static_pointer_cast<Geometry,Point>(object->getLocation()));
			}
			else
			{
				query.addFieldNull();
			}

			query.execute(transaction);
		}



		template<> void OldLoadSavePolicy<StopAreaTableSync,StopArea>::Unlink(
			StopArea* cp
		){
			// City
			City* city(const_cast<City*>(cp->getCity()));
			if (city != NULL)
			{
				city->removePlaceFromMatcher(*cp);
				city->removeIncludedPlace(*cp);
			}

			if(Env::GetOfficialEnv().contains(*cp))
			{
				// General all places
				GeographyModule::GetGeneralAllPlacesMatcher().remove(
					cp->getFullName()
				);

				// General public places
				PTModule::GetGeneralStopsMatcher().remove(
					cp->getFullName()
				);

				// Unregister data source links
				cp->cleanDataSourceLinks(true);
			}
		}



		template<> bool DBTableSyncTemplate<StopAreaTableSync>::CanDelete(
			const server::Session* session,
			util::RegistryKeyType object_id
		){
			return session && session->hasProfile() && session->getUser()->getProfile()->isAuthorized<TransportNetworkRight>(DELETE_RIGHT);
		}



		template<> void DBTableSyncTemplate<StopAreaTableSync>::BeforeDelete(
			util::RegistryKeyType id,
			db::DBTransaction& transaction
		){
			Env env;
			StopPointTableSync::SearchResult stops(StopPointTableSync::Search(env, id));
			BOOST_FOREACH(const StopPointTableSync::SearchResult::value_type& stop, stops)
			{
				StopPointTableSync::Remove(NULL, stop->getKey(), transaction, false);
			}
		}



		template<> void DBTableSyncTemplate<StopAreaTableSync>::AfterDelete(
			util::RegistryKeyType id,
			db::DBTransaction& transaction
		){
		}



		template<> void DBTableSyncTemplate<StopAreaTableSync>::LogRemoval(
			const server::Session* session,
			util::RegistryKeyType id
		){
			//TODO Log the removal
		}
	}

	namespace pt
	{
		StopAreaTableSync::SearchResult StopAreaTableSync::Search(
			Env& env,
			optional<RegistryKeyType> cityId,
			logic::tribool mainConnectionOnly,
			optional<string> creatorIdFilter,
			optional<string> nameFilter,
			optional<string> cityNameFilter,
			bool orderByCityNameAndName /*= true */
			, bool raisingOrder /*= true */
			, int first /*= 0 */
			, int number /*= 0 */,
			LinkLevel linkLevel
		){
			SelectQuery<StopAreaTableSync> query;
			if(orderByCityNameAndName || cityNameFilter)
			{
				query.addTableAndEqualJoin<CityTableSync>(TABLE_COL_ID, TABLE_COL_CITYID);
			}

			// Filters
			if (cityId)
			{
				query.addWhereField(TABLE_COL_CITYID, *cityId);
			}
			if (!logic::indeterminate(mainConnectionOnly))
			{
				query.addWhereField(TABLE_COL_ISCITYMAINCONNECTION, mainConnectionOnly);
			}
			if(creatorIdFilter)
			{
				query.addWhereField(COL_CODE_BY_SOURCE, *creatorIdFilter);
			}
			if(nameFilter)
			{
				query.addWhereField(TABLE_COL_NAME, *nameFilter, ComposedExpression::OP_LIKE);
			}
			if(cityNameFilter)
			{
				query.addWhereFieldOther<CityTableSync>(CityTableSync::TABLE_COL_NAME, *cityNameFilter, ComposedExpression::OP_LIKE);
			}

			// Ordering
			if(orderByCityNameAndName)
			{
				query.addOrderFieldOther<CityTableSync>(CityTableSync::TABLE_COL_NAME, raisingOrder);
				query.addOrderField(TABLE_COL_NAME, raisingOrder);
			}
			if (number > 0)
			{
				query.setNumber(number + 1);
				if (first > 0)
				{
					query.setFirst(first);
				}
			}

			return LoadFromQuery(query, env, linkLevel);
		}

	
	
		StopAreaTableSync::SearchResult StopAreaTableSync::SearchSameName(
			StopArea const& stoparea, 
			Env& env,
			LinkLevel linkLevel
		){
			SelectQuery<StopAreaTableSync> query;
			std::stringstream subQuery;

			if(stoparea.getCity())
			{
				subQuery << stoparea.getKey() << " != t007_connection_places.id AND "
					<< "\"" << stoparea.getName() << "\"" << " = t007_connection_places.name AND " 
					<< stoparea.getCity()->getKey() << " = t007_connection_places.city_id";
			}
			else
			{
				subQuery << stoparea.getKey() << " != t007_connection_places.id AND "
				<< "\"" << stoparea.getName() << "\"" << " = t007_connection_places.name"; 
			}
			
			query.addWhere(
				SubQueryExpression::Get(subQuery.str())
			);

			return LoadFromQuery(query, env, linkLevel);
		}
	}
}
