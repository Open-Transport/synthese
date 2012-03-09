
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

#include "DataSourceTableSync.h"
#include "DriverAllocation.hpp"
#include "DriverService.hpp"
#include "ImportableTableSync.hpp"
#include "RequestException.h"
#include "Request.h"
#include "SchedulesBasedService.h"
#include "UserTableSync.h"
#include "Webpage.h"

#include <boost/foreach.hpp>

using namespace std;
using namespace boost;
using namespace boost::gregorian;

namespace synthese
{
	using namespace impex;
	using namespace util;
	using namespace server;
	using namespace security;
	using namespace cms;
	using namespace pt;
	using namespace graph;


	template<> const string util::FactorableTemplate<Function,pt_operation::DriverAllocationsListService>::FACTORY_KEY("DriverAllocationsList");

	namespace pt_operation
	{
		const string DriverAllocationsListService::PARAMETER_DRIVER_ID = "driver_id";
		const string DriverAllocationsListService::PARAMETER_DATA_SOURCE_ID = "data_source_id";
		const string DriverAllocationsListService::PARAMETER_MIN_DATE = "min_date";
		const string DriverAllocationsListService::PARAMETER_PAGE_ID = "pa";

		const string DriverAllocationsListService::TAG_ALLOCATION = "allocation";

		ParametersMap DriverAllocationsListService::_getParametersMap() const
		{
			ParametersMap map;
			if(_page.get())
			{
				map.insert(PARAMETER_PAGE_ID, _page->getKey());
			}
			if(_driver.get())
			{
				map.insert(PARAMETER_DRIVER_ID, _driver->getKey());
			}
			return map;
		}

		void DriverAllocationsListService::_setFromParametersMap(const ParametersMap& map)
		{
			// Date
			if(map.getDefault<string>(PARAMETER_MIN_DATE).empty())
			{
				_minDate = day_clock::local_day();
			}
			else
			{
				_minDate = from_simple_string(map.get<string>(PARAMETER_MIN_DATE));
			}

			// Display page
			try
			{
				RegistryKeyType id(map.getDefault<RegistryKeyType>(PARAMETER_PAGE_ID, 0));
				if(id > 0)
				{
					_page = Env::GetOfficialEnv().get<Webpage>(id);
				}
			}
			catch(ObjectNotFoundException<Webpage>&)
			{
				throw RequestException("No such composition page");
			}

			// Data source
			try
			{
				RegistryKeyType id(map.getDefault<RegistryKeyType>(PARAMETER_DATA_SOURCE_ID, 0));
				if(id > 0)
				{
					_dataSource = DataSourceTableSync::Get(id, *_env);
				}
			}
			catch (ObjectNotFoundException<DataSource>&)
			{
				throw RequestException("No such data source");
			}

			// Driver id
			try
			{
				if(_dataSource.get())
				{
					string code(map.getDefault<string>(PARAMETER_DRIVER_ID));
					ImportableTableSync::ObjectBySource<UserTableSync> users(*_dataSource, *_env);
					ImportableTableSync::ObjectBySource<UserTableSync>::Set obj(users.get(code));
					if(obj.empty())
					{
						throw RequestException("No such driver");
					}
					_driver = _env->getSPtr<User>(*obj.begin());
				}
				else
				{
					RegistryKeyType id(map.getDefault<RegistryKeyType>(PARAMETER_DRIVER_ID, 0));
					if(id > 0)
					{
						_driver = UserTableSync::Get(id, *_env);
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



		util::ParametersMap DriverAllocationsListService::run(
			std::ostream& stream,
			const Request& request
		) const {

			ParametersMap map;
			size_t rank(0);
			BOOST_FOREACH(const DriverAllocation::Registry::value_type& alloc, Env::GetOfficialEnv().getRegistry<DriverAllocation>())
			{
				if(!_minDate.is_not_a_date() && alloc.second->get<Date>() < _minDate)
				{
					continue;
				}

				shared_ptr<ParametersMap> allocPM(new ParametersMap);
				allocPM->insert("id", alloc.second->getKey());
			
				BOOST_FOREACH(const DriverService::Vector::Type::value_type& service, alloc.second->get<DriverService::Vector>())
				{
					shared_ptr<ParametersMap> servicePM(new ParametersMap);
					service->toParametersMap(*servicePM);
					allocPM->insert("service", servicePM);
				}

				map.insert(TAG_ALLOCATION, allocPM);
			}

			if(_page.get())
			{
				BOOST_FOREACH(const shared_ptr<ParametersMap>& allocPM, map.getSubMaps(TAG_ALLOCATION))
				_page->display(stream, request, *allocPM);
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
			return _page.get() ? _page->getMimeType() : "text/html";
		}
}	}
