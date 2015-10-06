
/** VehicleService class header.
	@file VehicleService.hpp

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

#ifndef SYNTHESE_pt_operations_VehicleService_hpp__
#define SYNTHESE_pt_operations_VehicleService_hpp__

#include "Calendar.h"
#include "CalendarField.hpp"
#include "CastPointersVectorField.hpp"
#include "DataSourceLinksField.hpp"
#include "MinutesField.hpp"
#include "ImportableTemplate.hpp"
#include "OperationUnit.hpp"
#include "PointersVectorField.hpp"
#include "Registrable.h"
#include "Registry.h"
#include "DriverService.hpp"
#include "SchedulesBasedService.h"

#include "FrameworkTypes.hpp"

#include <set>
#include <vector>

namespace synthese
{
	FIELD_CAST_POINTERS_VECTOR(Services, pt::SchedulesBasedService)
	FIELD_DATASOURCE_LINKS(DataSourceLinksWithoutUnderscore)
	FIELD_MINUTES(OpeningDuration)
	FIELD_MINUTES(ClosingDuration)

	typedef boost::fusion::map<
		FIELD(Key),
		FIELD(Name),
		FIELD(Services),
		FIELD(DataSourceLinksWithoutUnderscore),
		FIELD(Dates),
		FIELD(pt_operation::OperationUnit),
		FIELD(OpeningDuration),
		FIELD(ClosingDuration)
	> VehicleServiceSchema;

	namespace pt_operation
	{
		/** Vehicle service class.
			@ingroup m37
		*/
		class VehicleService:
			public Object<VehicleService, VehicleServiceSchema>,
			public calendar::Calendar,
			public impex::ImportableTemplate<VehicleService>
		{
		public:
			static const std::string TAG_SERVICE;
			static const std::string ATTR_CLASS;
			static const std::string VALUE_DEAD_RUN;
			static const std::string VALUE_COMMERCIAL;

			struct Vector:
				public PointersVectorField<Vector, VehicleService>
			{

			};

			class DriverServiceChunkCompare
			{
			public:
				bool operator()(const DriverService::Chunk* ds1, const DriverService::Chunk* ds2) const;
			};

			typedef std::set<const DriverService::Chunk*, DriverServiceChunkCompare> DriverServiceChunks;

		private:
			mutable DriverServiceChunks _driverServiceChunks;

		public:
			VehicleService(util::RegistryKeyType id=0);
			~VehicleService();

			//! @name Updaters
			//@{
				void addDriverServiceChunk(const DriverService::Chunk& value);
				void removeDriverServiceChunk(const DriverService::Chunk& value);
				void insert(pt::SchedulesBasedService& value);
				void remove(pt::SchedulesBasedService& value);
				void clearServices();
				void setDriverServiceChunks(const DriverServiceChunks& value) const { _driverServiceChunks = value; }
			//@}

			//! @name Services
			//@{
				const DriverServiceChunks& getDriverServiceChunks() const { return _driverServiceChunks; }

				virtual bool isActive(
					const boost::gregorian::date& date
				) const;

				//////////////////////////////////////////////////////////////////////////
				/// Service extractor.
				/// @param rank the rank of the service in the vehicle service
				/// @return the nth service of the vehicle service, NULL if non existent
				pt::SchedulesBasedService* getService(std::size_t rank) const;

				virtual void link(util::Env& env, bool withAlgorithmOptimizations = false);
				virtual void unlink();

				virtual void addAdditionalParameters(
					util::ParametersMap& map,
					std::string prefix = std::string()
				) const;
			//@}

			virtual bool allowUpdate(const server::Session* session) const;
			virtual bool allowCreate(const server::Session* session) const;
			virtual bool allowDelete(const server::Session* session) const;
		};
}	}

#endif // SYNTHESE_pt_operations_Vehicle_hpp__
