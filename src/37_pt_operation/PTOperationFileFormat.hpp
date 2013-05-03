
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

#include "ImportableTableSync.hpp"

namespace synthese
{
	namespace impex
	{
		class ImportLogger;
	}

	namespace pt_operation
	{
		class VehicleService;
		class VehicleServiceTableSync;
		class DriverAllocation;
		class DriverAllocationTableSync;

		/** PTOperationFileFormat class.
			@ingroup m37
		*/
		class PTOperationFileFormat
		{
		public:

			//////////////////////////////////////////////////////////////////////////
			/// @return the created network object.
			/// The created object is owned by the environment (it is not required to
			/// maintain the returned shared pointer)
			static VehicleService* CreateOrUpdateVehicleService(
				impex::ImportableTableSync::ObjectBySource<VehicleServiceTableSync>& vehicleServices,
				const std::string& id,
				const impex::DataSource& source,
				util::Env& env,
				const impex::ImportLogger& logger
			);
		};
	}
}

#endif // SYNTHESE_pt_operation_PTOperationFileFormat_hpp__

