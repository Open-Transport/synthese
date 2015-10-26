
//////////////////////////////////////////////////////////////////////////
///	DepotTableSync class header.
///	@file DepotTableSync.hpp
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

#ifndef SYNTHESE_DepotTableSync_hpp__
#define SYNTHESE_DepotTableSync_hpp__

#include "Depot.hpp"
#include "DBDirectTableSyncTemplate.hpp"

namespace synthese
{
	namespace pt_operation
	{
		//////////////////////////////////////////////////////////////////////////
		///	Depot table synchronizer.
		///	@ingroup m37LS refLS
		///	@author Hugues Romain
		///	@date 2011
		/// @since 3.3.0
		class DepotTableSync:
			public db::DBDirectTableSyncTemplate<
				DepotTableSync,
				Depot,
				db::FullSynchronizationPolicy
			>
		{
		public:
			typedef std::vector<std::pair<boost::optional<util::RegistryKeyType>, std::string> > DepotsList;

			//! @name Services
			//@{
				//////////////////////////////////////////////////////////////////////////
				///	Depot search.
				///	@param env Environment to populate
				///	@param parameterId optional ID of a foreign key to filter on (deactivated if undefined)
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
					boost::optional<std::string> name = boost::optional<std::string>(),
					std::size_t first = 0,
					boost::optional<std::size_t> number = boost::optional<std::size_t>(),
					bool orderByName = true,
					bool raisingOrder = true,
					util::LinkLevel linkLevel = util::UP_LINKS_LOAD_LEVEL
				);

				static DepotsList GetDepotsList(
					util::Env& env,
					boost::optional<std::string> noDepotLabel
				);
			//@}

			virtual bool allowList( const server::Session* session ) const;
		};
	}
}

#endif // SYNTHESE_DepotTableSync_hpp__
