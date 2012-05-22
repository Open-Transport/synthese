
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
#include "DriverAllocation.hpp"
#include "DriverService.hpp"
#include "ImportableTableSync.hpp"
#include "JourneyPattern.hpp"
#include "MimeTypes.hpp"
#include "RequestException.h"
#include "Request.h"
#include "ScheduledService.h"
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
	const string FactorableTemplate<FunctionWithSite<false>, DriverAllocationsListService>::FACTORY_KEY = "DriverAllocationsList";

	namespace pt_operation
	{
		const string DriverAllocationsListService::PARAMETER_DRIVER_ID = "driver_id";
		const string DriverAllocationsListService::PARAMETER_DATA_SOURCE_ID = "data_source_id";
		const string DriverAllocationsListService::PARAMETER_MIN_DATE = "min_date";
		const string DriverAllocationsListService::PARAMETER_PAGE_ID = "p";
		const string DriverAllocationsListService::PARAMETER_WORK_DURATION_FILTER = "work_duration_filter";
		const string DriverAllocationsListService::PARAMETER_WORK_RANGE_FILTER = "work_range_filter";
		const string DriverAllocationsListService::PARAMETER_LINE_FILTER = "line_filter";
		const string DriverAllocationsListService::PARAMETER_HOURS_FILTER = "hours_filter";
		const string DriverAllocationsListService::PARAMETER_WITH_TICKET_SALES_FILTER = "with_ticket_sales_filter";

		const string DriverAllocationsListService::TAG_ALLOCATION = "allocation";
		const string DriverAllocationsListService::TAG_ALLOCATIONS = "allocations";



		DriverAllocationsListService::DriverAllocationsListService():
			_dataSource(NULL),
			_driver(NULL),
			_minDate(not_a_date_time),
			_date(not_a_date_time),
			_page(NULL),
			_minWorkDuration(not_a_date_time),
			_maxWorkDuration(not_a_date_time),
			_minWorkRange(not_a_date_time),
			_maxWorkRange(not_a_date_time),
			_lineFilter(NULL),
			_minHourFilter(not_a_date_time),
			_maxHourFilter(not_a_date_time),
			_withTicketSalesFilter(logic::indeterminate)
		{}



		ParametersMap DriverAllocationsListService::_getParametersMap() const
		{
			ParametersMap map;
			if(_page)
			{
				map.insert(PARAMETER_PAGE_ID, _page->getKey());
			}
			else if(!_mimeType.empty())
			{
				MimeType::SaveToParametersMap(_mimeType, map);
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
				Date::LoadFromRecord(_date, map);
			}

			// Hours filter
			string hoursFilter(map.getDefault<string>(PARAMETER_HOURS_FILTER));
			if(!hoursFilter.empty())
			{
				vector<string> bounds;
				split(bounds, hoursFilter, is_any_of(","));
				if(bounds.size() >= 1)
				{
					_minHourFilter = hours(lexical_cast<long>(bounds[0]));
				}
				if(bounds.size() >= 2)
				{
					_maxHourFilter = hours(lexical_cast<long>(bounds[1]));
				}
			}

			// Work range filter
			string workRangeFilter(map.getDefault<string>(PARAMETER_WORK_RANGE_FILTER));
			if(!workRangeFilter.empty())
			{
				vector<string> bounds;

				split(bounds, workRangeFilter, is_any_of(","));
				if(bounds.size() == 2)
				{
					_minWorkRange = hours(lexical_cast<long>(bounds[0]));
					_maxWorkRange = hours(lexical_cast<long>(bounds[1]));
				}
			}

			// Work duration filter
			string workDurationFilter(map.getDefault<string>(PARAMETER_WORK_DURATION_FILTER));
			if(!workDurationFilter.empty())
			{
				vector<string> bounds;
				split(bounds, workDurationFilter, is_any_of(","));
				if(bounds.size() == 2)
				{
					_minWorkDuration = hours(lexical_cast<long>(bounds[0]));
					_maxWorkDuration = hours(lexical_cast<long>(bounds[1]));
				}
			}

			// Line filter
			if(map.getDefault<RegistryKeyType>(PARAMETER_LINE_FILTER, 0))
			{
				_lineFilter = Env::GetOfficialEnv().get<CommercialLine>(
					map.get<RegistryKeyType>(PARAMETER_LINE_FILTER)
				).get();
			}

			// With ticket sales filter
			if(!map.getDefault<string>(PARAMETER_WITH_TICKET_SALES_FILTER).empty())
			{
				_withTicketSalesFilter = map.get<bool>(PARAMETER_WITH_TICKET_SALES_FILTER);
			}

			// Display page
			_page = getPage(map.getDefault<string>(PARAMETER_PAGE_ID));

			// Mime type
			if(!_page)
			{
				MimeType::LoadFromRecord(_mimeType, map);
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
			size_t rank(0);
			typedef std::set<std::pair<date, const DriverAllocation*> > Result;
			Result result;
			BOOST_FOREACH(const DriverAllocation::Registry::value_type& it, Env::GetOfficialEnv().getRegistry<DriverAllocation>())
			{
				// Init
				const DriverAllocation& alloc(*it.second);

				//////////////////////////////////////////////////////////////////////////
				// Checks

				// Min date check
				if(!_minDate.is_not_a_date() && alloc.get<Date>() < _minDate)
				{
					continue;
				}

				// Date check
				if(!_date.is_not_a_date() && alloc.get<Date>() != _date)
				{
					continue;
				}

				// Ticket sales filter
				if(!logic::indeterminate(_withTicketSalesFilter) && alloc.get<WithTicketSales>() != _withTicketSalesFilter)
				{
					continue;
				}

				// Line filter check
				if(_lineFilter)
				{
					bool ok(false);
					BOOST_FOREACH(const DriverService::Vector::Type::value_type& service, alloc.get<DriverService::Vector>())
					{
						BOOST_FOREACH(const DriverService::Chunks::value_type& chunk, service->getChunks())
						{
							BOOST_FOREACH(const DriverService::Chunk::Element& element, chunk.elements)
							{
								if(	dynamic_cast<ScheduledService*>(element.service) &&
									dynamic_cast<JourneyPattern*>(element.service->getPath())->getCommercialLine() == _lineFilter
								){
									ok = true;
									break;
								}
							}
							if(ok)
							{
								break;
							}
						}
						if(ok)
						{
							break;
						}
					}
					if(!ok)
					{
						continue;
					}
				}

				// Work range
				if(!_minWorkRange.is_not_a_date_time() && alloc.getWorkRange() < _minWorkRange)
				{
					continue;
				}
				if(!_maxWorkRange.is_not_a_date_time() && alloc.getWorkRange() > _maxWorkRange)
				{
					continue;
				}

				// Work duration
				if(!_minWorkDuration.is_not_a_date_time() && alloc.getWorkDuration() < _minWorkDuration)
				{
					continue;
				}
				if(!_maxWorkDuration.is_not_a_date_time() && alloc.getWorkDuration() > _maxWorkDuration)
				{
					continue;
				}

				// Defined driver : return only allocations for this driver
				if(_driver && (!alloc.get<Driver>() || &*alloc.get<Driver>() != _driver))
				{
					continue;
				}

				// Min hour filter
				if(!_minHourFilter.is_not_a_date_time() && alloc.getServiceBeginning() < _minHourFilter)
				{
					continue;
				}

				// Max hour filter
				if(!_maxHourFilter.is_not_a_date_time() && alloc.getServiceBeginning() > _maxHourFilter)
				{
					continue;
				}

				result.insert(make_pair(alloc.get<Date>(), &alloc));
			}

			BOOST_FOREACH(const Result::value_type& item, result)
			{
				// Init
				const DriverAllocation& alloc(*item.second);

				//////////////////////////////////////////////////////////////////////////
				// Output preparation

				shared_ptr<ParametersMap> allocPM(new ParametersMap);
				alloc.toParametersMap(*allocPM);

				if(alloc.get<DriverActivity>())
				{
					shared_ptr<ParametersMap> activityPM(new ParametersMap);
					alloc.get<DriverActivity>()->toParametersMap(*activityPM);
					allocPM->insert("activity", activityPM);
				}
				BOOST_FOREACH(const DriverService::Vector::Type::value_type& service, alloc.get<DriverService::Vector>())
				{
					shared_ptr<ParametersMap> servicePM(new ParametersMap);
					service->toParametersMap(*servicePM);
					allocPM->insert("service", servicePM);
				}

				map.insert(TAG_ALLOCATION, allocPM);
			}

			if(_page)
			{
				if(map.hasSubMaps(TAG_ALLOCATION))
				{
					BOOST_FOREACH(const shared_ptr<ParametersMap>& allocPM, map.getSubMaps(TAG_ALLOCATION))
					{
						allocPM->merge(getTemplateParameters());
						_page->display(stream, request, *allocPM);
			}	}	}
			else if(_mimeType == MimeTypes::XML)
			{
				map.outputXML(stream, TAG_ALLOCATIONS, true);
			}
			else if(_mimeType == MimeTypes::JSON)
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
			return _page ? _page->getMimeType() : _mimeType;
		}
}	}
