
//////////////////////////////////////////////////////////////////////////
///	CompositionTableSync class implementation.
///	@file CompositionTableSync.cpp
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

#include "CompositionTableSync.hpp"
#include "ReplaceQuery.h"
#include "SelectQuery.hpp"
#include "ScheduledServiceTableSync.h"
#include "VehicleTableSync.hpp"
#include "StopPointTableSync.hpp"
#include "ScheduledServiceTableSync.h"
#include "PTOperationModule.hpp"

#include <boost/algorithm/string/split.hpp>
#include <boost/foreach.hpp>

using namespace std;
using namespace boost;
using namespace boost::algorithm;


namespace synthese
{
	using namespace db;
	using namespace util;
	using namespace pt_operation;
	using namespace calendar;
	using namespace graph;
	using namespace pt;
	
		
	namespace util
	{
		template<> const string FactorableTemplate<DBTableSync,CompositionTableSync>::FACTORY_KEY("37.11 Compositions");
	}

	namespace pt_operation
	{
		const string CompositionTableSync::COL_SERVICE_ID("service_id");
		const string CompositionTableSync::COL_VEHICLES("vehicles");
		const string CompositionTableSync::COL_DATES("dates");
		const string CompositionTableSync::COL_SERVED_VERTICES("served_vertices");
	}
	
	namespace db
	{
		template<> const DBTableSync::Format DBTableSyncTemplate<CompositionTableSync>::TABLE(
			"t070_compositions"
		);



		template<> const DBTableSync::Field DBTableSyncTemplate<CompositionTableSync>::_FIELDS[]=
		{
			DBTableSync::Field(TABLE_COL_ID, SQL_INTEGER),
			DBTableSync::Field(CompositionTableSync::COL_SERVICE_ID, SQL_INTEGER),
			DBTableSync::Field(CompositionTableSync::COL_VEHICLES, SQL_TEXT),
			DBTableSync::Field(CompositionTableSync::COL_DATES, SQL_TEXT),
			DBTableSync::Field(CompositionTableSync::COL_SERVED_VERTICES, SQL_TEXT),
			DBTableSync::Field()
		};



		template<> const DBTableSync::Index DBTableSyncTemplate<CompositionTableSync>::_INDEXES[]=
		{
			DBTableSync::Index(
				CompositionTableSync::COL_SERVICE_ID.c_str(),
				""
			),
			DBTableSync::Index()
		};



		template<> void DBDirectTableSyncTemplate<CompositionTableSync,Composition>::Load(
			Composition* object,
			const db::DBResultSPtr& rows,
			Env& env,
			LinkLevel linkLevel
		){
			// Dates
			Calendar calendarValue;
			calendarValue.setFromSerializedString(rows->getText(CompositionTableSync::COL_DATES));
			object->setCalendar(calendarValue);

			if(linkLevel >= UP_LINKS_LOAD_LEVEL)
			{
				// Service
				RegistryKeyType serviceId(rows->getLongLong(CompositionTableSync::COL_SERVICE_ID));
				if(serviceId > 0)
				{
					try
					{
						object->setService(ScheduledServiceTableSync::GetEditable(serviceId, env, linkLevel).get());
					}
					catch(ObjectNotFoundException<ScheduledService>& e)
					{
						Log::GetInstance().warn("No such service "+ lexical_cast<string>(serviceId) +" in Composition "+ lexical_cast<string>(object->getKey()));
					}
					PTOperationModule::RegisterComposition(*object);
				}

				// Vehicles
				vector<string> vehicleLinksTexts;
				Composition::VehicleLinks vehicleLinks;
				string text(rows->getText(CompositionTableSync::COL_VEHICLES));
				if(!text.empty())
				{
					split(vehicleLinksTexts, text, is_any_of(","));
					BOOST_FOREACH(const string& vehicleLink, vehicleLinksTexts)
					{
						vector<string> values;
						split(values, vehicleLink, is_any_of("|"));

						Composition::VehicleLinks::value_type value;
						try
						{
							value.vehicle = VehicleTableSync::GetEditable(lexical_cast<RegistryKeyType>(values[0]), env, linkLevel).get();
							value.number = values[1];
							vehicleLinks.push_back(value);
						}
						catch(ObjectNotFoundException<Vehicle>& e)
						{
							Log::GetInstance().warn("No such vehicle "+ values[0] +" in Composition "+ lexical_cast<string>(object->getKey()));
						}
					}
				}
				object->setVehicles(vehicleLinks);

				// Vertices
				Service::ServedVertices vertices;
				if(rows->getText(CompositionTableSync::COL_SERVED_VERTICES).empty())
				{
					BOOST_FOREACH(const Path::Edges::value_type& edge, object->getService()->getPath()->getEdges())
					{
						vertices.push_back(edge->getFromVertex());
					}
				}
				else
				{
					vector<string> verticesString;
					string text(rows->getText(CompositionTableSync::COL_SERVED_VERTICES));
					if(!text.empty())
					{
						split(verticesString, text, is_any_of(","));
						BOOST_FOREACH(const string& vertexString, verticesString)
						{
							vertices.push_back(
								StopPointTableSync::GetEditable(lexical_cast<RegistryKeyType>(vertexString), env, linkLevel).get()
							);
						}
					}
				}
				object->setServedVertices(vertices);
			}
		}



		template<> void DBDirectTableSyncTemplate<CompositionTableSync,Composition>::Save(
			Composition* object,
			optional<DBTransaction&> transaction
		){
			// Vehicles
			stringstream vehiclesString;
			bool firstVehicle(true);
			BOOST_FOREACH(const Composition::VehicleLinks::value_type& vehicle, object->getVehicles())
			{
				if(firstVehicle)
				{
					firstVehicle = false;
				}
				else
				{
					vehiclesString << ",";
				}
				vehiclesString << vehicle.vehicle->getKey() << "|" << vehicle.number;
			}

			// Dates
			stringstream datesString;
			object->getCalendar().serialize(datesString);

			// Vertices
			stringstream verticesString;
			bool firstVertex(true);
			BOOST_FOREACH(const Service::ServedVertices::value_type& vertex, object->getServedVertices())
			{
				if(firstVertex)
				{
					firstVertex = false;
				}
				else
				{
					verticesString << ",";
				}
				verticesString << vertex->getKey();
			}


			ReplaceQuery<CompositionTableSync> query(*object);
			query.addField(object->getService() ? object->getService()->getKey() : RegistryKeyType(0));
			query.addField(vehiclesString.str());
			query.addField(datesString.str());
			query.addField(verticesString.str());
			query.execute(transaction);
		}



		template<> void DBDirectTableSyncTemplate<CompositionTableSync,Composition>::Unlink(
			Composition* obj
		){
			PTOperationModule::UnregisterComposition(*obj);
			obj->setService(NULL);
			obj->setVehicles(Composition::VehicleLinks());
			obj->setServedVertices(Service::ServedVertices());
		}
	}
	
	
	
	namespace pt_operation
	{
		CompositionTableSync::SearchResult CompositionTableSync::Search(
			util::Env& env,
			// boost::optional<util::RegistryKeyType> parameterId /*= boost::optional<util::RegistryKeyType>()*/,
			size_t first /*= 0*/,
			optional<size_t> number /*= boost::optional<std::size_t>()*/,
			bool orderByName,
			bool raisingOrder,
			util::LinkLevel linkLevel /*= util::FIELDS_ONLY_LOAD_LEVEL */
		){
			SelectQuery<CompositionTableSync> query;
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
