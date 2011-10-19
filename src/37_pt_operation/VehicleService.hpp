
/** VehicleService class header.
	@file VehicleService.hpp

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

#ifndef SYNTHESE_pt_operations_VehicleService_hpp__
#define SYNTHESE_pt_operations_VehicleService_hpp__

#include "Named.h"
#include "Importable.h"
#include "Registrable.h"
#include "Registry.h"

#include <set>
#include <vector>

namespace synthese
{
	namespace pt
	{
		class SchedulesBasedService;
	}

	namespace pt_operation
	{
		/** Vehicle service class.
			@ingroup m37
		*/
		class VehicleService:
			public util::Named,
			public impex::Importable,
			public virtual util::Registrable
		{
		public:
			typedef util::Registry<VehicleService> Registry;
			typedef std::vector<pt::SchedulesBasedService*> Services;

		private:
			Services _services;

		public:
			VehicleService(util::RegistryKeyType id=0);

			//! @name Setters
			//@{
				void setServices(const Services& value){ _services = value; }
			//@}

			//! @name Getters
			//@{
				const Services& getServices() const { return _services; }
			//@}

			//! @name Updaters
			//@{
				void insert(pt::SchedulesBasedService& value);
				void clear();
			//@}

			//! @name Services
			//@{
				/// @return the nth service of the vehicle service, NULL if non existent
				pt::SchedulesBasedService* getService(std::size_t rank) const;
			//@}
		};
}	}

#endif // SYNTHESE_pt_operations_Vehicle_hpp__
