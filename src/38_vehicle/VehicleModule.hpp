
/** VehicleModule class header.
	@file VehicleModule.h

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

#ifndef SYNTHESE_VehicleModule_H__
#define SYNTHESE_VehicleModule_H__

#include "ModuleClassTemplate.hpp"

#include "CurrentJourney.hpp"
#include "VehiclePosition.hpp"

#include <vector>
#include <utility>
#include <boost/optional.hpp>

namespace synthese
{
	namespace pt
	{
		class CommercialLine;
		class ScheduledService;
		class StopPoint;
	}

	//////////////////////////////////////////////////////////////////////////
	/// 38 Vehicle module namespace.
	///	@author Hugues Romain
	///	@ingroup m38
	namespace vehicle
	{
		class Vehicle;
		class ServiceComposition;

		/**	@defgroup m38Actions 38 Actions
			@ingroup m38

			@defgroup m38Pages 38 Pages
			@ingroup m38

			@defgroup m38Library 38 Interface library
			@ingroup m38

			@defgroup m38Functions 38 Functions
			@ingroup m38

			@defgroup m38LS 38 Table synchronizers
			@ingroup m38

			@defgroup m38Admin 38 Administration pages
			@ingroup m38

			@defgroup m38Rights 38 Rights
			@ingroup m38

			@defgroup m38Logs 38 DB Logs
			@ingroup m38

			@defgroup m38File 38.16 File formats
			@ingroup m38

			@defgroup m38WFS 38.39 WFS types
			@ingroup m38

			@defgroup m38 38 Vehicle
			@ingroup m3

		@{
		*/

		/** 38 Vehicle module class.
		*/
		class VehicleModule:
			public server::ModuleClassTemplate<VehicleModule>
		{
		public:
			typedef std::map<const pt::CommercialLine*, std::set<const Vehicle*> > LinesAllowedVehicles;
			typedef std::map<const pt::ScheduledService*, std::set<const ServiceComposition*> > ServiceCompositions;

		private:
			static CurrentJourney _currentJourney;
			static LinesAllowedVehicles _linesAllowedVehicles;
			static ServiceCompositions _serviceCompositions;
			static VehiclePosition _currentVehiclePosition;

		public:
			static void RegisterVehicle(const Vehicle& vehicle);
			static void UnregisterVehicle(const Vehicle& vehicle);
			static LinesAllowedVehicles::mapped_type GetAllowedVehicles(const pt::CommercialLine& line);

			static void RegisterComposition(const ServiceComposition& composition);
			static void UnregisterComposition(const ServiceComposition& composition);
			static ServiceCompositions::mapped_type GetCompositions(const pt::ScheduledService& service);

			static VehiclePosition& GetCurrentVehiclePosition(){ return _currentVehiclePosition; }

			static CurrentJourney& GetCurrentJourney(){ return _currentJourney; }
		};
	}
	/** @} */
}

#endif
