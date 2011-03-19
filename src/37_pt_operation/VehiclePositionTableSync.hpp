
//////////////////////////////////////////////////////////////////////////
///	VehiclePositionTableSync class header.
///	@file VehiclePositionTableSync.hpp
///	@author RCSobility
///	@date 2011
///
///	This file belongs to the SYNTHESE project (public transportation specialized software)
///	Copyright (C) 2002 Hugues Romain - RCS <contact@reseaux-conseil.com>
///
///	This program is free software; you can redistribute it and/or
///	modify it under the terms of the GNU General Public License
///	as published by the Free Software Foundation; either version 2
///	of the License, or (at your option) any later version.
///
///	This program is distributed in the hope that it will be useful,
///	but WITHOUT ANY WARRANTY; without even the implied warranty of
///	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
///	GNU General Public License for more details.
///
///	You should have received a copy of the GNU General Public License
///	along with this program; if not, write to the Free Software
///	Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.

#ifndef SYNTHESE_VehiclePositionTableSync_hpp__
#define SYNTHESE_VehiclePositionTableSync_hpp__

#include "VehiclePosition.hpp"
#include "SQLiteNoSyncTableSyncTemplate.h"

namespace synthese
{
	namespace pt_operation
	{
		//////////////////////////////////////////////////////////////////////////
		///	VehiclePosition table synchronizer.
		///	@ingroup m37LS refLS
		///	@author RCSobility
		///	@date 2011
		/// @since 3.2.1
		class VehiclePositionTableSync:
			public db::SQLiteNoSyncTableSyncTemplate<VehiclePositionTableSync,VehiclePosition>
		{
		public:
			//! @name Field names
			//@{
				static const std::string COL_STATUS;
				static const std::string COL_VEHICLE_ID;
				static const std::string COL_TIME;
				static const std::string COL_METER_OFFSET;
				static const std::string COL_STOP_POINT_ID;
				static const std::string COL_COMMENT;
				static const std::string COL_SERVICE_ID;
				static const std::string COL_RANK_IN_PATH;
				static const std::string COL_PASSENGERS;
				static const std::string COL_GEOMETRY;
			//@}
			

			//! @name Services
			//@{		
				//////////////////////////////////////////////////////////////////////////
				///	VehiclePosition search.
				///	@param env Environment to populate
				///	@param parameterId optional ID of a foreign key to filter on (deactivated if undefined)
				///	@param first First  object to answer
				///	@param number Number of  objects to answer (0 = all) The size of the vector is less or equal to number, then all users were returned despite of the number limit. If the size is greater than number (actually equal to number + 1) then there is others accounts to show. Test it to know if the situation needs a "click for more" button.
				///	@param orderByName The result records must be ordered by their name
				///	@param raisingOrder The result records must be sorted ascendantly
				///	@param linkLevel Level of links to build when reading foreign keys
				///	@return Found objects.
				///	@author RCSobility
				///	@date 2011
				/// @since 3.2.1
				static SearchResult Search(
					util::Env& env,
					// boost::optional<util::RegistryKeyType> parameterId = boost::optional<util::RegistryKeyType>(),
					std::size_t first = 0,
					boost::optional<std::size_t> number = boost::optional<std::size_t>(),
					bool orderByName = true,
					bool raisingOrder = true,
					util::LinkLevel linkLevel = util::FIELDS_ONLY_LOAD_LEVEL
				);
			//@}
		};
	}
}

#endif // SYNTHESE_VehiclePositionTableSync_hpp__
