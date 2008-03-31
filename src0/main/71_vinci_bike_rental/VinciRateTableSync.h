
/** VinciRateTableSync class header.
	@file VinciRateTableSync.h

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

#ifndef SYNTHESE_VinciRateTableSync_H__
#define SYNTHESE_VinciRateTableSync_H__

#include <vector>
#include <string>
#include <iostream>

#include <boost/shared_ptr.hpp>

#include "02_db/SQLiteNoSyncTableSyncTemplate.h"

namespace synthese
{
	namespace vinci
	{
		class VinciRate;

		/** Vinci Rate SQLite table synchronizer.
			@ingroup m71LS refLS
		*/
		class VinciRateTableSync : public db::SQLiteNoSyncTableSyncTemplate<VinciRateTableSync,VinciRate>
		{
		public:
			static const std::string TABLE_COL_NAME;
			static const std::string TABLE_COL_VALIDITY_DURATION;
			static const std::string TABLE_COL_START_FINANCIAL_PRICE;
			static const std::string TABLE_COL_START_TICKETS_PRICE;
			static const std::string TABLE_COL_END_FINANCIAL_PRICE;
			static const std::string TABLE_COL_END_TICKETS_PRICE;
			static const std::string TABLE_COL_FIRST_PENALTY;
			static const std::string TABLE_COL_FIRST_PENALTY_VALIDITY_DURATION;
			static const std::string TABLE_COL_RECURRING_PENALTY;
			static const std::string TABLE_COL_RECURRING_PENALTY_PERIOD;
			static const std::string TABLE_COL_RECURRING_PENALTY_CANCELS_FIRST;

			VinciRateTableSync();

			/** VinciRate search.
			@param sqlite SQLite thread
			@param first First user to answer
			@param number Number of users to answer (0 = all) The size of the vector is less or equal to number, then all users were returned despite of the number limit. If the size is greater than number (actually equal to number + 1) then there is others accounts to show. Test it to know if the situation needs a "click for more" button.
			@return Founded rates. 
			@author Hugues Romain
			@date 2006	
			*/
			static std::vector<boost::shared_ptr<VinciRate> > search(
				int first = 0, int number = -1);

		};

	}
}

#endif 

