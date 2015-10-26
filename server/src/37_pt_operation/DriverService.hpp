
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

#include "Object.hpp"

#include "Calendar.h"
#include "DataSourceLinksField.hpp"
#include "ImportableTemplate.hpp"
#include "PointerField.hpp"
#include "PointersVectorField.hpp"

namespace synthese
{
	namespace pt
	{
		class SchedulesBasedService;
	}

	namespace pt_operation
	{
		class DriverActivity;
		class OperationUnit;
		class VehicleService;

		FIELD_STRING(DriverServiceServices)
		FIELD_STRING(DriverServiceDates)
		FIELD_DATASOURCE_LINKS(DriverServiceDataSource)
		FIELD_POINTER(DriverServiceOperationUnit, OperationUnit)

		typedef boost::fusion::map<
			FIELD(Key),
			FIELD(Name),
			FIELD(DriverServiceServices),
			FIELD(DriverServiceDates),
			FIELD(DriverServiceDataSource),
			FIELD(DriverServiceOperationUnit)
		> DriverServiceSchema;

		/** DriverService class.
			@ingroup m37
		*/
		class DriverService:
			public impex::ImportableTemplate<DriverService>,
			public calendar::Calendar,
			public Object<DriverService, DriverServiceSchema>
		{
		public:
			static const std::string TAG_CHUNK;
			static const std::string TAG_ELEMENT;
			static const std::string TAG_VEHICLE_SERVICE;
			static const std::string TAG_ACTIVITY;
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

			struct Vector:
				public PointersVectorField<Vector, DriverService>
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
				DriverActivity* activity;
				std::vector<Element> elements;
				boost::posix_time::time_duration driverStartTime;
				boost::posix_time::time_duration driverEndTime;

				Chunk(
					VehicleService* _vehicleService = NULL
				);

				Chunk(
					DriverService* _driverService,
					VehicleService* _vehicleService,
					DriverActivity* _activity,
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
				virtual std::string getName() const { return get<Name>(); }
				const boost::optional<OperationUnit&> getOperationUnit() const;
			//@}

			//! @name Setters
			//@{
				void setChunks(const Chunks& value);
				void setName(const std::string& value){ set<Name>(value); }
				void setOperationUnit(const boost::optional<OperationUnit&>& value);
			//@}

			/// @name Services
			//@{
				void toParametersMap(
					util::ParametersMap& map,
					bool recursive,
					const VehicleService* vehicleServiceFilter
				) const;

				virtual void toParametersMap(
					util::ParametersMap& pm,
					bool withAdditionalParameters,
					boost::logic::tribool withFiles = boost::logic::indeterminate,
					std::string prefix = std::string()
				) const;

				boost::posix_time::time_duration getWorkRange() const;
				boost::posix_time::time_duration getWorkDuration() const;
				boost::posix_time::time_duration getServiceBeginning() const;
				boost::posix_time::time_duration getServiceEnd() const;

				virtual void link(util::Env& env, bool withAlgorithmOptimizations = false);
				virtual void unlink();

				static std::string SerializeServices(
					const DriverService::Chunks& services
				);

				static DriverService::Chunks UnserializeServices(
					const std::string& value,
					util::Env& env,
					util::LinkLevel linkLevel = util::UP_LINKS_LOAD_LEVEL
				);
			//@}

			virtual bool allowUpdate(const server::Session* session) const;
			virtual bool allowCreate(const server::Session* session) const;
			virtual bool allowDelete(const server::Session* session) const;
		};
}	}

#endif // SYNTHESE_pt_operation_DriverService_hpp__
