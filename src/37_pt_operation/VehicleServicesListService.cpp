
//////////////////////////////////////////////////////////////////////////////////////////
///	VehicleServicesListService class implementation.
///	@file VehicleServicesListService.cpp
///	@author Hugues Romain
///	@date 2012
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

#include "VehicleServicesListService.hpp"

#include "DateField.hpp"
#include "MimeTypes.hpp"
#include "OperationUnit.hpp"
#include "RequestException.h"
#include "Request.h"
#include "ScheduledService.h"
#include "StringField.hpp"
#include "VehicleService.hpp"

#include <boost/algorithm/string.hpp>

using namespace boost;
using namespace std;
using namespace boost::algorithm;
using namespace boost::gregorian;

namespace synthese
{
	using namespace cms;
	using namespace util;
	using namespace pt;
	using namespace pt_operation;
	using namespace server;
	using namespace security;

	template<>
	const string FactorableTemplate<FunctionWithSite<false>, VehicleServicesListService>::FACTORY_KEY = "VehicleServicesList";

	namespace pt_operation
	{
		const string VehicleServicesListService::PARAMETER_PAGE = "p";
		const string VehicleServicesListService::PARAMETER_WITH_DETAIL = "with_detail";
		const string VehicleServicesListService::PARAMETER_SERVICE = "service";

		const string VehicleServicesListService::TAG_VEHICLE_SERVICE = "vehicleService";
		const string VehicleServicesListService::TAG_VEHICLE_SERVICES = "vehicleServices";



		VehicleServicesListService::VehicleServicesListService():
			_page(NULL),
			_withDetail(true),
			_scheduledService(NULL)
		{}



		ParametersMap VehicleServicesListService::_getParametersMap() const
		{
			ParametersMap map;
			if(_page)
			{
				map.insert(PARAMETER_PAGE, _page->getKey());
			}
			else
			{
				map.insert(PARAMETER_OUTPUT_FORMAT, _outputFormat);
			}

			if(!_date.is_not_a_date())
			{
				map.insert(Date::FIELD.name, Date::DateToString(_date));
			}
			if(!_name.empty())
			{
				map.insert(Name::FIELD.name, _name);
			}
			if(!_withDetail)
			{
				map.insert(PARAMETER_WITH_DETAIL, _withDetail);
			}
			return map;
		}



		void VehicleServicesListService::_setFromParametersMap(const ParametersMap& map)
		{
			// Date
			if(!map.getDefault<string>(Date::FIELD.name).empty())
			{
				_date = from_string(map.get<string>(Date::FIELD.name));
			}

			// Page
			_page = getPage(map.getDefault<string>(PARAMETER_PAGE));

			// Mime type
			if(!_page)
			{
				setOutputFormatFromMap(map, MimeTypes::XML);
			}

			// Name
			_name = map.getDefault<string>(Name::FIELD.name);

			// Detail
			_withDetail = map.getDefault<bool>(PARAMETER_WITH_DETAIL, true);

			// id filter
			RegistryKeyType id(map.getDefault<RegistryKeyType>(Request::PARAMETER_OBJECT_ID, 0));
			if(id) try
			{
				_service = Env::GetOfficialEnv().get<VehicleService>(id);
			}
			catch(ObjectNotFoundException<VehicleService>&)
			{
				throw RequestException("No such vehicle service");
			}

			// Operation unit filter
			RegistryKeyType unitId(map.getDefault<RegistryKeyType>(OperationUnit::FIELD.name, 0));
			if(unitId) try
			{
				_operationUnit = *Env::GetOfficialEnv().get<OperationUnit>(unitId);
			}
			catch(ObjectNotFoundException<OperationUnit>&)
			{
				throw RequestException("No such operation unit");
			}

			// Scheduled service filter
			RegistryKeyType serviceId(map.getDefault<RegistryKeyType>(PARAMETER_SERVICE, 0));
			if(serviceId) try
			{
				_scheduledService = Env::GetOfficialEnv().get<ScheduledService>(serviceId).get();
			}
			catch(ObjectNotFoundException<ScheduledService>&)
			{
				throw RequestException("No such service");
			}
		}



		ParametersMap VehicleServicesListService::run(
			std::ostream& stream,
			const Request& request
		) const {

			ParametersMap map;

			if(_service)
			{
				_exportService(*_service, map);
			}
			else
			{
				recursive_mutex::scoped_lock registryLock(Env::GetOfficialEnv().getRegistry<VehicleService>().getMutex());
				BOOST_FOREACH(const VehicleService::Registry::value_type& item, Env::GetOfficialEnv().getRegistry<VehicleService>())
				{
					const VehicleService& vs(*item.second);

					// Date filter
					if(!_date.is_not_a_date() && !vs.isActive(_date))
					{
						continue;
					}

					// Operation unit filter
					if(_operationUnit && (!vs.getOperationUnit() || &*vs.getOperationUnit() != &*_operationUnit))
					{
						continue;
					}

					// Name filter
					if(!_name.empty())
					{
						if(	vs.getName().size() < _name.size() ||
							vs.getName().substr(0, _name.size()) != _name
						){
							continue;
						}
					}

					// Service filter
					if(_scheduledService)
					{
						bool result(false);
						BOOST_FOREACH(const VehicleService::Services::value_type service, vs.getServices())
						{
							if(!dynamic_cast<const ScheduledService*>(service))
							{
								continue;
							}
							if(static_cast<const ScheduledService*>(service) == _scheduledService)
							{
								result = true;
								break;
							}
						}
						if(!result)
						{
							continue;
						}
					}

					_exportService(vs, map);
				}
			}

			if(_page)
			{
				if(map.hasSubMaps(TAG_VEHICLE_SERVICE))
				{
					BOOST_FOREACH(const boost::shared_ptr<ParametersMap>& vsMap, map.getSubMaps(TAG_VEHICLE_SERVICE))
					{
						vsMap->merge(getTemplateParameters());
						_page->display(stream, request, *vsMap);
					}
				}
			}
			else if(_outputFormat == MimeTypes::XML)
			{
				map.outputXML(stream, TAG_VEHICLE_SERVICES, true, "");
			}
			else if(_outputFormat == MimeTypes::JSON)
			{
				map.outputJSON(stream, TAG_VEHICLE_SERVICES);
			}

			return map;
		}



		bool VehicleServicesListService::isAuthorized(
			const Session* session
		) const {
			return true;
		}



		std::string VehicleServicesListService::getOutputMimeType() const
		{
			return _page ? _page->getMimeType() : getOutputMimeTypeFromOutputFormat();
		}



		void VehicleServicesListService::_exportService( const VehicleService& vs, util::ParametersMap& map ) const
		{
			boost::shared_ptr<ParametersMap> vsMap(new ParametersMap);
			vs.toParametersMap(*vsMap, _withDetail);
			map.insert(TAG_VEHICLE_SERVICE, vsMap);
		}
}	}
