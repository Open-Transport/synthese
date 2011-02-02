
/** GetVehiclesService class implementation.
	@file GetVehiclesService.cpp
	@author RCSobility
	@date 2011

	This file belongs to the SYNTHESE project (public transportation specialized software)
	Copyright (C) 2002 Hugues Romain - RCS <contact@reseaux-conseil.com>

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
#include "GetVehiclesService.hpp"
#include "Webpage.h"
#include "CommercialLine.h"
#include "Vehicle.hpp"
#include "WebPageDisplayFunction.h"
#include "StaticFunctionRequest.h"

using namespace std;

namespace synthese
{
	using namespace util;
	using namespace server;
	using namespace security;
	using namespace cms;
	using namespace pt;

	template<> const string util::FactorableTemplate<Function,pt_operation::GetVehiclesService>::FACTORY_KEY("GetVehiclesService");
	
	namespace pt_operation
	{
		const string GetVehiclesService::PARAMETER_VEHICLE_PAGE_ID("vp");
		const string GetVehiclesService::PARAMETER_LINE_ID("li");

		const string GetVehiclesService::DATA_NAME("name");
		const string GetVehiclesService::DATA_NUMBER("number");
		const string GetVehiclesService::DATA_PICTURE("picture");
		const string GetVehiclesService::DATA_RANK("rank");


		ParametersMap GetVehiclesService::_getParametersMap() const
		{
			ParametersMap map;

			if(_vehiclePage.get())
			{
				map.insert(PARAMETER_VEHICLE_PAGE_ID, _vehiclePage->getKey());
			}

			if(_line.get())
			{
				map.insert(PARAMETER_LINE_ID, _line->getKey());
			}

			return map;
		}

		void GetVehiclesService::_setFromParametersMap(const ParametersMap& map)
		{
			// Vehicle display page
			try
			{
				RegistryKeyType id(map.getDefault<RegistryKeyType>(PARAMETER_VEHICLE_PAGE_ID, 0));
				if(id > 0)
				{
					_vehiclePage = Env::GetOfficialEnv().getEditable<Webpage>(id);
				}
			}
			catch(ObjectNotFoundException<Webpage>&)
			{
				throw RequestException("No such vehicle page");
			}

			// Line page
			try
			{
				RegistryKeyType id(map.getDefault<RegistryKeyType>(PARAMETER_LINE_ID, 0));
				if(id > 0)
				{
					_line = Env::GetOfficialEnv().getEditable<CommercialLine>(id);
				}
			}
			catch(ObjectNotFoundException<CommercialLine>&)
			{
				throw RequestException("No such line");
			}
		}

		void GetVehiclesService::run(
			std::ostream& stream,
			const Request& request
		) const {

			size_t rank(0);
			BOOST_FOREACH(const Vehicle::Registry::value_type& vehicle, Env::GetOfficialEnv().getRegistry<Vehicle>())
			{
				if(_line.get())
				{
					Vehicle::AllowedLines::const_iterator it(vehicle.second->getAllowedLines().find(_line.get()));
					if(it == vehicle.second->getAllowedLines().end())
					{
						continue;
					}
				}

				_displayVehicle(
					stream,
					request,
					*vehicle.second,
					rank++
				);
			}
		}
		
		
		
		bool GetVehiclesService::isAuthorized(
			const Session* session
		) const {
			return true;
		}



		std::string GetVehiclesService::getOutputMimeType() const
		{
			return "text/html";
		}



		void GetVehiclesService::_displayVehicle(
			std::ostream& stream,
			const server::Request& request,
			const Vehicle& vehicle,
			std::size_t rank
		) const {

			ParametersMap pm;

			pm.insert(DATA_NAME, vehicle.getName());
			pm.insert(DATA_NUMBER, vehicle.getNumber());
			pm.insert(DATA_PICTURE, vehicle.getPicture());
			pm.insert(DATA_RANK, rank);
			pm.insert(Request::PARAMETER_OBJECT_ID, vehicle.getKey());

			// Launch of the display
			StaticFunctionRequest<WebPageDisplayFunction> displayRequest(request, false);
			displayRequest.getFunction()->setPage(_vehiclePage);
			displayRequest.getFunction()->setUseTemplate(false);
			displayRequest.getFunction()->setAditionnalParametersMap(pm);
			displayRequest.run(stream);
		}
	}
}
