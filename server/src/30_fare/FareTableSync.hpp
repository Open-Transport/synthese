
/** FareTableSync class header.
	@file FareTableSync.hpp

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

#ifndef SYNTHESE_FareTableSync_H__
#define SYNTHESE_FareTableSync_H__


#include <vector>
#include <string>
#include <iostream>

#include "Fare.hpp"
#include "FareType.hpp"
#include "DBDirectTableSyncTemplate.hpp"

namespace synthese
{
	namespace fare
	{
		//////////////////////////////////////////////////////////////////////////
		/// 30.10 Table : Fare.
		///	@ingroup m30LS refLS
		/// @author Hugues Romain
		/// @date 2006
		//////////////////////////////////////////////////////////////////////////
		/// Synchronization : permanent.
		class FareTableSync:
			public db::DBDirectTableSyncTemplate<
				FareTableSync,
				Fare
			>
		{
		public:
			//////////////////////////////////////////////////////////////////////////
			/// Fare search.
			/// @param env Environment to populate when loading objects
			/// @param name Filter on fare name (LIKE operator)
			/// @param orderByName Order results by fare name
			/// @param raisingOrder Ordering is ascendantly
			///	@param first First Fare object to answer
			///	@param number Number of Fare objects to answer (0 = all) The size of the vector is less or equal to number, then all users were returned despite of the number limit. If the size is greater than number (actually equal to number + 1) then there is others accounts to show. Test it to know if the situation needs a "click for more" button.
			///	@param linkLevel Level of link to use when loading objects.
			///	@return Found Fare objects.
			///	@author Hugues Romain
			///	@date 2006
			static SearchResult Search(
				util::Env& env,
				boost::optional<std::string> name = boost::optional<std::string>(),
				bool orderByName = true,
				bool raisingOrder = true,
				int first = 0,
				boost::optional<std::size_t> number = boost::optional<std::size_t>(),
				util::LinkLevel linkLevel = util::UP_LINKS_LOAD_LEVEL
			);



			typedef std::vector<std::pair<boost::optional<util::RegistryKeyType>, std::string> > NamedList;



			//////////////////////////////////////////////////////////////////////////
			/// Gets fares list.
			/// @param env Environment to populate when loading objects
			/// @return The list of fares id and names, ordered alphabetically.
			/// @author Hugues Romain
			/// @date 2010
			/// @since 3.1.16
			//////////////////////////////////////////////////////////////////////////
			/// Designed to be used by HTMLForm::getSelectInput.
			static NamedList GetList(
				util::Env& env
			);

			virtual bool allowList( const server::Session* session ) const;
		};
	}
}

#endif // SYNTHESE_FareTableSync_H__

