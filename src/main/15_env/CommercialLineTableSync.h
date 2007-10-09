
/** CommercialLineTableSync class header.
	@file CommercialLineTableSync.h

	This file belongs to the SYNTHESE project (public transportation specialized software)
	Copyright (C) 2002 Hugues Romain - RCS <contact@reseaux-conseil.com>

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

#include "CommercialLine.h"

#include <vector>
#include <string>
#include <iostream>

#include "02_db/SQLiteRegistryTableSyncTemplate.h"

#include "12_security/Types.h"

namespace synthese
{
	namespace env
	{
		class TransportNetwork;

		/** CommercialLine table synchronizer.
			@ingroup m15LS refLS
		*/
		class CommercialLineTableSync : public db::SQLiteRegistryTableSyncTemplate<CommercialLineTableSync,CommercialLine>
		{
		public:
			static const std::string COL_NETWORK_ID;
			static const std::string COL_NAME;
			static const std::string COL_SHORT_NAME;
			static const std::string COL_LONG_NAME;
			static const std::string COL_COLOR;
			static const std::string COL_STYLE;
			static const std::string COL_IMAGE;
			static const std::string COL_OPTIONAL_RESERVATION_PLACES;

			CommercialLineTableSync();


			/** CommercialLine search.
				(other search parameters)
				@param first First CommercialLine object to answer
				@param number Number of CommercialLine objects to answer (0 = all) The size of the vector is less or equal to number, then all users were returned despite of the number limit. If the size is greater than number (actually equal to number + 1) then there is others accounts to show. Test it to know if the situation needs a "click for more" button.
				@return vector<CommercialLine*> Founded CommercialLine objects.
				@author Hugues Romain
				@date 2006
			*/
			static std::vector<boost::shared_ptr<CommercialLine> > search(
				const TransportNetwork* network = NULL
				, std::string name = std::string()
				, int first = 0
				, int number = 0
				, bool orderByNetwork = true
				, bool orderByName = false
				, bool raisingOrder = true
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
				@param mustbeBookable true = only transport on demand lines
				@return vector<CommercialLine*> Founded CommercialLine objects.
				@author Hugues Romain
				@date 2006

				To use the method, specify the rights like this :

				@code
rights = request->getUser()->getProfile()->getRightsForModuleClass<ArrivalDepartureTableRight>()
totalControl = request->getUser()->getProfile()->getGlobalPublicRight<ArrivalDepartureTableRight>() >= READ
neededLevel = READ
				@endcode
			*/
			static std::vector<boost::shared_ptr<CommercialLine> > search(
				const security::RightsOfSameClassMap& rights
				, bool totalControl
				, security::RightLevel neededLevel
				, int first = 0
				, int number = 0
				, bool orderByNetwork = true
				, bool orderByName = false
				, bool raisingOrder = true
				, bool mustBeBookable = false
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
		};
	}
}

#endif // SYNTHESE_CommercialLineTableSync_H__
