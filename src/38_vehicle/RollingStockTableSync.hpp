
/** RollingStockTableSync class header.
	@file RollingStockTableSync.hpp
	@author Hugues Romain
	@date 2007

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

#ifndef SYNTHESE_RollingStockTableSync_H__
#define SYNTHESE_RollingStockTableSync_H__


#include <vector>
#include <string>
#include <iostream>

#include "RollingStock.hpp"
#include "DBDirectTableSyncTemplate.hpp"
#include "OldLoadSavePolicy.hpp"

#include <boost/optional.hpp>

namespace synthese
{
	namespace vehicle
	{
		//////////////////////////////////////////////////////////////////////////
		/// 38.10 Table : Vehicle types.
		///	@ingroup m35LS refLS
		/// @author Hugues Romain
		/// @date 2007
		//////////////////////////////////////////////////////////////////////////
		/// See : RollingStock
		class RollingStockTableSync:
			public db::DBDirectTableSyncTemplate<
				RollingStockTableSync,
				RollingStock,
				db::FullSynchronizationPolicy,
				db::OldLoadSavePolicy
			>
		{
		public:
			static const std::string COL_NAME;
			static const std::string COL_ARTICLE;
			static const std::string COL_INDICATOR;
			static const std::string COL_TRIDENT;
			static const std::string COL_IS_TRIDENT_REFERENCE;
			static const std::string COL_CO2_EMISSIONS;
			static const std::string COL_ENERGY_CONSUMPTION;
			static const std::string COL_DATASOURCE_LINKS;

			RollingStockTableSync() {}


			//////////////////////////////////////////////////////////////////////////
			/// Vehicle types search.
			/// @param env environment to populate when loading objects
			/// @param tridentKey filter on trident key (exact search)
			/// @param tridentReference filter on trident reference attribute
			/// @param orderByName order results by vehicle type name
			/// @param raisingOrder order results ascendantly
			/// @param first First RollingStock object to answer
			///	@param number Number of objects to return (0 = all) The size of the vector is less or equal to number, then all users were returned despite of the number limit. If the size is greater than number (actually equal to number + 1) then there is others accounts to show. Test it to know if the situation needs a "click for more" button.
			/// @param linkLevel level of link to use when loading objects
			///	@return the found vehicle types.
			///	@author Hugues Romain
			///	@date 2007
			static SearchResult Search(
				util::Env& env,
				boost::optional<std::string> tridentKey = boost::optional<std::string>(),
				bool tridentReference = false,
				bool orderByName = true,
				bool raisingOrder = true,
				int first = 0,
				int number = 0,
				util::LinkLevel linkLevel = util::UP_LINKS_LOAD_LEVEL
			);


			typedef std::vector<std::pair<boost::optional<util::RegistryKeyType>, std::string> > Labels;

			///////////////////////////////////////////////////////////////////////////
			/// Generates a list of transport modes to be used by select input.
			/// @param unknownLabel label of the undefined mode option. Empty = no undefined option
			/// @return list of transport modes ordered by name.
			/// @author Hugues Romain
			/// @since 3.1.16
			/// @date 2010
			static Labels GetLabels(
				std::string unknownLabel = "(inconnu)"
			);

			db::RowsList SearchForAutoComplete(
				const boost::optional<std::string> prefix,
				const boost::optional<std::size_t> limit,
				const boost::optional<std::string> optionalParameter) const;
		};
	}
}

#endif // SYNTHESE_RollingStockTableSync_H__
