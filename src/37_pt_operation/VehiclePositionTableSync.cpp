
//////////////////////////////////////////////////////////////////////////
///	VehiclePositionTableSync class implementation.
///	@file VehiclePositionTableSync.cpp
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

#include "VehiclePositionTableSync.hpp"
#include "ReplaceQuery.h"
#include "SelectQuery.hpp"

using namespace std;
using namespace boost;

namespace synthese
{
	using namespace db;
	using namespace util;
	using namespace pt_operation;

	namespace util
	{
		template<> const string FactorableTemplate<DBTableSync,VehiclePositionTableSync>::FACTORY_KEY("37.20 Vehicle positions");
	}

	namespace pt_operation
	{
		const std::string VehiclePositionTableSync::COL_STATUS;
		const std::string VehiclePositionTableSync::COL_VEHICLE_ID;
		const std::string VehiclePositionTableSync::COL_TIME;
		const std::string VehiclePositionTableSync::COL_METER_OFFSET;
		const std::string VehiclePositionTableSync::COL_STOP_POINT_ID;
		const std::string VehiclePositionTableSync::COL_COMMENT;
		const std::string VehiclePositionTableSync::COL_SERVICE_ID;
		const std::string VehiclePositionTableSync::COL_RANK_IN_PATH;
		const std::string VehiclePositionTableSync::COL_PASSENGERS;
		const std::string VehiclePositionTableSync::COL_GEOMETRY;
	}
	
	namespace db
	{
		template<> const DBTableSync::Format DBTableSyncTemplate<VehiclePositionTableSync>::TABLE(
			"t072_vehicle_positions"
		);



		template<> const DBTableSync::Field DBTableSyncTemplate<VehiclePositionTableSync>::_FIELDS[]=
		{
			DBTableSync::Field(TABLE_COL_ID, SQL_INTEGER),
			DBTableSync::Field(VehiclePositionTableSync::COL_STATUS, SQL_INTEGER),
			DBTableSync::Field(VehiclePositionTableSync::COL_VEHICLE_ID, SQL_INTEGER),
			DBTableSync::Field(VehiclePositionTableSync::COL_TIME, SQL_TEXT),
			DBTableSync::Field(VehiclePositionTableSync::COL_METER_OFFSET, SQL_DOUBLE),
			DBTableSync::Field(VehiclePositionTableSync::COL_STOP_POINT_ID, SQL_INTEGER),
			DBTableSync::Field(VehiclePositionTableSync::COL_COMMENT, SQL_TEXT),
			DBTableSync::Field(VehiclePositionTableSync::COL_SERVICE_ID, SQL_INTEGER),
			DBTableSync::Field(VehiclePositionTableSync::COL_RANK_IN_PATH, SQL_INTEGER),
			DBTableSync::Field(VehiclePositionTableSync::COL_PASSENGERS, SQL_INTEGER),
			DBTableSync::Field(VehiclePositionTableSync::COL_GEOMETRY, SQL_GEOM_POINT),
			DBTableSync::Field()
		};



		template<> const DBTableSync::Index DBTableSyncTemplate<VehiclePositionTableSync>::_INDEXES[]=
		{
			// DBTableSync::Index(
			//	VehiclePositionTableSync::COL_NAME.c_str(),
			// ""),
			DBTableSync::Index()
		};



		template<> void DBDirectTableSyncTemplate<VehiclePositionTableSync,VehiclePosition>::Load(
			VehiclePosition* object,
			const db::DBResultSPtr& rows,
			Env& env,
			LinkLevel linkLevel
		){
			// object->setName(rows->getText(VehiclePositionTableSync::COL_NAME));

			// if(linkLevel >= UP_LINKS_LOAD_LEVEL)
			// {
			//	RegistryKeyType pid(rows->getLongLong(VehiclePositionTableSync::COL_PARENT_ID));
			//	if(pid > 0)
			//	{
			//		try
			//		{
			//			object->setParent(GetEditable(pid, env, linkLevel).get());
			//		}
			//		catch(ObjectNotFoundException<xxx>& e)
			//		{
			//			Log::GetInstance().warn("No such parent "+ lexical_cast<string>(pid) +" in VehiclePosition "+ lexical_cast<string>(object->getKey()));
			//		}
			//	}
			// }
		}



		template<> void DBDirectTableSyncTemplate<VehiclePositionTableSync,VehiclePosition>::Save(
			VehiclePosition* object,
			optional<DBTransaction&> transaction
		){
			ReplaceQuery<VehiclePositionTableSync> query(*object);
			// query.addField(object->getName());
			// query.addField(object->_getParent() ? object->_getParent()->getKey() : RegistryKeyType(0));
			query.execute(transaction);
		}



		template<> void DBDirectTableSyncTemplate<VehiclePositionTableSync,VehiclePosition>::Unlink(
			VehiclePosition* obj
		){
		}



		template<> bool DBTableSyncTemplate<VehiclePositionTableSync>::CanDelete(
			const server::Session* session,
			util::RegistryKeyType object_id
		){
			//TODO Check the user rights
			return true;
		}



		template<> void DBTableSyncTemplate<VehiclePositionTableSync>::BeforeDelete(
			util::RegistryKeyType id,
			db::DBTransaction& transaction
		){
		}



		template<> void DBTableSyncTemplate<VehiclePositionTableSync>::AfterDelete(
			util::RegistryKeyType id,
			db::DBTransaction& transaction
		){
		}



		void DBTableSyncTemplate<VehiclePositionTableSync>::LogRemoval(
			const server::Session* session,
			util::RegistryKeyType id
		){
			//TODO Log the removal
		}
	}
	
	
	
	namespace pt_operation
	{
		VehiclePositionTableSync::SearchResult VehiclePositionTableSync::Search(
			util::Env& env,
			// boost::optional<util::RegistryKeyType> parameterId /*= boost::optional<util::RegistryKeyType>()*/,
			size_t first /*= 0*/,
			optional<size_t> number /*= boost::optional<std::size_t>()*/,
			bool orderByName,
			bool raisingOrder,
			util::LinkLevel linkLevel /*= util::FIELDS_ONLY_LOAD_LEVEL */
		){
			SelectQuery<VehiclePositionTableSync> query;
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
	}
}
