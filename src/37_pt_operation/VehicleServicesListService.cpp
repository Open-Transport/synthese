
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
#include "RequestException.h"
#include "Request.h"
#include "SchedulesBasedService.h"
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
				if(!_date.is_not_a_date() && !vs.isActive(_date))
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

				boost::shared_ptr<ParametersMap> vsMap(new ParametersMap);
				vs.toParametersMap(*vsMap);
				map.insert(TAG_VEHICLE_SERVICE, vsMap);
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
}	}
