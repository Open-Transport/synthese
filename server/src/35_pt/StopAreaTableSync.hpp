
/** StopAreaTableSync class header.
	@file StopAreaTableSync.hpp

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

#ifndef SYNTHESE_ENVLSSQL_CONNECTIONPLACETABLESYNC_H
#define SYNTHESE_ENVLSSQL_CONNECTIONPLACETABLESYNC_H

#include "StopArea.hpp"

#include "DBDirectTableSyncTemplate.hpp"
#include "FetcherTemplate.h"

#include <string>
#include <iostream>

namespace synthese
{
	namespace pt
	{
		/** ConnectionPlace table synchronizer.
			@ingroup m35LS refLS

			Connection places table :
				- on insert :
				- on update :
				- on delete : X
		*/
		class StopAreaTableSync:
			public db::DBDirectTableSyncTemplate<
				StopAreaTableSync,
				StopArea,
				db::FullSynchronizationPolicy
			>,
			public db::FetcherTemplate<geography::NamedPlace, StopAreaTableSync>
		{
		public:
			static const std::string FORBIDDEN_DELAY_SYMBOL;


			//////////////////////////////////////////////////////////////////////////
			/// Commercial stop point search.
			/// @param env Environment to populate
			/// @param cityId id of the city the returned stops must belong.
			/// @param mainConnection the returned stops must belong to the list of main stops of the city
			/// @param creatorIdFilter search of a stop by creator ID
			/// @param nameFilter search of a stop by name (uses the LIKE operator)
			/// @param cityNameFilter search of a stop by the name of its city (uses the LIKE operator)
			/// @param orderByCityNameAndName order the results by city name, then by stop name
			/// @param raisingOrder order the results ascendantly
			/// @param first rank of the first result to return
			/// @param number maximal number of results to return
			/// @param linkLevel level of link to follow when loading data of the returned objects
			/// @return the commercial stop points whose respect the search criterias
			/// @author Hugues Romain
			static SearchResult Search(
				util::Env& env,
				boost::optional<util::RegistryKeyType> cityId = boost::optional<util::RegistryKeyType>(),
				boost::logic::tribool mainConnection = boost::logic::indeterminate,
				boost::optional<std::string> creatorIdFilter = boost::optional<std::string>(),
				boost::optional<std::string> nameFilter = boost::optional<std::string>(),
				boost::optional<std::string> cityNameFilter = boost::optional<std::string>(),
				bool orderByCityNameAndName = true
				, bool raisingOrder = true
				, int first = 0
				, int number = 0
				, util::LinkLevel linkLevel = util::UP_LINKS_LOAD_LEVEL
			);

			
			
			static SearchResult SearchSameName(
				StopArea const& stoparea, 
				util::Env& env,
				util::LinkLevel linkLevel
			);

			virtual bool allowList( const server::Session* session ) const;
		};
	}
}
#endif
