
/** GetCompositionsService class implementation.
	@file GetCompositionsService.cpp
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
#include "GetCompositionsService.hpp"
#include "ScheduledService.h"
#include "Webpage.h"
#include "PTOperationModule.hpp"
#include "Composition.hpp"
#include "Vehicle.hpp"
#include "StopPoint.hpp"

#include <boost/foreach.hpp>

using namespace std;
using namespace boost::gregorian;

namespace synthese
{
	using namespace util;
	using namespace server;
	using namespace security;
	using namespace cms;
	using namespace pt;
	using namespace graph;


	template<> const string util::FactorableTemplate<Function,pt_operation::GetCompositionsService>::FACTORY_KEY("GetCompositionsService");

	namespace pt_operation
	{
		const string GetCompositionsService::PARAMETER_DATE("da");
		const string GetCompositionsService::PARAMETER_COMPOSITION_PAGE_ID("ci");
		const string GetCompositionsService::PARAMETER_VEHICLE_PAGE_ID("vi");
		const string GetCompositionsService::PARAMETER_SERVICE_ID("si");
		const string GetCompositionsService::PARAMETER_QUAY_PAGE_ID("qi");

		const string GetCompositionsService::DATA_QUAYS("quays");
		const string GetCompositionsService::DATA_VEHICLES("vehicles");
		const string GetCompositionsService::DATA_RANK("rank");
		const string GetCompositionsService::DATA_NUMBER("number");
		const string GetCompositionsService::DATA_NAME("name");
		const string GetCompositionsService::DATA_PICTURE("picture");
		const string GetCompositionsService::DATA_SEATS("seats");
		const string GetCompositionsService::DATA_COMPOSITION_ID("composition_id");
		const string GetCompositionsService::DATA_VEHICLE_ID("vehicle_id");
		const string GetCompositionsService::DATA_QUAY_ID("quay_id");

		ParametersMap GetCompositionsService::_getParametersMap() const
		{
			ParametersMap map;
			if(_service.get())
			{
				map.insert(PARAMETER_SERVICE_ID, _service->getKey());
			}
			if(_compositionPage.get())
			{
				map.insert(PARAMETER_COMPOSITION_PAGE_ID, _compositionPage->getKey());
			}
			if(_vehiclePage.get())
			{
				map.insert(PARAMETER_VEHICLE_PAGE_ID, _vehiclePage->getKey());
			}
			if(_quayPage.get())
			{
				map.insert(PARAMETER_QUAY_PAGE_ID, _quayPage->getKey());
			}
			return map;
		}

		void GetCompositionsService::_setFromParametersMap(const ParametersMap& map)
		{
			// Date
			if(map.getDefault<string>(PARAMETER_DATE).empty())
			{
				_date = day_clock::local_day();
			}
			else
			{
				_date = from_simple_string(map.get<string>(PARAMETER_DATE));
			}

			// Service
			try
			{
				_service = Env::GetOfficialEnv().get<ScheduledService>(map.get<RegistryKeyType>(PARAMETER_SERVICE_ID));
			}
			catch(ObjectNotFoundException<ScheduledService>&)
			{
				throw RequestException("No such service");
			}


			// Composition display page
			try
			{
				RegistryKeyType id(map.getDefault<RegistryKeyType>(PARAMETER_COMPOSITION_PAGE_ID, 0));
				if(id > 0)
				{
					_compositionPage = Env::GetOfficialEnv().get<Webpage>(id);
				}
			}
			catch(ObjectNotFoundException<Webpage>&)
			{
				throw RequestException("No such composition page");
			}

			// Vehicle display page
			try
			{
				RegistryKeyType id(map.getDefault<RegistryKeyType>(PARAMETER_VEHICLE_PAGE_ID, 0));
				if(id > 0)
				{
					_vehiclePage = Env::GetOfficialEnv().get<Webpage>(id);
				}
			}
			catch(ObjectNotFoundException<Webpage>&)
			{
				throw RequestException("No such vehicle page");
			}

			// Quay display page
			try
			{
				RegistryKeyType id(map.getDefault<RegistryKeyType>(PARAMETER_QUAY_PAGE_ID, 0));
				if(id > 0)
				{
					_quayPage = Env::GetOfficialEnv().get<Webpage>(id);
				}
			}
			catch(ObjectNotFoundException<Webpage>&)
			{
				throw RequestException("No such quay page");
			}
		}



		void GetCompositionsService::run(
			std::ostream& stream,
			const Request& request
		) const {

			if(!_compositionPage.get())
			{
				return;
			}

			size_t rank(0);
			BOOST_FOREACH(const PTOperationModule::ServiceCompositions::mapped_type::value_type& composition, PTOperationModule::GetCompositions(*_service))
			{
				if(composition->getCalendar().isActive(_date))
				{
					displayComposition(
						stream,
						request,
						*composition,
						rank++
					);
				}
			}
		}



		bool GetCompositionsService::isAuthorized(
			const Session* session
		) const {
			return true;
		}



		std::string GetCompositionsService::getOutputMimeType() const
		{
			return _compositionPage.get() ? _compositionPage->getMimeType() : "text/html";
		}



		void GetCompositionsService::displayComposition(
			std::ostream& stream,
			const server::Request& request,
			const Composition& composition,
			size_t rank
		) const {

			ParametersMap pm(_savedParameters);

			// Rank
			pm.insert(DATA_RANK, rank);

			// ID
			pm.insert(DATA_COMPOSITION_ID, composition.getKey());

			// Vehicles
			if(_vehiclePage.get())
			{
				stringstream s;
				size_t rank(0);

				BOOST_FOREACH(const Composition::VehicleLinks::value_type& vehicle, composition.getVehicles())
				{
					displayVehicle(
						s,
						request,
						*vehicle.vehicle,
						vehicle.number,
						rank++
					);
				}

				pm.insert(DATA_VEHICLES, s.str());
			}

			// Quays
			if(_quayPage.get())
			{
				stringstream s;
				size_t rank(0);

				BOOST_FOREACH(const Service::ServedVertices::value_type& quay, composition.getServedVertices())
				{
					displayQuay(
						s,
						request,
						*dynamic_cast<const StopPoint*>(quay),
						rank++
					);
				}

				pm.insert(DATA_QUAYS, s.str());
			}


			// Launch of the display
			_compositionPage->display(stream, request, pm);
		}



		void GetCompositionsService::displayVehicle(
			std::ostream& stream,
			const server::Request& request,
			const Vehicle& vehicle,
			const std::string& number,
			size_t rank
		) const {

			ParametersMap pm(_savedParameters);

			// Rank
			pm.insert(DATA_RANK, rank);

			// Picture
			pm.insert(DATA_PICTURE, vehicle.getPicture());

			pm.insert(DATA_SEATS, vehicle.getSeats().size());

			// Name
			pm.insert(DATA_NAME, vehicle.getName());

			// Number
			pm.insert(DATA_NUMBER, vehicle.getNumber());

			// ID
			pm.insert(DATA_VEHICLE_ID, vehicle.getKey());

			// Launch of the display
			_vehiclePage->display(stream, request, pm);
		}



		void GetCompositionsService::displayQuay(
			std::ostream& stream,
			const server::Request& request,
			const StopPoint& quay,
			size_t rank
		) const {

			ParametersMap pm(_savedParameters);

			// Rank
			pm.insert(DATA_RANK, rank);

			// Name
			pm.insert(DATA_NAME, quay.getName());

			// ID
			pm.insert(DATA_QUAY_ID, quay.getKey());

			// Launch of the display
			_quayPage->display(stream, request, pm);
		}
}	}
