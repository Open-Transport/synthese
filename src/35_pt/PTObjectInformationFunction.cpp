
/** PTObjectInformationFunction class implementation.
	@file PTObjectInformationFunction.cpp
	@author Hugues Romain
	@date 2010

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

#include "RequestException.h"
#include "Request.h"
#include "PTObjectInformationFunction.hpp"
#include "TransportNetworkTableSync.h"
#include "CommercialLineTableSync.h"
#include "JourneyPatternTableSync.hpp"
#include "StopAreaTableSync.hpp"
#include "CityTableSync.h"
#include "StopPoint.hpp"
#include "StopArea.hpp"
#include "JourneyPattern.hpp"
#include "ScheduledServiceTableSync.h"
#include "JourneyPatternCopy.hpp"

using namespace std;
using namespace boost;

namespace synthese
{
	using namespace util;
	using namespace server;
	using namespace security;
	using namespace geography;

	template<> const string util::FactorableTemplate<Function,pt::PTObjectInformationFunction>::FACTORY_KEY("PTObjectInformationFunction");

	namespace pt
	{
		const string PTObjectInformationFunction::PARAMETER_INFO("info");

		ParametersMap PTObjectInformationFunction::_getParametersMap() const
		{
			ParametersMap map;
			return map;
		}

		void PTObjectInformationFunction::_setFromParametersMap(const ParametersMap& map)
		{
			RegistryKeyType id(map.get<RegistryKeyType>(Request::PARAMETER_OBJECT_ID));
			RegistryTableType tableId(decodeTableId(id));

			if(tableId == TransportNetworkTableSync::TABLE.ID)
			{
				_network = Env::GetOfficialEnv().get<TransportNetwork>(id);
			}
			else if(tableId == CommercialLineTableSync::TABLE.ID)
			{
				_line = Env::GetOfficialEnv().get<CommercialLine>(id);
			}
			else if(tableId == JourneyPatternTableSync::TABLE.ID)
			{
				_route = Env::GetOfficialEnv().get<JourneyPattern>(id);
			}
			else if(tableId == StopAreaTableSync::TABLE.ID)
			{
				_stop = Env::GetOfficialEnv().get<StopArea>(id);
			}
			else if(tableId == ScheduledServiceTableSync::TABLE.ID)
			{
				_service = Env::GetOfficialEnv().get<ScheduledService>(id);
			}
			else if(tableId == CityTableSync::TABLE.ID)
			{
				_city = Env::GetOfficialEnv().get<City>(id);
			}

			_info = map.get<string>(PARAMETER_INFO);
		}



		util::ParametersMap PTObjectInformationFunction::run(
			std::ostream& stream,
			const Request& request
		) const {

			util::ParametersMap pm;

			if(_network.get())
			{
				if(_info == "name")
				{
					stream << _network->getName();
				}
			}
			else if(_city.get())
			{
				if(_info == "name")
				{
					stream << _city->getName();
				}
			}
			else if(_line.get())
			{
				if(_info == "short_name")
				{
					stream << _line->getShortName();
				}
				else if(_info == "name")
				{
					stream << _line->getName();
				}
				else if(_info == "style")
				{
					stream << _line->getStyle();
				}
				else if(_info == "image")
				{
					stream << _line->getImage();
				}
				else if(_info == "network_id" && _line->getNetwork())
				{
					stream << _line->getNetwork()->getKey();
				}
				else if(_info == "map_url")
				{
					stream << _line->getMapURL();
				}
				else if(_info == "doc_url")
				{
					stream << _line->getDocURL();
				}
				else if(_info == "timetable_id")
				{
					stream << _line->getTimetableId();
				}
			}
			else if(_route.get())
			{
				if(_info == "line_id" && _route->getCommercialLine())
				{
					stream << _route->getCommercialLine()->getKey();
				}
				else if(_info == "origin_city_name" && _route->getEdges().size() > 1)
				{
					stream << _route->getOrigin()->getConnectionPlace()->getCity()->getName();
				}
				else if(_info == "origin_stop_name" && _route->getEdges().size() > 1)
				{
					stream << _route->getOrigin()->getConnectionPlace()->getName();
				}
				else if(_info == "destination_city_name" && _route->getEdges().size() > 1)
				{
					stream << _route->getDestination()->getConnectionPlace()->getCity()->getName();
				}
				else if(_info == "destination_stop_name" && _route->getEdges().size() > 1)
				{
					stream << _route->getDestination()->getConnectionPlace()->getName();
				}
				else if(_info == "name" && _route->getEdges().size() > 1)
				{
					stream << _route->getName();
				}
			}
			else if(_stop.get())
			{
				if(_info == "city_name" && _stop->getCity())
				{
					stream << _stop->getCity()->getName();
				}
				else if(_info == "name")
				{
					stream << _stop->getName();
				}
				else if(_info == "short_name")
				{
					stream << _stop->getName26();
				}
			}
			else if(_service.get())
			{
				if(_info == "line_id")
				{
					stream << _service->getPath()->getPathGroup()->getKey();
				}
				else if(_info == "route_id")
				{
					stream <<
						(	dynamic_cast<const JourneyPatternCopy*>(_service->getPath()) ?
							dynamic_cast<const JourneyPatternCopy*>(_service->getPath())->getMainLine()->getKey() :
							_service->getPath()->getKey()
						)
					;
				}
				else if(_info == "number")
				{
					stream << _service->getServiceNumber();
				}
			}

			return pm;
		}



		bool PTObjectInformationFunction::isAuthorized(
			const Session* session
		) const {
			return true;
		}



		std::string PTObjectInformationFunction::getOutputMimeType() const
		{
			return "text/plain";
		}
	}
}
