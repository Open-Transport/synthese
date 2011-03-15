
/** VehiclePosition class header.
	@file VehiclePosition.hpp

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

#ifndef SYNTHESE_pt_operation_VehiclePosition_hpp__
#define SYNTHESE_pt_operation_VehiclePosition_hpp__

#include "Registrable.h"
#include "Registry.h"

namespace geos
{
	namespace geom
	{
		class Point;
	}
}

namespace synthese
{
	namespace pt
	{
		class StopPoint;
		class ScheduledService;
	}

	namespace pt_operation
	{
		class Vehicle;

		/** VehiclePosition class.
			@ingroup m37
		*/
		class VehiclePosition:
			public util::Registrable
		{
		public:
			typedef util::Registry<Vehicle> Registry;
			typedef double Meters;

			typedef enum
			{
				TRAINING,
				DEAD_RUN_DEPOT,
				DEAD_RUN_TRANSFER,
				SERVICE,
				COMMERCIAL
			} Status;

		private:
			Vehicle* _vehicle;
			boost::shared_ptr<geos::geom::Point> _point;
			boost::posix_time::ptime _time;
			Meters _meterOffset;
			bool _zero;
			pt::StopPoint* _stopPoint;
			std::string _comment;
			pt::ScheduledService* _service;
			boost::optional<std::size_t> _rankInPath;
			boost::optional<std::size_t> _passengers;

		public:
			VehiclePosition(
				util::RegistryKeyType id = 0
			);
		};
}	}

#endif // SYNTHESE_pt_operation_VehiclePosition_hpp__
