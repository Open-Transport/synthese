
/** PTOperationFileFormat class header.
	@file PTOperationFileFormat.hpp

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

#ifndef SYNTHESE_pt_operation_PTOperationFileFormat_hpp__
#define SYNTHESE_pt_operation_PTOperationFileFormat_hpp__

#include "Importer.hpp"

#include "ImportableTableSync.hpp"

namespace synthese
{
	namespace pt_operation
	{
		class VehicleService;
		class VehicleServiceTableSync;
		class DriverAllocation;
		class DriverAllocationTableSync;
	}

	namespace data_exchange
	{
		/** PTOperationFileFormat class.
			@ingroup m37
		*/
		class PTOperationFileFormat:
			public virtual impex::Importer
		{
		public:

			PTOperationFileFormat(
				util::Env& env,
				const impex::Import& import,
				impex::ImportLogLevel minLogLevel,
				const std::string& logPath,
				boost::optional<std::ostream&> outputStream,
				util::ParametersMap& pm
			);



			pt_operation::VehicleService* _createOrUpdateVehicleService(
				impex::ImportableTableSync::ObjectBySource<pt_operation::VehicleServiceTableSync>& vehicleServices,
				const std::string& id
			) const;
		};
	}
}

#endif // SYNTHESE_pt_operation_PTOperationFileFormat_hpp__

