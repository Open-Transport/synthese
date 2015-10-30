/** StopPointTableSync class implementation.
	@file StopPointTableSync.cpp

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

#include "StopPointTableSync.hpp"

#include "CityTableSync.h"
#include "CoordinatesSystem.hpp"
#include "CrossingTableSync.hpp"
#include "DataSourceLinksField.hpp"
#include "ImportableTableSync.hpp"
#include "LinkException.h"
#include "Profile.h"
#include "PTUseRuleTableSync.h"
#include "ReplaceQuery.h"
#include "RoadChunkTableSync.h"
#include "SelectQuery.hpp"
#include "Session.h"
#include "StopAreaTableSync.hpp"
#include "TransportNetworkRight.h"
#include "User.h"

#include <geos/geom/Coordinate.h>
#include <geos/geom/Point.h>

using namespace std;
using namespace boost;
using namespace geos::geom;

namespace synthese
{
	using namespace db;
	using namespace util;
	using namespace pt;
	using namespace geography;
	using namespace road;
	using namespace impex;
	using namespace security;
	using namespace graph;

	template<> const string util::FactorableTemplate<DBTableSync,StopPointTableSync>::FACTORY_KEY("35.55.01 Physical stops");
	template<> const string FactorableTemplate<Fetcher<graph::Vertex>, StopPointTableSync>::FACTORY_KEY("12");

	namespace pt
	{
	}

	namespace db
	{
		template<> const DBTableSync::Format DBTableSyncTemplate<StopPointTableSync>::TABLE(
			"t012_physical_stops"
		);


		template<> const Field DBTableSyncTemplate<StopPointTableSync>::_FIELDS[]=
		{
			Field()
		};


		template<>
		DBTableSync::Indexes DBTableSyncTemplate<StopPointTableSync>::GetIndexes()
		{
			DBTableSync::Indexes r;
			r.push_back(DBTableSync::Index(ConnectionPlace::FIELD.name.c_str(), ""));
			r.push_back(DBTableSync::Index(OperatorCode::FIELD.name.c_str(), ""));
			return r;
		}


		template<> bool DBTableSyncTemplate<StopPointTableSync>::CanDelete(
			const server::Session* session,
			util::RegistryKeyType object_id
		){
			return session && session->hasProfile() && session->getUser()->getProfile()->isAuthorized<TransportNetworkRight>(DELETE_RIGHT);
		}



		template<> void DBTableSyncTemplate<StopPointTableSync>::BeforeDelete(
			util::RegistryKeyType id,
			db::DBTransaction& transaction
		){
		}



		template<> void DBTableSyncTemplate<StopPointTableSync>::AfterDelete(
			util::RegistryKeyType id,
			db::DBTransaction& transaction
		){
			Env env;
			try
			{
				boost::shared_ptr<const StopPoint> stopPoint(StopPointTableSync::Get(id, env));
				StopArea::TransferDelaysMap tdMap(stopPoint->getConnectionPlace()->getTransferDelays());
				StopArea::TransferDelaysMap value(stopPoint->getConnectionPlace()->getTransferDelays());
				BOOST_FOREACH(const StopArea::TransferDelaysMap::value_type& td, tdMap)
				{
					if(td.first.first == id || td.first.second == id)
					{
						value.erase(
							make_pair(
								td.first.first,
								td.first.second
						)	);
					}
				}
				if(value != tdMap)
				{
					const_cast<StopArea*>(stopPoint->getConnectionPlace())->setTransferDelaysMatrix(value);
					StopAreaTableSync::Save(const_cast<StopArea*>(stopPoint->getConnectionPlace()), transaction);
				}
			}
			catch (...)
			{}
		}



		template<> void DBTableSyncTemplate<StopPointTableSync>::LogRemoval(
			const server::Session* session,
			util::RegistryKeyType id
		){
			//TODO Log the removal
		}
	}

	namespace pt
	{

		bool StopPointTableSync::allowList(const server::Session* session) const
		{
			return true;
		}

		StopPointTableSync::SearchResult StopPointTableSync::Search(
			Env& env,
			optional<RegistryKeyType> placeId,
			optional<string> operatorCode,
			bool orderByCityAndStopName,
			bool raisingOrder,
			int first /*= 0 */,
			boost::optional<std::size_t> number  /*= 0 */,
			LinkLevel linkLevel
		){
			SelectQuery<StopPointTableSync> query;
			if(operatorCode)
			{
				query.addWhereField(OperatorCode::FIELD.name, *operatorCode, ComposedExpression::OP_LIKE);
			}
			if(placeId)
			{
				query.addWhereField(ConnectionPlace::FIELD.name, *placeId);
			}
			if(orderByCityAndStopName)
			{
				query.addTableAndEqualJoin<StopAreaTableSync>(Key::FIELD.name, ConnectionPlace::FIELD.name);
				query.addTableAndEqualOtherJoin<CityTableSync, StopAreaTableSync>(Key::FIELD.name, pt::CityId::FIELD.name);
				query.addOrderFieldOther<CityTableSync>(CityTableSync::TABLE_COL_NAME, raisingOrder);
				query.addOrderFieldOther<StopAreaTableSync>(SimpleObjectFieldDefinition<Name>::FIELD.name, raisingOrder);
				query.addOrderField(Name::FIELD.name, raisingOrder);
			}
			if(number)
			{
				query.setNumber(*number + 1);
				if(first > 0)
					query.setFirst(first);
			}

			return LoadFromQuery(query, env, linkLevel);
		}

		
		
		StopPointTableSync::SearchResult StopPointTableSync::SearchDistance(
			StopPoint const& point,
			Env& env,
			bool insideSameStopArea,
			double const& distance,
			bool maxDistance,
			LinkLevel linkLevel
		){
			SelectQuery<StopPointTableSync> query;
			std::stringstream subQuery;

			subQuery << "Glength(GeomFromText('LINESTRING(" << lexical_cast<string>(point.getGeometry()->getX()) << " " 
				<< lexical_cast<string>(point.getGeometry()->getY()) << ", '||" << "t012_physical_stops.x" 
				<< "||' '||" << "t012_physical_stops.y" << "||')'))" << (maxDistance ? ">" : "<=") << distance 
				<< " AND " << point.getKey() <<"!=" << "t012_physical_stops.id ";
			if(insideSameStopArea)
				subQuery << "AND " << point.getConnectionPlace()->getKey() << "=" << "t012_physical_stops.place_id";

			query.addWhere(
				SubQueryExpression::Get(subQuery.str())
			);
			
			return LoadFromQuery(query, env, linkLevel);
		}



		bool StopPointTableSync::SearchDistance(
			StopPoint const& point1,
			StopPoint const& point2,
			double const& distance,
			bool maxDistance
		){
			std::stringstream query;
			DB* db = DBModule::GetDB();

			query << "SELECT Glength(GeomFromText('LINESTRING(" << lexical_cast<string>(point1.getGeometry()->getX()) << " " 
				<< lexical_cast<string>(point1.getGeometry()->getY()) << ", " << lexical_cast<string>(point2.getGeometry()->getX()) << " "
				<< lexical_cast<string>(point2.getGeometry()->getY()) << ")'))" << (maxDistance ? ">" : "<=") << distance << " AS distance";
			
			DBResultSPtr rows = db->execQuery(query.str());
			
			if (rows->next())
				return rows->getBool("distance");
			else
				return false;
		}



		db::RowsList StopPointTableSync::SearchForAutoComplete(
			const boost::optional<std::string> prefix,
			const boost::optional<std::size_t> limit,
			const boost::optional<std::string> optionalParameter
			) const {
				RowsList result;

				SelectQuery<StopPointTableSync> query;
				Env env;
				if(prefix)
				{
					query.addWhereField(Name::FIELD.name, "%"+ *prefix +"%", ComposedExpression::OP_LIKE);
				}
				if(optionalParameter)
				{
					query.addWhereField(ConnectionPlace::FIELD.name, *optionalParameter);
				}
				if(limit)
				{
					query.setNumber(*limit);
				}
				query.addOrderField(Name::FIELD.name, true);
				StopPointTableSync::SearchResult stops(StopPointTableSync::LoadFromQuery(query, env, UP_LINKS_LOAD_LEVEL));
				BOOST_FOREACH(const boost::shared_ptr<StopPoint>& stop, stops)
				{
					result.push_back(std::make_pair(stop->getKey(), stop->getCodeBySources() + " / " + stop->getName()));
				}
				return result;
		}
}	}
