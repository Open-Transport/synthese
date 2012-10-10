
/** Composition class header.
	@file Composition.hpp

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

#ifndef SYNTHESE_pt_operations_Composition_hpp__
#define SYNTHESE_pt_operations_Composition_hpp__

#include "Registrable.h"
#include "Registry.h"
#include "Calendar.h"

#include <vector>

namespace synthese
{
	namespace vehicle
	{
		class Vehicle;

		//////////////////////////////////////////////////////////////////////////
		/// Composition class.
		/// Allocation of one or more vehicles (train) to one or more services
		/// (ScheduledService or VehicleService), valid on one or more days
		/// (Calendar)
		//////////////////////////////////////////////////////////////////////////
		///	@ingroup m37
		/// @author Hugues Romain
		class Composition:
			public virtual util::Registrable,
			public calendar::Calendar
		{
		public:
			typedef util::Registry<Composition> Registry;

			struct VehicleLink
			{
				const Vehicle* vehicle;
				std::string number;
			};

			typedef std::vector<VehicleLink> VehicleLinks;

		protected:
			VehicleLinks _vehicles;

		public:
			Composition(util::RegistryKeyType id=0);


			//! @name Setters
			//@{
				void setVehicles(const VehicleLinks& value){ _vehicles = value; }
			//@}

			//! @name Getters
			//@{
				const VehicleLinks& getVehicles() const { return _vehicles; }
			//@}
		};
}	}

#endif // SYNTHESE_pt_operations_Composition_hpp__
