
//////////////////////////////////////////////////////////////////////////////////////////
///	DevicesService class implementation.
///	@file DevicesService.cpp
///	@author Camille Hue
///	@date 2013
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

#include "DevicesService.hpp"

#include "Device.h"
#include "RequestException.h"
#include "Request.h"

using namespace boost;
using namespace std;

namespace synthese
{
	using namespace util;
	using namespace server;
	using namespace security;

	template<>
	const string FactorableTemplate<Function,server::DevicesService>::FACTORY_KEY = "devices";
	
	namespace server
	{
		const string DevicesService::TAG_DEVICE = "device";
		


		ParametersMap DevicesService::_getParametersMap() const
		{
			ParametersMap map;
			return map;
		}



		void DevicesService::_setFromParametersMap(const ParametersMap& map)
		{
		}



		ParametersMap DevicesService::run(
			std::ostream& stream,
			const Request& request
		) const {
			ParametersMap map;
			
			BOOST_FOREACH(boost::shared_ptr<Device> device, Factory<Device>::GetNewCollection())
			{
				boost::shared_ptr<ParametersMap> devicePM(new ParametersMap);
				device->toParametersMap(*devicePM);
				map.insert(TAG_DEVICE, devicePM);
			}

			return map;
		}
		
		
		
		bool DevicesService::isAuthorized(
			const Session* session
		) const {
			return true;
		}



		std::string DevicesService::getOutputMimeType() const
		{
			return "text/html";
		}
}	}
