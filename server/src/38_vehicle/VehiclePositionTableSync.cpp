
//////////////////////////////////////////////////////////////////////////
///	VehiclePositionTableSync class implementation.
///	@file VehiclePositionTableSync.cpp
///	@author Hugues Romain
///	@date 2011
///
///	This file belongs to the SYNTHESE project (public transportation specialized software)
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
///	along with this program; if not, write to the Free Software
///	Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.

#include "VehiclePositionTableSync.hpp"

#include "DepotTableSync.hpp"
#include "Profile.h"
#include "PtimeField.hpp"
#include "PTUseRule.h"
#include "ReplaceQuery.h"
#include "ScheduledServiceTableSync.h"
#include "SelectQuery.hpp"
#include "StopArea.hpp"
#include "StopPointTableSync.hpp"
#include "UpdateQuery.hpp"
#include "User.h"
#include "VehicleTableSync.hpp"


#include <geos/geom/Point.h>
#include <sstream>

using namespace std;
using namespace boost;
using namespace geos::geom;

namespace synthese
{
	using namespace db;
	using namespace util;
	using namespace vehicle;
	using namespace pt;
	using namespace pt_operation;

	namespace util
	{
		template<> const string FactorableTemplate<DBTableSync,VehiclePositionTableSync>::FACTORY_KEY("37.20 Vehicle positions");
	}

	namespace db
	{
		template<> const DBTableSync::Format DBTableSyncTemplate<VehiclePositionTableSync>::TABLE(
			"t072_vehicle_positions"
		);



		template<> const Field DBTableSyncTemplate<VehiclePositionTableSync>::_FIELDS[]=
		{
			Field()
		};



		template<>
		DBTableSync::Indexes DBTableSyncTemplate<VehiclePositionTableSync>::GetIndexes()
		{
			DBTableSync::Indexes r;
			r.push_back(
				DBTableSync::Index(
					VehiclePositionVehicle::FIELD.name.c_str(),
					Time::FIELD.name.c_str(),
			"")	);
			return r;
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



		template<> void DBTableSyncTemplate<VehiclePositionTableSync>::LogRemoval(
			const server::Session* session,
			util::RegistryKeyType id
		){
			//TODO Log the removal
		}
	}



	namespace vehicle
	{
		VehiclePositionTableSync::SearchResult VehiclePositionTableSync::Search(
			util::Env& env,
			boost::optional<util::RegistryKeyType> vehicleId,
			boost::optional<boost::posix_time::ptime> startDate,
			boost::optional<boost::posix_time::ptime> endDate,
			size_t first /*= 0*/,
			optional<size_t> number /*= boost::optional<std::size_t>()*/,
			bool orderByDate,
			bool raisingOrder,
			util::LinkLevel linkLevel
		){
			SelectQuery<VehiclePositionTableSync> query;
			if(startDate)
			{
				query.addWhereField(Time::FIELD.name, *startDate, ComposedExpression::OP_SUPEQ);
			}
			if(endDate)
			{
				query.addWhereField(Time::FIELD.name, *endDate, ComposedExpression::OP_INFEQ);
			}
			if(vehicleId)
			{
				query.addWhereField(VehiclePositionVehicle::FIELD.name, *vehicleId);
			}
			if(orderByDate)
			{
				query.addOrderField(Time::FIELD.name, raisingOrder);
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



		void VehiclePositionTableSync::ChangePassengers(
			const VehiclePosition& startPosition,
			const VehiclePosition& endPosition,
			std::size_t passengersToAdd,
			std::size_t passengersToRemove,
			boost::optional<db::DBTransaction&> transaction
		){
			UpdateQuery<VehiclePositionTableSync> query;
			query.addUpdateField(
				Passengers::FIELD.name,
				RawSQL(
					Passengers::FIELD.name + "+" + lexical_cast<string>(passengersToAdd) + "-" + lexical_cast<string>(passengersToRemove)
			)	);
			query.addWhereField(VehiclePositionVehicle::FIELD.name, startPosition.getVehicle()->getKey());
			query.addWhereField(Time::FIELD.name, startPosition.getTime(), ComposedExpression::OP_SUPEQ);
			query.addWhereField(Time::FIELD.name, endPosition.getTime(), ComposedExpression::OP_INF);
			query.execute(transaction);
		}

		bool VehiclePositionTableSync::allowList(const server::Session* session) const
		{
			return session && session->hasProfile() && session->getUser()->getProfile()->isAuthorized<security::GlobalRight>(security::READ);
		}
	}
}
