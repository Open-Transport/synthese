
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
#include "ImportableTemplate.hpp"
#include "Registrable.h"
#include "Registry.h"
#include "DriverService.hpp"

#include "FrameworkTypes.hpp"

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
		class DriverService;

		/** Vehicle service class.
			@ingroup m37
		*/
		class VehicleService:
			public calendar::Calendar,
			public impex::ImportableTemplate<VehicleService>,
			public virtual util::Registrable
		{
		public:
			static const std::string TAG_SERVICE;
			static const std::string ATTR_CLASS;
			static const std::string VALUE_DEAD_RUN;
			static const std::string VALUE_COMMERCIAL;

			class DriverServiceChunkCompare
			{
			public:
				bool operator()(const DriverService::Chunk* ds1, const DriverService::Chunk* ds2) const;
			};

			typedef util::Registry<VehicleService> Registry;
			typedef std::vector<pt::SchedulesBasedService*> Services;
			typedef std::set<const DriverService::Chunk*, DriverServiceChunkCompare> DriverServiceChunks;

		private:
			Services _services;
			DriverServiceChunks _driverServiceChunks;
			std::string _name;

		public:
			VehicleService(util::RegistryKeyType id=0);

			//! @name Setters
			//@{
				void setServices(const Services& value){ _services = value; }
				void setName(const std::string& value){ _name = value; }
				void setDriverServices(const DriverServiceChunks& value){ _driverServiceChunks = value; }
			//@}

			//! @name Getters
			//@{
				const Services& getServices() const { return _services; }
				virtual std::string getName() const { return _name; }
				const DriverServiceChunks& getDriverServiceChunks() const { return _driverServiceChunks; }
			//@}

			//! @name Updaters
			//@{
				void addDriverServiceChunk(const DriverService::Chunk& value);
				void removeDriverServiceChunk(const DriverService::Chunk& value);
				void insert(pt::SchedulesBasedService& value);
				void clearServices();
			//@}

			//! @name Services
			//@{
				//////////////////////////////////////////////////////////////////////////
				/// Service extractor.
				/// @param rank the rank of the service in the vehicle service
				/// @return the nth service of the vehicle service, NULL if non existent
				pt::SchedulesBasedService* getService(std::size_t rank) const;

				virtual void toParametersMap(
					util::ParametersMap& pm,
					bool withAdditionalParameters,
					boost::logic::tribool withFiles = boost::logic::indeterminate,
					std::string prefix = std::string()
				) const;

				
				virtual bool loadFromRecord(
					const Record& record,
					util::Env& env
				);

				virtual void link(util::Env& env, bool withAlgorithmOptimizations = false);

				virtual LinkedObjectsIds getLinkedObjectsIds(
					const Record& record
				) const;
			//@}
		};
}	}

#endif // SYNTHESE_pt_operations_Vehicle_hpp__
