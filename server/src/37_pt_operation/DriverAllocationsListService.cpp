
/** DriverAllocationsListService class implementation.
	@file DriverAllocationsListService.cpp
	@author Hugues Romain
	@date 2011

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

#include "DriverAllocationsListService.hpp"

#include "CommercialLine.h"
#include "DataSourceTableSync.h"
#include "DriverAllocationTableSync.hpp"
#include "DriverService.hpp"
#include "ImportableTableSync.hpp"
#include "JourneyPattern.hpp"
#include "MimeTypes.hpp"
#include "Profile.h"
#include "RequestException.h"
#include "Request.h"
#include "ScheduledService.h"
#include "User.h"
#include "UserTableSync.h"
#include "Webpage.h"

#include <boost/foreach.hpp>

using namespace std;
using namespace boost;
using namespace algorithm;
using namespace boost::gregorian;
using namespace boost::posix_time;

namespace synthese
{
	using namespace impex;
	using namespace util;
	using namespace server;
	using namespace security;
	using namespace cms;
	using namespace pt;
	using namespace graph;
	using namespace pt_operation;

	template<>
	const string FactorableTemplate<FunctionWithSite<false>, DriverAllocationsListService>::FACTORY_KEY = "driver_allocations";

	namespace pt_operation
	{
		const string DriverAllocationsListService::PARAMETER_DRIVER_ID = "driver_id";
		const string DriverAllocationsListService::PARAMETER_DATA_SOURCE_ID = "data_source_id";
		const string DriverAllocationsListService::PARAMETER_MIN_DATE = "min_date";
		const string DriverAllocationsListService::PARAMETER_PAGE_ID = "p";

		const string DriverAllocationsListService::TAG_ALLOCATION = "allocation";
		const string DriverAllocationsListService::TAG_ALLOCATIONS = "allocations";



		DriverAllocationsListService::DriverAllocationsListService():
			_dataSource(NULL),
			_driver(NULL),
			_minDate(not_a_date_time),
			_date(not_a_date_time),
			_page(NULL)
		{}



		ParametersMap DriverAllocationsListService::_getParametersMap() const
		{
			ParametersMap map;
			if(_page)
			{
				map.insert(PARAMETER_PAGE_ID, _page->getKey());
			}
			else if(!_outputFormat.empty())
			{
				map.insert(PARAMETER_OUTPUT_FORMAT, _outputFormat);
			}
			if(_driver)
			{
				map.insert(PARAMETER_DRIVER_ID, _driver->getKey());
			}
			return map;
		}



		void DriverAllocationsListService::_setFromParametersMap(const ParametersMap& map)
		{
			// Min Date
			if(map.getDefault<string>(PARAMETER_MIN_DATE).empty())
			{
				_minDate = day_clock::local_day();
			}
			else
			{
				_minDate = from_simple_string(map.get<string>(PARAMETER_MIN_DATE));
			}

			// Date
			if(!map.getDefault<string>(Date::FIELD.name).empty())
			{
				_date = from_string(map.get<string>(Date::FIELD.name));
			}

			// Display page
			_page = getPage(map.getDefault<string>(PARAMETER_PAGE_ID));

			// Mime type
			if(!_page)
			{
				setOutputFormatFromMap(map, string());
			}

			// Driver
			string driverStr(map.getDefault<string>(PARAMETER_DRIVER_ID));
			if(!driverStr.empty())
			{
				// Data source + code
				try
				{
					RegistryKeyType id(map.getDefault<RegistryKeyType>(PARAMETER_DATA_SOURCE_ID, 0));
					if(id > 0)
					{
						_dataSource = DataSourceTableSync::Get(id, *_env).get();
					}
				}
				catch (ObjectNotFoundException<DataSource>&)
				{
					throw RequestException("No such data source");
				}

				// Driver id
				try
				{
					if(_dataSource)
					{
						string code(map.getDefault<string>(PARAMETER_DRIVER_ID));
						ImportableTableSync::ObjectBySource<UserTableSync> users(*_dataSource, *_env);
						ImportableTableSync::ObjectBySource<UserTableSync>::Set obj(users.get(code));
						if(obj.empty())
						{
							throw RequestException("No such driver");
						}
						_driver = *obj.begin();
					}
					else
					{
						RegistryKeyType id(map.getDefault<RegistryKeyType>(PARAMETER_DRIVER_ID, 0));
						if(id > 0)
						{
							_driver = UserTableSync::Get(id, *_env).get();
						}
						else
						{
							throw RequestException("A driver must be specified");
						}
					}
				}
				catch(ObjectNotFoundException<User>&)
				{
					throw RequestException("No such driver");
				}
			}
		}



		util::ParametersMap DriverAllocationsListService::run(
			std::ostream& stream,
			const Request& request
		) const {

			ParametersMap map;

			DriverAllocationTableSync::SearchResult allocations(
				DriverAllocationTableSync::Search(
					Env::GetOfficialEnv(),
					_date,
					_minDate,
					_driver ? optional<RegistryKeyType>(_driver->getKey()) : optional<RegistryKeyType>()
			)	);
			BOOST_FOREACH(const boost::shared_ptr<DriverAllocation>& it, allocations)
			{
				// Init
				const DriverAllocation& alloc(*it);

				//////////////////////////////////////////////////////////////////////////
				// Output preparation

				boost::shared_ptr<ParametersMap> allocPM(new ParametersMap);
				alloc.toParametersMap(*allocPM);

				if(alloc.get<DriverActivity>())
				{
					boost::shared_ptr<ParametersMap> activityPM(new ParametersMap);
					alloc.get<DriverActivity>()->toParametersMap(*activityPM);
					allocPM->insert("activity", activityPM);
				}
				if(alloc.get<DriverAllocationTemplate>())
				{
					// Adds template attributes to the map
					alloc.get<DriverAllocationTemplate>()->toParametersMap(*allocPM);

					// Adds services to the map
					BOOST_FOREACH(
						const DriverService::Vector::Type::value_type& service,
						alloc.get<DriverAllocationTemplate>()->get<DriverService::Vector>()
					){
						boost::shared_ptr<ParametersMap> servicePM(new ParametersMap);
						service->toParametersMap(*servicePM, true);
						allocPM->insert("service", servicePM);
					}
				}

				map.insert(TAG_ALLOCATION, allocPM);
			}

			if(_page)
			{
				if(map.hasSubMaps(TAG_ALLOCATION))
				{
					BOOST_FOREACH(const boost::shared_ptr<ParametersMap>& allocPM, map.getSubMaps(TAG_ALLOCATION))
					{
						allocPM->merge(getTemplateParameters());
						_page->display(stream, request, *allocPM);
			}	}	}
			else if(_outputFormat == MimeTypes::XML)
			{
				map.outputXML(stream, TAG_ALLOCATIONS, true);
			}
			else if(_outputFormat == MimeTypes::JSON)
			{
				map.outputJSON(stream, TAG_ALLOCATIONS);
			}

			return map;
		}



		bool DriverAllocationsListService::isAuthorized(
			const Session* session
		) const {
			return true;
		}



		std::string DriverAllocationsListService::getOutputMimeType() const
		{
			return _page ? _page->getMimeType() : getOutputMimeTypeFromOutputFormat();
		}
}	}
