
/** OnlineReservationRuleTableSync class header.
	@file OnlineReservationRuleTableSync.h

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

#ifndef SYNTHESE_OnlineReservationRuleTableSync_H__
#define SYNTHESE_OnlineReservationRuleTableSync_H__


#include <vector>
#include <string>
#include <iostream>

#include "OnlineReservationRule.h"

#include "DBDirectTableSyncTemplate.hpp"
#include "OldLoadSavePolicy.hpp"

namespace synthese
{
	namespace resa
	{
		/** OnlineReservationRule table synchronizer.
			@ingroup m51LS refLS
		*/
		class OnlineReservationRuleTableSync:
			public db::DBDirectTableSyncTemplate<
				OnlineReservationRuleTableSync,
				OnlineReservationRule,
				db::FullSynchronizationPolicy,
				db::OldLoadSavePolicy
			>
		{
		public:
			static const std::string COL_RESERVATION_CONTACT_ID;
			static const std::string COL_EMAIL;
			static const std::string COL_COPY_EMAIL;
			static const std::string COL_NEEDS_SURNAME;
			static const std::string COL_NEEDS_ADDRESS;
			static const std::string COL_NEEDS_PHONE;
			static const std::string COL_NEEDS_EMAIL;
			static const std::string COL_NEEDS_CUSTOMER_NUMBER;
			static const std::string COL_MAX_SEATS;
			static const std::string COL_THRESHOLDS;
			static const std::string COL_SENDER_EMAIL;
			static const std::string COL_SENDER_NAME;
			static const std::string COL_CONFIRMATION_EMAIL_CMS_ID;
			static const std::string COL_CANCELLATION_EMAIL_CMS_ID;
			static const std::string COL_PASSWORD_EMAIL_CMS_ID;
			static const std::string COL_MULTI_RESERVATIONS_EMAIL_CMS_ID;



			/** OnlineReservationRule search.
				(other search parameters)
				@param first First OnlineReservationRule object to answer
				@param number Number of OnlineReservationRule objects to answer (0 = all) The size of the vector is less or equal to number, then all users were returned despite of the number limit. If the size is greater than number (actually equal to number + 1) then there is others accounts to show. Test it to know if the situation needs a "click for more" button.
				@return vector<OnlineReservationRule*> Founded OnlineReservationRule objects.
				@author Hugues Romain
				@date 2006
			*/
			static SearchResult Search(
				util::Env& env,
				int first = 0,
				int number = 0,
				util::LinkLevel linkLevel = util::UP_LINKS_LOAD_LEVEL
			);
		};
	}
}

#endif // SYNTHESE_OnlineReservationRuleTableSync_H__
