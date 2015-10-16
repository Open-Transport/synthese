
/** CommercialLineTableSync class header.
	@file CommercialLineTableSync.h

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

#ifndef SYNTHESE_CommercialLineTableSync_H__
#define SYNTHESE_CommercialLineTableSync_H__

// Env
#include "CommercialLine.h"

// Std
#include <vector>
#include <string>
#include <iostream>

// Db
#include "DBDirectTableSyncTemplate.hpp"

// Security
#include "SecurityTypes.hpp"

#include <boost/optional.hpp>

namespace synthese
{
	namespace pt
	{
		/** Commercial JourneyPattern table synchronizer.
			@ingroup m35LS refLS
		*/
		class CommercialLineTableSync:
			public db::DBDirectTableSyncTemplate<
				CommercialLineTableSync,
				CommercialLine,
				db::FullSynchronizationPolicy
			>
		{
		public:
			/** CommercialLine search.
				@param networkId Id of the network which the lines must belong (default = undefined = all networks)
				@param name SQL LIKE mask that line names must respect (default = "%" = all names). Use % and ? to specify jokers.
				@param first First CommercialLine object to answer
				@param number Number of CommercialLine objects to answer (0 = all) The size of the vector is less or equal to number, then all users were returned despite of the number limit. If the size is greater than number (actually equal to number + 1) then there is others accounts to show. Test it to know if the situation needs a "click for more" button.
				@param orderByNetwork Order the results by network name, and by line name (default = true)
				@param orderByName Order the result by line name (default = false)
				@param raisingOrder true = ascendant order, false = descendant order (default = true)
				@return Found CommercialLine objects.
				@author Hugues Romain
				@date 2006
			*/
			static SearchResult Search(
				util::Env& env,
				boost::optional<util::RegistryKeyType> parentId = boost::optional<util::RegistryKeyType>(),
				boost::optional<std::string> name = boost::optional<std::string>(),
				boost::optional<std::string> creatorId = boost::optional<std::string>(),
				int first = 0,
				boost::optional<std::size_t> number = boost::optional<std::size_t>(),
				bool orderByNetwork = true,
				bool orderByName = false,
				bool raisingOrder = true,
				util::LinkLevel linkLevel = util::UP_LINKS_LOAD_LEVEL,
				boost::optional<const security::RightsOfSameClassMap&> rights = boost::optional<const security::RightsOfSameClassMap&>(),
				security::RightLevel neededLevel = security::FORBIDDEN,
				boost::optional<util::RegistryKeyType> contactCenterId = boost::optional<util::RegistryKeyType>()
			);



			/** CommercialLine search.
				@param rights Rights vector defining the profile, see getSQLLinesList
				@param totalControl Default authorization level
				@param neededLevel Right level to reach to be in the results
				@param first First CommercialLine object to answer
				@param number Number of CommercialLine objects to answer (0 = all) The size of the vector is less or equal to number, then all users were returned despite of the number limit. If the size is greater than number (actually equal to number + 1) then there is others accounts to show. Test it to know if the situation needs a "click for more" button.
				@param orderByNetwork Orders the result by network, name
				@param orderByName Orders the result by name
				@param raisingOrder true = ascendant order, false = descendant order
				@param mustbeBookable true = only demand responsive lines
				@return Found CommercialLine objects.
				@author Hugues Romain
				@date 2006

				To use the method, specify the rights like this :

				@code
rights = request->getUser()->getProfile()->getRightsForModuleClass<ArrivalDepartureTableRight>()
totalControl = request->getUser()->getProfile()->getGlobalPublicRight<ArrivalDepartureTableRight>() >= READ
neededLevel = READ
				@endcode
			*/
			static SearchResult Search(
				util::Env& env,
				const security::RightsOfSameClassMap& rights
				, bool totalControl
				, security::RightLevel neededLevel
				, int first = 0
				, boost::optional<std::size_t> number = boost::optional<std::size_t>()
				, bool orderByNetwork = true
				, bool orderByName = false
				, bool raisingOrder = true
				, bool mustBeBookable = false
				, util::LinkLevel linkLevel = util::UP_LINKS_LOAD_LEVEL
			);



			/** Build of a SQL query selecting all the lines allowed by a profile.
				@param rights Rights vector defining the profile
				@param totalControl Default authorization level
				@param neededLevel Right level to reach to be in the results

				The right policy is defined by two right families :
					- Rights on a network
					- Rights on a line

				If the default authorization level is true, the selected lines are in increasing priority order :
					- all the lines
					- except all the lines of a forbidden network
					- and additionally all the specific allowed lines
					- except all the specific forbidden lines

				In this case, an allowed network is useless because the right is covered by the general authorization.

				If the default authorization level is false, the selected lines are in increasing priority order :
					- all the lines of allowed networks
					- and additionally all the specific allowed lines
					- except all the specific forbidden lines

				In this case, a forbidden network is useless because the right is covered by the general authorization and a specific line will always be selected.
			*/
			static std::string getSQLLinesList(
				const security::RightsOfSameClassMap& rights
				, bool totalControl
				, security::RightLevel neededLevel
				, bool mustBeBookable
				, std::string selectedColumns = db::TABLE_COL_ID
			);

			virtual bool allowList( const server::Session* session ) const;
		};
	}
}

#endif // SYNTHESE_CommercialLineTableSync_H__
