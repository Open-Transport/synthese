
//////////////////////////////////////////////////////////////////////////
///	DeadRunTableSync class header.
///	@file DeadRunTableSync.hpp
///	@author Hugues Romain
///	@date 2011
///
///	This file belongs to the SYNTHESE project (public transportation specialized software)
///	Copyright (C) 2002 Hugues Romain - RCSmobility <contact@rcsmobility.com>
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

#ifndef SYNTHESE_DeadRunTableSync_hpp__
#define SYNTHESE_DeadRunTableSync_hpp__

#include "DBRegistryTableSyncTemplate.hpp"
#include "DeadRun.hpp"

namespace synthese
{
	namespace pt_operation
	{
		//////////////////////////////////////////////////////////////////////////
		///	Dead run table synchronizer.
		///	@ingroup m37LS refLS
		///	@author Hugues Romain
		///	@date 2011
		/// @since 3.3.0
		class DeadRunTableSync:
			public db::DBRegistryTableSyncTemplate<DeadRunTableSync,DeadRun>
		{
		public:
			//! @name Field names
			//@{
				static const std::string COL_NETWORK_ID;
				static const std::string COL_DEPOT_ID;
				static const std::string COL_STOP_ID;
				static const std::string COL_DIRECTION;
				static const std::string COL_SCHEDULES;
				static const std::string COL_DATES;
				static const std::string COL_SERVICE_NUMBER;
				static const std::string COL_LENGTH;
				static const std::string COL_DATASOURCE_LINKS;
			//@}

			//! @name Services
			//@{
				//////////////////////////////////////////////////////////////////////////
				///	Dead run search.
				///	@param env Environment to populate
				///	@param first First  object to answer
				///	@param number Number of  objects to answer (0 = all) The size of the vector is less or equal to number, then all users were returned despite of the number limit. If the size is greater than number (actually equal to number + 1) then there is others accounts to show. Test it to know if the situation needs a "click for more" button.
				///	@param orderByName The result records must be ordered by their name
				///	@param raisingOrder The result records must be sorted ascendantly
				///	@param linkLevel Level of links to build when reading foreign keys
				///	@return Found objects.
				///	@author Hugues Romain
				///	@date 2011
				/// @since 3.3.0
				static SearchResult Search(
					util::Env& env,
					boost::optional<util::RegistryKeyType> networkId = boost::optional<util::RegistryKeyType>(),
					std::size_t first = 0,
					boost::optional<std::size_t> number = boost::optional<std::size_t>(),
					bool orderByName = true,
					bool raisingOrder = true,
					util::LinkLevel linkLevel = util::UP_LINKS_LOAD_LEVEL
				);
			//@}
		};
	}
}

#endif
