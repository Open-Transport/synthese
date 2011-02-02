
/** Composition class header.
	@file Composition.hpp

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

#ifndef SYNTHESE_pt_operations_Composition_hpp__
#define SYNTHESE_pt_operations_Composition_hpp__

#include "Service.h"
#include "Registrable.h"
#include "Calendar.h"

#include <vector>

namespace synthese
{
	namespace pt
	{
		class ScheduledService;
	}

	namespace pt_operation
	{
		class Vehicle;

		/** Composition class.
			@ingroup m37
		*/
		class Composition:
			public virtual util::Registrable
		{
		public:
			typedef util::Registry<Composition> Registry;

			struct VehicleLink
			{
				const Vehicle* vehicle;
				std::string number;
			};

			typedef std::vector<VehicleLink> VehicleLinks;

		private:
			const pt::ScheduledService* _service;
			VehicleLinks _vehicles;
			calendar::Calendar _calendar;
			graph::Service::ServedVertices _servedVertices;


		public:
			Composition(util::RegistryKeyType id=0);

			//! @name Setters
			//@{
				void setService(const pt::ScheduledService* value){ _service = value; }
				void setVehicles(const VehicleLinks& value){ _vehicles = value; }
				void setCalendar(const calendar::Calendar& value){ _calendar = value; }
				void setServedVertices(const graph::Service::ServedVertices& value){ _servedVertices = value; }
			//@}

			//! @name Getters
			//@{
				const pt::ScheduledService* getService() const { return _service; }
				const VehicleLinks& getVehicles() const { return _vehicles; }
				const calendar::Calendar& getCalendar() const { return _calendar; }
				const graph::Service::ServedVertices& getServedVertices() const { return _servedVertices; }
			//@}
		};
	}
}

#endif // SYNTHESE_pt_operations_Composition_hpp__
