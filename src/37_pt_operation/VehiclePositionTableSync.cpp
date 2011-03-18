
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
		template<> const string FactorableTemplate<SQLiteTableSync,VehiclePositionTableSync>::FACTORY_KEY("37.20 Vehicle positions");
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
		const std::string VehiclePositionTableSync::COL_RANK_PASSENGERS;
		const std::string VehiclePositionTableSync::COL_GEOMETRY;
	}
	
	namespace db
	{
		template<> const SQLiteTableSync::Format SQLiteTableSyncTemplate<VehiclePositionTableSync>::TABLE(
			"t072_vehicle_positions"
		);



		template<> const SQLiteTableSync::Field SQLiteTableSyncTemplate<VehiclePositionTableSync>::_FIELDS[]=
		{
			SQLiteTableSync::Field(TABLE_COL_ID, SQL_INTEGER, false),
			SQLiteTableSync::Field(VehiclePositionTableSync::COL_STATUS, SQL_INTEGER),
			SQLiteTableSync::Field(VehiclePositionTableSync::COL_VEHICLE_ID, SQL_INTEGER),
			SQLiteTableSync::Field(VehiclePositionTableSync::COL_TIME, SQL_TEXT),
			SQLiteTableSync::Field(VehiclePositionTableSync::COL_METER_OFFSET, SQL_DOUBLE),
			SQLiteTableSync::Field(VehiclePositionTableSync::COL_STOP_POINT_ID, SQL_INTEGER),
			SQLiteTableSync::Field(VehiclePositionTableSync::COL_COMMENT, SQL_TEXT),
			SQLiteTableSync::Field(VehiclePositionTableSync::COL_SERVICE_ID, SQL_INTEGER),
			SQLiteTableSync::Field(VehiclePositionTableSync::COL_RANK_IN_PATH, SQL_INTEGER),
			static const std::string COL_RANK_IN_PATH;
			static const std::string COL_RANK_PASSENGERS;
			static const std::string COL_GEOMETRY;
			SQLiteTableSync::Field()
		};



		template<> const SQLiteTableSync::Index SQLiteTableSyncTemplate<VehiclePositionTableSync>::_INDEXES[]=
		{
			// SQLiteTableSync::Index(
			//	VehiclePositionTableSync::COL_NAME.c_str(),
			// ""),
			SQLiteTableSync::Index()
		};



		template<> void SQLiteDirectTableSyncTemplate<VehiclePositionTableSync,VehiclePosition>::Load(
			VehiclePosition* object,
			const db::SQLiteResultSPtr& rows,
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



		template<> void SQLiteDirectTableSyncTemplate<VehiclePositionTableSync,VehiclePosition>::Save(
			VehiclePosition* object,
			optional<SQLiteTransaction&> transaction
		){
			ReplaceQuery<VehiclePositionTableSync> query(*object);
			// query.addField(object->getName());
			// query.addField(object->_getParent() ? object->_getParent()->getKey() : RegistryKeyType(0));
			query.execute(transaction);
		}



		template<> void SQLiteDirectTableSyncTemplate<VehiclePositionTableSync,VehiclePosition>::Unlink(
			VehiclePosition* obj
		){
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
