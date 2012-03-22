
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
			static const std::string TAG_CHUNK;
			static const std::string TAG_ELEMENT;
			static const std::string TAG_VEHICLE_SERVICE;
			static const std::string ATTR_START_TIME;
			static const std::string ATTR_DRIVER_START_TIME;
			static const std::string ATTR_END_TIME;
			static const std::string ATTR_DRIVER_END_TIME;
			static const std::string ATTR_START_STOP;
			static const std::string ATTR_END_STOP;
			static const std::string ATTR_WORK_DURATION;
			static const std::string ATTR_WORK_RANGE;
			static const std::string ATTR_SERVICE_ID;
			static const std::string ATTR_CLASS;
			static const std::string VALUE_DEAD_RUN;
			static const std::string VALUE_COMMERCIAL;

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
				boost::posix_time::time_duration driverStartTime;
				boost::posix_time::time_duration driverEndTime;

				Chunk(
					VehicleService* _vehicleService = NULL
				);
				
				Chunk(
					DriverService* _driverService,
					VehicleService& _vehicleService,
					const boost::gregorian::date& date,
					const boost::posix_time::time_duration& startTime,
					const boost::posix_time::time_duration& endTime,
					const boost::posix_time::time_duration& startTimeD,
					const boost::posix_time::time_duration& endTimeD
				);

				boost::posix_time::time_duration getDriverStartTime() const;
				boost::posix_time::time_duration getDriverEndTime() const;
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

			/// @name Services
			//@{
				void toParametersMap(
					util::ParametersMap& map,
					bool recursive = true,
					const VehicleService* vehicleServiceFilter = NULL
				) const;
				boost::posix_time::time_duration getWorkRange() const;
				boost::posix_time::time_duration getWorkDuration() const;
				boost::posix_time::time_duration getServiceBeginning() const;
				boost::posix_time::time_duration getServiceEnd() const;
			//@}
		};
}	}

#endif // SYNTHESE_pt_operation_DriverService_hpp__
