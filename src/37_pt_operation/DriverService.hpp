
/** DriverService class header.
	@file DriverService.hpp

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

#ifndef SYNTHESE_pt_operation_DriverService_hpp__
#define SYNTHESE_pt_operation_DriverService_hpp__

#include "ImportableTemplate.hpp"
#include "Calendar.h"
#include "Named.h"
#include "StandardFields.hpp"

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
			public util::Named,
			public impex::ImportableTemplate<DriverService>,
			public calendar::Calendar,
			public virtual util::Registrable,
			public ObjectField<DriverService, boost::optional<DriverService&> >
		{
		public:
			//////////////////////////////////////////////////////////////////////////
			/// Auto generated object vector field (will be defined by Object in the future)
			class Vector:
				public ObjectField<Vector, std::vector<DriverService*> >
			{
			};


			struct Chunk
			{
				struct Element
				{
					pt::SchedulesBasedService* service;
					std::size_t startRank;
					std::size_t endRank;

					Element():
						service(NULL),
						startRank(0),
						endRank(0)
					{}
				};

				DriverService* driverService;
				VehicleService* vehicleService;
				std::vector<Element> elements;

				Chunk(
					VehicleService* _vehicleService = NULL
				):	driverService(NULL),
					vehicleService(_vehicleService)
				{}

				Chunk(
					DriverService* _driverService,
					VehicleService& _vehicleService,
					const boost::posix_time::time_duration& startTime,
					const boost::posix_time::time_duration& endTime
				);
			};

			typedef std::vector<Chunk> Chunks;
			typedef util::Registry<DriverService> Registry;

		private:
			Chunks _chunks;

		public:
			DriverService(util::RegistryKeyType id = 0);

			//! @name Getters
			//@{
				const Chunks& getChunks() const { return _chunks; }
			//@}

			//! @name Setters
			//@{
				void setChunks(const Chunks& value);
			//@}
		};
}	}

#endif // SYNTHESE_pt_operation_DriverService_hpp__
