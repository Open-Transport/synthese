
/** TransportNetworkTableSync class header.
	@file TransportNetworkTableSync.h

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

#ifndef SYNTHESE_ENVLSSQL_TRANSPORTNETWORKTABLESYNC_H
#define SYNTHESE_ENVLSSQL_TRANSPORTNETWORKTABLESYNC_H

#include <string>
#include <iostream>

#include "TransportNetwork.h"

#include "DBDirectTableSyncTemplate.hpp"
#include "OldLoadSavePolicy.hpp"

namespace synthese
{
	namespace pt
	{
		//////////////////////////////////////////////////////////////////////////
		/// 35.10 Table : Transport network.
		///	@ingroup m35LS refLS
		/// @author Hugues Romain
		/// @date 2006
		//////////////////////////////////////////////////////////////////////////
		class TransportNetworkTableSync:
			public db::DBDirectTableSyncTemplate<
				TransportNetworkTableSync,
				TransportNetwork,
				db::FullSynchronizationPolicy,
				db::OldLoadSavePolicy
			>
		{
		 public:
			static const std::string COL_NAME;
			static const std::string COL_CREATOR_ID;
			static const std::string COL_DAYS_CALENDARS_PARENT_ID;
			static const std::string COL_PERIODS_CALENDARS_PARENT_ID;
			static const std::string COL_IMAGE;
			static const std::string COL_TIMEZONE;
			static const std::string COL_LANG;
			static const std::string COL_CONTACT_ID;
			static const std::string COL_FARE_CONTACT_ID;
			static const std::string COL_COUNTRY_CODE;



			/** Network search.
				@param name Part of the name of the server
				@param first First JourneyPattern object to answer
				@param number Number of JourneyPattern objects to answer (0 = all) The size of the vector is less
					or equal to number, then all users were returned despite of the number limit. If the
					size is greater than number (actually equal to number + 1) then there is others accounts
					to show. Test it to know if the situation needs a "click for more" button.
				@return Found network objects.
				@author Hugues Romain
				@date 2007
			*/
			static SearchResult Search(
				util::Env& env,
				std::string name = std::string(),
				std::string creatorId = std::string()
				, int first = 0
				, boost::optional<std::size_t> number = boost::optional<std::size_t>()
				, bool orderByName = true
				, bool raisingOrder = true,
				util::LinkLevel linkLevel = util::UP_LINKS_LOAD_LEVEL
			);



			/** Network search for AutoComplete.
				@param prefix Part of the name of the transport network
				@param limit Number of results to answer. The size of the vector is less
					or equal to number
				@return Found networks (object id and name).
				@author Gaël Sauvanet
				@date 2012
			*/
			db::RowsList SearchForAutoComplete(
				const boost::optional<std::string> prefix,
				const boost::optional<std::size_t> limit,
				const boost::optional<std::string> optionalParameter) const;
		};
	}
}

#endif
