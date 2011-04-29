
//////////////////////////////////////////////////////////////////////////
///	JunctionTableSync class header.
///	@file JunctionTableSync.hpp
///	@author Hugues Romain
///	@date 2010
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

#ifndef SYNTHESE_JunctionTableSync_hpp__
#define SYNTHESE_JunctionTableSync_hpp__

#include "Junction.hpp"
#include "DBRegistryTableSyncTemplate.hpp"

namespace synthese
{
	namespace pt
	{
		//////////////////////////////////////////////////////////////////////////
		///	Junction table synchronizer.
		///	@ingroup m35LS refLS
		///	@author Hugues Romain
		///	@date 2010
		class JunctionTableSync:
			public db::DBRegistryTableSyncTemplate<JunctionTableSync,Junction>
		{
		public:
			//! @name Field names
			//@{
				static const std::string COL_START_PHYSICAL_STOP_ID;
				static const std::string COL_END_PHYSICAL_STOP_ID;
				static const std::string COL_LENGTH;
				static const std::string COL_DURATION;
				static const std::string COL_BIDIRECTIONAL;
			//@}


			//! @name Services
			//@{
				//////////////////////////////////////////////////////////////////////////
				///	Junction search.
				///	@param env Environment to populate
				///	@param startStopFilter optional ID of the start stop of the junction (deactivated if undefined)
				///	@param endStopFilter optional ID of the end stop of the junction (deactivated if undefined)
				///	@param first First  object to answer
				///	@param number Number of  objects to answer (0 = all) The size of the vector is less or equal to number, then all users were returned despite of the number limit. If the size is greater than number (actually equal to number + 1) then there is others accounts to show. Test it to know if the situation needs a "click for more" button.
				///	@param orderByName The result records must be ordered by their name
				///	@param raisingOrder The result records must be sorted ascendantly
				///	@param linkLevel Level of links to build when reading foreign keys
				///	@return Found objects.
				///	@author Hugues Romain
				///	@date 2010
				static SearchResult Search(
					util::Env& env,
					boost::optional<util::RegistryKeyType> startStopFilter = boost::optional<util::RegistryKeyType>(),
					boost::optional<util::RegistryKeyType> endStopFilter = boost::optional<util::RegistryKeyType>(),
					std::size_t first = 0,
					boost::optional<std::size_t> number = boost::optional<std::size_t>(),
					bool orderByStop = true,
					bool raisingOrder = true,
					util::LinkLevel linkLevel = util::UP_LINKS_LOAD_LEVEL
				);
			//@}
		};
	}
}

#endif // SYNTHESE_JunctionTableSync_hpp__
