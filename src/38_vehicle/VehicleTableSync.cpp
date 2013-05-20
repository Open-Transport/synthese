
//////////////////////////////////////////////////////////////////////////
///	VehicleTableSync class implementation.
///	@file VehicleTableSync.cpp
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

#include "VehicleTableSync.hpp"

#include "CommercialLine.h"
#include "CommercialLineTableSync.h"
#include "DataSourceLinksField.hpp"
#include "ImportableTableSync.hpp"
#include "ReplaceQuery.h"
#include "SelectQuery.hpp"
#include "Vehicle.hpp"
#include "VehicleModule.hpp"

#include <sstream>
#include <boost/foreach.hpp>
#include <boost/algorithm/string/split.hpp>

using namespace std;
using namespace boost;
using namespace boost::algorithm;

namespace synthese
{
	using namespace db;
	using namespace impex;
	using namespace pt;
	using namespace util;
	using namespace vehicle;

	namespace util
	{
		template<> const string FactorableTemplate<DBTableSync,VehicleTableSync>::FACTORY_KEY("37.01 Vehicles");
	}

	namespace vehicle
	{
		const string VehicleTableSync::COL_NAME("name");
		const string VehicleTableSync::COL_NUMBER("number");
		const string VehicleTableSync::COL_ALLOWED_LINES("allowed_lines");
		const string VehicleTableSync::COL_SEATS("seats");
		const string VehicleTableSync::COL_PICTURE("picture");
		const string VehicleTableSync::COL_AVAILABLE("available");
		const string VehicleTableSync::COL_URL("url");
		const string VehicleTableSync::COL_REGISTRATION_NUMBERS("registration_numbers");
		const string VehicleTableSync::COL_DATA_SOURCE_LINKS = "datasource_links";
	}

	namespace db
	{
		template<> const DBTableSync::Format DBTableSyncTemplate<VehicleTableSync>::TABLE(
			"t069_vehicles"
		);



		template<> const Field DBTableSyncTemplate<VehicleTableSync>::_FIELDS[]=
		{
			Field(TABLE_COL_ID, SQL_INTEGER),
			Field(VehicleTableSync::COL_NAME, SQL_TEXT),
			Field(VehicleTableSync::COL_NUMBER, SQL_TEXT),
			Field(VehicleTableSync::COL_ALLOWED_LINES, SQL_TEXT),
			Field(VehicleTableSync::COL_SEATS, SQL_TEXT),
			Field(VehicleTableSync::COL_PICTURE, SQL_TEXT),
			Field(VehicleTableSync::COL_AVAILABLE, SQL_BOOLEAN),
			Field(VehicleTableSync::COL_URL, SQL_TEXT),
			Field(VehicleTableSync::COL_REGISTRATION_NUMBERS, SQL_TEXT),
			Field(VehicleTableSync::COL_DATA_SOURCE_LINKS, SQL_TEXT),
			Field()
		};



		template<>
		DBTableSync::Indexes DBTableSyncTemplate<VehicleTableSync>::GetIndexes()
		{
			return DBTableSync::Indexes();
		}



		template<> void OldLoadSavePolicy<VehicleTableSync,Vehicle>::Load(
			Vehicle* object,
			const db::DBResultSPtr& rows,
			Env& env,
			LinkLevel linkLevel
		){
			object->setName(rows->getText(VehicleTableSync::COL_NAME));
			object->setNumber(rows->getText(VehicleTableSync::COL_NUMBER));
			object->setPicture(rows->getText(VehicleTableSync::COL_PICTURE));
			object->setAvailable(rows->getBool(VehicleTableSync::COL_AVAILABLE));
			object->setURL(rows->getText(VehicleTableSync::COL_URL));
			object->setRegistrationNumbers(rows->getText(VehicleTableSync::COL_REGISTRATION_NUMBERS));

			// Seats
			object->setSeats(VehicleTableSync::UnserializeSeats(rows->getText(VehicleTableSync::COL_SEATS)));

			if(linkLevel >= UP_LINKS_LOAD_LEVEL)
			{
				object->setAllowedLines(VehicleTableSync::UnserializeAllowedLines(rows->getText(VehicleTableSync::COL_ALLOWED_LINES), env, linkLevel));
				VehicleModule::RegisterVehicle(*object);
			}

			// Data source links (at the end of the load to avoid registration of objects which are removed later by an exception)
			if (linkLevel > FIELDS_ONLY_LOAD_LEVEL)
			{
				if(&env == &Env::GetOfficialEnv())
				{
					object->setDataSourceLinksWithRegistration(
						ImportableTableSync::GetDataSourceLinksFromSerializedString(
							rows->getText(VehicleTableSync::COL_DATA_SOURCE_LINKS),
							env
					)	);
				}
				else
				{
					object->setDataSourceLinksWithoutRegistration(
						ImportableTableSync::GetDataSourceLinksFromSerializedString(
							rows->getText(VehicleTableSync::COL_DATA_SOURCE_LINKS),
							env
					)	);
				}
			}
		}



		template<> void OldLoadSavePolicy<VehicleTableSync,Vehicle>::Save(
			Vehicle* object,
			optional<DBTransaction&> transaction
		){
			ReplaceQuery<VehicleTableSync> query(*object);
			query.addField(object->getName());
			query.addField(object->getNumber());
			query.addField(VehicleTableSync::SerializeAllowedLines(object->getAllowedLines()));
			query.addField(VehicleTableSync::SerializeSeats(object->getSeats()));
			query.addField(object->getPicture());
			query.addField(object->getAvailable());
			query.addField(object->getURL());
			query.addField(object->getRegistrationNumbers());
			query.addField(
				DataSourceLinks::Serialize(
					object->getDataSourceLinks()
			)	);
			query.execute(transaction);
		}



		template<> void OldLoadSavePolicy<VehicleTableSync,Vehicle>::Unlink(
			Vehicle* obj
		){
			VehicleModule::UnregisterVehicle(*obj);
			obj->setAllowedLines(Vehicle::AllowedLines());

			if(Env::GetOfficialEnv().contains(*obj))
			{
				obj->cleanDataSourceLinks(true);
			}
		}



		template<> bool DBTableSyncTemplate<VehicleTableSync>::CanDelete(
			const server::Session* session,
			util::RegistryKeyType object_id
		){
			//TODO Check tue user rights
			return true;
		}



		template<> void DBTableSyncTemplate<VehicleTableSync>::BeforeDelete(
			util::RegistryKeyType id,
			db::DBTransaction& transaction
		){
		}



		template<> void DBTableSyncTemplate<VehicleTableSync>::AfterDelete(
			util::RegistryKeyType id,
			db::DBTransaction& transaction
		){
		}



		template<> void DBTableSyncTemplate<VehicleTableSync>::LogRemoval(
			const server::Session* session,
			util::RegistryKeyType id
		){
			//TODO Log the removal
		}
	}



	namespace vehicle
	{
		VehicleTableSync::SearchResult VehicleTableSync::Search(
			util::Env& env,
			boost::optional<std::string> name,
			boost::optional<std::string> vehicleNumber,
			boost::optional<std::string> registration,
			size_t first /*= 0*/,
			optional<size_t> number /*= boost::optional<std::size_t>()*/,
			bool orderByName,
			bool orderByNumber,
			bool orderByRegistration,
			bool raisingOrder,
			util::LinkLevel linkLevel
		){
			SelectQuery<VehicleTableSync> query;
			if(name)
			{
				query.addWhereField(COL_NAME, "%"+ *name +"%", ComposedExpression::OP_LIKE);
			}
			if(vehicleNumber)
			{
				query.addWhereField(COL_NUMBER, "%"+ *vehicleNumber +"%", ComposedExpression::OP_LIKE);
			}
			if(registration)
			{
				query.addWhereField(COL_REGISTRATION_NUMBERS, "%"+ *registration +"%", ComposedExpression::OP_LIKE);
			}
			if(orderByName)
			{
				query.addOrderField(COL_NAME, raisingOrder);
			}
			else if(orderByNumber)
			{
				query.addOrderField(COL_NUMBER, raisingOrder);
			}
			else if(orderByRegistration)
			{
				query.addOrderField(COL_REGISTRATION_NUMBERS, raisingOrder);
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



		std::string VehicleTableSync::SerializeAllowedLines( const Vehicle::AllowedLines& lines )
		{
			stringstream allowedLinesStr;
			bool firstAllowedLine(true);
			BOOST_FOREACH(const Vehicle::AllowedLines::value_type& line, lines)
			{
				if(firstAllowedLine)
				{
					firstAllowedLine = false;
				}
				else
				{
					allowedLinesStr << ",";
				}
				allowedLinesStr << line->getKey();
			}
			return allowedLinesStr.str();
		}



		std::string VehicleTableSync::SerializeSeats( const Vehicle::Seats& seats )
		{
			stringstream seatsStr;
			bool firstSeat(true);
			BOOST_FOREACH(const Vehicle::Seats::value_type& seat, seats)
			{
				if(firstSeat)
				{
					firstSeat = false;
				}
				else
				{
					seatsStr << ",";
				}
				seatsStr << seat;
			}
			return seatsStr.str();
		}



		Vehicle::AllowedLines VehicleTableSync::UnserializeAllowedLines(
			const std::string& value,
			Env& env,
			LinkLevel linkLevel
		){
			vector<string> allowedLinesStrs;
			if(!value.empty())
			{
				split(allowedLinesStrs, value, is_any_of(","));
			}

			Vehicle::AllowedLines allowedLinesValue;

			BOOST_FOREACH(const string& lineId, allowedLinesStrs)
			{
				try
				{
					allowedLinesValue.insert(
						CommercialLineTableSync::Get(lexical_cast<RegistryKeyType>(lineId), env, linkLevel).get()
					);
				}
				catch(ObjectNotFoundException<CommercialLine>&)
				{
					Log::GetInstance().warn("No such line "+ lexical_cast<string>(lineId));
				}
			}

			return allowedLinesValue;
		}



		Vehicle::Seats VehicleTableSync::UnserializeSeats( const std::string& value )
		{
			Vehicle::Seats seatsValue;
			if(!value.empty())
			{
				split(seatsValue, value, is_any_of(","));
			}
			return seatsValue;
		}



		VehicleTableSync::VehiclesList VehicleTableSync::GetVehiclesList(
			util::Env& env,
			optional<string> noVehicleLabel
		){
			VehiclesList result;
			if(noVehicleLabel)
			{
				result.push_back(make_pair(optional<RegistryKeyType>(0), *noVehicleLabel));
			}
			SearchResult vehicles(Search(env));
			BOOST_FOREACH(const boost::shared_ptr<Vehicle>& vehicle, vehicles)
			{
				result.push_back(make_pair(optional<RegistryKeyType>(vehicle->getKey()), vehicle->getName()));
			}
			return result;
		}
}	}
