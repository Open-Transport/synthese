
/** ReservationContactTableSync class header.
	@file ReservationContactTableSync.h

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

#ifndef SYNTHESE_ReservationRuleTableSync_H__
#define SYNTHESE_ReservationRuleTableSync_H__

#include "DBDirectTableSyncTemplate.hpp"
#include "ReservationContact.h"

#include <iostream>
#include <vector>
#include <string>
#include <boost/shared_ptr.hpp>

namespace synthese
{
	namespace pt
	{
		class ReservationContact;

		/** Reservation contact table synchronizer.
			@ingroup m35LS refLS
		*/
		class ReservationContactTableSync:
			public db::DBDirectTableSyncTemplate<
				ReservationContactTableSync,
				ReservationContact
			>
		{
		public:

			ReservationContactTableSync() {}


			/** ReservationRule search.
				(other search parameters)
				@param first First ReservationRule object to answer
				@param number Number of ReservationRule objects to answer (0 = all) The size of the vector is less or equal to number, then all users were returned despite of the number limit. If the size is greater than number (actually equal to number + 1) then there is others accounts to show. Test it to know if the situation needs a "click for more" button.
				@return vector<ReservationRule*> Founded ReservationRule objects.
				@author Hugues Romain
				@date 2006
			*/
			static SearchResult Search(
				util::Env& env,
				boost::optional<const std::string&> name = boost::optional<const std::string&>(),
				bool orderByName = true,
				bool raisingOrder = true,
				size_t first = 0,
				size_t number = 0,
				util::LinkLevel linkLevel = util::UP_LINKS_LOAD_LEVEL
			);
		};
}	}

#endif // SYNTHESE_ReservationRuleTableSync_H__
