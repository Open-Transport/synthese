
/** VinciContractTableSync class header.
	@file VinciContractTableSync.h

	This file belongs to the VINCI BIKE RENTAL SYNTHESE module
	Copyright (C) 2006 Vinci Park 
	Contact : Raphaël Murat - Vinci Park <rmurat@vincipark.com>

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

#ifndef SYNTHESE_VinciContractTableSync_H__
#define SYNTHESE_VinciContractTableSync_H__

#include <vector>
#include <string>
#include <iostream>

#include <boost/shared_ptr.hpp>

#include "02_db/SQLiteNoSyncTableSyncTemplate.h"

#include "71_vinci_bike_rental/VinciContract.h"

namespace synthese
{
	namespace accounts
	{
		class TransactionPart;
	}

	namespace vinci
	{
		/** Vinci Contract SQLite table synchronizer.
			@ingroup m71LS refLS
		*/

		class VinciContractTableSync : public db::SQLiteNoSyncTableSyncTemplate<VinciContractTableSync,VinciContract>
		{
		public:
			static const std::string COL_USER_ID;
			static const std::string COL_SITE_ID;
			static const std::string COL_DATE;
			static const std::string COL_PASSPORT;

			VinciContractTableSync();

			/** VinciContract search.
				@param sqlite SQLite thread
				@param name Name to search
				@param surname Surname to search
				@param first First user to answer
				@param number Number of users to answer (0 = all) The size of the vector is less or equal to number, then all users were returned despite of the number limit. If the size is greater than number (actually equal to number + 1) then there is others accounts to show. Test it to know if the situation needs a "click for more" button.
				@return Founded contracts. 
				@author Hugues Romain
				@date 2006	
				@warning The returned pointers must be deleted after use to avoid memory leak
			*/
			static std::vector<boost::shared_ptr<VinciContract> > search(
				std::string name = std::string()
				, std::string surname = std::string()
				, int first = 0
				, int number = -1
				, bool orderByNameAndSurname = true
				, bool orderBySurnameAndName = false
				, bool raisingOrder = true
				);

			static std::vector<boost::shared_ptr<accounts::TransactionPart> > getBikeRentTransactionParts(boost::shared_ptr<const VinciContract> contract);

		};

	}
}

#endif 

