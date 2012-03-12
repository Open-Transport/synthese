
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

#include "MimeTypes.hpp"
#include "RequestException.h"
#include "Request.h"
#include "SchedulesBasedService.h"
#include "VehicleService.hpp"

#include <boost/algorithm/string.hpp>

using namespace boost;
using namespace std;
using namespace boost::algorithm;

namespace synthese
{
	using namespace cms;
	using namespace util;
	using namespace pt_operation;
	using namespace server;
	using namespace security;

	template<>
	const string FactorableTemplate<FunctionWithSite<false>, VehicleServicesListService>::FACTORY_KEY = "VehicleServicesList";
	
	namespace pt_operation
	{
		const string VehicleServicesListService::PARAMETER_PAGE = "p";
		const string VehicleServicesListService::TAG_VEHICLE_SERVICE = "vehicleService";
		const string VehicleServicesListService::TAG_VEHICLE_SERVICES = "vehicleServices";
		


		VehicleServicesListService::VehicleServicesListService():
			_page(NULL)
		{}



		ParametersMap VehicleServicesListService::_getParametersMap() const
		{
			ParametersMap map;
			if(_page)
			{
				map.insert(PARAMETER_PAGE, _page->getKey());
			}
			else if(!_mimeType.empty())
			{
				MimeType::SaveToParametersMap(_mimeType, map);
			}
			if(!_date.is_not_a_date())
			{
				Date::SaveToParametersMap(_date, map);
			}
			if(!_name.empty())
			{
				Name::SaveToParametersMap(_name, map);
			}
			return map;
		}



		void VehicleServicesListService::_setFromParametersMap(const ParametersMap& map)
		{
			// Date
			Date::LoadFromRecord(_date, map);

			// Page
			_page = getPage(map.getDefault<string>(PARAMETER_PAGE));

			// Mime type
			if(!_page)
			{
				MimeType::LoadFromRecord(_mimeType, map);
				if(_mimeType.empty())
				{
					_mimeType = MimeTypes::XML;
			}	}

			// Name
			Name::LoadFromRecord(_name, map);
		}



		ParametersMap VehicleServicesListService::run(
			std::ostream& stream,
			const Request& request
		) const {
			
			ParametersMap map;

			BOOST_FOREACH(const VehicleService::Registry::value_type& item, _env->getRegistry<VehicleService>())
			{
				const VehicleService& vs(*item.second);

				// Date filter
				if(!_date.is_not_a_date())
				{
					bool ok(false);
					BOOST_FOREACH(const VehicleService::Services::value_type& service, vs.getServices())
					{
						if(service->isActive(_date))
						{
							ok = true;
							break;
						}
					}
					if(!ok)
					{
						continue;
					}
				}

				// Name filter
				if(!_name.empty())
				{
					if(vs.getName().find(_name) == string::npos)
					{
						continue;
					}
				}

				shared_ptr<ParametersMap> vsMap(new ParametersMap);
				vs.toParametersMap(*vsMap);
				map.insert(TAG_VEHICLE_SERVICE, vsMap);
			}

			if(_page)
			{
				if(map.hasSubMaps(TAG_VEHICLE_SERVICE))
				{
					BOOST_FOREACH(const shared_ptr<ParametersMap>& vsMap, map.getSubMaps(TAG_VEHICLE_SERVICE))
					{
						_page->display(stream, request, *vsMap);
					}
				}
			}
			else if(_mimeType == MimeTypes::XML)
			{
				map.outputXML(stream, TAG_VEHICLE_SERVICES, true, "");
			}
			else if(_mimeType == MimeTypes::JSON)
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
			return _page ? _page->getMimeType() : _mimeType;
		}
}	}
