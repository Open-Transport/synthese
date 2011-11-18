
/** DriverService class header.
	@file DriverService.hpp

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

#ifndef SYNTHESE_pt_operation_DriverService_hpp__
#define SYNTHESE_pt_operation_DriverService_hpp__

#include "Importable.h"
#include "Calendar.h"

namespace synthese
{
	namespace pt
	{
		class SchedulesBasedService;
	}

	namespace pt_operation
	{
		class VehicleService;

		/** DriverService class.
			@ingroup m37
		*/
		class DriverService:
			public impex::ImportableTemplate<DriverService>,
			public calendar::Calendar,
			public virtual util::Registrable
		{
		public:
			struct Element
			{
				pt::SchedulesBasedService* service;
				std::size_t startRank;
				std::size_t endRank;
			};
			typedef std::vector<Element> Services;
			typedef util::Registry<DriverService> Registry;

		private:
			Services _services;
			VehicleService* _vehicleService;

		public:
			DriverService(util::RegistryKeyType id = 0);

			//! @name Getters
			//@{
				const Services& getServices() const { return _services; }
				VehicleService* getVehicleService() const { return _vehicleService; }
			//@}

			//! @name Setters
			//@{
				void setServices(const Services& value){ _services = value; }
				void setVehicleService(VehicleService* value){ _vehicleService = value; }
			//@}
		};
	}
}

#endif // SYNTHESE_pt_operation_DriverService_hpp__

